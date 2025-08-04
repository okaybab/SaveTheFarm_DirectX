#pragma once
#include <ScriptBehaviour.h>
#include <TimeManager.h>
#include <AudioManager.h>
#include <AudioClip.h>
#include <AudioSource.h>
#include "RectTransform.h"

namespace GOTOEngine
{
	class SoundManager : public ScriptBehaviour
	{
	public:
    SoundManager()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
    }
		static SoundManager* instance;
		void Awake();
		void OnDestroy();
		void SetSFXVolume(float volume);
		void SetBGMVolume(float volume);
		float GetsfxVolume();
		float GetbgmVolume();
		void PlaySFX(const std::string& key);
		void PlayBGM(const std::string& key);
	private:
		float sfxVolume = 1.0f;
		float bgmVolume = 1.0f;
		std::unordered_map<std::string, AudioClip*> sfxClips;
		std::unordered_map<std::string, AudioClip*> bgmClips;
		std::unordered_map<std::string, AudioSource*> sfxSources;
		AudioSource* bgmSource;
	};
}

