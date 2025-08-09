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

void GOTOEngine::CrosshairController::ChangeType(CrosshairType type)
{
    if (m_type == type)
        return;

    switch (m_type)
    {
    case GOTOEngine::CrosshairType::TriggerGun:
        break;
    case GOTOEngine::CrosshairType::HoldingGun:
        break;
    case GOTOEngine::CrosshairType::MachineGun:
        break;
    case GOTOEngine::CrosshairType::ShotGun:
        break;
    case GOTOEngine::CrosshairType::BigGun:
        break;
    default:
        break;
    }
}

void GOTOEngine::CrosshairController::OnSceneLoaded()
{
	//설정 리셋
	//일반 모드로 전환 -> 
}

void GOTOEngine::CrosshairController::Awake()
{

}

void GOTOEngine::CrosshairController::Update()
{
	
}

void GOTOEngine::CrosshairController::StartAnimation(CrosshairType type)
{
}

void GOTOEngine::CrosshairController::EndAnimation(CrosshairType type)
{
}
