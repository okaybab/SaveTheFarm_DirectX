#include <d2d1helper.h>
#include <WICHelper.h>
#include "DWriteHelper.h"
#include "D2DRenderAPI.h"
#include "D2DFont.h"
#include "D2DBitmap.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

using namespace GOTOEngine;

bool D2DRenderAPI::Initialize(IWindow* window)
{
	if (!window)
		return false;

	m_window = window;

	// D3D11 디바이스 생성
	D3D_FEATURE_LEVEL featureLevel;
	D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
	HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT, levels, 1,
		D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &featureLevel, nullptr);
	if (FAILED(hr))
		return false;

	// D2D 팩토리 생성
	D2D1_FACTORY_OPTIONS options = {};
#ifdef _DEBUG
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, m_d2dFactory.GetAddressOf());
	if (FAILED(hr))
		return false;

	// 최신 Factory로 업캐스팅 시도
	hr = m_d2dFactory.As(&m_d2dFactoryAdvanced);
	if (FAILED(hr))
	{
		m_d2dFactoryAdvanced = nullptr;
		m_useAdvancedFeatures = false;
		OutputDebugStringA("Using basic D2D Factory\n");
	}
	else
	{
		m_useAdvancedFeatures = true;
		OutputDebugStringA("Using advanced D2D Factory\n");
	}

	// DXGI 디바이스 설정
	ComPtr<IDXGIDevice> dxgiDevice;
	hr = m_d3dDevice.As(&dxgiDevice);
	if (FAILED(hr))
		return false;

	hr = dxgiDevice.As(&m_dxgiDevice);
	if (FAILED(hr))
	{
		m_dxgiDevice = dxgiDevice;
	}

	ComPtr<IDXGIAdapter> dxgiAdapter;
	hr = m_dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
	if (FAILED(hr))
		return false;

	m_dxgiAdapter = dxgiAdapter;

	// 최신 DXGI 어댑터로 업캐스팅 시도
	hr = dxgiAdapter.As(&m_dxgiAdapterAdvanced);
	if (SUCCEEDED(hr))
	{
		// VRAM 쿼리 지원 여부 확인
		DXGI_QUERY_VIDEO_MEMORY_INFO testMemInfo = {};
		hr = m_dxgiAdapterAdvanced->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &testMemInfo);
		m_supportsVRAMQuery = SUCCEEDED(hr);
	}
	else
	{
		m_dxgiAdapterAdvanced = nullptr;
		m_supportsVRAMQuery = false;
	}

	// D2D Device 생성 (기본 팩토리로만 가능)
	ComPtr<ID2D1Device> d2dDevice;
	if (m_d2dFactoryAdvanced)
	{
		hr = m_d2dFactoryAdvanced->CreateDevice(dxgiDevice.Get(), d2dDevice.GetAddressOf());
	}
	else
	{
		// ID2D1Factory1이 필요함
		ComPtr<ID2D1Factory1> factory1;
		hr = m_d2dFactory.As(&factory1);
		if (SUCCEEDED(hr))
		{
			hr = factory1->CreateDevice(dxgiDevice.Get(), d2dDevice.GetAddressOf());
		}
		else
		{
			OutputDebugStringA("Cannot create D2D Device - ID2D1Factory1 not available\n");
			return false;
		}
	}

	if (FAILED(hr))
		return false;

	// DeviceContext 생성
	hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, m_d2dContext.GetAddressOf());
	if (FAILED(hr))
		return false;

	// 최신 DeviceContext로 업캐스팅 시도
	hr = m_d2dContext.As(&m_d2dContextAdvanced);
	if (SUCCEEDED(hr))
	{
		// SpriteBatch 지원 여부 확인
		ComPtr<ID2D1SpriteBatch> testSpriteBatch;
		hr = m_d2dContextAdvanced->CreateSpriteBatch(testSpriteBatch.GetAddressOf());
		m_supportsSpriteBatch = SUCCEEDED(hr);

		if (m_supportsSpriteBatch)
		{
			OutputDebugStringA("SpriteBatch supported\n");
		}
		else
		{
			OutputDebugStringA("SpriteBatch creation failed\n");
		}
	}
	else
	{
		m_d2dContextAdvanced = nullptr;
		m_supportsSpriteBatch = false;
		OutputDebugStringA("Advanced DeviceContext not available\n");
	}

	// DXGI Factory 생성
	ComPtr<IDXGIFactory> baseFactory;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&baseFactory));
	if (FAILED(hr))
		return false;

	ComPtr<IDXGIFactory2> dxgiFactory2;
	hr = baseFactory.As(&dxgiFactory2);
	if (FAILED(hr))
		return false;

	// SwapChain 생성
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = m_window->GetWidth();
	scDesc.Height = m_window->GetHeight();
	scDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = 2;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.Flags = 0;

	hr = dxgiFactory2->CreateSwapChainForHwnd(
		m_d3dDevice.Get(),
		static_cast<HWND>(m_window->GetNativeHandle()),
		&scDesc,
		nullptr,
		nullptr,
		m_swapChain.GetAddressOf()
	);
	if (FAILED(hr))
		return false;

	// Alt+Enter 비활성화
	dxgiFactory2->MakeWindowAssociation(static_cast<HWND>(m_window->GetNativeHandle()), DXGI_MWA_NO_ALT_ENTER);

	// 백버퍼를 타겟으로 설정
	ComPtr<IDXGISurface> backBuffer;
	hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(hr))
		return false;

	D2D1_BITMAP_PROPERTIES1 bmpProps = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(scDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);

	hr = m_d2dContext->CreateBitmapFromDxgiSurface(backBuffer.Get(), &bmpProps, m_renderTarget.GetAddressOf());
	if (FAILED(hr))
		return false;

	m_d2dContext->SetTarget(m_renderTarget.Get());

	hr = m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_solidColorBrush);
	if (FAILED(hr))
		return false;

	// SpriteBatch 생성 시도 (지원되는 경우만)
	if (m_supportsSpriteBatch && m_d2dContextAdvanced)
	{
		hr = m_d2dContextAdvanced->CreateSpriteBatch(m_spriteBatch.GetAddressOf());
		if (FAILED(hr))
		{
			m_supportsSpriteBatch = false;
			m_spriteBatch = nullptr;
		}
	}

	return true;
}

