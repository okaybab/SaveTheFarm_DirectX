#define NOMINMAX
#include "AudioClip.h"
#include "AudioManager.h"
#include "WStringHelper.h"
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace GOTOEngine;

AudioClip::AudioClip()
    : m_loadMode(AudioLoadMode::DecompressOnLoad)
    , m_originalLoadMode(AudioLoadMode::DecompressOnLoad)
    , m_recommendedLoadMode(AudioLoadMode::DecompressOnLoad)
    , m_loadModeOverridden(false)
    , m_length(0.0f)
    , m_channels(0)
    , m_sampleRate(0)
    , m_isLoaded(false)
    , m_preloadAudioData(false)
    , m_audioData(nullptr)
    , m_resampledData(nullptr)
    , m_targetSampleRate(0)
    , m_autoResampleOnPreload(true)
    , m_compressedDataLoaded(false)
    , m_firstChunkLoaded(false)          
    , m_firstChunkDuration(0.0f)         
    , m_firstChunkFrames(0)              
{
}

AudioClip::~AudioClip()
{
    UnloadAudioDataInternal();
    UnloadCompressedDataInternal();
    UnloadFirstChunkInternal();
    ClearResampledData();
}

// ===================================================================
// 파일 분석 및 로드타입 결정
// ===================================================================

bool AudioClip::AnalyzeAudioFile(const std::wstring& filePath, AudioFileInfo& fileInfo)
{
    // 1. 파일 크기 획득
    fileInfo.fileSize = GetFileSize(filePath);

    // 2. 파일 포맷 확인
    size_t lastDot = filePath.find_last_of(L".");
    if (lastDot != std::wstring::npos) {
        std::wstring extension = filePath.substr(lastDot + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);
        fileInfo.format = WStringHelper::wstring_to_string(extension);
        fileInfo.isCompressed = (extension == L"mp3" || extension == L"ogg" || extension == L"m4a");
    }

    // 3. 오디오 정보 획득 (miniaudio 디코더 사용)
    std::string filePathUtf8 = WStringHelper::wstring_to_string(filePath);
    ma_decoder_config config = ma_decoder_config_init_default();
    ma_decoder decoder;

    ma_result result = ma_decoder_init_file(filePathUtf8.c_str(), &config, &decoder);
    if (result != MA_SUCCESS) {
        return false;
    }

    fileInfo.channels = decoder.outputChannels;
    fileInfo.sampleRate = decoder.outputSampleRate;

    // 길이 계산
    ma_uint64 frameCount;
    if (ma_decoder_get_length_in_pcm_frames(&decoder, &frameCount) == MA_SUCCESS) {
        fileInfo.duration = (float)frameCount / (float)decoder.outputSampleRate;
    }

    ma_decoder_uninit(&decoder);

#ifdef _DEBUG_AUDIO
    std::cout << "Audio file analysis:" << std::endl;
    std::cout << "  - File: " << filePathUtf8 << std::endl;
    std::cout << "  - Size: " << (fileInfo.fileSize / 1024.0f) << " KB" << std::endl;
    std::cout << "  - Duration: " << fileInfo.duration << " seconds" << std::endl;
    std::cout << "  - Format: " << fileInfo.format << (fileInfo.isCompressed ? " (compressed)" : " (uncompressed)") << std::endl;
    std::cout << "  - Sample Rate: " << fileInfo.sampleRate << " Hz" << std::endl;
    std::cout << "  - Channels: " << fileInfo.channels << std::endl;
#endif

    return true;
}

AudioLoadMode AudioClip::DetermineLoadModeByFileInfo(const AudioFileInfo& fileInfo)
{
    // Unity 로직을 기반으로 한 결정 트리

    // 1. 매우 짧고 작은 파일 -> DecompressOnLoad
    if (fileInfo.duration < AudioFileInfo::SHORT_DURATION_THRESHOLD &&
        fileInfo.fileSize < AudioFileInfo::SMALL_FILE_THRESHOLD) {
        return AudioLoadMode::DecompressOnLoad;
    }

    // 2. 중간 길이 파일 -> CompressedInMemory
    else if (fileInfo.duration < AudioFileInfo::MEDIUM_DURATION_THRESHOLD) {
        // 압축 포맷이고 크기가 적당하면 CompressedInMemory
        if (fileInfo.isCompressed && fileInfo.fileSize < 2 * 1024 * 1024) { // 2MB 이하
            return AudioLoadMode::CompressedInMemory;
        }
        // 무압축이거나 작은 파일이면 DecompressOnLoad
        else if (fileInfo.fileSize < AudioFileInfo::SMALL_FILE_THRESHOLD) {
            return AudioLoadMode::DecompressOnLoad;
        }
        else {
            return AudioLoadMode::CompressedInMemory;
        }
    }

    // 3. 긴 파일 -> Stream
    else {
        return AudioLoadMode::Stream;
    }
}

