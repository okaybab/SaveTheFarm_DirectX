#pragma once
#include "Singleton.h"
#include "Color.h"
#include "Vector2.h"
#include "vector"
#include "TextHelper.h"
#include "Rect.h"

namespace GOTOEngine
{
	class IRenderBitmap;
	class IRenderFont;
	class IRenderAPI;
	class IWindow;
	struct RenderAPIMemoryStatus;
	class RenderManager : public Singleton<RenderManager>
	{
	public:
		void SetVSyncInterval(int interval);
		const IWindow* GetWindow() const;

		//OnGUI용 그리기 함수
		void DrawString(
			const wchar_t* text,
			Rect rect = Rect{ 0,0,0,0 },
			const IRenderFont* font = nullptr,
			size_t size = 24,
			IRenderFontStyle fontStyle = IRenderFontStyle::Bold,
			Color color = Color{ 255,255,255,255 },
			TextHoriAlign hAlign = TextHoriAlign::Left,
			TextVertAlign vAlign = TextVertAlign::Up);

		void DrawString(
			const wchar_t* text,
			Rect rect,
			Color color);

		void SetTargetWidth(const float& width)
		{
			m_targetWidth = width;
		}

		void SetTargetHeight(const float& height)
		{
			m_targetHeight = height;
		}

		const float& GetTargetWidth() const
		{
			return m_targetWidth;
		}

		const float& GetTargetHeight() const
		{
			return m_targetHeight;
		}

		RenderAPIMemoryStatus CollectMemoryUsage();
	private:
		friend class Engine;
		friend class Camera;
		friend class Renderer;
		friend class Texture2D; 
		friend class Font;
		friend class Canvas;
		friend class Graphic;
		friend class ParticleSystem;
		void StartUp(IWindow* window);
		void ShutDown();
		IRenderAPI* m_pRenderAPI = nullptr;

		std::vector<Camera*> m_cameras;
		std::vector<Renderer*> m_renderers;
		std::vector<Canvas*> m_canvases;
		std::vector<ParticleSystem*> m_particleSystems;

		float m_targetWidth;
		float m_targetHeight;

		IRenderAPI* GetRenderAPI() { return m_pRenderAPI; }

		void RegisterCamera(Camera* cam);
		void UnRegisterCamera(Camera* cam);
		void RegisterRenderer(Renderer* renderer);
		void UnRegisterRenderer(Renderer* renderer);
		void RegisterCanvas(Canvas* canvas);	
		void UnRegisterCanvas(Canvas* canvas);
		void RegisterParticleSystem(ParticleSystem* particleSystem);
		void UnRegisterParticleSystem(ParticleSystem* particleSystem);

		void SortCamera();
		void SortRenderer();
		void SortCanvas();

		void SetCamSortDirty() { m_needCamDepthSort = true; }
		void SetRendererSortDirty() { m_needRenderOrderSort = true; }
		void SetCanvasSortDirty() { m_needCanvasOrderSort = true; }

		/// <summary>
		/// 렌더링을 시작합니다.
		/// 1. BeginDraw
		/// 2. Clear
		/// </summary>
		void StartRender();

		/// <summary>
		/// 렌더목록을 이용해여 렌더타겟에 그립니다.
		/// 1. Camera Sort
		/// 2. Renderer Sort
		/// 3. Make Camera View Matrix
		/// 4. Set Viewport
		/// 5. Camera -> Renderer loop
		/// 6. Reset Viewport
		/// </summary>
		void Render();

		/// <summary>
		/// 렌더링을 종료하고 렌더타겟을 화면에 송출합니다.
		/// 1. EndDraw
		/// 2. SwapBuffer
		/// </summary>
		void EndRender();

		IRenderBitmap* CreateRenderBitmap(std::wstring filePath);
		IRenderFont* CreateRenderFontFromFilePath(std::wstring filePath);
		IRenderFont* CreateRenderFontFromOS(std::wstring fontName);

		bool m_needCamDepthSort = false;
		bool m_needRenderOrderSort = false;
		bool m_needCanvasOrderSort = false;
	};
}

//#define RENDER_DRAW_RECT GOTOEngine::RenderManager::Get()->DrawRect
//#define RENDER_DRAW_RECT GOTOEngine::RenderManager::Get()->DrawBitmap
#define RENDER_DRAW_RECT GOTOEngine::RenderManager::Get()->DrawString