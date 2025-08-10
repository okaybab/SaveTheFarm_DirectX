#define NOMINMAX

#include "AudioSource.h"
#include "AudioManager.h"
#include "AudioListener.h"
#include <thread>
#include "AudioClip.h"
#include "Transform.h"
#include "GameObject.h"
#include "WStringHelper.h"
#include <iostream>
#include <algorithm>

using namespace GOTOEngine;

AudioSource::AudioSource()
	: m_clip(nullptr)
	, m_audioBuffer{}
	, m_audioBufferInitialized(false)
	, m_sound{}
	, m_soundInitialized(false)
	, m_streamSound{}
	, m_streamSoundInitialized(false)
	, m_volume(1.0f)
	, m_pitch(1.0f)
	, m_loop(false)
	, m_playOnAwake(false)
	, m_spatialAudio(false)
	, m_minDistance(1.0f)
	, m_maxDistance(100.0f)
	, m_isPlaying(false)
	, m_isPaused(false)
	, m_lastPosition(Vector2{ 0, 0 })
	, m_positionDirty(true)
	, m_soundReady(false)
	, m_needsPrepare(false)
	, m_lastCleanupTime(0.0f)
{
	AudioManager::Get()->RegisterAudioSource(this);
	REGISTER_BEHAVIOUR_MESSAGE(OnEnable);
	REGISTER_BEHAVIOUR_MESSAGE(OnDisable);
}

void GOTOEngine::AudioSource::Dispose()
{
	CleanupSounds();
	if (IsValidObject(m_clip)
		&& !m_clip->IsDestroyed())
	{
		m_clip->DecreaseRefCount();
		m_clip = nullptr;
	}
}

void GOTOEngine::AudioSource::PrintDebugInfo() const
{
	std::cout << "=== AudioSource Debug Info ===" << std::endl;

	if (m_clip) {
		std::cout << "Clip: " << WStringHelper::wstring_to_string(m_clip->GetFilePath()) << std::endl;
		std::cout << "Load Mode: " << (m_clip->GetLoadMode() == AudioLoadMode::DecompressOnLoad ? "Memory" : "Stream") << std::endl;
		std::cout << "PreLoad Flag: " << m_clip->GetPreloadAudioData() << std::endl;
		std::cout << "Data Loaded: " << m_clip->IsAudioDataLoaded() << std::endl;
		std::cout << "Has Valid PCM: " << m_clip->HasValidPCMData() << std::endl;
		if (m_clip->IsAudioDataLoaded()) {
			std::cout << "Memory Usage: " << (m_clip->GetMemoryUsage() / 1024.0f) << " KB" << std::endl;
		}
	}
	else {
		std::cout << "No clip assigned" << std::endl;
	}

	std::cout << "Sound Ready: " << m_soundReady << std::endl;
	std::cout << "Memory Sound: " << m_soundInitialized << std::endl;
	std::cout << "Stream Sound: " << m_streamSoundInitialized << std::endl;
	std::cout << "Needs Prepare: " << m_needsPrepare << std::endl;
	std::cout << "Is Playing: " << m_isPlaying << std::endl;
	std::cout << "===============================" << std::endl;
}

void GOTOEngine::AudioSource::CleanupFinishedOneShots()
{
	auto currentTime = static_cast<float>(ma_engine_get_time(AudioManager::Get()->GetEngine()));

	// 완료된 사운드들을 찾아서 제거
	auto it = std::remove_if(m_oneShotSounds.begin(), m_oneShotSounds.end(),
		[currentTime](const std::unique_ptr<OneShotSound>& oneShot) {
			if (!oneShot->initialized) return true;

			// 사운드가 끝났는지 확인
			if (!ma_sound_is_playing(&oneShot->sound)) {
				return true;  // 제거 대상
			}

			// 너무 오래 재생된 경우도 정리 (안전장치, 예: 30초)
			if (currentTime - oneShot->startTime > 30.0f) {
				return true;
			}

			return false;
		});

	m_oneShotSounds.erase(it, m_oneShotSounds.end());
}

void AudioSource::OnEnable()
{
	if (AudioManager::Get()->IsInitialized())
	{
		if (m_clip)
		{
			m_needsPrepare = true;
		}

		if (m_playOnAwake && m_clip)
			Play();

#ifdef _DEBUG_AUDIO
		std::cout << "AudioSource enabled." << std::endl;
#endif
	}
}

void AudioSource::OnDisable()
{
	if (AudioManager::Get()->IsInitialized())
	{
		Stop();

#ifdef _DEBUG_AUDIO
		std::cout << "AudioSource disabled." << std::endl;
#endif
	}
}

