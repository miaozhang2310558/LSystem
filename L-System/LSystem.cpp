#include "LSystem.h"
#include <cmath>
#include <QApplication>
#include <QScreen>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QCheckBox>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QStandardPaths>
#include <QUrl>

// 常量定义
const double PI = 3.14159265358979323846;
const int ANIMATION_INTERVAL = 50; // 动画间隔(ms)
const int MIN_ITERATIONS = 1;
const int MAX_ITERATIONS = 7;

LSystem::LSystem(QWidget* parent)
    : QWidget(parent)
    , m_randomGen(std::random_device{}())
{
    // 初始化UI
    initUI();

    // 初始化定时器
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &LSystem::updateAnimation);

    // 初始化音效系统
    initSoundSystem();

    QTimer::singleShot(0, this, [this]() {
        if (m_enableBackgroundMusic) {
            playBackgroundMusic();
        }
    });

    // 默认参数
    m_params.angle = 25.0;
    m_params.length = 15.0;
    m_params.stemColor = QColor(0, 150, 0); // 绿色茎干
    m_params.tipColor = QColor(255, 0, 0);  // 红色顶端
    m_params.axiom = "F";
    m_params.rules['F'] = "FF+[+F-F-F]-[-F+F+F]"; // 经典植物规则
    m_originPoint = QPointF(width() / 2.0, height() - 50.0);

    // 生成初始字符串
    m_lsystemString = generateLSystemString();
    m_totalSteps = m_lsystemString.length();
}

LSystem::~LSystem()
{
}

