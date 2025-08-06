#include "DirIndicator.h"
#include <Transform.h>
#include <Camera.h>
#include <Mathf.h>
#include <SpriteRenderer.h>

void GOTOEngine::DirIndicator::LateUpdate()
{
	if (IsValidObject(target))
	{
		GetTransform()->SetPosition(target->GetPosition());
	}

	if (IsValidObject(currentCam))
	{
		auto currentPos = GetTransform()->GetPosition();
		Vector2 posMin;
		Vector2 posMax;

		posMin = currentCam->ViewportToWorldPoint({ 0,0 });
		posMax = currentCam->ViewportToWorldPoint({ 1.0f, 1.0f });

		GetTransform()->SetPosition({ Mathf::Clamp(currentPos.x,posMin.x,posMax.x), Mathf::Clamp(currentPos.y,posMin.y,posMax.y) });

		Vector2 dir = GetTransform()->GetPosition() - currentCam->GetTransform()->GetPosition();
		dir.Normalize();

		float angleInRadians = Mathf::Atan2(dir.y,dir.x);
		float angleInDegrees = angleInRadians * Mathf::Rad2Deg;

		GetTransform()->SetLocalRotation(angleInDegrees);

		auto s = GetTransform()->GetWorldMatrix();
	}
}
