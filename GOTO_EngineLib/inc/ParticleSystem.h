#pragma once
#include "Renderer.h"
#include "Color.h"
#include "Rect.h"
#include <random>

namespace GOTOEngine
{
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

        // 파티클 설정
        float m_particleLifeTime;                    // 파티클 생명 시간 (활성 상태 유지 시간)
        float m_fadeOutTime;                         // 파티클 사라지는 시간
        ParticleFadeMode m_fadeMode;                 // 파티클 사라짐 효과
        int m_particlesPerSpawn;                     // 한번에 등장할 파티클 수
        float m_spawnInterval;                       // 파티클 생성 주기
        int m_maxParticleCount;                      // 최대 파티클 수
        Rect m_particleCommonRect;                   // 공용 파티클 Rect
        
        // 물리 설정
        Vector2 m_gravity;                           // 중력 벡터
        
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

	public:
        ParticleSystem();

        // 파티클 시스템 재생 시작
        void Play()
        {
            m_isPlaying = true;
            m_spawnTimer = 0;
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

        void SetParticlesPerSpawn(int count) { m_particlesPerSpawn = count; }
        int GetParticlesPerSpawn() const { return m_particlesPerSpawn; }

        void SetSpawnInterval(float interval) { m_spawnInterval = interval; }
        float GetSpawnInterval() const { return m_spawnInterval; }

        void SetMaxParticleCount(int count)
        {
            m_maxParticleCount = count;
            m_particlePool.resize(count);
            m_activeParticles.reserve(count);
        }
        int GetMaxParticleCount() const { return m_maxParticleCount; }

        const Rect& GetParticleCommonRect() const { return m_particleCommonRect; }
        void  SetParticleCommonRect(const Rect& rect) { m_particleCommonRect = rect; }

        void SetGravity(const Vector2& g) { m_gravity = g; }
        Vector2 GetGravity() const { return m_gravity; }

        // 상태 조회
        bool IsPlaying() const { return m_isPlaying; }
        int GetActiveParticleCount() const { return m_activeParticles.size(); }
	};
}