void AudioSource::SetClip(AudioClip* clip)
{
	if (m_clip == clip) return;

	if (m_isPlaying) Stop();
	CleanupSounds();

	// 레퍼런스 카운트 관리
	if (clip)
		clip->IncreaseRefCount();
	if (m_clip)
		m_clip->DecreaseRefCount();

	m_clip = clip;
	m_soundReady = false;
	m_needsPrepare = (clip != nullptr);

#ifdef _DEBUG_AUDIO
	if (clip) {
		std::cout << "AudioClip set: " << WStringHelper::wstring_to_string(clip->GetFilePath()) << std::endl;
		std::cout << "Load mode: " << (clip->GetLoadMode() == AudioLoadMode::DecompressOnLoad ? "Memory" : "Stream") << std::endl;
		std::cout << "PreLoad enabled: " << (clip->GetPreloadAudioData() ? "Yes" : "No") << std::endl;
		std::cout << "Audio data loaded: " << (clip->IsAudioDataLoaded() ? "Yes" : "No") << std::endl;

		// PreLoad가 활성화되어 있지만 데이터가 없으면 자동 로드
		if (clip->GetPreloadAudioData() &&
			clip->GetLoadMode() == AudioLoadMode::DecompressOnLoad &&
			!clip->IsAudioDataLoaded()) {

			std::cout << "Auto-loading audio data..." << std::endl;
			if (clip->LoadAudioData()) {
				std::cout << "Audio data auto-loaded successfully!" << std::endl;
			}
			else {
				std::cout << "Failed to auto-load audio data" << std::endl;
			}
		}
	}
#endif
}

void AudioSource::AutoPrepareIfNeeded()
{
	if (!m_needsPrepare || !m_clip || !AudioManager::Get()->IsInitialized())
		return;

	if (m_clip->GetLoadMode() == AudioLoadMode::DecompressOnLoad) {
		if (m_clip->IsAudioDataLoaded()) {
			InitializeMemorySound();
		}
		else {
			InitializeStreamSound();
		}
	}
	else if (m_clip->GetLoadMode() == AudioLoadMode::CompressedInMemory) {
		if (m_clip->IsCompressedDataLoaded()) {
			InitializeCompressedMemorySound();
		}
		else {
			InitializeStreamSound();
		}
	}
	else { // Stream 모드
		if (m_clip->IsFirstChunkLoaded()) {
			// 첫 번째 청크가 프리로드된 스트림 초기화
			InitializeStreamSoundWithFirstChunk();
		}
		else {
			// 일반 스트림 초기화
			InitializeStreamSound();
		}
	}

	if (m_soundReady) {
		m_needsPrepare = false;
#ifdef _DEBUG_AUDIO
		const char* modeNames[] = { "DecompressOnLoad", "CompressedInMemory", "Stream" };
		std::string preloadStatus = "";

		if (m_clip->GetLoadMode() == AudioLoadMode::Stream && m_clip->IsFirstChunkLoaded()) {
			preloadStatus = " (FirstChunk PreLoaded)";
		}
		else if (m_clip->GetLoadMode() != AudioLoadMode::Stream && m_clip->GetPreloadAudioData()) {
			preloadStatus = " (PreLoaded)";
		}

		std::cout << "Sound auto-prepared successfully (Mode: " << modeNames[(int)m_clip->GetLoadMode()]
			<< preloadStatus << ")" << std::endl;
#endif
	}
}

void AudioSource::InitializeStreamSound()
{
	if (!m_clip)
		return;

	CleanupSounds();

	std::string filePathUtf8 = WStringHelper::wstring_to_string(m_clip->GetFilePath());

#ifdef _DEBUG_AUDIO
	std::cout << "Initializing sound from file: " << filePathUtf8 << std::endl;
#endif

	ma_uint32 flags = MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION;

	// 스트리밍 플래그는 일단 사용하지 않음 (안정성 위해)
	// if (m_clip->GetLoadMode() == AudioLoadMode::Stream)
	//     flags |= MA_SOUND_FLAG_STREAM;

	ma_result result = ma_sound_init_from_file(
		AudioManager::Get()->GetEngine(),
		filePathUtf8.c_str(),
		flags,
		nullptr,
		nullptr,
		&m_streamSound
	);

	if (result == MA_SUCCESS)
	{
		m_streamSoundInitialized = true;
		m_soundReady = true;
		ApplySettings();

#ifdef _DEBUG_AUDIO
		std::cout << "Sound initialized successfully" << std::endl;
#endif
	}
	else
	{
		m_soundReady = false;
#ifdef _DEBUG_AUDIO
		std::cout << "Failed to initialize sound. Error code: " << result << std::endl;
#endif
	}
}

