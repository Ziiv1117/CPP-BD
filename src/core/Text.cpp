#include "core/Text.h"

#include "core/Common.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <string>
#include <vector>

namespace {

struct FontEntry {
    int logicalSize = 0;
    int atlasSize = 0;
    Font font{};
};

std::vector<int> gCodepoints;
std::vector<FontEntry> gFonts;
std::string gFontPath;
bool gFontReady = false;

void AddUtf8Codepoints(const std::string& text, std::set<int>& codepoints) {
    for (std::size_t i = 0; i < text.size();) {
        const unsigned char c = static_cast<unsigned char>(text[i]);
        int codepoint = 0;
        int bytes = 1;
        if ((c & 0x80) == 0) {
            codepoint = c;
            bytes = 1;
        } else if ((c & 0xE0) == 0xC0 && i + 1 < text.size()) {
            codepoint = ((c & 0x1F) << 6) | (static_cast<unsigned char>(text[i + 1]) & 0x3F);
            bytes = 2;
        } else if ((c & 0xF0) == 0xE0 && i + 2 < text.size()) {
            codepoint = ((c & 0x0F) << 12) |
                        ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(text[i + 2]) & 0x3F);
            bytes = 3;
        } else if ((c & 0xF8) == 0xF0 && i + 3 < text.size()) {
            codepoint = ((c & 0x07) << 18) |
                        ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 12) |
                        ((static_cast<unsigned char>(text[i + 2]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(text[i + 3]) & 0x3F);
            bytes = 4;
        } else {
            ++i;
            continue;
        }
        codepoints.insert(codepoint);
        i += static_cast<std::size_t>(bytes);
    }
}

std::vector<int> BuildCodepoints() {
    std::set<int> codepoints;
    for (int c = 32; c <= 126; ++c) {
        codepoints.insert(c);
    }

    const std::string charset =
        "《》：，。；、？！“”‘’（）【】[]"
        "余光未熄图形测试版第二版入口流程完整开始游戏操作说明退出继续主菜单暂停结算胜利失败"
        "人类影守灯人工程师观测员旧钟楼上层廊桥镜光室电闸房中庭光井封印大厅"
        "回合当前选择所在房间虚弱正常受蚀半影完全同化任务三灯校准双闸供能运送光核最终净化"
        "点亮协作拾取交付救援手电方向键移动传送穿墙设置战绩返回按键联机单局"
        "核心目标状态完成未完成剩余全部存活被同化用时数量进入黑暗遮罩安全光区"
        "光阵营完成最终净化影阵营所有人类被同化时间耗尽按按钮重新开始结果写入"
        "键盘控制鼠标点击显示帮助人类有限视野影全图视野"
        "对局准备模式本地联机演示同一台电脑控制双方用于课堂展示单局适合验证任务闭环"
        "追击拖延压力已准备等待准备切换重新选阵营当前目标阻止正式玩法不依赖"
        "菜单阵营确认状态面板预留光只能照亮局部却掌控全局"
        "一三上不与中主二于井交亮人从付任传余作使供依停先光入全写净准出击切利到制前剩"
        "力务动助化区半协单印却厅压友双取受口只可台合同后向员和回图在地堂墙备大失始存"
        "守安完定室对封尽局层展工已师帮常并庭廊延开式弱当形影待心态戏成或战房所手把抗"
        "拖择拾指按换掌控援操救数整新方旧时明显暂暗最有未本机板果标校核桥楼模止正法活"
        "流测游源演灯点照熄版状玩环用电界留盘目瞄确示移程穿第等算类终结继绩续罩置耗联"
        "胜能脑色菜营虚蚀行被观视角认让设证试说课调败赖跑运近返进追退送适选通遮部重野"
        "量钟钮键镜闭间闸队阵阻附限靠面预验黑鼠";
    AddUtf8Codepoints(charset, codepoints);
    return {codepoints.begin(), codepoints.end()};
}

Font* LoadFontForSize(int fontSize) {
    if (!gFontReady) {
        return nullptr;
    }

    const int size = std::max(8, fontSize);
    const int atlasSize = std::max(8, static_cast<int>(std::ceil(fontSize * RenderScale())));
    for (FontEntry& entry : gFonts) {
        if (entry.logicalSize == size && entry.atlasSize == atlasSize) {
            return &entry.font;
        }
    }

    Font font = LoadFontEx(gFontPath.c_str(), atlasSize, const_cast<int*>(gCodepoints.data()),
                           static_cast<int>(gCodepoints.size()));
    if (font.texture.id == 0) {
        return nullptr;
    }

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    gFonts.push_back(FontEntry{size, atlasSize, font});
    return &gFonts.back().font;
}

}  // namespace

void InitChineseText() {
    gCodepoints = BuildCodepoints();
    const char* candidates[] = {
        "/assets/fonts/LXGWWenKai-Regular.ttf",
        "assets/fonts/LXGWWenKai-Regular.ttf",
        "/assets/fonts/GameChinese.ttf",
        "assets/fonts/GameChinese.ttf",
        "/assets/fonts/GameChinese.ttc",
        "assets/fonts/GameChinese.ttc",
        "/assets/fonts/NotoSansSC-VF.ttf",
        "assets/fonts/NotoSansSC-VF.ttf",
        "C:/Windows/Fonts/NotoSansSC-VF.ttf",
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/simsun.ttc"};

    for (const char* path : candidates) {
        if (FileExists(path)) {
            gFontPath = path;
            gFontReady = true;
            const int preloadSizes[] = {12, 13, 14, 15, 16, 18, 20, 22, 24, 26, 28, 40, 52};
            for (int size : preloadSizes) {
                LoadFontForSize(size);
            }
            if (!gFonts.empty()) {
                return;
            }
            gFontReady = false;
            gFontPath.clear();
        }
    }
}

void UnloadChineseText() {
    if (gFontReady) {
        for (FontEntry& entry : gFonts) {
            UnloadFont(entry.font);
        }
        gFonts.clear();
        gCodepoints.clear();
        gFontPath.clear();
        gFontReady = false;
    }
}

void DrawTextCN(const char* text, int x, int y, int fontSize, Color color) {
    if (Font* font = LoadFontForSize(fontSize)) {
        DrawTextEx(*font, text, {static_cast<float>(x), static_cast<float>(y)},
                   static_cast<float>(fontSize), 1.0f, color);
    } else {
        DrawText(text, x, y, fontSize, color);
    }
}

void DrawTextCN(const std::string& text, int x, int y, int fontSize, Color color) {
    DrawTextCN(text.c_str(), x, y, fontSize, color);
}

int MeasureTextCN(const char* text, int fontSize) {
    if (Font* font = LoadFontForSize(fontSize)) {
        return static_cast<int>(MeasureTextEx(*font, text, static_cast<float>(fontSize), 1.0f).x);
    }
    return MeasureText(text, fontSize);
}

int MeasureTextCN(const std::string& text, int fontSize) {
    return MeasureTextCN(text.c_str(), fontSize);
}
