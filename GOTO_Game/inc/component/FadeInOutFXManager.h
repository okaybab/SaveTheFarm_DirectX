#pragma once
#include<ScriptBehaviour.h>

namespace GOTOEngine
{
	class Image;
	class FadeInOutFXManager : public ScriptBehaviour
	{
	private:
		bool m_enableScreen = false;
		float m_fadingTimer = 0.0f;
	public:
    FadeInOutFXManager()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }

		static FadeInOutFXManager* instance;

		Image* blackScreen = nullptr;

		float fadingTime = 0.45f;

		void FadeIn();
		void FadeOut();

		bool IsPerfectlyFadeOut();

		void Awake();
		void Update();
	};
}