void GOTOEngine::AudioSource::InitializeStreamSoundWithFirstChunk()
{
	if (!m_clip) return;

	CleanupSounds();
	memset(&m_streamSound, 0, sizeof(ma_sound));

	std::string filePathUtf8 = WStringHelper::wstring_to_string(m_clip->GetFilePath());

#ifdef _DEBUG_AUDIO
	std::cout << "Initializing stream sound with first chunk preload..." << std::endl;

	if (m_clip->IsFirstChunkLoaded()) {
		std::cout << "  - First chunk available: " << m_clip->GetFirstChunkDuration() << "s" << std::endl;
		std::cout << "  - First chunk size: " << (m_clip->GetFirstChunkSize() / 1024.0f) << " KB" << std::endl;
	}
#endif

	// 일반적인 스트림 사운드 초기화 (첫 번째 청크는 백그라운드에서 도움)
	ma_uint32 flags = MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION;

	ma_result result = ma_sound_init_from_file(
		AudioManager::Get()->GetEngine(),
		filePathUtf8.c_str(),
		flags,
		nullptr,
		nullptr,
		&m_streamSound
	);

	if (result == MA_SUCCESS) {
		m_streamSoundInitialized = true;
		m_soundReady = true;
		ApplySettings();

#ifdef _DEBUG_AUDIO
		std::cout << "Stream sound initialized successfully!" << std::endl;
		if (m_clip->IsFirstChunkLoaded()) {
			std::cout << "  - First chunk will help reduce initial latency" << std::endl;
		}
#endif
	}
	else {
		m_soundReady = false;
		memset(&m_streamSound, 0, sizeof(ma_sound));
#ifdef _DEBUG_AUDIO
		std::cout << "Failed to initialize stream sound. Error: " << result << std::endl;
#endif
	}
}

void GOTOEngine::AudioSource::InitializeCompressedMemorySound()
{
	if (!m_clip || !m_clip->HasCompressedData()) {
#ifdef _DEBUG_AUDIO
		std::cout << "Cannot initialize compressed memory sound: No compressed data" << std::endl;
#endif
		return;
	}

	CleanupSounds();
	memset(&m_streamSound, 0, sizeof(ma_sound));

#ifdef _DEBUG_AUDIO
	std::cout << "Initializing compressed memory sound..." << std::endl;
	std::cout << "  - Compressed data size: " << (m_clip->GetCompressedDataSize() / 1024.0f) << " KB" << std::endl;
#endif

	// 압축된 데이터에서 메모리 디코더 생성
	ma_decoder_config config = ma_decoder_config_init_default();
	ma_decoder decoder;

	// 메모리에서 디코더 초기화
	ma_result result = ma_decoder_init_memory(
		m_clip->GetCompressedData(),
		m_clip->GetCompressedDataSize(),
		&config,
		&decoder
	);

	if (result != MA_SUCCESS) {
#ifdef _DEBUG_AUDIO
		std::cout << "Failed to initialize memory decoder. Error: " << result << std::endl;
#endif
		return;
	}

	// 디코더로부터 사운드 생성 (실시간 디코딩)
	result = ma_sound_init_from_data_source(
		AudioManager::Get()->GetEngine(),
		&decoder,
		MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION,
		nullptr,
		&m_streamSound
	);

	if (result == MA_SUCCESS) {
		m_streamSoundInitialized = true;
		m_soundReady = true;
		ApplySettings();

#ifdef _DEBUG_AUDIO
		std::cout << "Compressed memory sound initialized successfully!" << std::endl;
		std::cout << "  - Will decode in real-time during playback" << std::endl;
#endif
	}
	else {
		m_soundReady = false;
		memset(&m_streamSound, 0, sizeof(ma_sound));

#ifdef _DEBUG_AUDIO
		std::cout << "Failed to initialize compressed memory sound. Error: " << result << std::endl;
#endif
	}

	// 디코더는 사운드가 참조하므로 여기서 해제하지 않음
}

