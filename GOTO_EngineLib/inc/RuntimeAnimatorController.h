#pragma once
#include "Object.h"
#include <unordered_map>
#include "AnimatorController.h"
#include "Sprite.h"

namespace GOTOEngine
{
	class RuntimeAnimatorController : public Object
	{
	public:
		// state마다 event 추가
		using EventStateChange = std::function<void(AnimatorState* newState)>;
		using EventAnimationEnd = std::function<void()>;

	private:
		AnimatorController* m_controller;

		std::unordered_map<std::wstring, bool> m_triggers;
		std::unordered_map<std::wstring, bool> m_bools;
		std::unordered_map<std::wstring, int> m_ints;
		std::unordered_map<std::wstring, float> m_floats;

		//state
		AnimatorState* m_currentState;
		EventStateChange m_onStateEnter;
		EventAnimationEnd m_onAnimationEnd;

		float m_time;
		bool m_isFinished;

		bool CheckCondition(const AnimatorCondition& condition);
		void Dispose() override;
		
	public:
		RuntimeAnimatorController(AnimatorController* controller);
		void Update(float deltaTime);
		void ForceChangeState(std::wstring stateName);

		Sprite* GetCurrentSprite();
		const bool GetCurrentStateIsAlpha() { return m_currentState->GetIsAlpha(); }
		const std::wstring& GetCurrentStateName() const 
		{ 
			return m_currentState->GetStateName();
		}
		void SetOnStateEnter(const EventStateChange& callback) { m_onStateEnter = callback; }
		void SetOnAnimationEnd(const EventAnimationEnd& callback) { m_onAnimationEnd = callback; }

		void RegisterTrigger(std::wstring name, bool defaultvalue);
		void RegisterBool(std::wstring name, bool defaultvalue);
		void RegisterInt(std::wstring name, int defaultvalue);
		void RegisterFloat(std::wstring name, float defaultvalue);

		bool GetTrigger(std::wstring name) const { if (m_triggers.find(name) != m_triggers.end()) return m_triggers.at(name); return false; }
		bool GetBool(std::wstring name) const { if (m_bools.find(name) != m_bools.end()) return m_bools.at(name); return false; }
		int GetInt(std::wstring name) const { if (m_ints.find(name) != m_ints.end()) return m_ints.at(name); return 0; }
		float GetFloat(std::wstring name) const { if (m_floats.find(name) != m_floats.end()) return m_floats.at(name); return 0.0f; }
		float GetInterpolatedAlpha() const;

		void SetTrigger(std::wstring name) 
		{ 
			if (m_triggers.find(name) != m_triggers.end()) 
				m_triggers[name] = true; 
		}
		void ResetTrigger(std::wstring name) { if (m_triggers.find(name) != m_triggers.end()) m_triggers[name] = false; }
		void SetBool(std::wstring name, bool value) { if (m_bools.find(name) != m_bools.end()) m_bools[name] = value; }
		void SetInt(std::wstring name, int value) { if (m_ints.find(name) != m_ints.end()) m_ints[name] = value; }
		void SetFloat(std::wstring name, float value) { if (m_floats.find(name) != m_floats.end()) m_floats[name] = value; }
	};
}