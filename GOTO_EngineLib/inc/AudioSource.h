#pragma once
#include "Behaviour.h"
#include "Vector2.h"
#include <miniaudio.h>

namespace GOTOEngine
{
	class AudioClip;

	//======== 오디오 소스 ========
	//
	//          * 주의 사항 *
	// 
	//  GetClip()을 대입식에 넣어놓은 경우
	//  반드시 IncreaseRefCount()를 호출하여 
	//  참조카운트가 늘어나지 않도록 합니다.
	// 
	//  ex) AudioClip* clip = audioSource->GetClip();
	//      clip->IncreaseRefCount();
	//
	// 
	//  이후 사용을 끝마칠 경우 반드시 
	//  DecreaseRefCount()를 호출하여 마감가지로
	//  참조카운트가 늘어나지 않도록 합니다.
	// 
	//  ex) clip->DecreaseRefCount(); //코드 블록 끝에
	// 
	//
	//=================================

	class AudioSource : public Behaviour
	{
	private:
		AudioClip* m_clip;
		ma_sound m_sound;
		bool m_soundInitialized;

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

		void OnEnable();
		void OnDisable();
		void OnDestroy();

		void InitializeSound();
		void CleanupSound();
		void ApplySettings();

	protected:
		~AudioSource();

	public:
		void SetClip(AudioClip* clip);
		AudioClip* GetClip() const { return m_clip; } // 참조 카운트 명시적으로 설정하기
		AudioSource();

		void Play();
		void PlayOneShot(); 
		void Stop();
		void Pause();
		void Resume();

		bool IsPlaying() const { return m_isPlaying && !m_isPaused; }
		bool IsPaused() const { return m_isPaused; }

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

		void RegisterMessages();
	};
}