void AudioSource::InitializeMemorySound()
{
	if (!m_clip || !m_clip->HasValidPCMData()) {
#ifdef _DEBUG_AUDIO
		std::cout << "Cannot initialize memory sound: No valid PCM data" << std::endl;
#endif
		return;
	}

	CleanupSounds();

	// 구조체 초기화
	memset(&m_audioBuffer, 0, sizeof(ma_audio_buffer));
	memset(&m_sound, 0, sizeof(ma_sound));

#ifdef _DEBUG_AUDIO
	std::cout << "Initializing memory sound from preloaded PCM data..." << std::endl;
#endif

	// PreLoad된 PCM 데이터로 오디오 버퍼 생성
	ma_audio_buffer_config bufferConfig = ma_audio_buffer_config_init(
		ma_format_f32,                    // PCM 포맷
		m_clip->GetChannels(),            // 채널 수
		m_clip->GetFrameCount(),          // 프레임 수
		m_clip->GetPCMData(),             // PreLoad된 PCM 데이터
		nullptr                           // 해제 콜백 (AudioClip이 관리)
	);

	ma_result result = ma_audio_buffer_init(&bufferConfig, &m_audioBuffer);
	if (result != MA_SUCCESS) {
#ifdef _DEBUG_AUDIO
		std::cout << "Failed to initialize audio buffer from PCM data. Error: " << result << std::endl;
#endif
		return;
	}

	m_audioBufferInitialized = true;

	// 오디오 버퍼로부터 사운드 생성
	result = ma_sound_init_from_data_source(
		AudioManager::Get()->GetEngine(),
		&m_audioBuffer,                   // PreLoad된 데이터 소스
		MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION,
		nullptr,
		&m_sound
	);

	if (result == MA_SUCCESS) {
		m_soundInitialized = true;
		m_soundReady = true;
		ApplySettings();

#ifdef _DEBUG_AUDIO
		std::cout << "Memory sound initialized successfully from preloaded data!" << std::endl;
		std::cout << "  - Channels: " << m_clip->GetChannels() << std::endl;
		std::cout << "  - Sample Rate: " << m_clip->GetSampleRate() << std::endl;
		std::cout << "  - Frame Count: " << m_clip->GetFrameCount() << std::endl;
		std::cout << "  - Memory Usage: " << (m_clip->GetMemoryUsage() / 1024.0f) << " KB" << std::endl;
#endif
	}
	else {
		m_soundReady = false;

		// 버퍼 초기화는 성공했지만 사운드 실패시 버퍼 정리
		if (m_audioBufferInitialized) {
			ma_audio_buffer_uninit(&m_audioBuffer);
			memset(&m_audioBuffer, 0, sizeof(ma_audio_buffer));
			m_audioBufferInitialized = false;
		}

#ifdef _DEBUG_AUDIO
		std::cout << "Failed to initialize memory sound from buffer. Error: " << result << std::endl;
#endif
	}
}

void AudioSource::CleanupSounds()
{
	if (m_soundInitialized)
	{
		ma_sound_uninit(&m_sound);
		m_soundInitialized = false;
	}

	if (m_streamSoundInitialized)
	{
		ma_sound_uninit(&m_streamSound);
		m_streamSoundInitialized = false;
	}

	if (m_audioBufferInitialized)
	{
		ma_audio_buffer_uninit(&m_audioBuffer);
		m_audioBufferInitialized = false;
	}

	m_soundReady = false;
	m_isPlaying = false;
	m_isPaused = false;
	m_needsPrepare = (m_clip != nullptr);
}

ma_sound* AudioSource::GetActiveSound()
{
	//메모리 사운드를 우선적으로 반환
	if (m_soundInitialized)
		return &m_sound;          // PreLoad된 메모리 사운드
	else if (m_streamSoundInitialized)
		return &m_streamSound;    // 스트림 사운드
	else
		return nullptr;
}

GOTOEngine::AudioSource::~AudioSource()
{
	// OneShot 사운드들 즉시 정리
	m_oneShotSounds.clear();
	AudioManager::Get()->UnRegisterAudioSource(this);
}

void AudioSource::ApplySettings()
{
	ma_sound* activeSound = GetActiveSound();
	if (!activeSound) return;

	ma_sound_set_volume(activeSound, m_volume);
	ma_sound_set_pitch(activeSound, m_pitch);
	ma_sound_set_looping(activeSound, m_loop ? MA_TRUE : MA_FALSE);

	if (m_spatialAudio)
	{
		ma_sound_set_spatialization_enabled(activeSound, MA_TRUE);
		ma_sound_set_min_distance(activeSound, m_minDistance);
		ma_sound_set_max_distance(activeSound, m_maxDistance);
		UpdateTransform();
	}
	else
	{
		ma_sound_set_spatialization_enabled(activeSound, MA_FALSE);
	}
}

void AudioSource::InternalUpdate()
{
	AutoPrepareIfNeeded();

	float currentTime = static_cast<float>(ma_engine_get_time(AudioManager::Get()->GetEngine()));

	if (currentTime - m_lastCleanupTime >= CLEANUP_INTERVAL) {
		CleanupFinishedOneShots();
		m_lastCleanupTime = currentTime;
	}
}

