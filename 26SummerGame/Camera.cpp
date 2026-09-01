#include "Camera.h"
#include "SquareCollider3D.h"
#include "PlayerInput.h"
#include "Debug.h"
#include <DxLib.h>
#include <algorithm>
#include "vector.h"
#include <cmath>
#include <format>
#include "Object.h"
#include "Game.h"

namespace
{
	//移動速度
	constexpr float kSpeed = 20.0f;

	//加速状態の移動速度
	constexpr float kAccelSpeed = 100.0f;

	//ズーム感度
	constexpr float kZoomSens = -35.0f;

	//水平方向の速度
	constexpr float kCameraYawSpeed = 0.06f;
	//垂直方向の速度
	constexpr float kCameraPitchSpeed = 0.06f;

	// 平滑の減衰係数。値が大きいほど追従が速い。
	constexpr float kMoveSmoothingLambda = 0.15f;

	// Yaw回転の平滑の減衰係数
	constexpr float kYawSmoothingLambda = 0.15f;

	// Pitch回転の指数平滑の減衰係数
	constexpr float kPitchSmoothingLambda = 0.15f;

	//一回の入力における変化量
	constexpr float kChangeValue = 45;

	//<カメラの角度に関する定数>

	constexpr float kMinPitch = 0.0f;

	constexpr float kMaxPitch = DX_PI_F / 2 - 0.001f;

	//カメラ座標の限界値(暫定)

	const Vector kStartCameraPos = { 7900,1000,7900 };

	const Vector kMinCameraPos = { 500,400,500 };

	const Vector kMaxCameraPos = { 15500,3000,15500 };
}

Camera::Camera(std::weak_ptr<Object> parent):
	Component(parent)
{}

void Camera::Init() {
	Component::Init();
	//Zバッファを使用する
	SetUseZBuffer3D(true);
	//Zバッファに書き込みを行う
	SetWriteZBuffer3D(true);
	//バックカリングを行う
	SetUseBackCulling(true);

	SetPosition(kStartCameraPos);
	m_targetPos = kStartCameraPos;

}

void Camera::Update() {
	if (!m_isEnable) return;
	Component::Update();

	const auto& parentObject = GetParentObject().lock();
	if (!parentObject) return;

	UpdateMovement();
	UpdateRotation();
	UpdateZoom();



	float t = 1.0f - expf(-kMoveSmoothingLambda);
	auto temp = (m_targetPos - parentObject->GetPosition()) * t;
	parentObject->SetPosition(parentObject->GetPosition() + temp);

	m_targetPos.Clamp(kMinCameraPos,kMaxCameraPos);

	SetCameraPositionAndAngle(parentObject->GetPosition().ToVECTOR(), m_cameraPitch, m_cameraYaw,0.0f);
	GetCameraFrontVector();

	Debug::Log(std::format("CameraYaw: {}, CameraPitch: {}", m_cameraYaw, m_cameraPitch));

	Debug::Log(std::format("CameraPos: {}", parentObject->GetPosition().ToString()));
	Debug::Log(std::format("CameraGridPos: {}", parentObject->GetGridPosition().ToString()));
	Debug::Log(std::format("CameraChankPos: {}",
		Game::GridPosToChankPos(parentObject->GetGridPosition())
		.ToString()
	));
}

void Camera::SetPosition(const Vector& newPos)
{
	Component::SetPosition(newPos);
	m_targetPos = newPos;
}

void Camera::SetCameraPitch(float pitch)
{
	auto afterPitch = std::clamp(pitch, kMinPitch, kMaxPitch);
	m_targetCameraPitch = afterPitch;
	m_cameraPitch = afterPitch;
}

void Camera::SetCameraYaw(float yaw)
{
	m_targetCameraYaw = yaw;
	m_cameraYaw = yaw;
}

void Camera::UpdateZoom()
{
	m_zoomOffset = 0.0f;

	if (!m_isEnableMovement) return;

	Vector frontVector = GetCameraFrontVector();
	float inputValue = PlayerInput::GetInstance().GetAction("CameraZoom")->GetValue().AsAxis();

	m_zoomOffset = -frontVector * inputValue * kZoomSens;


	m_targetPos += m_zoomOffset;
	if (m_targetPos.m_y > kMaxCameraPos.m_y)
	{
		m_targetPos.m_x -= m_zoomOffset.m_x;
		m_targetPos.m_z -= m_zoomOffset.m_z;
	}
	if (m_targetPos.m_y < kMinCameraPos.m_y)
	{
		m_targetPos.m_x -= m_zoomOffset.m_x;
		m_targetPos.m_z -= m_zoomOffset.m_z;
	}

	m_targetPos.Clamp(kMinCameraPos, kMaxCameraPos);

} 

void Camera::UpdateRotation()
{
	if (!m_isEnableRotation)return;

	Vector inputValue =
	{
		PlayerInput::GetInstance().GetAction("CameraYaw")->GetValue().AsAxis(),
		PlayerInput::GetInstance().GetAction("CameraPitch")->GetValue().AsAxis()
	};

	m_targetCameraYaw += inputValue.m_x * 0.05f;
	float t = 1.0f - expf(-kYawSmoothingLambda);
	auto temp = (m_targetCameraYaw - m_cameraYaw) * t;
	m_cameraYaw += temp;

	m_targetCameraPitch += inputValue.m_y * 0.02f;
	//視点反転を防ぐクランプ
	m_targetCameraPitch = std::clamp(m_targetCameraPitch, kMinPitch, kMaxPitch);

	t = 1.0f - expf(-kPitchSmoothingLambda);
	temp = (m_targetCameraPitch - m_cameraPitch) * t;
	m_cameraPitch += temp;

	//視点反転を防ぐクランプ
	m_cameraPitch = std::clamp(m_cameraPitch, kMinPitch,kMaxPitch);
}

void Camera::UpdateMovement()
{
	PlayerInput& m_playerInput = PlayerInput::GetInstance();

	m_moveVelocity = {};

	if (!m_isEnableMovement)return;

	//入力
	m_moveVelocity.m_x += m_playerInput.GetAction("MoveHorizontal")->GetValue().AsAxis();
	m_moveVelocity.m_z += m_playerInput.GetAction("MoveVertical")->GetValue().AsAxis();

	//正規化
	if (m_moveVelocity.SqLength() > 0.0f)
	{
		m_moveVelocity.Normalize();
	}

	//カメラ基準に回転
	float sinY = sinf(m_cameraYaw);
	float cosY = cosf(m_cameraYaw);

	//速度ベクトルを回転
	float originalX = m_moveVelocity.m_x;
	float originalZ = m_moveVelocity.m_z;
	m_moveVelocity.m_x = originalX * cosY + originalZ * sinY;
	m_moveVelocity.m_z = -originalX * sinY + originalZ * cosY;

	//正規化
	if (m_moveVelocity.SqLength() > 0.0f)
	{
		m_moveVelocity.Normalize();
	}

	float speed = kAccelSpeed;
	
	//加速キーを押しているときは速度を上げる
	if (m_playerInput.GetAction("MoveAccel")->GetPhase() == ButtonPhase::kPressed) speed = kSpeed;

	m_moveVelocity *= speed;



	m_targetPos += m_moveVelocity;

	m_targetPos.Clamp(kMinCameraPos, kMaxCameraPos);

}

