#include "StartAnimal.h"
#include "SoundManager.h"
#include "Starthit.h"

using namespace GOTOEngine;

void StartAnimal::Awake() {
	crowanimator = Resource::Load<AnimatorController>(L"../Resources/Animation/controller/CrowAnimator_AnimController.json");
	crowanimator->IncreaseRefCount();
	goldcrowanimator = Resource::Load<AnimatorController>(L"../Resources/Animation/controller/GoldCrowAnimator_AnimController.json");
	goldcrowanimator->IncreaseRefCount();
	thiefmoleanimator = Resource::Load<AnimatorController>(L"../Resources/Animation/controller/ThiefMoleAnimator_AnimController.json");
	thiefmoleanimator->IncreaseRefCount();
	crow->AddComponent<SpriteRenderer>();
	goldcrow->AddComponent<SpriteRenderer>();
	thiefmole->AddComponent<SpriteRenderer>();
	crow->AddComponent<Animator>()->SetAnimatorController(crowanimator);
	goldcrow->AddComponent<Animator>()->SetAnimatorController(goldcrowanimator);
	thiefmole->AddComponent<Animator>()->SetAnimatorController(thiefmoleanimator);
	crow->GetTransform()->SetLossyScale({ 0.3f, 0.3f });
	goldcrow->GetTransform()->SetLossyScale({ 0.3f, 0.3f });
	thiefmole->GetTransform()->SetLossyScale({ 0.12f, 0.12f });
	crow->GetTransform()->SetPosition({ Screen::GetWidth() * 0.3f, Screen::GetHeight() * 0.4f });
	goldcrow->GetTransform()->SetPosition({ Screen::GetWidth() * ( - 0.3f), Screen::GetHeight() * 0.05f});
	thiefmole->GetTransform()->SetPosition({ Screen::GetWidth() * 0.2f, Screen::GetHeight() * (-0.3f) });
	crowdie = Resource::Load<Sprite>(L"../Resources/artResource/Sprint/Crow_die.png");
	crowdie->IncreaseRefCount();
	golddie = Resource::Load<Sprite>(L"../Resources/artResource/Sprint/GoldCrow_die.png");
	golddie->IncreaseRefCount();
	thiefdie = Resource::Load<Sprite>(L"../Resources/artResource/Sprint/ThiefMole_die.png");
	thiefdie->IncreaseRefCount();
	auto crowrect = crowdie->GetRect();
	crow->AddComponent<Collider2D>()->SetSize({crowrect.width*0.3f,crowrect.height*0.3f});
	auto goldrect = golddie->GetRect();
	goldcrow->AddComponent<Collider2D>()->SetSize({ goldrect.width * 0.3f,goldrect.height * 0.3f });
	auto thiefrect = thiefdie->GetRect();
	thiefmole->AddComponent<Collider2D>()->SetSize({ thiefrect.width * 0.12f,thiefrect.height * 0.12f });
	crow->layer = (1 << 1) | (1 << 2);
	goldcrow->layer = (1 << 1) | (1 << 2);
	thiefmole->layer = (1 << 1) | (1 << 2);
	auto crowhitcomp = crow->AddComponent<Starthit>();
	crowhitcomp->id = 0;
	crowhitcomp->startanimal = this;
	auto goldhitcomp = goldcrow->AddComponent<Starthit>();
	goldhitcomp->id = 1;
	goldhitcomp->startanimal = this;
	auto thiefhitcomp = thiefmole->AddComponent<Starthit>();
	thiefhitcomp->id = 2;
	thiefhitcomp->startanimal = this;
}

void StartAnimal::OnDestroy() {
	if (IsValidObject(crowanimator))
		crowanimator->DecreaseRefCount();
	if (IsValidObject(goldcrowanimator))
		goldcrowanimator->DecreaseRefCount();
	if (IsValidObject(thiefmoleanimator))
		thiefmoleanimator->DecreaseRefCount();
	if (IsValidObject(crowdie))
		crowdie->DecreaseRefCount();
	if (IsValidObject(golddie))
		golddie->DecreaseRefCount();
	if (IsValidObject(thiefdie))
		thiefdie->DecreaseRefCount();
}

void StartAnimal::Update() {
	auto crowpos = crow->GetTransform()->GetPosition();
	if (!crowup && !crowhit) {
		crowpos.y -= 100.0f * TIME_GET_DELTATIME();
		crow->GetTransform()->SetPosition(crowpos);
		if (crowpos.y < 0) {
			crowup = true;
		}
	}
	else if(crowup && !crowhit) {
		crowpos.y += 100.0f * TIME_GET_DELTATIME();
		crow->GetTransform()->SetPosition(crowpos);
		if (crowpos.y > Screen::GetHeight() * 0.4f) {
			crowup = false;
		}
	}
	auto thiefpos = thiefmole->GetTransform()->GetPosition();
	if (!thiefleft && !thiefhit) {
		thiefpos.x += 100.0f * TIME_GET_DELTATIME();
		thiefmole->GetTransform()->SetPosition(thiefpos);
		thiefmole->GetComponent<SpriteRenderer>()->SetFlipX(false);
		if (thiefpos.x > Screen::GetWidth() * 0.4f) {
			thiefleft = true;
		}
	}
	else if(thiefleft && !thiefhit){
		thiefpos.x -= 100.0f * TIME_GET_DELTATIME();
		thiefmole->GetTransform()->SetPosition(thiefpos);
		thiefmole->GetComponent<SpriteRenderer>()->SetFlipX(true);
		if (thiefpos.x < Screen::GetWidth() * 0.2f) {
			thiefleft = false;
		}
	}
	if (crowhittimer > 0.0f) {
		crowhit = true;
		crow->GetComponent<Animator>()->SetEnabled(false);
		crow->GetComponent<SpriteRenderer>()->SetSprite(crowdie);
		crowhittimer -= TIME_GET_DELTATIME();
		if (crowhittimer <= 0.0f) {
			crowhittimer = 0.0f;
			crow->GetComponent<Animator>()->SetEnabled(true);
			crowhit = false;
		}
	}
	if (goldhittimer > 0.0f) {
		goldhit = true;
		goldcrow->GetComponent<Animator>()->SetEnabled(false);
		goldcrow->GetComponent<SpriteRenderer>()->SetSprite(golddie);
		goldhittimer -= TIME_GET_DELTATIME();
		if (goldhittimer <= 0.0f) {
			goldhittimer = 0.0f;
			goldcrow->GetComponent<Animator>()->SetEnabled(true);
			goldhit = false;
		}
	}
	if (thiefhittimer > 0.0f) {
		thiefhit = true;
		thiefmole->GetComponent<Animator>()->SetEnabled(false);
		thiefmole->GetComponent<SpriteRenderer>()->SetSprite(thiefdie);
		thiefhittimer -= TIME_GET_DELTATIME();
		if (thiefhittimer <= 0.0f) {
			thiefmole->GetComponent<Animator>()->SetEnabled(true);
			thiefhittimer = 0.0f;
			thiefhit = false;
		}
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha3)) {
		crowhittimer = 1.0f;
	}
}