void D2DRenderAPI::Release()
{
	if (m_defaultFont)
		delete m_defaultFont;

	m_d3dDevice = nullptr;
	m_dxgiAdapter = nullptr;
	m_dxgiAdapterAdvanced = nullptr;
	m_dxgiDevice = nullptr;
	m_swapChain = nullptr;
	m_d2dContext = nullptr;
	m_d2dContextAdvanced = nullptr;
	m_renderTarget = nullptr;
	m_d2dFactory = nullptr;
	m_d2dFactoryAdvanced = nullptr;
	m_solidColorBrush = nullptr;
	m_spriteBatch = nullptr;
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
		2, // 버퍼 개수
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

void GOTOEngine::D2DRenderAPI::DrawBitmap(const IRenderBitmap* bitmap, const Matrix3x3& mat, const Rect& destRect, const Rect& sourceRect, Color color, TextureFiltering filter)
{
	auto d2dTransform = ConvertToD2DMatrix(mat);
	auto d2dBitmap = static_cast<D2DBitmap*>(const_cast<IRenderBitmap*>(bitmap))->GetRaw();

	D2D1_RECT_F dstRect = D2D1::RectF(
		0,
		0,
		destRect.width,
		destRect.height
	);

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

	m_d2dContext->DrawBitmap(
		d2dBitmap,
		&dstRect,
		(static_cast<float>(color.A) / 255.0f),
		mode,
		&srcRect
	);

	m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
}

void D2DRenderAPI::DrawString(const wchar_t* string, const Rect& rect, const IRenderFont* font, size_t size, const IRenderFontStyle& fontStyle, Color color, const Matrix3x3& mat, int hAlignment, int vAlignment)
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

	D2D1_RECT_F layoutRect = D2D1::RectF(0.0f, 0.0f, rect.width, rect.height);
	auto d2dTransform = ConvertToD2DMatrix(mat);

	m_d2dContext->SetTransform(d2dTransform);
	m_d2dContext->DrawText(string, static_cast<UINT32>(wcslen(string)), textFormat, &layoutRect, m_solidColorBrush.Get());

#ifdef _DEBUG
	m_d2dContext->DrawRectangle(layoutRect, m_solidColorBrush.Get());
#endif

	m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
}

