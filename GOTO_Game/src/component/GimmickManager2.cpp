#include "GimmickManager2.h"
#include <CrosshairMove.h>
//#include "EnemySpawner.h"
#include "GameManager2.h"

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
	if (p1gimmick1Timer > 0.0f) {
		p1gimmick1Timer -= TIME_GET_DELTATIME();
		if (p1gimmick1Timer <= 0.0f) {
			p1gimmick1Timer = 0.0f;
			//p1연사모드해제
		}
	}
	if (p2gimmick1Timer > 0.0f) {
		p2gimmick1Timer -= TIME_GET_DELTATIME();
		if (p2gimmick1Timer <= 0.0f) {
			p2gimmick1Timer = 0.0f;
			//p2연사모드해제
		}
	}
	if (p1gimmick2Timer > 0.0f) {
		p1gimmick2Timer -= TIME_GET_DELTATIME();
		if (p1gimmick2Timer <= 0.0f) {
			p1gimmick2Timer = 0.0f;
			//p1샷건모드해제
		}
	}
	if (p2gimmick2Timer > 0.0f) {
		p2gimmick2Timer -= TIME_GET_DELTATIME();
		if (p2gimmick2Timer <= 0.0f) {
			p2gimmick2Timer = 0.0f;
			//p2샷건모드해제
		}
	}
};

void GimmickManager2::GimmickOn(int player, int gimmick) {
	if (GameManager2::instance->setactive) {
		switch (gimmick)
		{
		case 1:
			if (player == 1) {
				//p1연사모드
				p1gimmick1Timer = timelimit;
			}
			else {
				//p2연사모드
				p2gimmick1Timer = timelimit;
			}
			break;
		case 2:
			if (player == 1) {
				//p1연사모드
				p1gimmick2Timer = timelimit;
			}
			else {
				//p2연사모드
				p2gimmick2Timer = timelimit;
			}
			break;
		case 3:
			//적 3마리 생성
			break;
		}
	}
};