#pragma once
#include "Renderer.h"
#include "Sprite.h"
#include "Rect.h"

namespace GOTOEngine
{
	//======== 스프라이트 렌더러 ========
	//
	//          * 유의 사항 *
	// 
	//  GetSprite()를 대입식에 집어넣은 경우
	//  반드시 IncreaseRefCount()를 호출하여 
	//  참조카운트가 누락되지 않도록 합니다.
	// 
	//  ex) Sprite* renderSprite = spriteRendererComp->GetSprite();
	//      renderSprite.IncreaseRefCount();
	//
	// 
	//  이후 사용을 끝마치는 경우 반드시 
	//  DecreaseRefCount()를 호출하여 마찬가지로
	//  참조카운트가 누락되지 않도록 합니다.
	// 
	//  ex) renderSprite.DecreaseRefCount(); //코드 블럭 끝에
	// 
	//
	//=================================

	class SpriteRenderer : public Renderer
	{
	protected:
		friend class RenderManager;
		Sprite* m_sprite;
		bool m_flipX;
		bool m_flipY;
		Color m_color;
		void Render(Matrix3x3& viewMatrix) override;
	public:
		SpriteRenderer(): m_sprite(nullptr), m_flipX(false), m_flipY(false), m_color({255,255,255,255}) {}
		void SetSprite(Sprite* sprite);
		void SetSprite(Sprite* sprite, Rect srcRect);

		void SetSprite(const std::wstring filePath);
		void SetSprite(const std::wstring filePath, Rect srcRect);

		bool GetFlipX() const { return m_flipX; }
		bool GetFlipY() const { return m_flipY; }
		void SetFlipX(bool value) { m_flipX = value; }
		void SetFlipY(bool value) { m_flipY = value; }

		void SetColor(const Color& color) { m_color = color; }
		const Color& GetColor() const { return m_color; }

		void Dispose() override { if (IsValidObject(m_sprite) && !m_sprite->IsDestroyed()) { m_sprite->DecreaseRefCount(); m_sprite = nullptr; } }
		Sprite* GetSprite() { return m_sprite; } //참조 카운트 명시적으로 설정하기
	};
 }
