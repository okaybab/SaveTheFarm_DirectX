#include "GimmickManager2.h"
#include <CrosshairMove.h>
#include "EnemySpawnManager.h"
#include "GameManager2.h"
#include "CrosshairController.h"
#include "SoundManager.h"
#include "BaseSpawnerObject.h"

using namespace GOTOEngine;

GimmickManager2* GimmickManager2::instance = nullptr;

void GimmickManager2::Awake()
{
	if (!instance)
	{
		instance = this;
		iced = Resource::Load<Sprite>(L"../Resources/artResource/UI/Item/Icebomb_Iced.png");
		iced->IncreaseRefCount();
		auto snowobject = new GameObject;
		snowobject->GetTransform()->SetLocalPosition({ 0.0f,Screen::GetHeight() / 2 });
		itemparticle = snowobject->AddComponent<ParticleSystem>();
		itemparticle->SetMaxParticleCount(100);
		itemparticle->SetRenderLayer((1 << 0));
		itemparticle->SetFadeOutTime(2.0f);
		itemparticle->SetParticlesPerSpawn(3);
		itemparticle->SetEmissionTangentLength(Screen::GetWidth());
		itemparticle->SetGravity({ 0.0f,-300.0f });
		itemparticle->SetEmissionShape(EmissionShape::Rectangle);
		itemparticle->SetRenderOrder(2500);
		itemparticle->SetMinScale(0.005f);
		itemparticle->SetMaxScale(0.02f);
		itemparticle->SetSpawnInterval(0.2f);
		itemparticle->SetEmissionDirectionDegrees(270.0f);
		snow = Resource::Load<Sprite>(L"../Resources/artResource/UI/Item/Icebomb_effect.png");
		snow->IncreaseRefCount();
		itemparticle->SetCommonSprite(snow);
	}
	else
	{
		Destroy(GetGameObject());
	}
	auto canvas = GameObject::Find(L"Canvas");
}
void GimmickManager2::OnDestroy() {
	if (instance == this)
		instance = nullptr;
	if (IsValidObject(iced))
		iced->DecreaseRefCount();
	if (IsValidObject(snow))
		snow->DecreaseRefCount();
}

void GimmickManager2::Update() {
	if (gimmick5Timer > 0.0f) {
		gimmick5Timer -= TIME_GET_DELTATIME();
		if (gimmick5Timer <= 0.0f) {
			gimmick5Timer = 0.0f;
			//빙결해제
			itemparticle->Stop();
		}
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha1)) {
		GimmickOn(1, 1);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha2)) {
		GimmickOn(1, 2);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha3)) {
		GimmickOn(1, 3);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha6)) {
		GimmickOn(2, 1);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha7)) {
		GimmickOn(2, 2);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha8)) {
		GimmickOn(2, 3);
	}
};

void GimmickManager2::GimmickOn(int player, int gimmick) {
	if (GameManager2::instance->setactive) {
		auto p1 = GameObject::Find(L"Player1");
		auto p2 = GameObject::Find(L"Player2");
		switch (gimmick)
		{
		case 1:
			if (player == 1) {
				if (Object::IsValidObject(p1)) {
					p1->GetComponent<CrosshairController>()->ChangeType(CrosshairType::MachineGun);
				}
			}
			else {
				if (Object::IsValidObject(p2)) {
					p2->GetComponent<CrosshairController>()->ChangeType(CrosshairType::MachineGun);
				}
			}
			break;
		case 2:
			if (player == 1) {
				if (Object::IsValidObject(p1)) {
					p1->GetComponent<CrosshairController>()->ChangeType(CrosshairType::ShotGun);
				}
			}
			else {
				if (Object::IsValidObject(p2)) {
					p2->GetComponent<CrosshairController>()->ChangeType(CrosshairType::ShotGun);
				}
			}
			break;
		case 3:
			//적 3마리 생성
			EnemySpawnManager::instance->CreateDefenseEnemey(1, E_Defense_Fly_Type::fly, E_Defense_Gimmick_Type::defense_nomal, true);
			EnemySpawnManager::instance->CreateDefenseEnemey(1, E_Defense_Fly_Type::fly, E_Defense_Gimmick_Type::defense_nomal, true);
			EnemySpawnManager::instance->CreateDefenseEnemey(1, E_Defense_Fly_Type::fly, E_Defense_Gimmick_Type::defense_nomal, true);
			break;
		case 4:
			//폭탄
			break;
		case 5:
			//얼음폭탄
			itemparticle->Play();
			gimmick5Timer = 2.0f;
			break;
		case 6:
			SoundManager::instance->PlaySFX("Mushroom");
			if (player == 1) {
				if (Object::IsValidObject(p1)) {
					p1->GetComponent<CrosshairController>()->ChangeType(CrosshairType::BigGun);
				}
			}
			else {
				if (Object::IsValidObject(p2)) {
					p2->GetComponent<CrosshairController>()->ChangeType(CrosshairType::BigGun);
				}
			}
		}
	}
};