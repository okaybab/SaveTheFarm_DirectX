#include "CrosshairController.h"

GOTOEngine::AnimationCurve GOTOEngine::CrosshairController::s_bigGunAnimationCurve = {R"({
    "keyframes": [
        {
            "time": 0.0,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.25,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.26,
            "value": 1.5,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.35,
            "value": 1.5,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.36,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.45,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.46,
            "value": 1.5,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.55,
            "value": 1.5,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.56,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.65,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.66,
            "value": 1.5,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.75,
            "value": 1.5,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.76,
            "value": 2.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.85,
            "value": 2.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.86,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.95,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.96,
            "value": 1.5,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.05,
            "value": 1.5,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.06,
            "value": 2.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.15,
            "value": 2.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.16,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.25,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.26,
            "value": 2.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        }
    ]
})"};

float GOTOEngine::CrosshairController::BulletSpriteStartX(int length, float bias)
{
    //짝수의 경우
    int i = length / 2;

    if (length % 2 == 0)
    {
        return (-bias * i) + (bias * 0.5f);
    }
    //홀수의 경우
    else
    {
        return -bias * i;
    }
}

void GOTOEngine::CrosshairController::ChangeType(CrosshairType type)
{
    if (m_type == type)
    {
        ResetValue(m_type);
        return;
    }

    OnExit(m_type);
    m_type = type;
    OnEnter(m_type);
}

void GOTOEngine::CrosshairController::OnSceneLoaded()
{
	//설정 리셋
	//일반 모드로 전환 -> 
}

void GOTOEngine::CrosshairController::Start()
{
    OnEnter(m_type);
    fire->onFire.Add([this](int id) { OnCrosshairFire(); });

    m_colSize = collide->GetColSize();
}

