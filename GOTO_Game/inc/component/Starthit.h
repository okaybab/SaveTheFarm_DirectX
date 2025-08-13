#pragma once
#include "ScriptBehaviour.h"
#include "StartAnimal.h"
#include "IAttackAble.h"
#include "SoundManager.h"

namespace GOTOEngine {
	class Starthit : public ScriptBehaviour, public IAttackAble
	{
	private:
	public:
		int id;//µ¿¹° id(crow : 0, goldcrow : 1, thiefmole : 2)
		void TakeDamage(int attackerID, float damage) override {
			SoundManager::instance->PlaySFX("Hit");
			if (id == 0) {
				startanimal->crowhittimer = 1.0f;
				startanimal->crowParticle->Play();
			}
			else if (id == 1) {
				startanimal->goldhittimer = 1.0f;
				startanimal->goldParticle->Play();
			}
			else if (id == 2) {
				startanimal->thiefhittimer = 1.0f;
				startanimal->thiefParticle->Play();
			}
		}
		StartAnimal* startanimal;
	};
}