#include "Animator.h"
#include "TimeManager.h"

void GOTOEngine::Animator::OnAnimationUpdate()
{
	if (m_runtimeController && IsActiveAndEnabled())
	{
		m_runtimeController->Update(TIME_GET_DELTATIME());

		if (IsValidObject(m_spriteRenderer) && !m_spriteRenderer->IsDestroyed())
		{
			m_spriteRenderer->SetSprite(m_runtimeController->GetCurrentSprite());
			
			if(m_runtimeController->GetCurrentStateIsAlpha())
			{
				float currentAlpha = m_runtimeController->GetInterpolatedAlpha();
				Color newColor = m_spriteRenderer->GetColor();
				newColor.A = currentAlpha;
				m_spriteRenderer->SetColor(newColor);
			}
		}
		else
		{
			m_spriteRenderer = GetComponent<SpriteRenderer>();
		}
	}
}

void GOTOEngine::Animator::Dispose()
{
	if (IsValidObject(m_controller) 
		&& !m_controller->IsDestroyed()) 
	{ 
		m_controller->DecreaseRefCount(); 
		m_controller = nullptr; 
	}

	if (IsValidObject(m_runtimeController)
		&& !m_runtimeController->IsDestroyed())
	{
		DestroyImmediate(m_runtimeController);
		m_runtimeController = nullptr;
	}

	m_spriteRenderer = nullptr;
}
void GOTOEngine::Animator::SetAnimatorController(AnimatorController* controller)
{
	if (m_controller != controller)
	{
		if (m_runtimeController)
			DestroyImmediate(m_runtimeController);

		if (controller)
		{
			m_runtimeController = new RuntimeAnimatorController(controller);
			controller->IncreaseRefCount();
		}
		else
		{
			m_runtimeController = nullptr;
		}
			
		if (m_controller)
			m_controller->DecreaseRefCount();
	}
}

void GOTOEngine::Animator::SetAnimatorController(const std::wstring& filePath)
{
	SetAnimatorController(Resource::Load<AnimatorController>(filePath));
}

void GOTOEngine::Animator::Play(std::wstring stateName)
{
	if (IsValidObject(m_runtimeController)
		&& !m_runtimeController->IsDestroyed())
	{
		m_runtimeController->ForceChangeState(stateName);
	}
}
