#pragma once
#include <ScriptBehaviour.h>
#include <SpriteRenderer.h>
#include <TimeManager.h>

namespace GOTOEngine
{
	class AnimateSprite : public ScriptBehaviour
	{
	private:
		float m_animationTime = 0.0f;
		int m_index = 0;
		std::vector<Sprite*> m_sprites;
	public:
		AnimateSprite()
		{
			REGISTER_BEHAVIOUR_MESSAGE(Awake);
			REGISTER_BEHAVIOUR_MESSAGE(Update);
		}

		SpriteRenderer* renderer = nullptr;

		std::wstring commonPath;
		std::vector<std::wstring> filenames;

		float animRate = 0.26f;

		void Awake()
		{
			if (commonPath.empty() || filenames.empty())
				return;

			for (auto& str : filenames)
			{
				auto sprite = Resource::Load<Sprite>(commonPath + str);
				sprite->IncreaseRefCount();
			}
		}

		void Update()
		{
			if (m_sprites.empty())
				return;

			m_animationTime += TIME_GET_DELTATIME();

			if (m_animationTime > animRate)
			{
				while (animRate > 0 && m_animationTime >= animRate)
				{
					m_animationTime -= animRate;
					m_index++;
				}
			}

			renderer->SetSprite(m_sprites[m_index]);
		}

		void OnDestroy()
		{
			for(auto sprite : m_sprites)
				if (IsValidObject(sprite)
					&& !sprite->IsDestroyed())
				{
					sprite->DecreaseRefCount();
				}
		}
	};
}