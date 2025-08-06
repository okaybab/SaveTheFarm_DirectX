#pragma once
#include <ScriptBehaviour.h>
#include <TimeManager.h>
#include <InputManager.h>
#include <RenderManager.h>
#include <SpriteRenderer.h>
#include <Animator.h>
#include "Canvas.h"
#include "Image.h"
#include "RectTransform.h"
#include "Screen.h"

namespace GOTOEngine
{
	enum class ItemType {
		Bomb = 0, Icebomb, Mushroom
	};
	class AnimationCurve;
	class ItemManager2 : public ScriptBehaviour
	{
	private:
		std::vector<ItemType> p1Items;
		std::vector<ItemType> p2Items;
		float IceTimer = 0.0f;
		float p1MushroomTimer = 0.0f;
		float p2MushroomTimer = 0.0f;
		Image* itembar1;
		Image* itembar2;
		Image* p1itemImage[7];
		Image* p2itemImage[7];

		Sprite* bombItemSprite;
		Sprite* icebombItemSprite;
		Sprite* mushroomItemSprite;
		Sprite* iced;
		AnimatorController* bombanimator;

		float p1AniTimer = 10.0f;
		float p2AniTimer = 10.0f;
		float p1AniTime[7]{ 0.0f, 0.13f, 0.25f, 0.38f, 0.52f, 0.65f, 0.83f };
		float p2AniTime[7]{ 0.0f, 0.13f, 0.25f, 0.38f, 0.52f, 0.65f, 0.83f };
		AnimationCurve* itemanimation;
	public:
    ItemManager2()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		static ItemManager2* instance;

		void Awake();
		int p1count = 0;
		int p2count = 0;
		void Update();
		void OnDestroy();
		void UseItem(int player, ItemType item);
		void AddItem(int player, ItemType item);
	};
}