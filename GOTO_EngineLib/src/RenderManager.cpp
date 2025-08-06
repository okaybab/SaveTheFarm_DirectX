#include "RenderManager.h"
#include "IRenderAPI.h"
#include "IRenderFont.h"
#include "IWindow.h"
#include "Camera.h"
#include "Delegate.h"
#include "D2DRenderAPI.h"
#include "Renderer.h"
#include <algorithm>
#include "Matrix3x3.h"
#include "Canvas.h"
#include "RectTransform.h"
#include "Graphic.h"
#include "Screen.h"
#include "ParticleSystem.h"
#ifdef _DEBUG
#include <iostream>
#include "PhysicsManager.h"
#endif

#undef CreateRenderFont

using namespace GOTOEngine;

void RenderManager::StartUp(IWindow* window)
{
#ifdef _USE_DIRECT2D
	m_pRenderAPI = new D2DRenderAPI();
#endif

	if (!m_pRenderAPI->Initialize(window))
	{
		delete m_pRenderAPI;
		m_pRenderAPI = nullptr;
	}
	else
	{
		static_cast<IWindow*>(window)->SetOnChangedWindowSize([this](int width, int height) {
			m_pRenderAPI->ChangeBufferSize(width, height);
#ifdef _DEBUG
			std::cout << width << "," << height << std::endl;
#endif 
			});
	}
}

void GOTOEngine::RenderManager::DrawString(const wchar_t* text, Rect rect, const IRenderFont* font, size_t size, IRenderFontStyle fontStyle, Color color, TextHoriAlign hAlign, TextVertAlign vAlign)
{
	if (rect.x == 0 &&
		rect.y == 0 &&
		rect.width == 0 &&
		rect.height == 0)
	{
		rect = Rect{ 0,0,static_cast<float>(m_pRenderAPI->GetWindow().GetWidth()),static_cast<float>(m_pRenderAPI->GetWindow().GetHeight()) };
	}

	m_pRenderAPI->DrawString(text, rect, font, 24, IRenderFontStyle::Bold, color, Matrix3x3{}, static_cast<int>(hAlign), static_cast<int>(vAlign),true);
}

void GOTOEngine::RenderManager::DrawString(const wchar_t* text, Rect rect, Color color)
{
	m_pRenderAPI->DrawString(text, rect, nullptr, 24, IRenderFontStyle::Bold, color, Matrix3x3{}, -1, 1, true);
}

RenderAPIMemoryStatus RenderManager::CollectMemoryUsage()
{
	return m_pRenderAPI->CollectMemoryUsage();
}

void RenderManager::ShutDown()
{
	m_cameras.clear();

	if (m_pRenderAPI)
	{
		delete m_pRenderAPI;
		m_pRenderAPI = nullptr;
	}
}

void GOTOEngine::RenderManager::RegisterCamera(Camera* cam)
{
	m_cameras.push_back(cam);
	SetCamSortDirty();
}

void GOTOEngine::RenderManager::UnRegisterCamera(Camera* cam)
{
	m_cameras.erase(
		std::remove_if(m_cameras.begin(), m_cameras.end(),
			[cam](const auto& item) { return item == cam; }),
		m_cameras.end());

	SetCamSortDirty();
}

void GOTOEngine::RenderManager::RegisterRenderer(Renderer* renderer)
{
	m_renderers.push_back(renderer);
	SetRendererSortDirty();
}

void GOTOEngine::RenderManager::UnRegisterRenderer(Renderer* renderer)
{
	m_renderers.erase(
		std::remove_if(m_renderers.begin(), m_renderers.end(),
			[renderer](const auto& item) { return item == renderer; }),
		m_renderers.end());
	SetRendererSortDirty();
}

void GOTOEngine::RenderManager::RegisterCanvas(Canvas* canvas)
{
	m_canvases.push_back(canvas);
	SetCanvasSortDirty();
}

void GOTOEngine::RenderManager::UnRegisterCanvas(Canvas* canvas)
{
	m_canvases.erase(
		std::remove_if(m_canvases.begin(), m_canvases.end(),
			[canvas](const auto& item) { return item == canvas; }),
		m_canvases.end());
	SetCanvasSortDirty();
}

void GOTOEngine::RenderManager::RegisterParticleSystem(ParticleSystem* particleSystem)
{
	m_particleSystems.push_back(particleSystem);
}

void GOTOEngine::RenderManager::UnRegisterParticleSystem(ParticleSystem* particleSystem)
{
	m_particleSystems.erase(
		std::remove_if(m_particleSystems.begin(), m_particleSystems.end(),
			[particleSystem](const auto& item) { return item == particleSystem; }),
		m_particleSystems.end());
}

void GOTOEngine::RenderManager::SortCamera()
{
	std::sort(m_cameras.begin(), m_cameras.end(),
		[](Camera* a, Camera* b) {
			return a->GetDepth() < b->GetDepth();
		});
	m_needCamDepthSort = false; // 정렬이 끝났으므로 플래그 초기화
}

void GOTOEngine::RenderManager::SortRenderer()
{
	std::sort(m_renderers.begin(), m_renderers.end(),
		[](Renderer* a, Renderer* b) {
			return a->m_renderOrder < b->m_renderOrder;
		});
	m_needRenderOrderSort = false; // 정렬이 끝났으므로 플래그 초기화
}

