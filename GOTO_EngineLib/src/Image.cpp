#include "Image.h"
#include "Canvas.h"
#include "Screen.h"
#include "RectTransform.h"
#include "IRenderAPI.h"
#include "RenderManager.h"
#include "Texture2D.h"

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
        renderAPI->DrawBitmap(m_sprite->GetTexture()->GetBitmap(), {}, { currentPos.x * sizeFactorX,currentPos.y * sizeFactorY,sizeDelta.x,sizeDelta.y }, m_sprite->GetRect(),m_color,filter, true);
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
            if (rectTransform && rectTransform->GetSizeDelta().x == 0 && rectTransform->GetSizeDelta().y)
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