// 初始化UI控制面板
void LSystem::initUI()
{
    // 设置窗口属性
    setWindowTitle("L-System分形植物生成器 3.0 - 增强版");
    setMinimumSize(900, 700);

    // 主布局（垂直）
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 第一行：植物参数控制（左侧）
    QHBoxLayout* paramLayout = new QHBoxLayout();
    paramLayout->setSpacing(15);

    // 植物类型
    QVBoxLayout* plantTypeLayout = new QVBoxLayout();
    plantTypeLayout->setSpacing(5);
    QLabel* presetLabel = new QLabel("植物类型");
    presetLabel->setAlignment(Qt::AlignCenter);
    QComboBox* presetCombo = new QComboBox();
    presetCombo->addItem("经典植物", ClassicPlant);
    presetCombo->addItem("蕨类", Fern);
    presetCombo->addItem("树木", Tree);
    connect(presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, [this, presetCombo](int index) {
            PlantPreset preset = static_cast<PlantPreset>(presetCombo->itemData(index).toInt());
            onPresetChanged(preset);
        });
    plantTypeLayout->addWidget(presetLabel);
    plantTypeLayout->addWidget(presetCombo);

    // 迭代次数
    QVBoxLayout* iterLayout = new QVBoxLayout();
    iterLayout->setSpacing(5);
    QLabel* iterLabel = new QLabel("迭代次数");
    iterLabel->setAlignment(Qt::AlignCenter);
    QSpinBox* iterSpin = new QSpinBox();
    iterSpin->setRange(MIN_ITERATIONS, MAX_ITERATIONS);
    iterSpin->setValue(m_iterations);
    iterSpin->setFixedWidth(60);
    connect(iterSpin, QOverload<int>::of(&QSpinBox::valueChanged),
        this, [this](int val) {
            m_iterations = val;
            m_lsystemString = generateLSystemString();
            m_totalSteps = m_lsystemString.length();
            update();
        });
    iterLayout->addWidget(iterLabel);
    iterLayout->addWidget(iterSpin);

    // 角度控制
    QVBoxLayout* angleLayout = new QVBoxLayout();
    angleLayout->setSpacing(5);
    QLabel* angleLabel = new QLabel("旋转角度");
    angleLabel->setAlignment(Qt::AlignCenter);
    QSlider* angleSlider = new QSlider(Qt::Horizontal);
    angleSlider->setRange(10, 60);
    angleSlider->setValue(static_cast<int>(m_params.angle));
    angleSlider->setFixedWidth(120);
    connect(angleSlider, &QSlider::valueChanged,
        this, [this](int val) {
            m_params.angle = static_cast<double>(val);
            update();
        });
    angleLayout->addWidget(angleLabel);
    angleLayout->addWidget(angleSlider);

    // 长度控制
    QVBoxLayout* lengthLayout = new QVBoxLayout();
    lengthLayout->setSpacing(5);
    QLabel* lengthLabel = new QLabel("线段长度");
    lengthLabel->setAlignment(Qt::AlignCenter);
    QSlider* lengthSlider = new QSlider(Qt::Horizontal);
    lengthSlider->setRange(5, 30);
    lengthSlider->setValue(static_cast<int>(m_params.length));
    lengthSlider->setFixedWidth(120);
    connect(lengthSlider, &QSlider::valueChanged,
        this, [this](int val) {
            m_params.length = static_cast<double>(val);
            update();
        });
    lengthLayout->addWidget(lengthLabel);
    lengthLayout->addWidget(lengthSlider);

    paramLayout->addLayout(plantTypeLayout);
    paramLayout->addLayout(iterLayout);
    paramLayout->addLayout(angleLayout);
    paramLayout->addLayout(lengthLayout);
    paramLayout->addStretch();

    // 第二行：背景主题和动画控制（中间）
    QHBoxLayout* themeAnimLayout = new QHBoxLayout();
    themeAnimLayout->setSpacing(15);

    // 背景主题
    QVBoxLayout* themeLayout = new QVBoxLayout();
    themeLayout->setSpacing(5);
    QLabel* themeLabel = new QLabel("背景主题");
    themeLabel->setAlignment(Qt::AlignCenter);
    QComboBox* themeCombo = new QComboBox();
    themeCombo->addItem("草地", Grassland);
    themeCombo->addItem("星空", StarrySky);
    themeCombo->addItem("雪景", SnowScene);
    connect(themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, [this, themeCombo](int index) {
            m_backgroundTheme = static_cast<BackgroundTheme>(themeCombo->itemData(index).toInt());
            update();
        });
    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(themeCombo);

    // 动画速度
    QVBoxLayout* speedLayout = new QVBoxLayout();
    speedLayout->setSpacing(5);
    QLabel* speedLabel = new QLabel("动画速度");
    speedLabel->setAlignment(Qt::AlignCenter);
    QSlider* speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(10, 200);
    speedSlider->setValue(m_animationInterval);
    speedSlider->setFixedWidth(120);
    connect(speedSlider, &QSlider::valueChanged,
        this, [this](int val) {
            m_animationInterval = val;
            if (m_animationTimer->isActive()) {
                m_animationTimer->stop();
                m_animationTimer->start(val);
            }
        });
    speedLayout->addWidget(speedLabel);
    speedLayout->addWidget(speedSlider);

    themeAnimLayout->addLayout(themeLayout);
    themeAnimLayout->addLayout(speedLayout);
    themeAnimLayout->addStretch();

    // 第三行：功能按钮（右侧分组）
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    // 主要功能按钮组
    QHBoxLayout* mainButtonLayout = new QHBoxLayout();
    mainButtonLayout->setSpacing(8);
    
    QPushButton* generateBtn = new QPushButton("随机生成");
    generateBtn->setFixedWidth(80);
    connect(generateBtn, &QPushButton::clicked, this, &LSystem::onGenerateClicked);

    QPushButton* animateBtn = new QPushButton("动画生长");
    animateBtn->setFixedWidth(80);
    connect(animateBtn, &QPushButton::clicked, this, &LSystem::onAnimateClicked);

    QPushButton* pauseBtn = new QPushButton("暂停/继续");
    pauseBtn->setFixedWidth(80);
    connect(pauseBtn, &QPushButton::clicked, this, [this]() {
        if (m_animationTimer->isActive()) {
            m_animationTimer->stop();
            if (m_enableGrowthSounds && m_pauseSound) {
                m_pauseSound->play();
            }
        } else if (m_isAnimating) {
            m_animationTimer->start(m_animationInterval);
            if (m_enableGrowthSounds && m_resumeSound) {
                m_resumeSound->play();
            }
        }
        update();
    });

    mainButtonLayout->addWidget(generateBtn);
    mainButtonLayout->addWidget(animateBtn);
    mainButtonLayout->addWidget(pauseBtn);

    // 辅助功能按钮组
    QHBoxLayout* auxButtonLayout = new QHBoxLayout();
    auxButtonLayout->setSpacing(8);
    
    QPushButton* colorBtn = new QPushButton("选择颜色");
    colorBtn->setFixedWidth(80);
    connect(colorBtn, &QPushButton::clicked, this, &LSystem::onColorClicked);

    QPushButton* saveBtn = new QPushButton("保存图片");
    saveBtn->setFixedWidth(80);
    connect(saveBtn, &QPushButton::clicked, this, &LSystem::onSaveClicked);

    QPushButton* aboutBtn = new QPushButton("关于");
    aboutBtn->setFixedWidth(60);
    connect(aboutBtn, &QPushButton::clicked, this, [this]() {
        QMessageBox::about(this, "关于 L-System 分形植物生成器 3.0",
            "<h3>L-System 分形植物生成器 3.0</h3>"
            "<p>基于 Lindenmayer 系统的分形植物生成工具</p>"
            "<p><b>主要功能：</b></p>"
            "<ul>"
            "<li>多种植物预设（经典植物、蕨类、树木）</li>"
            "<li>实时参数调节（迭代次数、角度、长度）</li>"
            "<li>动画生长效果与速度控制</li>"
            "<li>背景主题切换（草地、星空、雪景）</li>"
            "<li>枝条粗细渐变与花朵装饰</li>"
            "<li>鼠标点击生成植物</li>"
            "<li>图片保存功能</li>"
            "</ul>"
            "<p><b>操作说明：</b></p>"
            "<ul>"
            "<li>点击界面任意位置生成新植物</li>"
            "<li>使用控制面板调节参数</li>"
            "<li>点击'动画生长'观看植物生长过程</li>"
            "</ul>"
            "<p><b>技术实现：</b></p>"
            "<p>基于 Qt 框架，使用 L-System 字符串重写算法和海龟绘图技术</p>"
            "<p>版本：3.0 </p>");
    });

    auxButtonLayout->addWidget(colorBtn);
    auxButtonLayout->addWidget(saveBtn);
    auxButtonLayout->addWidget(aboutBtn);

    buttonLayout->addLayout(mainButtonLayout);
    buttonLayout->addSpacing(20);
    buttonLayout->addLayout(auxButtonLayout);
    buttonLayout->addStretch();

    // 第四行：特效和音效开关（底部居中）
    QHBoxLayout* effectLayout = new QHBoxLayout();
    effectLayout->setSpacing(15);
    
    QCheckBox* thicknessCheck = new QCheckBox("枝条粗细渐变");
    thicknessCheck->setChecked(m_enableBranchThickness);
    connect(thicknessCheck, &QCheckBox::toggled,
        this, [this](bool checked) {
            m_enableBranchThickness = checked;
            update();
        });

    QCheckBox* flowerCheck = new QCheckBox("花朵装饰");
    flowerCheck->setChecked(m_enableFlowerDecorations);
    connect(flowerCheck, &QCheckBox::toggled,
        this, [this](bool checked) {
            m_enableFlowerDecorations = checked;
            update();
        });

    // 音效控制
    QCheckBox* bgMusicCheck = new QCheckBox("背景音乐");
    bgMusicCheck->setChecked(m_enableBackgroundMusic);
    connect(bgMusicCheck, &QCheckBox::toggled,
        this, [this](bool checked) {
            m_enableBackgroundMusic = checked;
            if (checked) {
                playBackgroundMusic();
            } else {
                stopBackgroundMusic();
            }
        });

    QCheckBox* soundEffectCheck = new QCheckBox("生长音效");
    soundEffectCheck->setChecked(m_enableGrowthSounds);
    connect(soundEffectCheck, &QCheckBox::toggled,
        this, [this](bool checked) {
            m_enableGrowthSounds = checked;
        });

    effectLayout->addStretch();
    effectLayout->addWidget(thicknessCheck);
    effectLayout->addWidget(flowerCheck);
    effectLayout->addWidget(bgMusicCheck);
    effectLayout->addWidget(soundEffectCheck);
    effectLayout->addStretch();

    // 添加到主布局
    mainLayout->addLayout(paramLayout);
    mainLayout->addLayout(themeAnimLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(effectLayout);
    mainLayout->addStretch(); // 确保控件靠上显示

    // 设置主布局
    setLayout(mainLayout);
}

// 生成L-System字符串
QString LSystem::generateLSystemString()
{
    QString result = m_params.axiom;

    // 迭代生成
    for (int i = 0; i < m_iterations; ++i) {
        QString newResult;
        for (const QChar& c : result) {
            if (m_params.rules.contains(c)) {
                newResult += m_params.rules[c];
            }
            else {
                newResult += c;
            }
        }
        result = newResult;
    }

    return result;
}

// 绘制花朵装饰
void LSystem::drawFlowerDecoration(QPainter& painter, double x, double y, double ratio)
{
    if (!m_enableFlowerDecorations) return;
    
    // 只在末端绘制花朵
    if (ratio > 0.8) {
        QColor flowerColor;
        if (ratio > 0.95) {
            // 顶端花朵
            flowerColor = QColor(255, 0, 0); // 红色
            painter.setBrush(QBrush(flowerColor));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(QPointF(x, y), 3, 3);
        } else {
            // 分支末端小花
            flowerColor = QColor(255, 192, 203); // 粉色
            painter.setBrush(QBrush(flowerColor));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(QPointF(x, y), 2, 2);
        }
    }
}

// 绘制分形
void LSystem::drawLSystem(QPainter& painter, int step)
{
    painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿

    // 起始位置（支持鼠标点击生成原点）
    double startX = m_originPoint.x();
    double startY = m_originPoint.y();

    // 当前位置和角度
    double x = startX;
    double y = startY;
    double angle = 90.0; // 初始向上

    // 栈用于保存状态（[ 和 ]）
    QStack<QPair<QPointF, double>> stack;

    // 要绘制的字符数（-1表示全部）
    int drawCount = (step == -1) ? m_lsystemString.length() : step;

    for (int i = 0; i < drawCount; ++i) {
        QChar c = m_lsystemString[i];

        if (c == 'F') {
            // 绘制线段
            double rad = angle * PI / 180.0;
            double newX = x + m_params.length * cos(rad);
            double newY = y - m_params.length * sin(rad);

            // 渐变颜色（从茎干色到顶端色）
            double ratio = static_cast<double>(i) / drawCount;
            // 手动混合颜色（兼容 Qt 5/6）
            int r = static_cast<int>(m_params.stemColor.red() * (1 - ratio) + m_params.tipColor.red() * ratio);
            int g = static_cast<int>(m_params.stemColor.green() * (1 - ratio) + m_params.tipColor.green() * ratio);
            int b = static_cast<int>(m_params.stemColor.blue() * (1 - ratio) + m_params.tipColor.blue() * ratio);
            r = qBound(0, r, 255);
            g = qBound(0, g, 255);
            b = qBound(0, b, 255);
            QColor color(r, g, b);
            color = color.lighter(100 + static_cast<int>(ratio * 50));

            // 枝条粗细渐变
            double thickness = 2.0;
            if (m_enableBranchThickness) {
                thickness = 4.0 * (1.0 - ratio * 0.8); // 从粗到细
                thickness = qMax(1.0, thickness); // 最小厚度为1
            }

            painter.setPen(QPen(color, thickness));
            painter.drawLine(QPointF(x, y), QPointF(newX, newY));

            // 绘制花朵装饰
            drawFlowerDecoration(painter, newX, newY, ratio);

            x = newX;
            y = newY;
        }
        else if (c == '+') {
            // 左转
            angle += m_params.angle + (std::rand() % 5 - 2); // 随机扰动±2度
        }
        else if (c == '-') {
            // 右转
            angle -= m_params.angle + (std::rand() % 5 - 2); // 随机扰动±2度
        }
        else if (c == '[') {
            // 保存状态
            stack.push(QPair<QPointF, double>(QPointF(x, y), angle));
        }
        else if (c == ']') {
            // 恢复状态
            if (!stack.isEmpty()) {
                QPair<QPointF, double> state = stack.pop();
                x = state.first.x();
                y = state.first.y();
                angle = state.second;
            }
        }
    }
}

// 绘制背景
void LSystem::drawBackground(QPainter& painter)
{
    int w = width();
    int h = height();
    QRect widgetRect = rect();
    
    switch (m_backgroundTheme) {
    case Grassland:
        // 草地背景
        {
            QLinearGradient gradient(0, 0, 0, h);
            gradient.setColorAt(0, QColor(135, 206, 235)); // 天空蓝
            gradient.setColorAt(0.7, QColor(135, 206, 235));
            gradient.setColorAt(1, QColor(124, 252, 0));    // 草地绿
            painter.fillRect(widgetRect, gradient);
            
            // 绘制草地细节
            painter.setPen(QPen(QColor(34, 139, 34), 1));
            for (int i = 0; i < w; i += 5) {
                int grassHeight = 10 + (std::rand() % 15);
                painter.drawLine(i, h, i, h - grassHeight);
            }
        }
        break;
        
    case StarrySky:
        // 星空背景
        {
            painter.fillRect(widgetRect, QColor(10, 10, 40)); // 深蓝色夜空
            
            // 绘制星星
            painter.setPen(QPen(Qt::white, 1));
            for (int i = 0; i < 100; i++) {
                int x = std::rand() % w;
                int y = std::rand() % (h / 2);
                int size = 1 + (std::rand() % 3);
                painter.drawEllipse(x, y, size, size);
            }
            
            // 绘制月亮
            painter.setBrush(QBrush(QColor(255, 255, 200)));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(w - 80, 40, 60, 60);
        }
        break;
        
    case SnowScene:
        // 雪景背景
        {
            QLinearGradient gradient(0, 0, 0, h);
            gradient.setColorAt(0, QColor(200, 230, 255)); // 淡蓝色天空
            gradient.setColorAt(1, QColor(240, 248, 255)); // 雪地白
            painter.fillRect(widgetRect, gradient);
            
            // 绘制雪花
            painter.setPen(QPen(Qt::white, 1));
            for (int i = 0; i < 50; i++) {
                int x = std::rand() % w;
                int y = std::rand() % h;
                painter.drawPoint(x, y);
            }
        }
        break;
    }
}

// 绘制事件
void LSystem::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    
    // 绘制背景
    drawBackground(painter);

    // 绘制分形（动画模式绘制指定步数，否则绘制全部）
    if (m_isAnimating) {
        drawLSystem(painter, m_animationStep);
    }
    else {
        drawLSystem(painter, -1);
    }
}

