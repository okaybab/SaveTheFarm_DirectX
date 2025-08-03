#include "ParticleSystem.h"
#include "RenderManager.h"
#include "Transform.h"
#include "TimeManager.h"
#include "IRenderAPI.h"

GOTOEngine::ParticleSystem::~ParticleSystem()
{
    RenderManager::Get()->UnRegisterParticleSystem(this);
}

void GOTOEngine::ParticleSystem::Update()
{
    float deltaTime = TimeManager::Get()->GetDeltaTime();

    // 파티클 스폰
    if (m_isPlaying)
    {
        SpawnParticles(deltaTime);
    }

    // 활성화된 파티클 업데이트
    UpdateParticles(deltaTime);
}

void GOTOEngine::ParticleSystem::SpawnParticles(float deltaTime)
{
    m_spawnTimer += deltaTime;

    if (m_spawnTimer >= m_spawnInterval)
    {
        m_spawnTimer -= m_spawnInterval;

        // 설정된 개수만큼 파티클 생성
        for (int i = 0; i < m_particlesPerSpawn; ++i)
        {
            SpawnSingleParticle();
        }
    }
}

void GOTOEngine::ParticleSystem::SpawnSingleParticle()
{
    Particle* particle = GetParticleFromPool();
    if (!particle) return;

    // Transform 위치를 기준으로 파티클 생성
    Transform* transform = GetGameObject()->GetTransform();
    Vector2 spawnPos = transform ? transform->GetPosition() : Vector2(0, 0);

    // 랜덤 오프셋 추가
    float offsetX = (dis(gen) - 0.5f) * 20.0f;
    float offsetY = (dis(gen) - 0.5f) * 20.0f;
    spawnPos.x += offsetX;
    spawnPos.y += offsetY;

    // 랜덤 속도
    float angle = dis(gen) * 2.0f * 3.14159f;
    float speed = 50.0f + dis(gen) * 100.0f;
    Vector2 velocity(
        cos(angle) * speed,
        sin(angle) * speed - 50.0f  // 약간 위쪽으로 편향
    );

    // 랜덤 각속도
    float angularVel = (dis(gen) - 0.5f) * 6.28f;

    // 랜덤 색상
    Color color(
        255,
        255,
        255,
        255
        //static_cast<uint8_t>(128 + dis(gen) * 127),
        //static_cast<uint8_t>(77 + dis(gen) * 102),
        //static_cast<uint8_t>(26 + dis(gen) * 77),
        //255
    );

    // 랜덤 스케일
    float scale = 0.5f + dis(gen) * 1.0f;
    Vector2 particleScale(scale, scale);

    // 파티클 초기화 (생명주기 = 활성 시간 + 사라지는 시간)
    float totalLifeTime = m_particleLifeTime + m_fadeOutTime;
    particle->Initialize(spawnPos, velocity, angularVel,
        totalLifeTime, color, particleScale, true);

    // 활성화된 파티클 목록에 추가
    m_activeParticles.push_back(particle);
}

GOTOEngine::Particle* GOTOEngine::ParticleSystem::GetParticleFromPool()
{
    // 사용 가능한 파티클 찾기
    for (auto& particle : m_particlePool)
    {
        if (!particle.isActive)
        {
            return &particle;
        }
    }

    // 풀이 가득 찬 경우, 가장 오래된 활성 파티클 강제 회수
    if (!m_activeParticles.empty())
    {
        Particle* oldest = m_activeParticles[0];
        oldest->isActive = false;

        // 활성화된 목록에서 제거
        m_activeParticles.erase(m_activeParticles.begin());

        return oldest;
    }

    return nullptr;
}

void GOTOEngine::ParticleSystem::UpdateParticles(float deltaTime)
{
    // 역순으로 순회하여 안전한 제거
    for (int i = m_activeParticles.size() - 1; i >= 0; --i)
    {
        Particle* particle = m_activeParticles[i];

        UpdateParticle(*particle, deltaTime);

        // 죽은 파티클은 풀로 돌려보냄
        if (!particle->IsAlive())
        {
            particle->isActive = false;
            m_activeParticles.erase(m_activeParticles.begin() + i);
        }
    }
}

