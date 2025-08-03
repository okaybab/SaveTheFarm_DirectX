#include "XInputGamepadDevice.h"
#include "Vector2.h"
#include <cstring>
#include <cmath>
#include "TimeManager.h"
#include "InputManager.h" 
#include "Mathf.h"

namespace GOTOEngine
{
    XInputGamepadDevice::XInputGamepadDevice(int controllerIndex)
        : m_controllerIndex(controllerIndex)
        , m_isConnected(false)
        , m_wasConnectedLastFrame(false)
        , m_wasJustConnected(false)
        , m_wasJustDisconnected(false)
        , m_connectionCallback(nullptr)
    {
        ResetState();

    }

    bool XInputGamepadDevice::IsConnected() const
    {
        return m_isConnected;
    }

    void XInputGamepadDevice::Update()
    {
        // 이전 프레임 연결 상태 저장
        m_wasConnectedLastFrame = m_isConnected;

        // 이전 입력 상태 저장
        m_previousState = m_currentState;

        // 현재 상태 가져오기
        DWORD result = XInputGetState(m_controllerIndex, &m_currentState);
        m_isConnected = (result == ERROR_SUCCESS);

        // 핫플러그 상태 변화 처리
        HandleConnectionStateChange();

        if (!m_isConnected)
        {
            // 연결되지 않은 경우 입력 상태 초기화
            memset(&m_currentState, 0, sizeof(XINPUT_STATE));
        }

		m_vibrationTimer -= TIME_GET_DELTATIME();
		m_vibrationTimer = Mathf::Max(m_vibrationTimer, 0.0f);
		if (m_vibrationTimer <= 0.0f)
		{
			// 진동 타이머가 0이 되면 진동 중지
			XINPUT_VIBRATION vibration = { 0, 0 };
			XInputSetState(m_controllerIndex, &vibration);
		}
    }

    void XInputGamepadDevice::HandleConnectionStateChange()
    {
        // 연결 상태 변화 감지
        m_wasJustConnected = (!m_wasConnectedLastFrame && m_isConnected);
        m_wasJustDisconnected = (m_wasConnectedLastFrame && !m_isConnected);

        // 연결된 경우
        if (m_wasJustConnected)
        {
#ifdef _DEBUG
            printf("Gamepad %d connected\n", m_controllerIndex);
#endif
            ResetState(); // 상태 초기화

            if (m_connectionCallback)
            {
                m_connectionCallback(m_controllerIndex, true);
            }
        }

        // 연결 해제된 경우
        if (m_wasJustDisconnected)
        {
#ifdef _DEBUG
            printf("Gamepad %d disconnected\n", m_controllerIndex);
#endif
            ResetState(); // 상태 초기화

            if (m_connectionCallback)
            {
                m_connectionCallback(m_controllerIndex, false);
            }
        }
    }

    void XInputGamepadDevice::ResetState()
    {
        memset(&m_currentState, 0, sizeof(XINPUT_STATE));
        memset(&m_previousState, 0, sizeof(XINPUT_STATE));
    }

    void XInputGamepadDevice::SetConnectionCallback(GamepadConnectionCallback callback)
    {
        m_connectionCallback = callback;
    }

    bool XInputGamepadDevice::WasJustConnected() const
    {
        return m_wasJustConnected;
    }

    bool XInputGamepadDevice::WasJustDisconnected() const
    {
        return m_wasJustDisconnected;
    }

    void XInputGamepadDevice::ClearConnectionEvents()
    {
        m_wasJustConnected = false;
        m_wasJustDisconnected = false;
    }

    void XInputGamepadDevice::SetVibration(float leftMotor, float rightMotor)
    {
        if (!m_isConnected)
            return;
        XINPUT_VIBRATION vibration;
        vibration.wLeftMotorSpeed = static_cast<WORD>(leftMotor * 65535.0f);
        vibration.wRightMotorSpeed = static_cast<WORD>(rightMotor * 65535.0f);
        XInputSetState(m_controllerIndex, &vibration);
    }

    void XInputGamepadDevice::StopVibration()
    {
        if (!m_isConnected)
            return;
        XINPUT_VIBRATION vibration;
        vibration.wLeftMotorSpeed = static_cast<WORD>(0.0f);
        vibration.wRightMotorSpeed = static_cast<WORD>(0.0f);
        XInputSetState(m_controllerIndex, &vibration);
    }

