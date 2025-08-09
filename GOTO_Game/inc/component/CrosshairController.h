#pragma once
#include <ScriptBehaviour.h>
#include <AnimationCurve.h>

#include "CrosshairCollide.h"
#include "CrosshairFire.h"
#include "CrosshairMove.h"

namespace GOTOEngine
{
	enum class CrosshairType
	{
		TriggerGun = 0,
		HoldingGun,
		MachineGun,
		ShotGun,
		BigGun
	};

	class CrosshairController : public ScriptBehaviour
	{
	private:
		CrosshairType m_type = CrosshairType::TriggerGun;
		float m_coolTime;
		int m_bulletCount;
	public:
		CrosshairController()
		{
			SetExecutionOrder(1);
			REGISTER_BEHAVIOUR_MESSAGE(OnSceneLoaded);
			REGISTER_BEHAVIOUR_MESSAGE(Awake);
			REGISTER_BEHAVIOUR_MESSAGE(Update);
		}

		int id = 0;

		const int& GetBulletCount() const { return m_bulletCount; }

		CrosshairCollide* collide = nullptr;
		CrosshairFire* fire = nullptr;
		CrosshairMove* move = nullptr;

		GameObject* subCrosshair[3] { nullptr };

		void ChangeType(CrosshairType type);

		void OnSceneLoaded();
		void Awake();
		void Update();
	};
}