#ifndef GAME_H
#define GAME_H

#include <memory>
#include <iostream>
#include <mutex>
#include <GLFW/glfw3.h>
#include <vector>
#include <entt/entt.hpp>

#include <magic_enum.hpp>

struct Game;
struct Clock {
  friend Game;

  float dt {0.0f};
  float timer {0.0f};
  int64_t ticks {-1};
  float fps {0.0f};
  
private:
  float last {0.0f};
};

enum LayerState {
    LS_Load = 0,
    LS_Update = 1,
    LS_Render = 2,
    LS_Destroy = 3
};

struct Layer {
  friend Game;

  virtual void act(LayerState state) = 0;
};

class Game {
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    static std::unique_ptr<Game> instance;
    static std::once_flag onceFlag;

public:
  Game();
  ~Game();

  static auto* it() {
    std::call_once(Game::onceFlag, [] () {
	instance.reset(new Game());
    });

    return instance.get();
  }

  void update();

  void pushLayer(Layer* layer);
  void popLayer();
  void gotoLayer(Layer* layer);

  Clock get_clock();

  entt::registry* get_registry();

private:

  Clock clock;
  std::vector<std::unique_ptr<Layer>> layers;

  entt::registry* registry;
};

#endif//GAME_H
