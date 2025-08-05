#include <d2d1helper.h>
#include <WICHelper.h>
#include "DWriteHelper.h"
#include "D2DRenderAPI.h"
#include "D2DFont.h"
#include "D2DBitmap.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <psapi.h>                // GetProcessMemoryInfo, PROCESS_MEMORY_COUNTERS_EX
#pragma comment(lib, "psapi.lib")
//#ifdef _DEBUG
//#include <iostream>
//#endif

using namespace GOTOEngine;

bool D2DRenderAPI::Initialize(IWindow* window)
{
	if (!window)
		return false;

	m_window = window;

	// D3D11 디바이스 생성
	D3D_FEATURE_LEVEL featureLevel;
	D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT, levels, 1,
		D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &featureLevel, nullptr);

	// D2D 팩토리 및 디바이스
	D2D1_FACTORY_OPTIONS options = {};
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, m_d2dFactory.GetAddressOf());

	ComPtr<IDXGIDevice> dxgiDevice;
	m_d3dDevice.As(&dxgiDevice);
	ComPtr<IDXGIAdapter> dxgiAdapter;
	m_d3dDevice.As(&m_dxgiDevice);
	m_dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
	dxgiAdapter.As(&m_dxgiAdapter);

	ComPtr<ID2D1Device7> d2dDevice;
	m_d2dFactory->CreateDevice((dxgiDevice.Get()), d2dDevice.GetAddressOf());
	d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, m_d2dContext.GetAddressOf());

	ComPtr<IDXGIFactory7> dxgiFactory;
	CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));


	// SwapChain 생성
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = m_window->GetWidth();
	scDesc.Height = m_window->GetHeight();
	scDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scDesc.SampleDesc.Count = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = 2;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiFactory->CreateSwapChainForHwnd(m_d3dDevice.Get(), static_cast<HWND>(m_window->GetNativeHandle()), &scDesc, nullptr, nullptr, m_swapChain.GetAddressOf());

	// 백버퍼를 타겟으로 설정
	ComPtr<IDXGISurface> backBuffer;
	m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	D2D1_BITMAP_PROPERTIES1 bmpProps = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(scDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);
	m_d2dContext->CreateBitmapFromDxgiSurface(backBuffer.Get(), &bmpProps, m_renderTarget.GetAddressOf());
	m_d2dContext->SetTarget(m_renderTarget.Get());

	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_solidColorBrush);

	m_d2dContext->CreateSpriteBatch(m_spriteBatch.GetAddressOf());
	return true;
}

void D2DRenderAPI::Release()
{
	if (m_defaultFont)
		delete m_defaultFont;

	m_d3dDevice = nullptr;
	m_dxgiAdapter = nullptr;
	m_dxgiDevice = nullptr;
	m_swapChain = nullptr;
	m_d2dContext = nullptr;
	m_renderTarget = nullptr;
	m_d2dFactory = nullptr;
	m_solidColorBrush = nullptr;
}

void D2DRenderAPI::ChangeBufferSize(int newWidth, int newHeight)
{
	if (!m_swapChain || !m_d2dContext)
		return;

	// 현재 렌더 타겟 연결 해제
	m_d2dContext->SetTarget(nullptr);
	m_renderTarget = nullptr;

	// SwapChain 버퍼 크기 변경
	HRESULT hr = m_swapChain->ResizeBuffers(
		2, // 버퍼 개수 (CreateSwapChainForHwnd에서 설정한 값과 같아야 함)
		newWidth,
		newHeight,
		DXGI_FORMAT_B8G8R8A8_UNORM,
		0
	);

	if (FAILED(hr)) {
		OutputDebugStringW(L"ResizeBuffers 실패\n");
		return;
	}

	// 백버퍼 가져오기
	ComPtr<IDXGISurface> backBuffer;
	hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(hr)) {
		OutputDebugStringW(L"GetBuffer 실패\n");
		return;
	}

	// 렌더 타겟 다시 생성
	D2D1_BITMAP_PROPERTIES1 bmpProps = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);

	hr = m_d2dContext->CreateBitmapFromDxgiSurface(backBuffer.Get(), &bmpProps, m_renderTarget.GetAddressOf());
	if (FAILED(hr)) {
		OutputDebugStringW(L"렌더 타겟 생성 실패\n");
		return;
	}

	m_d2dContext->SetTarget(m_renderTarget.Get());
}

