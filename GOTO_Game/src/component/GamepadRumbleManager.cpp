#include "GamepadRumbleManager.h"
#include <TimeManager.h>
#include <Mathf.h>
#include <InputManager.h>

GOTOEngine::GamepadRumbleManager* GOTOEngine::GamepadRumbleManager::instance = nullptr;

void GOTOEngine::GamepadRumbleManager::Awake()
{
	if (instance == nullptr)
	{
		instance = this;
		DontDestroyOnLoad(this);
	}
	else
	{
		Destroy(GetGameObject());
	}
}

int GOTOEngine::GamepadRumbleManager::Play(int padID, const RumbleAnimationClip& rumbleClip, float weight)
{
    int currentRumbleID = m_nextRumbleID++;
	m_activeRumbles.emplace_back(ActiveRumble{ currentRumbleID, padID,rumbleClip,weight });

    return currentRumbleID;
}

void GOTOEngine::GamepadRumbleManager::Stop(int rumbleID)
{
    // rumbleID와 일치하는 ActiveRumble을 찾아 제거
    auto it = std::remove_if(m_activeRumbles.begin(), m_activeRumbles.end(),
        [rumbleID](const ActiveRumble& active) {
            return active.rumbleID == rumbleID;
        });

    if (it != m_activeRumbles.end()) {
        m_activeRumbles.erase(it, m_activeRumbles.end());
    }
}

void GOTOEngine::GamepadRumbleManager::ClearFinishedClips()
{
    // isFinished가 true인 클립들을 배열에서 제거
    auto newEnd = std::remove_if(m_activeRumbles.begin(), m_activeRumbles.end(),
        [](const ActiveRumble& rumble) {
            return rumble.isFinished;
        });

    m_activeRumbles.erase(newEnd, m_activeRumbles.end());
}

void GOTOEngine::GamepadRumbleManager::Update()
{
    float finalRightMotor[4] { 0.0f, };
    float finalLeftMotor[4] { 0.0f, };
    float totalWeight[4]{ 0.0f, };

    // 현재 재생 중인 모든 진동 클립을 순회
    for (auto& active : m_activeRumbles)
    {
        // 1. 시간 업데이트
        active.elapsedTime += TIME_GET_DELTATIME();

        // 2. 블렌딩을 위한 가중치 계산 (필요에 따라 복잡하게 만들 수 있음)
        // 예를 들어, 클립이 끝날 때 페이드 아웃(Fade-out) 효과를 줄 수 있습니다.
        float currentWeight = active.weight;

        // 3. 진동 값 샘플링
        float clampedTime = Mathf::Clamp(active.elapsedTime, 0.0f, active.clip.length);

        // 재생이 끝났는지 확인하고 플래그 설정
        if (active.elapsedTime >= active.clip.length)
        {
            active.isFinished = true;
        }

        float currentRight = active.clip.rightMotorCurve.Evaluate(clampedTime);
        float currentLeft = active.clip.leftMotorCurve.Evaluate(clampedTime);

        // 4. 가중치를 곱해 최종 값에 더하기
        finalRightMotor[active.padID] += Mathf::Clamp01(currentRight) * currentWeight;
        finalLeftMotor[active.padID] += Mathf::Clamp01(currentLeft) * currentWeight;
        totalWeight[active.padID] += currentWeight;
    }

    ClearFinishedClips();


    for (int i = 0; i < 4; ++i)
    {
        // 5. 정규화(Normalize)
        if (totalWeight[i] > 0.0f)
        {
            finalRightMotor[i] /= totalWeight[i];
            finalLeftMotor[i] /= totalWeight[i];
        }

        // 6. 최종 진동 값을 게임패드에 적용
        INPUT_SET_GAMEPAD_VIBRATION(i, finalLeftMotor[i], finalRightMotor[i]);
    }
}