void AudioSource::Play()
{
#ifdef _DEBUG_AUDIO
	std::cout << "=== Play() Debug Info ===" << std::endl;
	if (m_clip) {
		std::cout << "Clip: " << WStringHelper::wstring_to_string(m_clip->GetFilePath()) << std::endl;
		std::cout << "Load Mode: " << (m_clip->GetLoadMode() == AudioLoadMode::DecompressOnLoad ? "Memory" : "Stream") << std::endl;
		std::cout << "PreLoad Enabled: " << (m_clip->GetPreloadAudioData() ? "Yes" : "No") << std::endl;
		std::cout << "Data Loaded: " << (m_clip->IsAudioDataLoaded() ? "Yes" : "No") << std::endl;
		std::cout << "Sound Ready: " << (m_soundReady ? "Yes" : "No") << std::endl;
		std::cout << "Memory Sound Init: " << (m_soundInitialized ? "Yes" : "No") << std::endl;
		std::cout << "Stream Sound Init: " << (m_streamSoundInitialized ? "Yes" : "No") << std::endl;
	}
	std::cout << "=========================" << std::endl;
#endif

	if (!m_soundReady && m_clip) {
#ifdef _DEBUG_AUDIO
		std::cout << "Sound not ready, preparing..." << std::endl;
#endif
		AutoPrepareIfNeeded();
	}

	ma_sound* activeSound = GetActiveSound();
	if (!activeSound) {
#ifdef _DEBUG_AUDIO
		std::cout << "No active sound available for playback" << std::endl;
#endif
		return;
	}

#ifdef _DEBUG_AUDIO
	std::cout << "Using " << (m_soundInitialized ? "MEMORY" : "STREAM") << " sound for playback" << std::endl;
	std::cout << "Starting sound playback..." << std::endl;
#endif

	// 처음부터 재생하도록 시크
	ma_result seekResult = ma_sound_seek_to_pcm_frame(activeSound, 0);
	if (seekResult != MA_SUCCESS) {
#ifdef _DEBUG_AUDIO
		std::cout << "Warning: Failed to seek to beginning. Error: " << seekResult << std::endl;
#endif
	}

	ma_result result = ma_sound_start(activeSound);

	if (result == MA_SUCCESS) {
		m_isPlaying = true;
		m_isPaused = false;

#ifdef _DEBUG_AUDIO
		std::cout << "AudioSource playing successfully using "
			<< (m_soundInitialized ? "PRELOADED MEMORY" : "STREAM") << " data!" << std::endl;
#endif
	}
	else {
		m_isPlaying = false;
		m_isPaused = false;
#ifdef _DEBUG_AUDIO
		std::cout << "Failed to start audio playback. Error: " << result << std::endl;
#endif
	}
}

void AudioSource::PlayOneShot()
{
	PlayOneShot(m_clip, 1.0f);
}

void AudioSource::PlayOneShot(AudioClip* clip, float volumeScale)
{
	if (!clip || !AudioManager::Get()->IsInitialized())
		return;

	// 완료된 OneShot들 정리
	CleanupFinishedOneShots();

	// 새로운 OneShot 생성
	auto oneShot = std::make_unique<OneShotSound>();

	std::string filePathUtf8 = WStringHelper::wstring_to_string(clip->GetFilePath());
	ma_uint32 flags = MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION;

	ma_result result = ma_sound_init_from_file(
		AudioManager::Get()->GetEngine(),
		filePathUtf8.c_str(),
		flags,
		nullptr,
		nullptr,
		&oneShot->sound
	);

	if (result == MA_SUCCESS) {
		oneShot->initialized = true;
		oneShot->startTime = static_cast<float>(ma_engine_get_time(AudioManager::Get()->GetEngine()));

		// 설정 적용
		ma_sound_set_volume(&oneShot->sound, m_volume * volumeScale);
		ma_sound_set_pitch(&oneShot->sound, m_pitch);
		ma_sound_set_looping(&oneShot->sound, MA_FALSE);

		// 재생 시작
		result = ma_sound_start(&oneShot->sound);

		if (result == MA_SUCCESS) {
			// 성공적으로 재생 시작된 경우만 컨테이너에 추가
			m_oneShotSounds.push_back(std::move(oneShot));

#ifdef _DEBUG_AUDIO
			std::cout << "AudioSource one-shot played successfully." << std::endl;
#endif
		}
		else {
#ifdef _DEBUG_AUDIO
			std::cout << "Failed to start one-shot playback. Error: " << result << std::endl;
#endif
		}
	}
	else {
#ifdef _DEBUG_AUDIO
		std::cout << "Failed to initialize one-shot sound. Error: " << result << std::endl;
#endif
	}
}

void AudioSource::Stop()
{
	ma_sound* activeSound = GetActiveSound();
	if (activeSound && m_isPlaying)
	{
		ma_sound_stop(activeSound);
		ma_sound_seek_to_pcm_frame(activeSound, 0);
		m_isPlaying = false;
		m_isPaused = false;

#ifdef _DEBUG_AUDIO
		std::cout << "AudioSource stopped." << std::endl;
#endif
	}
}

