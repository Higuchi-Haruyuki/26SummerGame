#pragma once

class ResourceManager;

enum class SoundId;

/// <summary>
/// サウンドの管理をする
/// </summary>
class SoundManager
{
public:
	static SoundManager& GetInstance()
	{
		static SoundManager instance;
		return instance;
	}

	/// <summary>
	/// サウンドを再生する。
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	bool Play(SoundId id) const;

	/// <summary>
	/// サウンドを指定したボリュームで再生する
	/// </summary>
	/// <param name="id"></param>
	/// <param name="volume">0.0f - 1.0f</param>
	/// <returns></returns>
	bool Play(SoundId id, float volume) const;

	bool PlayLoop(SoundId id) const;

	bool StopLoop(SoundId id) const;

	/// <summary>
	/// サウンドのボリュームを変更する
	/// </summary>
	/// <param name="id"></param>
	/// <param name="volume"></param>
	/// <returns></returns>
	bool ChangeVolume(SoundId id, float volume) const;

private:
	SoundManager();
	~SoundManager() = default;

	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;

	SoundManager(SoundManager&&) = delete;
	SoundManager& operator=(SoundManager&&) = delete;
private:
	ResourceManager& m_resourceManager;

};

