#include "Camera.h"
#include "Transform.h"
#include "RenderManager.h"
#include "IWindow.h"
#include "TimeManager.h"
#include "IRenderAPI.h"
#include "AudioListener.h"
#include "Screen.h"
#include "IWindow.h"

GOTOEngine::Camera* GOTOEngine::Camera::s_mainCam = nullptr;


GOTOEngine::Camera::Camera() : m_depth(0), m_size(1.0f), m_rect(Rect{ 0.0f,0.0f,1.0f,1.0f }), m_backGroundColor(Color{ 0,0,0,255 }/* black */)
, m_renderLayer(static_cast<std::uint32_t>(-1)) , m_isMatrixDirty(true), m_lastPosition(0.0f, 0.0f), m_lastRotation(0.0f)
{
	REGISTER_BEHAVIOUR_MESSAGE(OnEnable);
	REGISTER_BEHAVIOUR_MESSAGE(OnDisable);
	REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);

	m_cachedMatrix.SetIdentity();
	RenderManager::Get()->RegisterCamera(this);
}

GOTOEngine::Camera::~Camera()
{
	RenderManager::Get()->UnRegisterCamera(this);
}

void GOTOEngine::Camera::OnEnable()
{
	m_isMatrixDirty = true;
}

void GOTOEngine::Camera::OnDisable()
{

}

void GOTOEngine::Camera::OnDestroy()
{
	if (s_mainCam == this)
		s_mainCam = nullptr;
}

void GOTOEngine::Camera::SetDepth(int value)
{
	RenderManager::Get()->SetCamSortDirty(); 
	m_depth = value;
}

GOTOEngine::Matrix3x3 GOTOEngine::Camera::GetMatrix()
{
	Transform* transform = GetGameObject()->GetTransform();

	// Transform의 WorldMatrix가 dirty한지 확인 (더 효율적)
	// 또는 단순히 값 비교로 변경 감지
	Vector2 currentPosition = transform->GetPosition();
	float currentRotation = transform->GetRotation();

	// Transform이 변경되었거나 사이즈가 변경된 경우에만 재계산
	if (m_isMatrixDirty ||
		currentPosition != m_lastPosition ||
		currentRotation != m_lastRotation )
	{
		m_cachedMatrix = transform->GetWorldMatrix().Inverse();
		auto currentLossyScale = transform->GetLossyScale();
		m_cachedMatrix = Matrix3x3::Scale(currentLossyScale.x * m_size, currentLossyScale.y * m_size) * m_cachedMatrix;

		// 캐시 상태 업데이트
		m_lastPosition = currentPosition;
		m_lastRotation = currentRotation;
		m_isMatrixDirty = false;
	}


	auto renderAPI = RenderManager::Get()->GetRenderAPI();

	float windowWidth = static_cast<float>(renderAPI->GetWindow().GetWidth());
	float windowHeight = static_cast<float>(renderAPI->GetWindow().GetHeight());

	float xScale = windowWidth / Screen::GetWidth();
	float yScale = windowHeight / Screen::GetHeight();

	return Matrix3x3::Translate(windowWidth * (m_rect.x + (m_rect.width * 0.5f)),
		windowHeight * (1.0f - m_rect.y - (m_rect.height * 0.5f)))
		* Matrix3x3::Scale(1.0f * xScale, -1.0f * yScale) * m_cachedMatrix;
}

GOTOEngine::Camera* GOTOEngine::Camera::GetMainCamera()
{
	if (!s_mainCam) 
	{
		//새로운 메인캠 찾기
		auto mainCamGOs = GameObject::FindGameObjectsWithTag("MainCamera");

		if (mainCamGOs.size() != 0)
			for (auto& camGO : mainCamGOs)
			{
				if (auto mainCam = camGO->GetComponent<Camera>())
				{
					s_mainCam = mainCam;
					break;
				}
			}
	}

	return s_mainCam;
}

GOTOEngine::GameObject* GOTOEngine::Camera::CreateMainCamera()
{
	//카메라 오브젝트를 생성할 씬이 없음
	if (!SceneManager::Get()->GetCurrentScene())
		return nullptr;

	auto cameraGO = new GameObject(L"MainCamera");
	cameraGO->SetTag("MainCamera");
	auto cam = cameraGO->AddComponent<Camera>();
	auto listener = cameraGO->AddComponent<AudioListener>();
	listener->SetAsMainListener();

	if (!s_mainCam)
		s_mainCam = cam;

	return cameraGO;
}