// 调整大小事件
void LSystem::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    if (!m_useCustomOrigin) {
        m_originPoint = QPointF(width() / 2.0, height() - 50.0);
    } else {
        m_originPoint.setX(qBound(20.0, m_originPoint.x(), static_cast<double>(width() - 20)));
        m_originPoint.setY(qBound(20.0, m_originPoint.y(), static_cast<double>(height() - 20)));
    }

    update(); // 窗口大小变化时重绘
}

// 鼠标点击事件
void LSystem::mousePressEvent(QMouseEvent* event)
{
    // 记录点击位置
    m_clickedPositions.append(event->pos());
    
    // 如果点击位置超过5个，移除最旧的
    if (m_clickedPositions.size() > 5) {
        m_clickedPositions.removeFirst();
    }

    m_originPoint = event->pos();
    m_useCustomOrigin = true;
    
    // 生成新的植物
    randomizeParams();
    if (m_enableGrowthSounds && m_growthSound) {
        m_growthSound->play();
    }
    update();
}

// 切换植物预设
void LSystem::onPresetChanged(PlantPreset preset)
{

    switch (preset) {
    case ClassicPlant:
        m_params.axiom = "F";
        m_params.rules['F'] = "FF+[+F-F-F]-[-F+F+F]";
        m_params.angle = 25.0;
        break;
    case Fern:
        m_params.axiom = "X";
        m_params.rules['X'] = "F+[[X]-X]-F[-FX]+X";
        m_params.rules['F'] = "FF";
        m_params.angle = 22.5;
        break;
    case Tree:
        m_params.axiom = "F";
        m_params.rules['F'] = "F[+F]F[-F][F]";
        m_params.angle = 20.0;
        break;
    }

    m_lsystemString = generateLSystemString();
    m_totalSteps = m_lsystemString.length();
    update();
}

