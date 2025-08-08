#include "SoundManager.h"

using namespace GOTOEngine;
SoundManager* SoundManager::instance = nullptr;

void SoundManager::Awake() {
	if (!instance)
	{
		instance = this;
		//경로 지정 및 리스트에 추가
		std::vector<std::pair<std::string, std::wstring>> sfxList = {
			{"Bomb", L"../Resources/Sound/Sound effect/Bomb_sound.wav"},
			{"Button", L"../Resources/Sound/Sound effect/Button_sound.wav"},
			{"Getitem", L"../Resources/Sound/Sound effect/Getitem_sound.wav"},
			{"GoldenTicket", L"../Resources/Sound/Sound effect/GoldenTicket_sound.wav"},
			{"Hit", L"../Resources/Sound/Sound effect/Hit_sound.wav"},
			{"IceBomb", L"../Resources/Sound/Sound effect/IceBomb_sound.mp3"},
			{"Shot1P", L"../Resources/Sound/Sound effect/Shot1P_sound.wav"},
			{"Shot2P", L"../Resources/Sound/Sound effect/Shot2P_sound.wav"},
			{"Continuous", L"../Resources/Sound/Sound effect/Continuous_sound.wav"},
			{"Round", L"../Resources/Sound/Sound effect/round_sound.wav"},
			{"Sling", L"../Resources/Sound/Sound effect/Sling_sound.wav"},
			{"Warning", L"../Resources/Sound/Sound effect/Warning_sound.mp3"}
		};

		for (const auto& [key, path] : sfxList) {
			auto clip = Resource::Load<AudioClip>(path.c_str());
			clip->IncreaseRefCount();
			sfxClips[key] = clip;

			auto sfxSourceitem = new GameObject;
			sfxSourceitem->GetTransform()->SetParent(GetGameObject()->GetTransform());
			AudioSource* source;
			source = sfxSourceitem->AddComponent<AudioSource>();
			source->SetLoop(false);
			source->SetVolume(sfxVolume);
			sfxSources[key] = source;
		}

		bgmClips["Battle1"] = Resource::Load<AudioClip>(L"../Resources/Sound/BGM/Ingame_BGM1.mp3");
		bgmClips["Battle1"]->IncreaseRefCount();
		bgmClips["Battle2"] = Resource::Load<AudioClip>(L"../Resources/Sound/BGM/Ingame_BGM2.mp3");
		bgmClips["Battle2"]->IncreaseRefCount();
		auto bgmSourceitem = new GameObject;
		bgmSourceitem->GetTransform()->SetParent(GetGameObject()->GetTransform());
		bgmSource = bgmSourceitem->AddComponent<AudioSource>();
		bgmSource->SetLoop(true);
		bgmSource->SetVolume(bgmVolume);

		DontDestroyOnLoad(GetGameObject());
	}
	else
	{
		Destroy(GetGameObject());
	}
	
}

void SoundManager::OnDestroy() {
	if (instance == this)
		instance = nullptr;
	for (auto& sfxClip : sfxClips) {
		sfxClip.second->DecreaseRefCount();
	}
	for (auto& bgmClip : bgmClips) {
		bgmClip.second->DecreaseRefCount();
	}
}

float SoundManager::GetsfxVolume() {
	return sfxVolume;
}

float SoundManager::GetbgmVolume() {
	return bgmVolume;
}

void SoundManager::SetSFXVolume(float volume) {
	sfxVolume = volume;
	for (auto& [key, source] : sfxSources) {
		source->SetVolume(sfxVolume);
	}
}
void SoundManager::SetBGMVolume(float volume) {
	bgmVolume = volume;
	bgmSource->SetVolume(bgmVolume);
}

void SoundManager::PlaySFX(const std::string& key) {
	auto clipIt = sfxClips.find(key);
	auto sourceIt = sfxSources.find(key);
	if (clipIt != sfxClips.end() && sourceIt != sfxSources.end()) {
		sourceIt->second->Stop();
		sourceIt->second->SetClip(clipIt->second);
		sourceIt->second->Play();
	}
};

void SoundManager::PlayBGM(const std::string& key) {
	auto it = bgmClips.find(key);
	if (it != bgmClips.end()) {
		bgmSource->Stop();
		bgmSource->SetClip(it->second);
		bgmSource->Play();
	}
};

void SoundManager::StopBGM() {
	bgmSource->Stop();
}