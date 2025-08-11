#include "DefenseModeCameraManager.h"
#include "CrosshairCollide.h"

#include <Camera.h>

GOTOEngine::DefenseModeCameraManager* GOTOEngine::DefenseModeCameraManager::instance = nullptr;
GOTOEngine::AnimationCurve GOTOEngine::DefenseModeCameraManager::s_openMotionCurve = { R"({
    "keyframes": [
        {
            "time": 0,
            "value": 0,
            "in_tangent": -0.0875,
            "out_tangent": -0.0875,
            "tangent_mode": 1
        },
        {
            "time": 0.235,
            "value": 0.0375,
            "in_tangent": 0.125,
            "out_tangent": 0.125,
            "tangent_mode": 1
        },
        {
            "time": 1.0,
            "value": 1.0,
            "in_tangent": 2.9625,
            "out_tangent": 2.9625,
            "tangent_mode": 1
        }
    ]
})" };

GOTOEngine::AnimationCurve GOTOEngine::DefenseModeCameraManager::s_wobbleMotionXCurve = { R"({
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

GOTOEngine::AnimationCurve GOTOEngine::DefenseModeCameraManager::s_wobbleMotionYCurve = { R"({
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

void GOTOEngine::DefenseModeCameraManager::Awake()
{
	if (instance == nullptr)
	{
		instance = this;

		auto p1CamGO = GameObject::Find(L"p1Cam");
		if (IsValidObject(p1CamGO))
		{
			m_p1Cam = p1CamGO->GetComponent<Camera>();
		}

		auto p2CamGO = GameObject::Find(L"p2Cam");
		if (IsValidObject(p2CamGO))
		{
			m_p2Cam = p2CamGO->GetComponent<Camera>();
		}

		auto commonCamGO = GameObject::Find(L"commonCam");
		if (IsValidObject(commonCamGO))
		{
			m_commonCam = commonCamGO->GetComponent<Camera>();
		}

		auto cameras = FindObjectsByType<Camera>();

		for (auto& cam : cameras)
		{
			if (cam->GetGameObject()->name == L"backgroundCam")
			{
				m_backgroundCam = cam;
				break;
			}
		}

		auto p1GO = GameObject::Find(L"Player1");
		if (IsValidObject(p1GO))
		{
			m_p1GO = p1GO;
		}
		auto p2GO = GameObject::Find(L"Player2");
		if (IsValidObject(p1GO))
		{
			m_p2GO = p1GO;
		}
		
		if (IsValidObject(m_p1GO))
		{
			m_p1CrossCol = m_p1GO->GetComponent<CrosshairCollide>();
		}

		if (IsValidObject(m_p2GO))
		{
			m_p2CrossCol = m_p2GO->GetComponent<CrosshairCollide>();
		}
	}
	else
	{
		Destroy(GetGameObject());
	}
}

void GOTOEngine::DefenseModeCameraManager::Update()
{
    if (INPUT_GET_KEYDOWN(KeyCode::F12))
        m_isOpen = !m_isOpen;


    if (m_isOpen)
    {
        m_motionTimer += Mathf::Min(TIME_GET_DELTATIME(), TIME_GET_FIXED_DELTATIME());
    }
    else
    {
        m_motionTimer -= Mathf::Min(TIME_GET_DELTATIME(), TIME_GET_FIXED_DELTATIME());
    }

    m_motionTimer = Mathf::Clamp01(m_motionTimer);
	

	auto t = (m_motionTimer / 1.0f);

    float t2 = s_openMotionCurve.Evaluate(t);
    if (t > 0.0f)
        m_backgroundCam->GetGameObject()->SetActive(true);
    else
        m_backgroundCam->GetGameObject()->SetActive(false);

	if (IsValidObject(m_p1Cam))
	{
		//p1 view Rect({ 0.0f, 0.0f, 0.5f, 1.0f });
		m_p1Cam->SetRect({ -0.5f * t2, 0.0f, 0.5f, 1.0f });
	}

	if (IsValidObject(m_p2Cam))
	{
		//p2 view Rect({ 0.5f, 0.0f, 0.5f, 1.0f });
		m_p2Cam->SetRect({ 0.5f + (0.5f * t2),0.0f, 0.5f, 1.0f });
	}
}

void GOTOEngine::DefenseModeCameraManager::OnDestroy()
{
	if (instance == this)
		instance = nullptr;
}

void GOTOEngine::DefenseModeCameraManager::Open()
{
    m_isOpen = true;
}

void GOTOEngine::DefenseModeCameraManager::Close()
{
    m_isOpen = false;
}
