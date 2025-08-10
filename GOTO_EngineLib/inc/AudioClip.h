#pragma once
#include "Resource.h"
#include <miniaudio.h>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

namespace GOTOEngine
{
    enum class AudioLoadMode
    {
        DecompressOnLoad,      // 메모리에 압축 해제된 상태로 로드 (짧은 효과음)
        CompressedInMemory,    // 압축된 상태로 메모리에 로드 (중간 길이)
        Stream                 // 파일에서 직접 스트리밍 (긴 배경음)
    };

    // 오디오 파일 정보 구조체
    struct AudioFileInfo
    {
        float duration = 0.0f;          // 재생 시간 (초)
        size_t fileSize = 0;            // 파일 크기 (바이트)
        ma_uint32 sampleRate = 0;       // 샘플레이트
        ma_uint32 channels = 0;         // 채널 수
        std::string format;             // 파일 포맷 (mp3, wav, ogg 등)
        bool isCompressed = false;      // 압축 포맷 여부

        // Unity 스타일 크기 임계값
        static constexpr size_t SMALL_FILE_THRESHOLD = 200 * 1024;      // 200KB
        static constexpr float SHORT_DURATION_THRESHOLD = 1.0f;         // 1초
        static constexpr float MEDIUM_DURATION_THRESHOLD = 20.0f;       // 20초
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

        // 로드 모드 관리
        AudioLoadMode m_loadMode;
        AudioLoadMode m_originalLoadMode;
        AudioLoadMode m_recommendedLoadMode;   // 자동 결정된 권장 모드
        bool m_loadModeOverridden;              // 수동으로 설정되었는지 여부

        // 기본 오디오 정보
        float m_length;
        ma_uint32 m_channels;
        ma_uint32 m_sampleRate;
        bool m_isLoaded;
        bool m_preloadAudioData;
        AudioFileInfo m_fileInfo;              // 파일 정보

        // CompressedInMemory 관련 추가 멤버
        std::vector<uint8_t> m_compressedData;      // 압축된 원본 파일 데이터
        bool m_compressedDataLoaded;                // 압축 데이터 로드 여부

        // CompressedInMemory 처리 함수들
        bool LoadCompressedDataInternal();
        void UnloadCompressedDataInternal();
        bool HasCompressedData() const { return m_compressedDataLoaded && !m_compressedData.empty(); }

        const void* GetCompressedData() { return &m_compressedData; }

        // Stream PreLoad 관련 멤버
        std::vector<float> m_firstChunkPCM;         // 첫 번째 청크 PCM 데이터
        bool m_firstChunkLoaded;                    // 첫 번째 청크 로드 여부
        float m_firstChunkDuration;                 // 첫 번째 청크 길이 (초)
        ma_uint64 m_firstChunkFrames;               // 첫 번째 청크 프레임 수

        // Stream PreLoad 설정
        static constexpr float DEFAULT_FIRST_CHUNK_DURATION = 0.5f;  // 0.5초
        static constexpr ma_uint64 MIN_CHUNK_FRAMES = 4096;          // 최소 프레임 수

        // Stream PreLoad 처리 함수들
        bool LoadFirstChunkInternal();
        void UnloadFirstChunkInternal();
        ma_uint64 CalculateFirstChunkSize();

        // 실제 오디오 데이터 저장
        std::unique_ptr<AudioData> m_audioData;

        // 리샘플링 관련
        std::unique_ptr<AudioData> m_resampledData;
        ma_uint32 m_targetSampleRate;
        bool m_autoResampleOnPreload;  // 프리로드 시 자동 리샘플링 여부

        // 스트리밍용 디코더 설정
        ma_decoder_config m_decoderConfig;

        // 내부 구현 함수들
        void LoadFromFilePath(const std::wstring& filePath) override;
        bool LoadAudioDataInternal();
        void UnloadAudioDataInternal();

        // 파일 분석 및 로드타입 결정
        AudioLoadMode DetermineLoadModeByFileInfo(const AudioFileInfo& fileInfo);
        bool AnalyzeAudioFile(const std::wstring& filePath, AudioFileInfo& fileInfo);
        size_t GetFileSize(const std::wstring& filePath);

