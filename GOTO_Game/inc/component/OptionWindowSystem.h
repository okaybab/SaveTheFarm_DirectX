#pragma once
#include <ScriptBehaviour.h>
#include <AnimationCurve.h>
#include <InputManager.h>
#include <cstring>
#include <SliderSprite.h>
#include <Texture2D.h>
#include <SpriteRenderer.h>

#include "CrosshairMove.h"
#include "SoundManager.h"

namespace GOTOEngine
{
	class OptionWindowSystem : public ScriptBehaviour
	{
	private:
		AnimationCurve m_openAnimation;
		float m_animationTime = 0.0f; // 0.0f ~ 1.0f
		bool m_isOpen = true;

		GameObject* m_cachedPlayer1 = nullptr;
		GameObject* m_cachedPlayer2 = nullptr;

        CrosshairMove* m_cachedMove1 = nullptr;
        CrosshairMove* m_cachedMove2 = nullptr;

		int m_focusIndex = 0;

        int m_maxButtonCount = 5;

        bool m_YpressedUpTrigger[2];
        bool m_YpressedDownTrigger[2];
        bool m_XpressedRightTrigger[2];
        bool m_XpressedLeftTrigger[2];

        bool m_YstickPressedUp[2];
        bool m_YstickPressedDown[2];
        bool m_XstickPressedRight[2];
        bool m_XstickPressedLeft[2];

        bool m_RightTriggerCheckTrigger[2];
        bool m_RightTriggerPressed[2];

	public:
    OptionWindowSystem()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDisable);
        REGISTER_BEHAVIOUR_MESSAGE(OnEnable);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		Transform* baseWindow = nullptr;
        Transform* focusUITransform = nullptr;
        float openAnimScale = 1.0f;

        SpriteRenderer* baseWindowSprite = nullptr;

        std::array<SliderSprite*, 4> sliderSprites;
        std::array<float, 4> sliderTargetValue;

        SpriteRenderer* exitButtonSprite = nullptr;

		void Awake()
		{
            m_openAnimation = AnimationCurve(std::string{ R"({
    "keyframes": [
        {
            "time": 0,
            "value": 0,
            "in_tangent": 0.3875,
            "out_tangent": -0.5375,
            "tangent_mode": 0
        },
        {
            "time": 0.54,
            "value": 1.2075,
            "in_tangent": -0.3386679358984317,
            "out_tangent": -0.3386679358984317,
            "tangent_mode": 1
        },
        {
            "time": 0.715,
            "value": 0.8725,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.89,
            "value": 1.0675,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 1,
            "value": 1,
            "in_tangent": 1,
            "out_tangent": 0,
            "tangent_mode": 1
        }
    ]
})" });

			m_cachedPlayer1 = GameObject::Find(L"Player1");
			m_cachedPlayer2 = GameObject::Find(L"Player2");

            m_cachedMove1 = m_cachedPlayer1->GetComponent<CrosshairMove>();
            m_cachedMove2 = m_cachedPlayer2->GetComponent<CrosshairMove>();

            for (int i = 0; i < 4; i++)
            {
                switch(i)
                {
                //배경음
                case 0:
                    sliderTargetValue[i] = SoundManager::instance->GetbgmVolume();
                    break;

                //효과음
                case 1:
                    sliderTargetValue[i] = SoundManager::instance->GetsfxVolume();
                    break;
                default:
                    sliderTargetValue[i] = 0.5f;
                }
            }
		}

        void OnEnable()
        {
            m_isOpen = true;

			if(IsValidObject(m_cachedPlayer1))
				m_cachedPlayer1->SetActive(false);
			if (IsValidObject(m_cachedPlayer2))
				m_cachedPlayer2->SetActive(false);
        }

        void OnDisable()
        {
            m_focusIndex = 0;

            if (IsValidObject(m_cachedPlayer1))
                m_cachedPlayer1->SetActive(true);
            if (IsValidObject(m_cachedPlayer2))
                m_cachedPlayer2->SetActive(true);

            std::memset(m_YpressedUpTrigger, false, sizeof(m_YpressedUpTrigger));
            std::memset(m_YpressedDownTrigger, false, sizeof(m_YpressedDownTrigger));
            std::memset(m_XpressedRightTrigger, false, sizeof(m_XpressedRightTrigger));
            std::memset(m_XpressedLeftTrigger, false, sizeof(m_XpressedLeftTrigger));
            std::memset(m_XpressedLeftTrigger, false, sizeof(m_XpressedLeftTrigger));
            std::memset(m_RightTriggerCheckTrigger, false, sizeof(m_RightTriggerCheckTrigger));
            StickPressedCheckReset();
            TriggerPressedCheckReset();
        }

