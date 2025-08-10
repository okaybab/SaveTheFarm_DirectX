#pragma once
#include "Behaviour.h"
#include "Vector2.h"
#include <miniaudio.h>

namespace GOTOEngine
{
	class AudioClip;

	class AudioSource : public Behaviour
	{
	private:
		friend class AudioManager;

		struct OneShotSound {
			ma_sound sound;
			bool initialized = false;
			float startTime = 0.0f;  // 재생 시작 시간 (정리용)

			~OneShotSound() {
				if (initialized) {
					ma_sound_uninit(&sound);
				}
			}
		};

		// OneShot 사운드들을 관리하는 컨테이너
		std::vector<std::unique_ptr<OneShotSound>> m_oneShotSounds;

		// OneShot 정리 함수 추가
		void CleanupFinishedOneShots();
		float m_lastCleanupTime;
		static constexpr float CLEANUP_INTERVAL = 1.0f;  // 1초마다 정리

		AudioClip* m_clip;

		// 메모리 기반 재생용
		ma_audio_buffer m_audioBuffer;
		bool m_audioBufferInitialized;
		ma_sound m_sound;
		bool m_soundInitialized;

		// 스트리밍 재생용
		ma_sound m_streamSound;
		bool m_streamSoundInitialized;

		float m_volume;
		float m_pitch;
		bool m_loop;
		bool m_playOnAwake;
		bool m_spatialAudio;

		float m_minDistance;
		float m_maxDistance;

		bool m_isPlaying;
		bool m_isPaused;
		Vector2 m_lastPosition;
		bool m_positionDirty;

		// 준비 상태 관리
		bool m_soundReady;
		bool m_needsPrepare;

		void OnEnable();
		void OnDisable();

		void InitializeMemorySound();    // 메모리 기반 사운드 초기화
		void InitializeStreamSound();    // 스트리밍 사운드 초기화
		void InitializeStreamSoundWithFirstChunk();
		void InitializeCompressedMemorySound();
		void CleanupSounds();           // 모든 사운드 정리
		void ApplySettings();
		void AutoPrepareIfNeeded();

		// AudioManager에서만 호출하는 함수들
		void InternalUpdate();
		void MarkNeedsPrepare() { m_needsPrepare = true; m_soundReady = false; }

	

		// 현재 사용할 사운드 객체 반환
		ma_sound* GetActiveSound();
		~AudioSource();
	public:
		AudioSource();

		void Dispose() override;

		void SetClip(AudioClip* clip);
		AudioClip* GetClip() const { return m_clip; }

		void Play();
		void PlayOneShot();
		void PlayOneShot(AudioClip* clip, float volumeScale = 1.0f);
		void Stop();
		void Pause();
		void Resume();

		bool IsPlaying() const { return m_isPlaying && !m_isPaused; }
		bool IsPaused() const { return m_isPaused; }
		bool IsSoundReady() const { return m_soundReady; }

		void SetVolume(float volume);
		float GetVolume() const { return m_volume; }

		void SetPitch(float pitch);
		float GetPitch() const { return m_pitch; }

		void SetLoop(bool loop);
		bool IsLooping() const { return m_loop; }

		void SetPlayOnAwake(bool playOnAwake) { m_playOnAwake = playOnAwake; }
		bool GetPlayOnAwake() const { return m_playOnAwake; }

		void SetSpatialAudio(bool enabled);
		bool IsSpatialAudio() const { return m_spatialAudio; }

		void SetMinDistance(float distance);
		float GetMinDistance() const { return m_minDistance; }

		void SetMaxDistance(float distance);
		float GetMaxDistance() const { return m_maxDistance; }

		void UpdateTransform();


		void PrintDebugInfo() const;

		void FullAudioDiagnosis();

		void DiagnosePlaybackIssues();
		void ForceAudioSystemTest();
		void PlayTestTone();
	};
}