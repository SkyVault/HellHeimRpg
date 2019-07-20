#include "game.hpp"

Game::Game() {
  registry = new entt::registry();
}

Game::~Game() {}

void Game::pushLayer(Layer* layer) {
  layer->act(LayerState::LS_Load);
  layers.emplace_back(std::unique_ptr<Layer>(layer));
}

void Game::popLayer() {
  if (layers.size() > 0) {
    (*layers.end())->act(LayerState::LS_Destroy);
    layers.erase(layers.end());
  }
}

void Game::gotoLayer(Layer* layer) {
  popLayer();
  pushLayer(layer);
}

Clock Game::get_clock() {
  return clock;
}

entt::registry* Game::get_registry() {
  return registry;
}

void Game::update() {
  const auto now = glfwGetTime();
  const auto dt = (now - clock.last);
  clock.last = now;
  clock.dt = dt;
  clock.timer += dt;
  clock.ticks++;
  clock.fps = 1 / (dt == 0.0f?0.0001f:dt);

  if (clock.ticks % (120*2) == 0) {
    printf("Time - dt: %f fps: %f ticks: %ld\n", dt, clock.fps, clock.ticks);
  }
}
