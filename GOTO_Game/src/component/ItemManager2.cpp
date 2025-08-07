#include "ItemManager2.h"
#include "GameManager2.h"
#include "CameraShaker.h"
#include "BaseEnemyObject.h"
//#include "EnemySpawner.h"
#include "SoundManager.h"
#include <AnimationCurve.h>

using namespace GOTOEngine;

ItemManager2* ItemManager2::instance = nullptr;

void ItemManager2::Awake() {

	if (!instance)
	{
		instance = this;
		auto canvas = GameObject::Find(L"Canvas");
		//// 이미지 간격 및 기본 위치 설정
		float spacing = 100.0f;
		Vector2 p1StartPos = { Screen::GetWidth() * 0.05f, Screen::GetHeight() * 0.06f };
		Vector2 p2StartPos = { Screen::GetWidth() * 0.55f, Screen::GetHeight() * 0.06f };
		auto p1itembar = new GameObject;
		p1itembar->GetTransform()->SetParent(canvas->GetTransform());
		itembar1 = p1itembar->AddComponent<Image>();
		itembar1->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.02f, Screen::GetHeight() * 0.05f });
		itembar1->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.45f, Screen::GetHeight() * 0.1f });
		itembar1->SetSprite(L"../Resources/artResource/UI/Ingame/아이템 창 UI.png");
		auto p2itembar = new GameObject;
		p2itembar->GetTransform()->SetParent(canvas->GetTransform());
		itembar2 = p2itembar->AddComponent<Image>();
		itembar2->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.52f, Screen::GetHeight() * 0.05f });
		itembar2->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.45f, Screen::GetHeight() * 0.1f });
		itembar2->SetSprite(L"../Resources/artResource/UI/Ingame/아이템 창 UI.png");

		bombItemSprite = Resource::Load<Sprite>(L"../Resources/artResource/UI/Item/Bomb_item.png");
		bombItemSprite->IncreaseRefCount();
		icebombItemSprite = Resource::Load<Sprite>(L"../Resources/artResource/UI/Item/Icebomb_item.png");
		icebombItemSprite->IncreaseRefCount();
		mushroomItemSprite = Resource::Load<Sprite>(L"../Resources/artResource/UI/Item/Golden_ticket.png");
		mushroomItemSprite->IncreaseRefCount();
		iced = Resource::Load<Sprite>(L"../Resources/artResource/UI/Item/Icebomb_Iced.png");
		iced->IncreaseRefCount();
		bombanimator = Resource::Load<AnimatorController>(L"../Resources/Animation/controller/BombAnimator_AnimController.json");
		bombanimator->IncreaseRefCount();

		for (int i = 0; i < 7; ++i)
		{
			// 플레이어 1 아이템 오브젝트 생성 및 설정
			auto p1item = new GameObject;
			p1item->GetTransform()->SetParent(canvas->GetTransform());
			p1itemImage[i] = p1item->AddComponent<Image>();
			p1itemImage[i]->GetRectTransform()->SetAnchoredPosition({
				p1StartPos.x + spacing * i, p1StartPos.y });
			p1itemImage[i]->GetRectTransform()->SetSizeDelta({ 90.0f, 90.0f });

			// 플레이어 2 아이템 오브젝트 생성 및 설정
			auto p2item = new GameObject;
			p2item->GetTransform()->SetParent(canvas->GetTransform());
			p2itemImage[i] = p2item->AddComponent<Image>();
			p2itemImage[i]->GetRectTransform()->SetAnchoredPosition({
				p2StartPos.x + spacing * i, p2StartPos.y });
			p2itemImage[i]->GetRectTransform()->SetSizeDelta({ 90.0f, 90.0f });
		}
		itemanimation = new AnimationCurve({ R"({
    "keyframes": [
        {
            "time": 0.0,
            "value": 0.0035049389948139213,
            "in_tangent": 0,
            "out_tangent": 1,
            "tangent_mode": 1
        },
        {
            "time": 0.25,
            "value": 0.07,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.5,
            "value": -0.07,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 0
        },
        {
            "time": 0.75,
            "value": 0.07,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.0,
            "value": 0.0,
            "in_tangent": 1.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        }
    ]
})" });
	}
	else
	{
		Destroy(GetGameObject());
	}

}

void ItemManager2::OnDestroy() {
	if (instance == this)
		instance = nullptr;
	delete(itemanimation);

	if (IsValidObject(bombItemSprite))
		bombItemSprite->DecreaseRefCount();
	if (IsValidObject(icebombItemSprite))
		icebombItemSprite->DecreaseRefCount();
	if (IsValidObject(mushroomItemSprite))
		mushroomItemSprite->DecreaseRefCount();
	if (IsValidObject(iced))
		iced->DecreaseRefCount();
	if (IsValidObject(bombanimator))
		bombanimator->DecreaseRefCount();
}