void D2DRenderAPI::Clear()
{
	m_d2dContext->BeginDraw();
	m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::Black));
}

void GOTOEngine::D2DRenderAPI::DrawBitmap(const IRenderBitmap* bitmap, const Matrix3x3& mat, const Rect& destRect, const Rect& sourceRect, Color color, TextureFiltering filter, bool useScreenPos)
{
	auto d2dTransform = ConvertToD2DMatrix(mat);
	auto d2dBitmap = static_cast<D2DBitmap*>(const_cast<IRenderBitmap*>(bitmap))->GetRaw();
	float screenHeight = static_cast<float>(m_window->GetHeight());

	D2D1_RECT_F dstRect;
	if (useScreenPos)
	{
		dstRect = D2D1::RectF(
			destRect.x,
			(screenHeight - destRect.y - destRect.height),
			(destRect.x + destRect.width),
			(screenHeight - destRect.y)
		);
	}
	else
	{
		dstRect = D2D1::RectF(
			0,
			0,
			destRect.width,
			destRect.height
		);
	}

	auto d2dDestY = bitmap->GetHeight() - sourceRect.y - sourceRect.height;
	D2D1_RECT_F srcRect = D2D1::RectF(
		sourceRect.x,
		d2dDestY,
		sourceRect.x + sourceRect.width,
		d2dDestY + sourceRect.height
	);

	D2D1_BITMAP_INTERPOLATION_MODE mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
	switch (filter)
	{
	case TextureFiltering::Nearest:
		mode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
		break;
	case TextureFiltering::Linear:
		mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
		break;
	}

	m_d2dContext->SetTransform(d2dTransform);

	// 색상 변경이 필요한지 확인 (RGB가 모두 255가 아니거나 알파가 255가 아닌 경우)
	//bool needColorEffect = (color.R != 255 || color.G != 255 || color.B != 255 || color.A != 255);

	//if (needColorEffect)
	//{
	//	// Color Matrix Effect 사용
	//	ID2D1Effect* pColorMatrixEffect = nullptr;
	//	HRESULT hr = m_d2dContext->CreateEffect(CLSID_D2D1ColorMatrix, &pColorMatrixEffect);

	//	if (SUCCEEDED(hr))
	//	{
	//		// 색상 매트릭스 설정
	//		float rTint = static_cast<float>(color.R) / 255.0f;
	//		float gTint = static_cast<float>(color.G) / 255.0f;
	//		float bTint = static_cast<float>(color.B) / 255.0f;
	//		float alpha = static_cast<float>(color.A) / 255.0f;

	//		D2D1_MATRIX_5X4_F colorMatrix = {
	//			rTint, 0, 0, 0,        // R 채널
	//			0, gTint, 0, 0,        // G 채널
	//			0, 0, bTint, 0,        // B 채널
	//			0, 0, 0, alpha,        // A 채널
	//			0, 0, 0, 0             // 오프셋
	//		};

	//		pColorMatrixEffect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, colorMatrix);
	//		pColorMatrixEffect->SetInput(0, d2dBitmap);

	//		ID2D1Image* pOutputImage = nullptr;
	//		pColorMatrixEffect->GetOutput(&pOutputImage);

	//		if (pOutputImage)
	//		{
	//			// DrawImage는 픽셀 좌표를 직접 사용 (UV 변환 불필요)
	//			// 하지만 스케일링을 위해서는 transform을 사용해야 함

	//			if (useScreenPos)
	//			{
	//				// 스케일 계산
	//				float scaleX = (dstRect.right - dstRect.left) / (srcRect.right - srcRect.left);
	//				float scaleY = (dstRect.bottom - dstRect.top) / (srcRect.bottom - srcRect.top);

	//				// 새로운 transform 계산 (기존 transform * 스케일 + 위치)
	//				D2D1_MATRIX_3X2_F scaleTransform = D2D1::Matrix3x2F::Scale(scaleX, scaleY);
	//				D2D1_MATRIX_3X2_F translateTransform = D2D1::Matrix3x2F::Translation(
	//					dstRect.left - srcRect.left * scaleX,
	//					dstRect.top - srcRect.top * scaleY
	//				);

	//				D2D1_MATRIX_3X2_F finalTransform = scaleTransform * translateTransform * d2dTransform;
	//				m_d2dContext->SetTransform(finalTransform);
	//			}
	//			
	//			// DrawImage에서는 srcRect를 픽셀 좌표로 직접 사용
	//			m_d2dContext->DrawImage(
	//				pOutputImage,
	//				D2D1::Point2F(srcRect.left, srcRect.top), // 소스 위치
	//				srcRect,                                   // 소스 사각형 (픽셀 좌표)
	//				D2D1_INTERPOLATION_MODE_LINEAR
	//			);

	//			pOutputImage->Release();
	//		}

	//		pColorMatrixEffect->Release();
	//	}
	//	else
	//	{
	//		// Effect 생성 실패시 기본 DrawBitmap 사용 (색상 틴트 없이)
	//		m_d2dContext->DrawBitmap(
	//			d2dBitmap,
	//			&dstRect,
	//			(static_cast<float>(color.A) / 255.0f),
	//			mode,
	//			&srcRect
	//		);
	//	}
	//}
	//else
	{
		// 색상 변경이 필요없는 경우 기존 방식 사용
		m_d2dContext->DrawBitmap(
			d2dBitmap,
			&dstRect,
			(static_cast<float>(color.A) / 255.0f),
			mode,
			&srcRect
		);
	}

	m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
}