size_t AudioClip::GetFileSize(const std::wstring& filePath)
{
    std::ifstream file(filePath, std::ifstream::ate | std::ifstream::binary);
    if (file.good()) {
        return static_cast<size_t>(file.tellg());
    }
    return 0;
}

bool GOTOEngine::AudioClip::LoadCompressedDataInternal()
{
    if (m_compressedDataLoaded && !m_compressedData.empty()) {
        return true; // 이미 로드됨
    }

    if (!m_isLoaded) {
        return false;
    }

    std::string filePathUtf8 = WStringHelper::wstring_to_string(m_filePath);

    // 파일을 바이너리로 읽어서 메모리에 저장
    std::ifstream file(m_filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
#ifdef _DEBUG_AUDIO
        std::cout << "Failed to open file for compressed loading: " << filePathUtf8 << std::endl;
#endif
        return false;
    }

    // 파일 크기 확인
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // 압축 데이터 저장
    m_compressedData.resize(fileSize);
    if (!file.read(reinterpret_cast<char*>(m_compressedData.data()), fileSize)) {
#ifdef _DEBUG_AUDIO
        std::cout << "Failed to read compressed data: " << filePathUtf8 << std::endl;
#endif
        m_compressedData.clear();
        return false;
    }

    file.close();
    m_compressedDataLoaded = true;

#ifdef _DEBUG_AUDIO
    std::cout << "Compressed data loaded: " << filePathUtf8
        << " (" << (fileSize / 1024.0f) << " KB)" << std::endl;
#endif

    return true;
}

void GOTOEngine::AudioClip::UnloadCompressedDataInternal()
{
    if (m_compressedDataLoaded) {
        m_compressedData.clear();
        m_compressedDataLoaded = false;

#ifdef _DEBUG_AUDIO
        std::cout << "Compressed data unloaded from memory" << std::endl;
#endif
    }
}

void AudioClip::LoadFromFilePath(const std::wstring& filePath)
{
    if (!AudioManager::Get()->IsInitialized()) {
#ifdef _DEBUG_AUDIO
        std::cout << "AudioManager not initialized." << std::endl;
#endif
        return;
    }

    // 1. 파일 분석
    if (!AnalyzeAudioFile(filePath, m_fileInfo)) {
#ifdef _DEBUG_AUDIO
        std::cout << "Failed to analyze audio file: " << WStringHelper::wstring_to_string(filePath) << std::endl;
#endif
        m_isLoaded = false;
        return;
    }

    // 2. 권장 로드타입 결정
    m_recommendedLoadMode = DetermineLoadModeByFileInfo(m_fileInfo);

    // 3. 실제 로드타입 설정
    if (!m_loadModeOverridden) {
        m_loadMode = m_recommendedLoadMode;
    }

    m_originalLoadMode = m_loadMode;

    // 4. 기본 오디오 정보 설정
    m_channels = m_fileInfo.channels;
    m_sampleRate = m_fileInfo.sampleRate;
    m_length = m_fileInfo.duration;

    // 5. 디코더 설정 초기화
    m_decoderConfig = ma_decoder_config_init_default();
    m_isLoaded = true;

    // 6. 로드 모드에 따른 데이터 로드
    if (m_preloadAudioData) {
        if (m_loadMode == AudioLoadMode::DecompressOnLoad) {
            // PCM 데이터 로드 + 리샘플링
            LoadAudioDataInternal();
            if (m_autoResampleOnPreload && AudioManager::Get()->IsInitialized()) {
                ma_uint32 engineSampleRate = ma_engine_get_sample_rate(AudioManager::Get()->GetEngine());
                if (engineSampleRate != m_sampleRate) {
                    ResamplePCMData(engineSampleRate);
                }
            }
        }
        else if (m_loadMode == AudioLoadMode::CompressedInMemory) {
            // 압축된 원본 파일 데이터 로드
            LoadCompressedDataInternal();
        }
        // Stream 모드는 프리로드하지 않음
    }

#ifdef _DEBUG_AUDIO
    PrintDetailedFileInfo();
#endif
}

