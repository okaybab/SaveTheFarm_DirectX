#pragma once
#include "Component.h"
#include "PhysicsManager.h"
#include "Vector2.h"
#include "box2d-lite/Body.h"

namespace GOTOEngine
{
	class Collider2D : public Component
	{
	private:
		friend class PhysicsManager;
		friend class PhysicsManager::Body2DWrapper;
		PhysicsManager::Body2DWrapper* m_wrapperBody;

		PhysicsManager::Body2DWrapper* GetWrapperBody() { return m_wrapperBody; };

		~Collider2D();

		void AdditionalInitialize() override;
	public:
		Collider2D();

		Vector2 GetSize() 
		{ 
			 if(GetWrapperBody() && GetWrapperBody()->m_pBody )
				 return  { GetWrapperBody()->m_pBody->width.x,m_wrapperBody->m_pBody->width.y};

			 return { 0,0 };
		}

		bool GetIsTrigger()
		{
			if (GetWrapperBody() && GetWrapperBody()->m_pBody)
				return  { GetWrapperBody()->m_pBody->isTrigger };

			return false;
		}

		void SetSize(const Vector2& size) 
		{ 
			if (GetWrapperBody() && GetWrapperBody()->m_pBody)
				GetWrapperBody()->m_pBody->width = { size.x, size.y };
		}

		void SetIsTrigger(bool value)
		{
			if (GetWrapperBody() && GetWrapperBody()->m_pBody)
				GetWrapperBody()->m_pBody->isTrigger = value;
		}
	};
}