#include "Base_Quest.h"
#include "FactoryManager.h"


Base_Quest::Base_Quest(const std::string& name, const std::string& description) :
	m_factoryManager(FactoryManager::GetInstance()),
	m_name(name),
	m_description(description)
{
}