void D2DRenderAPI::DrawString(const wchar_t* string, const Rect& rect, const IRenderFont* font, size_t size, const IRenderFontStyle& fontStyle, Color color, const Matrix3x3& mat, int hAlignment, int vAlignment, bool useScreenPos)
{
	if (!string || !m_d2dContext)
		return;

	if (!font)
	{
		if (!m_defaultFont)
			m_defaultFont = new D2DFont(L"Segoe UI");

		font = m_defaultFont;
	}

	IDWriteTextFormat* textFormat = static_cast<D2DFont*>(const_cast<IRenderFont*>(font))->GetRaw(size, fontStyle);
	if (!textFormat)
		return;

	m_solidColorBrush->SetColor(D2D1::ColorF(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f));

	// 정렬 설정
	switch (hAlignment)
	{
	case -1:
		textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		break;
	case 0:
		textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); 
		break;
	case 1:
		textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		break;
	}

	switch (vAlignment)
	{
	case -1:
		textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		break;
	case 0:
		textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	case 1:
		textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	}

	float screenHeight = static_cast<float>(m_window->GetHeight());

	D2D1_RECT_F layoutRect; 
	
	if (useScreenPos)
	{
		layoutRect = D2D1::RectF(rect.x, (screenHeight - rect.y - rect.height), (rect.x + rect.width), (screenHeight - rect.y));
	}
	else
	{
		layoutRect = D2D1::RectF(0.0f, 0.0f, rect.width, rect.height);
	}

	auto d2dTransform = ConvertToD2DMatrix(mat);

	m_d2dContext->SetTransform(d2dTransform);
	m_d2dContext->DrawText(string, static_cast<UINT32>(wcslen(string)), textFormat, &layoutRect, m_solidColorBrush.Get());
}

void GOTOEngine::D2DRenderAPI::DrawRect(const Rect& rect, bool fill, const Matrix3x3& mat, Color color, bool useScreenPos)
{
	if (!m_solidColorBrush) {
		OutputDebugStringA("SolidColorBrush가 초기화되지 않았습니다.\n");
		return;
	}

	auto d2dTransform = ConvertToD2DMatrix(mat);

	float screenHeight = static_cast<float>(m_window->GetHeight());
	m_d2dContext->SetTransform(d2dTransform);
	m_solidColorBrush->SetColor(D2D1::ColorF(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f));

	D2D1_RECT_F dstRect;

	if (useScreenPos)
	{
		dstRect = D2D1::RectF(
			rect.x,
			(screenHeight - rect.y - rect.height),
			(rect.x + rect.width),
			(screenHeight - rect.y)
		);
	}
	else
	{
		dstRect = D2D1::RectF(
			0,
			0,
			rect.width,
			rect.height
		);
	}

	if (fill) {
		m_d2dContext->FillRectangle(dstRect, m_solidColorBrush.Get());
	}
	else {
		m_d2dContext->DrawRectangle(dstRect, m_solidColorBrush.Get());
	}
}