    void XInputGamepadDevice::PlaySimpleVibration(float duration, float strength)
    {
		if (!m_isConnected)
			return;
		XINPUT_VIBRATION vibration;
		vibration.wLeftMotorSpeed = static_cast<WORD>(strength * 65535.0f);
		vibration.wRightMotorSpeed = static_cast<WORD>(strength * 65535.0f);
		XInputSetState(m_controllerIndex, &vibration);
		// 진동 타이머 설정
		m_vibrationTimer = duration;
    }

    bool XInputGamepadDevice::GetButton(int buttonIndex) const
    {
        if (!m_isConnected || buttonIndex < 0 || buttonIndex >= static_cast<int>(GamepadButton::Count))
            return false;

        WORD xinputButton = GetXInputButton(buttonIndex);
        return IsButtonPressed(xinputButton, m_currentState);
    }

    bool XInputGamepadDevice::GetButton(GamepadButton button) const
    {
        if (!m_isConnected)
            return false;

        WORD xinputButton = GetXInputButton(button);
        return IsButtonPressed(xinputButton, m_currentState);
    }

    bool XInputGamepadDevice::GetButtonDown(int buttonIndex) const
    {
        if (!m_isConnected || buttonIndex < 0 || buttonIndex >= static_cast<int>(GamepadButton::Count))
            return false;

        WORD xinputButton = GetXInputButton(buttonIndex);
        return !IsButtonPressed(xinputButton, m_previousState) &&
            IsButtonPressed(xinputButton, m_currentState);
    }

    bool XInputGamepadDevice::GetButtonDown(GamepadButton button) const
    {
        if (!m_isConnected)
            return false;

        WORD xinputButton = GetXInputButton(button);
        return !IsButtonPressed(xinputButton, m_previousState) &&
            IsButtonPressed(xinputButton, m_currentState);
    }

    bool XInputGamepadDevice::GetButtonUp(int buttonIndex) const
    {
        if (!m_isConnected || buttonIndex < 0 || buttonIndex >= static_cast<int>(GamepadButton::Count))
            return false;

        WORD xinputButton = GetXInputButton(buttonIndex);
        return IsButtonPressed(xinputButton, m_previousState) &&
            !IsButtonPressed(xinputButton, m_currentState);
    }

    bool XInputGamepadDevice::GetButtonUp(GamepadButton button) const
    {
        if (!m_isConnected)
            return false;

        WORD xinputButton = GetXInputButton(button);
        return IsButtonPressed(xinputButton, m_previousState) &&
            !IsButtonPressed(xinputButton, m_currentState);
    }

    float XInputGamepadDevice::GetAxis(int axisIndex) const
    {
        if (!m_isConnected || axisIndex < 0 || axisIndex >= static_cast<int>(GamepadAxis::Count))
            return 0.0f;

        return GetAxis(static_cast<GamepadAxis>(axisIndex));
    }