void ItemManager2::Update() {

	if (INPUT_GET_KEYDOWN(KeyCode::Alpha1) || INPUT_GET_GAMEPAD_BUTTONDOWN(0, GamepadButton::ButtonL1)) {
		if (!p1Items.empty()) {
			ItemType item = p1Items[0];
			UseItem(1, item);
			p1Items.erase(p1Items.begin());
		}
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha2) || INPUT_GET_GAMEPAD_BUTTONDOWN(1, GamepadButton::ButtonL1)) {
		if (!p2Items.empty()) {
			ItemType item = p2Items[0];
			UseItem(2, item);
			p2Items.erase(p2Items.begin());
		}
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha3)) {
		AddItem(1, ItemType::Bomb);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha4)) {
		AddItem(1, ItemType::Icebomb);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha5)) {
		AddItem(1, ItemType::Mushroom);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha6)) {
		AddItem(2, ItemType::Bomb);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha7)) {
		AddItem(2, ItemType::Icebomb);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha8)) {
		AddItem(2, ItemType::Mushroom);
	}
	if (p1MushroomTimer > 0.0f) {
		p1MushroomTimer -= TIME_GET_DELTATIME();
		if (p1MushroomTimer <= 0.0f) {
			p1MushroomTimer = 0.0f;
			//사이즈복구
		}
	}

	if (p2MushroomTimer > 0.0f) {
		p2MushroomTimer -= TIME_GET_DELTATIME();
		if (p2MushroomTimer <= 0.0f) {
			p2MushroomTimer = 0.0f;
			//사이즈복구
		}
	}
	if (IceTimer > 0.0f) {
		IceTimer -= TIME_GET_DELTATIME();
		if (IceTimer <= 0.0f) {
			IceTimer = 0.0f;
			//빙결해제
		}
	}
	for (int i = 0; i < 7; ++i) {
		if (i < p1Items.size()) {
			ItemType item = p1Items[i];
			switch (item) {
			case ItemType::Bomb:
				p1itemImage[i]->SetSprite(bombItemSprite);
				break;
			case ItemType::Icebomb:
				p1itemImage[i]->SetSprite(icebombItemSprite);
				break;
			case ItemType::Mushroom:
				p1itemImage[i]->SetSprite(mushroomItemSprite);
				break;
			}
		}
		else {
			p1itemImage[i]->SetSprite(nullptr);
		}
	}
	for (int i = 0; i < 7; ++i) {
		if (i < p2Items.size()) {
			ItemType item = p2Items[i];
			switch (item) {
			case ItemType::Bomb:
				p2itemImage[i]->SetSprite(bombItemSprite);
				break;
			case ItemType::Icebomb:
				p2itemImage[i]->SetSprite(icebombItemSprite);
				break;
			case ItemType::Mushroom:
				p2itemImage[i]->SetSprite(mushroomItemSprite);
				break;
			}
		}
		else {
			p2itemImage[i]->SetSprite(nullptr);
		}
	}
	if (p1Items.empty()) {
		p1AniTimer = 10.0f;
		for (int i = 0; i < 7; ++i) {
			p1itemImage[i]->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.05f + 100.0f * i, Screen::GetHeight() * 0.06f });
		}
	}
	if (!p1Items.empty() && p1AniTimer > 0.0f) {
		p1AniTimer -= TIME_GET_DELTATIME();
		if (p1AniTimer <= 0.0f) {
			p1AniTimer = 0.0f;
		}
	}
	if (p1AniTimer == 0.0f) {
		for (int i = 0; i < 7; ++i) {
			p1AniTime[i] += TIME_GET_DELTATIME();
			if (p1AniTime[i] > 1.0f) {
				p1AniTime[i] = 0.0f;
			}
			float animValue = itemanimation->Evaluate(p1AniTime[i]);
			p1itemImage[i]->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.05f + 100.0f * i + animValue * 100.0f, Screen::GetHeight() * 0.06f });
		}
	}
	if (p2Items.empty()) {
		p2AniTimer = 10.0f;
		for (int i = 0; i < 7; ++i) {
			p2itemImage[i]->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.55f + 100.0f * i, Screen::GetHeight() * 0.06f });
		}
	}
	if (!p2Items.empty() && p2AniTimer > 0.0f) {
		p2AniTimer -= TIME_GET_DELTATIME();
		if (p2AniTimer <= 0.0f) {
			p2AniTimer = 0.0f;
		}
	}
	if (p2AniTimer == 0.0f) {
		for (int i = 0; i < 7; ++i) {
			p2AniTime[i] += TIME_GET_DELTATIME();
			if (p2AniTime[i] > 1.0f) {
				p2AniTime[i] = 0.0f;
			}
			float animValue = itemanimation->Evaluate(p2AniTime[i]);
			p2itemImage[i]->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.55f + 100.0f * i + animValue * 100.0f, Screen::GetHeight() * 0.06f });
		}
	}
}

void ItemManager2::UseItem(int player, ItemType item)
{
	if (GameManager2::instance->setactive) {
		switch (item) {
		case ItemType::Bomb:
			//폭발
			SoundManager::instance->PlaySFX("Bomb");
			break;
		case ItemType::Icebomb:
			//빙결
			SoundManager::instance->PlaySFX("IceBomb");
			IceTimer = 5.0f;
			break;
		case ItemType::Mushroom:
			if (player == 1) {
				//p1버섯
				p1MushroomTimer = 10.0f;
			}
			else {
				//p2버섯
				p2MushroomTimer = 10.0f;
			}
			break;
		}
	}
}

void ItemManager2::AddItem(int player, ItemType item) {
	if (GameManager2::instance->setactive) {
		if (player == 1) {
			if (p1Items.size() < 7) {
				SoundManager::instance->PlaySFX("Getitem");
				p1Items.push_back(item);
			}
		}
		else {
			if (p2Items.size() < 7) {
				SoundManager::instance->PlaySFX("Getitem");
				p2Items.push_back(item);
			}
		}
	}
}