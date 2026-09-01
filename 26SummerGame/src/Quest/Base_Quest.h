#pragma once
#include <string>

class FactoryManager;

class Base_Quest
{
public:
	Base_Quest(const std::string& name, const std::string& description);

	virtual ~Base_Quest() = default;

	virtual void Init() {};

	virtual bool ClearCheck() { return false; };

	virtual float GetProgress() const { return 0.0f; };
	/// <summary>
	/// 説明を取得
	/// </summary>
	/// <returns></returns>
	const std::string& GetDescription() const { return m_description; }

protected:

	FactoryManager& m_factoryManager;

private:

	std::string m_name;

	std::string m_description;

	//クリア済みか
	bool m_isClear = false;
};

