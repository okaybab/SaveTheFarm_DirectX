#include "Matrix3x3.h"

GOTOEngine::Matrix3x3 GOTOEngine::Matrix3x3::operator*(const Matrix3x3& rhs) const
{
    //Matrix3x3 result;

    //for (int c = 0; c < 3; ++c) // result의 열
    //{
    //    for (int r = 0; r < 3; ++r) // result의 행
    //    {
    //        result.m[c][r] =
    //            m[0][r] * rhs.m[c][0] +
    //            m[1][r] * rhs.m[c][1] +
    //            m[2][r] * rhs.m[c][2];
    //    }
    //}
    //return result;

    Matrix3x3 result;

    // 상단 2x2 회전/스케일 부분 계산 (m[c][r], c=0,1, r=0,1)
    // result.m[c][r] = m[0][r] * rhs.m[c][0] + m[1][r] * rhs.m[c][1] + m[2][r] * rhs.m[c][2];
    // 여기서 m[2][r]은 r=0,1일 때 0이므로, m[2][r]*rhs.m[c][2] 항은 0이 됩니다.
    result.m[0][0] = m[0][0] * rhs.m[0][0] + m[1][0] * rhs.m[0][1];
    result.m[0][1] = m[0][1] * rhs.m[0][0] + m[1][1] * rhs.m[0][1];

    result.m[1][0] = m[0][0] * rhs.m[1][0] + m[1][0] * rhs.m[1][1];
    result.m[1][1] = m[0][1] * rhs.m[1][0] + m[1][1] * rhs.m[1][1];

    // 평행이동 부분 계산 (m[2][r], r=0,1)
    // result.m[2][r] = m[0][r] * rhs.m[2][0] + m[1][r] * rhs.m[2][1] + m[2][r] * rhs.m[2][2];
    // 여기서 rhs.m[2][2]는 1이므로, + m[2][r]이 됩니다.
    result.m[2][0] = m[0][0] * rhs.m[2][0] + m[1][0] * rhs.m[2][1] + m[2][0];
    result.m[2][1] = m[0][1] * rhs.m[2][0] + m[1][1] * rhs.m[2][1] + m[2][1];

    // 마지막 열은 항상 고정된 값 (2D 변환 행렬의 특성)
    result.m[0][2] = 0.0f;
    result.m[1][2] = 0.0f;
    result.m[2][2] = 1.0f;

    return result;
}

void GOTOEngine::Matrix3x3::SetIdentity()
{
    std::memset(m, 0, sizeof(m));
    for (int i = 0; i < 3; ++i)
        m[i][i] = 1.0f;
}

GOTOEngine::Matrix3x3 GOTOEngine::Matrix3x3::Transpose() const
{
    Matrix3x3 result;
    for (int c = 0; c < 3; ++c)
    {
        for (int r = 0; r < 3; ++r)
        {
            result.m[r][c] = m[c][r];
        }
    }
    return result;
}

void GOTOEngine::Matrix3x3::ToRowMajorArray(float out[9]) const
{
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            out[r * 3 + c] = m[c][r];
        }
    }
}

void GOTOEngine::Matrix3x3::ToColumnMajorArray(float out[9]) const
{
    std::memcpy(out, &m[0][0], sizeof(float) * 9);
}