void AudioSource::Pause()
{
	ma_sound* activeSound = GetActiveSound();
	if (activeSound && m_isPlaying && !m_isPaused)
	{
		ma_sound_stop(activeSound);
		m_isPaused = true;
	}
}

void AudioSource::Resume()
{
	ma_sound* activeSound = GetActiveSound();
	if (activeSound && m_isPaused)
	{
		ma_sound_start(activeSound);
		m_isPlaying = true;
		m_isPaused = false;
	}
}

void AudioSource::SetVolume(float volume)
{
	m_volume = std::max(0.0f, std::min(1.0f, volume));
	ma_sound* activeSound = GetActiveSound();
	if (activeSound)
		ma_sound_set_volume(activeSound, m_volume);
}

void AudioSource::SetPitch(float pitch)
{
	m_pitch = std::max(0.1f, std::min(3.0f, pitch));
	ma_sound* activeSound = GetActiveSound();
	if (activeSound)
		ma_sound_set_pitch(activeSound, m_pitch);
}

void AudioSource::SetLoop(bool loop)
{
	m_loop = loop;
	ma_sound* activeSound = GetActiveSound();
	if (activeSound)
		ma_sound_set_looping(activeSound, m_loop ? MA_TRUE : MA_FALSE);
}

void AudioSource::SetSpatialAudio(bool enabled)
{
	m_spatialAudio = enabled;
	ma_sound* activeSound = GetActiveSound();
	if (activeSound)
	{
		ma_sound_set_spatialization_enabled(activeSound, enabled ? MA_TRUE : MA_FALSE);
		if (enabled)
			ApplySettings();
	}
}

void AudioSource::SetMinDistance(float distance)
{
	m_minDistance = std::max(0.01f, distance);
	ma_sound* activeSound = GetActiveSound();
	if (activeSound && m_spatialAudio)
		ma_sound_set_min_distance(activeSound, m_minDistance);
}

void AudioSource::SetMaxDistance(float distance)
{
	m_maxDistance = std::max(m_minDistance, distance);
	ma_sound* activeSound = GetActiveSound();
	if (activeSound && m_spatialAudio)
		ma_sound_set_max_distance(activeSound, m_maxDistance);
}

void AudioSource::UpdateTransform()
{
	ma_sound* activeSound = GetActiveSound();
	if (!activeSound || !m_spatialAudio || !GetTransform())
		return;

	Vector2 currentPosition = GetTransform()->GetPosition();

	if (currentPosition.x != m_lastPosition.x ||
		currentPosition.y != m_lastPosition.y ||
		m_positionDirty)
	{
		ma_sound_set_position(activeSound, currentPosition.x, currentPosition.y, 0.0f);
		m_lastPosition = currentPosition;
		m_positionDirty = false;
	}
}

// ===================================================================
// 오디오 재생 문제 진단 코드
// ===================================================================

