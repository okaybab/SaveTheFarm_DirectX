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
#include <Animator.h>


using namespace GOTOEngine;

GameObject* CrosshairPrefab::CreateCrosshair(int id)
{
	auto GO = new GameObject(L"Player");
	auto crosshairMove = GO->AddComponent<CrosshairMove>();
	crosshairMove->id = id;

	auto GimmickAnimatorGO = new GameObject(L"Gimmick Animation");
	auto gimmickSprite = GimmickAnimatorGO->AddComponent<SpriteRenderer>();
	gimmickSprite->SetRenderLayer((1 << (id + 1)));
	gimmickSprite->SetRenderOrder(1000 + id + 2);
	GimmickAnimatorGO->AddComponent<Animator>()->SetAnimatorController(L"../Resources/Animation/controller/Gimmick1_effect_AnimController.json");
	crosshairMove->gimmickAnimator = GimmickAnimatorGO->GetComponent<Animator>();
	crosshairMove->gimmickAnimator->SetEnabled(false);
	crosshairMove->gimmickAnimSprite = gimmickSprite;
	crosshairMove->gimmickAnimSprite->SetEnabled(false);
	GimmickAnimatorGO->GetTransform()->SetLocalScale({ 0.18f,0.18f });
	GimmickAnimatorGO->GetTransform()->SetLocalPosition({ 0.0f,26.0f });

	GimmickAnimatorGO->GetTransform()->SetParent(GO->GetTransform());

	auto crosshairFire = GO->AddComponent<CrosshairFire>();
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

	crosshairFire->onFire.Add([physAnimation, crosshairFire](int id) { physAnimation->ApplyTorque(400.0f); });
	//crosshairFire->onFire.Add([physAnimation, crosshairFire](int id) { physAnimation->ApplyScaleForce(3.36f); });
	crosshairCollide->spriteRenderer = spriteRenderer;

	GO->AddComponent<Collider2D>()->SetSize({ 45.0f, 45.0f }); // Collider 크기 조정

	auto GageGO = new GameObject(L"Croshair Gage");
	auto radialRenderer = GageGO->AddComponent<RadialSpriteRenderer>();
	radialRenderer->SetSprite(id == 0 ? L"../Resources/Demo/Crosshair_gage.png" : L"../Resources/Demo/Crosshair_gage2.png");
	radialRenderer->SetRenderOrder(1000 - id - 1); // 커서가 항상 위에 보이도록 설정
	radialRenderer->SetRenderLayer((1 << (id + 1)));
	crosshairFire->gageSprite = radialRenderer;

	GageGO->GetTransform()->SetParent(GO->GetTransform(), false);

	crosshairFire->onCharge.Add([physAnimation, crosshairFire](int id) { physAnimation->ApplyScaleForce(3.2f); });

	auto GoldFXGO = new GameObject((id == 0 ? L"p1 Gold FX" : L"p2 Gold FX"));
	auto GoldFX = GoldFXGO->AddComponent<ParticleSystem>();
	GoldFX->SetMaxParticleCount(55);
	GoldFX->SetSprite(L"../Resources/artResource/UI/Item/Goldenticket_effect.png");
	GoldFXGO->GetTransform()->SetParent(GO->GetTransform(), false);
	GoldFX->SetRenderLayer(1 << (id+1));
	GoldFX->SetMinScale(0.05f);
	GoldFX->SetMaxScale(0.1f);
	GoldFX->SetRenderOrder(2000);

	

	return GO;
}

GameObject* GOTOEngine::CrosshairPrefab::CreateEnhancedCrosshair(int id)
{
	auto GO = new GameObject(L"Player");
	auto crosshairMove = GO->AddComponent<CrosshairMove>();
	crosshairMove->id = id;

	auto GimmickAnimatorGO = new GameObject(L"Gimmick Animation");
	auto gimmickSprite = GimmickAnimatorGO->AddComponent<SpriteRenderer>();
	gimmickSprite->SetRenderLayer((1 << (id + 1)));
	gimmickSprite->SetRenderOrder(1000 + id + 2);
	GimmickAnimatorGO->AddComponent<Animator>()->SetAnimatorController(L"../Resources/Animation/controller/Gimmick1_effect_AnimController.json");
	crosshairMove->gimmickAnimator = GimmickAnimatorGO->GetComponent<Animator>();
	crosshairMove->gimmickAnimator->SetEnabled(false);
	crosshairMove->gimmickAnimSprite = gimmickSprite;
	crosshairMove->gimmickAnimSprite->SetEnabled(false);
	GimmickAnimatorGO->GetTransform()->SetLocalScale({ 0.18f,0.18f });
	GimmickAnimatorGO->GetTransform()->SetLocalPosition({ 0.0f,26.0f });

	GimmickAnimatorGO->GetTransform()->SetParent(GO->GetTransform());

	auto crosshairFire = GO->AddComponent<EnhancedCrosshairFire>();
	crosshairFire->id = id;
	auto crosshairCollide = GO->AddComponent<CrosshairCollide>();
	crosshairCollide->id = id;
	GO->layer = (1 << (id + 1)) | (1 << 0); // 레이어 설정: 1 << 1 - Player 1, 1 << 2 - Player 2 // 1 << 0 - 공통

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

	auto GoldFXGO = new GameObject((id == 0 ? L"p1 Gold FX" : L"p2 Gold FX"));
	auto GoldFX = GoldFXGO->AddComponent<ParticleSystem>();
	GoldFX->SetMaxParticleCount(55);
	GoldFX->SetSprite(L"../Resources/artResource/UI/Goldenticket_effect.png");
	GoldFXGO->GetTransform()->SetParent(GO->GetTransform(), false);

	return GO;
}
