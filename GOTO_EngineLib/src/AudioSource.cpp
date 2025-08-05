#define NOMINMAX

#include "AudioSource.h"
#include "AudioManager.h"
#include "AudioClip.h"
#include "Transform.h"
#include "GameObject.h"
#include <iostream>
#include <algorithm>

using namespace GOTOEngine;

AudioSource::AudioSource()
	: m_clip(nullptr)
	, m_soundInitialized(false)
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
{
	RegisterMessages();
}

AudioSource::~AudioSource()
{
	CleanupSound();

	// 소멸자에서도 레퍼런스 카운트 관리
	if (m_clip)
	{
		m_clip->DecreaseRefCount();
		m_clip = nullptr;
	}
}

void AudioSource::RegisterMessages()
{
	REGISTER_BEHAVIOUR_MESSAGE(OnEnable);
	REGISTER_BEHAVIOUR_MESSAGE(OnDisable);
	REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
}

void AudioSource::OnEnable()
{
	if (AudioManager::Get()->IsInitialized())
	{
		AudioManager::Get()->RegisterAudioSource(this);

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
		AudioManager::Get()->UnRegisterAudioSource(this);

#ifdef _DEBUG
		std::cout << "AudioSource disabled." << std::endl;
#endif
	}
}

void AudioSource::OnDestroy()
{
	CleanupSound();

	// 소멸 시 레퍼런스 카운트 감소
	if (m_clip)
	{
		m_clip->DecreaseRefCount();
		m_clip = nullptr;
	}
}

void AudioSource::SetClip(AudioClip* clip)
{
	if (m_clip == clip) return;

	if (m_isPlaying) Stop();
	CleanupSound();

	// 레퍼런스 카운팅 관리 (SpriteRenderer 패턴과 동일)
	if (clip)
		clip->IncreaseRefCount();
	if (m_clip)
		m_clip->DecreaseRefCount();

	m_clip = clip;

	if (m_clip && GetEnabled())
		InitializeSound();

#ifdef _DEBUG
	std::cout << "AudioClip set for AudioSource." << std::endl;
#endif
}

void AudioSource::InitializeSound()
{
	if (!m_clip || !AudioManager::Get()->IsInitialized())
		return;

	CleanupSound();

	ma_result result = m_clip->CreateSound(&m_sound, AudioManager::Get()->GetEngine());
	
	if (result == MA_SUCCESS)
	{
		ma_sound_seek_to_pcm_frame(&m_sound, 0);
		m_soundInitialized = true;
		ApplySettings();

#ifdef _DEBUG
		std::cout << "AudioSource sound initialized." << std::endl;
#endif
	}
}

void AudioSource::CleanupSound()
{
	if (m_soundInitialized)
	{
		ma_sound_uninit(&m_sound);
		m_soundInitialized = false;
		m_isPlaying = false;
		m_isPaused = false;
	}
}

void AudioSource::ApplySettings()
{
	if (!m_soundInitialized) return;

	ma_sound_set_volume(&m_sound, m_volume);
	ma_sound_set_pitch(&m_sound, m_pitch);
	ma_sound_set_looping(&m_sound, m_loop ? MA_TRUE : MA_FALSE);

	if (m_spatialAudio)
	{
		ma_sound_set_spatialization_enabled(&m_sound, MA_TRUE);
		ma_sound_set_min_distance(&m_sound, m_minDistance);
		ma_sound_set_max_distance(&m_sound, m_maxDistance);
		UpdateTransform();
	}
	else
	{
		ma_sound_set_spatialization_enabled(&m_sound, MA_FALSE);
	}
}

void AudioSource::Play()
{
	if (!m_soundInitialized)
		InitializeSound();

	if (m_soundInitialized)
	{
		ma_result result = ma_sound_start(&m_sound);
		ma_sound_seek_to_pcm_frame(&m_sound,0);
		
		if (result == MA_SUCCESS)
		{
			m_isPlaying = true;
			m_isPaused = false;

#ifdef _DEBUG
			std::cout << "AudioSource playing." << std::endl;
#endif
		}
	}
}

void AudioSource::PlayOneShot()
{
	if (!m_clip || !AudioManager::Get()->IsInitialized())
		return;

	// 임시 사운드 객체 생성하여 한 번만 재생
	ma_sound tempSound;
	ma_result result = m_clip->CreateSound(&tempSound, AudioManager::Get()->GetEngine());

	if (result == MA_SUCCESS)
	{
		// 볼륨과 피치만 적용 (루프나 공간 오디오는 적용하지 않음)
		ma_sound_set_volume(&tempSound, m_volume);
		ma_sound_set_pitch(&tempSound, m_pitch);
		ma_sound_set_looping(&tempSound, MA_FALSE);  // 원샷은 항상 루프 없음

		// 즉시 재생하고 자동으로 정리됨
		ma_sound_start(&tempSound);

#ifdef _DEBUG
		std::cout << "AudioSource one-shot played." << std::endl;
#endif
	}
}

void AudioSource::Stop()
{
	if (m_soundInitialized && m_isPlaying)
	{
		ma_sound_stop(&m_sound);
		m_isPlaying = false;
		m_isPaused = false;
	}
}

void AudioSource::Pause()
{
	if (m_soundInitialized && m_isPlaying && !m_isPaused)
	{
		ma_sound_stop(&m_sound);
		m_isPaused = true;
	}
}

void AudioSource::Resume()
{
	if (m_soundInitialized && m_isPaused)
	{
		ma_sound_start(&m_sound);
		m_isPaused = false;
	}
}

void AudioSource::SetVolume(float volume)
{
	m_volume = std::max(0.0f, std::min(1.0f, volume));
	if (m_soundInitialized)
		ma_sound_set_volume(&m_sound, m_volume);
}

void AudioSource::SetPitch(float pitch)
{
	m_pitch = std::max(0.1f, std::min(3.0f, pitch));
	if (m_soundInitialized)
		ma_sound_set_pitch(&m_sound, m_pitch);
}

void AudioSource::SetLoop(bool loop)
{
	m_loop = loop;
	if (m_soundInitialized)
		ma_sound_set_looping(&m_sound, m_loop ? MA_TRUE : MA_FALSE);
}

void AudioSource::SetSpatialAudio(bool enabled)
{
	m_spatialAudio = enabled;
	if (m_soundInitialized)
	{
		ma_sound_set_spatialization_enabled(&m_sound, enabled ? MA_TRUE : MA_FALSE);
		if (enabled)
			ApplySettings();
	}
}

void AudioSource::SetMinDistance(float distance)
{
	m_minDistance = std::max(0.01f, distance);
	if (m_soundInitialized && m_spatialAudio)
		ma_sound_set_min_distance(&m_sound, m_minDistance);
}

void AudioSource::SetMaxDistance(float distance)
{
	m_maxDistance = std::max(m_minDistance, distance);
	if (m_soundInitialized && m_spatialAudio)
		ma_sound_set_max_distance(&m_sound, m_maxDistance);
}

void AudioSource::UpdateTransform()
{
	if (!m_soundInitialized || !m_spatialAudio || !GetTransform())
		return;

	Vector2 currentPosition = GetTransform()->GetPosition();

	if (currentPosition.x != m_lastPosition.x ||
		currentPosition.y != m_lastPosition.y ||
		m_positionDirty)
	{
		ma_sound_set_position(&m_sound, currentPosition.x, currentPosition.y, 0.0f);
		m_lastPosition = currentPosition;
		m_positionDirty = false;
	}
}