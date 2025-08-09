#pragma once
#include <ScriptBehaviour.h>
#include <AnimationCurve.h>

#include "CrosshairCollide.h"
#include "CrosshairFire.h"
#include "CrosshairMove.h"
#include "TextRenderer.h"

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
		bool m_animationDone = true;
		float m_animationTime = 0.0f;
		static AnimationCurve s_bigGunAnimationCurve;

		float BulletSpriteStartX(int length, float bias);
	public:
    CrosshairController()
    {
        SetExecutionOrder(1);
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnSceneLoaded);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }

		int id = 0;

		const int& GetBulletCount() const { return m_bulletCount; }

		CrosshairCollide* collide = nullptr;
		CrosshairFire* fire = nullptr;
		CrosshairMove* move = nullptr;
		
		TextRenderer* text = nullptr;

		GameObject* subCrosshairs[3] { nullptr };

		Transform* bulletImageTransforms[5] { nullptr };

		void ChangeType(CrosshairType type);

		void OnSceneLoaded();
		void Awake();
		void Update();

		void OnEnter(CrosshairType type);
		void OnExit(CrosshairType type);

		void OnCrosshairFire();
	};
}