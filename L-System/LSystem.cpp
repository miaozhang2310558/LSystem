#include "LSystem.h"
#include <QPainter>
#include <stack>
#include <cmath>

LSystem::LSystem(QWidget* parent) : QMainWindow(parent) {
    ui.setupUi(this);

    // 初始化 L-System 规则
    ls = LSystemLogic("X");
    ls.addRule('X', "F-[[X]+X]+F[+FX]-X");
    ls.addRule('F', "FF");
}

LSystem::~LSystem() {}

void LSystem::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::green);

    // 初始位置：窗口底部中央，向上画
    painter.translate(width() / 2, height() - 20);
    painter.rotate(-90);

    std::string commands = ls.generate(4); // 迭代次数不要太大，否则会卡顿
    float length = 5.0f;
    float angle = 25.0f;

    struct State { QTransform transform; };
    std::stack<State> stateStack;

    for (char c : commands) {
        if (c == 'F') {
            painter.drawLine(0, 0, length, 0);
            painter.translate(length, 0);
        }
        else if (c == '+') {
            painter.rotate(angle);
        }
        else if (c == '-') {
            painter.rotate(-angle);
        }
        else if (c == '[') {
            stateStack.push({ painter.transform() });
        }
        else if (c == ']') {
            if (!stateStack.empty()) {
                painter.setTransform(stateStack.top().transform);
                stateStack.pop();
            }
        }
    }
}