// ===================================================================
// 리샘플링 구현
// ===================================================================

bool AudioClip::ResamplePCMData(ma_uint32 targetSampleRate)
{
    if (!IsAudioDataLoaded() || targetSampleRate == 0) {
        return false;
    }

    // 이미 목표 샘플레이트와 같으면 리샘플링 불필요
    if (m_sampleRate == targetSampleRate) {
        return true;
    }

    // 기존 리샘플링 데이터가 있고 같은 타겟이면 재사용
    if (m_resampledData && m_targetSampleRate == targetSampleRate) {
        return true;
    }

#ifdef _DEBUG_AUDIO
    std::cout << "Resampling audio data: " << m_sampleRate << " -> " << targetSampleRate << " Hz" << std::endl;
#endif

    // 리샘플러 설정
    ma_resampler_config resamplerConfig = ma_resampler_config_init(
        ma_format_f32,          // 입력 포맷
        m_channels,             // 채널 수
        m_sampleRate,           // 입력 샘플레이트
        targetSampleRate,       // 출력 샘플레이트
        ma_resample_algorithm_linear  // 리샘플링 알고리즘
    );

    ma_resampler resampler;
    ma_result result = ma_resampler_init(&resamplerConfig, nullptr, &resampler);

    if (result != MA_SUCCESS) {
#ifdef _DEBUG_AUDIO
        std::cout << "Failed to initialize resampler: " << result << std::endl;
#endif
        return false;
    }

    // 출력 프레임 수 계산
    ma_uint64 outputFrameCount = (ma_uint64)((double)m_audioData->frameCount * targetSampleRate / m_sampleRate);

    // 리샘플링된 데이터 생성
    if (!m_resampledData) {
        m_resampledData = std::make_unique<AudioData>();
    }

    m_resampledData->channels = m_channels;
    m_resampledData->sampleRate = targetSampleRate;
    m_resampledData->frameCount = outputFrameCount;
    m_resampledData->samples.resize(outputFrameCount * m_channels);

    // 리샘플링 실행
    ma_uint64 inputFramesRead = m_audioData->frameCount;
    ma_uint64 outputFramesWritten = outputFrameCount;

    result = ma_resampler_process_pcm_frames(
        &resampler,
        m_audioData->samples.data(),
        &inputFramesRead,
        m_resampledData->samples.data(),
        &outputFramesWritten
    );

    ma_resampler_uninit(&resampler, nullptr);

    if (result == MA_SUCCESS) {
        m_resampledData->frameCount = outputFramesWritten;
        m_resampledData->isLoaded = true;
        m_targetSampleRate = targetSampleRate;

#ifdef _DEBUG_AUDIO
        std::cout << "Resampling completed:" << std::endl;
        std::cout << "  - Input frames: " << inputFramesRead << std::endl;
        std::cout << "  - Output frames: " << outputFramesWritten << std::endl;
        std::cout << "  - Size change: " << (m_audioData->samples.size() * sizeof(float) / 1024.0f)
            << " -> " << (m_resampledData->samples.size() * sizeof(float) / 1024.0f) << " KB" << std::endl;
#endif
        return true;
    }
    else {
#ifdef _DEBUG_AUDIO
        std::cout << "Resampling failed: " << result << std::endl;
#endif
        m_resampledData.reset();
        return false;
    }
}

void AudioClip::ClearResampledData()
{
    if (m_resampledData) {
        m_resampledData.reset();
        m_targetSampleRate = 0;
#ifdef _DEBUG_AUDIO
        std::cout << "Resampled data cleared" << std::endl;
#endif
    }
}

const float* AudioClip::GetResampledPCMData(ma_uint32 targetSampleRate)
{
    if (ResamplePCMData(targetSampleRate)) {
        return m_resampledData->samples.data();
    }
    return GetPCMData();  // 폴백: 원본 데이터
}

