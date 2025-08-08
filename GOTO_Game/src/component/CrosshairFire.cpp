#include "CrosshairFire.h"
#include "SoundManager.h"

GOTOEngine::RumbleAnimationClip* GOTOEngine::CrosshairFire::s_pfireRumbleClip = nullptr; 
GOTOEngine::RumbleAnimationClip* GOTOEngine::CrosshairFire::s_pholdRumbleClip = nullptr;
GOTOEngine::RumbleAnimationClip* GOTOEngine::CrosshairFire::s_pholdLoopRumbleClip = nullptr;
int GOTOEngine::CrosshairFire::s_crosshairCount = 0;

void GOTOEngine::CrosshairFire::Awake()
{
    if (IsValidObject(gageSprite))
    {
        gageSprite->SetFillAmount(0.0f);
    }

	m_collider = GetComponent<CrosshairCollide>();
    if (!s_pholdRumbleClip)
    {
        s_pholdRumbleClip = new RumbleAnimationClip(AnimationCurve{ R"({
    "keyframes": [
        {
            "time": 0.0,
            "value": 0.45,
            "in_tangent": -0.7006375000000012,
            "out_tangent": -0.7006375000000012,
            "tangent_mode": 1
        },
        {
            "time": 0.69,
            "value": 0.025,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.155,
            "value": 0.0225,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.2577950000000002,
            "value": 0.21628750000000002,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.365,
            "value": 0.0275,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.947253,
            "value": 0.025,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 2.0497400000000003,
            "value": 0.44921250000000007,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 2.1595475,
            "value": 0.025,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 3.0,
            "value": 0.025,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 5.0,
            "value": 0.35,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        }
    ]
})" }, AnimationCurve{ R"({
    "keyframes": [
        {
            "time": 0.0,
            "value": 0.08,
            "in_tangent": 0.014131470776337807,
            "out_tangent": 0.014131470776337807,
            "tangent_mode": 1
        },
        {
            "time": 0.67,
            "value": 0.0125,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.125,
            "value": 0.0075,
            "in_tangent": -0.025,
            "out_tangent": -0.025,
            "tangent_mode": 1
        },
        {
            "time": 1.265,
            "value": 0.0525,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.385,
            "value": 0.0075,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.945,
            "value": 0.0075,
            "in_tangent": -0.0,
            "out_tangent": -0.0,
            "tangent_mode": 1
        },
        {
            "time": 2.05,
            "value": 0.0825,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 2.155,
            "value": 0.0025,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 3.03,
            "value": -0.0025,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 5.0,
            "value": 0.1,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        }
    ]
})" }, 5.0f);
    }

    if (!s_pfireRumbleClip)
    {
        s_pfireRumbleClip = new RumbleAnimationClip(AnimationCurve{ R"({
    "keyframes": [
        {
            "time": 0.0,
            "value": 1.0,
            "in_tangent": -0.375,
            "out_tangent": -0.375,
            "tangent_mode": 0
        },
        {
            "time": 0.135,
            "value": 0.4575,
            "in_tangent": -0.2,
            "out_tangent": -0.2,
            "tangent_mode": 1
        },
        {
            "time": 0.275,
            "value": 0.6875,
            "in_tangent": -0.35,
            "out_tangent": -0.35,
            "tangent_mode": 1
        },
        {
            "time": 0.43,
            "value": 0.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.0,
            "value": 0.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        }
    ]
})" }, AnimationCurve{ R"({
    "keyframes": [
        {
            "time": 0.0,
            "value": 1.0,
            "in_tangent": -0.375,
            "out_tangent": -0.375,
            "tangent_mode": 0
        },
        {
            "time": 0.135,
            "value": 0.4575,
            "in_tangent": -0.2,
            "out_tangent": -0.2,
            "tangent_mode": 1
        },
        {
            "time": 0.275,
            "value": 0.6875,
            "in_tangent": -0.35,
            "out_tangent": -0.35,
            "tangent_mode": 1
        },
        {
            "time": 0.43,
            "value": 0.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.0,
            "value": 0.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        }
    ]
})" }, 1.0f);
    }
    if (!s_pholdLoopRumbleClip)
    {
        s_pholdLoopRumbleClip = new RumbleAnimationClip(
            AnimationCurve{ R"({
    "keyframes": [
        {
            "time": 0.0,
            "value": 0.35,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.44,
            "value": 0.3275,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.58,
            "value": 0.5675,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.71,
            "value": 0.3125,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.21,
            "value": 0.2925,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.31,
            "value": 0.3525,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.415,
            "value": 0.1075,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.5,
            "value": 0.2975,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 2.0,
            "value": 0.35,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        }
    ]
})" },
AnimationCurve{ R"({
    "keyframes": [
        {
            "time": 0.0,
            "value": 0.08,
            "in_tangent": 0.014131470776337807,
            "out_tangent": 0.014131470776337807,
            "tangent_mode": 1
        },
        {
            "time": 0.67,
            "value": 0.0125,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.125,
            "value": 0.0075,
            "in_tangent": -0.025,
            "out_tangent": -0.025,
            "tangent_mode": 1
        },
        {
            "time": 1.265,
            "value": 0.0525,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1.385,
            "value": 0.0075,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.945,
            "value": 0.0075,
            "in_tangent": -0.0,
            "out_tangent": -0.0,
            "tangent_mode": 1
        },
        {
            "time": 2.05,
            "value": 0.0825,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 2.155,
            "value": 0.0025,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 3.03,
            "value": -0.0025,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 5.0,
            "value": 0.1,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        }
    ]
})" },
2.0f);
    }


	s_crosshairCount++;

    OnEnter(m_fireMode);
}

