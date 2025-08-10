#include "Text.h"
#include "Canvas.h"
#include "RectTransform.h"
#include "RenderManager.h"
#include "IRenderAPI.h"
#include "Screen.h"


void GOTOEngine::Text::AdditionalInitialize()
{
    auto rectTransform = GetRectTransform();
    if (rectTransform && rectTransform->GetSizeDelta().x == 0 && rectTransform->GetSizeDelta().y == 0)
    {
        rectTransform->SetSizeDelta({ 100, 100 });
    }
}

void GOTOEngine::Text::SetFont(Font* font)
{
    if (m_font != font)
    {
        if (font)
        {
            font->IncreaseRefCount();
        }
        if (m_font)
            m_font->DecreaseRefCount();
    }
    m_font = font;
}

void GOTOEngine::Text::SetFont(Font* font, size_t size)
{
	SetFont(font);
	if (m_font)
	{
		this->size = size;
	}
}

void GOTOEngine::Text::SetFont(const std::wstring filePath)
{
	SetFont(Resource::Load<Font>(filePath));
}

void GOTOEngine::Text::SetFont(const std::wstring filePath, size_t size)
{
	SetFont(Resource::Load<Font>(filePath));
	if (m_font)
	{
        this->size = size;
	}
}

void GOTOEngine::Text::Dispose()
{
    Graphic::Dispose();
    if (IsValidObject(m_font)
        && !m_font->IsDestroyed())
    {
        m_font->DecreaseRefCount();
        m_font = nullptr;
    }
}

void GOTOEngine::Text::Render()
{
    auto renderAPI = GetRenderAPIFromManager();
    auto screenSize = Screen::GetSize();
    auto canvasSize = m_canvas->GetCanvasSize();
    auto rectTransform = GetRectTransform();
    auto sizeDelta = rectTransform->GetSizeDelta();
    auto sizeFactorX = canvasSize.x / screenSize.x;
    auto sizeFactorY = canvasSize.y / screenSize.y;
    auto currentPos = rectTransform->GetAnchoredPosition();
    auto pivot = rectTransform->GetPivot();
    auto scale = rectTransform->GetLocalScale();

    // Unity 좌표계 (좌하단 기준)을 D2D 좌표계 (좌상단 기준)으로 변환
    // 1. 피벗 기준 변환 - Unity에서는 pivot (0,0)이 좌하단이므로
    auto transform = Matrix3x3::Translate(
        sizeDelta.x * -pivot.x,
        sizeDelta.y * (pivot.y - 1.0f)  // Y축을 뒤집어서 좌하단 기준으로 만듦
    );

    // 2. TRS 적용 (위치도 Y축 뒤집기 적용)
    transform = Matrix3x3::TRS(
        { currentPos.x * sizeFactorX, -currentPos.y * sizeFactorY }, // Y축 뒤집기
        rectTransform->GetLocalRotation(),
        { scale.x * sizeFactorX, scale.y * sizeFactorY }
    ) * transform;

    // 3. 최종적으로 화면 높이만큼 아래로 이동 (D2D 좌표계에서 Unity 좌표계처럼 보이게)
    transform = Matrix3x3::Translate(0, canvasSize.y) * transform;

    renderAPI->DrawString(text.c_str(), { 0.0f, 0.0f, sizeDelta.x, sizeDelta.y },
        IsValidObject(m_font) ? m_font->GetFont() : nullptr,
        size, FontStyleHelper::ToRenderFontStyle(fontStyle),
        m_color, transform,
        static_cast<int>(horizontalAlign), static_cast<int>(verticalAlign),
        false);
}

