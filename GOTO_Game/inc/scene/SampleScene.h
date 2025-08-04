#pragma once
#include "Scene.h"

using namespace GOTOEngine;
class SampleScene : public Scene
{
public:
	SampleScene(const std::wstring& sceneName) : Scene(sceneName) {}
	void Initialize() override;
};
