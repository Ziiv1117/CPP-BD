#include "core/Game.h"
#include "core/Text.h"

#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace {

Game* gGame = nullptr;

void RunFrame() {
    gGame->Update();
    BeginDrawing();
    gGame->Draw();
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

    InitWindow(kScreenW, kScreenH, "余光未熄 - 第二版单局演示");
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