void GOTOEngine::CrosshairController::Update()
{
    //디버그 코드
    //if (INPUT_GET_KEYDOWN(KeyCode::F5))
    //{
    //    ChangeType(CrosshairType::TriggerGun);
    //}    
    //if (INPUT_GET_KEYDOWN(KeyCode::F6))
    //{
    //    ChangeType(CrosshairType::HoldingGun);
    //}    
    //if (INPUT_GET_KEYDOWN(KeyCode::F7))
    //{
    //    ChangeType(CrosshairType::MachineGun);
    //}    
    //if (INPUT_GET_KEYDOWN(KeyCode::F8))
    //{
    //    ChangeType(CrosshairType::ShotGun);
    //}    
    //if (INPUT_GET_KEYDOWN(KeyCode::F9))
    //{
    //    ChangeType(CrosshairType::BigGun);
    //}

#ifdef _DEBUG
    if (INPUT_GET_KEYDOWN(KeyCode::F9))
    {
        SCENE_GET_CURRENTSCENE()->PrintCurrentGameObjects();
    }
#endif

    //트랜지션 체크 및 애니메이션 처리
    switch (m_type)
    {
    case CrosshairType::TriggerGun:
    case CrosshairType::HoldingGun:
        break;
    case CrosshairType::MachineGun:
        if (m_bulletCount <= 0)
        {
            ChangeType(CrosshairType::HoldingGun);
            break;
        }

        text->text = L"X " + std::to_wstring(m_bulletCount);

        break;
    case CrosshairType::ShotGun:
    {
        float deltaTime = TIME_GET_DELTATIME();
        auto maxAnimTime = 0.25f;
        float percent = (m_animationTime / maxAnimTime);
        float offset = 110.0f; // 원하는 거리

        //흩어지기
        if (m_bulletCount > 0 && !m_animationDone)
        {
            m_animationTime += deltaTime;

            if (m_animationTime > maxAnimTime)
            {
                m_animationTime = maxAnimTime;
                m_animationDone = true;
            }

            subCrosshairs[0]->GetTransform()->SetLocalPosition({ offset * percent, 0.0f });
            subCrosshairs[1]->GetTransform()->SetLocalPosition({ offset * percent, -offset * percent });
            subCrosshairs[2]->GetTransform()->SetLocalPosition({ 0.0f, -offset * percent });

            move->clampOffset = { -(offset * 0.5f * percent),(offset * 0.5f * percent) };
            GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + (Vector2{ -(offset * 0.5f), (offset * 0.5f) } / maxAnimTime) * deltaTime);

            break;
        }

        if (m_bulletCount > 0 && m_animationDone)
        {
            float offset = 110.0f; // 원하는 거리

            float bias = 17.5f;
            int length = 5;

            float startX = BulletSpriteStartX(m_bulletCount, bias);

            for (int i = 0; i < length; i++)
            {
                if (i > m_bulletCount - 1)
                {
                    bulletImageTransforms[i]->GetGameObject()->SetActive(false);
                    continue;
                }

                bulletImageTransforms[i]->GetGameObject()->SetActive(true);
                bulletImageTransforms[i]->SetLocalPosition({ startX + (offset * 0.5f), -195.0f });
                bulletImageTransforms[i]->GetPosition();
                startX += bias;
            }
        }

        //합치기
        if (m_bulletCount <= 0 && !m_animationDone)
        {
            m_animationTime -= deltaTime;

            if (m_animationTime <= 0)
            {
                m_animationTime = 0;
                m_animationDone = true;
            }

            subCrosshairs[0]->GetTransform()->SetLocalPosition({ offset * percent, 0.0f });
            subCrosshairs[1]->GetTransform()->SetLocalPosition({ offset * percent, -offset * percent });
            subCrosshairs[2]->GetTransform()->SetLocalPosition({ 0.0f, -offset * percent });

            move->clampOffset = { -(offset * 0.5f * percent),(offset * 0.5f * percent) };
            GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + (Vector2{ (offset * 0.5f), -(offset * 0.5f) } / maxAnimTime) * deltaTime);

            break;
        }

        //교체
        if (m_bulletCount <= 0 && m_animationDone)
        {
            ChangeType(CrosshairType::HoldingGun);
            break;
        }
        break;
    }
    case CrosshairType::BigGun:
    {
        float deltaTime = TIME_GET_DELTATIME();
        float lastCurveKeyFrameTime = s_bigGunAnimationCurve.GetKeyframes().back().time;
        m_coolTime -= deltaTime;

        if (m_coolTime > 0 && !m_animationDone)
        {
            m_animationTime += deltaTime;

            float curveValue = s_bigGunAnimationCurve.Evaluate(m_animationTime * 1.8f);

            collide->SetColSize(m_colSize * curveValue);

            GetTransform()->SetLocalScale({ curveValue,curveValue });

            if (m_animationTime * 1.8f > lastCurveKeyFrameTime)
            {
                m_animationDone = true;
                m_animationTime = 0.0f;
            }
        }
        
        text->text = std::to_wstring(static_cast<int>(Mathf::Max(0.0f, m_coolTime + 1.0f)));

        if (m_coolTime <= 0)
        {
            m_animationDone = false;
            text->GetGameObject()->SetActive(false);
        }

        if (m_coolTime <= 0 && !m_animationDone)
        {
            m_animationTime += deltaTime;

            float curveValue = s_bigGunAnimationCurve.Evaluate(lastCurveKeyFrameTime - (m_animationTime * 1.8f));

            collide->SetColSize(m_colSize * curveValue);

            GetTransform()->SetLocalScale({ curveValue,curveValue });

            if (m_animationTime * 1.8f > lastCurveKeyFrameTime)
            {
                m_animationDone = true;
                m_animationTime = 0.0f;
            }
        }

        if (m_coolTime <= 0 && m_animationDone)
        {
            ChangeType(CrosshairType::HoldingGun);
        }
        break;
    }
        
    }
}

