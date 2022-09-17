#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include "ThreadSafeMap.h"


/* 基于条件变量的任务有序性定时器，不适合高精度的定时任务执行 */
class Timer
{
public:
	struct IntervalS {
		std::chrono::time_point<std::chrono::high_resolution_clock> time_point;
		std::function<void()> func;
		int repeat_id;
		bool operator<(const IntervalS& r) const { return time_point > r.time_point; } // 给优先队列做排序
	};
	Timer() {
		repeated_fun_id.store(0);
	}
	int Size() {
		return queue.size();
	}

	// 入口
	void Start() {
		std::thread([this]() {Run(); }).detach();

	}

	//
	void Stop() {
		is_running.store(false);
	};

	/* 某段时间后执行任务 */
	template<typename R, typename P, typename F, typename ... Args>
	void addDuractionFun(const std::chrono::duration<R, P>& duration, F&& fun, Args&& ... args) {

		IntervalS s;
		s.time_point = std::chrono::high_resolution_clock::now() + duration;
		s.func = std::bind(fun, args...);
		//s.func = std::bind(std::forward<F>(fun), std::forward<Args>(args)...);
		mutex.lock();
		queue.push(s);
		mutex.unlock();
		cond_v.notify_one();
	}

	/* 在指定时间执行任务 */
	template<typename F, typename ...Args>
	void addTimePointFun(std::chrono::time_point<std::chrono::high_resolution_clock>& time_point, F&& f, Args ... args) {
		IntervalS s;
		s.time_point = time_point;
		s.func = std::bind(f, args...);
		mutex.lock();
		queue.push(s);
		mutex.unlock();
		cond_v.notify_one();
	}

	/* 增添有限次数时间间隔任务 */
	template <typename R, typename P, typename F, typename... Args>
	int AddRepeatedFunc(int repeat_num, const std::chrono::duration<R, P>& time, F&& f, Args&&... args) {
		int id = GetNextRepeatedFuncId();
		repeatid_state_map.Emplace(id, RepeatedIdState::Running);
		auto foo = std::bind(f, args...);
		AddRepeatedFuncLocal(repeat_num - 1, time, id, std::move(foo));
		return id;
	}


private:
	enum class RepeatedIdState { Init = 0, Running = 1, Stop = 2 };

	std::priority_queue<IntervalS> queue; // 线程不安全
	std::atomic<bool> is_running;
	std::mutex mutex;
	std::condition_variable cond_v;
	std::atomic<int> repeated_fun_id;
	ThreadSafeMap<int, RepeatedIdState> repeatid_state_map;


	// 获取自增有序函数id
	int GetNextRepeatedFuncId() { return repeated_fun_id++; }

	/* 取消循环任务 */
	void CancelRepeatFunc(int id_) { repeatid_state_map.EraseKey(id_); }

	/* 执行有限次数循环任务 */
	template <typename R, typename P, typename F>
	void AddRepeatedFuncLocal(int repeat_num, const std::chrono::duration<R, P>& time, int id, F&& f) {
		if (!repeatid_state_map.IsKeyExist(id)) {
			return;
		}
		IntervalS s;
		s.repeat_id = id;
		s.time_point = std::chrono::high_resolution_clock::now() + time;
		s.func = [this, repeat_num, time, id, &f]() {
			f();
			if (!repeatid_state_map.IsKeyExist(id) || repeat_num == 0) {
				return;
			}
			AddRepeatedFuncLocal(repeat_num - 1, time, id, f);
		};
		mutex.lock();
		queue.push(s);
		mutex.unlock();
		cond_v.notify_one();


	}

	void Run() {
		is_running.store(true);
		while (is_running.load()) {
			std::unique_lock<std::mutex> lock(mutex);
			if (queue.empty()) { // 线程安全问题
				cond_v.wait(lock);
				continue; // 轮回判断队列空否
			}
			else {
				auto interval = queue.top();
				auto diff = interval.time_point - std::chrono::high_resolution_clock::now();
				if (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() > 0) {
					cond_v.wait_for(lock, diff); // 这个定时器在wait_for 3秒的过程中可以被其他任务所打断唤醒，此时会重新拿最小时间的任务进行等待，周而复始，所以后加入的时间短的任务也能准时执行
				}
				else {
					queue.pop();
					lock.unlock();
					interval.func();
				}
			}
		}
	}
};

