#include "GimmickManager.h"
#include <CrosshairMove.h>
#include "EnemySpawner.h"
#include <time.h>
#include "GameManager.h"

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
	auto canvas = GameObject::Find(L"Canvas");
	auto p1gimmick2 = new GameObject;
	p1gimmick2->GetTransform()->SetParent(canvas->GetTransform());
	p1gimmick2Image = p1gimmick2->AddComponent<Image>();
	p1gimmick2Image->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.5f, Screen::GetHeight()});
	p1gimmick2Image->GetRectTransform()->SetAnchoredPosition({
		0, 0});
	auto p2gimmick2 = new GameObject;
	p2gimmick2->GetTransform()->SetParent(canvas->GetTransform());
	p2gimmick2Image = p2gimmick2->AddComponent<Image>();
	p2gimmick2Image->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.5f, Screen::GetHeight()});
	p2gimmick2Image->GetRectTransform()->SetAnchoredPosition({
		Screen::GetWidth() * 0.5f, 0});
	srand(time(NULL));
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
			p2gimmick2Image->SetSprite(nullptr);
		}
	}
	if (p2gimmick2Timer > 0.0f) {
		p2gimmick2Timer -= TIME_GET_DELTATIME();
		if (p2gimmick2Timer <= 0.0f) {
			p2gimmick2Timer = 0.0f;
			p1gimmick2Image->SetSprite(nullptr);
		}
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha9)) {
		GimmickOn(1, 1);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha0)) {
		GimmickOn(2, 1);
	}
	//if (INPUT_GET_KEYDOWN(KeyCode::A)) {
	//	GimmickOn(1, 2);
	//}
	//if (INPUT_GET_KEYDOWN(KeyCode::L)) {
	//	GimmickOn(2, 2);
	//}
};

void GimmickManager::GimmickOn(int player, int gimmick) {
	if (GameManager::instance->setactive) {
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
				int i = rand() % 3 + 1;
				switch (i) {
				case 1:
					p2gimmick2Image->SetSprite(L"../Resources/artResource/UI/Gimmick/Gimmick2_effect.png");
					break;
				case 2:
					p2gimmick2Image->SetSprite(L"../Resources/artResource/UI/Gimmick/Gimmick2_effect2.png");
					break;
				case 3:
					p2gimmick2Image->SetSprite(L"../Resources/artResource/UI/Gimmick/Gimmick2_effect3.png");
					break;
				}
				p1gimmick2Timer = timelimit;
			}
			else {
				int i = rand() % 3 + 1;
				switch (i) {
				case 1:
					p1gimmick2Image->SetSprite(L"../Resources/artResource/UI/Gimmick/Gimmick2_effect.png");
					break;
				case 2:
					p1gimmick2Image->SetSprite(L"../Resources/artResource/UI/Gimmick/Gimmick2_effect2.png");
					break;
				case 3:
					p1gimmick2Image->SetSprite(L"../Resources/artResource/UI/Gimmick/Gimmick2_effect3.png");
					break;
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
	}
};