#pragma once
#include <ScriptBehaviour.h>
#include <SpriteRenderer.h>
#include <Transform.h>
#include <TimeManager.h>
#include <Texture2D.h>
#include <cmath>

namespace GOTOEngine
{
	class SliderSprite : public ScriptBehaviour
	{
	private:
		float m_value = 1.0f;
	
		Texture2D* m_backgroundTexture = nullptr;
		Texture2D* m_sliderBarTexture = nullptr;
		Texture2D* m_sliderHandleTexture = nullptr;
	public:
    SliderSprite()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }

		SpriteRenderer* backgroundRenderer = nullptr;
		SpriteRenderer* sliderBarRenderer = nullptr;
		SpriteRenderer* sliderHandleRenderer = nullptr;

		int renderOrder = 504;
		float sliderWidth;
		float GetValue() { return m_value; }
		void SetValue(float value) { m_value = Mathf::Clamp01(value); }

		void SetBackgroundTexture(Texture2D* tex)
		{
			if (m_backgroundTexture != tex)
			{
				if (m_backgroundTexture)
					m_backgroundTexture->DecreaseRefCount();
				if (tex)
					tex->IncreaseRefCount();
				m_backgroundTexture = tex;
			}
		}

		void SetSliderBarTexture(Texture2D* tex)
		{
			if (m_sliderBarTexture != tex)
			{
				if (m_sliderBarTexture)
					m_sliderBarTexture->DecreaseRefCount();
				if (tex)
					tex->IncreaseRefCount();
				m_sliderBarTexture = tex;
			}
		}

		void SetSliderHandleTexture(Texture2D* tex)
		{
			if (m_sliderHandleTexture != tex)
			{
				if (m_sliderHandleTexture)
					m_sliderHandleTexture->DecreaseRefCount();
				if (tex)
					tex->IncreaseRefCount();
				m_sliderHandleTexture = tex;
			}
		}

		void Awake()
		{
			//백그라운드 스프라이트 설정
			if (backgroundRenderer)
			{
				auto backgroundSprite = new Sprite();
				backgroundSprite->SetTexture(m_backgroundTexture);
				backgroundSprite->SetPivotX(0.0f);

				if (m_backgroundTexture)
				{
					backgroundSprite->SetRect(m_backgroundTexture->GetRect());
					sliderWidth = backgroundSprite->GetRect().width;
				}

				backgroundRenderer->SetSprite(backgroundSprite);
				backgroundRenderer->SetRenderOrder(renderOrder);
			}

			//게이지 바 스프라이트 설정
			if (sliderBarRenderer)
			{
				auto sliderSprite = new Sprite();
				sliderSprite->SetTexture(m_sliderBarTexture);
				sliderSprite->SetPivotX(0.0f);
				if (sliderSprite->GetTexture())
				{
					sliderSprite->SetRect(m_sliderBarTexture->GetRect());
				}

				sliderBarRenderer->SetSprite(sliderSprite);
				sliderBarRenderer->SetRenderOrder(renderOrder + 1);
			}

			//게이지 핸들 스프라이트 설정
			if (sliderHandleRenderer)
			{
				auto handleSprite = new Sprite();
				handleSprite->SetTexture(m_sliderHandleTexture);
				if (handleSprite->GetTexture())
				{
					handleSprite->SetRect(m_sliderHandleTexture->GetRect());
				}

				sliderHandleRenderer->SetSprite(handleSprite);
				sliderHandleRenderer->SetRenderOrder(renderOrder + 2);

				if (sliderWidth)
				{
					sliderHandleRenderer->GetTransform()->SetLocalPosition({ sliderWidth,0 });
				}
			}
		}

		void Update()
		{
			auto barSprite = sliderBarRenderer->GetSprite();
			auto barTexRect = barSprite->GetTexture()->GetRect();
			barSprite->SetRect({ 0,0,barTexRect.width * m_value,barTexRect.height });
			sliderHandleRenderer->GetTransform()->SetLocalPosition({ sliderWidth * m_value, 0.0f });
		}
	};
}