#include "CrosshairPrefab.h"
#include "CrosshairCollide.h"
#include "CrosshairMove.h"
#include "CrosshairFire.h"
#include "CrosshairController.h"
#include "ButtonAnimation.h"

#include <RectRenderer.h>
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
	gimmickSprite->SetRenderLayer((1 << (id + 1)) | (1 << 3));
	gimmickSprite->SetRenderOrder(1000 + id + 2);
	GimmickAnimatorGO->AddComponent<Animator>()->SetAnimatorController(L"../Resources/Animation/controller/Gimmick1_effect_AnimController.json");
	crosshairMove->gimmickAnimator = GimmickAnimatorGO->GetComponent<Animator>();
	crosshairMove->gimmickAnimator->SetEnabled(false);
	crosshairMove->gimmickAnimSprite = gimmickSprite;
	crosshairMove->gimmickAnimSprite->SetEnabled(false);
	GimmickAnimatorGO->GetTransform()->SetLocalScale({ 0.24f,0.24f });
	GimmickAnimatorGO->GetTransform()->SetLocalPosition({ 0.0f,26.0f });

	GimmickAnimatorGO->GetTransform()->SetParent(GO->GetTransform());

	auto crosshairFire = GO->AddComponent<CrosshairFire>();
	crosshairFire->id = id;
	auto crosshairCollide = GO->AddComponent<CrosshairCollide>();
	crosshairCollide->id = id;
	GO->layer = (1 << (id + 1)); // 레이어 설정: 1 << 1 for Player 1, 1 << 2 for Player 2

	auto SpriteRendererGO = new GameObject(L"Crosshair Sprite");
	auto spriteRenderer = SpriteRendererGO->AddComponent<SpriteRenderer>();
	spriteRenderer->SetSprite(id == 0 ? L"../Resources/Demo/Crosshair.png" : L"../Resources/Demo/Crosshair2.png");
	spriteRenderer->SetRenderOrder(1000 - (id * 2)); // 커서가 항상 위에 보이도록 설정

    spriteRenderer->SetRenderLayer((1 << (id + 1)) | (1 << 3));
	SpriteRendererGO->GetTransform()->SetParent(GO->GetTransform(), false);

	auto physAnimation = SpriteRendererGO->AddComponent<ButtonAnimation>();
	physAnimation->scaleDamping = 16.0f;
	physAnimation->strength = 220.0f;

	crosshairFire->physAnimation = physAnimation;

	crosshairCollide->spriteRenderer = spriteRenderer;

	auto col2d = GO->AddComponent<Collider2D>(); // Collider 크기 조정
	col2d->SetSize({ 45.0f, 45.0f });
	col2d->SetIsTrigger(true);

	auto GageGO = new GameObject(L"Croshair Gage");
	auto radialRenderer = GageGO->AddComponent<RadialSpriteRenderer>();
	radialRenderer->SetSprite(id == 0 ? L"../Resources/Demo/Crosshair_gage.png" : L"../Resources/Demo/Crosshair_gage2.png");
	radialRenderer->SetRenderOrder(1000 - (id * 2) - 1); // 커서가 항상 위에 보이도록 설정
	radialRenderer->SetRenderLayer((1 << (id + 1)) | (1 << 3));
	crosshairFire->gageSprite = radialRenderer;

	GageGO->GetTransform()->SetParent(GO->GetTransform(), false);

	auto particleSys = GO->AddComponent<ParticleSystem>();
	particleSys->SetRenderLayer((1 << (id + 1)) | (1 << 3));
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
	strengthText->SetRenderLayer((1 << (id + 1)) | (1 << 3));
	strengthText->SetRenderOrder(1000 - (id * 2) - 1); // 커서가 항상 위에 보이도록 설정

	StrengthTextGO->GetTransform()->SetParent(GO->GetTransform(), false);
	StrengthTextGO->GetTransform()->SetLocalPosition({ 0.0f,-65.0f });

	crosshairFire->strText = strengthText;

	auto GoldFXGO = new GameObject((id == 0 ? L"p1 Gold FX" : L"p2 Gold FX"));
	auto GoldFX = GoldFXGO->AddComponent<ParticleSystem>();
	GoldFX->SetMaxParticleCount(55);
	GoldFX->SetSprite(L"../Resources/artResource/UI/Item/Goldenticket_effect.png");
	GoldFXGO->GetTransform()->SetParent(GO->GetTransform(), false);
	GoldFX->SetRenderLayer(1 << (id+1));
	GoldFX->SetMinScale(0.05f);
	GoldFX->SetMaxScale(0.1f);
	GoldFX->SetRenderOrder(2000);

	auto ControllerTextGO = new GameObject(L"Controller Text");
	auto ControllerText = ControllerTextGO->AddComponent<TextRenderer>();
	ControllerText->SetFont(L"../Resources/Maplestory Bold.ttf");
	ControllerText->size = 18;
	ControllerText->color = { 0,255,0,255 };
	ControllerText->SetRenderLayer((1 << (id + 1)) | (1 << 3));
	ControllerText->SetRenderOrder(1000 - (id * 2) - 1); // 커서가 항상 위에 보이도록 설정

	ControllerTextGO->GetTransform()->SetParent(GO->GetTransform(), false);
	ControllerTextGO->GetTransform()->SetLocalPosition({ 0.0f,-65.0f });
	
	auto crosshairCon = GO->AddComponent<CrosshairController>();
	crosshairCon->collide = crosshairCollide;
	crosshairCon->move = crosshairMove;
	crosshairCon->fire = crosshairFire;
	crosshairCon->text = ControllerText;

	for (int i = 0; i < 3; i++)
	{
		auto subCrosshairGO = CreateSubCrosshair(id);
		subCrosshairGO->GetTransform()->SetParent(GO->GetTransform(),false);
		crosshairCon->subCrosshairs[i] = subCrosshairGO;
		crosshairMove->subColliders[i] = subCrosshairGO->GetComponent<CrosshairCollide>();
	}

	for (int i = 0; i < 5; i++)
	{
		auto bulletSpriteRendererGO = new GameObject(L"bulletSprite");
		
		//추후에 스프라이트로 변경
		auto rect = bulletSpriteRendererGO->AddComponent<RectRenderer>();
		rect->SetRect({ 0,0,15,15 });
		rect->SetColor({ 15,255,15,255 });
		rect->SetRenderOrder(1000 - (id * 2) - 1);
		rect->SetRenderLayer((1 << (id + 1)) | (1 << 3));

		bulletSpriteRendererGO->GetTransform()->SetParent(GO->GetTransform(),false);

		crosshairCon->bulletImageTransforms[i] = bulletSpriteRendererGO->GetTransform();
		crosshairCon->bulletImageTransforms[i]->GetGameObject()->SetActive(false);
	}

	return GO;
}

