#include "GimmickManager2.h"
#include <CrosshairMove.h>
//#include "EnemySpawner.h"
#include "GameManager2.h"
#include "CrosshairController.h"

using namespace GOTOEngine;

GimmickManager2* GimmickManager2::instance = nullptr;

void GimmickManager2::Awake()
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
}
void GimmickManager2::OnDestroy() {
	if (instance == this)
		instance = nullptr;
}

void GimmickManager2::Update() {
	if (gimmick5Timer > 0.0f) {
		gimmick5Timer -= TIME_GET_DELTATIME();
		if (gimmick5Timer <= 0.0f) {
			gimmick5Timer = 0.0f;
			//葫搬秦力
		}
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha1)) {
		GimmickOn(1, 1);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha2)) {
		GimmickOn(1, 2);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha3)) {
		GimmickOn(1, 6);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha6)) {
		GimmickOn(2, 1);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha7)) {
		GimmickOn(2, 2);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha8)) {
		GimmickOn(2, 6);
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
			//利 3付府 积己
			break;
		case 4:
			//气藕
			break;
		case 5:
			//倔澜气藕
			gimmick5Timer = 2.0f;
			break;
		case 6:
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