void GOTOEngine::CrosshairFire::OnEnable()
{
	m_fireCooldown = 0.0f;
}

void GOTOEngine::CrosshairFire::OnDestroy()
{
    s_crosshairCount--;
    if (s_crosshairCount <= 0)
    {
        if (s_pfireRumbleClip)
            delete s_pfireRumbleClip;
        s_pfireRumbleClip = nullptr;

        if (s_pholdRumbleClip)
            delete s_pholdRumbleClip;
        s_pholdRumbleClip = nullptr;

        if (s_pholdLoopRumbleClip)
            delete s_pholdLoopRumbleClip;
        s_pholdLoopRumbleClip = nullptr;
    }
}

void GOTOEngine::CrosshairFire::Update()
{    
    if (INPUT_GET_GAMEPAD_BUTTONDOWN(id,GamepadButton::ButtonNorth))
    {
        CrosshairFireMode nextMode = static_cast<CrosshairFireMode>(static_cast<int>(m_fireMode) + 1);
        if (nextMode == CrosshairFireMode::Count)
            nextMode = CrosshairFireMode::Trigger;

        ChangeMode(nextMode);
    }

    switch (m_fireMode)
    {
    case CrosshairFireMode::Trigger:
        TriggerModeUdpate();
        break;
    case CrosshairFireMode::Hold:
        HoldModeUdpate();
        break;
    case CrosshairFireMode::FullAuto:
        FullAutoModeUdpate();
        break;
    }
}

void GOTOEngine::CrosshairFire::TriggerModeUdpate()
{
    auto lastfireCoolDown = m_fireCooldown;

    m_fireCooldown -= TIME_GET_DELTATIME();
    m_fireCooldown = Mathf::Max(m_fireCooldown, 0.0f);

    if (m_fireCooldown > 0.0f)
    {
        if (IsValidObject(gageSprite))
        {
            gageSprite->SetFillAmount((fireRate - m_fireCooldown) / fireRate);
        }
        return;
    }
    else
    {
        if (lastfireCoolDown > 0.0f)
        {
            onCharge.Invoke(id);
            if (IsValidObject(physAnimation))
                physAnimation->ApplyScaleForce(3.2f);
        }

        if (IsValidObject(gageSprite))
        {
            gageSprite->SetFillAmount(0.0f);
        }
    }



    //입력 감지: 플레이어 ID별 키 또는 버튼 입력
    bool firePressed = (id == 0 && INPUT_GET_KEYDOWN(KeyCode::LeftShift)) ||
        (id == 1 && INPUT_GET_KEYDOWN(KeyCode::RightShift)) ||
        INPUT_GET_GAMEPAD_BUTTONDOWN(id, GamepadButton::ButtonR1);

    if (!firePressed || !IsValidObject(m_collider))
        return;

    m_fireCooldown = fireRate;

    bool isHit = false;
    for (auto* obj : m_collider->GetCollideObjects())
    {
        std::wcout << obj->name << std::endl;

        for (auto* comp : obj->GetAllComponents())
        {
            if (auto* attackable = dynamic_cast<IAttackAble*>(comp))
            {
                attackable->TakeDamage(id, 1);
                isHit = true;
            }
        }
    }

    if (id == 0)
        SoundManager::instance->PlaySFX("Shot1P");
    else
        SoundManager::instance->PlaySFX("Shot2P");

    if (IsValidObject(physAnimation))
    {
        physAnimation->ApplyTorque(400.0f);
        physAnimation->ApplyScaleForce(3.2f);
    }

    onFire.Invoke(id);
    if (isHit)
    {
        GamepadRumbleManager::instance->Play(id, *s_pfireRumbleClip, 1.0f);

        if (m_shaker)
            m_shaker->ShakeCamera(24, 55, 8);
    }

#ifdef _DEBUG
    std::cout << "Crosshair Fire! : " << id << std::endl;
#endif
}