// 随机生成参数
void LSystem::randomizeParams()
{
    // 随机角度（15-45度）
    m_params.angle = 15.0 + (std::rand() % 30);
    // 随机长度（10-20）
    m_params.length = 10.0 + (std::rand() % 10);
    // 随机颜色
    m_params.stemColor = QColor(std::rand() % 50, 100 + std::rand() % 100, std::rand() % 50);
    m_params.tipColor = QColor(200 + std::rand() % 55, std::rand() % 50, std::rand() % 50);

    m_lsystemString = generateLSystemString();
    m_totalSteps = m_lsystemString.length();
    update();
}


// 初始化音效系统
void LSystem::initSoundSystem()
{
    // 初始化播放器与音频输出
    m_backgroundPlayer = new QMediaPlayer(this);
    m_backgroundAudio = new QAudioOutput(this);
    m_backgroundPlayer->setAudioOutput(m_backgroundAudio);
    m_backgroundAudio->setVolume(0.5);
    m_backgroundPlayer->setLoops(QMediaPlayer::Infinite);

    connect(m_backgroundPlayer, &QMediaPlayer::errorOccurred, this, [=](QMediaPlayer::Error err, const QString& msg) {
        qWarning() << "背景音乐错误：" << err << msg;
        qWarning() << "当前路径：" << m_backgroundPlayer->source();
    });

    connect(m_backgroundPlayer, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status) {
        if ((status == QMediaPlayer::LoadedMedia || status == QMediaPlayer::BufferedMedia)
            && m_enableBackgroundMusic
            && m_backgroundPlayer->playbackState() != QMediaPlayer::PlayingState) {
            m_backgroundPlayer->play();
        }
        if (status == QMediaPlayer::EndOfMedia) {
            m_backgroundPlayer->setPosition(0);
            m_backgroundPlayer->play();
        }
    });

    const QString backgroundFilePath = resolveBackgroundMusicPath();
    if (!backgroundFilePath.isEmpty()) {
        m_backgroundPlayer->setSource(QUrl::fromLocalFile(backgroundFilePath));
    } else {
        qWarning() << "未找到可播放的背景音乐文件";
    }

    // -----------------------------------------------------

    
    // 初始化音效
    m_growthSound = new QSoundEffect(this);
    m_completeSound = new QSoundEffect(this);
    m_pauseSound = new QSoundEffect(this);
    m_resumeSound = new QSoundEffect(this);

    configureToneEffect(m_growthSound, QStringLiteral("lsystem_growth.wav"), 740.0, 120, 0.18);
    configureToneEffect(m_completeSound, QStringLiteral("lsystem_complete.wav"), 520.0, 220, 0.22);
    configureToneEffect(m_pauseSound, QStringLiteral("lsystem_pause.wav"), 320.0, 110, 0.16);
    configureToneEffect(m_resumeSound, QStringLiteral("lsystem_resume.wav"), 620.0, 130, 0.16);
    
    // 设置音效音量（适中，突出而不突兀）
    m_growthSound->setVolume(0.4);
    m_completeSound->setVolume(0.5);
    m_pauseSound->setVolume(0.3);
    m_resumeSound->setVolume(0.3);
}

