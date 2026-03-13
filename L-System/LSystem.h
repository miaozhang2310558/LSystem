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
#include <random>
#include <QStack>
#include <utility>
#include <QPointF>

// L-System分形植物生成器 2.0
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

private slots:
    // 按钮点击槽函数
    void onGenerateClicked();       // 生成新植物
    void onAnimateClicked();        // 动画生长
    void onColorClicked();          // 选择颜色
    void onSaveClicked();           // 保存图片
    void onPresetChanged(int index);// 切换植物预设
    void updateAnimation();         // 动画帧更新

private:
    // 初始化UI控制面板
    void initUI();
    // 生成L-System字符串
    QString generateLSystemString();
    // 绘制分形（支持分步动画）
    void drawLSystem(QPainter& painter, int step = -1);
    // 随机生成参数（让植物更自然）
    void randomizeParams();

    // L-System核心参数
    struct LSystemParams {
        QString axiom;          // 初始字符串
        QMap<QChar, QString> rules; // 生成规则
        double angle;           // 旋转角度(度)
        double length;          // 线段长度
        QColor stemColor;       // 茎干颜色
        QColor tipColor;        // 顶端颜色
    };

    // 预设植物类型
    enum PlantPreset {
        ClassicPlant,   // 经典分形植物
        Fern,           // 蕨类
        Tree            // 树木
    };

    // 成员变量
    LSystemParams m_params;         // 当前参数
    int m_iterations = 4;           // 迭代次数
    int m_animationStep = 0;        // 动画当前步数
    int m_totalSteps = 0;           // 动画总步数
    QTimer* m_animationTimer;       // 动画定时器
    bool m_isAnimating = false;     // 是否正在动画
    QString m_lsystemString;        // 生成的LSystem字符串
    std::mt19937 m_randomGen;      // 随机数生成器
};

#endif // LSYSTEM_H
