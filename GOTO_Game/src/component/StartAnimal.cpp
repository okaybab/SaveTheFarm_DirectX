#include "StartAnimal.h"
#include "SoundManager.h"
#include "EnemySpawnManager.h"
#include "Starthit.h"

using namespace GOTOEngine;

AnimationCurve StartAnimal::wobbleXCurve = { R"({
    "keyframes": [
        {
            "time": 0.0,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.5475734,
            "value": 0.704598125000001,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.9179907,
            "value": 1.155540925000001,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.2159350500000001,
            "value": 1.058910325000001,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.4897217500000002,
            "value": 1.219961325000001,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.8681916,
            "value": 0.801228725000001,
            "in_tangent": -0.05,
            "out_tangent": -0.05,
            "tangent_mode": 1
        },
        {
            "time": 2.2225038,
            "value": 1.058910325000001,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 2.5285007000000004,
            "value": 0.6965455750000009,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 2.9391807500000002,
            "value": 1.187751125000001,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 3.18075725,
            "value": 0.8898067750000009,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 3.5,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        }
    ]
})" };

AnimationCurve StartAnimal::wobbleYCurve = { R"({
    "keyframes": [
        {
            "time": 0.0,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.1815,
            "value": 1.1404250000000007,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.38115,
            "value": 1.0315250000000007,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.62315,
            "value": 1.1041250000000007,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.78045,
            "value": 0.9589250000000006,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.9922,
            "value": 1.0617750000000006,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.1192499999999999,
            "value": 0.9105250000000006,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.2281499999999999,
            "value": 1.1041250000000007,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.3491499999999998,
            "value": 1.0496750000000006,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.5306499999999998,
            "value": 1.2916750000000006,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.5911499999999998,
            "value": 1.1464750000000006,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.694,
            "value": 1.1767250000000007,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.8,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        }
    ]
})" };

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
	crow->GetTransform()->SetLossyScale({ 0.6f, 0.6f });
	goldcrow->GetTransform()->SetLossyScale({ 0.3f, 0.3f });
	thiefmole->GetTransform()->SetLossyScale({ 0.12f, 0.12f });
	crow->GetTransform()->SetPosition({ Screen::GetWidth() * 0.3f, Screen::GetHeight() * 0.4f });
	goldcrow->GetTransform()->SetPosition({ Screen::GetWidth() * ( - 0.3f), Screen::GetHeight() * 0.05f});
	thiefmole->GetTransform()->SetPosition({ Screen::GetWidth() * 0.2f, Screen::GetHeight() * (-0.3f) });
	crowdie = Resource::Load<Sprite>(L"../Resources/artResource/SpriteSheet/Crow/Crow_Die.png");
	crowdie->IncreaseRefCount();
	golddie = Resource::Load<Sprite>(L"../Resources/artResource/Sprint/GoldCrow_die.png");
	golddie->IncreaseRefCount();
	thiefdie = Resource::Load<Sprite>(L"../Resources/artResource/Sprint/ThiefMole_die.png");
	thiefdie->IncreaseRefCount();
	auto crowrect = crowdie->GetRect();
	crow->AddComponent<Collider2D>()->SetSize({crowrect.width * crow->GetTransform()->GetLossyScale().x,crowrect.height * crow->GetTransform()->GetLossyScale().x });
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
    float currentTime = TIME_GET_TOTALTIME();
    
    float lastKeyFrameTime = wobbleXCurve.GetKeyframes()[wobbleXCurve.GetKeyframes().size() - 1].time;
    float t = fmod(abs(currentTime * 4.5f), lastKeyFrameTime);
    float wobbleXValue = wobbleXCurve.Evaluate(t);

    lastKeyFrameTime = wobbleYCurve.GetKeyframes()[wobbleYCurve.GetKeyframes().size() - 1].time;
    t = fmod(abs(currentTime * 3.2f), lastKeyFrameTime);
    float wobbleYValue = wobbleYCurve.Evaluate(t);

	auto crowpos = crow->GetTransform()->GetPosition();
	if (!crowup && !crowhit) {
		crowpos.y -= 100.0f * TIME_GET_DELTATIME();
		crow->GetTransform()->SetPosition(crowpos);
		if (crowpos.y < 0) {
			crowup = true;
		}
        crowSavedPos = crowpos;
	}
	else if(crowup && !crowhit) {
		crowpos.y += 100.0f * TIME_GET_DELTATIME();
		crow->GetTransform()->SetPosition(crowpos);
		if (crowpos.y > Screen::GetHeight() * 0.4f) {
			crowup = false;
		}
        crowSavedPos = crowpos;
	}
	auto thiefpos = thiefmole->GetTransform()->GetPosition();
	if (!thiefleft && !thiefhit) {
		thiefpos.x += 100.0f * TIME_GET_DELTATIME();
		thiefmole->GetTransform()->SetPosition(thiefpos);
		thiefmole->GetComponent<SpriteRenderer>()->SetFlipX(false);
		if (thiefpos.x > Screen::GetWidth() * 0.4f) {
			thiefleft = true;
		}
        thiefmoleSvaedPos = thiefpos;
	}
	else if(thiefleft && !thiefhit){
		thiefpos.x -= 100.0f * TIME_GET_DELTATIME();
		thiefmole->GetTransform()->SetPosition(thiefpos);
		thiefmole->GetComponent<SpriteRenderer>()->SetFlipX(true);
		if (thiefpos.x < Screen::GetWidth() * 0.2f) {
			thiefleft = false;
		}
        thiefmoleSvaedPos = thiefpos;
	}

    if (!goldhit)
        goldcrowSavedPos = goldcrow->GetTransform()->GetPosition();

	if (crowhittimer > 0.0f) {
		crowhit = true;
		crow->GetComponent<Animator>()->SetEnabled(false);
		crow->GetComponent<SpriteRenderer>()->SetSprite(crowdie);
		crowhittimer -= TIME_GET_DELTATIME();

        crow->GetTransform()->SetLocalPosition({ crowSavedPos.x + (1.0f - wobbleXValue) * crowhittimer * 45.0f,crowSavedPos.y + (1.0f - wobbleYValue) * crowhittimer * 45.0f });

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

        goldcrow->GetTransform()->SetLocalPosition({ goldcrowSavedPos.x + (1.0f - wobbleXValue) * goldhittimer * 45.0f,goldcrowSavedPos.y + (1.0f - wobbleYValue) * goldhittimer * 45.0f });

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

        thiefmole->GetTransform()->SetLocalPosition({ thiefmoleSvaedPos.x + (1.0f - wobbleXValue) * thiefhittimer * 45.0f,thiefmoleSvaedPos.y + (1.0f - wobbleYValue) * thiefhittimer * 45.0f });

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