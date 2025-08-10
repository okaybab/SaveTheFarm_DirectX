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
    float rotation = rectTransform->GetLocalRotation();

    // 피벗 이동
    Vector2 pivotOffset{
        sizeDelta.x * -pivot.x,
        sizeDelta.y * (pivot.y - 1.0f)
    };

    // 스케일 팩터
    Vector2 finalScale{
        scale.x * sizeFactorX,
        scale.y * sizeFactorY
    };

    // 피벗 오프셋을 스케일 + 회전에 맞춰 변환
    float c = std::cos(rotation);
    float s = std::sin(rotation);
    Vector2 rotatedPivot{
        pivotOffset.x * c * finalScale.x + pivotOffset.y * -s * finalScale.y,
        pivotOffset.x * s * finalScale.x + pivotOffset.y * c * finalScale.y
    };

    // 최종 위치
    Vector2 finalPos{
        currentPos.x * sizeFactorX + rotatedPivot.x,
        -currentPos.y * sizeFactorY + rotatedPivot.y + canvasSize.y
    };

    // 하나의 TRS로 최종 행렬 생성
    auto transform = Matrix3x3::TRS(finalPos, rotation, finalScale);

    renderAPI->DrawString(text.c_str(), { 0.0f, 0.0f, sizeDelta.x, sizeDelta.y },
        IsValidObject(m_font) ? m_font->GetFont() : nullptr,
        size, FontStyleHelper::ToRenderFontStyle(fontStyle),
        m_color, transform,
        static_cast<int>(horizontalAlign), static_cast<int>(verticalAlign));
}