ma_uint64 AudioClip::GetResampledFrameCount(ma_uint32 targetSampleRate)
{
    if (ResamplePCMData(targetSampleRate)) {
        return m_resampledData->frameCount;
    }
    return GetFrameCount();  // 폴백: 원본 데이터
}

bool AudioClip::HasResampledData(ma_uint32 targetSampleRate) const
{
    return m_resampledData && m_resampledData->isLoaded && m_targetSampleRate == targetSampleRate;
}

// ===================================================================
// PCM 데이터 로딩 및 관리
// ===================================================================

bool AudioClip::LoadAudioDataInternal()
{
    if (m_audioData && m_audioData->isLoaded)
        return true; // 이미 로드됨

    if (!m_isLoaded)
        return false;

    std::string filePathUtf8 = WStringHelper::wstring_to_string(m_filePath);

    ma_decoder decoder;

    // 디코더 설정을 float32로 명시적 설정
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 0, 0);
    ma_result result = ma_decoder_init_file(filePathUtf8.c_str(), &config, &decoder);

    if (result != MA_SUCCESS)
    {
#ifdef _DEBUG_AUDIO
        std::cout << "Failed to initialize decoder. Error: " << result << std::endl;
#endif
        return false;
    }

    // 전체 프레임 수 얻기
    ma_uint64 frameCount;
    if (ma_decoder_get_length_in_pcm_frames(&decoder, &frameCount) != MA_SUCCESS)
    {
        ma_decoder_uninit(&decoder);
#ifdef _DEBUG_AUDIO
        std::cout << "Failed to get frame count" << std::endl;
#endif
        return false;
    }

    // AudioData 생성
    if (!m_audioData)
        m_audioData = std::make_unique<AudioData>();

    // 디코더에서 실제 출력 포맷 정보 가져오기
    m_audioData->channels = decoder.outputChannels;
    m_audioData->sampleRate = decoder.outputSampleRate;
    m_audioData->frameCount = frameCount;

#ifdef _DEBUG_AUDIO
    std::cout << "Decoder output format:" << std::endl;
    std::cout << "  - Format: " << decoder.outputFormat << " (expected: " << ma_format_f32 << ")" << std::endl;
    std::cout << "  - Channels: " << decoder.outputChannels << std::endl;
    std::cout << "  - Sample Rate: " << decoder.outputSampleRate << std::endl;
    std::cout << "  - Frame Count: " << frameCount << std::endl;
#endif

    // PCM 데이터 읽기 (float 형식)
    ma_uint64 totalSamples = frameCount * decoder.outputChannels;
    m_audioData->samples.resize(totalSamples);

    ma_uint64 framesRead;
    result = ma_decoder_read_pcm_frames(&decoder, m_audioData->samples.data(), frameCount, &framesRead);

    ma_decoder_uninit(&decoder);

    if (result == MA_SUCCESS || result == MA_AT_END)
    {
        m_audioData->frameCount = framesRead;
        m_audioData->isLoaded = true;

        // PCM 데이터 유효성 검사
        ValidateAndClampPCMData();

#ifdef _DEBUG_AUDIO
        float sizeInMB = (totalSamples * sizeof(float)) / (1024.0f * 1024.0f);
        std::cout << "Audio data loaded into memory: " << filePathUtf8
            << " (" << sizeInMB << " MB)" << std::endl;

        // 첫 몇 샘플 출력하여 확인
        std::cout << "First samples after validation: ";
        for (int i = 0; i < std::min(8, (int)totalSamples); ++i) {
            std::cout << m_audioData->samples[i] << " ";
        }
        std::cout << std::endl;
#endif
        return true;
    }

#ifdef _DEBUG_AUDIO
    std::cout << "Failed to read PCM frames. Error: " << result << std::endl;
#endif
    return false;
}

