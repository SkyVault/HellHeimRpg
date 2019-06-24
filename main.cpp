#include <iostream>
#include <raylib.h>

int main() {
  InitWindow(1280, 720, "Hello World");


  Camera camera = {};
  camera.position = {0.0f, 10.0f, 10.0f};
  camera.target = { 0.0f, 0.0f, 0.0f };
  camera.up = { 0.0f, 1.0f, 0.0f };
  camera.fovy = 45.0f;
  camera.type = CAMERA_PERSPECTIVE;
 
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(Color{20, 20, 20, 255});
    BeginMode3D(camera);

    DrawCube({-4.0, 0.0, 2.0f}, 2.0f, 5.0f, 2.0f, RED);

    EndMode3D();

    DrawFPS(10, 10);
    EndDrawing();
  }

  return 0;
}
