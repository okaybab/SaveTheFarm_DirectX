#pragma once
#include "Resource.h"
#include "AnimationClip.h"
#include <unordered_map>

namespace GOTOEngine
{
	enum class AnimatorParameterType { Int, Float, Bool, Trigger };

	struct AnimatorParameter
	{
		std::wstring name;
		std::string type;
		float defaultFloat = 0.0f;
		int defaultInt = 0; 
		bool defaultBool = false;
	};

	//functor
	struct AnimatorCondition
	{
		std::wstring parameter; // 조건 파라미터 이름
		std::string mode; // 조건 모드 (예: Greater, IfNot 등)
		AnimatorParameterType type; // 파라미터 타입 (예: Int, Float, Bool, Trigger)
		float threshold = 0.0f; // 조건 임계값
	};

	struct AnimatorTransition
	{
		std::wstring fromState;
		std::wstring toState;
		bool hasExitTime = false; // 종료 시간이 있는지 여부
		float exitTime = -1.0f; // 1.0이면 100% 완료 후 전이, -1.0이면 종료 시간 없음
		std::vector<AnimatorCondition> conditions;
	};

	//Load<T>(filePath)로 불러올 수 없음 - 고유 클래스
	class AnimatorState : public Object
	{
	private:
		friend class AnimatorController;
		friend class RuntimeAnimatorController;
		AnimationClip* m_clip;
		std::vector<AnimatorTransition> m_transitions;
		std::wstring m_stateName;
		bool m_isAlpha;

		void Dispose() override
		{
			if (IsValidObject(m_clip)
				&& !m_clip->IsDestroyed())
			{
				m_clip->DecreaseRefCount();
				m_clip = nullptr;
			}
		}
	public:
		AnimatorState(AnimationClip* clip)
			: m_clip(clip)
		{
			if (m_clip)
			{
				m_clip->IncreaseRefCount();
				m_isAlpha = m_clip->IsAlpha();
			}
		}
		const float& GetDuration() const { return m_clip->m_duration; }
		const bool GetIsAlpha() const { return m_isAlpha; }
		const std::wstring& GetStateName() const { return m_stateName; }
		const std::vector<AnimatorTransition>& GetTransitions() const { return m_transitions; }
	};

	class AnimatorController : public Resource
	{
	private:
		friend class ResourceManager;
		friend class RuntimeAnimatorController;
		std::wstring m_defaultState;
		std::unordered_map<std::wstring, AnimatorState*> m_states;
		std::vector<AnimatorParameter> m_parameters;
		std::unordered_map<std::wstring, AnimatorParameterType> m_paramTypes;
		void LoadFromFilePath(const std::wstring& filePath) override;
		bool IsValidData() override { return m_states.size() > 0; }
		AnimatorParameterType stringToParamType(const std::string& type);
	public:
		void Dispose() override;
		AnimatorState* GetState(std::wstring name);
		const std::vector<AnimatorParameter>& GetParameters() const { return m_parameters; }
	};
}