void GOTOEngine::ParticleSystem::UpdateParticle(Particle& particle, float deltaTime)
{
    if (!particle.IsAlive()) return;

    // 생명주기 감소
    particle.lifeTime -= deltaTime;

    // 물리 업데이트
    if (particle.useGravity)
    {
        particle.velocity.x += m_gravity.x * deltaTime;
        particle.velocity.y += m_gravity.y * deltaTime;
    }

    // 위치 업데이트
    particle.position.x += particle.velocity.x * deltaTime;
    particle.position.y += particle.velocity.y * deltaTime;

    // 회전 업데이트
    particle.rotation += particle.angularVelocity * deltaTime;

    // 사라짐 효과 적용
    ApplyFadeEffect(particle);
}

void GOTOEngine::ParticleSystem::ApplyFadeEffect(Particle& particle)
{
    float remainingLife = particle.lifeTime;

    // 사라지는 단계인지 확인 (남은 생명이 사라지는 시간보다 적을 때)
    if (remainingLife <= m_fadeOutTime)
    {
        

        // 사라지는 단계에서의 진행률 (0.0 ~ 1.0, 1.0이 완전히 살아있는 상태)
        float fadeRatio = remainingLife / m_fadeOutTime;

        switch (m_fadeMode)
        {
        case ParticleFadeMode::None:
            // 아무 효과 없음
            break;

        case ParticleFadeMode::Shrink:
            // 스케일이 0으로 가까워짐
            particle.scale.x = particle.initialScale.x * fadeRatio;
            particle.scale.y = particle.initialScale.y * fadeRatio;
            break;

        case ParticleFadeMode::Fade:
            // 알파값이 0으로 가까워짐
            particle.color.A = static_cast<uint8_t>(particle.initialColor.A * fadeRatio);
            break;

        case ParticleFadeMode::ShrinkFade:
            // 스케일과 알파값 모두 감소
            particle.scale.x = particle.initialScale.x * fadeRatio;
            particle.scale.y = particle.initialScale.y * fadeRatio;
            particle.color.A = static_cast<uint8_t>(particle.initialColor.A * fadeRatio);
            break;
        }
    }
    else
    {
        // 아직 사라지는 단계가 아니므로 원래 상태 유지
        particle.scale = particle.initialScale;
        particle.color.A = particle.initialColor.A;
    }
}


void GOTOEngine::ParticleSystem::Render(Matrix3x3& viewMatrix)
{
    if (!GetEnabled() || m_activeParticles.empty()) return;

    auto renderAPI = GetRenderAPIFromManager();
    if (!renderAPI) return;

    // TODO: DrawSpriteBatch 구현 대기
    // 현재는 개별 사각형으로 렌더링
    for (const auto* particle : m_activeParticles)
    {
        if (!particle->IsAlive()) continue;

        //피벗 이동
        auto transform = Matrix3x3::Translate(m_particleCommonRect.width * -0.5f, m_particleCommonRect.width * -0.5f);

        //유니티 좌표계 플립
        transform = Matrix3x3::Scale(1.0f, -1.0f) * transform;

        ////TRS 세팅
        transform = Matrix3x3::TRS(
            particle->position,
            particle->rotation, // degree를 radian으로 변환
            particle->scale
        ) * transform;

        ////유니티 좌표계 매트릭스 적용
        transform = viewMatrix * transform;

        renderAPI->DrawRect(
            m_particleCommonRect,
            true,               // 채워진 사각형
            transform,
            particle->color,
            false               // useScreenPos = false
        );
    }
}

GOTOEngine::ParticleSystem::ParticleSystem()
    : m_particleLifeTime(2.0f), m_fadeOutTime(1.0f), m_fadeMode(ParticleFadeMode::Fade),
    m_particlesPerSpawn(1), m_spawnInterval(0.1f), m_maxParticleCount(25), m_particleCommonRect({0.0f,0.0f,1.0f,1.0f}),
    m_gravity(0, -200.0f), m_spawnTimer(0), m_isPlaying(false),
    gen(rd()), dis(0.0f, 1.0f)
{
    // 파티클 풀 초기화 (기본 25개)
    m_particlePool.resize(m_maxParticleCount);
    m_activeParticles.reserve(m_maxParticleCount);

    RenderManager::Get()->RegisterParticleSystem(this);
}