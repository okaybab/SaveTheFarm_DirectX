#define NOMINMAX

#include "AudioSource.h"
#include "AudioManager.h"
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

#ifdef _DEBUG
		std::cout << "AudioSource enabled." << std::endl;
#endif
	}
}

void AudioSource::OnDisable()
{
	if (AudioManager::Get()->IsInitialized())
	{
		Stop();

#ifdef _DEBUG
		std::cout << "AudioSource disabled." << std::endl;
#endif
	}
}

void AudioSource::SetClip(AudioClip* clip)
{
	if (m_clip == clip) return;

	if (m_isPlaying) Stop();
	CleanupSounds();

	// 레퍼런스 카운팅 관리
	if (clip)
		clip->IncreaseRefCount();
	if (m_clip)
		m_clip->DecreaseRefCount();

	m_clip = clip;
	m_soundReady = false;
	m_needsPrepare = (clip != nullptr);

#ifdef _DEBUG
	if (clip) {
		std::cout << "AudioClip set: " << WStringHelper::wstring_to_string(clip->GetFilePath()) << std::endl;
		std::cout << "Load mode: " << (clip->GetLoadMode() == AudioLoadMode::DecompressOnLoad ? "Memory" : "Stream") << std::endl;
		std::cout << "Audio data loaded: " << (clip->IsAudioDataLoaded() ? "Yes" : "No") << std::endl;
	}
#endif
}

void AudioSource::AutoPrepareIfNeeded()
{
	if (!m_needsPrepare || !m_clip || !AudioManager::Get()->IsInitialized())
		return;

	// 일단 간단하게 파일 기반으로만 시도
	InitializeStreamSound();

	if (m_soundReady)
	{
		m_needsPrepare = false;
#ifdef _DEBUG
		std::cout << "Sound auto-prepared successfully" << std::endl;
#endif
	}
	else
	{
#ifdef _DEBUG
		std::cout << "Failed to auto-prepare sound" << std::endl;
#endif
	}
}

void AudioSource::InitializeStreamSound()
{
	if (!m_clip)
		return;

	CleanupSounds();

	std::string filePathUtf8 = WStringHelper::wstring_to_string(m_clip->GetFilePath());

#ifdef _DEBUG
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

#ifdef _DEBUG
		std::cout << "Sound initialized successfully" << std::endl;
#endif
	}
	else
	{
		m_soundReady = false;
#ifdef _DEBUG
		std::cout << "Failed to initialize sound. Error code: " << result << std::endl;
#endif
	}
}

void AudioSource::InitializeMemorySound()
{
	// 일단 사용하지 않음 - 문제 해결 후 나중에 구현
#ifdef _DEBUG
	std::cout << "Memory sound initialization not implemented yet" << std::endl;
#endif
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
	if (m_streamSoundInitialized)
		return &m_streamSound;
	else if (m_soundInitialized)
		return &m_sound;
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
#ifdef _DEBUG
	std::cout << "Play() called" << std::endl;
#endif

	if (!m_soundReady && m_clip)
	{
#ifdef _DEBUG
		std::cout << "Sound not ready, preparing..." << std::endl;
#endif
		AutoPrepareIfNeeded();
	}

	ma_sound* activeSound = GetActiveSound();
	if (activeSound)
	{
#ifdef _DEBUG
		std::cout << "Starting sound playback..." << std::endl;
#endif

		// 처음부터 재생하도록 시크
		ma_sound_seek_to_pcm_frame(activeSound, 0);

		ma_result result = ma_sound_start(activeSound);

		if (result == MA_SUCCESS)
		{
			m_isPlaying = true;
			m_isPaused = false;

#ifdef _DEBUG
			std::cout << "AudioSource playing successfully!" << std::endl;
#endif
		}
		else
		{
#ifdef _DEBUG
			std::cout << "Failed to start AudioSource. Error: " << result << std::endl;
#endif
		}
	}
	else
	{
#ifdef _DEBUG
		std::cout << "No active sound available for playback" << std::endl;
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

#ifdef _DEBUG
			std::cout << "AudioSource one-shot played successfully." << std::endl;
#endif
		}
		else {
#ifdef _DEBUG
			std::cout << "Failed to start one-shot playback. Error: " << result << std::endl;
#endif
		}
	}
	else {
#ifdef _DEBUG
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

#ifdef _DEBUG
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