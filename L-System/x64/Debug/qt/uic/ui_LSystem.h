/********************************************************************************
** Form generated from reading UI file 'LSystem.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LSYSTEM_H
#define UI_LSYSTEM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LSystemClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *LSystemClass)
    {
        if (LSystemClass->objectName().isEmpty())
            LSystemClass->setObjectName("LSystemClass");
        LSystemClass->resize(600, 400);
        menuBar = new QMenuBar(LSystemClass);
        menuBar->setObjectName("menuBar");
        LSystemClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(LSystemClass);
        mainToolBar->setObjectName("mainToolBar");
        LSystemClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(LSystemClass);
        centralWidget->setObjectName("centralWidget");
        LSystemClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(LSystemClass);
        statusBar->setObjectName("statusBar");
        LSystemClass->setStatusBar(statusBar);

        retranslateUi(LSystemClass);

        QMetaObject::connectSlotsByName(LSystemClass);
    } // setupUi

    void retranslateUi(QMainWindow *LSystemClass)
    {
        LSystemClass->setWindowTitle(QCoreApplication::translate("LSystemClass", "LSystem", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LSystemClass: public Ui_LSystemClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LSYSTEM_H