GOTOEngine::GameObject* GOTOEngine::Camera::CreateSubCamera()
{
	//카메라 오브젝트를 생성할 씬이 없음
	if (!SceneManager::Get()->GetCurrentScene())
		return nullptr;

	auto cameraGO = new GameObject(L"SubCamera");
	auto cam = cameraGO->AddComponent<Camera>();

	return cameraGO;
}



GOTOEngine::Vector2 GOTOEngine::Camera::ScreenToViewportPoint(const Vector2& screenPoint)
{
	// 1. 렌더링 API와 윈도우 크기를 가져옵니다.
	auto renderAPI = RenderManager::Get()->GetRenderAPI();
	float windowWidth = static_cast<float>(renderAPI->GetWindow().GetWidth());
	float windowHeight = static_cast<float>(renderAPI->GetWindow().GetHeight());

	// 2. 뷰포트의 픽셀 좌표 및 크기를 계산합니다.
	// m_rect는 (x, y, width, height)로 정규화된 값 (0.0f ~ 1.0f)이며,
	// GOTOEngine의 화면 좌표계 (0,0)은 왼쪽 하단입니다.
	float viewportPixelX = windowWidth * m_rect.x;
	float viewportPixelY = windowHeight * m_rect.y; // 뷰포트의 하단 Y 픽셀 좌표 (화면 하단 기준)
	float viewportPixelWidth = windowWidth * m_rect.width;
	float viewportPixelHeight = windowHeight * m_rect.height;

	// 3. 입력된 screenPoint (화면 좌측 하단 0,0 기준)를 뷰포트의 픽셀 공간으로 변환합니다.
	// screenPoint.x와 screenPoint.y는 이미 GOTOEngine의 좌표계에 맞춰져 있으므로
	// 뷰포트의 좌측 하단 픽셀 좌표를 기준으로 상대적인 위치를 계산합니다.
	float relativeX = screenPoint.x - viewportPixelX;
	float relativeY = screenPoint.y - viewportPixelY; // 뷰포트 하단 기준 상대 Y

	// 4. 상대 픽셀 좌표를 뷰포트의 정규화된 좌표로 변환합니다.
	GOTOEngine::Vector2 viewportPoint;

	if (viewportPixelWidth != 0.0f)
	{
		viewportPoint.x = relativeX / viewportPixelWidth;
	}
	else
	{
		viewportPoint.x = 0.0f; // 뷰포트 너비가 0이면 0으로 처리
	}

	if (viewportPixelHeight != 0.0f)
	{
		// GOTOEngine의 Y축과 뷰포트 좌표계의 Y축 방향이 일치하므로 Y축을 뒤집을 필요가 없습니다.
		viewportPoint.y = relativeY / viewportPixelHeight;
	}
	else
	{
		viewportPoint.y = 0.0f; // 뷰포트 높이가 0이면 0으로 처리
	}

	return viewportPoint;
}
GOTOEngine::Vector2 GOTOEngine::Camera::ScreenToWorldPoint(const Vector2& screenPoint)
{
	auto renderAPI = RenderManager::Get()->GetRenderAPI();

	float windowWidth = static_cast<float>(renderAPI->GetWindow().GetWidth());
	float windowHeight = static_cast<float>(renderAPI->GetWindow().GetHeight());

	float xScale = windowWidth / Screen::GetWidth();
	float yScale = windowHeight / Screen::GetHeight();

	Transform* transform = GetGameObject()->GetTransform();

	// 뷰 행렬 (World → Camera space) 구성
	Matrix3x3 viewMatrix = transform->GetWorldMatrix().Inverse();

	// 전체 스케일 적용 (m_size 포함)
	float sx = xScale * m_size;
	float sy = yScale * m_size;

	// 카메라 중심이 차지하는 스크린 공간 위치 (픽셀 좌표)
	float screenCenterX = windowWidth * (m_rect.x + m_rect.width * 0.5f);
	float screenCenterY = windowHeight * (m_rect.y + m_rect.height * 0.5f);

	// 최종 변환: 화면 좌표 → 월드 좌표
	// step 1: 화면 기준점 보정
	Vector2 centeredScreenPoint = screenPoint - Vector2(screenCenterX, screenCenterY);

	// step 2: 스케일 역변환
	centeredScreenPoint.x /= sx;
	centeredScreenPoint.y /= sy;

	// step 3: 뷰 역변환 적용
	return viewMatrix.MultiplyPoint(centeredScreenPoint);
}

