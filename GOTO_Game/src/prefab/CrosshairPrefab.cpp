#include "CrosshairPrefab.h"
#include "CrosshairCollide.h"
#include "CrosshairMove.h"
#include "CrosshairFire.h"
#include "ButtonAnimation.h"

#include <GameObject.h>
#include <SpriteRenderer.h>
#include <RadialSpriteRenderer.h>
#include <Collider2D.h>
#include <ParticleSystem.h>
#include <EnhancedCrosshairFire.h>
#include <TextRenderer.h>


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

	return GO;
}

GameObject* GOTOEngine::CrosshairPrefab::CreateEnhancedCrosshair(int id)
{
	auto GO = new GameObject(L"Player");
	GO->AddComponent<CrosshairMove>()->id = id;
	auto crosshairFire = GO->AddComponent<EnhancedCrosshairFire>();
	crosshairFire->id = id;
	auto crosshairCollide = GO->AddComponent<CrosshairCollide>();
	crosshairCollide->id = id;
	GO->layer = (1 << (id + 1)) | (1 << 0); // 레이어 설정: 1 << 1 for Player 1, 1 << 2 for Player 2

	auto SpriteRendererGO = new GameObject(L"Crosshair Sprite");
	auto spriteRenderer = SpriteRendererGO->AddComponent<SpriteRenderer>();
	spriteRenderer->SetSprite(id == 0 ? L"../Resources/Demo/Crosshair.png" : L"../Resources/Demo/Crosshair2.png");
	spriteRenderer->SetRenderOrder(1000 - id); // 커서가 항상 위에 보이도록 설정

	spriteRenderer->SetRenderLayer((1 << (id + 1)));
	SpriteRendererGO->GetTransform()->SetParent(GO->GetTransform(), false);
	
	auto physAnimation = SpriteRendererGO->AddComponent<ButtonAnimation>();
	physAnimation->scaleDamping = 16.0f;
	physAnimation->strength = 220.0f;

	crosshairFire->onFire.Add([physAnimation, crosshairFire](int id) { physAnimation->ApplyTorque(80.0f * Mathf::Max(1.25f,crosshairFire->GetCurrentStrength())); });
	crosshairFire->onFire.Add([physAnimation, crosshairFire](int id) { physAnimation->ApplyScaleForce(0.56f * Mathf::Max(1.25f,crosshairFire->GetCurrentStrength())); });
	crosshairCollide->spriteRenderer = spriteRenderer;

	crosshairFire->onCharge.Add([physAnimation, crosshairFire](int id) { physAnimation->ApplyScaleForce(3.2f); });

	GO->AddComponent<Collider2D>()->SetSize({ 45.0f, 45.0f }); // Collider 크기 조정

	auto GageGO = new GameObject(L"Croshair Gage");
	auto radialRenderer = GageGO->AddComponent<RadialSpriteRenderer>();
	radialRenderer->SetSprite(id == 0 ? L"../Resources/Demo/Crosshair_gage.png" : L"../Resources/Demo/Crosshair_gage2.png");
	radialRenderer->SetRenderOrder(1000 - id - 1); // 커서가 항상 위에 보이도록 설정
	radialRenderer->SetRenderLayer((1 << (id + 1)));
	crosshairFire->gageSprite = radialRenderer;

	GageGO->GetTransform()->SetParent(GO->GetTransform(), false);

	auto particleSys = GO->AddComponent<ParticleSystem>();
	particleSys->SetRenderLayer((1 << (id + 1)));
	particleSys->SetRenderOrder(2);
	particleSys->SetFadeOutTime(0.3f);
	//particleSys->SetFadeMode(ParticleFadeMode::Shrink);
	particleSys->SetSprite(L"../Resources/Demo/drops.png");
	particleSys->SetMinAngularVelocityDegrees(0.0f);
	particleSys->SetMaxAngularVelocityDegrees(35.0f);
	particleSys->SetSpawnInterval(0.65f);
	particleSys->SetParticlesPerSpawn(2);
	particleSys->SetGravity({ 0.0f,-450.0f });

	crosshairFire->dropParticleSys = particleSys;

	auto StrengthTextGO = new GameObject(L"Strength Text");
	auto strengthText = StrengthTextGO->AddComponent<TextRenderer>();
	strengthText->SetFont(L"../Resources/Maplestory Bold.ttf");
	strengthText->size = 18;
	strengthText->SetRenderLayer((1 << (id + 1)));
	strengthText->SetRenderOrder(1000 - id - 1); // 커서가 항상 위에 보이도록 설정

	StrengthTextGO->GetTransform()->SetParent(GO->GetTransform(), false);
	StrengthTextGO->GetTransform()->SetLocalPosition({ 0.0f,-65.0f });

	crosshairFire->strText = strengthText;

	return GO;
}
