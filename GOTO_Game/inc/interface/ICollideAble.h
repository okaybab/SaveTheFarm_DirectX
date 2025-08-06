#pragma once

namespace GOTOEngine
{
	class GameObject;
	class ICollideAble
	{
	public:
		virtual ~ICollideAble() = default;
		// 공격을 받았을 때 호출되는 함수
		virtual void OnCollide(GameObject* obj) = 0;
	};
}