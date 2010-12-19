/********************************************************************************
** Form generated from reading UI file 'gui.ui'
**
** Created: Sun Dec 19 13:55:02 2010
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUI_H
#define UI_GUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_guiClass
{
public:
    QWidget *centralwidget;
    QPushButton *pushButton;
    QTextEdit *textEdit;
    QTextEdit *textEdit_2;
    QDoubleSpinBox *doubleSpinBox;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *guiClass)
    {
        if (guiClass->objectName().isEmpty())
            guiClass->setObjectName(QString::fromUtf8("guiClass"));
        guiClass->resize(975, 639);
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(guiClass->sizePolicy().hasHeightForWidth());
        guiClass->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(guiClass);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(310, 60, 85, 27));
        textEdit = new QTextEdit(centralwidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(20, 30, 261, 421));
        sizePolicy.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
        textEdit->setSizePolicy(sizePolicy);
        textEdit_2 = new QTextEdit(centralwidget);
        textEdit_2->setObjectName(QString::fromUtf8("textEdit_2"));
        textEdit_2->setGeometry(QRect(420, 20, 219, 439));
        doubleSpinBox = new QDoubleSpinBox(centralwidget);
        doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));
        doubleSpinBox->setGeometry(QRect(300, 20, 101, 31));
        doubleSpinBox->setDecimals(3);
        doubleSpinBox->setValue(2);
        guiClass->setCentralWidget(centralwidget);
        menubar = new QMenuBar(guiClass);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 975, 21));
        guiClass->setMenuBar(menubar);
        statusbar = new QStatusBar(guiClass);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        guiClass->setStatusBar(statusbar);

        retranslateUi(guiClass);
        QObject::connect(pushButton, SIGNAL(clicked()), guiClass, SLOT(CorrectAll()));

        QMetaObject::connectSlotsByName(guiClass);
    } // setupUi

    void retranslateUi(QMainWindow *guiClass)
    {
        guiClass->setWindowTitle(QApplication::translate("guiClass", "MainWindow", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("guiClass", "Correct All", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class guiClass: public Ui_guiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_H