void GOTOEngine::D2DRenderAPI::DrawSpriteBatch(const IRenderBitmap* bitmap, size_t count, const std::vector<Matrix3x3>& mats, const Rect& destRect, const Rect& sourceRect, const std::vector<Color>& colors, TextureFiltering filter, bool useScreenPos)
{
	m_d2dContext->SetTransform(D2D1::IdentityMatrix());

	D2D1_ANTIALIAS_MODE originalAntialiasMode = m_d2dContext->GetAntialiasMode();
	m_d2dContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	m_spriteBatch->Clear();

	auto d2dBitmap = static_cast<D2DBitmap*>(const_cast<IRenderBitmap*>(bitmap))->GetRaw();

	std::vector<D2D1_RECT_F> d2dDestRects(count);
	std::vector<D2D1_RECT_U> d2dSrcRects(count);
	std::vector<D2D1_COLOR_F> d2dColors(count);
	std::vector<D2D1_MATRIX_3X2_F> d2dTransforms(count);

	auto d2dDestY = bitmap->GetHeight() - sourceRect.y - sourceRect.height;
	float screenHeight = static_cast<float>(m_window->GetHeight());

	for (size_t i = 0; i < count; ++i)
	{
		d2dTransforms[i] = ConvertToD2DMatrix(mats[i]);

		if (useScreenPos)
		{
			d2dDestRects[i] = D2D1::RectF(
				destRect.x,
				(screenHeight - destRect.y - destRect.height),
				(destRect.x + destRect.width),
				(screenHeight - destRect.y)
			);
		}
		else
		{
			d2dDestRects[i] = D2D1::RectF(
				0,
				0,
				destRect.width,
				destRect.height
			);
		}


		d2dSrcRects[i] = D2D1::RectU(
			(UINT32)sourceRect.x,
			(UINT32)d2dDestY,
			(UINT32)(sourceRect.x + sourceRect.width),
			(UINT32)(d2dDestY + sourceRect.height)
		);

		d2dColors[i] = D2D1::ColorF(static_cast<float>(colors[i].R / 255.0f), static_cast<float>(colors[i].G / 255.0f), static_cast<float>(colors[i].B / 255.0f), static_cast<float>(colors[i].A / 255.0f));

	}

	m_spriteBatch->AddSprites(count, d2dDestRects.data(), d2dSrcRects.data(), d2dColors.data(), d2dTransforms.data());
	D2D1_BITMAP_INTERPOLATION_MODE mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;

	switch (filter)
	{
	case TextureFiltering::Nearest:
		mode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
		break;
	case TextureFiltering::Linear:
		mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
		break;
	}

	m_d2dContext->DrawSpriteBatch(
		m_spriteBatch.Get(),
		0, count,
		d2dBitmap,
		mode,
		D2D1_SPRITE_OPTIONS_NONE);

	m_d2dContext->SetAntialiasMode(originalAntialiasMode);
}

void D2DRenderAPI::SetViewport(Rect rect)
{
	m_d2dContext->SetTransform(D2D1::IdentityMatrix());
	float screenWidth = static_cast<float>(m_window->GetWidth());
	float screenHeight = static_cast<float>(m_window->GetHeight());

	m_clipRect = D2D1::RectF(
		rect.x * screenWidth,                           // 좌측 x 좌표 (픽셀)
		(1.0f - (rect.y + rect.height)) * screenHeight, // 상단 y 좌표 (픽셀)
		(rect.x + rect.width) * screenWidth,            // 우측 x 좌표 (픽셀)
		(1.0f - rect.y) * screenHeight                  // 하단 y 좌표 (픽셀)
	);

	m_d2dContext->PushAxisAlignedClip(
		m_clipRect,
		D2D1_ANTIALIAS_MODE_ALIASED
		// D2D1_ANTIALIAS_MODE_PER_PRIMITIVE // 클립 경계에 안티앨리어싱 적용
		// D2D1_ANTIALIAS_MODE_ALIASED // 픽셀 완벽한 클리핑이 필요한 경우 (성능상 이점 있을 수 있음)
	);
}