QString LSystem::createToneWavFile(const QString& fileName, double frequency, int durationMs, double amplitude) const
{
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (tempPath.isEmpty()) {
        tempPath = QDir::tempPath();
    }

    QDir tempDir(tempPath);
    const QString filePath = tempDir.filePath(fileName);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return QString();
    }

    const int sampleRate = 22050;
    const int channels = 1;
    const int bitsPerSample = 16;
    const int bytesPerSample = bitsPerSample / 8;
    const int sampleCount = sampleRate * durationMs / 1000;
    const int dataSize = sampleCount * channels * bytesPerSample;
    const double durationSeconds = durationMs / 1000.0;
    const double attackSeconds = 0.02;
    const double releaseSeconds = 0.06;

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.writeRawData("RIFF", 4);
    stream << quint32(36 + dataSize);
    stream.writeRawData("WAVE", 4);
    stream.writeRawData("fmt ", 4);
    stream << quint32(16);
    stream << quint16(1);
    stream << quint16(channels);
    stream << quint32(sampleRate);
    stream << quint32(sampleRate * channels * bytesPerSample);
    stream << quint16(channels * bytesPerSample);
    stream << quint16(bitsPerSample);
    stream.writeRawData("data", 4);
    stream << quint32(dataSize);

    for (int i = 0; i < sampleCount; ++i) {
        const double time = static_cast<double>(i) / sampleRate;
        double envelope = 1.0;
        if (time < attackSeconds) {
            envelope = time / attackSeconds;
        } else if (time > durationSeconds - releaseSeconds) {
            envelope = qMax(0.0, (durationSeconds - time) / releaseSeconds);
        }

        const double wave = qSin(2.0 * PI * frequency * time) * amplitude * envelope;
        const qint16 sample = static_cast<qint16>(qBound(-1.0, wave, 1.0) * 32767.0);
        stream << sample;
    }

    file.close();
    return filePath;
}

