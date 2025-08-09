#pragma once
#include "Resource.h"
#include <miniaudio.h>
#include <memory>
#include <vector>

namespace GOTOEngine
{
	enum class AudioLoadMode
	{
		DecompressOnLoad,  // 메모리에 전체 로드 (효과음용)
		Stream            // 스트리밍 (배경음악용)
	};

	// 실제 오디오 데이터를 보관하는 구조체
	struct AudioData
	{
		std::vector<float> samples;  // PCM 샘플 데이터
		ma_uint32 channels;
		ma_uint32 sampleRate;
		ma_uint64 frameCount;
		bool isLoaded;

		AudioData() : channels(0), sampleRate(0), frameCount(0), isLoaded(false) {}
	};

	class AudioClip : public Resource
	{
	private:
		friend class ResourceManager;
		friend class AudioSource;
		friend class AudioManager;

		AudioLoadMode m_loadMode;
		float m_length;
		ma_uint32 m_channels;
		ma_uint32 m_sampleRate;
		bool m_isLoaded;
		bool m_preloadAudioData;

		// 실제 오디오 데이터 저장
		std::unique_ptr<AudioData> m_audioData;

		// 스트리밍용 디코더 설정
		ma_decoder_config m_decoderConfig;

		void LoadFromFilePath(const std::wstring& filePath) override;
		AudioLoadMode DetermineLoadMode(const std::wstring& filePath);

		// 내부 구현 함수들
		bool LoadAudioDataInternal();   // 실제 PCM 데이터 로딩
		void UnloadAudioDataInternal(); // 메모리 해제

	public:
		AudioClip();
		~AudioClip();

		bool IsValidData() override { return m_isLoaded; }
		float GetLength() const { return m_length; }
		ma_uint32 GetChannels() const { return m_channels; }
		ma_uint32 GetSampleRate() const { return m_sampleRate; }
		AudioLoadMode GetLoadMode() const { return m_loadMode; }

		// preloadAudioData 관련 함수들
		void SetPreloadAudioData(bool preload);
		bool GetPreloadAudioData() const { return m_preloadAudioData; }
		bool IsAudioDataLoaded() const { return m_audioData && m_audioData->isLoaded; }

		// 유니티 스타일 API
		bool LoadAudioData();    // 유니티의 AudioClip.LoadAudioData()와 동일
		bool UnloadAudioData();  // 유니티의 AudioClip.UnloadAudioData()와 동일

		// 메모리 관리
		size_t GetMemoryUsage() const;
		void ForcedLoadAudioData();   // 씬 로드시 강제 로딩용
		void ForcedUnloadAudioData(); // 강제 언로딩용

		// AudioSource에서 직접 PCM 데이터 접근용
		const float* GetPCMData() const;
		ma_uint64 GetFrameCount() const;
		bool HasValidPCMData() const;
	};
}