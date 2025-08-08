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


    //피벗 이동
    auto transform = Matrix3x3::Translate(sizeDelta.x * -pivot.x, sizeDelta.y * -pivot.y  + ((sizeDelta.y * -sizeFactorY) / rectTransform->GetLocalScale().y));

    //유니티 좌표계 플립
    transform = Matrix3x3::Scale(1.0f, -1.0f) * transform;

    ////TRS 세팅
    transform = Matrix3x3::TRS({ currentPos.x * sizeFactorX, currentPos.y * sizeFactorY }, rectTransform->GetLocalRotation(), { scale.x * sizeFactorX, scale.y * sizeFactorY }) * transform;

    //transform = Matrix3x3::Translate(0, (sizeDelta.y * -sizeFactorY / rectTransform->GetLocalScale().y)) * transform;

    renderAPI->DrawString(text.c_str(), { 0.0f, 0.0f, sizeDelta.x ,sizeDelta.y  }, IsValidObject(m_font) ? m_font->GetFont() : nullptr, size, FontStyleHelper::ToRenderFontStyle(fontStyle), m_color, transform, static_cast<int>(horizontalAlign), static_cast<int>(verticalAlign), true);
}

