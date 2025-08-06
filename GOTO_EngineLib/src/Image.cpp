#include "Image.h"
#include "Canvas.h"
#include "Screen.h"
#include "RectTransform.h"
#include "IRenderAPI.h"
#include "RenderManager.h"
#include "Texture2D.h"
#include "Matrix3x3.h"

void GOTOEngine::Image::Render()
{
    if (m_canvas && m_sprite && m_sprite->GetTexture())
    {
        auto renderAPI = GetRenderAPIFromManager();

        auto screenSize = Screen::GetSize();
        auto canvasSize = m_canvas->GetCanvasSize();

        auto rectTransform = GetRectTransform();

        auto sizeDelta = rectTransform->GetSizeDelta();
        auto sizeFactorX = canvasSize.x / screenSize.x;
        auto sizeFactorY = canvasSize.y / screenSize.y;
        auto sizePivotX = GetRectTransform()->GetPivot().x * sizeDelta.x * sizeFactorX;
        auto sizePivotY = GetRectTransform()->GetPivot().y * sizeDelta.y * sizeFactorY;
        auto currentPos = rectTransform->GetAnchoredPosition();


        TextureFiltering filter = TextureFiltering::Nearest;
        switch (m_sprite->GetTexture()->GetRenderMode())
        {
        case TextureRenderMode::Point:
            filter = TextureFiltering::Nearest;
            break;
        case TextureRenderMode::Bilinear:
            filter = TextureFiltering::Linear;
            break;
        }

        switch (m_type)
        {
        case ImageType::Simple:
            renderAPI->DrawBitmap(m_sprite->GetTexture()->GetBitmap(), {}, { currentPos.x * sizeFactorX - sizePivotX,currentPos.y * sizeFactorY - sizePivotY,sizeDelta.x * sizeFactorX,sizeDelta.y * sizeFactorY }, m_sprite->GetRect(), m_color, filter, true);
            break;
        case ImageType::RadialFill:
            if(m_fillAmount < 1.0f)
                renderAPI->DrawRadialFillBitmap(m_sprite->GetTexture()->GetBitmap(), {}, { currentPos.x * sizeFactorX - sizePivotX,currentPos.y * sizeFactorY - sizePivotY,sizeDelta.x * sizeFactorX,sizeDelta.y * sizeFactorY }, m_sprite->GetRect(), m_fillAmount, m_startAngle, m_clockwise, m_color, filter, true);
            else
                renderAPI->DrawBitmap(m_sprite->GetTexture()->GetBitmap(), {}, { currentPos.x * sizeFactorX - sizePivotX,currentPos.y * sizeFactorY - sizePivotY,sizeDelta.x * sizeFactorX,sizeDelta.y * sizeFactorY }, m_sprite->GetRect(), m_color, filter, true);
            break;
        }        
    }
    else if (m_type == ImageType::FilledRect)
    {
        auto renderAPI = GetRenderAPIFromManager();

        auto screenSize = Screen::GetSize();
        auto canvasSize = m_canvas->GetCanvasSize();

        auto rectTransform = GetRectTransform();

        auto sizeDelta = rectTransform->GetSizeDelta();
        auto sizeFactorX = canvasSize.x / screenSize.x;
        auto sizeFactorY = canvasSize.y / screenSize.y;
        auto sizePivotX = GetRectTransform()->GetPivot().x * sizeDelta.x * sizeFactorX;
        auto sizePivotY = GetRectTransform()->GetPivot().y * sizeDelta.y * sizeFactorY;
        auto currentPos = rectTransform->GetAnchoredPosition();

        renderAPI->DrawRect({ currentPos.x * sizeFactorX - sizePivotX,currentPos.y * sizeFactorY - sizePivotY,sizeDelta.x * sizeFactorX,sizeDelta.y * sizeFactorY }, true, {}, m_color, true);
    }
}

void GOTOEngine::Image::Dispose()
{
    Graphic::Dispose();
    if (IsValidObject(m_sprite)
        && !m_sprite->IsDestroyed())
    {
        m_sprite->DecreaseRefCount();
        m_sprite = nullptr;
    }
}

void GOTOEngine::Image::SetSprite(Sprite* sprite)
{
    if (m_sprite != sprite)
    {
        if (sprite)
        {
            sprite->IncreaseRefCount();
			auto rectTransform = GetRectTransform();
            if (rectTransform && rectTransform->GetSizeDelta().x == 0 && rectTransform->GetSizeDelta().y == 0)
            {
                rectTransform->SetSizeDelta({ sprite->GetRect().width,sprite->GetRect().height });
            }
        }
        if (m_sprite)
            m_sprite->DecreaseRefCount();
    }
    m_sprite = sprite;
}

void GOTOEngine::Image::SetSprite(Sprite* sprite, Rect srcRect)
{
    SetSprite(sprite);
    if (m_sprite)
    {
        m_sprite->SetRect(srcRect);
    }
}

void GOTOEngine::Image::SetSprite(const std::wstring filePath)
{
    SetSprite(Resource::Load<Sprite>(filePath));
}

void GOTOEngine::Image::SetSprite(const std::wstring filePath, Rect srcRect)
{
    SetSprite(Resource::Load<Sprite>(filePath));
    if (m_sprite)
    {
        m_sprite->SetRect(srcRect);
    }
}