void LSystem::configureToneEffect(QSoundEffect* effect, const QString& fileName, double frequency, int durationMs, double amplitude)
{
    if (!effect) {
        return;
    }

    const QString filePath = createToneWavFile(fileName, frequency, durationMs, amplitude);
    if (!filePath.isEmpty()) {
        effect->setSource(QUrl::fromLocalFile(filePath));
    }
}

// 播放有节奏感的生长音效
void LSystem::playGrowthSound()
{
    if (!m_enableGrowthSounds) return;

    if (m_growthSound) {
        m_growthSound->play();
    }
}

// 播放平缓的背景音乐
void LSystem::playBackgroundMusic()
{
    if (!m_enableBackgroundMusic) return;
    
    // 播放自定义背景音乐
    if (m_backgroundPlayer && !m_backgroundPlayer->source().isEmpty() && m_backgroundPlayer->playbackState() != QMediaPlayer::PlayingState) {
        if (m_backgroundPlayer->playbackState() == QMediaPlayer::StoppedState) {
            m_backgroundPlayer->setPosition(0);
        }
        m_backgroundPlayer->play();
    }
}

// 停止背景音乐
void LSystem::stopBackgroundMusic()
{
    if (m_backgroundPlayer && m_backgroundPlayer->playbackState() == QMediaPlayer::PlayingState) {
        m_backgroundPlayer->stop();
    }
}

