#pragma once
#include <ScriptBehaviour.h>
#include <functional>

#include "GameObject.h"
#include "TimeManager.h"
#include <SpriteRenderer.h>
#include "Image.h"
#include "Mathf.h"

namespace GOTOEngine
{
	class FadeComponent : public ScriptBehaviour
	{
	private :
		float m_timer = 0.0f;
		float m_duration = 1.0f;
		float m_startAlpha = 1.0f;
		float m_endAlpha = 0.0f;

		std::function<void()> m_onComplete;

		// 공통된 것 Color값 SetColor로 알파값 조절하고 있음
		class SpriteRenderer* m_spriteRenderer = nullptr;
		//class image* m_uiImage = nullptr;

	public:
    FadeComponent()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		void Initialize()
		{
			m_spriteRenderer = GetGameObject()->GetComponent<SpriteRenderer>();
		}
		void Awake()
		{
			SetEnabled(false);
		}
		void Update()
		{
			m_timer += TIME_GET_DELTATIME();

			float progress = Mathf::Clamp01(m_timer / m_duration);
			float currentAlpha = Mathf::Lerp(m_startAlpha, m_endAlpha, progress);

			if (m_spriteRenderer != nullptr)
			{
				const Color& originalColor = m_spriteRenderer->GetColor();
				Color modifiedColor = originalColor;
				modifiedColor.A = static_cast<uint8_t>(currentAlpha * 255.0f);
				m_spriteRenderer->SetColor(modifiedColor);
			}
			else
			{
				//std::cout << "FadeComponent Update() m_spriteRenderer is null  " << std::endl;
			}
			/*else if (m_uiImage != nullptr)
			{
				Color color = m_uiImage
				color.A = currentAlpha;
				m_uiImage->SetColor(color);
			}*/

			if (progress >= 1.0f)
			{
				if (m_onComplete)
				{
					m_onComplete();
				}
				SetEnabled(false);
			}
		
		}

		void StartFade(float startAlpha, float endAlpha, float duration, std::function<void()> onComplete = nullptr)
		{
			m_startAlpha = startAlpha;
			m_endAlpha = endAlpha;
			m_duration = duration;
			m_onComplete = onComplete;

			m_timer = 0.0f;

			SetEnabled(true);
		}

		void FadeIn(float duration, std::function<void()> onComplete = nullptr) {
			StartFade(0.0f, 1.0f, duration, onComplete);
		}
		void FadeOut(float duration, std::function<void()> onComplete = nullptr) {
			StartFade(1.0f, 0.0f, duration, onComplete);
		}
	};
}