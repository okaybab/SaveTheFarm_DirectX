#include "SoundManager.h"

using namespace GOTOEngine;
SoundManager* SoundManager::instance = nullptr;

void SoundManager::Awake() {
	if (!instance)
	{
		instance = this;
	}
	else
	{
		Destroy(GetGameObject());
	}
	//경로 지정 및 리스트에 추가
	sfxClips["Bomb"] = Resource::Load<AudioClip>(L"../Resources/Sound/Sound effect/Bomb_sound.wav");
	sfxClips["Bomb"]->IncreaseRefCount();
	sfxClips["Button"] = Resource::Load<AudioClip>(L"../Resources/Sound/Sound effect/Button_sound.wav");
	sfxClips["Button"]->IncreaseRefCount();
	sfxClips["Getitem"] = Resource::Load<AudioClip>(L"../Resources/Sound/Sound effect/Getitem_sound.wav");
	sfxClips["Getitem"]->IncreaseRefCount();
	sfxClips["GoldenTicket"] = Resource::Load<AudioClip>(L"../Resources/Sound/Sound effect/GoldenTicket_sound.wav");
	sfxClips["GoldenTicket"]->IncreaseRefCount();
	sfxClips["Hit"] = Resource::Load<AudioClip>(L"../Resources/Sound/Sound effect/Hit_sound.wav");
	sfxClips["Hit"]->IncreaseRefCount();
	sfxClips["IceBomb"] = Resource::Load<AudioClip>(L"../Resources/Sound/Sound effect/IceBomb_sound.mp3");
	sfxClips["IceBomb"]->IncreaseRefCount();
	sfxClips["Shot1P"] = Resource::Load<AudioClip>(L"../Resources/Sound/Sound effect/Shot1P_sound.wav");
	sfxClips["Shot1P"]->IncreaseRefCount();
	sfxClips["Shot2P"] = Resource::Load<AudioClip>(L"../Resources/Sound/Sound effect/Shot2P_sound.wav");
	sfxClips["Shot2P"]->IncreaseRefCount();
	sfxSource->SetLoop(false);
	sfxSource->SetVolume(sfxVolume);
	bgmSource->SetLoop(true);
	bgmSource->SetVolume(bgmVolume);
}

void SoundManager::OnDestroy() {
	if (instance == this)
		instance = nullptr;
}

float SoundManager::GetsfxVolume() {
	return sfxVolume;
}

float SoundManager::GetbgmVolume() {
	return bgmVolume;
}

void SoundManager::SetSFXVolume(float volume) {
	sfxVolume = volume;
	sfxSource->SetVolume(sfxVolume);
}
void SoundManager::SetBGMVolume(float volume) {
	bgmVolume = volume;
	bgmSource->SetVolume(bgmVolume);
}

void SoundManager::PlaySFX(const std::string& key) {
	auto it = sfxClips.find(key);
	if (it != sfxClips.end()) {
		sfxSource->SetClip(it->second);
		sfxSource->Play();
	}
};

void SoundManager::PlayBGM(const std::string& key) {
	auto it = bgmClips.find(key);
	if (it != bgmClips.end()) {
		bgmSource->SetClip(it->second);
		bgmSource->Play();
	}
};