// AudioSource.cpp - 상세 진단 함수 추가
void AudioSource::DiagnosePlaybackIssues()
{
	std::cout << "\n=== AUDIO PLAYBACK DIAGNOSIS ===" << std::endl;

	// 1. AudioManager 상태 확인
	std::cout << "1. AudioManager Status:" << std::endl;
	std::cout << "   - Initialized: " << AudioManager::Get()->IsInitialized() << std::endl;
	std::cout << "   - Master Volume: " << AudioManager::Get()->GetMasterVolume() << std::endl;

	// 2. ma_engine 상태 확인
	ma_engine* engine = AudioManager::Get()->GetEngine();
	if (engine) {
		std::cout << "   - Engine Valid: Yes" << std::endl;

		// 엔진 상태 상세 확인
		ma_uint32 sampleRate = ma_engine_get_sample_rate(engine);
		ma_uint32 channels = ma_engine_get_channels(engine);
		std::cout << "   - Engine Sample Rate: " << sampleRate << std::endl;
		std::cout << "   - Engine Channels: " << channels << std::endl;

		// 엔진이 실행 중인지 확인
		std::cout << "   - Engine Running: " << (ma_engine_get_time(engine) > 0 ? "Yes" : "Unknown") << std::endl;
	}
	else {
		std::cout << "   - Engine Valid: NO!" << std::endl;
	}

	// 3. 사운드 객체 상태 확인
	std::cout << "\n2. Sound Object Status:" << std::endl;
	ma_sound* activeSound = GetActiveSound();
	if (activeSound) {
		std::cout << "   - Active Sound: " << (m_soundInitialized ? "Memory" : "Stream") << std::endl;

		// 사운드 상태 상세 확인
		ma_bool32 isPlaying = ma_sound_is_playing(activeSound);
		ma_bool32 atEnd = ma_sound_at_end(activeSound);
		float volume = ma_sound_get_volume(activeSound);
		float pitch = ma_sound_get_pitch(activeSound);

		std::cout << "   - Is Playing: " << isPlaying << std::endl;
		std::cout << "   - At End: " << atEnd << std::endl;
		std::cout << "   - Volume: " << volume << std::endl;
		std::cout << "   - Pitch: " << pitch << std::endl;

		// 현재 재생 위치 확인
		ma_uint64 currentFrame;
		ma_result result = ma_sound_get_cursor_in_pcm_frames(activeSound, &currentFrame);
		if (result == MA_SUCCESS) {
			std::cout << "   - Current Frame: " << currentFrame << std::endl;
		}

		// 사운드 길이 확인
		ma_uint64 lengthInFrames;
		result = ma_sound_get_length_in_pcm_frames(activeSound, &lengthInFrames);
		if (result == MA_SUCCESS) {
			std::cout << "   - Total Frames: " << lengthInFrames << std::endl;
			std::cout << "   - Length: " << (float)lengthInFrames / m_clip->GetSampleRate() << " seconds" << std::endl;
		}

		// 공간 오디오 설정 확인
		ma_bool32 spatialEnabled = ma_sound_is_spatialization_enabled(activeSound);
		std::cout << "   - Spatial Audio: " << (spatialEnabled ? "Enabled" : "Disabled") << std::endl;

		if (spatialEnabled) {
			ma_vec3f position = ma_sound_get_position(activeSound);
			std::cout << "   - Sound Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
		}

	}
	else {
		std::cout << "   - Active Sound: NONE!" << std::endl;
	}

	// 4. 리스너 상태 확인
	std::cout << "\n3. Listener Status:" << std::endl;
	AudioListener* mainListener = AudioManager::Get()->GetMainListener();
	if (mainListener) {
		std::cout << "   - Main Listener: Present" << std::endl;
		if (mainListener->GetTransform()) {
			Vector2 pos = mainListener->GetTransform()->GetPosition();
			std::cout << "   - Listener Position: (" << pos.x << ", " << pos.y << ")" << std::endl;
		}

		// 엔진의 리스너 위치 확인
		if (engine) {
			ma_vec3f listenerPos = ma_engine_listener_get_position(engine, 0);
			std::cout << "   - Engine Listener Pos: (" << listenerPos.x << ", " << listenerPos.y << ", " << listenerPos.z << ")" << std::endl;
		}
	}
	else {
		std::cout << "   - Main Listener: MISSING!" << std::endl;
	}

	// 5. 오디오 데이터 유효성 재확인
	std::cout << "\n4. Audio Data Validation:" << std::endl;
	if (m_clip && m_clip->IsAudioDataLoaded()) {
		const float* pcmData = m_clip->GetPCMData();
		ma_uint64 frameCount = m_clip->GetFrameCount();
		ma_uint32 channels = m_clip->GetChannels();

		std::cout << "   - PCM Data Pointer: " << (pcmData ? "Valid" : "NULL!") << std::endl;
		std::cout << "   - Frame Count: " << frameCount << std::endl;
		std::cout << "   - Channels: " << channels << std::endl;

		if (pcmData && frameCount > 0) {
			// 실제 오디오 데이터 샘플 확인 (처음 몇 샘플)
			std::cout << "   - First samples: ";
			for (int i = 0; i < std::min(8, (int)(frameCount * channels)); ++i) {
				std::cout << pcmData[i] << " ";
			}
			std::cout << std::endl;

			// 오디오 데이터가 모두 0인지 확인 (무음 데이터 체크)
			bool hasNonZeroSample = false;
			for (ma_uint64 i = 0; i < std::min(1000ULL, frameCount * channels); ++i) {
				if (pcmData[i] != 0.0f) {
					hasNonZeroSample = true;
					break;
				}
			}
			std::cout << "   - Has Non-Zero Samples: " << (hasNonZeroSample ? "Yes" : "NO! (Silent audio)") << std::endl;
		}
	}

	std::cout << "=================================" << std::endl;
}

// ===================================================================
// 오디오 시스템 강제 테스트 함수
// ===================================================================

