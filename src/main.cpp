#include "core/Game.h"
#include "core/Text.h"

#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

namespace {

Game* gGame = nullptr;
float gDrawScale = 1.0f;

void RunFrame() {
    gGame->Update();
    BeginDrawing();
#ifdef __EMSCRIPTEN__
    BeginMode2D(Camera2D{{0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, gDrawScale});
#endif
    gGame->Draw();
#ifdef __EMSCRIPTEN__
    EndMode2D();
#endif
    EndDrawing();
}

}  // namespace

int main(int argc, char** argv) {
    bool smokeTest = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--smoke") {
            smokeTest = true;
        }
    }
    if (smokeTest) {
        SetConfigFlags(FLAG_WINDOW_HIDDEN);
    }

#ifdef __EMSCRIPTEN__
    if (!smokeTest) {
        gDrawScale = static_cast<float>(emscripten_get_device_pixel_ratio());
        if (gDrawScale < 1.0f) {
            gDrawScale = 1.0f;
        }
        if (gDrawScale > 2.0f) {
            gDrawScale = 2.0f;
        }
    }
#endif
    SetRenderScale(gDrawScale);

    InitWindow(static_cast<int>(kScreenW * gDrawScale), static_cast<int>(kScreenH * gDrawScale),
               "余光未熄 - 第二版单局演示");
    InitChineseText();
    SetTargetFPS(60);

    Game game;
    gGame = &game;
    if (smokeTest) {
        BeginDrawing();
        game.Draw();
        EndDrawing();
        UnloadChineseText();
        CloseWindow();
        return 0;
    }

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(RunFrame, 0, 1);
#else
    while (!WindowShouldClose()) {
        RunFrame();
    }

    UnloadChineseText();
    CloseWindow();
#endif
    return 0;
}