void AudioClip::ValidateAndClampPCMData()
{
    if (!m_audioData || !m_audioData->isLoaded || m_audioData->samples.empty())
        return;

    bool foundOutOfRange = false;
    float maxValue = 0.0f;
    float minValue = 0.0f;

    // 범위를 벗어난 값들 찾기
    for (float& sample : m_audioData->samples)
    {
        if (sample > 1.0f || sample < -1.0f)
        {
            foundOutOfRange = true;
        }
        maxValue = std::max(maxValue, sample);
        minValue = std::min(minValue, sample);
    }

#ifdef _DEBUG_AUDIO
    std::cout << "PCM data range: " << minValue << " to " << maxValue << std::endl;
#endif

    if (foundOutOfRange)
    {
#ifdef _DEBUG_AUDIO
        std::cout << "Warning: PCM data out of range [-1.0, 1.0]. Clamping values." << std::endl;
#endif

        // 범위를 벗어난 값들을 클램핑
        for (float& sample : m_audioData->samples)
        {
            sample = std::max(-1.0f, std::min(1.0f, sample));
        }
    }

    // 추가 검사: NaN이나 무한대 값 제거
    for (float& sample : m_audioData->samples)
    {
        if (!std::isfinite(sample))
        {
#ifdef _DEBUG_AUDIO
            std::cout << "Warning: Found non-finite sample value, setting to 0" << std::endl;
#endif
            sample = 0.0f;
        }
    }
}

void AudioClip::UnloadAudioDataInternal()
{
    if (m_audioData)
    {
        m_audioData.reset();
#ifdef _DEBUG_AUDIO
        std::cout << "Audio data unloaded from memory" << std::endl;
#endif
    }
}

// ===================================================================
// 프리로드 및 로드타입 관리
// ===================================================================

void AudioClip::SetPreloadAudioData(bool preload)
{
    if (m_preloadAudioData == preload)
        return;

    m_preloadAudioData = preload;

    if (preload) {
        if (m_loadMode == AudioLoadMode::DecompressOnLoad && !IsAudioDataLoaded()) {
            LoadAudioDataInternal();

            if (m_autoResampleOnPreload && AudioManager::Get()->IsInitialized()) {
                ma_uint32 engineSampleRate = ma_engine_get_sample_rate(AudioManager::Get()->GetEngine());
                if (engineSampleRate != m_sampleRate) {
                    ResamplePCMData(engineSampleRate);
                }
            }
        }
        else if (m_loadMode == AudioLoadMode::CompressedInMemory && !IsCompressedDataLoaded()) {
            LoadCompressedDataInternal();
        }
        else if (m_loadMode == AudioLoadMode::Stream && !IsFirstChunkLoaded()) {
            // Stream 모드에서 첫 번째 청크만 프리로드
            LoadFirstChunkInternal();
        }
    }
    else {
        // 언로드
        UnloadAudioDataInternal();
        UnloadCompressedDataInternal();
        UnloadFirstChunkInternal();
        ClearResampledData();
    }
}

ma_uint64 AudioClip::CalculateFirstChunkSize()
{
    if (!m_isLoaded) return 0;

    // 1. 시간 기반 계산 (기본 0.5초)
    ma_uint64 timeBasedFrames = (ma_uint64)(m_sampleRate * DEFAULT_FIRST_CHUNK_DURATION);

    // 2. 최소 프레임 수 보장
    ma_uint64 chunkFrames = std::max(timeBasedFrames, MIN_CHUNK_FRAMES);

    // 3. 전체 길이를 초과하지 않도록 제한
    std::string filePathUtf8 = WStringHelper::wstring_to_string(m_filePath);
    ma_decoder_config config = ma_decoder_config_init_default();
    ma_decoder decoder;

    ma_result result = ma_decoder_init_file(filePathUtf8.c_str(), &config, &decoder);
    if (result == MA_SUCCESS) {
        ma_uint64 totalFrames;
        if (ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames) == MA_SUCCESS) {
            chunkFrames = std::min(chunkFrames, totalFrames);
        }
        ma_decoder_uninit(&decoder);
    }

    return chunkFrames;
}

