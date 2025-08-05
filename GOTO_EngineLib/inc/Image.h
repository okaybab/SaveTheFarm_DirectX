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
		float m_startAngle = 0.0f;      // 시작 각도 (도 단위, 0 = 위쪽)
		bool m_clockwise = true;
	public:
		Image() : Graphic() {}
		void Dispose() override;
		void SetSprite(Sprite* sprite);
		void SetSprite(Sprite* sprite, Rect srcRect);

		void SetSprite(const std::wstring filePath);
		void SetSprite(const std::wstring filePath, Rect srcRect);

		void SetStartAngle(float angle) { m_startAngle = angle; }
		void SetClockwise(bool clockwise) { m_clockwise = clockwise; }

		float GetStartAngle() const { return m_startAngle; }
		bool GetClockwise() const { return m_clockwise; }

		void SetImageType(ImageType type) { m_type = type; }
		ImageType GetImageType() { return m_type; }

		void SetFillAmount(float value) { m_fillAmount = Mathf::Max(0.0f, Mathf::Min(1.0f, m_fillAmount)); }
		const float& GetFillAmount() const { return m_fillAmount; }

		Sprite* GetSprite() { return m_sprite; };
		void Render() override;
	};
}