void AudioSource::ForceAudioSystemTest()
{
	std::cout << "\n=== AUDIO SYSTEM FORCED TEST ===" << std::endl;

	// 1. 마스터 볼륨 강제 설정
	std::cout << "1. Setting master volume to 1.0..." << std::endl;
	AudioManager::Get()->SetMasterVolume(1.0f);
	std::cout << "   Master volume set to: " << AudioManager::Get()->GetMasterVolume() << std::endl;

	// 2. 사운드 볼륨 강제 설정
	if (GetActiveSound()) {
		std::cout << "2. Setting sound volume to 1.0..." << std::endl;
		ma_sound_set_volume(GetActiveSound(), 1.0f);
		std::cout << "   Sound volume set to: " << ma_sound_get_volume(GetActiveSound()) << std::endl;
	}

	// 3. 공간 오디오 강제 비활성화
	if (GetActiveSound()) {
		std::cout << "3. Disabling spatial audio..." << std::endl;
		ma_sound_set_spatialization_enabled(GetActiveSound(), MA_FALSE);
		std::cout << "   Spatial audio disabled" << std::endl;
	}

	// 4. 사운드 위치 초기화
	if (GetActiveSound()) {
		std::cout << "4. Resetting sound position..." << std::endl;
		ma_sound_set_position(GetActiveSound(), 0.0f, 0.0f, 0.0f);
	}

	// 5. 강제 재생 테스트
	std::cout << "5. Force play test..." << std::endl;
	ma_sound* activeSound = GetActiveSound();
	if (activeSound) {
		// 처음부터 재생
		ma_sound_seek_to_pcm_frame(activeSound, 0);
		ma_result result = ma_sound_start(activeSound);

		std::cout << "   Start result: " << (result == MA_SUCCESS ? "SUCCESS" : "FAILED") << std::endl;

		// 잠시 후 상태 확인
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		std::cout << "   Is playing after 100ms: " << ma_sound_is_playing(activeSound) << std::endl;

		ma_uint64 currentFrame;
		if (ma_sound_get_cursor_in_pcm_frames(activeSound, &currentFrame) == MA_SUCCESS) {
			std::cout << "   Current frame after 100ms: " << currentFrame << std::endl;
		}
	}

	std::cout << "=================================" << std::endl;
}

// ===================================================================
// 간단한 테스트 톤 생성 (시스템 테스트용)
// ===================================================================

void AudioSource::PlayTestTone()
{
	std::cout << "\n=== PLAYING TEST TONE ===" << std::endl;

	ma_engine* engine = AudioManager::Get()->GetEngine();
	if (!engine) {
		std::cout << "Engine not available!" << std::endl;
		return;
	}

	// 간단한 사인파 테스트 톤 재생
	ma_waveform_config waveformConfig = ma_waveform_config_init(
		ma_format_f32,
		2,          // 스테레오
		44100,      // 샘플레이트
		ma_waveform_type_sine,
		0.2,        // 볼륨 (작게)
		440.0       // 440Hz (A4 음)
	);

	ma_waveform waveform;
	ma_result result = ma_waveform_init(&waveformConfig, &waveform);

	if (result == MA_SUCCESS) {
		ma_sound testSound;
		result = ma_sound_init_from_data_source(
			engine,
			&waveform,
			0,
			nullptr,
			&testSound
		);

		if (result == MA_SUCCESS) {
			std::cout << "Playing test tone for 1 second..." << std::endl;
			ma_sound_start(&testSound);

			// 1초 재생
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));

			ma_sound_stop(&testSound);
			ma_sound_uninit(&testSound);
			std::cout << "Test tone finished." << std::endl;
		}
		else {
			std::cout << "Failed to create test sound: " << result << std::endl;
		}

		ma_waveform_uninit(&waveform);
	}
	else {
		std::cout << "Failed to create test waveform: " << result << std::endl;
	}

	std::cout << "===========================" << std::endl;
}

// ===================================================================
// 통합 진단 함수
// ===================================================================

void AudioSource::FullAudioDiagnosis()
{
	std::cout << "\n=== FULL AUDIO SYSTEM DIAGNOSIS ===" << std::endl;

	// 기본 정보 출력
	PrintDebugInfo();

	// 상세 진단
	DiagnosePlaybackIssues();

	// 시스템 강제 테스트
	ForceAudioSystemTest();

	// 테스트 톤 재생 (시스템이 정상인지 확인)
	PlayTestTone();

	std::cout << "\n=== DIAGNOSIS SUMMARY ===" << std::endl;
	std::cout << "1. Check if test tone played successfully" << std::endl;
	std::cout << "2. If test tone works but your audio doesn't:" << std::endl;
	std::cout << "   - Audio file may be corrupted or silent" << std::endl;
	std::cout << "   - PCM data conversion issue" << std::endl;
	std::cout << "3. If test tone also doesn't work:" << std::endl;
	std::cout << "   - Audio device/driver issue" << std::endl;
	std::cout << "   - miniaudio engine configuration problem" << std::endl;
	std::cout << "=================================" << std::endl;
}

// ===================================================================
// 사용법 예시
// ===================================================================

/*
게임 코드에서 이렇게 사용:

// 오디오 문제 발생 시
audioSource->FullAudioDiagnosis();

// 또는 개별 진단
audioSource->DiagnosePlaybackIssues();
audioSource->ForceAudioSystemTest();
audioSource->PlayTestTone();
*/