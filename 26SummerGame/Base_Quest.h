#pragma once
#include <functional>
#include <string>

class FactoryManager;

class Base_Quest
{
public:
	Base_Quest(const std::string& name, const std::string& description);

	virtual ~Base_Quest() = default;

	virtual void Init() {};

	virtual void ClearCheck() {};

	/// <summary>
	/// 説明を取得
	/// </summary>
	/// <returns></returns>
	const std::string& GetDescription() const { return m_description; }

	void SetOnClear(const std::function<void()>& onClear)
	{
		m_onClear = onClear;
	}

protected:

	FactoryManager& m_factoryManager;

	std::function<void()> m_onClear;

private:

	std::string m_name;

	std::string m_description;

	//クリア済みか
	bool m_isClear = false;
};