void GOTOEngine::CrosshairFire::HoldModeUdpate()
{
    m_fireCooldown -= TIME_GET_DELTATIME();
    m_fireCooldown = Mathf::Max(m_fireCooldown, 0.0f);

    if (m_fireCooldown > 0.0f)
    {
        if (IsValidObject(gageSprite))
        {
            gageSprite->SetFillAmount((fireRate - m_fireCooldown) / fireRate);
        }
        return;
    }

    if (INPUT_GET_GAMEPAD_BUTTON(id, GamepadButton::ButtonR1) && m_fireCooldown == 0.0f)
    {
        gageSprite->SetClockwise(true);

        m_fireGage += fireGageUpRate * TIME_GET_DELTATIME();

        m_strCount = 1;

        if (m_rumbleAnimID == -1)
        {
            m_rumbleAnimID = GamepadRumbleManager::instance->Play(id, *s_pholdRumbleClip, 0.6f);
        }

        if (m_fireGage >= 2.5f && !m_startDrop && IsValidObject(dropParticleSys))
        {
            dropParticleSys->Play();
            m_startDrop = true;
        }

        if (m_fireGage >= 1.0f)
        {
            if (!m_charged)
            {
                m_charged = true;
                if(IsValidObject(physAnimation))
                    physAnimation->ApplyScaleForce(3.2f);
                onCharge.Invoke(id);
            }

            m_strCount = 2 + static_cast<int>((m_fireGage - 1.0f) / 1.65f);
            m_strCount = min(5, m_strCount);
        }

        if (m_fireGage >= 2.9f)
        {
            float x = Mathf::PerlinNoise(m_noiseSeed, TIME_GET_TOTALTIME() * 15.0f) * 2.0f - 1.0f;
            float y = Mathf::PerlinNoise(m_noiseSeed + 100.0f, TIME_GET_TOTALTIME() * 15.0f) * 2.0f - 1.0f;

            Vector2 moveVec = { x,y };
            moveVec = moveVec.Normalized() * Mathf::Min(2.0f, (m_fireGage - 2.9f) / 2.0f) * 320.0f;

            m_shakeMove = Vector2::Lerp(m_shakeMove, moveVec, 12.0f * TIME_GET_DELTATIME());

            GetTransform()->SetPosition(GetTransform()->GetPosition() + m_shakeMove * TIME_GET_DELTATIME());
        }

        if (m_fireGage >= 6.2f)
        {
            if (m_holdingRumbleAnimID == -1)
            {
                m_holdingRumbleAnimID = GamepadRumbleManager::instance->Play(id, *s_pholdLoopRumbleClip, 0.5f);
                m_nextPlayHoldingRumbleTime = TIME_GET_TOTALTIME() + 2.0f;
            }
            else if (m_nextPlayHoldingRumbleTime < TIME_GET_TOTALTIME())
            {
                m_holdingRumbleAnimID = -1;
            }
        }
    }
    else
    {
        bool hasHit = false;
        if (m_strCount != 0)
        {
            int loopCount = m_strCount;
            for (auto* obj : m_collider->GetCollideObjects())
            {
                if (loopCount == 0)
                    break;

                std::wcout << obj->name << std::endl;

                for (auto* comp : obj->GetAllComponents())
                {
                    bool isHit = false;
                    if (auto* attackable = dynamic_cast<IAttackAble*>(comp))
                    {
                        attackable->TakeDamage(id, 1);
                        isHit = true;
                        hasHit = true;
                    }

                    if (isHit)
                        loopCount--;
                }
            }

            onFire.Invoke(id);
            if (IsValidObject(physAnimation))
            {
                physAnimation->ApplyTorque(320.0f * (1.0f + (float)(m_strCount - 1) * (2.25f - 1.0f) / (5 - 1)));
                physAnimation->ApplyScaleForce(0.56f * Mathf::Max(1.25f, m_strCount));
            }

            if (hasHit)
            {
                GamepadRumbleManager::instance->Play(id, *s_pfireRumbleClip, 1.0f);
                if (m_shaker)
                    m_shaker->ShakeCamera(24, 55, 8);
            }

            if (id == 0)
                SoundManager::instance->PlaySFX("Shot1P");
            else
                SoundManager::instance->PlaySFX("Shot2P");

            m_strCount = 0;

            m_fireCooldown = 0.5f;
        }

        m_fireGage = 0;

        if (m_startDrop && IsValidObject(dropParticleSys))
        {
            m_startDrop = false;
            dropParticleSys->Stop();
        }

        if (m_rumbleAnimID != -1)
        {
            GamepadRumbleManager::instance->Stop(m_rumbleAnimID);
        }

        if (m_holdingRumbleAnimID != -1)
        {
            GamepadRumbleManager::instance->Stop(m_holdingRumbleAnimID);
        }

        m_strCount = 0;
        m_charged = false;
        m_rumbleAnimID = -1;
        m_holdingRumbleAnimID = -1;
        m_shakeMove = { 0,0 };

        gageSprite->SetClockwise(false);
    }

    if (IsValidObject(strText))
    {
        if (m_strCount < 2)
        {
            strText->text = L"";
        }
        else
        {
            strText->text = std::to_wstring(m_strCount);
        }
    }

    if (IsValidObject(gageSprite))
    {
        gageSprite->SetFillAmount(m_fireGage);
    }
}

