#pragma once
#include <ScriptBehaviour.h>
#include <TimeManager.h>
#include <InputManager.h>
#include <RenderManager.h>
#include <SpriteRenderer.h>
#include <Animator.h>
#include <ParticleSystem.h>
#include "Canvas.h"
#include "Image.h"
#include "RectTransform.h"
#include "Screen.h"

namespace GOTOEngine
{
	enum class ItemType {
		Bomb = 0, 
		Icebomb, 
		Ticket,
		Item_Count
	};
	class AnimationCurve;
	class ItemManager : public ScriptBehaviour
	{
	private:
		std::vector<ItemType> p1Items;
		std::vector<ItemType> p2Items;
		float p1TicketTimer = 0.0f;
		float p2TicketTimer = 0.0f;
		float p1IceTimer = 0.0f;
		float p2IceTimer = 0.0f;
		const float timelimit = 5.0f;
		Image* itembar1;
		Image* itembar2;
		Image* p1itemImage[7];
		Image* p2itemImage[7];

		Sprite* bombItemSprite;
		Sprite* icebombItemSprite;
		Sprite* ticketItemSprite;
		Sprite* iced;
		AnimatorController* bombanimator;

		float p1AniTimer = 10.0f;
		float p2AniTimer = 10.0f;
		float p1AniTime[7]{ 0.0f, 0.13f, 0.25f, 0.38f, 0.52f, 0.65f, 0.83f };
		float p2AniTime[7]{ 0.0f, 0.13f, 0.25f, 0.38f, 0.52f, 0.65f, 0.83f };
		AnimationCurve* itemanimation;

		ParticleSystem* itemparticle1;
		ParticleSystem* itemparticle2;
		Sprite* snow;
	public:
    ItemManager()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		static ItemManager* instance;

		void Awake();
		int p1count = 0;
		int p2count = 0;
		void Update();
		void OnDestroy();
		void UseItem(int player, ItemType item);
		void AddItem(std::uint32_t player, ItemType item);
	};
}