void GOTOEngine::D2DRenderAPI::DrawRect(const Rect& rect, bool fill, const Matrix3x3& mat, Color color)
{
	if (!m_solidColorBrush) {
		OutputDebugStringA("SolidColorBrush가 초기화되지 않았습니다.\n");
		return;
	}

	auto d2dTransform = ConvertToD2DMatrix(mat);

	D2D1_RECT_F dstRect = D2D1::RectF(
		0,
		0,
		rect.width,
		rect.height
	);

	m_d2dContext->SetTransform(d2dTransform);
	m_solidColorBrush->SetColor(D2D1::ColorF(static_cast<float>(color.R) / 255.0f, static_cast<float>(color.G) / 255.0f, static_cast<float>(color.B) / 255.0f, static_cast<float>(color.A) / 255.0f));

	if (fill) {
		m_d2dContext->FillRectangle(dstRect, m_solidColorBrush.Get());
	}
	else {
		m_d2dContext->DrawRectangle(dstRect, m_solidColorBrush.Get());
	}

	m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
}

void GOTOEngine::D2DRenderAPI::DrawSpriteBatch(const IRenderBitmap* bitmap, size_t count, const std::vector<Matrix3x3>& mats, const Rect& destRect, const Rect& sourceRect, const std::vector<Color>& colors, TextureFiltering filter)
{
	// SpriteBatch가 지원되지 않는 경우 개별 DrawBitmap 호출로 대체
	if (!m_supportsSpriteBatch || !m_spriteBatch || !m_d2dContextAdvanced)
	{
		for (size_t i = 0; i < count; ++i)
		{
			DrawBitmap(bitmap, mats[i], destRect, sourceRect, colors[i], filter);
		}
		return;
	}

	// SpriteBatch가 지원되는 경우 기존 로직 사용
	m_d2dContextAdvanced->SetTransform(D2D1::IdentityMatrix());

	D2D1_ANTIALIAS_MODE originalAntialiasMode = m_d2dContextAdvanced->GetAntialiasMode();
	m_d2dContextAdvanced->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	m_spriteBatch->Clear();

	auto d2dBitmap = static_cast<D2DBitmap*>(const_cast<IRenderBitmap*>(bitmap))->GetRaw();

	std::vector<D2D1_RECT_F> d2dDestRects(count);
	std::vector<D2D1_RECT_U> d2dSrcRects(count);
	std::vector<D2D1_COLOR_F> d2dColors(count);
	std::vector<D2D1_MATRIX_3X2_F> d2dTransforms(count);

	auto d2dDestY = bitmap->GetHeight() - sourceRect.y - sourceRect.height;

	for (size_t i = 0; i < count; ++i)
	{
		d2dTransforms[i] = ConvertToD2DMatrix(mats[i]);

		d2dDestRects[i] = D2D1::RectF(
			0,
			0,
			destRect.width,
			destRect.height
		);

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

	m_d2dContextAdvanced->DrawSpriteBatch(
		m_spriteBatch.Get(),
		0, count,
		d2dBitmap,
		mode,
		D2D1_SPRITE_OPTIONS_NONE);

	m_d2dContextAdvanced->SetAntialiasMode(originalAntialiasMode);
}

void D2DRenderAPI::SetViewport(Rect rect)
{
	m_d2dContext->SetTransform(D2D1::IdentityMatrix());
	float screenWidth = static_cast<float>(m_window->GetWidth());
	float screenHeight = static_cast<float>(m_window->GetHeight());

	m_clipRect = D2D1::RectF(
		rect.x * screenWidth,
		(1.0f - (rect.y + rect.height)) * screenHeight,
		(rect.x + rect.width) * screenWidth,
		(1.0f - rect.y) * screenHeight
	);

	m_d2dContext->PushAxisAlignedClip(
		m_clipRect,
		D2D1_ANTIALIAS_MODE_ALIASED
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
	IDWriteFactory* dwriteFactory = DWriteHelper::GetFactory();
	if (!dwriteFactory)
		return nullptr;

	// 고급 기능을 사용할 수 있는 경우에만 파일 기반 폰트 로딩 시도
	if (m_useAdvancedFeatures && m_d2dFactoryAdvanced)
	{
		// IDWriteFactory3로 업캐스팅 시도
		ComPtr<IDWriteFactory3> dwriteFactory3;
		HRESULT hr = dwriteFactory->QueryInterface(IID_PPV_ARGS(&dwriteFactory3));
		if (SUCCEEDED(hr))
		{
			OutputDebugStringA("Attempting advanced font loading...\n");

			// FontSetBuilder 사용한 고급 폰트 로딩
			ComPtr<IDWriteFontSetBuilder> baseBuilder;
			hr = dwriteFactory3->CreateFontSetBuilder(&baseBuilder);
			if (SUCCEEDED(hr))
			{
				// IDWriteFontSetBuilder1로 업캐스팅
				ComPtr<IDWriteFontSetBuilder1> fontSetBuilder;
				hr = baseBuilder.As(&fontSetBuilder);
				if (SUCCEEDED(hr))
				{
					// FontFile 생성
					ComPtr<IDWriteFontFile> fontFile;
					hr = dwriteFactory->CreateFontFileReference(filePath.c_str(), nullptr, &fontFile);
					if (SUCCEEDED(hr))
					{
						// FontFile을 FontSetBuilder에 추가
						hr = fontSetBuilder->AddFontFile(fontFile.Get());
						if (SUCCEEDED(hr))
						{
							// FontSet 생성
							ComPtr<IDWriteFontSet> fontSet;
							hr = fontSetBuilder->CreateFontSet(&fontSet);
							if (SUCCEEDED(hr))
							{
								// FontCollection 생성
								ComPtr<IDWriteFontCollection1> fontCollection;
								hr = dwriteFactory3->CreateFontCollectionFromFontSet(fontSet.Get(), &fontCollection);
								if (SUCCEEDED(hr))
								{
									// FontFamily 가져오기
									ComPtr<IDWriteFontFamily1> fontFamily;
									hr = fontCollection->GetFontFamily(0, &fontFamily);
									if (SUCCEEDED(hr))
									{
										// 폰트 패밀리 이름 추출
										ComPtr<IDWriteLocalizedStrings> familyNames;
										hr = fontFamily->GetFamilyNames(&familyNames);
										if (SUCCEEDED(hr))
										{
											UINT32 index = 0;
											BOOL exists = FALSE;

											// 영어 이름 우선 검색
											hr = familyNames->FindLocaleName(L"en-us", &index, &exists);
											if (!exists)
											{
												// 영어가 없으면 첫 번째 이름 사용
												index = 0;
											}

											UINT32 length = 0;
											hr = familyNames->GetStringLength(index, &length);
											if (SUCCEEDED(hr))
											{
												std::wstring fontFamilyName(length + 1, L'\0');
												hr = familyNames->GetString(index, &fontFamilyName[0], length + 1);
												if (SUCCEEDED(hr))
												{
													fontFamilyName.resize(length);

													// D2DFont 객체 생성 및 고급 정보 설정
													D2DFont* d2dFont = new D2DFont(fontFamilyName);
													d2dFont->m_fontCollection = fontCollection;
													d2dFont->m_fontFile = fontFile;

													OutputDebugStringA("Advanced font loading successful!\n");
													std::string debugMsg = "Font family: " + std::string(fontFamilyName.begin(), fontFamilyName.end()) + "\n";
													OutputDebugStringA(debugMsg.c_str());

													return d2dFont;
												}
											}
										}
									}
								}
							}
						}
						else
						{
							OutputDebugStringA("Failed to add font file to FontSetBuilder\n");
						}
					}
					else
					{
						OutputDebugStringA("Failed to create font file reference\n");
					}
				}
				else
				{
					OutputDebugStringA("Failed to cast to IDWriteFontSetBuilder1\n");
				}
			}
			else
			{
				OutputDebugStringA("Failed to create FontSetBuilder\n");
			}
		}
		else
		{
			OutputDebugStringA("IDWriteFactory3 not available for advanced font loading\n");
		}
	}
	else
	{
		OutputDebugStringA("Advanced features not available, trying basic font file loading...\n");

		// 기본적인 폰트 파일 로딩 시도 (제한적)
		ComPtr<IDWriteFontFile> fontFile;
		HRESULT hr = dwriteFactory->CreateFontFileReference(filePath.c_str(), nullptr, &fontFile);
		if (SUCCEEDED(hr))
		{
			// 폰트 파일 유효성 검사
			BOOL isSupportedFontType = FALSE;
			DWRITE_FONT_FILE_TYPE fontFileType;
			DWRITE_FONT_FACE_TYPE fontFaceType;
			UINT32 numberOfFaces = 0;

			hr = fontFile->Analyze(&isSupportedFontType, &fontFileType, &fontFaceType, &numberOfFaces);
			if (SUCCEEDED(hr) && isSupportedFontType && numberOfFaces > 0)
			{
				// FontFace 생성 시도
				ComPtr<IDWriteFontFace> fontFace;
				hr = dwriteFactory->CreateFontFace(fontFaceType, 1, fontFile.GetAddressOf(), 0, DWRITE_FONT_SIMULATIONS_NONE, &fontFace);
				if (SUCCEEDED(hr))
				{
					// 기본 폰트 이름으로 생성 (실제 폰트 이름을 가져오기 어려움)
					std::wstring baseName = filePath.substr(filePath.find_last_of(L"/\\") + 1);
					if (baseName.find(L'.') != std::wstring::npos)
					{
						baseName = baseName.substr(0, baseName.find_last_of(L'.'));
					}

					D2DFont* d2dFont = new D2DFont(baseName);
					d2dFont->m_fontFile = fontFile;

					OutputDebugStringA("Basic font file loading successful\n");
					std::string debugMsg = "Font file: " + std::string(baseName.begin(), baseName.end()) + "\n";
					OutputDebugStringA(debugMsg.c_str());

					return d2dFont;
				}
				else
				{
					OutputDebugStringA("Failed to create font face from font file\n");
				}
			}
			else
			{
				OutputDebugStringA("Font file is not supported or invalid\n");
			}
		}
		else
		{
			OutputDebugStringA("Failed to create font file reference\n");
		}
	}

	// 모든 시도가 실패한 경우 기본 시스템 폰트로 대체
	OutputDebugStringA("All font loading attempts failed, falling back to system font\n");
	return CreateRenderFontFromOS(L"Segoe UI");
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
	TextureFiltering filter)
{
	if (!bitmap || fillAmount <= 0.0f) return;

	fillAmount = max(0.0f, min(1.0f, fillAmount));
	if (fillAmount >= 1.0f)
	{
		DrawBitmap(bitmap, mat, destRect, sourceRect, color, filter);
		return;
	}

	auto d2dBitmap = static_cast<D2DBitmap*>(const_cast<IRenderBitmap*>(bitmap))->GetRaw();
	auto d2dTransform = ConvertToD2DMatrix(mat);

	m_d2dContext->SetTransform(d2dTransform);

	D2D1_RECT_F dstRect = D2D1::RectF(
		0,
		0,
		destRect.width,
		destRect.height
	);

	float centerX = (dstRect.left + dstRect.right) * 0.5f;
	float centerY = (dstRect.top + dstRect.bottom) * 0.5f;
	float radius = min(dstRect.right - dstRect.left, dstRect.bottom - dstRect.top) * 0.5f;

	float startRad = (startAngle - 90.0f) * (M_PI / 180.0f);
	float sweepAngle = 360.0f * fillAmount;
	if (!clockwise) sweepAngle = -sweepAngle;
	float endRad = startRad + (sweepAngle * M_PI / 180.0f);

	ComPtr<ID2D1PathGeometry> pathGeometry;
	ComPtr<ID2D1GeometrySink> geometrySink;
	HRESULT hr = m_d2dFactory->CreatePathGeometry(&pathGeometry);
	if (FAILED(hr)) return;

	hr = pathGeometry->Open(&geometrySink);
	if (FAILED(hr)) return;

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

	ComPtr<ID2D1Layer> layer;
	hr = m_d2dContext->CreateLayer(&layer);
	if (FAILED(hr)) return;

	auto d2dSourceY = bitmap->GetHeight() - sourceRect.y - sourceRect.height;
	D2D1_RECT_F srcRect = D2D1::RectF(
		sourceRect.x,
		d2dSourceY,
		sourceRect.x + sourceRect.width,
		d2dSourceY + sourceRect.height
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

	m_d2dContext->DrawBitmap(
		d2dBitmap,
		&dstRect,
		static_cast<float>(color.A) / 255.0f,
		mode,
		&srcRect
	);

	m_d2dContext->PopLayer();
}

void GOTOEngine::D2DRenderAPI::DrawRectSimple(const Rect& rect, bool fill, Color color)
{
	m_d2dContext->SetTransform(D2D1::IdentityMatrix());

	float screenHeight = static_cast<float>(m_window->GetHeight());
	m_solidColorBrush->SetColor(D2D1::ColorF(static_cast<float>(color.R) / 255.0f, static_cast<float>(color.G) / 255.0f, static_cast<float>(color.B) / 255.0f, static_cast<float>(color.A) / 255.0f));

	D2D1_RECT_F dstRect = D2D1::RectF(
		rect.x,
		(screenHeight - rect.y - rect.height),
		(rect.x + rect.width),
		(screenHeight - rect.y)
	);

	if (fill) {
		m_d2dContext->FillRectangle(dstRect, m_solidColorBrush.Get());
	}
	else {
		m_d2dContext->DrawRectangle(dstRect, m_solidColorBrush.Get());
	}

	m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
}

IRenderFont* GOTOEngine::D2DRenderAPI::CreateRenderFontFromOS(std::wstring fontName)
{
	IDWriteFactory* dwriteFactory = DWriteHelper::GetFactory();
	if (!dwriteFactory)
		return nullptr;

	ComPtr<IDWriteFontCollection> systemFontCollection;
	HRESULT hr = dwriteFactory->GetSystemFontCollection(systemFontCollection.GetAddressOf());
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

	// D2DFont 객체 생성
	D2DFont* d2dFont = new D2DFont(fontName);
	return d2dFont;
}

void D2DRenderAPI::SwapBuffer()
{
	m_d2dContext->EndDraw();
	m_swapChain->Present(m_vSyncInterval, 0);
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

	// VRAM 정보 쿼리 시도 (지원되는 경우만)
	if (m_supportsVRAMQuery && m_dxgiAdapterAdvanced)
	{
		DXGI_QUERY_VIDEO_MEMORY_INFO memInfo = {};
		HRESULT hr = m_dxgiAdapterAdvanced->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memInfo);
		if (SUCCEEDED(hr))
		{
			status.vramUsage = FormatBytes(memInfo.CurrentUsage);
		}
		else
		{
			status.vramUsage = "Query failed";
		}
	}
	else
	{
		status.vramUsage = "Not supported on this Windows version";
	}

	HANDLE hProcess = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS_EX pmc;
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS_EX);

	// 현재 프로세스의 메모리 사용 정보 조회
	if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
	{
		status.dramUsage = FormatBytes(pmc.WorkingSetSize);
		status.pageFileUsage = FormatBytes(pmc.PagefileUsage - pmc.WorkingSetSize);
	}
	else
	{
		status.dramUsage = "N/A";
		status.pageFileUsage = "N/A";
	}

	return status;
}

// 기능 지원 여부를 확인할 수 있는 헬퍼 함수들
bool D2DRenderAPI::SupportsSpriteBatch() const
{
	return m_supportsSpriteBatch;
}

bool D2DRenderAPI::SupportsVRAMQuery() const
{
	return m_supportsVRAMQuery;
}

bool D2DRenderAPI::SupportsAdvancedFeatures() const
{
	return m_useAdvancedFeatures;
}