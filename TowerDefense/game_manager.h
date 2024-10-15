#ifndef  _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

#include "manager.h"
#include "config_manager.h"
#include "resources_manager.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

class GameManager : public Manager<GameManager>
{
	// 指定另一个类或函数作为当前类的友元，可以访问当前类的所有 私有 和 受保护 成员
	friend class Manager<GameManager>;

public:
	int run(int argc, char** argv)
	{
		Uint64 last_counter = SDL_GetPerformanceCounter();
		const Uint64 counter_freq = SDL_GetPerformanceFrequency();
		while (!is_quit)
		{
			while (SDL_PollEvent(&event))
			{
				on_input();
			}

			Uint64 current_counter = SDL_GetPerformanceCounter();
			double delta = (double)(current_counter - last_counter) / counter_freq;
			last_counter = current_counter;
			if (delta * 1000 < 1000.0 / 60)
			{
				SDL_Delay((Uint64)(1000.0 / 60 - delta * 1000));
			}

			// 更新数据
			on_update(delta);

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			// 绘制画面
			on_render();

			SDL_RenderPresent(renderer);

		}

		return 0;
	}

protected:
	GameManager()
	{
		init_assert(!SDL_Init(SDL_INIT_EVERYTHING), u8"SDL2 初始化失败");
		init_assert(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG), u8"SDL_image 初始化失败");
		init_assert(Mix_Init(MIX_INIT_MP3), u8"SDL_mixer 初始化失败");
		init_assert(!TTF_Init(), u8"SDL_TTF 初始化失败");

		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

		SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1"); // 启用输入法编辑器（IME）时显示其用户界面

		ConfigManager* config = ConfigManager::instance();

		init_assert(config->map.load("map.csv"), u8"加载游戏地图失败");
		init_assert(config->load_level_config("level.json"), u8"加载游戏关卡失败");
		init_assert(config->load_game_config("config.json"), u8"加载游戏配置失败");

		window = SDL_CreateWindow(config->basic_template.window_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			config->basic_template.window_width, config->basic_template.window_height, SDL_WINDOW_SHOWN);
		init_assert(window, u8"创建游戏窗口失败");

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
		init_assert(renderer, u8"创建渲染器失败");

		init_assert(ResourcesManager::instance()->load_from_file(renderer), u8"加载游戏资源失败");

		init_assert(generate_tile_map_texture(), u8"瓦片地图加载失败");

	}

	~GameManager()
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);

		TTF_Quit();
		Mix_Quit();
		IMG_Quit();
		SDL_Quit();
	}

private:
	SDL_Event event;
	bool is_quit = false;

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	SDL_Texture* tex_tile_map = nullptr;

private:
	void init_assert(bool flag, const char* err_msg)
	{
		if (flag) return;

		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"游戏启动失败", err_msg, window);
		exit(-1);
	}

	void on_input()
	{

	}

	void on_update(double delta)
	{

	}

	void on_render()
	{
		static ConfigManager* instance = ConfigManager::instance();
		static SDL_Rect& rect_dst = instance->rect_tile_map;
		SDL_RenderCopy(renderer, tex_tile_map, nullptr, &rect_dst);
	}

	bool generate_tile_map_texture()
	{
		const Map& map = ConfigManager::instance()->map;
		const TileMap& tile_map = map.get_tile_map();
		SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;
		SDL_Texture* tex_tile_set = ResourcesManager::instance()->get_texture_pool().find(ResID::Tex_Tileset)->second;

		int width_tex_tile_set, height_tex_tile_set; // 整个纹理的宽高
		SDL_QueryTexture(tex_tile_set, nullptr, nullptr, &width_tex_tile_set, &height_tex_tile_set);
		int num_tile_single_line = (int)std::ceil((double)width_tex_tile_set / SIZE_TILE); // 瓦片地图中一行的格子数量

		int width_tex_tile_map, height_tex_tile_map;
		width_tex_tile_map = (int)map.get_width() * SIZE_TILE;		// 整个地图的总像素宽度
		height_tex_tile_map = (int)map.get_height() * SIZE_TILE;	// 整个地图的总像素高度

		tex_tile_map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, 
			SDL_TEXTUREACCESS_TARGET, width_tex_tile_map, height_tex_tile_map);
		if (!tex_tile_map) return false;

		ConfigManager* config = ConfigManager::instance();
		rect_tile_map.x = (config->basic_template.window_width - width_tex_tile_map) / 2;
		rect_tile_map.y = (config->basic_template.window_height - height_tex_tile_map) / 2;
		rect_tile_map.w = width_tex_tile_map;
		rect_tile_map.h = height_tex_tile_map;

		SDL_SetTextureBlendMode(tex_tile_map, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(renderer, tex_tile_map);

		for (int y = 0; y < map.get_height(); y++)
		{
			for (int x = 0; x < map.get_width(); x++)
			{
				SDL_Rect rect_src;
				const Tile& tile = tile_map[y][x];

				const SDL_Rect& rect_dst =
				{
					x * SIZE_TILE, y * SIZE_TILE,
					SIZE_TILE, SIZE_TILE
				};

				rect_src =
				{
					(tile.terrian % num_tile_single_line) * SIZE_TILE,	// 在纹理中的 X 坐标
					(tile.terrian / num_tile_single_line) * SIZE_TILE,	// 在纹理中的 Y 坐标
					SIZE_TILE, SIZE_TILE
				};
				SDL_RenderCopy(renderer, tex_tile_set, &rect_src, &rect_dst);

				if (tile.decoration >= 0)
				{
					rect_src =
					{
						(tile.decoration % num_tile_single_line) * SIZE_TILE,	// 在纹理中的 X 坐标
						(tile.decoration / num_tile_single_line) * SIZE_TILE,	// 在纹理中的 Y 坐标
						SIZE_TILE, SIZE_TILE
					};
					SDL_RenderCopy(renderer, tex_tile_set, &rect_src, &rect_dst);
				}
			}
		}

		const SDL_Point& idx_home = map.get_idx_home();
		const SDL_Rect rect_dst =
		{
			idx_home.x * SIZE_TILE, idx_home.y * SIZE_TILE,
			SIZE_TILE, SIZE_TILE
		};
		SDL_RenderCopy(renderer, ResourcesManager::instance()->get_texture_pool().find(ResID::Tex_Home)->second, nullptr, &rect_dst);

		SDL_SetRenderTarget(renderer, nullptr);

		return true;
	}
};



#endif // ! _GAME_MANAGER_H_
