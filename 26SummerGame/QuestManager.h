#pragma once
#include <queue>
#include <memory>

class Base_Quest;

class QuestManager
{
public:
	// シングルトンのインスタンスを取得します（C++11以降のMeyers Singleton、スレッドセーフ）。
	static QuestManager& GetInstance();

	void Init();

	void Update();

	/// <summary>
	/// 現在のクエストのポインタを返す。
	/// クエストが存在しないときはnullptrをかえす。
	/// </summary>
	/// <returns></returns>
	Base_Quest* GetCurrentQuest() const;

	// コピー・ムーブを禁止
	QuestManager(const QuestManager&) = delete;
	QuestManager& operator=(const QuestManager&) = delete;
	QuestManager(QuestManager&&) = delete;
	QuestManager& operator=(QuestManager&&) = delete;

private:
	QuestManager() = default;
	virtual ~QuestManager() = default;

	void QuestClearCheck();

	void GenerateQuest();

	/// <summary>
	/// キューの一番前のクエストをアクティブ状態にする。
	/// </summary>
	void SetActiveQuest();

	/// <summary>
	/// キューの一番前のクエストのクリアイベントを購読する。
	/// </summary>
	void ListenQuestClearEvent();

	void NextQuest();

private:
	// クエストデータや状態をここに追加

	std::queue<std::unique_ptr<Base_Quest>> m_questQueue;

};