void GOTOEngine::D2DRenderAPI::ResetViewport()
{
	m_d2dContext->PopAxisAlignedClip();
}

IRenderBitmap* GOTOEngine::D2DRenderAPI::CreateRenderBitmap(std::wstring filePath)
{
	ComPtr<IWICBitmapDecoder>     decoder;
	ComPtr<IWICBitmapFrameDecode> frame;
	ComPtr<IWICFormatConverter>   converter;

	HRESULT hr = WICHelper::GetFactory()->CreateDecoderFromFilename(
		filePath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
	if (FAILED(hr)) return nullptr;

	hr = decoder->GetFrame(0, &frame);
	if (FAILED(hr)) return nullptr;

	hr = WICHelper::GetFactory()->CreateFormatConverter(&converter);
	if (FAILED(hr)) return nullptr;

	hr = converter->Initialize(
		frame.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0f,
		WICBitmapPaletteTypeCustom
	);
	if (FAILED(hr)) return nullptr;

	// ⑤ Direct2D 비트맵 속성 (premultiplied alpha, B8G8R8A8_UNORM)
	D2D1_BITMAP_PROPERTIES1 bmpProps = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_NONE,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);

	ComPtr<ID2D1Bitmap1> bitmap;

	hr = m_d2dContext->CreateBitmapFromWicBitmap(converter.Get(), &bmpProps, bitmap.GetAddressOf());

	if (FAILED(hr)) return nullptr;

	return new D2DBitmap(bitmap);
}

IRenderFont* GOTOEngine::D2DRenderAPI::CreateRenderFontFromFilePath(std::wstring filePath)
{
	IDWriteFactory3* dwriteFactory = DWriteHelper::GetFactory();
	if (!dwriteFactory)
		return nullptr;

	// 1. FontSetBuilder (IDWriteFontSetBuilder1) 생성
	ComPtr<IDWriteFontSetBuilder> baseBuilder;
	HRESULT hr = dwriteFactory->CreateFontSetBuilder(&baseBuilder);
	if (FAILED(hr)) return nullptr;

	ComPtr<IDWriteFontSetBuilder1> fontSetBuilder;
	hr = baseBuilder.As(&fontSetBuilder);
	if (FAILED(hr)) return nullptr;

	// 2. FontFile 생성
	ComPtr<IDWriteFontFile> fontFile;
	hr = dwriteFactory->CreateFontFileReference(filePath.c_str(), nullptr, &fontFile);
	if (FAILED(hr)) return nullptr;

	// 3. FontFile을 FontSetBuilder에 추가
	hr = fontSetBuilder->AddFontFile(fontFile.Get());
	if (FAILED(hr)) return nullptr;

	// 4. FontSet 생성
	ComPtr<IDWriteFontSet> fontSet;
	hr = fontSetBuilder->CreateFontSet(&fontSet);
	if (FAILED(hr)) return nullptr;

	// 5. FontCollection 생성
	ComPtr<IDWriteFontCollection1> fontCollection;
	hr = dwriteFactory->CreateFontCollectionFromFontSet(fontSet.Get(), &fontCollection);
	if (FAILED(hr)) return nullptr;

	// 6. FontFamily 이름 추출
	ComPtr<IDWriteFontFamily1> fontFamily;
	hr = fontCollection->GetFontFamily(0, &fontFamily);
	if (FAILED(hr)) return nullptr;

	ComPtr<IDWriteLocalizedStrings> familyNames;
	hr = fontFamily->GetFamilyNames(&familyNames);
	if (FAILED(hr)) return nullptr;

	UINT32 index = 0;
	BOOL exists = FALSE;
	hr = familyNames->FindLocaleName(L"en-us", &index, &exists);
	if (!exists) index = 0;

	UINT32 length = 0;
	hr = familyNames->GetStringLength(index, &length);
	if (FAILED(hr)) return nullptr;

	std::wstring fontFamilyName(length + 1, L'\0');
	hr = familyNames->GetString(index, &fontFamilyName[0], length + 1);
	if (FAILED(hr)) return nullptr;

	fontFamilyName.resize(length);

	// 7. D2DFont 생성 및 반환
	D2DFont* d2dFont = new D2DFont(fontFamilyName);
	d2dFont->m_fontCollection = fontCollection;
	d2dFont->m_fontFile = fontFile; // IDWriteFontFile으로 설정됨

	return d2dFont;
}

