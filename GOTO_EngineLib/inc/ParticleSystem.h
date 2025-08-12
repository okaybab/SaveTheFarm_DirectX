#pragma once
#include "Renderer.h"
#include "Color.h"
#include "Rect.h"
#include "Sprite.h"
#include <random>

namespace GOTOEngine
{
    enum class EmissionShape
    {
        Rectangle,  // 사각형 안에서 무작위 위치로 방출
        Cone,       // 특정 각도의 부채꼴 방향으로 방출
    };

    // 파티클 사라짐 효과 모드
    enum class ParticleFadeMode
    {
        None,       // 갑자기 사라짐
        Shrink,     // 작아지면서 사라짐
        Fade,       // 흐려지면서 사라짐
        ShrinkFade  // 작아지면서 동시에 흐려짐
    };

    // 파티클 구조체
    struct Particle
    {
        // 트랜스폼
        Vector2 position;
        Vector2 scale;
        float rotation;

        // 물리
        Vector2 velocity;           // 초기 선형속도
        float angularVelocity;      // 초기 각속도
        bool useGravity;            // 중력 여부

        // 생명주기
        float lifeTime;             // 현재 남은 생명 시간
        float maxLifeTime;          // 최대 생명 시간

        // 시각적 속성
        Color color;                // 현재 색상 (알파값 포함)
        Color initialColor;         // 초기 색상
        Vector2 initialScale;       // 초기 스케일

        // 활성화 상태
        bool isActive;

        // 생성자
        Particle()
            : position(0, 0), scale(1, 1), rotation(0),
            velocity(0, 0), angularVelocity(0), useGravity(true),
            lifeTime(0), maxLifeTime(1),
            color(255, 255, 255, 255), initialColor(255, 255, 255, 255),
            initialScale(1, 1), isActive(false) {
        }

        // 파티클 초기화
        void Initialize(const Vector2& pos, const Vector2& vel, float angVel,
            float life, const Color& col, const Vector2& scl, bool gravity = true)
        {
            position = pos;
            velocity = vel;
            angularVelocity = angVel;
            lifeTime = maxLifeTime = life;
            color = initialColor = col;
            scale = initialScale = scl;
            rotation = 0;
            useGravity = gravity;
            isActive = true;
        }

        // 파티클이 살아있는지 확인
        bool IsAlive() const { return isActive && lifeTime > 0; }

        // 생명 비율 (0.0 ~ 1.0)
        float GetLifeRatio() const
        {
            return maxLifeTime > 0 ? lifeTime / maxLifeTime : 0;
        }
    };

	class ParticleSystem : public Renderer
	{
	private:
        friend class RenderManager;

        // 파티클 풀과 활성화된 파티클
        std::vector<Particle> m_particlePool;        // 파티클 풀
        std::vector<Particle*> m_activeParticles;    // 활성화된 파티클 포인터들

        // 루프 추적
        bool m_loop;
        bool m_hasPlayedOnce;

        // 파티클 설정
        float m_particleLifeTime;                    // 파티클 생명 시간 (활성 상태 유지 시간)
        float m_fadeOutTime;                         // 파티클 사라지는 시간
        ParticleFadeMode m_fadeMode;                 // 파티클 사라짐 효과
        EmissionShape m_emissionShape;               // 파티클 방출 모양
        int m_particlesPerSpawn;                     // 한번에 등장할 파티클 수
        float m_spawnInterval;                       // 파티클 생성 주기
        int m_maxParticleCount;                      // 최대 파티클 수
        Sprite* m_particleCommonSprite;
        
        // 물리 설정
        Vector2 m_gravity;                           // 중력 벡터
        float m_emissionDirection;                   // 부채꼴의 중심 방향 (라디안)
        float m_emissionAngle;                       // 부채꼴의 각도 (라디안, 0 = 직선, π*2 = 원)
        float m_emissionTangentLength;
        float m_minSpeed;                            // 최소 방출 속도
        float m_maxSpeed;                            // 최대 방출 속도

