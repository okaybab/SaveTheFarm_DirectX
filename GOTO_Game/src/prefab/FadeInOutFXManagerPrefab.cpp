#include "FadeInOutFXManagerPrefab.h"
#include <Canvas.h>
#include <Image.h>
#include <RectTransform.h>
#include "FadeInOutFXManager.h"
#include "Screen.h"

GOTOEngine::GameObject* GOTOEngine::FadeInOutFXManagerPrefab::CreateFadeInOutFXPrefab()
{
    auto GO = new GameObject(L"FadeInOut FX Manager");


    auto canvas = GO->AddComponent<Canvas>();

    auto manager = GO->AddComponent<FadeInOutFXManager>();

    auto blackScreenGO = new GameObject(L"BlackScreen");
    auto blackScreen = blackScreenGO->AddComponent<Image>();

    blackScreen->GetRectTransform()->SetParent(GO->GetTransform(), false);
    blackScreen->SetSortOrder(2500);
    blackScreen->SetImageType(ImageType::FilledRect);
    blackScreen->SetColor({ 0,0,0,255 });
    blackScreen->GetRectTransform()->SetSizeDelta(Screen::GetSize());
    blackScreen->GetRectTransform()->SetAnchoredPosition({ 0,0 });

    manager->blackScreen = blackScreen;
    return GO;
}