bool AudioClip::LoadFirstChunkInternal()
{
    if (m_firstChunkLoaded && !m_firstChunkPCM.empty()) {
        return true; // 이미 로드됨
    }

    if (!m_isLoaded) {
        return false;
    }

    std::string filePathUtf8 = WStringHelper::wstring_to_string(m_filePath);

#ifdef _DEBUG_AUDIO
    std::cout << "Loading first chunk for streaming: " << filePathUtf8 << std::endl;
#endif

    // 디코더 초기화
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 0, 0);
    ma_decoder decoder;

    ma_result result = ma_decoder_init_file(filePathUtf8.c_str(), &config, &decoder);
    if (result != MA_SUCCESS) {
#ifdef _DEBUG_AUDIO
        std::cout << "Failed to initialize decoder for first chunk. Error: " << result << std::endl;
#endif
        return false;
    }

    // 첫 번째 청크 크기 계산
    ma_uint64 chunkFrames = CalculateFirstChunkSize();
    ma_uint64 totalSamples = chunkFrames * decoder.outputChannels;

    // 첫 번째 청크 데이터 읽기
    m_firstChunkPCM.resize(totalSamples);

    ma_uint64 framesRead;
    result = ma_decoder_read_pcm_frames(&decoder, m_firstChunkPCM.data(), chunkFrames, &framesRead);

    ma_decoder_uninit(&decoder);

    if (result == MA_SUCCESS || result == MA_AT_END) {
        // 실제로 읽은 프레임 수로 조정
        m_firstChunkFrames = framesRead;
        m_firstChunkPCM.resize(framesRead * m_channels);
        m_firstChunkDuration = (float)framesRead / (float)m_sampleRate;
        m_firstChunkLoaded = true;

        // PCM 데이터 유효성 검사 (간단한 버전)
        for (float& sample : m_firstChunkPCM) {
            if (!std::isfinite(sample)) {
                sample = 0.0f;
            }
            sample = std::max(-1.0f, std::min(1.0f, sample));
        }

#ifdef _DEBUG_AUDIO
        float sizeInKB = (totalSamples * sizeof(float)) / 1024.0f;
        std::cout << "First chunk loaded successfully:" << std::endl;
        std::cout << "  - Duration: " << m_firstChunkDuration << " seconds" << std::endl;
        std::cout << "  - Frames: " << framesRead << std::endl;
        std::cout << "  - Size: " << sizeInKB << " KB" << std::endl;
        std::cout << "  - Percentage of total: " << (m_firstChunkDuration / m_length * 100.0f) << "%" << std::endl;
#endif
        return true;
    }

#ifdef _DEBUG_AUDIO
    std::cout << "Failed to read first chunk. Error: " << result << std::endl;
#endif
    return false;
}

bool AudioClip::SetLoadModeOverride(AudioLoadMode mode, bool force)
{
    if (m_loadMode == mode && m_loadModeOverridden) {
        return true;
    }

    if (!force && !CanChangeLoadMode()) {
#ifdef _DEBUG_AUDIO
        std::cout << "Cannot change load mode: Audio is currently in use" << std::endl;
#endif
        return false;
    }

    if (mode != m_recommendedLoadMode) {
#ifdef _DEBUG_AUDIO
        const char* modeNames[] = { "DecompressOnLoad", "CompressedInMemory", "Stream" };
        std::cout << "Warning: Overriding recommended load mode" << std::endl;
        std::cout << "  - Recommended: " << modeNames[(int)m_recommendedLoadMode] << std::endl;
        std::cout << "  - Override: " << modeNames[(int)mode] << std::endl;
        std::cout << "  - Reason: " << GetLoadModeRecommendationReason() << std::endl;
#endif
    }

    // 기존 데이터 정리
    UnloadAudioDataInternal();
    UnloadCompressedDataInternal();
    ClearResampledData();

    m_loadMode = mode;
    m_loadModeOverridden = true;

    // 새로운 모드에 따라 데이터 로드
    if (m_preloadAudioData) {
        if (mode == AudioLoadMode::DecompressOnLoad) {
            LoadAudioDataInternal();

            if (m_autoResampleOnPreload && AudioManager::Get()->IsInitialized()) {
                ma_uint32 engineSampleRate = ma_engine_get_sample_rate(AudioManager::Get()->GetEngine());
                if (engineSampleRate != m_sampleRate) {
                    ResamplePCMData(engineSampleRate);
                }
            }
        }
        else if (mode == AudioLoadMode::CompressedInMemory) {
            LoadCompressedDataInternal();
        }
    }

#ifdef _DEBUG_AUDIO
    std::cout << "Load mode override applied successfully" << std::endl;
#endif

    return true;
}

void AudioClip::UnloadFirstChunkInternal()
{
    if (m_firstChunkLoaded) {
        m_firstChunkPCM.clear();
        m_firstChunkLoaded = false;
        m_firstChunkFrames = 0;
        m_firstChunkDuration = 0.0f;

#ifdef _DEBUG_AUDIO
        std::cout << "First chunk unloaded from memory" << std::endl;
#endif
    }
}

