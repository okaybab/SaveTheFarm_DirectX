#pragma once
#include "Singleton.h"
#include "Vector2.h"
#include <functional>

#define MAX_GAMEPADS 4

namespace GOTOEngine
{
    // 키보드/마우스 전용 KeyCode
    enum class KeyCode
    {
        // 알파벳
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        // 숫자
        Alpha0, Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9,

        // 특수 키
        Escape, Space, Enter, Tab, Backspace, Delete,
        LeftShift, RightShift, LeftControl, RightControl, LeftAlt, RightAlt,
        UpArrow, DownArrow, LeftArrow, RightArrow,

        Comma, Period, Slash, Semicolon, Quote, LeftBracket, RightBracket, Minus, Equals,

        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        // 마우스 버튼
        MouseLeft, MouseRight, MouseMiddle,

        // 기타
        Unknown,
    };

    // 게임패드 버튼 (플랫폼 독립적)
    enum class GamepadButton
    {
        ButtonSouth = 0,  // A / Cross
        ButtonEast,       // B / Circle  
        ButtonNorth,      // Y / Triangle
        ButtonWest,       // X / Square
        ButtonL1,         // Left Shoulder
        ButtonR1,         // Right Shoulder
        ButtonStart,      // Start/Options/Menu
        ButtonSelect,     // Back/Share/View
        DPadUp,           // D-Pad 위
        DPadDown,         // D-Pad 아래
        DPadLeft,         // D-Pad 왼쪽
        DPadRight,        // D-Pad 오른쪽
        Count
    };

    // 게임패드 축 (플랫폼 독립적)
    enum class GamepadAxis
    {
        LeftStickX = 0,
        LeftStickY,
        RightStickX,
        RightStickY,
        LeftTrigger,
        RightTrigger,
        DPadX,          // D-Pad 가로축 (-1: 왼쪽, 0: 중립, +1: 오른쪽)
        DPadY,          // D-Pad 세로축 (-1: 아래, 0: 중립, +1: 위)
        Count
    };

    // 게임패드 연결/해제 이벤트 콜백
    using GamepadConnectionCallback = std::function<void(int gamepadIndex, bool connected)>;

    class IInputSystem;
    class InputManager : public Singleton<InputManager>
    {
    public:
        InputManager() = default;
        ~InputManager() = default;

        void Startup(void* windowHandle);
        void Shutdown();
        void Update();

        // === 키보드 및 마우스 입력 ===
        Vector2 GetMousePos();
        bool GetKey(KeyCode keyCode);
        bool GetKeyDown(KeyCode keyCode);
        bool GetKeyUp(KeyCode keyCode);

        // === 게임패드 입력 ===
        // 버튼 입력
        bool GetGamepadButton(int gamepadIndex, GamepadButton button);
        bool GetGamepadButtonDown(int gamepadIndex, GamepadButton button);
        bool GetGamepadButtonUp(int gamepadIndex, GamepadButton button);

        // 아날로그 축 입력
        float GetGamepadAxis(int gamepadIndex, GamepadAxis axis);
        float GetGamepadAxisRaw(int gamepadIdex, GamepadAxis axis);
        Vector2 GetLeftStick(int gamepadIndex);   // 왼쪽 스틱 (X, Y)
        Vector2 GetRightStick(int gamepadIndex);  // 오른쪽 스틱 (X, Y)
        float GetLeftTrigger(int gamepadIndex);   // 왼쪽 트리거 (0.0f ~ 1.0f)
        float GetRightTrigger(int gamepadIndex);  // 오른쪽 트리거 (0.0f ~ 1.0f)

        // D-Pad 편의 함수들
        Vector2 GetDPad(int gamepadIndex);           // D-Pad 축 (X, Y)
        float GetDPadX(int gamepadIndex);            // D-Pad 가로축 (-1, 0, +1)
        float GetDPadY(int gamepadIndex);            // D-Pad 세로축 (-1, 0, +1)

        // === 게임패드 상태 정보 ===
        bool IsGamepadConnected(int gamepadIndex);
        int GetConnectedGamepadCount();
        int GetFirstConnectedGamepad();

