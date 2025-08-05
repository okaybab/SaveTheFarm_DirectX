#include "RectRenderer.h"
#include "RenderManager.h"
#include "IRenderAPI.h"
#include "Transform.h"

void GOTOEngine::RectRenderer::Render(Matrix3x3& viewMatrix)
{
    auto renderAPI = GetRenderAPIFromManager();

	//피벗 이동
	auto transform = Matrix3x3::Translate(m_rect.width * -0.5f, m_rect.height * -0.5f);

	//유니티 좌표계 플립
	transform = Matrix3x3::Scale(1.0f, -1.0f) * transform;

	////TRS 세팅
	transform = GetTransform()->GetWorldMatrix();

	////유니티 좌표계 매트릭스 적용
	transform = viewMatrix * transform;

	renderAPI->DrawRect(m_rect, m_fillDraw, transform, m_color, false);
}
