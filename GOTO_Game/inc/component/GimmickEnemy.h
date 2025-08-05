#pragma once
#include "BaseEnemyObject.h"

#include <SpriteRenderer.h>
#include <Collider2D.h>
#include <Animator.h>

#include "GimmickManager.h"

namespace GOTOEngine
{
	enum E_Gimmick_Enemy_Type
	{
		rabbit,		// 토끼
		squirrel,	// 다람쥐
		thiefmole,	// 도둑두더지
		gimmick_type_count
	};

	class GimmickEnemy : public BaseEnemyObject
	{
		E_Gimmick_Enemy_Type m_gimmickEnemyType;


	public:
		void Dispose() override
		{
			__super::Dispose();

			if (m_isDelayByDispone)
			{
				if (m_layer == 1)
				{
					GameManager::instance->P1Score -= 1;
				}
				else if (m_layer == 2)
				{
					GameManager::instance->P2Score -= 1;
				}
			}
			else
			{
				// 기믹 호출 GimmickManager::GimmickOn()
				GimmickManager::instance->GimmickOn(m_layer, m_gimmickEnemyType + 1);
			}
		}
		void Initialize(std::any param) override
		{
			if (param.type() == typeid(E_Gimmick_Enemy_Type)) m_gimmickEnemyType = std::any_cast<E_Gimmick_Enemy_Type>(param);
		}
		void Awake()
		{
			__super::Awake();

			m_enemyType = E_EnemyType::gimmick;
			m_isMoveLoop = true;
			m_destroyTime = 8.0f;

			switch (m_gimmickEnemyType)
			{
			case rabbit:
				m_moveFlag = 0b1001;
				GetGameObject()->name = L"토끼";
				AddComponent<SpriteRenderer>()->SetSprite(L"../Resources/artResource/Sprint/Rabit.png");
				AddComponent<Animator>()->SetAnimatorController(Resource::Load<AnimatorController>(L"../Resources/Animation/controller/RabbitAnimator_AnimController.json"));
				SetRandomYPosition(-0.4f, -0.1f);
				GetTransform()->SetLossyScale({ 0.2f, 0.2f });
				break;
			case squirrel:
				m_moveFlag = 0b1001;
				GetGameObject()->name = L"다람쥐";
				AddComponent<SpriteRenderer>()->SetSprite(L"../Resources/artResource/Sprint/Squirrel.png");
				AddComponent<Animator>()->SetAnimatorController(Resource::Load<AnimatorController>(L"../Resources/Animation/controller/SquirrelAnimator_AnimController.json"));
				SetRandomYPosition(-0.4f, -0.1f);
				GetTransform()->SetLossyScale({ 0.2f, 0.2f });
				break;
			case thiefmole:
				m_moveFlag = 0b0001;
				GetGameObject()->name = L"도둑두더지";
				AddComponent<SpriteRenderer>()->SetSprite(L"../Resources/artResource/Sprint/ThiefMole.png");
				AddComponent<Animator>()->SetAnimatorController(Resource::Load<AnimatorController>(L"../Resources/Animation/controller/ThiefMoleAnimator_AnimController.json"));
				SetRandomYPosition(-0.4f, -0.1f);
				GetTransform()->SetLossyScale({ 0.12f, 0.12f });
				break;
			}
			GetComponent<SpriteRenderer>()->SetRenderLayer((1 << m_layer));

			auto spriteRect = GetComponent<SpriteRenderer>()->GetSprite()->GetRect();
			auto localScale = GetTransform()->GetLossyScale();
			auto collider = AddComponent<Collider2D>();

			collider->SetSize({ spriteRect.width * localScale.x , spriteRect.height * localScale.y });

			SetMovementComponents();
		}

		void OnBulletDie() override
		{
			__super::OnBulletDie();

			// 죽는 애니메이션 필요

			Destroy(GetGameObject(), 2.0f);
		}
	};
}