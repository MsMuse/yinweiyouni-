#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QWidget>
#include <QPixmap>
#include<QPointer>
#include "documentdialog.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void paintEvent(QPaintEvent *event) override;  // 重写绘制事件，用于绘制背景
private slots:
    void on_btn_play_clicked();
    void on_btn_learn_clicked();
    void on_btn_info_clicked();
    void on_btn_settings_clicked();

private:
    Ui::MainWindow *ui;
    QPixmap m_background;  // 存储背景图片
    QPointer<QWidget> learnedWidget; // 使用 QPointer 来自动管理子窗口的生命周期

private:
    DocumentDialog *docDialog = nullptr;  // 文档对话框指针
};

#endif // MAINWINDOW_H
