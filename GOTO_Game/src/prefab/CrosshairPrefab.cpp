#include "CrosshairPrefab.h"
#include "CrosshairCollide.h"
#include "CrosshairMove.h"
#include "CrosshairFire.h"

#include <GameObject.h>
#include <SpriteRenderer.h>
#include <Collider2D.h>
#include <ParticleSystem.h>


using namespace GOTOEngine;

GameObject* CrosshairPrefab::CreateCrosshair(int id)
{
	auto GO = new GameObject(L"Player");
	GO->AddComponent<CrosshairMove>()->id = id;
	GO->AddComponent<CrosshairFire>()->id = id;
	GO->AddComponent<CrosshairCollide>()->id = id;
	GO->layer = (1 << (id + 1)) | (1 << 0); // 레이어 설정: 1 << 1 for Player 1, 1 << 2 for Player 2

	auto spriteRenderer = GO->AddComponent<SpriteRenderer>();
	spriteRenderer->SetSprite(id == 0 ? L"../Resources/Demo/Crosshair.png" : L"../Resources/Demo/Crosshair2.png");
	spriteRenderer->SetRenderOrder(1000 - id); // 커서가 항상 위에 보이도록 설정

    spriteRenderer->SetRenderLayer((1 << (id + 1)));

	GO->AddComponent<Collider2D>()->SetSize({ 45.0f, 45.0f }); // Collider 크기 조정

	auto particleSys = GO->AddComponent<ParticleSystem>();
	particleSys->SetRenderLayer((1 << (id + 1)));
	particleSys->SetParticleCommonRect({ 0,0,24.0f,24.0f });
	particleSys->SetFadeOutTime(0.3f);
	particleSys->SetFadeMode(ParticleFadeMode::Shrink);
	particleSys->Play();

	return GO;
}