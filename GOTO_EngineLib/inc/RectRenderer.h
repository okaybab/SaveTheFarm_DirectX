#pragma once
#include "Renderer.h"
#include "Rect.h"
#include "Color.h"

namespace GOTOEngine
{
	class RectRenderer : public Renderer
	{
	protected:
		friend class RenderManager;
		Rect m_rect;
		Color m_color;
		bool m_fillDraw;
		void Render(Matrix3x3& viewMatrix) override;
	public:
		RectRenderer() : m_rect({0,0,100.0f,100.0f}), m_color({255,255,255,255}), m_fillDraw(true) {}

		void SetRect(const Rect& rect) { m_rect = rect; }
		const Rect& GetRect() const { return m_rect; }
		void SetColor(const Color& color) { m_color = color; }
		const Color& GetColor() const { return m_color; }
	};
}