        // === 핫플러그 지원 ===
        void SetGamepadConnectionCallback(GamepadConnectionCallback callback);
        bool WasGamepadJustConnected(int gamepadIndex);
        bool WasGamepadJustDisconnected(int gamepadIndex);
        void ClearGamepadConnectionEvents();

		// === 게임패드 진동 기능 ===
		void SetGamepadVibration(int gamepadIndex, float leftMotor, float rightMotor);
		void StopGamepadVibration(int gamepadIndex);
		void PlaySimpleGamepadVibration(int gamepadIndex, float duration, float strength);
    private:
        IInputSystem* m_inputSystem = nullptr;
        GamepadConnectionCallback m_connectionCallback;
    };
}

// === 키보드/마우스 입력 매크로 ===
#define INPUT_GET_KEY GOTOEngine::InputManager::Get()->GetKey
#define INPUT_GET_KEYDOWN GOTOEngine::InputManager::Get()->GetKeyDown
#define INPUT_GET_KEYUP GOTOEngine::InputManager::Get()->GetKeyUp
#define INPUT_GET_MOUSEPOS GOTOEngine::InputManager::Get()->GetMousePos

// === 게임패드 입력 매크로 ===
#define INPUT_GET_GAMEPAD_BUTTON(index, button) GOTOEngine::InputManager::Get()->GetGamepadButton(index, button)
#define INPUT_GET_GAMEPAD_BUTTONDOWN(index, button) GOTOEngine::InputManager::Get()->GetGamepadButtonDown(index, button)
#define INPUT_GET_GAMEPAD_BUTTONUP(index, button) GOTOEngine::InputManager::Get()->GetGamepadButtonUp(index, button)
#define INPUT_GET_GAMEPAD_AXIS(index, axis) GOTOEngine::InputManager::Get()->GetGamepadAxis(index, axis)
#define INPUT_GET_GAMEPAD_AXISRAW(index, axis) GOTOEngine::InputManager::Get()->GetGamepadAxisRaw(index, axis)
#define INPUT_GAMEPAD_IS_CONNECTED(index) GOTOEngine::InputManager::Get()->IsGamepadConnected(index)
#define INPUT_GET_LEFTSTICK(index) GOTOEngine::InputManager::Get()->GetLeftStick(index)
#define INPUT_GET_RIGHTSTICK(index) GOTOEngine::InputManager::Get()->GetRightStick(index)
#define INPUT_GET_LEFTTRIGGER(index) GOTOEngine::InputManager::Get()->GetLeftTrigger(index)
#define INPUT_GET_RIGHTTRIGGER(index) GOTOEngine::InputManager::Get()->GetRightTrigger(index)

// === D-Pad 입력 매크로 ===
#define INPUT_GET_DPAD(index) GOTOEngine::InputManager::Get()->GetDPad(index)
#define INPUT_GET_DPAD_X(index) GOTOEngine::InputManager::Get()->GetDPadX(index)
#define INPUT_GET_DPAD_Y(index) GOTOEngine::InputManager::Get()->GetDPadY(index)

// === 핫플러그 매크로 ===
#define INPUT_GAMEPAD_JUST_CONNECTED(index) GOTOEngine::InputManager::Get()->WasGamepadJustConnected(index)
#define INPUT_GAMEPAD_JUST_DISCONNECTED(index) GOTOEngine::InputManager::Get()->WasGamepadJustDisconnected(index)

// === 게임패드 진동 매크로 ===
#define INPUT_SET_GAMEPAD_VIBRATION(index, leftMotor, rightMotor) GOTOEngine::InputManager::Get()->SetGamepadVibration(index, leftMotor, rightMotor)
#define INPUT_STOP_GAMEPAD_VIBRATION(index) GOTOEngine::InputManager::Get()->StopGamepadVibration(index)
#define INPUT_PLAY_SIMPLE_GAMEPAD_VIBRATION(index, duration, strength) GOTOEngine::InputManager::Get()->PlaySimpleGamepadVibration(index, duration, strength)