        void TriggerPressedCheck()
        {
            for (int i = 0; i < 2; i++)
            {
                auto currentRightTrigger = INPUT_GET_GAMEPAD_AXIS(i, GamepadAxis::RightTrigger);
                if (!m_RightTriggerCheckTrigger[i])
                {
                    if (currentRightTrigger > 0.89f)
                    {
                        m_RightTriggerCheckTrigger[i] = true;
                        m_RightTriggerPressed[i] = true;
                        return;
                    }
                }
                else if ((m_RightTriggerCheckTrigger[i] && currentRightTrigger < 0.2f))
                {
                    m_RightTriggerCheckTrigger[i] = false;
                }
            }
        }

        void TriggerPressedCheckReset()
        {
            std::memset(m_RightTriggerPressed, false, sizeof(m_RightTriggerPressed));
        }

        void StickPressedCheck()
        {
            for (int i = 0; i < 2; i++)
            {
                auto currentYstick = INPUT_GET_GAMEPAD_AXIS(i, GamepadAxis::LeftStickY);
                if (!m_YpressedUpTrigger[i] && !m_YpressedDownTrigger[i])
                {
                    if (currentYstick > 0.89f)
                    {
                        m_YpressedUpTrigger[i] = true;
                        m_YstickPressedUp[i] = true;
                        return;
                    }

                    if (currentYstick < -0.89f)
                    {
                        m_YpressedDownTrigger[i] = true;
                        m_YstickPressedDown[i] = true;
                        return;
                    }
                }
                else if ((m_YpressedDownTrigger[i] && currentYstick > -0.2f)
                    || (m_YpressedUpTrigger[i] && currentYstick < 0.2f))
                {
                    m_YpressedDownTrigger[i] = false;
                    m_YpressedUpTrigger[i] = false;
                }
            }

            for (int i = 0; i < 2; i++)
            {
                auto currentXstick = INPUT_GET_GAMEPAD_AXIS(i, GamepadAxis::LeftStickX);
                if (!m_XpressedRightTrigger[i] && !m_XpressedLeftTrigger[i])
                {
                    if (currentXstick > 0.89f)
                    {
                        m_XpressedRightTrigger[i] = true;
                        m_XstickPressedRight[i] = true;
                        return;
                    }

                    if (currentXstick < -0.89f)
                    {
                        m_XpressedLeftTrigger[i] = true;
                        m_XstickPressedLeft[i] = true;
                        return;
                    }
                }
                else if ((m_XpressedLeftTrigger[i] && currentXstick > -0.2f)
                    || (m_XpressedRightTrigger[i] && currentXstick < 0.2f))
                {
                    m_XpressedRightTrigger[i] = false;
                    m_XpressedLeftTrigger[i] = false;
                }
            }

        }

        void StickPressedCheckReset()
        {
            std::memset(m_YstickPressedUp, false, sizeof(m_YstickPressedUp));
            std::memset(m_YstickPressedDown, false, sizeof(m_YstickPressedDown));
            std::memset(m_XstickPressedRight, false, sizeof(m_XstickPressedRight));
            std::memset(m_XstickPressedLeft, false, sizeof(m_XstickPressedLeft));
        }
        
        void ApplyBaseWindowAnimation()
        {
            if (IsValidObject(baseWindow))
            {
                // 애니메이션 시간 업데이트
                m_animationTime += (m_isOpen ? TIME_GET_DELTATIME() : -TIME_GET_DELTATIME()) * 1.5f;
                if (m_animationTime > 1.0f)
                    m_animationTime = 1.0f;
                // 애니메이션 값 계산
                float animValue = m_openAnimation.Evaluate(m_animationTime);
                // 윈도우 크기 조정
                baseWindow->SetLocalScale({ openAnimScale * animValue,openAnimScale * animValue });
                //baseWindowSprite->SetColor({ 255,255,255,(static_cast<uint8_t>(255.0f * m_animationTime)) });
                // ESC 키로 창 닫기
                if (InputManager::Get()->GetKeyDown(KeyCode::Escape)
                    || INPUT_GET_GAMEPAD_BUTTONDOWN(0, GamepadButton::ButtonEast)
                    || INPUT_GET_GAMEPAD_BUTTONDOWN(1, GamepadButton::ButtonEast))
                {
                    m_isOpen = false;
                }

                if (!m_isOpen && m_animationTime <= 0.0f)
                {
                    // 애니메이션이 끝나면 창을 비활성화
                    m_animationTime = 0.0f; // 애니메이션 초기화
                    GetGameObject()->SetActive(false);
                    baseWindow->SetLocalScale({ 0, 0 });
                }
            }
        }

