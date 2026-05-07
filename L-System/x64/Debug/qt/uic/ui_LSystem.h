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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LSystemClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSlider *angleSlider;
    QSpinBox *iterSpinBox;
    QLabel *label_2;
    QLabel *label;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *LSystemClass)
    {
        if (LSystemClass->objectName().isEmpty())
            LSystemClass->setObjectName("LSystemClass");
        LSystemClass->resize(796, 400);
        centralWidget = new QWidget(LSystemClass);
        centralWidget->setObjectName("centralWidget");
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName("horizontalLayout");
        angleSlider = new QSlider(centralWidget);
        angleSlider->setObjectName("angleSlider");
        angleSlider->setMaximum(90);
        angleSlider->setValue(25);
        angleSlider->setOrientation(Qt::Orientation::Horizontal);

        horizontalLayout->addWidget(angleSlider);

        iterSpinBox = new QSpinBox(centralWidget);
        iterSpinBox->setObjectName("iterSpinBox");
        iterSpinBox->setMinimum(1);
        iterSpinBox->setMaximum(6);
        iterSpinBox->setValue(4);

        horizontalLayout->addWidget(iterSpinBox);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName("label_2");

        horizontalLayout->addWidget(label_2);

        label = new QLabel(centralWidget);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);


        verticalLayout->addLayout(horizontalLayout);

        LSystemClass->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(LSystemClass);
        mainToolBar->setObjectName("mainToolBar");
        LSystemClass->addToolBar(Qt::ToolBarArea::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(LSystemClass);
        statusBar->setObjectName("statusBar");
        LSystemClass->setStatusBar(statusBar);

        retranslateUi(LSystemClass);

        QMetaObject::connectSlotsByName(LSystemClass);
    } // setupUi

    void retranslateUi(QMainWindow *LSystemClass)
    {
        LSystemClass->setWindowTitle(QCoreApplication::translate("LSystemClass", "LSystem", nullptr));
        label_2->setText(QCoreApplication::translate("LSystemClass", "\345\201\217\350\275\254\350\247\222\345\272\246", nullptr));
        label->setText(QCoreApplication::translate("LSystemClass", "\350\277\255\344\273\243\346\254\241\346\225\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LSystemClass: public Ui_LSystemClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LSYSTEM_H
