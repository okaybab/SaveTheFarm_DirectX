#include "DirIndicatorPrefab.h"
#include "DirIndicator.h"
#include <SpriteRenderer.h>
#include <Transform.h>

GOTOEngine::GameObject* GOTOEngine::DirIndicatorPrefab::CreateIndicator(int id)
{
    auto GO = new GameObject(L"Enemy Direction Indicator");

    auto RootGO = new GameObject(L"Indicator Root");
    RootGO->GetTransform()->SetParent(GO->GetTransform());
    auto RootSprite = RootGO->AddComponent<SpriteRenderer>(); 
    RootSprite->SetSprite(L"../Resources/Demo/enemyDir_right.png");
    RootSprite->SetRenderLayer(1 << (id + 1));
    RootSprite->SetRenderOrder(1250);

    RootGO->GetTransform()->SetLocalPosition({ -70.0f, 0.0f });

    auto indicator = GO->AddComponent<DirIndicator>();

    GO->SetActive(false);

    return GO;
}