GOTOEngine::Matrix3x3 GOTOEngine::Matrix3x3::Inverse() const
{
    //Matrix3x3 inv;
    //// 이 코드는 Column-Major 저장 방식에 따라 인덱스를 조정해야 합니다.
    //// mat[col * 3 + row] 형태로 변환하여 접근하거나 m[col][row] 형태로 직접 접근합니다.
    //// 현재 코드는 mat[idx] 형식으로 접근하므로, 이를 m[c][r] 형태로 변경합니다.
    //const float* mat_ptr = &this->m[0][0]; // m[0][0], m[1][0], m[2][0], m[0][1] ...

    //// 행렬 요소에 직접 접근하도록 변경 (m[column][row])
    //float det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
    //    m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2]) +
    //    m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);

    //if (std::fabs(det) < 1e-6f)
    //{
    //    // Singular matrix
    //    return Matrix3x3(); // 단위행렬 반환
    //}

    //float invDet = 1.0f / det;

    //// 수반 행렬의 전치 (Adjugate)를 계산한 후 역행렬 공식 적용
    //// (column-major 출력을 위해)
    //inv.m[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet;
    //inv.m[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invDet;
    //inv.m[2][0] = (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * invDet;

    //inv.m[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invDet;
    //inv.m[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet;
    //inv.m[2][1] = (m[0][1] * m[2][0] - m[0][0] * m[2][1]) * invDet;

    //inv.m[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet;
    //inv.m[1][2] = (m[0][2] * m[1][0] - m[0][0] * m[1][2]) * invDet;
    //inv.m[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * invDet;

    //return inv;
    Matrix3x3 inv;

    // 상단 2x2 행렬의 행렬식
    float det = m[0][0] * m[1][1] - m[1][0] * m[0][1];
    if (std::fabs(det) < 1e-6f)
        return Matrix3x3(); // 단위 행렬 (혹은 실패 처리)

    float invDet = 1.0f / det;

    // 2x2 역행렬 (회전+스케일의 역변환)
    inv.m[0][0] = m[1][1] * invDet;
    inv.m[0][1] = -m[0][1] * invDet;
    inv.m[1][0] = -m[1][0] * invDet;
    inv.m[1][1] = m[0][0] * invDet;

    // (translation 역변환)
    float tx = m[2][0];
    float ty = m[2][1];

    inv.m[2][0] = -(inv.m[0][0] * tx + inv.m[1][0] * ty);
    inv.m[2][1] = -(inv.m[0][1] * tx + inv.m[1][1] * ty);

    // 나머지 고정
    inv.m[0][2] = 0.0f;
    inv.m[1][2] = 0.0f;
    inv.m[2][2] = 1.0f;

    return inv;
}

float GOTOEngine::Matrix3x3::GetRotation() const
{
    //// X축 방향 벡터의 제곱 길이를 구함 (sqrt 연산 최소화)
    //float scaleXSquared = m[0][0] * m[0][0] + m[0][1] * m[0][1];

    //// 아주 작은 스케일 체크 (제곱값으로 비교하여 sqrt 생략)
    //if (scaleXSquared < 1e-12f) {
    //    return 0.0f;
    //}

    //// 빠른 역제곱근 또는 일반 sqrt 사용
    //float invScaleX = 1.0f / std::sqrt(scaleXSquared);

    //// 정규화된 벡터로부터 회전 각도 추출
    //return std::atan2(m[0][1] * invScaleX, m[0][0] * invScaleX);

    float scaleXSquared = m[0][0] * m[0][0] + m[0][1] * m[0][1];

    if (scaleXSquared < 1e-12f) { // 스케일이 거의 0인 경우 (회전 무의미)
        return 0.0f;
    }

    // m[0][1]은 sin(theta), m[0][0]은 cos(theta)에 비례하므로
    // 바로 atan2를 사용하여 각도 추출. 별도의 정규화 필요 없음.
    return std::atan2(m[0][1], m[0][0]);
}

GOTOEngine::Vector2 GOTOEngine::Matrix3x3::GetLossyScale() const
{
    // 열 우선 행렬에서 스케일 추출
    // m[0][0] = Sx * cos(theta), m[0][1] = Sx * sin(theta)
    // m[1][0] = -Sy * sin(theta), m[1][1] = Sy * cos(theta)

    // 첫 번째 열 (로컬 X축)의 길이: sqrt(m[0][0]^2 + m[0][1]^2) = Sx
    float scaleX = std::sqrt(m[0][0] * m[0][0] + m[0][1] * m[0][1]);
    // 두 번째 열 (로컬 Y축)의 길이: sqrt(m[1][0]^2 + m[1][1]^2) = Sy
    float scaleY = std::sqrt(m[1][0] * m[1][0] + m[1][1] * m[1][1]);

    return Vector2(scaleX, scaleY);
}

