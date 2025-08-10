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
        auto currentPos = rectTransform->GetAnchoredPosition();
        auto pivot = rectTransform->GetPivot();
        auto scale = rectTransform->GetLocalScale();
        float rotation = rectTransform->GetLocalRotation();


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

        switch (m_type)
        {
        case ImageType::Simple:
            renderAPI->DrawBitmap(m_sprite->GetTexture()->GetBitmap(), transform, { 0.0f, 0.0f, sizeDelta.x, sizeDelta.y }, m_sprite->GetRect(), m_color, filter);
            break;
        case ImageType::RadialFill:
            if(m_fillAmount < 1.0f)
                renderAPI->DrawRadialFillBitmap(m_sprite->GetTexture()->GetBitmap(), transform, { 0.0f, 0.0f, sizeDelta.x, sizeDelta.y }, m_sprite->GetRect(), m_fillAmount, m_startAngle, m_clockwise, m_color, filter);
            else
                renderAPI->DrawBitmap(m_sprite->GetTexture()->GetBitmap(), transform, { 0.0f, 0.0f, sizeDelta.x, sizeDelta.y }, m_sprite->GetRect(), m_color, filter);
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

        renderAPI->DrawRect({ 0.0f, 0.0f, sizeDelta.x, sizeDelta.y }, true, transform, m_color);
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