QString LSystem::resolveBackgroundMusicPath() const
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QStringList candidates = {
        QDir(appDir).filePath(QStringLiteral("audio/background3.mp3")),
        QDir(appDir).filePath(QStringLiteral("../audio/background3.mp3")),
        QDir(appDir).filePath(QStringLiteral("audio/background.mp3")),
        QDir(appDir).filePath(QStringLiteral("../audio/background.mp3"))
    };

    for (const QString& candidate : candidates) {
        QFileInfo info(candidate);
        if (info.exists() && info.isFile() && info.size() > 0) {
            return info.absoluteFilePath();
        }
    }

    QFile resourceFile(QStringLiteral(":/audio/background3.mp3"));
    if (!resourceFile.exists()) {
        resourceFile.setFileName(QStringLiteral(":/audio/audio/background3.mp3"));
    }
    if (resourceFile.exists()) {
        const QString extracted = extractResourceToTempFile(resourceFile.fileName(), QStringLiteral("lsystem_background3.mp3"));
        if (!extracted.isEmpty()) {
            return extracted;
        }
    }

    resourceFile.setFileName(QStringLiteral(":/audio/background.mp3"));
    if (!resourceFile.exists()) {
        resourceFile.setFileName(QStringLiteral(":/audio/audio/background.mp3"));
    }
    if (resourceFile.exists()) {
        const QString extracted = extractResourceToTempFile(resourceFile.fileName(), QStringLiteral("lsystem_background.mp3"));
        if (!extracted.isEmpty()) {
            return extracted;
        }
    }

    return QString();
}

