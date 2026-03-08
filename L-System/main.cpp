#include "LSystem.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    LSystem window;
    window.show();
    return app.exec();
}