void GOTOEngine::RenderManager::SortCanvas()
{
	std::sort(m_canvases.begin(), m_canvases.end(),
		[](Canvas* a, Canvas* b) {
			return a->GetSortOrder() < b->GetSortOrder();
		});
	m_needCanvasOrderSort = false; // 정렬이 끝났으므로 플래그 초기화
}

void GOTOEngine::RenderManager::StartRender()
{
	m_pRenderAPI->Clear();
}

void GOTOEngine::RenderManager::EndRender()
{
	m_pRenderAPI->SwapBuffer();
}

IRenderBitmap* GOTOEngine::RenderManager::CreateRenderBitmap(std::wstring filePath)
{
	return m_pRenderAPI->CreateRenderBitmap(filePath);
}

IRenderFont* GOTOEngine::RenderManager::CreateRenderFontFromFilePath(std::wstring filePath)
{
	return m_pRenderAPI->CreateRenderFontFromFilePath(filePath);
}

IRenderFont* GOTOEngine::RenderManager::CreateRenderFontFromOS(std::wstring fontName)
{
	return m_pRenderAPI->CreateRenderFontFromOS(fontName);
}

void RenderManager::SetVSyncInterval(int interval)
{
	if (m_pRenderAPI)
	{
		m_pRenderAPI->SetVSyncInterval(interval);
	}
}

void GOTOEngine::RenderManager::Render()
{
	//활성카메라, 비활성카메라 설정
	//CheckActiveCamera();
	//CheckActiveRenderer();
	//둘 중 하나라도 없는 경우 렌더 종료
	//if(m_activeCamera.size() == 0 || m_activeRenderer.size() == 0) { Clear(); SwapBuffer(); return; }

	if(m_needCamDepthSort)
		SortCamera();

	if (m_needRenderOrderSort)
		SortRenderer();

	if (m_needCanvasOrderSort)
		SortCanvas();

	//파티클 시스템 업데이트
	for (auto& particleSystem : m_particleSystems)
	{
		if (!particleSystem->GetEnabled())
			continue;

		particleSystem->Update();
	}

	//렌더링
	//멀티 카메라를 구현하려면 렌더타겟(백 버퍼)이 카메라마다 존재해야함
	//활성화된 카메라를 돌면서 Clear -> 렌더링해주고 최종 렌더타겟을 하나에 모아서 Composite(합치기)
	//그리고 그 렌더타겟을 스왑체인이나 메인버퍼로 올려주고 플립핑
	for (const auto& camera : m_cameras)
	{
		if (!camera->GetEnabled())
			continue;

		//카메라 행렬 구하기
		Matrix3x3 cameraMat = camera->GetMatrix();
		auto camRect = camera->GetRect();

		//그리기 전에 카메라 영역 박스색칠 (렌더타겟이 없기 때문에 클리어 대신 씀)
		m_pRenderAPI->SetViewport(camRect);
		m_pRenderAPI->DrawRect(Rect{
			m_pRenderAPI->GetWindow().GetWidth() * camRect.x,
			m_pRenderAPI->GetWindow().GetHeight() * camRect.y,
			m_pRenderAPI->GetWindow().GetWidth() * camRect.width,
			m_pRenderAPI->GetWindow().GetHeight() * camRect.height },
			true,
			Matrix3x3{},
			camera->GetBackGroundColor(),
			true);
		
		for (const auto& renderer : m_renderers)
		{
			if (!renderer->GetEnabled()
				|| (renderer->GetRenderLayer() & camera->GetRenderLayer()) == 0)
				continue;

			//뷰포트 제한
			renderer->Render(cameraMat);
		}

#ifdef _DEBUG
		//Physics 디버그 드로잉
		for (auto pair : PhysicsManager::Get()->GetBody2DWrappers())
		{
			auto wrapper = pair.second;

			if (wrapper->IsInPhysicsWorld())
			{
				Rect drawRect = { 0,0,wrapper->GetBody()->width.x,wrapper->GetBody()->width.y };

				//피벗 이동
				auto transform = Matrix3x3::Translate(drawRect.width * -0.5f, drawRect.height * -0.5f);

				//유니티 좌표계 플립
				transform = Matrix3x3::Scale(1.0f, -1.0f) * transform;

				////TRS 세팅
				transform = Matrix3x3::TRS(
					{ wrapper->GetBody()->position.x,wrapper->GetBody()->position.y },
					wrapper->GetBody()->rotation, // degree를 radian으로 변환
					{ 1.0f, 1.0f }
				) * transform;

				////유니티 좌표계 매트릭스 적용
				transform = cameraMat * transform;

				m_pRenderAPI->DrawRect(drawRect, false, transform, { 0,255,0,255 }, false);
			}
		}
#endif
		m_pRenderAPI->ResetViewport();
	}

	// 캔버스 렌더링
	for (auto canvas : m_canvases)
	{
		if(!canvas->IsActiveAndEnabled())
			continue;

		if(canvas->IsNeedGraphicSort())
			canvas->SortGraphics();

		for (auto graphic : canvas->m_graphics)
		{
			if (!graphic->IsActiveAndEnabled())
				continue;

			graphic->Render();
		}
	}
}

const GOTOEngine::IWindow* GOTOEngine::RenderManager::GetWindow() const
{
	return &m_pRenderAPI->GetWindow();
}

