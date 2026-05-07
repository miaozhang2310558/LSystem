#include "LSystem.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    // 设置字体（解决中文乱码）
    QFont font("Microsoft YaHei", 9);
    a.setFont(font);

    LSystem w;
    w.show();

    return a.exec();
}