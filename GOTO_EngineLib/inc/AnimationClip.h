#pragma once
#include "Resource.h"
#include "RenderManager.h"
#include "Sprite.h"
#include <vector>

namespace GOTOEngine
{
	class AnimationKeyframe : public Object
	{
	private:
		friend class AnimationClip;
		Sprite* m_sprite;
		float m_time;
		float m_alpha;
	public:
		AnimationKeyframe(Sprite* sprite, float time, float alpha = 255.0f )
			: m_sprite(sprite)
			, m_time(time)
			, m_alpha(alpha)
		{
		}

		void Dispose() override
		{
			if (IsValidObject(m_sprite) 
				&& !m_sprite->IsDestroyed())
			{
				m_sprite->DecreaseRefCount();
				m_sprite = nullptr;
			}
		}

		const float& GetTime() { return m_time; }
		Sprite* GetSprite() { return m_sprite; }
		const float& GetAlpha() const { return m_alpha; }
	};

	
	//======================= 스프라이트 클립 ===========================//
	//
	//
	//    유의 사항 - clip texture와 spriteSheet json이 동일한 경로에 있어야 합니다.
	//
	//==================================================================//

	class AnimationClip : public Resource
	{
	private:
		friend class ResourceManager;
		friend class AnimatorState;
		void LoadFromFilePath(const std::wstring& filePath) override;
		bool IsValidData() override { return m_keyframes.size() > 0; }
		std::vector<AnimationKeyframe*> m_keyframes;
		bool m_isLoop;
		bool m_isAlpha;
		float m_duration;
		float m_animSpeed = 1.0f;
		std::wstring m_texturePath;
		std::wstring m_clipName;
		void Dispose() override;
	public:
		const bool& IsLoop() const { return m_isLoop; }
		const bool& IsAlpha() const { return m_isAlpha; }
		const std::vector<AnimationKeyframe*>& GetKeyframes() const { return m_keyframes; }
		const float& GetDuration() const { return m_duration; }
		float GetSpeed() const { return m_animSpeed; }
		const std::wstring& GetClipName() const { return m_clipName; }
		const std::vector<AnimationKeyframe*>& GetKeyframes() { return m_keyframes; }
	};
}