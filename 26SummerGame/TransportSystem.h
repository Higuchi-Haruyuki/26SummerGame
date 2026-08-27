#pragma once
#include "Component.h"
#include "Timer.h"
#include "VectorInt.h"
#include <DxLib.h>
#include <memory>

class FactroyManager;
class FactoryComponent;

using Radian = float;

class TransportSystem :
    public Component
{
public:
	TransportSystem(std::weak_ptr<Object> parentObject);
	virtual ~TransportSystem() {};

	void Init() override;
	void Update() override;

	template<std::derived_from<FactoryComponent> T>
	void SetInputAndOutputObject()
	{
		SetInputObjectFromOutputDir();
		SetOutputObjectFromOutputDir<T>();
	}

	void SetInputAndOutputObject()
	{
		SetInputObjectFromOutputDir();
		SetOutputObjectFromOutputDir();
	}

	/*GETTER*/

	Radian GetRotation() const { return m_rotationAngle; }

	std::weak_ptr<FactoryComponent> GetOutputObject() const { return m_output; }

	VectorInt GetOutputDir() const { return m_outputDir.RotationY(m_rotationAngle); }

	VectorInt GetInputPos() const; 

	VectorInt GetOutputPos() const;

	bool IsPreviewMode() const { return m_isPreviewMode; }

	/*SETTER*/

	void SetRotation(Radian angle) { m_rotationAngle = angle; }

	void SetIsPreviewMode(bool b) { m_isPreviewMode = b; }


	bool TransportItemInputToMyself(std::shared_ptr<FactoryComponent> myself);

	bool TransportItemMyselfToOutput(std::shared_ptr<FactoryComponent> myself);


private:

	void SetOutputObject(std::shared_ptr<FactoryComponent> obj);

	void SetInputObject(std::shared_ptr<FactoryComponent> obj);

	std::weak_ptr<FactoryComponent> GetOutputObjectFromOutputDir();

	std::weak_ptr<FactoryComponent> GetInputObjectFromOutputDir();

	void SetInputObjectFromOutputDir()
	{
		const auto& inputComponent = GetInputObjectFromOutputDir();
		if (!inputComponent.lock()) return;
		SetInputObject(inputComponent.lock());
	}

	template<std::derived_from<FactoryComponent> T>
	void SetOutputObjectFromOutputDir()
	{
		const auto& outputComponent = GetOutputObjectFromOutputDir().lock();
		if (!outputComponent) return;

		const auto& classT = std::dynamic_pointer_cast<T>(outputComponent);
		if (!classT) return;

		SetOutputObject(classT);
	}

	void SetOutputObjectFromOutputDir()
	{
		const auto& outputComponent = GetOutputObjectFromOutputDir();
		if (!outputComponent.lock()) return;

		SetOutputObject(outputComponent.lock());
	}

	/// <summary>
	/// アイテムを入力オブジェクトの出力アイテムスロットから、
	/// 出力オブジェクトの入力アイテムスロットに移動させる。
	/// </summary>
	/// <param name="input"></param>
	/// <param name="output"></param>
	void TransportItem(std::shared_ptr<FactoryComponent> input, std::shared_ptr<FactoryComponent> output);

private:
	//シングルトンへの参照
	FactoryManager& m_factoryManager;

	//入力のFactoryComonent
	std::weak_ptr<FactoryComponent> m_input;
	//出力のFactoryComponent
	std::weak_ptr<FactoryComponent> m_output;

	//出力する向き
	VectorInt m_outputDir = { 1,0,0 };

	Radian m_rotationAngle = 0.0f;

	bool m_isPreviewMode = false;
};

