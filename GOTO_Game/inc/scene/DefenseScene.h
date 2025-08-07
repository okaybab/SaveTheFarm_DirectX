#pragma once
#include "Scene.h"

using namespace GOTOEngine;
class DefenseScene : public Scene
{
public:
	DefenseScene(const std::wstring& sceneName) : Scene(sceneName) {}
	void Initialize() override;
};