        // 파티클 랜덤 범위 설정
        float m_minAngularVelocity;                  // 최소 각속도 (라디안/초)
        float m_maxAngularVelocity;                  // 최대 각속도 (라디안/초)
        float m_minScale;                            // 최소 스케일
        float m_maxScale;                            // 최대 스케일
        
        // 스폰 타이머
        float m_spawnTimer;
        bool m_isPlaying;
        
        // 랜덤 생성기
        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<float> dis;

        ~ParticleSystem();

        void Update();
        void SpawnParticles(float deltaTime);

        void SpawnSingleParticle();

        // 파티클 풀에서 사용 가능한 파티클 가져오기
        Particle* GetParticleFromPool();

        // 활성화된 파티클들 업데이트
        void UpdateParticles(float deltaTime);

        // 단일 파티클 업데이트
        void UpdateParticle(Particle& particle, float deltaTime);

        // 사라짐 효과 적용
        void ApplyFadeEffect(Particle& particle);
        void Render(Matrix3x3& viewMatrix) override;

        void RenderWithRect(Matrix3x3& viewMatrix);
        void RenderWithSprite(Matrix3x3& viewMatrix);

        // 부채꼴 방출 속도 계산
        Vector2 CalculateEmissionVelocity(const EmissionShape& shape);
        Vector2 CalculateEmissionPosition(const EmissionShape& shape);

	public:
        ParticleSystem();

        void Dispose() override { if (IsValidObject(m_particleCommonSprite) && !m_particleCommonSprite->IsDestroyed()) { m_particleCommonSprite->DecreaseRefCount(); m_particleCommonSprite = nullptr; } }

        // 파티클 시스템 재생 시작
        void Play()
        {
            m_isPlaying = true;
            m_spawnTimer = 0;
            m_hasPlayedOnce = false;
        }

        // 파티클 시스템 정지
        void Stop()
        {
            m_isPlaying = false;
        }

        // 모든 파티클 즉시 제거
        void Clear()
        {
            for (auto* particle : m_activeParticles)
            {
                particle->isActive = false;
            }
            m_activeParticles.clear();
        }

        // 설정 함수들
        void SetParticleLifeTime(float lifeTime) { m_particleLifeTime = lifeTime; }
        float GetParticleLifeTime() const { return m_particleLifeTime; }

        void SetFadeOutTime(float fadeTime) { m_fadeOutTime = fadeTime; }
        float GetFadeOutTime() const { return m_fadeOutTime; }

        void SetFadeMode(ParticleFadeMode mode) { m_fadeMode = mode; }
        ParticleFadeMode GetFadeMode() const { return m_fadeMode; }

        void SetEmissionShape(EmissionShape shape) { m_emissionShape = shape; }
        EmissionShape GetEmissionShape() { return m_emissionShape; }

        void SetParticlesPerSpawn(int count) { m_particlesPerSpawn = count; }
        int GetParticlesPerSpawn() const { return m_particlesPerSpawn; }

        void SetSpawnInterval(float interval) { m_spawnInterval = interval; }
        float GetSpawnInterval() const { return m_spawnInterval; }

        // 속도 범위 설정
        void SetSpeedRange(float min, float max)
        {
            m_minSpeed = min;
            m_maxSpeed = max;
        }

        void SetMinSpeed(float speed) { m_minSpeed = speed; }
        void SetMaxSpeed(float speed) { m_maxSpeed = speed; }
        float GetMinSpeed() const { return m_minSpeed; }
        float GetMaxSpeed() const { return m_maxSpeed; }

        void SetMinScale(float scale) { m_minScale = scale; }
        void SetMaxScale(float scale) { m_maxScale = scale; }
        float GetMinScale() const { return m_minScale; }
        float GetMaxScale() const { return m_maxScale; }

        // 각속도 범위 설정
        void SetAngularVelocityRange(float minAngular, float maxAngular)
        {
            m_minAngularVelocity = minAngular;
            m_maxAngularVelocity = maxAngular;
        }

        void SetAngularVelocityRangeDegrees(float minDegrees, float maxDegrees)
        {
            m_minAngularVelocity = minDegrees * Mathf::Deg2Rad;
            m_maxAngularVelocity = maxDegrees * Mathf::Deg2Rad;
        }