const float* AudioClip::GetFirstChunkPCM() const
{
    if (m_firstChunkLoaded && !m_firstChunkPCM.empty()) {
        return m_firstChunkPCM.data();
    }
    return nullptr;
}

size_t AudioClip::GetFirstChunkSize() const
{
    return m_firstChunkPCM.size() * sizeof(float);
}

void AudioClip::ResetToRecommendedLoadMode()
{
    if (!m_loadModeOverridden) {
        return; // 이미 권장 모드 사용 중
    }

    m_loadModeOverridden = false;
    SetLoadModeOverride(m_recommendedLoadMode, true);

#ifdef _DEBUG_AUDIO
    std::cout << "Reset to recommended load mode" << std::endl;
#endif
}

bool AudioClip::CanChangeLoadMode() const
{
    // 파일이 로드되어 있고, AudioSource에서 현재 사용 중이지 않으면 변경 가능
    return m_isLoaded && m_refCount <= 1;  // 자기 자신만 참조하고 있을 때
}

std::string AudioClip::GetLoadModeRecommendationReason() const
{
    std::stringstream reason;

    if (m_fileInfo.duration < AudioFileInfo::SHORT_DURATION_THRESHOLD &&
        m_fileInfo.fileSize < AudioFileInfo::SMALL_FILE_THRESHOLD) {
        reason << "Short duration (" << m_fileInfo.duration << "s) and small size ("
            << (m_fileInfo.fileSize / 1024) << "KB) - optimal for DecompressOnLoad";
    }
    else if (m_fileInfo.duration < AudioFileInfo::MEDIUM_DURATION_THRESHOLD) {
        if (m_fileInfo.isCompressed) {
            reason << "Medium duration (" << m_fileInfo.duration << "s) compressed format - optimal for CompressedInMemory";
        }
        else {
            reason << "Medium duration (" << m_fileInfo.duration << "s) uncompressed format - consider DecompressOnLoad or CompressedInMemory";
        }
    }
    else {
        reason << "Long duration (" << m_fileInfo.duration << "s) - optimal for Streaming to save memory";
    }

    return reason.str();
}

