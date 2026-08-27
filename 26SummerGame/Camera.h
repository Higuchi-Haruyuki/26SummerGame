#pragma once
#include "vector.h"
#include "Component.h"
#include <memory>

class PlayerInput;


class Camera : public Component
{
public:
	Camera(std::weak_ptr<Object> parent);
	~Camera() = default;


	void Init() override;
	void Update() override;

	float GetCameraYaw() const { return m_cameraYaw; }
	float GetCameraPitch() const { return m_cameraPitch;}

	void SetIsEnableMovement(bool b) { m_isEnableMovement = b; }
	void SetIsEnableRotation(bool b) { m_isEnableRotation = b; }
private:
	//コピー禁止
	Camera(const Camera&) = delete;
	Camera& operator = (const Camera&) = delete;
	//ムーブ禁止
	Camera(Camera&&) = delete;
	Camera& operator = (Camera&&) = delete;

private:

	void UpdateZoom();

	void UpdateRotation();

	void UpdateMovement();

private:

	bool m_isEnableMovement = true;
	bool m_isEnableRotation = true;

	//カメラ用変数
	//原点からの距離
	float m_cameraDistance = -1000.0f;
	//水平方向の回転
	float m_cameraYaw = 0.0f;

	float m_targetCameraYaw = 0.0f;

	//垂直方向の回転
	float m_cameraPitch = 0.0f;

	float m_targetCameraPitch = 0.0f;

	Vector m_targetPos = {};

	Vector m_zoomOffset = {};

	Vector m_moveVelocity = {};
};