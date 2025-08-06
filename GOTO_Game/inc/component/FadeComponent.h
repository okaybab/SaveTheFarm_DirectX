#pragma once
#include <ScriptBehaviour.h>
#include <functional>

namespace GOTOEngine
{
	class FadeComponent : public ScriptBehaviour
	{
	public:
    FadeComponent()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
    }
		void Awake() {}


	};
}