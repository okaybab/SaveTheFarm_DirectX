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
	if (gimmick5Timer > 0.0f) {
		gimmick5Timer -= TIME_GET_DELTATIME();
		if (gimmick5Timer <= 0.0f) {
			gimmick5Timer = 0.0f;
			//ºù°áÇØÁ¦
		}
	}
	if (p1gimmick6Timer > 0.0f) {
		p1gimmick6Timer -= TIME_GET_DELTATIME();
		if (p1gimmick6Timer <= 0.0f) {
			p1gimmick6Timer = 0.0f;
			//ºò°ÇÇØÁ¦
		}
	}
	if (p1gimmick6Timer > 0.0f) {
		p1gimmick6Timer -= TIME_GET_DELTATIME();
		if (p1gimmick6Timer <= 0.0f) {
			p1gimmick6Timer = 0.0f;
			//ºò°ÇÇØÁ¦
		}
	}
};

void GimmickManager2::GimmickOn(int player, int gimmick) {
	if (GameManager2::instance->setactive) {
		switch (gimmick)
		{
		case 1:
			if (player == 1) {
				//p1¿¬»ç¸ðµå
			}
			else {
				//p2¿¬»ç¸ðµå
			}
			break;
		case 2:
			if (player == 1) {
				//p1¼¦°Ç¸ðµå
			}
			else {
				//p2¼¦°Ç¸ðµå
			}
			break;
		case 3:
			//Àû 3¸¶¸® »ý¼º
			break;
		case 4:
			//ÆøÅº
			break;
		case 5:
			//¾óÀ½ÆøÅº
			gimmick5Timer = 2.0f;
			break;
		case 6:
			if (player == 1) {
				//p1ºò°Ç¸ðµå
				p1gimmick6Timer = 10.0f;
			}
			else {
				//p2ºò°Ç¸ðµå
				p2gimmick6Timer = 10.0f;
			}
		}
	}
};