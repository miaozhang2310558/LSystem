#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_LSystem.h" // 这里的名字要和你的项目自动生成的匹配
#include <string>
#include <map>

// 1.0 的逻辑类
class LSystemLogic {
private:
    std::string axiom;
    std::map<char, std::string> rules;
public:
    LSystemLogic() {}
    LSystemLogic(std::string a) : axiom(a) {}
    void addRule(char input, std::string output) { rules[input] = output; }
    std::string generate(int iterations) {
        std::string current = axiom;
        for (int i = 0; i < iterations; ++i) {
            std::string next = "";
            for (char c : current) {
                if (rules.count(c)) next += rules[c];
                else next += c;
            }
            current = next;
        }
        return current;
    }
};

// 窗口类 - 请确保这里的类名与你的 main.cpp 中调用的一致
class LSystem : public QMainWindow {
    Q_OBJECT

public:
    LSystem(QWidget* parent = nullptr);
    ~LSystem();

protected:
    // 必须在这里声明 paintEvent，否则会报 C2065
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::LSystemClass ui; // 自动生成的 UI 变量
    LSystemLogic ls;     // 我们的逻辑类对象
};