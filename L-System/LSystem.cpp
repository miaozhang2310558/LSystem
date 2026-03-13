#include "LSystem.h"
#include <cmath>
#include <QApplication>
#include <QScreen>

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

    // 默认参数
    m_params.angle = 25.0;
    m_params.length = 15.0;
    m_params.stemColor = QColor(0, 150, 0); // 绿色茎干
    m_params.tipColor = QColor(255, 0, 0);  // 红色顶端
    m_params.axiom = "F";
    m_params.rules['F'] = "FF+[+F-F-F]-[-F+F+F]"; // 经典植物规则

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
    setWindowTitle("L-System分形植物生成器 2.0");
    setMinimumSize(800, 600);

    // 主布局（垂直）
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 控制面板（水平布局）
    QHBoxLayout* controlLayout = new QHBoxLayout();

    // 1. 植物预设选择
    QLabel* presetLabel = new QLabel("植物类型:");
    QComboBox* presetCombo = new QComboBox();
    presetCombo->addItem("经典植物", ClassicPlant);
    presetCombo->addItem("蕨类", Fern);
    presetCombo->addItem("树木", Tree);
    connect(presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LSystem::onPresetChanged);

    // 2. 迭代次数控制
    QLabel* iterLabel = new QLabel("迭代次数:");
    QSpinBox* iterSpin = new QSpinBox();
    iterSpin->setRange(MIN_ITERATIONS, MAX_ITERATIONS);
    iterSpin->setValue(m_iterations);
    connect(iterSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int val) {
                m_iterations = val;
                m_lsystemString = generateLSystemString();
                m_totalSteps = m_lsystemString.length();
                update();
            });

    // 3. 角度控制
    QLabel* angleLabel = new QLabel("旋转角度:");
    QSlider* angleSlider = new QSlider(Qt::Horizontal);
    angleSlider->setRange(10, 60);
    angleSlider->setValue(static_cast<int>(m_params.angle));
    angleSlider->setFixedWidth(100);
    connect(angleSlider, &QSlider::valueChanged,
            this, [this](int val) {
                m_params.angle = static_cast<double>(val);
                update();
            });

    // 4. 长度控制
    QLabel* lengthLabel = new QLabel("线段长度:");
    QSlider* lengthSlider = new QSlider(Qt::Horizontal);
    lengthSlider->setRange(5, 30);
    lengthSlider->setValue(static_cast<int>(m_params.length));
    lengthSlider->setFixedWidth(100);
    connect(lengthSlider, &QSlider::valueChanged,
            this, [this](int val) {
                m_params.length = static_cast<double>(val);
                update();
            });

    // 5. 功能按钮
    QPushButton* generateBtn = new QPushButton("随机生成");
    connect(generateBtn, &QPushButton::clicked, this, &LSystem::onGenerateClicked);

    QPushButton* animateBtn = new QPushButton("动画生长");
    connect(animateBtn, &QPushButton::clicked, this, &LSystem::onAnimateClicked);

    QPushButton* colorBtn = new QPushButton("选择颜色");
    connect(colorBtn, &QPushButton::clicked, this, &LSystem::onColorClicked);

    QPushButton* saveBtn = new QPushButton("保存图片");
    connect(saveBtn, &QPushButton::clicked, this, &LSystem::onSaveClicked);

    // 添加控件到控制面板
    controlLayout->addWidget(presetLabel);
    controlLayout->addWidget(presetCombo);
    controlLayout->addSpacing(10);
    controlLayout->addWidget(iterLabel);
    controlLayout->addWidget(iterSpin);
    controlLayout->addSpacing(10);
    controlLayout->addWidget(angleLabel);
    controlLayout->addWidget(angleSlider);
    controlLayout->addSpacing(10);
    controlLayout->addWidget(lengthLabel);
    controlLayout->addWidget(lengthSlider);
    controlLayout->addSpacing(20);
    controlLayout->addWidget(generateBtn);
    controlLayout->addWidget(animateBtn);
    controlLayout->addWidget(colorBtn);
    controlLayout->addWidget(saveBtn);
    controlLayout->addStretch();

    // 绘制区域（占满剩余空间）
    //QWidget* drawWidget = new QWidget();
    //drawWidget->setStyleSheet("background-color: white;");
    //drawWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 添加到主布局
    mainLayout->addLayout(controlLayout);
    //mainLayout->addWidget(drawWidget, 1); // 1表示占满剩余空间

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

// 绘制分形
void LSystem::drawLSystem(QPainter& painter, int step)
{
    painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿

    // 起始位置（窗口底部中间）
    int startX = width() / 2;
    int startY = height() - 50;

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

            painter.setPen(QPen(color, 2));
            painter.drawLine(QPointF(x, y), QPointF(newX, newY));


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

// 绘制事件
void LSystem::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    // 绘制背景
    painter.fillRect(rect(), QColor(245, 245, 245));

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
    update(); // 窗口大小变化时重绘
}

// 切换植物预设
void LSystem::onPresetChanged(int index)
{
    PlantPreset preset = static_cast<PlantPreset>(sender()->property("currentData").toInt());

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

// 动画更新
void LSystem::updateAnimation()
{
    m_animationStep++;
    if (m_animationStep > m_totalSteps) {
        m_animationTimer->stop();
        m_isAnimating = false;
        m_animationStep = 0;
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
        m_animationTimer->start(ANIMATION_INTERVAL);
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