GameObject* GOTOEngine::CrosshairPrefab::CreateSubCrosshair(int id)
{
	auto GO = new GameObject(L"SubCrosshair");
    auto fire =	GO->AddComponent<CrosshairFire>();
	auto collide = GO->AddComponent<CrosshairCollide>();

	fire->id = id;
	fire->dontShake = true;
	collide->id = id;

	auto SpriteRendererGO = new GameObject(L"Crosshair Sprite");
	auto spriteRenderer = SpriteRendererGO->AddComponent<SpriteRenderer>();
	spriteRenderer->SetSprite(id == 0 ? L"../Resources/Demo/Crosshair.png" : L"../Resources/Demo/Crosshair2.png");
	spriteRenderer->SetRenderOrder(1000 - (id * 2)); // 커서가 항상 위에 보이도록 설정

	spriteRenderer->SetRenderLayer((1 << (id + 1)) | (1 << 3));
	SpriteRendererGO->GetTransform()->SetParent(GO->GetTransform(), false);

	auto physAnimation = SpriteRendererGO->AddComponent<ButtonAnimation>();
	physAnimation->scaleDamping = 16.0f;
	physAnimation->strength = 220.0f;

	fire->physAnimation = physAnimation;

	collide->spriteRenderer = spriteRenderer;

	auto col2d = GO->AddComponent<Collider2D>(); // Collider 크기 조정
	col2d->SetSize({ 45.0f, 45.0f });
	col2d->SetIsTrigger(true);


	auto GageGO = new GameObject(L"Croshair Gage");
	auto radialRenderer = GageGO->AddComponent<RadialSpriteRenderer>();
	radialRenderer->SetSprite(id == 0 ? L"../Resources/Demo/Crosshair_gage.png" : L"../Resources/Demo/Crosshair_gage2.png");
	radialRenderer->SetRenderOrder(1000 - (id * 2) - 1); // 커서가 항상 위에 보이도록 설정
	radialRenderer->SetRenderLayer((1 << (id + 1)) | (1 << 3));
	fire->gageSprite = radialRenderer;

	GageGO->GetTransform()->SetParent(GO->GetTransform(), false);

	auto particleSys = GO->AddComponent<ParticleSystem>();
	particleSys->SetRenderLayer((1 << (id + 1)) | (1 << 3));
	particleSys->SetRenderOrder(2);
	particleSys->SetFadeOutTime(0.3f);
	particleSys->SetSprite(L"../Resources/Demo/drops.png");
	particleSys->SetMinAngularVelocityDegrees(0.0f);
	particleSys->SetMaxAngularVelocityDegrees(35.0f);
	particleSys->SetSpawnInterval(0.65f);
	particleSys->SetParticlesPerSpawn(2);
	particleSys->SetGravity({ 0.0f,-450.0f });

	fire->dropParticleSys = particleSys;

	auto StrengthTextGO = new GameObject(L"Strength Text");
	auto strengthText = StrengthTextGO->AddComponent<TextRenderer>();
	strengthText->SetFont(L"../Resources/Maplestory Bold.ttf");
	strengthText->size = 18;
	strengthText->SetRenderLayer((1 << (id + 1)) | (1 << 3));
	strengthText->SetRenderOrder(1000 - (id * 2) - 1); // 커서가 항상 위에 보이도록 설정

	StrengthTextGO->GetTransform()->SetParent(GO->GetTransform(), false);
	StrengthTextGO->GetTransform()->SetLocalPosition({ 0.0f,-65.0f });

	fire->strText = strengthText;

	fire->ChangeMode(CrosshairFireMode::Hold);

	GO->layer = (1 << (id + 1)); // 레이어 설정: 1 << 1 for Player 1, 1 << 2 for Player 2

	GO->SetActive(false);

	return GO;
}