        // 리샘플링 함수들
        bool ResamplePCMData(ma_uint32 targetSampleRate);
        void ClearResampledData();

        // PCM 데이터 유효성 검사
        void ValidateAndClampPCMData();

        ~AudioClip();
    public:
        AudioClip();

        // 기본 정보
        bool IsValidData() override { return m_isLoaded; }
        float GetLength() const { return m_length; }
        ma_uint32 GetChannels() const { return m_channels; }
        ma_uint32 GetSampleRate() const;  // 리샘플링 고려한 샘플레이트 반환
        ma_uint32 GetOriginalSampleRate() const { return m_sampleRate; }  // 원본 샘플레이트

        // 로드 모드 관리
        AudioLoadMode GetLoadMode() const { return m_loadMode; }
        AudioLoadMode GetOriginalLoadMode() const { return m_originalLoadMode; }
        AudioLoadMode GetRecommendedLoadMode() const { return m_recommendedLoadMode; }
        const AudioFileInfo& GetFileInfo() const { return m_fileInfo; }
        bool IsLoadModeOverridden() const { return m_loadModeOverridden; }

        // Stream PreLoad 관련 함수들
        bool IsFirstChunkLoaded() const { return m_firstChunkLoaded; }
        const float* GetFirstChunkPCM() const;
        ma_uint64 GetFirstChunkFrames() const { return m_firstChunkFrames; }
        float GetFirstChunkDuration() const { return m_firstChunkDuration; }
        size_t GetFirstChunkSize() const;

        // preloadAudioData 관련 함수들
        void SetPreloadAudioData(bool preload);
        bool GetPreloadAudioData() const { return m_preloadAudioData; }
        bool IsAudioDataLoaded() const { return m_audioData && m_audioData->isLoaded; }

        // 로드타입 변경 기능 (Unity Inspector 스타일)
        bool SetLoadModeOverride(AudioLoadMode mode, bool force = false);
        void ResetToRecommendedLoadMode();
        bool CanChangeLoadMode() const;

        // 리샘플링 관련 함수들
        void SetAutoResampleOnPreload(bool autoResample) { m_autoResampleOnPreload = autoResample; }
        bool GetAutoResampleOnPreload() const { return m_autoResampleOnPreload; }
        bool HasResampledData(ma_uint32 targetSampleRate) const;
        const float* GetResampledPCMData(ma_uint32 targetSampleRate);
        ma_uint64 GetResampledFrameCount(ma_uint32 targetSampleRate);

        // 분석 및 권장사항
        std::string GetLoadModeRecommendationReason() const;

        // 유니티 스타일 API
        bool LoadAudioData();
        bool UnloadAudioData();

        // 메모리 관리
        size_t GetMemoryUsage() const;
        size_t GetTotalMemoryUsage() const;  // 원본 + 리샘플링 데이터 포함
        void ForcedLoadAudioData();
        void ForcedUnloadAudioData();

        // CompressedInMemory 관련 함수들
        size_t GetCompressedDataSize() const;
        bool IsCompressedDataLoaded() const { return m_compressedDataLoaded; }

        // AudioSource에서 직접 PCM 데이터 접근용
        const float* GetPCMData() const;
        ma_uint64 GetFrameCount() const;
        bool HasValidPCMData() const;

        // 디버그 정보
        void PrintDetailedFileInfo() const;
    };

    // AudioClip 배치 분석 클래스
    class AudioClipAnalyzer
    {
    public:
        struct AnalysisResult {
            int totalClips = 0;
            int decompressOnLoadCount = 0;
            int compressedInMemoryCount = 0;
            int streamCount = 0;
            int overriddenCount = 0;
            float totalMemoryMB = 0.0f;
            std::vector<std::string> recommendations;
        };

        static AnalysisResult AnalyzeProject(const std::vector<AudioClip*>& clips);
        static void PrintAnalysisReport(const AnalysisResult& result);
    };
}