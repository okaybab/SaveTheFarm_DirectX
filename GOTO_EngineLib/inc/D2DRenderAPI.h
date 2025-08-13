#pragma once
#include <unordered_map>
#include <string>
#include <windows.h>
#include <assert.h>
#include <wrl.h>  // ComPtr 사용을 위한 헤더

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <d2d1_3.h> //ID2D1Factory8,ID2D1DeviceContext7
#pragma comment(lib, "d2d1.lib")

#include <dxgi1_6.h> // IDXGIFactory7
#pragma comment(lib, "dxgi.lib")

#include <wincodec.h>
#pragma comment(lib,"windowscodecs.lib")
#include "IRenderAPI.h"
#include "IWindow.h"
#include "Matrix3x3.h"

#include <d2d1effects.h>     
#pragma comment(lib, "dxguid.lib")

using namespace Microsoft::WRL;

namespace GOTOEngine
{
    class D2DRenderAPI : public IRenderAPI
    {
    public:
        bool Initialize(IWindow* window) override;
        void Release() override;
        void ChangeBufferSize(int newWidth, int newHeight) override;
        void Clear() override;
        //void DrawImage(int x, int y, float scale, bool flipX, const IRenderImage* image) override;
        void DrawBitmap(const IRenderBitmap* bitmap, const Matrix3x3& mat, const Rect& destRect, const Rect& sourceRect, Color color, TextureFiltering filter = TextureFiltering::Linear) override;
        void DrawString(const wchar_t* string, const Rect& rect, const IRenderFont* font, size_t size, const IRenderFontStyle& fontStyle, Color color, const Matrix3x3& mat, int hAlignment, int vAlignment) override;
        void DrawRect(const Rect& rect, bool fill, const Matrix3x3& mat, Color color) override;
        void DrawSpriteBatch(const IRenderBitmap* bitmap, size_t count, const std::vector<Matrix3x3>& mats, const Rect& destRect, const Rect& sourceRect, const std::vector<Color>& colors, TextureFiltering filter) override;
        void DrawRadialFillBitmap(
            const IRenderBitmap* bitmap,
            const Matrix3x3& mat,
            const Rect& destRect,
            const Rect& sourceRect,
            float fillAmount,
            float startAngle,
            bool clockwise,
            Color color,
            TextureFiltering filter) override;

        void DrawRectSimple(const Rect& rect, bool fill, Color color) override;

        void SetViewport(Rect rect) override;
        void ResetViewport() override;
        IRenderBitmap* CreateRenderBitmap(std::wstring filePath);
        IRenderFont* CreateRenderFontFromFilePath(std::wstring filePath);
        IRenderFont* CreateRenderFontFromOS(std::wstring fontName);

        void SwapBuffer() override;
        IWindow& GetWindow() const override { return *m_window; }
        void SetVSyncInterval(int interval) override { m_vSyncInterval = interval; }
        ~D2DRenderAPI() override;

        ID2D1DeviceContext* GetContext() { return m_d2dContext.Get(); }
        ID2D1Bitmap1* GetRenderTarget() { return m_renderTarget.Get(); }

        const D2D1_RECT_F& GetClipRect() const { return m_clipRect; }

        D2D1::Matrix3x2F ConvertToD2DMatrix(const Matrix3x3& mat)
        {
            return D2D1::Matrix3x2F(
                mat.m[0][0], mat.m[0][1], // m11, m12 (첫 번째 행)
                mat.m[1][0], mat.m[1][1], // m21, m22 (두 번째 행)  
                mat.m[2][0], mat.m[2][1]  // dx, dy (변환 벡터)
            );
        }

        std::string FormatBytes(UINT64 bytes);
        RenderAPIMemoryStatus CollectMemoryUsage() override;

        bool SupportsSpriteBatch() const;
        bool SupportsVRAMQuery() const;
        bool SupportsAdvancedFeatures() const;
    private:
        ComPtr<ID3D11Device> m_d3dDevice;
        ComPtr<IDXGISwapChain1> m_swapChain;

        // 기본 인터페이스들 (모든 윈도우 버전에서 지원)
        ComPtr<ID2D1DeviceContext> m_d2dContext;
        ComPtr<ID2D1Bitmap1> m_renderTarget;
        ComPtr<ID2D1Factory> m_d2dFactory;
        ComPtr<IDXGIAdapter> m_dxgiAdapter;
        ComPtr<IDXGIDevice> m_dxgiDevice;
        ComPtr<ID2D1SolidColorBrush> m_solidColorBrush;

        // 고급 인터페이스들 (최신 윈도우 버전에서만 지원)
        ComPtr<ID2D1DeviceContext7> m_d2dContextAdvanced;    // SpriteBatch 기능용
        ComPtr<ID2D1Factory8> m_d2dFactoryAdvanced;          // 고급 기능용
        ComPtr<IDXGIAdapter3> m_dxgiAdapterAdvanced;         // VRAM 쿼리용
        ComPtr<ID2D1SpriteBatch> m_spriteBatch;              // SpriteBatch 객체

        IWindow* m_window;
        D2D1_RECT_F m_clipRect;
        IRenderFont* m_defaultFont;
        int m_vSyncInterval = 1;

        // 기능 지원 여부 플래그
        bool m_supportsSpriteBatch = false;
        bool m_supportsVRAMQuery = false;
        bool m_useAdvancedFeatures = false;
    };
}

