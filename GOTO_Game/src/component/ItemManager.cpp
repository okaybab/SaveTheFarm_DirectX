#include "ItemManager.h"
#include "GameManager.h"
#include "CameraShaker.h"
#include "BaseEnemyObject.h"
#include "EnemySpawner.h"
#include "SoundManager.h"

using namespace GOTOEngine;

ItemManager* ItemManager::instance = nullptr;

void ItemManager::Awake() {

	if (!instance)
	{
		instance = this;
	}
	else
	{
		Destroy(GetGameObject());
	}
	auto canvas = GameObject::Find(L"Canvas");
	// 이미지 간격 및 기본 위치 설정
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

	for (int i = 0; i < 7; ++i)
	{
		// 플레이어 1 아이템 오브젝트 생성 및 설정
		auto p1item = new GameObject;
		p1item->GetTransform()->SetParent(canvas->GetTransform());
		p1itemImage[i] = p1item->AddComponent<Image>();
		p1itemImage[i]->GetRectTransform()->SetAnchoredPosition({
			p1StartPos.x + spacing * i, p1StartPos.y });
		p1itemImage[i]->GetRectTransform()->SetSizeDelta({ 100.0f, 100.0f });

		// 플레이어 2 아이템 오브젝트 생성 및 설정
		auto p2item = new GameObject;
		p2item->GetTransform()->SetParent(canvas->GetTransform());
		p2itemImage[i] = p2item->AddComponent<Image>();
		p2itemImage[i]->GetRectTransform()->SetAnchoredPosition({
			p2StartPos.x + spacing * i, p2StartPos.y });
		p2itemImage[i]->GetRectTransform()->SetSizeDelta({ 100.0f, 100.0f });
	}
}

void ItemManager::OnDestroy() {
	if (instance == this)
		instance = nullptr;
}

void ItemManager::Update(){

	if (INPUT_GET_KEYDOWN(KeyCode::Alpha1)||INPUT_GET_GAMEPAD_BUTTONDOWN(0,GamepadButton::ButtonL1)) {
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
		AddItem(1, ItemType::Ticket);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha6)) {
		AddItem(2, ItemType::Bomb);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha7)) {
		AddItem(2, ItemType::Icebomb);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha8)) {
		AddItem(2, ItemType::Ticket);
	}
	if (p1TicketTimer > 0.0f) {
		p1TicketTimer -= TIME_GET_DELTATIME();
		if (p1TicketTimer <= 0.0f) {
			p1TicketTimer = 0.0f;
			GameManager::instance->P1Bonus = 1;
		}
	}

	if (p2TicketTimer > 0.0f) {
		p2TicketTimer -= TIME_GET_DELTATIME();
		if (p2TicketTimer <= 0.0f) {
			p2TicketTimer = 0.0f;
			GameManager::instance->P2Bonus = 1;
		}
	}
	if (p1IceTimer > 0.0f) {
		p1IceTimer -= TIME_GET_DELTATIME();
		if (p1IceTimer <= 0.0f) {
			p1IceTimer = 0.0f;
			auto& enemies = *EnemySpawner::instance->Getp1Enemy();
			for (auto* enemy : enemies)
			{
				enemy->GetComponent<BaseEnemyObject>()->SetEnemyFrozen(false);
			}
		}
	}

	if (p2IceTimer > 0.0f) {
		p2IceTimer -= TIME_GET_DELTATIME();
		if (p2IceTimer <= 0.0f) {
			p2IceTimer = 0.0f;
			auto& enemies = *EnemySpawner::instance->Getp2Enemy();
			for (auto* enemy : enemies)
			{
				enemy->GetComponent<BaseEnemyObject>()->SetEnemyFrozen(false);
			}
		
		}
	}
	for (int i = 0; i < 7; ++i) {
		if (i < p1Items.size()) {
			ItemType item = p1Items[i];
			switch (item) {
			case ItemType::Bomb:
				p1itemImage[i]->SetSprite(L"../Resources/artResource/UI/Item/Bomb_item.png");
				break;
			case ItemType::Icebomb:
				p1itemImage[i]->SetSprite(L"../Resources/artResource/UI/Item/Icebomb_item.png");
				break;
			case ItemType::Ticket:
				p1itemImage[i]->SetSprite(L"../Resources/artResource/UI/Item/Golden_ticket.png");
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
				p2itemImage[i]->SetSprite(L"../Resources/artResource/UI/Item/Bomb_item.png");
				break;
			case ItemType::Icebomb:
				p2itemImage[i]->SetSprite(L"../Resources/artResource/UI/Item/Icebomb_item.png");
				break;
			case ItemType::Ticket:
				p2itemImage[i]->SetSprite(L"../Resources/artResource/UI/Item/Golden_ticket.png");
				break;
			}
		}
		else {
			p2itemImage[i]->SetSprite(nullptr);
		}
	}
}

