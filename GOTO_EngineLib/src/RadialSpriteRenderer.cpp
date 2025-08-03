#include "RadialSpriteRenderer.h"
#include "D2DRenderAPI.h"
#include "Sprite.h"
#include "Texture2D.h"
#include "Transform.h"

void GOTOEngine::RadialSpriteRenderer::Render(Matrix3x3& matrix)
{
    if (!m_sprite || !m_sprite->m_texture)
        return;

    auto renderAPI = static_cast<D2DRenderAPI*>(GetRenderAPIFromManager());
    auto bitmap = m_sprite->m_texture->GetBitmap();
    auto spriteRect = m_sprite->GetRect();

    // 피벗 이동
    auto transform = Matrix3x3::Translate(
        spriteRect.width * -m_sprite->GetPivotX(),
        spriteRect.height * m_sprite->GetPivotY() - spriteRect.height
    );

    // 유니티 좌표계 이미지 플립
    transform = Matrix3x3::Scale(
        GetFlipX() ? -1.0f : 1.0f,
        GetFlipY() ? 1.0f : -1.0f
    ) * transform;

    // TRS 적용
    transform = GetGameObject()->GetTransform()->GetWorldMatrix() * transform;

    // 유니티 좌표계 매트릭스 적용
    transform = matrix * transform;

    // 텍스처 필터링 설정
    TextureFiltering filter = TextureFiltering::Nearest;
    switch (m_sprite->m_texture->GetRenderMode())
    {
    case TextureRenderMode::Point:
        filter = TextureFiltering::Nearest;
        break;
    case TextureRenderMode::Bilinear:
        filter = TextureFiltering::Linear;
        break;
    }

    // Radial Fill 사용 여부에 따라 다른 그리기 방법 사용
    if (m_useRadialFill && m_fillAmount < 1.0f)
    {
        renderAPI->DrawRadialFillBitmap(
            bitmap,
            transform,
            { 0, 0, spriteRect.width, spriteRect.height },
            spriteRect,
            m_fillAmount,   // 채움 정도
            m_startAngle,   // 시작 각도
            m_clockwise,    // 시계방향 여부
            GetColor(),
            filter,
            false
        );
    }
    else
    {
        // 기본 스프라이트 렌더링
        renderAPI->DrawBitmap(
            bitmap,
            transform,
            { 0, 0, spriteRect.width, spriteRect.height },
            spriteRect,
            GetColor(),
            filter,
            false
        );
    }
}