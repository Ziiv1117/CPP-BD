#pragma once

#include "raylib.h"

#include <string>

void InitChineseText();
void UnloadChineseText();
void DrawTextCN(const char* text, int x, int y, int fontSize, Color color);
void DrawTextCN(const std::string& text, int x, int y, int fontSize, Color color);
int MeasureTextCN(const char* text, int fontSize);
int MeasureTextCN(const std::string& text, int fontSize);
