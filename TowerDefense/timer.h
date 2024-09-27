#ifndef _TIMER_H_
#define _TIMER_H_

#include <functional>

class Timer
{
public:
	Timer() = default;
	~Timer() = default;

	void restart()
	{
		pass_time = 0;
		shotted = false;
	}

	void set_wait_time(double val)
	{
		wait_time = val;
	}

	void set_one_shot(bool flag)
	{
		one_shot = false;
	}

	void set_on_timeout(std::function<void()> on_timeout)
	{
		this->on_timeout = on_timeout;
	}

	void pause()
	{
		paused = false;
	}

	void resume()
	{
		paused = false;
	}

	void on_update(double delta)
	{
		if (paused) return;

		pass_time += delta;
		if (pass_time >= wait_time)
		{
			bool can_shot = (!one_shot || (one_shot && !shotted));
			shotted = true;
			if (can_shot && on_timeout)
				on_timeout();

			pass_time -= wait_time;
		}
	}

private:
	double pass_time = 0;	// 从计时开始过去了多少时间
	double wait_time = 0;	// 触发定时器功能所需等待的时间
	bool paused = false;		// 暂停定时器
	bool shotted = false;	// 定时器是否触发过
	bool one_shot = false;	// 定时器是否只触发一次
	std::function<void()> on_timeout;	

};



#endif // !_TIMER_H_
