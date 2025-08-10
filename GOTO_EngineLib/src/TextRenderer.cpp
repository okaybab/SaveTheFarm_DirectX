#include "TextRenderer.h"
#include "Transform.h"
#include "IRenderAPI.h"

void GOTOEngine::TextRenderer::Render(Matrix3x3& matrix)
{
    auto renderAPI = GetRenderAPIFromManager();

    IRenderFont* fontHandle = nullptr;
    if(m_font)
        fontHandle = m_font->GetFont();

    //피벗 이동
    auto transform = Matrix3x3::Translate(rect.width * -pivot.x, rect.height * -pivot.y);

    //유니티 좌표계 플립
    transform = Matrix3x3::Scale(1.0f, -1.0f) * transform;

    ////TRS 세팅
    transform = GetGameObject()->GetTransform()->GetWorldMatrix() * transform;

    ////유니티 좌표계 매트릭스 적용
    transform = matrix * transform;

    renderAPI->DrawString(text.c_str(), rect, fontHandle, size, FontStyleHelper::ToRenderFontStyle(fontStyle), color, transform, static_cast<int>(horizontalAlign), static_cast<int>(verticalAlign));
}

void GOTOEngine::TextRenderer::SetFont(Font* font)
{
    if (m_font != font)
    {
        if (font)
            font->IncreaseRefCount();
        if (m_font)
            m_font->DecreaseRefCount();
    }
    m_font = font;
}

void GOTOEngine::TextRenderer::SetFont(std::wstring filePath)
{
	SetFont(Resource::Load<Font>(filePath)); // Resource::Load를 통해 Font 객체를 로드하고 참조 카운트를 관리합니다.
}