void D2DRenderAPI::DrawRadialFillBitmap(
	const IRenderBitmap* bitmap,
	const Matrix3x3& mat,
	const Rect& destRect,
	const Rect& sourceRect,
	float fillAmount,
	float startAngle,
	bool clockwise,
	Color color,
	TextureFiltering filter,
	bool useScreenPos)
{
	if (!bitmap || fillAmount <= 0.0f) return;

	// fillAmount를 0~1로 클램프
	fillAmount = max(0.0f, min(1.0f, fillAmount));

	if (fillAmount >= 1.0f)
	{
		DrawBitmap(bitmap, mat, destRect, sourceRect, color, filter, useScreenPos);
		return;
	}

	auto d2dBitmap = static_cast<D2DBitmap*>(const_cast<IRenderBitmap*>(bitmap))->GetRaw();
	auto d2dTransform = ConvertToD2DMatrix(mat);
	float screenHeight = static_cast<float>(m_window->GetHeight());

	// 원형 중심점과 반지름 계산
	float centerX = destRect.width * 0.5f;
	float centerY = destRect.height * 0.5f;
	float radius = min(destRect.width, destRect.height) * 0.5f;

	// 각도를 라디안으로 변환 (startAngle은 도 단위, 0 = 위쪽)
	float startRad = (startAngle - 90.0f) * (M_PI / 180.0f);
	float sweepAngle = 360.0f * fillAmount;
	if (!clockwise) sweepAngle = -sweepAngle;
	float endRad = startRad + (sweepAngle * M_PI / 180.0f);

	// 기하학적 경로 생성
	ComPtr<ID2D1PathGeometry> pathGeometry;
	ComPtr<ID2D1GeometrySink> geometrySink;

	HRESULT hr = m_d2dFactory->CreatePathGeometry(&pathGeometry);
	if (FAILED(hr)) return;

	hr = pathGeometry->Open(&geometrySink);
	if (FAILED(hr)) return;

	// 부채꼴
	D2D1_POINT_2F startPoint = {
		centerX + radius * cosf(startRad),
		centerY + radius * sinf(startRad)
	};

	D2D1_POINT_2F endPoint = {
		centerX + radius * cosf(endRad),
		centerY + radius * sinf(endRad)
	};

	geometrySink->BeginFigure(D2D1::Point2F(centerX, centerY), D2D1_FIGURE_BEGIN_FILLED);
	geometrySink->AddLine(startPoint);

	float sweepAngleAbs = abs(sweepAngle * M_PI / 180.0f);
	D2D1_ARC_SEGMENT arc = {
		endPoint,
		D2D1::SizeF(radius, radius),
		0.0f,
		clockwise ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
		(sweepAngleAbs > M_PI) ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL
	};
	geometrySink->AddArc(&arc);
	geometrySink->AddLine(D2D1::Point2F(centerX, centerY));
	geometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);

	hr = geometrySink->Close();
	if (FAILED(hr)) return;

	// Layer를 사용하여 클리핑 마스크 적용
	ComPtr<ID2D1Layer> layer;
	hr = m_d2dContext->CreateLayer(&layer);
	if (FAILED(hr)) return;

	// 목적지 사각형 설정
	D2D1_RECT_F dstRect;
	if (useScreenPos)
	{
		dstRect = D2D1::RectF(
			destRect.x,
			(screenHeight - destRect.y - destRect.height),
			(destRect.x + destRect.width),
			(screenHeight - destRect.y)
		);
	}
	else
	{
		dstRect = D2D1::RectF(0, 0, destRect.width, destRect.height);
	}

	// 소스 사각형 설정
	auto d2dDestY = bitmap->GetHeight() - sourceRect.y - sourceRect.height;
	D2D1_RECT_F srcRect = D2D1::RectF(
		sourceRect.x,
		d2dDestY,
		sourceRect.x + sourceRect.width,
		d2dDestY + sourceRect.height
	);

	// 필터링 모드 설정
	D2D1_BITMAP_INTERPOLATION_MODE mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
	switch (filter)
	{
	case TextureFiltering::Nearest:
		mode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
		break;
	case TextureFiltering::Linear:
		mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
		break;
	}

	// 변환 행렬 적용 및 레이어로 클리핑하여 그리기
	m_d2dContext->SetTransform(d2dTransform);

	m_d2dContext->PushLayer(
		D2D1::LayerParameters(
			D2D1::InfiniteRect(),
			pathGeometry.Get(),
			D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
			D2D1::IdentityMatrix(),
			static_cast<float>(color.A) / 255.0f,
			nullptr,
			D2D1_LAYER_OPTIONS_NONE
		),
		layer.Get()
	);

	// 이미지 그리기
	m_d2dContext->DrawBitmap(
		d2dBitmap,
		&dstRect,
		static_cast<float>(color.A) / 255.0f,
		mode,
		&srcRect
	);

	m_d2dContext->PopLayer();
}