QString LSystem::extractResourceToTempFile(const QString& resourcePath, const QString& fileName) const
{
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (tempPath.isEmpty()) {
        tempPath = QDir::tempPath();
    }

    QDir tempDir(tempPath);
    const QString filePath = tempDir.filePath(fileName);

    QFile targetFile(filePath);
    if (targetFile.exists() && targetFile.size() > 0) {
        return filePath;
    }

    QFile sourceFile(resourcePath);
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开资源文件：" << resourcePath;
        return QString();
    }

    if (!targetFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "无法创建临时音频文件：" << filePath;
        return QString();
    }

    targetFile.write(sourceFile.readAll());
    targetFile.close();
    sourceFile.close();
    return filePath;
}

// 动画更新
void LSystem::updateAnimation()
{
    m_animationStep++;
    
    // 有节奏感的生长音效系统
    if (m_enableGrowthSounds) {
        // 根据不同的生长阶段播放不同的音效
        if (m_animationStep % 3 == 0) { // 每3步播放一次轻快的生长音效
            playGrowthSound();
        }
        
        // 在关键节点播放特殊音效
        if (m_animationStep == m_totalSteps / 4) {
            // 生长到1/4时的音效
            if (m_completeSound) {
                m_completeSound->setVolume(0.3);
                // m_completeSound->play();
            }
        } else if (m_animationStep == m_totalSteps / 2) {
            // 生长到一半时的音效
            if (m_completeSound) {
                m_completeSound->setVolume(0.4);
                // m_completeSound->play();
            }
        } else if (m_animationStep == m_totalSteps * 3 / 4) {
            // 生长到3/4时的音效
            if (m_completeSound) {
                m_completeSound->setVolume(0.5);
                // m_completeSound->play();
            }
        }
    }
    
    if (m_animationStep > m_totalSteps) {
        m_animationTimer->stop();
        m_isAnimating = false;
        m_animationStep = 0;
        
        // 动画结束时播放完整的完成音效
        if (m_enableGrowthSounds && m_completeSound) {
            m_completeSound->setVolume(0.6);
            m_completeSound->play();
        }
    }
    
    update();
}

// 按钮点击 - 生成新植物
void LSystem::onGenerateClicked()
{
    randomizeParams();
}

// 按钮点击 - 动画生长
void LSystem::onAnimateClicked()
{
    if (m_isAnimating) {
        m_animationTimer->stop();
        m_isAnimating = false;
        m_animationStep = 0;
    }
    else {
        m_animationStep = 0;
        m_isAnimating = true;
        m_animationTimer->start(m_animationInterval);
        playBackgroundMusic();
    }
    update();
}

// 按钮点击 - 选择颜色
void LSystem::onColorClicked()
{
    QColor color = QColorDialog::getColor(m_params.stemColor, this, "选择茎干颜色");
    if (color.isValid()) {
        m_params.stemColor = color;
        update();
    }
}

// 按钮点击 - 保存图片
void LSystem::onSaveClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存图片",
        "LSystem_Plant.png", "PNG图片 (*.png);;所有文件 (*.*)");

    if (!fileName.isEmpty()) {
        // 创建pixmap并绘制
        QPixmap pixmap(size());
        QPainter painter(&pixmap);
        render(&painter);
        pixmap.save(fileName);
    }
}