        void SetMinAngularVelocity(float angularVel) { m_minAngularVelocity = angularVel; }
        void SetMaxAngularVelocity(float angularVel) { m_maxAngularVelocity = angularVel; }
        void SetMinAngularVelocityDegrees(float angularVelDegree) { m_minAngularVelocity = angularVelDegree * Mathf::Deg2Rad; }
        void SetMaxAngularVelocityDegrees(float angularVelDegree) { m_maxAngularVelocity = angularVelDegree * Mathf::Deg2Rad; }
        float GetMinAngularVelocity() const { return m_minAngularVelocity; }
        float GetMaxAngularVelocity() const { return m_maxAngularVelocity; }
        float GetMinAngularVelocityDegrees() const { return m_minAngularVelocity * Mathf::Rad2Deg; }
        float GetMaxAngularVelocityDegrees() const { return m_maxAngularVelocity * Mathf::Rad2Deg; }


        // 방출 방향 설정 함수들
        void SetEmissionDirection(float angleRadians)
        {
            m_emissionDirection = angleRadians;
        }

        void SetEmissionDirectionDegrees(float angleDegrees)
        {
            m_emissionDirection = angleDegrees * Mathf::Deg2Rad;
        }

        float GetEmissionDirection() const { return m_emissionDirection; }
        float GetEmissionDirectionDegrees() const { return m_emissionDirection * Mathf::Rad2Deg; }

        // 부채꼴 각도 설정 (0 = 직선, π*2 = 원)
        void SetEmissionAngle(float angleRadians)
        {
            // 0 ~ 2π 범위로 제한
            m_emissionAngle = Mathf::Max(0.0f, Mathf::Min(2.0f * Mathf::PI, angleRadians));
        }

        void SetEmissionAngleDegrees(float angleDegrees)
        {
            float radians = angleDegrees * Mathf::Deg2Rad;
            SetEmissionAngle(radians);
        }

        void SetEmissionTangentLength(float length)
        {
            m_emissionTangentLength = length;
        }

        void SetCommonSprite(Sprite* sprite)
        {
            if (m_particleCommonSprite != sprite)
            {
                if (sprite)
                    sprite->IncreaseRefCount();
                if (m_particleCommonSprite)
                    m_particleCommonSprite->DecreaseRefCount();
            }
            m_particleCommonSprite = sprite;
        }

        void SetCommonSprite(Sprite* sprite, Rect srcRect)
        {
            SetCommonSprite(sprite);
            if (m_particleCommonSprite)
            {
                m_particleCommonSprite->SetRect(srcRect);
            }
        }

        void SetSprite(const std::wstring filePath)
        {
            SetCommonSprite(Resource::Load<Sprite>(filePath));
        }

        void SetSprite(const std::wstring filePath, Rect srcRect)
        {
            SetCommonSprite(Resource::Load<Sprite>(filePath));
            if (m_particleCommonSprite)
            {
                m_particleCommonSprite->SetRect(srcRect);
            }
        }

        Sprite* GetCommonSprite() { return m_particleCommonSprite; }

        float GetEmissionAngle() const { return m_emissionAngle; }
        float GetEmissionAngleDegrees() const { return m_emissionAngle * Mathf::Rad2Deg; }

        float GetEmissionTangentLength() const { return m_emissionTangentLength; }

        void SetMaxParticleCount(int count)
        {
            m_maxParticleCount = count;
            m_particlePool.resize(count);
            m_activeParticles.reserve(count);
        }
        int GetMaxParticleCount() const { return m_maxParticleCount; }

        void SetGravity(const Vector2& g) { m_gravity = g; }
        Vector2 GetGravity() const { return m_gravity; }

        // 상태 조회
        bool GetLoop() const { return m_loop; }
        void SetLoop(bool loop) { m_loop = loop; }
        
        bool IsPlaying() const { return m_isPlaying; }
        int GetActiveParticleCount() const { return m_activeParticles.size(); }
	};
}