GOTOEngine::Vector2 GOTOEngine::Camera::WorldToViewportPoint(const Vector2& worldPoint)
{

	Transform* transform = GetGameObject()->GetTransform();
	auto renderAPI = RenderManager::Get()->GetRenderAPI();

	float windowWidth = static_cast<float>(renderAPI->GetWindow().GetWidth());
	float windowHeight = static_cast<float>(renderAPI->GetWindow().GetHeight());

	float xScale = windowWidth / Screen::GetWidth();
	float yScale = windowHeight / Screen::GetHeight();

	Matrix3x3 viewMatrix = transform->GetWorldMatrix().Inverse();
	GOTOEngine::Vector2 cameraSpacePoint = viewMatrix.MultiplyPoint(worldPoint);

	float sx = xScale * m_size;
	float sy = yScale * m_size;
	GOTOEngine::Vector2 scaledCameraSpacePoint = Vector2(cameraSpacePoint.x * sx, cameraSpacePoint.y * sy);

	float screenCenterX = windowWidth * (m_rect.x + m_rect.width * 0.5f);
	float screenCenterY = windowHeight * (m_rect.y + m_rect.height * 0.5f);

	GOTOEngine::Vector2 screenPixelPoint = Vector2(
		scaledCameraSpacePoint.x + screenCenterX,
		scaledCameraSpacePoint.y + screenCenterY
	);

	float viewportPixelX = windowWidth * m_rect.x;
	float viewportPixelY = windowHeight * m_rect.y; // 뷰포트의 하단 Y 픽셀 좌표 (화면 하단 기준)
	float viewportPixelWidth = windowWidth * m_rect.width;
	float viewportPixelHeight = windowHeight * m_rect.height;

	float relativeX = screenPixelPoint.x - viewportPixelX;
	float relativeY = screenPixelPoint.y - viewportPixelY;

	GOTOEngine::Vector2 viewportPoint;

	if (viewportPixelWidth != 0.0f)
	{
		viewportPoint.x = relativeX / viewportPixelWidth;
	}
	else
	{
		viewportPoint.x = 0.0f; // 뷰포트 너비가 0이면 0으로 처리
	}

	if (viewportPixelHeight != 0.0f)
	{
		viewportPoint.y = relativeY / viewportPixelHeight;
	}
	else
	{
		viewportPoint.y = 0.0f; // 뷰포트 높이가 0이면 0으로 처리
	}

	return viewportPoint;
}

GOTOEngine::Vector2 GOTOEngine::Camera::WorldToScreenPoint(const Vector2& worldPoint)
{
	Transform* transform = GetGameObject()->GetTransform();
	auto renderAPI = RenderManager::Get()->GetRenderAPI();

	float windowWidth = static_cast<float>(renderAPI->GetWindow().GetWidth());
	float windowHeight = static_cast<float>(renderAPI->GetWindow().GetHeight());

	float xScale = windowWidth / Screen::GetWidth();
	float yScale = windowHeight / Screen::GetHeight();

	// 1. 월드 포인트를 카메라 공간(Camera Local Space)으로 변환합니다.
	// 이는 카메라의 월드 행렬의 역행렬을 곱하는 것과 같습니다.
	Matrix3x3 viewMatrix = transform->GetWorldMatrix().Inverse();
	GOTOEngine::Vector2 cameraSpacePoint = viewMatrix.MultiplyPoint(worldPoint);

	// 2. 카메라의 스케일(m_size 및 lossyScale)을 적용하여 화면 공간 픽셀 스케일에 맞춥니다.
	// ScreenToWorldPoint에서 나눗셈을 했으므로 여기서는 곱셈을 합니다.
	float sx = xScale * m_size;
	float sy = yScale * m_size;
	GOTOEngine::Vector2 scaledCameraSpacePoint = Vector2(cameraSpacePoint.x * sx, cameraSpacePoint.y * sy);

	// 3. 카메라 중심이 차지하는 스크린 공간 위치(픽셀 좌표)를 가져옵니다.
	// 이 좌표는 GOTOEngine의 화면 좌표계 (좌측 하단 0,0 기준)를 따릅니다.

	float screenCenterX = windowWidth * (m_rect.x + m_rect.width * 0.5f);
	float screenCenterY = windowHeight * (m_rect.y + m_rect.height * 0.5f);

	// 4. 스케일이 적용된 카메라 공간 포인트를 화면 픽셀 좌표(좌측 하단 0,0 기준)로 변환합니다.
	// ScreenToWorldPoint에서 뺄셈을 했으므로 여기서는 덧셈을 합니다.
	GOTOEngine::Vector2 screenPixelPoint = Vector2(
		scaledCameraSpacePoint.x + screenCenterX,
		scaledCameraSpacePoint.y + screenCenterY
	);

	return screenPixelPoint;
}

