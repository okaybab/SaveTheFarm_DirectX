#pragma once
#include "Behaviour.h"
#include "Vector2.h"

namespace GOTOEngine
{
	class AudioListener : public Behaviour
	{
	private:
		Vector2 m_lastPosition;
		bool m_positionDirty;
	protected:
		~AudioListener();

	public:
		AudioListener();
		void UpdateListenerPosition();

		static AudioListener* GetMain();
		void SetAsMainListener();
	};
}