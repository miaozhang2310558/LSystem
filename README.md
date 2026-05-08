**Project**
- **简介**: L-System 分形植物生成器是一个基于 Lindenmayer 系统（L-System）的可视化项目，使用 Qt（Widgets + Multimedia）实现分形植物的生成、动画生长、背景主题与声音效果。

**Features**
- **可视化生成**: 基于可配置的公理与重写规则生成分形植物。
- **交互控制**: 支持迭代次数、角度、线段长度、植物预设、主题与动画速度实时调节。
- **动画与音效**: 动画生长效果；生长短音效与背景音乐（支持多路径加载与 qrc 回退）。
- **导出与保存**: 支持保存当前画面为 PNG 图片。

**Branches**
- **main**: 初始版本（基础 L-System，可视化和核心交互）。包含[第一阶段.md](第一阶段.md)。
- **phase-2**: 增强版本（完整 UI 控件、动画控制、保存等）。见 [第二阶段.md](https://github.com/miaozhang2310558/LSystem/blob/phase-2/第二阶段.md)。
- **phase-3**: 当前增强版（背景主题、花朵装饰、音效与兼容性修复）。见 [第三阶段增强功能说明.md](https://github.com/miaozhang2310558/LSystem/blob/phase-3/第三阶段增强功能说明.md)。

**Project Structure**
- **L-System/**: 源代码与资源
  - **LSystem.cpp / LSystem.h**: 主要窗口与逻辑实现
  - **main.cpp**: 程序入口
  - **LSystem.qrc**: Qt 资源（包含 audio 文件）
  - **audio/**: 本地音频资源（background.mp3, background3.mp3）
- **第一阶段.md / 第二阶段.md / 第三阶段增强功能说明.md**: 各阶段需求与实现说明

**Build & Run (建议)**
- 推荐使用 Qt Creator 打开工程 `L-System/CMakeLists.txt` 或 `L-System.sln`（Windows Visual Studio）。

- 使用 CMake 命令行（示例，需根据本机 Qt 路径调整）：
```bash
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.x/your-msvc-path"   # 将此路径替换为你的 Qt 安装目录
cmake --build . --config Debug
```
- 或直接使用 Visual Studio 打开 `L-System.sln` 并选择 Debug/Release 构建，然后运行 `LSystemPlant.exe`。

**Runtime Notes (音频与兼容性)**
- 背景音乐加载策略：程序会首先在可执行文件附近查找 `audio/background3.mp3` / `audio/background.mp3`，找不到时回退到 qrc 资源并导出到临时目录再播放。这解决了部分构建环境下 qrc 直读 MP3 失败的问题。
- 如果运行时无声音：
  - 检查 `audio` 目录是否随可执行文件一起部署（`build` 下或可执行上级目录）。
  - 查看控制台日志中关于 `QMediaPlayer::ResourceError` 或 `无法打开资源文件` 的警告，它们会给出缺失路径信息。

**Troubleshooting**
- 编辑器静态检查可能提示找不到 Qt 头文件（includePath），这是本地 IDE 配置问题，不影响运行，只需在构建环境配置 Qt include/lib 即可。
- 如需复现或测试音频加载问题，可参考代码中的 `resolveBackgroundMusicPath()` 与 `extractResourceToTempFile()` 函数（文件：L-System/LSystem.cpp）。

**How to explore branches**
- 要查看每个阶段实现的差异：
  - 切到对应分支 `main`, `phase-2`, `phase-3`。
  - 例如在命令行：
```bash
git checkout phase-3
```

