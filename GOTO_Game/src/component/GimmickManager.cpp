#include "GimmickManager.h"
#include <CrosshairMove.h>
#include "EnemySpawner.h"

using namespace GOTOEngine;

GimmickManager* GimmickManager::instance = nullptr;

void GimmickManager::Awake()
{
	if (!instance)
	{
		instance = this;
	}
	else
	{
		Destroy(GetGameObject());
	}
	for (int i = 0; i < 5; ++i)
	{
		auto canvas = GameObject::Find(L"Canvas");
		auto p1gimmick2 = new GameObject;
		p1gimmick2->GetTransform()->SetParent(canvas->GetTransform());
		p1gimmick2Image[i] = p1gimmick2->AddComponent<Image>();
		p1gimmick2Image[i]->GetRectTransform()->SetSizeDelta({ 100.0f, 100.0f });

		auto p2gimmick2 = new GameObject;
		p2gimmick2->GetTransform()->SetParent(canvas->GetTransform());
		p2gimmick2Image[i] = p2gimmick2->AddComponent<Image>();
		p2gimmick2Image[i]->GetRectTransform()->SetSizeDelta({ 100.0f, 100.0f });
	}
	p1gimmick2Image[0]->GetRectTransform()->SetAnchoredPosition({
		Screen::GetWidth() * 0.1f, Screen::GetHeight() * 0.2f });
	p1gimmick2Image[1]->GetRectTransform()->SetAnchoredPosition({
		Screen::GetWidth() * 0.4f, Screen::GetHeight() * 0.2f });
	p1gimmick2Image[2]->GetRectTransform()->SetAnchoredPosition({
		Screen::GetWidth() * 0.2f, Screen::GetHeight() * 0.4f });
	p1gimmick2Image[3]->GetRectTransform()->SetAnchoredPosition({
		Screen::GetWidth() * 0.1f, Screen::GetHeight() * 0.7f });
	p1gimmick2Image[4]->GetRectTransform()->SetAnchoredPosition({
		Screen::GetWidth() * 0.4f, Screen::GetHeight() * 0.7f });

	for (int i = 0; i < 5; ++i) {
		p2gimmick2Image[i]->GetRectTransform()->SetAnchoredPosition({
			Screen::GetWidth() * 0.5f + p1gimmick2Image[i]->GetRectTransform()->GetAnchoredPosition().x,
			p1gimmick2Image[i]->GetRectTransform()->GetAnchoredPosition().y });
	}
}
void GimmickManager::OnDestroy() {
	if (instance == this)
		instance = nullptr;
	CrosshairMove::reverseInput1 = false;
	CrosshairMove::reverseInput2 = false;
}

void GimmickManager::Update() {
	if (p1gimmick1Timer > 0.0f) {
		p1gimmick1Timer -= TIME_GET_DELTATIME();
		if (p1gimmick1Timer <= 0.0f) {
			p1gimmick1Timer = 0.0f;
			CrosshairMove::reverseInput2 = false;
		}
	}
	if (p2gimmick1Timer > 0.0f) {
		p2gimmick1Timer -= TIME_GET_DELTATIME();
		if (p2gimmick1Timer <= 0.0f) {
			p2gimmick1Timer = 0.0f;
			CrosshairMove::reverseInput1 = false;
		}
	}
	if (p1gimmick2Timer > 0.0f) {
		p1gimmick2Timer -= TIME_GET_DELTATIME();
		if (p1gimmick2Timer <= 0.0f) {
			p1gimmick2Timer = 0.0f;
			for (int i = 0; i < 5; ++i) {
				p2gimmick2Image[i]->SetSprite(nullptr);
			}
		}
	}
	if (p2gimmick2Timer > 0.0f) {
		p2gimmick2Timer -= TIME_GET_DELTATIME();
		if (p2gimmick2Timer <= 0.0f) {
			p2gimmick2Timer = 0.0f;
			for (int i = 0; i < 5; ++i) {
				p1gimmick2Image[i]->SetSprite(nullptr);
			}
		}
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha9)) {
		GimmickOn(1, 1);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha0)) {
		GimmickOn(2, 1);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::A)) {
		GimmickOn(1, 2);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::L)) {
		GimmickOn(2, 2);
	}
};

void GimmickManager::GimmickOn(int player, int gimmick) {
	switch (gimmick)
	{
	case 1:   //반전술식
		if (player == 1) {
			CrosshairMove::reverseInput2 = true;  
			p1gimmick1Timer = timelimit;
		}
		else {
			CrosshairMove::reverseInput1 = true;
			p2gimmick1Timer = timelimit;
		}
		break;
	case 2:
		if (player == 1) {
			for (int i = 0; i < 5; ++i) {
				p2gimmick2Image[i]->SetSprite(L"../Resources/Mushroom.png");
			}
			p1gimmick2Timer = timelimit;
		}
		else {
			for (int i = 0; i < 5; ++i) {
				p1gimmick2Image[i]->SetSprite(L"../Resources/Mushroom.png");
			}
			p2gimmick2Timer = timelimit;
		}
		break;
	case 3:
		player = player == 1 ? 2 : 1;
		for (int i = 0; i < 3; i++)
		{
			// moveEnemy
			EnemySpawner::instance->CreateEnemy(static_cast<E_EnemyType>(0), player);
		}
		break;
	}
};