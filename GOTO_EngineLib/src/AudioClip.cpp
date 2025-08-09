#include "AudioClip.h"
#include "AudioManager.h"
#include "WStringHelper.h"
#include <iostream>

using namespace GOTOEngine;

AudioClip::AudioClip()
	: m_loadMode(AudioLoadMode::DecompressOnLoad)
	, m_length(0.0f)
	, m_channels(0)
	, m_sampleRate(0)
	, m_isLoaded(false)
	, m_preloadAudioData(true)
	, m_audioData(nullptr)
{
}

AudioClip::~AudioClip()
{
	UnloadAudioDataInternal();
}

void AudioClip::LoadFromFilePath(const std::wstring& filePath)
{
	if (!AudioManager::Get()->IsInitialized())
	{
#ifdef _DEBUG
		std::cout << "AudioManager not initialized." << std::endl;
#endif
		return;
	}

	// 오디오 파일 로드 모드 결정
	m_loadMode = DetermineLoadMode(filePath);

	// 파일 정보 읽기
	std::string filePathUtf8 = WStringHelper::wstring_to_string(filePath);

	ma_decoder_config config = ma_decoder_config_init_default();
	ma_decoder decoder;

	ma_result result = ma_decoder_init_file(filePathUtf8.c_str(), &config, &decoder);
	if (result != MA_SUCCESS)
	{
		m_isLoaded = false;
		return;
	}

	m_channels = decoder.outputChannels;
	m_sampleRate = decoder.outputSampleRate;
	m_decoderConfig = config;

	// 길이 계산
	ma_uint64 frameCount;
	if (ma_decoder_get_length_in_pcm_frames(&decoder, &frameCount) == MA_SUCCESS)
	{
		m_length = (float)frameCount / (float)m_sampleRate;
	}

	ma_decoder_uninit(&decoder);
	m_isLoaded = true;

	// preloadAudioData가 활성화되어 있고 DecompressOnLoad 모드라면 즉시 로딩
	if (m_preloadAudioData && m_loadMode == AudioLoadMode::DecompressOnLoad)
	{
		LoadAudioDataInternal();
	}

#ifdef _DEBUG
	std::string modeStr = (m_loadMode == AudioLoadMode::DecompressOnLoad) ? "Memory" : "Stream";
	std::string preloadStr = m_preloadAudioData ? "Preloaded" : "On-Demand";
	std::cout << "AudioClip loaded: " << filePathUtf8
		<< " (Mode: " << modeStr << ", " << preloadStr << ", Length: " << m_length << "s)" << std::endl;
#endif
}

AudioLoadMode AudioClip::DetermineLoadMode(const std::wstring& filePath)
{
	// 확장자 추출
	size_t lastDot = filePath.find_last_of(L".");
	if (lastDot == std::wstring::npos)
		return AudioLoadMode::DecompressOnLoad;

	std::wstring extension = filePath.substr(lastDot + 1);

	// 소문자 변환
	for (auto& c : extension)
		c = towlower(c);

	// 긴 파일들은 스트리밍, 짧은 파일들은 메모리 로드
	if (extension == L"mp3" || extension == L"ogg" || extension == L"flac")
		return AudioLoadMode::Stream;
	else
		return AudioLoadMode::DecompressOnLoad;
}

void AudioClip::SetPreloadAudioData(bool preload)
{
	if (m_preloadAudioData == preload)
		return;

	m_preloadAudioData = preload;

	if (preload && m_loadMode == AudioLoadMode::DecompressOnLoad && !IsAudioDataLoaded())
	{
		LoadAudioDataInternal();
	}
	else if (!preload && IsAudioDataLoaded())
	{
		UnloadAudioDataInternal();
	}
}

bool AudioClip::LoadAudioData()
{
	// DecompressOnLoad 모드가 아니면 실패 (유니티와 동일)
	if (m_loadMode != AudioLoadMode::DecompressOnLoad)
	{
#ifdef _DEBUG
		std::cout << "LoadAudioData failed: Clip is in streaming mode" << std::endl;
#endif
		return false;
	}

	return LoadAudioDataInternal();
}

bool AudioClip::UnloadAudioData()
{
	if (!IsAudioDataLoaded())
		return false;

	UnloadAudioDataInternal();

	// 관련된 AudioSource들에게 준비 필요 상태로 설정
	if (AudioManager::Get()->IsInitialized())
	{
		AudioManager::Get()->NotifyClipUnloaded(this);
	}

#ifdef _DEBUG
	std::cout << "Audio data unloaded manually" << std::endl;
#endif
	return true;
}

bool AudioClip::LoadAudioDataInternal()
{
	if (m_audioData && m_audioData->isLoaded)
		return true; // 이미 로딩됨

	if (!m_isLoaded)
		return false;

	std::string filePathUtf8 = WStringHelper::wstring_to_string(m_filePath);

	ma_decoder decoder;
	ma_result result = ma_decoder_init_file(filePathUtf8.c_str(), &m_decoderConfig, &decoder);

	if (result != MA_SUCCESS)
		return false;

	// 전체 프레임 수 얻기
	ma_uint64 frameCount;
	if (ma_decoder_get_length_in_pcm_frames(&decoder, &frameCount) != MA_SUCCESS)
	{
		ma_decoder_uninit(&decoder);
		return false;
	}

	// AudioData 생성
	if (!m_audioData)
		m_audioData = std::make_unique<AudioData>();

	m_audioData->channels = m_channels;
	m_audioData->sampleRate = m_sampleRate;
	m_audioData->frameCount = frameCount;

	// PCM 데이터 읽기 (float 형식)
	ma_uint64 totalSamples = frameCount * m_channels;
	m_audioData->samples.resize(totalSamples);

	ma_uint64 framesRead;
	result = ma_decoder_read_pcm_frames(&decoder, m_audioData->samples.data(), frameCount, &framesRead);

	ma_decoder_uninit(&decoder);

	if (result == MA_SUCCESS || result == MA_AT_END)
	{
		m_audioData->frameCount = framesRead;
		m_audioData->isLoaded = true;

#ifdef _DEBUG
		float sizeInMB = (totalSamples * sizeof(float)) / (1024.0f * 1024.0f);
		std::cout << "Audio data loaded into memory: " << filePathUtf8
			<< " (" << sizeInMB << " MB)" << std::endl;
#endif
		return true;
	}

	return false;
}

void AudioClip::UnloadAudioDataInternal()
{
	if (m_audioData)
	{
		m_audioData.reset();
#ifdef _DEBUG
		std::cout << "Audio data unloaded from memory" << std::endl;
#endif
	}
}

size_t AudioClip::GetMemoryUsage() const
{
	if (!IsAudioDataLoaded())
		return 0;

	return m_audioData->samples.size() * sizeof(float);
}

void AudioClip::ForcedLoadAudioData()
{
	if (m_loadMode == AudioLoadMode::DecompressOnLoad)
	{
		LoadAudioDataInternal();
	}
}

void AudioClip::ForcedUnloadAudioData()
{
	UnloadAudioDataInternal();
}

// PCM 데이터 직접 접근용 함수들
const float* AudioClip::GetPCMData() const
{
	if (!IsAudioDataLoaded())
		return nullptr;

	return m_audioData->samples.data();
}

ma_uint64 AudioClip::GetFrameCount() const
{
	if (!IsAudioDataLoaded())
		return 0;

	return m_audioData->frameCount;
}

bool AudioClip::HasValidPCMData() const
{
	return IsAudioDataLoaded() && m_audioData->samples.size() > 0;
}