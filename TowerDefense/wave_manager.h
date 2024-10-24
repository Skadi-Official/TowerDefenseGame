#ifndef _WAVE_MANAGER_H_
#define _WAVE_MANAGER_H_

#include "timer.h"
#include "manager.h"
#include "config_manager.h"
#include "enemy_manager.h"
#include "coin_manager.h"

class WaveManager : public Manager<WaveManager>
{
	friend class Manager<WaveManager>;
public:
	void on_update(double delta)
	{
		//其中会重新启动 timer_start_wave
		static ConfigManager* instance = ConfigManager::instance();

		if (instance->is_game_over)
			return;

		if (!is_wave_started)
			timer_start_wave.on_update(delta);
		else
			timer_spawn_enemy.on_update(delta);

		if (is_spawned_last_enemy && EnemyManager::instance()->check_cleared())
		{
			//当前波次结束
			CoinManager::instance()->increase_coin(instance->wave_list[idx_wave].rewards);
			
			idx_wave++;
			if (idx_wave >= instance->wave_list.size())
			{
				instance->is_game_win = true;
				instance->is_game_over = true;
			}
			else
			{
				idx_spawn_event = 0;			//重置索引
				is_wave_started = false;
				is_spawned_last_enemy = false;
				
				const Wave& wave = instance->wave_list[idx_wave];
				timer_start_wave.set_wait_time(wave.interval);
				timer_start_wave.restart();

			}
		}
	}

protected:
	WaveManager()
	{
		static const std::vector<Wave>& wave_list = ConfigManager::instance()->wave_list;

		timer_start_wave.set_one_shot(true);
		timer_start_wave.set_wait_time(wave_list[0].interval);
		timer_start_wave.set_on_timeout(
			[&]()
			{
				is_wave_started = true;
				timer_spawn_enemy.set_wait_time(wave_list[idx_wave].spawn_event_list[0].interval);
				timer_spawn_enemy.restart();
			}
		);

		timer_spawn_enemy.set_one_shot(true);
		timer_spawn_enemy.set_on_timeout(
			[&]()
			{
				const std::vector<Wave::SpawnEvent>& spawn_event_list = wave_list[idx_wave].spawn_event_list;
				const Wave::SpawnEvent& spawn_event = spawn_event_list[idx_spawn_event];

				EnemyManager::instance()->spawn_enemy(spawn_event.enemy_type, spawn_event.spawn_point);
				
				idx_spawn_event++;
				if (idx_spawn_event >= spawn_event_list.size())
				{
					is_spawned_last_enemy = true;
					return;
				}

				timer_spawn_enemy.set_wait_time(spawn_event_list[idx_spawn_event].interval);
				timer_spawn_enemy.restart();
			}
		);
	}

	~WaveManager() = default;

private:
	int idx_wave = 0;						//第几波次
	int idx_spawn_event = 0;				//一个波次里面的第几个生成事件
	Timer timer_start_wave;					//游戏开始后一段时间刷新第一波敌人
	Timer timer_spawn_enemy;				//生成事件的时间间隔
	bool is_wave_started = false;			//波次是否开始
	bool is_spawned_last_enemy = false;		//有没有生成最后一波敌人

};


#endif // !_WAVE_MANAGER_H_