IRenderFont* GOTOEngine::D2DRenderAPI::CreateRenderFontFromOS(std::wstring fontName)
{
	IDWriteFactory* dwriteFactory = DWriteHelper::GetFactory();
	// DWriteHelper가 초기화되지 않은 경우 종료
	if (!dwriteFactory)
	{
		return nullptr;
	}

	// 시스템 폰트 컬렉션 가져오기
	ComPtr<IDWriteFontCollection> systemFontCollection;
	HRESULT hr = dwriteFactory->GetSystemFontCollection(
		systemFontCollection.GetAddressOf()
	);

	if (FAILED(hr))
		return nullptr;

	// 폰트 패밀리 찾기
	UINT32 index = 0;
	BOOL exists = FALSE;
	hr = systemFontCollection->FindFamilyName(
		fontName.c_str(),
		&index,
		&exists
	);

	if (FAILED(hr) || !exists)
	{
		// 폰트를 찾지 못했을 경우 기본 폰트 사용
		fontName = L"Segoe UI";
		hr = systemFontCollection->FindFamilyName(
			fontName.c_str(),
			&index,
			&exists
		);

		if (FAILED(hr) || !exists)
			return nullptr;
	}

	// D2DFont 객체 생성 (시스템 폰트의 경우 fontFile과 fontCollection은 nullptr)
	D2DFont* d2dFont = new D2DFont(fontName);

	return d2dFont;
}

void D2DRenderAPI::SwapBuffer()
{
	m_d2dContext->EndDraw();
    m_swapChain->Present(m_vSyncInterval, 0); // vsync 켜고 1 프레임 기다림
}

D2DRenderAPI::~D2DRenderAPI()
{
	Release();
}

std::string GOTOEngine::D2DRenderAPI::FormatBytes(UINT64 bytes)
{
	constexpr double KB = 1024.0;
	constexpr double MB = KB * 1024.0;
	constexpr double GB = MB * 1024.0;

	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2);

	if (bytes >= static_cast<UINT64>(GB))
		oss << (bytes / GB) << " GB";
	else if (bytes >= static_cast<UINT64>(MB))
		oss << (bytes / MB) << " MB";
	else if (bytes >= static_cast<UINT64>(KB))
		oss << (bytes / KB) << " KB";
	else
		oss << bytes << " B";

	return oss.str();
}

RenderAPIMemoryStatus GOTOEngine::D2DRenderAPI::CollectMemoryUsage()
{
	RenderAPIMemoryStatus status;

	DXGI_QUERY_VIDEO_MEMORY_INFO memInfo = {};
	m_dxgiAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memInfo);
	status.vramUsage = FormatBytes(memInfo.CurrentUsage);

	HANDLE hProcess = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS_EX pmc;
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS_EX);

	// 현재 프로세스의 메모리 사용 정보 조회
	GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	status.dramUsage = FormatBytes(pmc.WorkingSetSize);
	status.pageFileUsage = FormatBytes(pmc.PagefileUsage - pmc.WorkingSetSize);

	return status;
}
