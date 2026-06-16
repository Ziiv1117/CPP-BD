# CPP-BD

《余光未熄》Raylib C++ 第二版单局演示。

## 当前阶段

仓库已经从终端规则原型推进到 Raylib 图形化单局演示。这个版本使用几何图形占位，重点验证完整入口流程、中文 UI、地图显示、角色操作、灯光、任务链和胜负流程。

已包含：

- Raylib 图形窗口
- 主菜单、阵营/模式选择、准备确认、操作说明、暂停菜单和结算界面
- 游戏内文字已统一为中文，并自动加载 Windows 中文字体
- 6 房间横版隔断地图
- 3 名人类与 1 名影
- 人类移动、手电方向、点灯、救援、协作
- 影移动、穿墙、房间传送、强光虚弱
- 同化条与同化阶段
- 目标任务链按新设计推进：重燃残灯、维修机关门、护光开门、灯源校准、影迹追踪、最终净化
- 任务面板、状态面板、胜负遮罩
- 按最终联机单局制推进，当前本地演示允许一人切换控制多角色，最终成品应为每名玩家操控一名角色

## 一键启动网页版

在资源管理器里双击：

```text
start_web.bat
```

它会自动启动本地网页服务器，并打开：

```text
http://localhost:8080
```

网页版本需要点击游戏画面后再使用键盘操作。

## 桌面版启动

PowerShell:

```powershell
.\start_game.ps1
```

如果已经构建过，只想直接启动：

```powershell
.\start_game.ps1 -SkipBuild
```

自动冒烟测试：

```powershell
.\start_game.ps1 -Smoke
```

## 网页版

本项目已经支持 Raylib C++ 编译为 WebAssembly 网页版。本机工具链放在：

```text
C:\Users\15963\Desktop\emsdk
C:\Users\15963\Desktop\raylib-5.5-web
```

构建网页：

```powershell
.\build_web.ps1
```

启动本地网页：

```powershell
.\start_web.ps1
```

或直接双击：

```text
start_web.bat
```

浏览器打开：

```text
http://localhost:8080
```

网页输出文件位于：

```text
web/
  index.html
  index.js
  index.wasm
  index.data
```


## 构建

需要 MSYS2 UCRT64 的 `g++` 和 Raylib。

安装 Raylib：

```powershell
D:\msys64\usr\bin\pacman.exe -S --noconfirm mingw-w64-ucrt-x86_64-raylib
```

只构建不启动：

```powershell
.\build.ps1
```

输出：

```powershell
.\build\afterglow_raylib.exe
```

## 代码结构

当前源码已从单文件拆分为模块化结构：

```text
src/
  main.cpp
  core/
    Common.h / Common.cpp
    Game.h / Game.cpp
    Text.h / Text.cpp
  entities/
    Entity.h / Entity.cpp
    Character.h / Character.cpp
    HumanPlayer.h / HumanPlayer.cpp
    ShadowPlayer.h / ShadowPlayer.cpp
  map/
    Map.h / Map.cpp
    LightSource.h / LightSource.cpp
  systems/
    TaskManager.h / TaskManager.cpp
  ui/
    UIManager.h / UIManager.cpp
```

其中继承结构为：

```text
Entity
  Character
    HumanPlayer
    ShadowPlayer
```

## 项目文档

- `余光未熄_游戏策划书.md`：总策划书，已按守灯人、工程师、观测员、影和六段任务链的新设计更新。
- `第二版阶段推进书.md`：第二版阶段目标、当前完成情况、待修改事务和后续版本推进书。
- `完整游戏素材框架清单.md`：角色、地图、UI、音效、特效等完整素材需求清单。

## 操作

- 主菜单：点击 `开始游戏` 或按 `Enter` 进入对局准备
- 主菜单：点击 `操作说明` 或按 `H` 查看操作说明
- 对局准备：`1/2/3` 选择本地演示、光阵营视角或影阵营视角
- 对局准备：`Space` 切换准备状态，双方准备后按 `Enter` 进入对局
- 游戏中：按 `Esc` 打开暂停菜单
- 暂停/说明/结算界面：按界面按钮或对应快捷键操作
- `Tab/C`：切换当前人类
- `W/A/S/D`：移动当前人类
- `E`：基础交互，点亮灯源、修复灯源或尝试救援
- `Q`：职业技能，守灯人眩晕影、工程师操作维修面板、观测员影迹观测
- `L`：单独救援/净化附近队友
- `R`：影破坏附近已点亮灯源
- 方向键：移动影，影可以穿墙
- `1-6`：影传送到指定房间，主键盘数字和小键盘数字都支持，虚弱或眩晕状态不能传送
- `V`：切换人类有限视野和影全图视野

## 后续任务

下一步建议继续补齐课程要求：

- 继续完善角色选择、房间界面和 `PlayerId -> CharacterId` 玩家槽位结构
- 增加更明确的任务反馈、音效和结果统计
- 增加贴图、动画和场景氛围
- 增加测试报告与最终项目报告
