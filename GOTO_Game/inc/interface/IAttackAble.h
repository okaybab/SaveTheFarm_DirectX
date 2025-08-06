#pragma once

namespace GOTOEngine
{
	class IAttackAble
	{
	public:
		virtual ~IAttackAble() = default;
		// 공격을 받았을 때 호출되는 함수
		virtual void TakeDamage(float damage) = 0;
	};
}