#pragma once
#include <string>

struct SaveData;

class SaveManager
{
public:
	static SaveManager& GetInstance()
	{
		static SaveManager instance;
		return instance;
	}

	bool Save(const std::string& filePath);

	bool Load(const std::string& filePath);

	bool Exists(const std::string& filePath) const;

private:
	SaveManager() = default;
	~SaveManager() = default;

	SaveManager(const SaveManager&) = delete;
	SaveManager& operator=(const SaveManager&) = delete;

	SaveManager(SaveManager&&) = delete;
	SaveManager& operator=(SaveManager&&) = delete;

private:
	SaveManager CaptureGameState() const;

	bool WriteFile(const SaveData& saveData, const std::string& filePath) const;

	bool ReadFile(SaveData* result, const std::string& filePath) const;

};

