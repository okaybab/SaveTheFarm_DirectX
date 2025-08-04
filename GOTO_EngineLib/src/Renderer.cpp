#include "Renderer.h"
#include "RenderManager.h"

GOTOEngine::Renderer::Renderer() : m_enabled(true), m_renderOrder(0), m_renderLayer(static_cast<std::uint32_t>(-1))
{
	RenderManager::Get()->RegisterRenderer(this);
}

GOTOEngine::Renderer::~Renderer()
{
	RenderManager::Get()->UnRegisterRenderer(this);
}

GOTOEngine::IRenderAPI* GOTOEngine::Renderer::GetRenderAPIFromManager()
{
	return RenderManager::Get()->GetRenderAPI();
}

bool GOTOEngine::Renderer::GetEnabled()
{
	return m_enabled
		&& IsValidObject(GetGameObject())
		&& GetGameObject()->IsActiveInHierarchy();
}

void GOTOEngine::Renderer::SetEnabled(bool enabled)
{
	auto lastValue = m_enabled;
	m_enabled = enabled;

	//렌더러에 보낼 메시지 혹은 마킹
}

int GOTOEngine::Renderer::GetRenderOrder()
{
	return m_renderOrder;
}

void GOTOEngine::Renderer::SetRenderOrder(int value)
{
	m_renderOrder = value;
	RenderManager::Get()->SetRendererSortDirty();
}

void GOTOEngine::Renderer::SetRenderLayer(std::uint32_t layer)
{
	m_renderLayer = layer;
}