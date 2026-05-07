#ifndef LSYSTEM_H
#define LSYSTEM_H


#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QColorDialog>
#include <QFileDialog>
#include <QPixmap>
#include <QComboBox>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPointF>
#include <random>
#include <QStack>
#include <utility>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSoundEffect>

// L-System分形植物生成器 3.0
class LSystem : public QWidget
{
    Q_OBJECT

public:
    LSystem(QWidget* parent = nullptr);
    ~LSystem() override;

protected:
    // 重写绘制事件
    void paintEvent(QPaintEvent* event) override;
    // 重写调整大小事件（自适应窗口）
    void resizeEvent(QResizeEvent* event) override;
    // 重写鼠标点击事件
    void mousePressEvent(QMouseEvent* event) override;

private:
    // 预设植物类型
    enum PlantPreset {
        ClassicPlant,   // 经典分形植物
        Fern,           // 蕨类
        Tree            // 树木
    };

    // 背景主题
    enum BackgroundTheme {
        Grassland,      // 草地
        StarrySky,      // 星空
        SnowScene       // 雪景
    };

    // L-System核心参数
    struct LSystemParams {
        QString axiom;          // 初始字符串
        QMap<QChar, QString> rules; // 生成规则
        double angle;           // 旋转角度(度)
        double length;          // 线段长度
        QColor stemColor;       // 茎干颜色
        QColor tipColor;        // 顶端颜色
    };

private slots:
    // 按钮点击槽函数
    void onGenerateClicked();       // 生成新植物
    void onAnimateClicked();        // 动画生长
    void onColorClicked();          // 选择颜色
    void onSaveClicked();           // 保存图片
    void onPresetChanged(PlantPreset preset);// 切换植物预设
    void updateAnimation();         // 动画帧更新
    void playGrowthSound();         // 播放生长音效

private:
    // 初始化UI控制面板
    void initUI();
    // 生成L-System字符串
    QString generateLSystemString();
    // 绘制分形（支持分步动画）
    void drawLSystem(QPainter& painter, int step = -1);
    // 绘制背景
    void drawBackground(QPainter& painter);
    // 绘制花朵装饰
    void drawFlowerDecoration(QPainter& painter, double x, double y, double ratio);
    // 随机生成参数（让植物更自然）
    void randomizeParams();
    // 音效相关函数
    void initSoundSystem();          // 初始化音效系统
    void playBackgroundMusic();
    void stopBackgroundMusic();
    QString resolveBackgroundMusicPath() const;
    QString extractResourceToTempFile(const QString& resourcePath, const QString& fileName) const;
    QString createToneWavFile(const QString& fileName, double frequency, int durationMs, double amplitude) const;
    void configureToneEffect(QSoundEffect* effect, const QString& fileName, double frequency, int durationMs, double amplitude);

    // 成员变量
    LSystemParams m_params;         // 当前参数
    int m_iterations = 4;           // 迭代次数
    int m_animationStep = 0;        // 动画当前步数
    int m_totalSteps = 0;           // 动画总步数
    int m_animationInterval = 50;   // 动画间隔
    QTimer* m_animationTimer;       // 动画定时器
    bool m_isAnimating = false;     // 是否正在动画
    QString m_lsystemString;        // 生成的LSystem字符串
    std::mt19937 m_randomGen;      // 随机数生成器
    BackgroundTheme m_backgroundTheme = Grassland; // 当前背景主题
    QVector<QPointF> m_clickedPositions; // 鼠标点击位置列表
    bool m_enableBranchThickness = true; // 是否启用枝条粗细渐变
    bool m_enableFlowerDecorations = true; // 是否启用花朵装饰
    QPointF m_originPoint;          // 当前植物生成原点
    bool m_useCustomOrigin = false; // 是否使用鼠标点击原点
    
    // 音效相关
    QMediaPlayer* m_backgroundPlayer;   // 背景音乐播放器
    QAudioOutput* m_backgroundAudio;    // 背景音乐音频输出
    QSoundEffect* m_growthSound;        // 生长音效
    QSoundEffect* m_completeSound;      // 完成音效
    QSoundEffect* m_pauseSound;         // 暂停音效
    QSoundEffect* m_resumeSound;        // 继续音效
    bool m_enableBackgroundMusic = true; // 是否启用背景音乐
    bool m_enableGrowthSounds = true;   // 是否启用生长音效
};

#endif // LSYSTEM_H
