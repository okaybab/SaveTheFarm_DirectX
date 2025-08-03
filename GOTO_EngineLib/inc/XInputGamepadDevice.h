#pragma once
#include "IGamepadDevice.h"
#include <Windows.h>
#include <Xinput.h>

#pragma comment(lib, "xinput.lib")

namespace GOTOEngine
{
    class XInputGamepadDevice : public IGamepadDevice
    {
    public:
        XInputGamepadDevice(int controllerIndex);
        virtual ~XInputGamepadDevice() = default;

        // IGamepadDevice 구현
        bool IsConnected() const override;
        void Update() override;

        bool GetButton(int buttonIndex) const override;
        bool GetButton(GamepadButton button) const override;

        bool GetButtonDown(int buttonIndex) const override;
        bool GetButtonDown(GamepadButton button) const override;
        bool GetButtonUp(int buttonIndex) const override;
        bool GetButtonUp(GamepadButton button) const override;

        // 아날로그 축 입력
        float GetAxis(int axisIndex) const override;
        float GetAxis(GamepadAxis axis) const override;
        float GetAxisRaw(int axisIndex) const override;
        float GetAxisRaw(GamepadAxis axis) const override;

        // 편의 함수들
        Vector2 GetLeftStick() const override;
        Vector2 GetRightStick() const override;
        float GetLeftTrigger() const override;
        float GetRightTrigger() const override;

        // 핫플러그 지원
        void SetConnectionCallback(GamepadConnectionCallback callback) override;
        bool WasJustConnected() const override;
        bool WasJustDisconnected() const override;
        void ClearConnectionEvents() override;

		// 진동 기능
		void SetVibration(float leftMotor, float rightMotor) override;
		void StopVibration() override;
		void PlaySimpleVibration(float duration, float strength) override;

		// 게임패드 인덱스
		int GetGamepadIndex() const override { return m_controllerIndex; }

    private:
        int m_controllerIndex;
        bool m_isConnected;
        bool m_wasConnectedLastFrame;
        bool m_wasJustConnected;
        bool m_wasJustDisconnected;

        XINPUT_STATE m_currentState;
        XINPUT_STATE m_previousState;

        GamepadConnectionCallback m_connectionCallback;

        float m_vibrationTimer = 0.0f;

        // GamepadButton을 XInput 버튼으로 매핑
        WORD GetXInputButton(GamepadButton button) const;
        WORD GetXInputButton(int buttonIndex) const;

        // 버튼 상태 확인 헬퍼
        bool IsButtonPressed(WORD button, const XINPUT_STATE& state) const;

        // 축 값 정규화 (데드존 처리 포함)
        float NormalizeAxis(SHORT value, SHORT deadzone) const;
        float NormalizeAxisWithRaw(SHORT value) const;
        float NormalizeTrigger(BYTE value) const;
        float NormalizeTriggerWithRaw(BYTE value) const;

        // D-Pad 헬퍼 함수들
        float GetDPadX(const XINPUT_GAMEPAD& gamepad) const;
        float GetDPadY(const XINPUT_GAMEPAD& gamepad) const;

        // 핫플러그 처리
        void HandleConnectionStateChange();
        void ResetState();
    };
}