GOTOEngine::Vector2 GOTOEngine::Camera::ViewportToScreenPoint(const Vector2& viewportPoint)
{
	// 1. 렌더링 API와 윈도우 크기를 가져옵니다.
	auto renderAPI = RenderManager::Get()->GetRenderAPI();
	float windowWidth = static_cast<float>(renderAPI->GetWindow().GetWidth());
	float windowHeight = static_cast<float>(renderAPI->GetWindow().GetHeight());

	// 2. 뷰포트의 픽셀 좌표 및 크기를 계산합니다.
	// m_rect는 (x, y, width, height)로 정규화된 값 (0.0f ~ 1.0f)이며,
	// GOTOEngine의 화면 좌표계 (0,0)은 왼쪽 하단입니다.
	float viewportPixelX = windowWidth * m_rect.x;
	float viewportPixelY = windowHeight * m_rect.y; // 뷰포트의 하단 Y 픽셀 좌표 (화면 하단 기준)
	float viewportPixelWidth = windowWidth * m_rect.width;
	float viewportPixelHeight = windowHeight * m_rect.height;

	// 3. 뷰포트의 정규화된 좌표를 뷰포트 내의 상대적인 픽셀 좌표로 변환합니다.
	float relativePixelX = viewportPoint.x * viewportPixelWidth;
	float relativePixelY = viewportPoint.y * viewportPixelHeight;

	// 4. 상대 픽셀 좌표에 뷰포트의 시작 픽셀 좌표를 더하여 최종 화면 픽셀 좌표를 얻습니다.
	GOTOEngine::Vector2 screenPixelPoint = Vector2(
		relativePixelX + viewportPixelX,
		relativePixelY + viewportPixelY
	);

	return screenPixelPoint;
}

GOTOEngine::Vector2 GOTOEngine::Camera::ViewportToWorldPoint(const Vector2& viewportPoint)
{
	// 1. 뷰포트 좌표를 화면 픽셀 좌표로 변환합니다. (ViewportToScreenPoint 함수 재사용)
	GOTOEngine::Vector2 screenPixelPoint = ViewportToScreenPoint(viewportPoint);

	// 2. ScreenToWorldPoint 로직을 역으로 적용하여 화면 픽셀 좌표를 월드 좌표로 변환합니다.
	Transform* transform = GetGameObject()->GetTransform();

	// 카메라 중심이 차지하는 스크린 공간 위치 (픽셀 좌표)
	auto renderAPI = RenderManager::Get()->GetRenderAPI();
	float windowWidth = static_cast<float>(renderAPI->GetWindow().GetWidth());
	float windowHeight = static_cast<float>(renderAPI->GetWindow().GetHeight());


	float xScale = windowWidth / Screen::GetWidth();
	float yScale = windowHeight / Screen::GetHeight();

	float screenCenterX = windowWidth * (m_rect.x + m_rect.width * 0.5f);
	float screenCenterY = windowHeight * (m_rect.y + m_rect.height * 0.5f);

	// step 1: 화면 기준점 보정 (screenPixelPoint에서 screenCenter를 빼서 중앙을 0,0으로 만듭니다)
	Vector2 centeredScreenPoint = screenPixelPoint - Vector2(screenCenterX, screenCenterY);

	// step 2: 스케일 역변환 (ScreenToWorldPoint에서 했던 그대로 나눗셈을 합니다)
	// 0으로 나누는 것을 방지하기 위한 안전 장치 추가
	float sx = xScale * m_size;
	float sy = yScale * m_size;

	if (sx != 0.0f) {
		centeredScreenPoint.x /= sx;
	}
	else {
		centeredScreenPoint.x = 0.0f; // 스케일 X가 0이면 0으로 처리
	}
	if (sy != 0.0f) {
		centeredScreenPoint.y /= sy;
	}
	else {
		centeredScreenPoint.y = 0.0f; // 스케일 Y가 0이면 0으로 처리
	}

	// step 3: 뷰 역변환 적용
	// (World → Camera space) 행렬의 역행렬은 (Camera space → World) 행렬입니다.
	Matrix3x3 viewMatrixInverse = transform->GetWorldMatrix(); // GetWorldMatrix()는 World -> Camera Inverse가 이미 적용되어 있음
	// 아니, ScreenToWorldPoint에서 viewMatrix = transform->GetWorldMatrix().Inverse(); 였습니다.
	// 이는 World -> Camera space 변환입니다.
	// 우리는 Camera space -> World space로 가야하므로, transform->GetWorldMatrix()가 필요합니다.
	Matrix3x3 cameraToWorldMatrix = transform->GetWorldMatrix();


	return cameraToWorldMatrix.MultiplyPoint(centeredScreenPoint);
}