        void FocusMove()
        {
            if (IsValidObject(focusUITransform) && m_isOpen)
            {
                // 포커스 인덱스 증가/감소
                if (INPUT_GET_KEYDOWN(KeyCode::DownArrow)
                    || INPUT_GET_KEYDOWN(KeyCode::F)
                    || m_YstickPressedDown[0]
                    || m_YstickPressedDown[1])
                {
                    SoundManager::instance->PlaySFX("Button");

                    m_focusIndex++;
                    if (m_focusIndex > m_maxButtonCount - 1)
                        m_focusIndex = m_maxButtonCount - 1;

                }
                if (InputManager::Get()->GetKeyDown(KeyCode::UpArrow)
                    || INPUT_GET_KEYDOWN(KeyCode::R)
                    || m_YstickPressedUp[0]
                    || m_YstickPressedUp[1])
                {
                    SoundManager::instance->PlaySFX("Button");

                    m_focusIndex--;
                    if (m_focusIndex < 0)
                        m_focusIndex = 0;
                }


                // 포커스 UI 위치 업데이트
                float targetPosY;
                Vector2 targetScale;

                if (m_focusIndex < m_maxButtonCount - 1)
                {
                    auto focusSpace = 150.0f;
                    auto startPosY = 255.0f;

                    targetPosY = startPosY + focusSpace * -m_focusIndex;
                    targetScale = { 1.0f,1.0f };
                }
                else
                {
                    targetPosY = exitButtonSprite->GetTransform()->GetPosition().y - 55.0f;
                    targetScale = { 0.3f, 1.55f };
                }

                auto scale = Vector2::Lerp(focusUITransform->GetLocalScale(), targetScale, 1.0f - std::exp(-10.0f * TIME_GET_DELTATIME()));

                auto pos = Vector2::Lerp(
                    focusUITransform->GetLocalPosition(),
                    { 0.0f, targetPosY },
                    1.0f - std::exp(-10.0f * TIME_GET_DELTATIME()));

                focusUITransform->SetLocalPosition(pos);
                focusUITransform->SetLocalScale(scale);
            }
        }

        void SliderApply()
        {
            if (m_focusIndex >= m_maxButtonCount - 1)
            {
                if (INPUT_GET_KEYDOWN(KeyCode::Enter)
                    || INPUT_GET_GAMEPAD_BUTTONDOWN(0, GamepadButton::ButtonSouth)
                    || INPUT_GET_GAMEPAD_BUTTONDOWN(0, GamepadButton::ButtonSouth)
                    || m_RightTriggerPressed[0]
                    || m_RightTriggerPressed[1])
                {
                    m_isOpen = false;
                }
                return;
            }

            if (IsValidObject(sliderSprites[m_focusIndex]))
            {
                if (INPUT_GET_KEYDOWN(KeyCode::LeftArrow)
                    || m_XstickPressedLeft[0]
                    || m_XstickPressedLeft[1])
                {
                    SoundManager::instance->PlaySFX("Button");
                    sliderTargetValue[m_focusIndex] -= 0.1f;
                }

                if (INPUT_GET_KEYDOWN(KeyCode::RightArrow)
                    || m_XstickPressedRight[0]
                    || m_XstickPressedRight[1])
                {
                    SoundManager::instance->PlaySFX("Button");
                    sliderTargetValue[m_focusIndex] += 0.1f;
                }

                sliderTargetValue[m_focusIndex] = Mathf::Clamp01(sliderTargetValue[m_focusIndex]);
            }

            for (size_t i = 0; i < sliderSprites.size(); i++)
            {
                if(IsValidObject(sliderSprites[i]))
                    sliderSprites[i]->SetValue(Mathf::Lerp(sliderSprites[i]->GetValue(), sliderTargetValue[i], TIME_GET_DELTATIME() * 15.0f));
            }

            switch (m_focusIndex)
            {
            //배경음
            case 0:
                SoundManager::instance->SetBGMVolume(sliderTargetValue[m_focusIndex]);
                break;

            //효과음
            case 1:
                SoundManager::instance->SetSFXVolume(sliderTargetValue[m_focusIndex]);
                break;

            //1P 감도
            case 2:
                m_cachedMove1->moveSpeed = m_cachedMove1->defalutMoveSpeed * 2 * Mathf::Clamp(sliderTargetValue[m_focusIndex], 0.2, 1.0f);
                break;
            //2P 감도
            case 3:
                m_cachedMove2->moveSpeed = m_cachedMove2->defalutMoveSpeed * 2 * Mathf::Clamp(sliderTargetValue[m_focusIndex], 0.2, 1.0f);
                break;
            }
        }

		void Update()
		{
            TriggerPressedCheckReset();
            TriggerPressedCheck();
            StickPressedCheckReset();
            StickPressedCheck();
            ApplyBaseWindowAnimation();
            FocusMove();
            SliderApply();
		}
	};
}