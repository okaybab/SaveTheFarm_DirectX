#include "RuntimeAnimatorController.h"
#include "AnimationClip.h"
#include "Mathf.h"

bool GOTOEngine::RuntimeAnimatorController::CheckCondition(const AnimatorCondition& condition)
{
	std::wstring param = condition.parameter;
	std::string mode = condition.mode;
	float threshold = condition.threshold;

	switch (condition.type)
	{
	case AnimatorParameterType::Bool:
	{
		bool val = GetBool(param);
		if (mode == "If") return val;
		if (mode == "IfNot") return !val;
		break;
	}
	case AnimatorParameterType::Int:
	{
		int val = GetInt(param);
		if (mode == "Greater") return val > threshold;
		if (mode == "Less") return val < threshold;
		if (mode == "Equals") return val == static_cast<int>(threshold);
		if (mode == "NotEqual") return val != static_cast<int>(threshold);
		break;
	}
	case AnimatorParameterType::Float:
	{
		float val = GetFloat(param);
		if (mode == "Greater") return val > threshold;
		if (mode == "Less") return val < threshold;
		if (mode == "Equals") return val == threshold;
		if (mode == "NotEqual") return val != threshold;
		break;
	}
	case AnimatorParameterType::Trigger:
	{
		bool val = GetTrigger(param);
		if (mode == "If") return val;
		if (mode == "IfNot") return !val;
		break;
	}
	default:
		return false;
	}
	return false;
}
void GOTOEngine::RuntimeAnimatorController::Dispose()
{
	if(IsValidObject(m_controller) && !m_controller->IsDestroyed())
		m_controller->DecreaseRefCount();
}

GOTOEngine::RuntimeAnimatorController::RuntimeAnimatorController(AnimatorController* controller)
{
	//지역변수 초기화
	m_controller = controller;
	m_currentState = nullptr;

	if (m_controller)
	{
		m_controller->IncreaseRefCount();
		for (auto param : m_controller->m_parameters)
		{
			if (param.type == "Trigger") {
				RegisterTrigger(param.name, param.defaultBool);
			}
			else if (param.type == "Bool") {
				RegisterBool(param.name, param.defaultBool);
			}
			else if (param.type == "Int") {
				RegisterInt(param.name, param.defaultInt);
			}
			else if (param.type == "Float") {
				RegisterFloat(param.name, param.defaultFloat);
			}
		}

		m_currentState = m_controller->GetState(m_controller->m_defaultState);
	}

	m_time = 0.0f;
}

void GOTOEngine::RuntimeAnimatorController::Update(float deltaTime)
{
	auto currentClip = m_currentState->m_clip;

	if (currentClip && !currentClip->GetKeyframes().empty())
		m_time += deltaTime;

	float currentClipDuration = currentClip->GetDuration();

	if (currentClip->IsLoop()) {
		if (currentClipDuration > 0.0f) {
			m_time = std::fmod(m_time, currentClipDuration);
		}
	}
	else {
		if (m_time > currentClipDuration) {
			m_time = currentClipDuration;
		}
	}

	float normalizedTime = (currentClipDuration > 0.0f) ? (m_time / currentClipDuration) : 0.0f;

	for (const auto& transition : m_currentState->GetTransitions())
	{
		// 1. ExitTime 조건 확인
		if (transition.hasExitTime)
		{
			if (normalizedTime < transition.exitTime)
				continue; // 아직 전이 가능하지 않음
		}

		// 2. 전이 조건 확인
		bool allConditionsMet = true;
		std::vector<std::wstring> savedTriggers;
		for (const auto& condition : transition.conditions)
		{
			if (!CheckCondition(condition)) {
				allConditionsMet = false;
				break;
			}

			if (condition.type == AnimatorParameterType::Trigger)
			{
				savedTriggers.emplace_back(condition.parameter);
			}
		}

		if (allConditionsMet)
		{
			for (auto& trigger : savedTriggers)
			{
				ResetTrigger(trigger);
			}

			m_currentState = m_controller->GetState(transition.toState);
			m_time = 0.0f; // 애니메이션 시간 초기화
			break;
		}
	}
}

GOTOEngine::Sprite* GOTOEngine::RuntimeAnimatorController::GetCurrentSprite()
{
	auto currentKeyframes = m_currentState->m_clip->GetKeyframes();

	// 뒤에서부터 검색하여 현재 시간보다 작거나 같은 프레임을 찾음
	for (int i = static_cast<int>(currentKeyframes.size()) - 1; i >= 0; --i) {
		if (m_time >= currentKeyframes[i]->GetTime()) {
			return currentKeyframes[i]->GetSprite();
		}
	}
	//0번째 이미지 출력
	return currentKeyframes.front()->GetSprite();
}

void GOTOEngine::RuntimeAnimatorController::RegisterTrigger(std::wstring name, bool defaultvalue)
{
	m_triggers[name] = defaultvalue;
}

void GOTOEngine::RuntimeAnimatorController::RegisterBool(std::wstring name, bool defaultvalue)
{
	m_bools[name] = defaultvalue;
}

void GOTOEngine::RuntimeAnimatorController::RegisterInt(std::wstring name, int defaultvalue)
{
	m_ints[name] = defaultvalue;
}

void GOTOEngine::RuntimeAnimatorController::RegisterFloat(std::wstring name, float defaultvalue)
{
	m_floats[name] = defaultvalue;
}

float GOTOEngine::RuntimeAnimatorController::GetInterpolatedAlpha() const
{
	auto& keyframes = m_currentState->m_clip->GetKeyframes();
	if (keyframes.empty())
	{
		return 1.0f;
	}

	// 키프레임이 하나뿐이거나, 애니메이션 시작 전이면 첫 프레임의 알파를 반환
	if (keyframes.size() == 1 || m_time <= keyframes.front()->GetTime())
	{
		return keyframes.front()->GetAlpha();
	}

	// 애니메이션 재생 시간을 넘어섰으면 마지막 프레임의 알파를 반환
	if (m_time >= keyframes.back()->GetTime())
	{
		return keyframes.back()->GetAlpha();
	}

	// 현재 시간(m_time)이 어떤 두 키프레임 사이에 있는지 찾음
	AnimationKeyframe* startFrame = nullptr;
	AnimationKeyframe* endFrame = nullptr;
	for (size_t i = 0; i < keyframes.size() - 1; ++i)
	{
		if (m_time >= keyframes[i]->GetTime() && m_time <= keyframes[i + 1]->GetTime())
		{
			startFrame = keyframes[i];
			endFrame = keyframes[i + 1];
			break;
		}
	}

	// 두 프레임 사이를 찾았다면, 선형 보간(Lerp)을 수행
	if (startFrame && endFrame)
	{
		float segmentDuration = endFrame->GetTime() - startFrame->GetTime();
		if (segmentDuration <= 0.0f)
		{
			// 시간이 같으면 시작 프레임 값 사용
			return startFrame->GetAlpha();
		}

		// 현재 구간에서의 진행률(0.0 ~ 1.0)을 계산
		float progress = (m_time - startFrame->GetTime()) / segmentDuration;

		// 시작 알파와 끝 알파 사이를 보간한 값을 반환
		return Mathf::Lerp(startFrame->GetAlpha(), endFrame->GetAlpha(), progress);
	}

	// 예외적인 경우, 마지막 프레임의 알파를 반환
	return keyframes.back()->GetAlpha();
}
