#pragma once
#include <ScriptBehaviour.h>

namespace GOTOEngine
{
	class SpriteRenderer;
	class Camera;
	class DirIndicator : public ScriptBehaviour
	{
	public:
    DirIndicator()
    {
        SetExecutionOrder(151);
        REGISTER_BEHAVIOUR_MESSAGE(LateUpdate);
    }

		Transform* target = nullptr;
		Camera* currentCam = nullptr;

		void LateUpdate();
	};
}