GOTOEngine::Vector2 GOTOEngine::Matrix3x3::MultiplyPoint(const Vector2& point) const
{
    Vector2 result;

#ifdef _SIMD_OPTIMIZED
    // SIMD 최적화: 2D 점 변환에 맞게 수정 (3x3 * 3x1 벡터)
    // 입력 point = (x, y, 1.0f)
    // 행렬의 열 벡터를 로드하여 곱셈 및 덧셈 수행

    // point.x, point.y, 1.0f를 각각 스칼라로 사용
    __m128 p_vec = _mm_set_ps(0.0f, 1.0f, point.y, point.x); // (x, y, 1, 0)

    // 행렬의 열들을 로드
    // col0 = (m[0][0], m[0][1], m[0][2], 0)
    // col1 = (m[1][0], m[1][1], m[1][2], 0)
    // col2 = (m[2][0], m[2][1], m[2][2], 0)
    __m128 col0 = _mm_loadu_ps(&m[0][0]); // m[0][0], m[0][1], m[0][2], dummy
    __m128 col1 = _mm_loadu_ps(&m[1][0]); // m[1][0], m[1][1], m[1][2], dummy
    __m128 col2 = _mm_loadu_ps(&m[2][0]); // m[2][0], m[2][1], m[2][2], dummy

    // x * col0
    __m128 mul0 = _mm_mul_ps(col0, _mm_shuffle_ps(p_vec, p_vec, _MM_SHUFFLE(0, 0, 0, 0))); // point.x
    // y * col1
    __m128 mul1 = _mm_mul_ps(col1, _mm_shuffle_ps(p_vec, p_vec, _MM_SHUFFLE(1, 1, 1, 1))); // point.y
    // 1.0f * col2
    __m128 mul2 = _mm_mul_ps(col2, _mm_shuffle_ps(p_vec, p_vec, _MM_SHUFFLE(2, 2, 2, 2))); // 1.0f

    // (x * col0) + (y * col1) + (1.0f * col2)
    __m128 res = _mm_add_ps(_mm_add_ps(mul0, mul1), mul2);

    // 결과 추출
    // 결과는 res의 첫 두 요소에 있음 (x', y')
    _mm_store_ss(&result.x, res); // res의 첫 번째 요소 (x)
    _mm_store_ss(&result.y, _mm_shuffle_ps(res, res, _MM_SHUFFLE(1, 1, 1, 1))); // res의 두 번째 요소 (y)

#else
    // 열 우선 행렬에서 점 변환:
    // P_transformed.x = m[0][0] * P.x + m[1][0] * P.y + m[2][0] * 1.0
    // P_transformed.y = m[0][1] * P.x + m[1][1] * P.y + m[2][1] * 1.0
    // 마지막 1.0은 2D 동차 좌표계의 w값
    result.x = m[0][0] * point.x + m[1][0] * point.y + m[2][0];
    result.y = m[0][1] * point.x + m[1][1] * point.y + m[2][1];
#endif

    return result;
}

GOTOEngine::Vector2 GOTOEngine::Matrix3x3::MultiplyVector(const Vector2& vector) const
{
    Vector2 result;

    // 벡터 변환은 위치 이동 없이 회전과 스케일만 적용
    // P_transformed.x = m[0][0] * P.x + m[1][0] * P.y
    // P_transformed.y = m[0][1] * P.x + m[1][1] * P.y
    result.x = m[0][0] * vector.x + m[1][0] * vector.y;
    result.y = m[0][1] * vector.x + m[1][1] * vector.y;

    return result;
}

GOTOEngine::Matrix3x3 GOTOEngine::Matrix3x3::TRS(const Vector2& position, float rotationRadians, const Vector2& scale)
{
    // 열 우선 행렬의 TRS 구성
    // 일반적으로 OpenGL에서 사용하는 2D 변환 행렬 구조:
    // [ Sx*cos(theta)   -Sy*sin(theta)   Tx ]
    // [ Sx*sin(theta)    Sy*cos(theta)   Ty ]
    // [ 0                0               1  ]

    Matrix3x3 mat;
    mat.SetIdentity();

    float c, s;

    // 회전각이 0이면 cos/sin 호출을 건너뛴다
    if (fabs(rotationRadians) < 1e-6f) {
        c = 1.0f;
        s = 0.0f;
    }
    else {
        c = std::cos(rotationRadians);
        s = std::sin(rotationRadians);
    }

    // Column 0 (Scaled X-axis)
    mat.m[0][0] = c * scale.x;
    mat.m[0][1] = s * scale.x;
    mat.m[0][2] = 0.0f;

    // Column 1 (Scaled Y-axis)
    mat.m[1][0] = -s * scale.y;
    mat.m[1][1] = c * scale.y;
    mat.m[1][2] = 0.0f;

    // Column 2 (Translation)
    mat.m[2][0] = position.x;
    mat.m[2][1] = position.y;
    mat.m[2][2] = 1.0f;

    return mat;
}