void GOTOEngine::CrosshairFire::FullAutoModeUdpate()
{

}

void GOTOEngine::CrosshairFire::ChangeMode(CrosshairFireMode mode)
{
    if (m_fireMode == mode)
        return;
    OnExit(m_fireMode);
    m_fireMode = mode;
    OnEnter(m_fireMode);
}

void GOTOEngine::CrosshairFire::OnEnter(CrosshairFireMode mode)
{
    switch (mode)
    {
    case CrosshairFireMode::Trigger:
        gageSprite->SetClockwise(false);
        break;
    case CrosshairFireMode::Hold:
        break;
    case CrosshairFireMode::FullAuto:
        break;
    }
}

void GOTOEngine::CrosshairFire::OnExit(CrosshairFireMode mode)
{
    switch (mode)
    {
    case CrosshairFireMode::Trigger:
        break;
    case CrosshairFireMode::Hold:
        if (m_rumbleAnimID != -1)
        {
            GamepadRumbleManager::instance->Stop(m_rumbleAnimID);
        }

        if (m_holdingRumbleAnimID != -1)
        {
            GamepadRumbleManager::instance->Stop(m_holdingRumbleAnimID);
        }
        break;
    case CrosshairFireMode::FullAuto:
        break;
    }
}

void GOTOEngine::CrosshairFire::OnSceneLoaded()
{
    auto camGO = id == 0 ? GameObject::Find(L"p1Cam") : GameObject::Find(L"p2Cam");
    if(camGO)
        m_shaker = camGO->GetComponent<CameraShaker>();
}

GOTOEngine::CrosshairFireMode& GOTOEngine::operator++(CrosshairFireMode& state)
{
    state = static_cast<CrosshairFireMode>(static_cast<int>(state) + 1);
    if (state == CrosshairFireMode::Count) // 마지막을 넘어가면 다시 처음으로
        state = CrosshairFireMode::Trigger;
    return state;
}
