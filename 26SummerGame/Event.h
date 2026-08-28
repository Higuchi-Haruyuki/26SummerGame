#pragma once
#include <array>
#include <memory>
#include <functional>
#include <optional>
#include <utility>

template<typename... Args>
class Event
{
public:

	/// <summary>
	/// 購読を表すRAIIハンドル。
	/// スコープを抜けて、このオブジェクトが破棄されると自動的に購読解除される。
	/// </summary>
	class Connection
	{
		friend class Event<Args...>;

	public:
		Connection() = default;
		~Connection() { Disconnect(); };

		//コピー禁止
		Connection(const Connection&) = delete;
		Connection& operator =(const Connection&) = delete;

		//ムーブ
		Connection(Connection&& other) noexcept :
			m_event(std::exchange(other.m_event, nullptr)),
			m_id(std::exchange(other.m_id, -1)),
			m_generation(other.m_generation)
		{

		}

		Connection& operator=(Connection&& other) noexcept
		{
			if (this != &other)
			{
				Disconnect();
				m_event = std::exchange(other.m_event, nullptr);
				m_id = std::exchange(other.m_id, -1);
				m_generation = other.m_generation;
			}
			return *this;
		}

		// 明示的に購読を解除する。
		void Disconnect()
		{
			if (m_event)
			{
				m_event->RemoveListenerIfCurrent(m_id, m_generation);
				m_event = nullptr;
				m_id = -1;
			}
		}

		// Eventの管理から切り離す。以後、破棄されても購読解除しなくなる。
		void Release()
		{
			m_event = nullptr;
			m_id = -1;
		}

		bool IsConnected() const { return m_event != nullptr; }

	private:
		Connection(Event<Args...>* event, int id, std::uint64_t generation)
			: m_event(event), m_id(id), m_generation(generation)
		{}

		Event<Args...>* m_event = nullptr;
		int m_id = -1;
		std::uint64_t m_generation = 0;
	};

public:

	Event() {};
	~Event()
	{
		RemoveAllListener();
	};

	// アドレスが変わるコピー/ムーブは禁止する。
	Event(const Event&) = delete;
	Event& operator=(const Event&) = delete;
	Event(Event&&) = delete;
	Event& operator=(Event&&) = delete;

	/// <summary>
	/// イベントに関数オブジェクトを登録する。
	/// 登録したものはイベント発火時に呼び出される。
	/// </summary>
	/// <typeparam name="F"></typeparam>
	/// <param name="func"></param>
	/// <returns></returns>
	template<std::invocable<Args...> F>
	Connection AddListener(F func)
	{
		const auto id = FindEmptyID();

		if (!id.has_value()) return Connection{};

		const auto generation = m_nextGeneration;
		m_nextGeneration++;

		m_slots[id.value()] = Slot{std::move(func), generation};

		return Connection{ this,id.value(),generation };
 	}

	void RemoveListener(int id)
	{
		m_slots.at(id) = Slot{};
	}

	void RemoveAllListener()
	{
		for (auto& slot : m_slots)
		{
			slot = Slot{};
		}
	}

	void Invoke(Args... args)
	{
		for (auto& slot : m_slots)
		{
			if (slot.func) slot.func(args...);
		}
	}

	int GetListerCount() const
	{
		int count = 0;
		
		for (const auto& slot : m_slots)
		{
			if (slot.func) count++;
		}

		return count;
	}

private:
	std::optional<int> FindEmptyID() const
	{
		for (int i = 0; i < m_slots.size(); i++)
		{
			if (!m_slots[i].func) return i;
		}

		return std::nullopt;
	}

	// idのリスナーが解除/上書きされずに残っている場合のみ解除する。
	void RemoveListenerIfCurrent(int id, std::uint64_t generation)
	{
		if (id < 0 || id >= m_slots.size()) return;

		if (m_slots[id].generation == generation) m_slots[id] = Slot{};

	}

private:
	struct Slot
	{
		std::function<void(Args...)> func;
		std::uint64_t generation = 0;
	};

	static const int kMaxListener = 10;
	std::array<Slot, kMaxListener> m_slots{};

	//再利用された同じIDを古いConnectionが解除するのを防ぐ世代番号
	std::uint64_t m_nextGeneration = 1;
};