bool AudioClip::LoadAudioData()
{
    // DecompressOnLoad 모드가 아니면 실패 (유니티와 동일)
    if (m_loadMode != AudioLoadMode::DecompressOnLoad)
    {
#ifdef _DEBUG_AUDIO
        std::cout << "LoadAudioData failed: Clip is not in DecompressOnLoad mode" << std::endl;
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
    ClearResampledData();

    // 관련된 AudioSource들에게 준비 필요 상태로 설정
    if (AudioManager::Get()->IsInitialized())
    {
        AudioManager::Get()->NotifyClipUnloaded(this);
    }

#ifdef _DEBUG_AUDIO
    std::cout << "Audio data unloaded manually" << std::endl;
#endif
    return true;
}

// ===================================================================
// 데이터 접근 함수들
// ===================================================================

// 리샘플링을 고려한 데이터 반환
const float* AudioClip::GetPCMData() const
{
    // 리샘플링된 데이터가 있으면 우선 반환
    if (m_resampledData && m_resampledData->isLoaded) {
        return m_resampledData->samples.data();
    }

    // 원본 데이터 반환
    if (!IsAudioDataLoaded()) return nullptr;
    return m_audioData->samples.data();
}

ma_uint64 AudioClip::GetFrameCount() const
{
    if (m_resampledData && m_resampledData->isLoaded) {
        return m_resampledData->frameCount;
    }

    if (!IsAudioDataLoaded()) return 0;
    return m_audioData->frameCount;
}

ma_uint32 AudioClip::GetSampleRate() const
{
    if (m_resampledData && m_resampledData->isLoaded) {
        return m_resampledData->sampleRate;
    }

    return m_sampleRate;
}

bool AudioClip::HasValidPCMData() const
{
    return IsAudioDataLoaded() && m_audioData->samples.size() > 0;
}

size_t AudioClip::GetMemoryUsage() const
{
    if (!IsAudioDataLoaded())
        return 0;

    return m_audioData->samples.size() * sizeof(float);
}

size_t AudioClip::GetTotalMemoryUsage() const
{
    size_t total = 0;

    if (IsAudioDataLoaded()) {
        total += m_audioData->samples.size() * sizeof(float);
    }

    if (m_resampledData && m_resampledData->isLoaded) {
        total += m_resampledData->samples.size() * sizeof(float);
    }

    if (IsCompressedDataLoaded()) {
        total += m_compressedData.size();
    }

    if (IsFirstChunkLoaded()) {
        total += GetFirstChunkSize();
    }

    return total;
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
    ClearResampledData();
}

size_t GOTOEngine::AudioClip::GetCompressedDataSize() const
{
    return m_compressedData.size();
}

void AudioClip::PrintDetailedFileInfo() const
{
    const char* modeNames[] = { "DecompressOnLoad", "CompressedInMemory", "Stream" };

    std::cout << "\n=== AUDIO CLIP ANALYSIS ===" << std::endl;
    std::cout << "File: " << WStringHelper::wstring_to_string(m_filePath) << std::endl;
    std::cout << "Size: " << (m_fileInfo.fileSize / 1024.0f) << " KB" << std::endl;
    std::cout << "Duration: " << m_fileInfo.duration << " seconds" << std::endl;
    std::cout << "Format: " << m_fileInfo.format << (m_fileInfo.isCompressed ? " (compressed)" : " (uncompressed)") << std::endl;
    std::cout << "Sample Rate: " << m_fileInfo.sampleRate << " Hz" << std::endl;
    std::cout << "Channels: " << m_fileInfo.channels << std::endl;
    std::cout << "Recommended Mode: " << modeNames[(int)m_recommendedLoadMode] << std::endl;
    std::cout << "Current Mode: " << modeNames[(int)m_loadMode]
        << (m_loadModeOverridden ? " (OVERRIDDEN)" : " (recommended)") << std::endl;
    std::cout << "Reason: " << GetLoadModeRecommendationReason() << std::endl;
    std::cout << "=========================" << std::endl;
}

// ===================================================================
// AudioClipAnalyzer 구현
// ===================================================================

AudioClipAnalyzer::AnalysisResult AudioClipAnalyzer::AnalyzeProject(const std::vector<AudioClip*>& clips)
{
    AnalysisResult result;

    for (auto* clip : clips) {
        if (!clip) continue;

        result.totalClips++;

        switch (clip->GetLoadMode()) {
        case AudioLoadMode::DecompressOnLoad:
            result.decompressOnLoadCount++;
            break;
        case AudioLoadMode::CompressedInMemory:
            result.compressedInMemoryCount++;
            break;
        case AudioLoadMode::Stream:
            result.streamCount++;
            break;
        }

        if (clip->IsLoadModeOverridden()) {
            result.overriddenCount++;
        }

        result.totalMemoryMB += clip->GetTotalMemoryUsage() / (1024.0f * 1024.0f);

        // 권장사항 생성
        if (clip->GetLoadMode() != clip->GetRecommendedLoadMode()) {
            std::string recommendation = "Consider changing '" +
                WStringHelper::wstring_to_string(clip->GetFilePath()) +
                "' to recommended mode for better performance";
            result.recommendations.push_back(recommendation);
        }
    }

    return result;
}

void AudioClipAnalyzer::PrintAnalysisReport(const AnalysisResult& result)
{
    std::cout << "\n=== PROJECT AUDIO ANALYSIS ===" << std::endl;
    std::cout << "Total Clips: " << result.totalClips << std::endl;
    std::cout << "Load Mode Distribution:" << std::endl;
    std::cout << "  - DecompressOnLoad: " << result.decompressOnLoadCount << std::endl;
    std::cout << "  - CompressedInMemory: " << result.compressedInMemoryCount << std::endl;
    std::cout << "  - Stream: " << result.streamCount << std::endl;
    std::cout << "Overridden: " << result.overriddenCount << std::endl;
    std::cout << "Total Memory Usage: " << result.totalMemoryMB << " MB" << std::endl;

    if (!result.recommendations.empty()) {
        std::cout << "\nRecommendations:" << std::endl;
        for (const auto& rec : result.recommendations) {
            std::cout << "  - " << rec << std::endl;
        }
    }
    std::cout << "==============================" << std::endl;
}