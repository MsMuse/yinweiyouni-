#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"widget.h"
#include "widget1.h" // 包含 Widget 类的头文件
#include <QMessageBox>
#include <QPainter>
#include <QFileDialog>
#include "settingsdialog.h"
#include "documentdialog.h"
#include<QFile>
#include <QDialog>          // 或 QWidget，根据 ui 类型
#include <QtUiTools/QUiLoader> // Qt 6 需显式包含此头文件


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 可选：设置窗口标题
    if (QFile::exists(":/images/background.png")) {
        qDebug() << "Resource file exists!";
    } else {
        qDebug() << "Resource file NOT found!";
    }

    // 加载背景
    m_background.load(":/images/background.png");
    qDebug() << "Background size:" << m_background.size();
    setWindowTitle("主页面");
    // 加载背景图片（两种方式选一种）：
    // 方式1：从资源文件加载（推荐）
    m_background.load(":/images/background.png");  // 确保图片已添加到资源文件

    // 方式2：从本地文件加载（需确保路径正确）
    // m_background.load("C:/path/to/background.jpg");

    // 如果图片加载失败，使用默认颜色
    if (m_background.isNull()) {
        m_background = QPixmap(size());
        m_background.fill(QColor(240, 240, 240));  // 浅灰色背景
    }

    // 设置按钮样式，使其在背景上更清晰
    QString buttonStyle = "QPushButton { "
                          "background-color: rgba(255, 255, 255, 180); "
                          "border: 2px solid #8f8f91; "
                          "border-radius: 8px; "
                          "padding: 8px; "
                          "font-size: 14px; "
                          "min-width: 100px; }"
                          "QPushButton:hover { "
                          "background-color: rgba(255, 255, 255, 220); }";
    // 连接信号与槽（也可以在 Designer 中直接右键 -> 转到槽）

}
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    // 缩放图片以适应窗口大小
    painter.drawPixmap(rect(), m_background.scaled(size(), Qt::KeepAspectRatioByExpanding));
    QMainWindow::paintEvent(event);  // 确保其他控件正常绘制
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_play_clicked()
{
    Widget2 *play = new Widget2(nullptr);
    play->setWindowTitle("演奏界面");
    play->setAttribute(Qt::WA_DeleteOnClose); // 窗口关闭时自动释放内存
    connect(play, &QWidget::destroyed, this, [this]() {
        this->show(); // 学习窗口销毁时，显示主窗口
    });
    connect(play, &Widget2::request2, this, &MainWindow::show);
    play->show();
    this->hide();
}

void MainWindow::on_btn_learn_clicked()
{
    // 直接创建 Widget 类的实例（而非动态加载 .ui 文件）

    Widget1 *learnedWidget = new Widget1(nullptr);
    learnedWidget->setWindowTitle("学习界面");
    learnedWidget->setAttribute(Qt::WA_DeleteOnClose); // 窗口关闭时自动释放内存
    connect(learnedWidget, &QWidget::destroyed, this, [this]() {
        this->show(); // 学习窗口销毁时，显示主窗口
    });
    connect(learnedWidget, &Widget1::request, this, &MainWindow::show);
    learnedWidget->show();
    this->hide();

}

// 在MainWindow中调用
void MainWindow::on_btn_info_clicked()
{
    // 方案1：直接模态打开（推荐）
    DocumentDialog docDialog(this);
    docDialog.exec();  // 阻塞式显示，窗口关闭后继续执行

    // 方案2：非模态打开（如需保持对话框）
    // DocumentDialog *docDialog = new DocumentDialog(this);
    // docDialog->setAttribute(Qt::WA_DeleteOnClose); // 自动内存管理
    // docDialog->show();
}

void MainWindow::on_btn_settings_clicked()
{
    SettingsDialog dlg(this);
    dlg.exec();  // 模态方式弹出设置窗口
}

