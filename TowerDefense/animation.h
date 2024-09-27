#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "timer.h"

#include <SDL.h>
#include <vector>
#include <functional>

class Animation
{
public:
	typedef std::function<void()> PlayCallback;

public:
	Animation()
	{
		timer.set_one_shot(false);
		timer.set_on_timeout(
			[&]()
			{
				idx_frame++;
				if (idx_frame >= rect_src_list.size())
				{
					idx_frame = is_loop ? 0 : rect_src_list.size() - 1;
					if (!is_loop && on_finished)
						on_finished();
				}
			}
		);
	}

	~Animation() = default;

	void reset()
	{
		timer.restart();

		idx_frame = 0;
	}

	void set_frame_data(SDL_Texture* texture, int num_h, int num_v,const std::vector<int>& idx_list)
	{

	}

private:
	Timer timer;
	bool is_loop = true;
	size_t idx_frame = 0;
	PlayCallback on_finished;
	SDL_Texture* texture = nullptr;
	std::vector<SDL_Rect> rect_src_list;		// Ö¡µÄÔ´¾ØÐÎ
	int width_frame = 0, height_frame = 0;

};

#endif // ! _ANIMATION_H_
