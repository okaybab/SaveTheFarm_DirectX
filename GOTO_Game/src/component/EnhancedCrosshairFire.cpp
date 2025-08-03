#include "EnhancedCrosshairFire.h"

void GOTOEngine::EnhancedCrosshairFire::Awake()
{
	m_collider = GetComponent<CrosshairCollide>();
}

void GOTOEngine::EnhancedCrosshairFire::OnEnable()
{
	m_fireCooldown = 0.0f;
	m_RightTriggerCheckTrigger = false;
	TriggerPressedCheckReset();
}

void GOTOEngine::EnhancedCrosshairFire::OnDestroy()
{
}

void GOTOEngine::EnhancedCrosshairFire::TriggerPressedCheck()
{
	auto currentRightTrigger = INPUT_GET_GAMEPAD_AXIS(id, GamepadAxis::RightTrigger);
	if (!m_RightTriggerCheckTrigger)
	{
		if (currentRightTrigger > 0.89f)
		{
			m_RightTriggerCheckTrigger = true;
			m_RightTriggerPressed = true;
			return;
		}
	}
	else if ((m_RightTriggerCheckTrigger && currentRightTrigger < 0.2f))
	{
		m_RightTriggerCheckTrigger = false;
	}
}

void GOTOEngine::EnhancedCrosshairFire::TriggerPressedCheckReset()
{
	m_RightTriggerPressed = false;
}

void GOTOEngine::EnhancedCrosshairFire::Update()
{
	TriggerPressedCheckReset();
	TriggerPressedCheck();

	if (m_RightTriggerCheckTrigger)
	{
		m_fireGage += fireGageUpRate * TIME_GET_DELTATIME();

		if (m_fireGage >= 2.5f && !m_startDrop && IsValidObject(dropParticleSys))
		{
			dropParticleSys->Play();
			m_startDrop = true;
		}

		if (m_fireGage >= 1.0f)
		{
			m_strCount = 1 + static_cast<int>((m_fireGage - 1.0f) / 1.2f);
			m_strCount = min(5, m_strCount);
		}
	}
	else
	{
		if (m_strCount != 0)
		{
			for (auto* obj : m_collider->GetCollideObjects())
			{
				if (m_strCount == 0)
					break;

				std::wcout << obj->name << std::endl;

				for (auto* comp : obj->GetAllComponents())
				{
					bool isHit = false;
					if (auto* attackable = dynamic_cast<IAttackAble*>(comp))
					{
						attackable->TakeDamage(1);
						isHit = true;
					}

					if(isHit)
						m_strCount--;
				}
			}

			onFire.Invoke(id);
		}

		m_fireGage = 0;

		if (m_startDrop && IsValidObject(dropParticleSys))
		{
			m_startDrop = false;
			dropParticleSys->Stop();
		}

		m_strCount = 0;
	}

	if (IsValidObject(strText))
	{
		if (m_strCount == 0)
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