void GOTOEngine::CrosshairController::OnEnter(CrosshairType type)
{
    switch (m_type)
    {
    case CrosshairType::TriggerGun:
        fire->ChangeMode(CrosshairFireMode::Trigger);
        break;
    case CrosshairType::HoldingGun:
        fire->ChangeMode(CrosshairFireMode::Hold);
        break;
    case CrosshairType::MachineGun:
    {
        fire->ChangeMode(CrosshairFireMode::FullAuto);
        fire->p1fireSFXClipname = "Continuous";
        fire->p2fireSFXClipname = "Continuous";
        bulletImageTransforms[0]->GetGameObject()->SetActive(true);
        bulletImageTransforms[0]->SetLocalPosition({ -19.0f, -65.0f });
        m_bulletCount = 30;
        text->GetGameObject()->SetActive(true);
        text->horizontalAlign = TextHoriAlign::Left;
        text->GetTransform()->SetLocalPosition({ 45.0f, -65.0f });
        text->text = L"X " + std::to_wstring(m_bulletCount);
        break;
    }
    case CrosshairType::ShotGun:
    {
        m_animationDone = false;
        m_animationTime = 0.0f;
        fire->ChangeMode(CrosshairFireMode::Hold);

        for (int i = 0; i < 3; i++)
        {
            subCrosshairs[i]->SetActive(true);
        }

        subCrosshairs[0]->GetTransform()->SetLocalPosition({ 0.0f, 0.0f });
        subCrosshairs[1]->GetTransform()->SetLocalPosition({ 0.0f, 0.0f });
        subCrosshairs[2]->GetTransform()->SetLocalPosition({ 0.0f, 0.0f });

        m_bulletCount = 5;

        break;
    }
    case CrosshairType::BigGun:
    {
        fire->p1fireSFXClipname = "ShotBig1P";
        fire->p2fireSFXClipname = "ShotBig2P";
        m_coolTime = 10.0f;
        m_animationDone = false;
        m_animationTime = 0.0f;
        fire->damage = 2;
        fire->ChangeMode(CrosshairFireMode::Hold);
        text->GetGameObject()->SetActive(true);
        text->horizontalAlign = TextHoriAlign::Center;
        text->GetTransform()->SetLocalPosition({ 0.0f, -90.0f });
        text->text = std::to_wstring(static_cast<int>(m_coolTime));
        break;
    }
    }
}

void GOTOEngine::CrosshairController::OnExit(CrosshairType type)
{
    switch (m_type)
    {
    case CrosshairType::TriggerGun:
        break;
    case CrosshairType::HoldingGun:
        break;
    case CrosshairType::MachineGun:
        fire->p1fireSFXClipname = "Shot1P";
        fire->p2fireSFXClipname = "Shot2P";
        bulletImageTransforms[0]->GetGameObject()->SetActive(false);
        text->GetGameObject()->SetActive(false);
        break;
    case CrosshairType::ShotGun:
    {
        for (int i = 0; i < 3; i++)
        {
            subCrosshairs[i]->SetActive(false);
        }

        int length = 5;
        for (int i = 0; i < length; i++)
        {
            bulletImageTransforms[i]->GetGameObject()->SetActive(false);
        }

        auto offset = 110.f;

        auto maxAnimTime = 0.25f;
        float percent = (m_animationTime / maxAnimTime);

        GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + Vector2{ (offset * 0.5f * percent), -(offset * 0.5f * percent) });

        move->clampOffset = { 0.0f,0.0f };

        break;
    }
    case CrosshairType::BigGun:
        fire->p1fireSFXClipname = "Shot1P";
        fire->p2fireSFXClipname = "Shot2P";
        fire->damage = 1;
        collide->SetColSize(m_colSize);
        GetTransform()->SetLocalScale({1.0f,1.0f});
        text->GetGameObject()->SetActive(false);
        break;
    }
}

void GOTOEngine::CrosshairController::ResetValue(CrosshairType type)
{
    switch (m_type)
    {
    case CrosshairType::TriggerGun:
        break;
    case CrosshairType::HoldingGun:
        break;
    case CrosshairType::MachineGun:
        m_bulletCount = 30;
        break;
    case CrosshairType::ShotGun:
        m_bulletCount = 5;
        break;
    case CrosshairType::BigGun:
        if (m_coolTime <= 0 && !m_animationDone)
        {
            OnEnter(m_type);
            return;
        }
        m_coolTime = 10;
        text->GetGameObject()->SetActive(true);
        break;
    }
}

void GOTOEngine::CrosshairController::OnCrosshairFire()
{
    switch (m_type)
    {
    case CrosshairType::TriggerGun:
        break;
    case CrosshairType::HoldingGun:
        break;
    case CrosshairType::MachineGun:
        m_bulletCount--;
        break;
    case CrosshairType::ShotGun:
        m_bulletCount--; 
        if (m_bulletCount <= 0)
        {
            bulletImageTransforms[0]->GetGameObject()->SetActive(false);
            m_animationDone = false;
        }
        break;
    case CrosshairType::BigGun:
        break;
    }
}


