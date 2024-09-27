#ifndef _WAVE_H_
#define _WAVE_H_

#include "enemy_type.h"

#include <vector>

struct Wave
{
	struct SpawnEvent // 生成事件
	{
		double interval = 10;
		int spawn_point = 1;
		EnemyType enemy_type = EnemyType::Slim;
	};

	double rewards = 0;
	double interval = 0;
	std::vector<SpawnEvent> spawn_event_list;
};

#endif // !_WAVE_H_