    float XInputGamepadDevice::GetAxis(GamepadAxis axis) const
    {
        if (!m_isConnected)
            return 0.0f;

        const XINPUT_GAMEPAD& gamepad = m_currentState.Gamepad;

        switch (axis)
        {
        case GamepadAxis::LeftStickX:
            return NormalizeAxis(gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        case GamepadAxis::LeftStickY:
            return NormalizeAxis(gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        case GamepadAxis::RightStickX:
            return NormalizeAxis(gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        case GamepadAxis::RightStickY:
            return NormalizeAxis(gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        case GamepadAxis::LeftTrigger:
            return NormalizeTrigger(gamepad.bLeftTrigger);
        case GamepadAxis::RightTrigger:
            return NormalizeTrigger(gamepad.bRightTrigger);
        default:
            return 0.0f;
        }
    }

    float XInputGamepadDevice::GetAxisRaw(int axisIndex) const
    {
        if (!m_isConnected || axisIndex < 0 || axisIndex >= static_cast<int>(GamepadAxis::Count))
            return 0.0f;

		return GetAxisRaw(static_cast<GamepadAxis>(axisIndex));
    }

    float XInputGamepadDevice::GetAxisRaw(GamepadAxis axis) const
    {
        if (!m_isConnected)
            return 0.0f;

        const XINPUT_GAMEPAD& gamepad = m_currentState.Gamepad;

        switch (axis)
        {
        case GamepadAxis::LeftStickX:
            return NormalizeAxisWithRaw(gamepad.sThumbLX);
        case GamepadAxis::LeftStickY:
            return NormalizeAxisWithRaw(gamepad.sThumbLY);
        case GamepadAxis::RightStickX:
            return NormalizeAxisWithRaw(gamepad.sThumbRX);
        case GamepadAxis::RightStickY:
            return NormalizeAxisWithRaw(gamepad.sThumbRY);
        case GamepadAxis::LeftTrigger:
            return NormalizeTriggerWithRaw(gamepad.bLeftTrigger);
        case GamepadAxis::RightTrigger:
            return NormalizeTriggerWithRaw(gamepad.bRightTrigger);
        default:
            return 0.0f;
        }
    }

    Vector2 XInputGamepadDevice::GetLeftStick() const
    {
        return Vector2{
            GetAxis(GamepadAxis::LeftStickX),
            GetAxis(GamepadAxis::LeftStickY)
        };
    }

    Vector2 XInputGamepadDevice::GetRightStick() const
    {
        return Vector2{
            GetAxis(GamepadAxis::RightStickX),
            GetAxis(GamepadAxis::RightStickY)
        };
    }

    float XInputGamepadDevice::GetLeftTrigger() const
    {
        return GetAxis(GamepadAxis::LeftTrigger);
    }

    float XInputGamepadDevice::GetRightTrigger() const
    {
        return GetAxis(GamepadAxis::RightTrigger);
    }

    WORD XInputGamepadDevice::GetXInputButton(GamepadButton button) const
    {
        switch (button)
        {
        case GamepadButton::ButtonSouth:  return XINPUT_GAMEPAD_A;
        case GamepadButton::ButtonEast:   return XINPUT_GAMEPAD_B;
        case GamepadButton::ButtonNorth:  return XINPUT_GAMEPAD_Y;
        case GamepadButton::ButtonWest:   return XINPUT_GAMEPAD_X;
        case GamepadButton::ButtonL1:     return XINPUT_GAMEPAD_LEFT_SHOULDER;
        case GamepadButton::ButtonR1:     return XINPUT_GAMEPAD_RIGHT_SHOULDER;
        case GamepadButton::ButtonStart:  return XINPUT_GAMEPAD_START;
        case GamepadButton::ButtonSelect: return XINPUT_GAMEPAD_BACK;
        case GamepadButton::DPadUp:       return XINPUT_GAMEPAD_DPAD_UP;
        case GamepadButton::DPadDown:     return XINPUT_GAMEPAD_DPAD_DOWN;
        case GamepadButton::DPadLeft:     return XINPUT_GAMEPAD_DPAD_LEFT;
        case GamepadButton::DPadRight:    return XINPUT_GAMEPAD_DPAD_RIGHT;
        default: return 0;
        }
    }

    WORD XInputGamepadDevice::GetXInputButton(int buttonIndex) const
    {
        if (buttonIndex < 0 || buttonIndex >= static_cast<int>(GamepadButton::Count))
            return 0;

        return GetXInputButton(static_cast<GamepadButton>(buttonIndex));
    }

    bool XInputGamepadDevice::IsButtonPressed(WORD button, const XINPUT_STATE& state) const
    {
        return (state.Gamepad.wButtons & button) != 0;
    }

    float XInputGamepadDevice::NormalizeAxis(SHORT value, SHORT deadzone) const
    {
        const float max = 32767.0f;
        if (std::abs(value) < deadzone)
            return 0.0f;

        // 부호 보존
        float norm = (std::abs(value) - deadzone) / (max - deadzone);
        return (value < 0 ? -norm : norm);
    }

    float XInputGamepadDevice::NormalizeAxisWithRaw(SHORT value) const
    {
        const float max = 32767.0f;

        // 부호 보존
        float norm = (std::abs(value)) / (max);
        return (value < 0 ? -norm : norm);
    }

    float XInputGamepadDevice::NormalizeTrigger(BYTE value) const
    {
        const BYTE deadzone = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
        if (value < deadzone)
            return 0.0f;

        return static_cast<float>(value - deadzone) / (255.0f - deadzone);
    }

    float XInputGamepadDevice::NormalizeTriggerWithRaw(BYTE value) const
    {
        return static_cast<float>(value) / (255.0f);
    }

    float XInputGamepadDevice::GetDPadX(const XINPUT_GAMEPAD& gamepad) const
    {
        float value = 0.0f;

        if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
            value -= 1.0f;
        if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
            value += 1.0f;

        return value;
    }

    float XInputGamepadDevice::GetDPadY(const XINPUT_GAMEPAD& gamepad) const
    {
        float value = 0.0f;

        if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
            value -= 1.0f;
        if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
            value += 1.0f;

        return value;
    }
}