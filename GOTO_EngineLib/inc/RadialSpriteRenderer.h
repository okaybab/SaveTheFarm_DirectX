#pragma once
#include "SpriteRenderer.h"

namespace GOTOEngine
{
	class RadialSpriteRenderer : public SpriteRenderer
	{
	private:
		float m_fillAmount = 1.0f;      // 0.0f ~ 1.0f (채움 정도)
		float m_startAngle = 0.0f;      // 시작 각도 (도 단위, 0 = 위쪽)
		bool m_clockwise = true;        // 시계방향 여부
		bool m_useRadialFill = true;   // Radial Fill 사용 여부

    public:
        // Radial Fill 설정 함수들
        void SetFillAmount(float amount) { m_fillAmount = max(0.0f, min(1.0f, amount)); }
        void SetStartAngle(float angle) { m_startAngle = angle; }
        void SetClockwise(bool clockwise) { m_clockwise = clockwise; }
        void SetUseRadialFill(bool use) { m_useRadialFill = use; }

        float GetFillAmount() const { return m_fillAmount; }
        float GetStartAngle() const { return m_startAngle; }
        bool GetClockwise() const { return m_clockwise; }
        bool GetUseRadialFill() const { return m_useRadialFill; }

    protected:
        void Render(Matrix3x3& matrix) override;
	};
}