void ItemManager::UseItem(int player, ItemType item)
{
	switch (item) {
	case ItemType::Bomb:
		if (player == 1) {
			//P1의 동물리스트 내부 객체 카운트
			//P1의 동물리스트 내부 객체 전부 삭제
			auto& enemies = *EnemySpawner::instance->Getp1Enemy();

			p1count = enemies.size();
			for (auto enemy : enemies) {
				auto bombeffect = new GameObject;
				bombeffect->GetTransform()->SetPosition(enemy->GetTransform()->GetPosition());
				bombeffect->GetTransform()->SetLocalScale({ 0.5f, 0.5f });
				bombeffect->AddComponent<SpriteRenderer>()->SetRenderLayer(1<<1);
				bombeffect->AddComponent<Animator>()->SetAnimatorController(Resource::Load<AnimatorController>(L"../Resources/Animation/controller/BombAnimator_AnimController.json"));
				Destroy(bombeffect, 0.583f);
			}
			EnemySpawner::instance->Setp1EnemyAllDestroy();

			auto p1cam = GameObject::Find(L"p1Cam");
			if (IsValidObject(p1cam))
				p1cam->GetComponent<CameraShaker>()->ShakeCamera(20, 30, 4);

			if (p1count >= 1 && p1count <= 3) {
				GameManager::instance->P1Score += 3 * GameManager::instance->P1Bonus;
			}
			else if (p1count >= 4 && p1count <= 6) {
				GameManager::instance->P1Score += 5 * GameManager::instance->P1Bonus;
			}
			else if (p1count >= 7) {
				GameManager::instance->P1Score += 7 * GameManager::instance->P1Bonus;
			}
			SoundManager::instance->PlaySFX("Bomb");
		}
		else {
			//P2의 동물리스트 내부 객체 카운트
			//P2의 동물리스트 내부 객체 전부 삭제
			auto& enemies = *EnemySpawner::instance->Getp2Enemy();

			p2count = enemies.size();
			for (auto enemy : enemies) {
				auto bombeffect = new GameObject;
				bombeffect->GetTransform()->SetPosition(enemy->GetTransform()->GetPosition());
				bombeffect->GetTransform()->SetLocalScale({ 0.5f, 0.5f });
				bombeffect->AddComponent<SpriteRenderer>()->SetRenderLayer(1 << 2);
				bombeffect->AddComponent<Animator>()->SetAnimatorController(Resource::Load<AnimatorController>(L"../Resources/Animation/controller/BombAnimator_AnimController.json"));
				Destroy(bombeffect, 0.583f);
			}
			EnemySpawner::instance->Setp2EnemyAllDestroy();

			auto p2cam = GameObject::Find(L"p2Cam");
			if (IsValidObject(p2cam))
				p2cam->GetComponent<CameraShaker>()->ShakeCamera(20, 30, 4);

			if (p2count >= 1 && p2count <= 3) {
				GameManager::instance->P2Score += 3 * GameManager::instance->P2Bonus;
			}
			else if (p2count >= 4 && p2count <= 6) {
				GameManager::instance->P2Score += 5 * GameManager::instance->P2Bonus;
			}
			else if (p2count >= 7) {
				GameManager::instance->P2Score += 7 * GameManager::instance->P2Bonus;
			}
			SoundManager::instance->PlaySFX("Bomb");
		}
		break;
	case ItemType::Icebomb:
		if (player == 1) {
			//P1의 동물리스트 내부 객체 전부 이동 정지
			//P1의 동물리스트 내부 객체 전부 디스폰 시간 정지
			auto& enemies = *EnemySpawner::instance->Getp1Enemy();

			for (auto* enemy : enemies)
			{
				auto iceeffect = new GameObject;
				iceeffect->GetTransform()->SetPosition(enemy->GetTransform()->GetPosition());
				iceeffect->GetTransform()->SetLocalScale({ 0.4f, 0.4f });
				iceeffect->AddComponent<SpriteRenderer>()->SetRenderLayer(1 << 1);
				iceeffect->GetComponent<SpriteRenderer>()->SetSprite(L"../Resources/artResource/UI/Item/Icebomb_Iced.png");
				iceeffect->GetTransform()->SetParent(enemy->GetTransform());
				Destroy(iceeffect, 5.0f);
				enemy->GetComponent<BaseEnemyObject>()->SetEnemyFrozen(true);
			}
			SoundManager::instance->PlaySFX("IceBomb");
			p1IceTimer = timelimit;
		}
		else {
			//P2의 동물리스트 내부 객체 전부 이동 정지
			//P2의 동물리스트 내부 객체 전부 디스폰 시간 정지

			auto& enemies = *EnemySpawner::instance->Getp2Enemy();

			for (auto* enemy : enemies)
			{
				auto iceeffect = new GameObject;
				iceeffect->GetTransform()->SetPosition(enemy->GetTransform()->GetPosition());
				iceeffect->GetTransform()->SetLocalScale({ 0.4f, 0.4f });
				iceeffect->AddComponent<SpriteRenderer>()->SetRenderLayer(1 << 2);
				iceeffect->GetComponent<SpriteRenderer>()->SetSprite(L"../Resources/artResource/UI/Item/Icebomb_Iced.png");
				iceeffect->GetTransform()->SetParent(enemy->GetTransform());
				Destroy(iceeffect, 5.0f);
				enemy->GetComponent<BaseEnemyObject>()->SetEnemyFrozen(true);
			}
			SoundManager::instance->PlaySFX("IceBomb");
			p2IceTimer = timelimit;
		}
		break;
	case ItemType::Ticket:
		if (player == 1) {
			GameManager::instance->P1Bonus = 2;
			SoundManager::instance->PlaySFX("GoldenTicket");
			p1TicketTimer = timelimit;
		}
		else {
			GameManager::instance->P2Bonus = 2;
			SoundManager::instance->PlaySFX("GoldenTicket");
			p2TicketTimer = timelimit;
		}
		break;
	}
}

void ItemManager::AddItem(int player, ItemType item) {
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