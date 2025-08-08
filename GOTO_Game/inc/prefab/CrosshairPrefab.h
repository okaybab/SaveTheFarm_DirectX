#pragma once
namespace GOTOEngine
{
	class GameObject;
	class CrosshairPrefab
	{
	public:
		static GameObject* CreateCrosshair(int id);
		static GameObject* CreateSubCrosshair(int id);
	};
}
