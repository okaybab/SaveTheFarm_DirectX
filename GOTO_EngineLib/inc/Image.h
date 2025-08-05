#pragma once
#include "Graphic.h"
#include "Sprite.h"

namespace GOTOEngine
{
	enum class ImageType
	{
		Simple = 0,
		RadialFill,
	};
	class Image : public Graphic
	{
	private:
		Sprite* m_sprite = nullptr;
		ImageType m_type = ImageType::Simple;

		float m_fillAmount = 1.0f;
	public:
		Image() : Graphic() {}
		void Dispose() override;
		void SetSprite(Sprite* sprite);
		void SetSprite(Sprite* sprite, Rect srcRect);

		void SetSprite(const std::wstring filePath);
		void SetSprite(const std::wstring filePath, Rect srcRect);

		void SetImageType(ImageType type) { m_type = type; }
		ImageType GetImageType() { return m_type; }

		void SetFillAmount(float value) { m_fillAmount = value; }
		const float& GetFillAmount() const { return m_fillAmount; }

		Sprite* GetSprite() { return m_sprite; };
		void Render() override;
	};
}