#include "documentdialog.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QFile>
#include <QMessageBox>
#include <QTemporaryFile>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <QUuid>
#include <QTimer>       // 添加QTimer头文件
#include <QUrl>         // 添加QUrl头文件
#include <QCoreApplication> // 添加qApp和aboutToQuit
#include <QApplication>  // 替代qApp的另一种方式

DocumentDialog::DocumentDialog(QWidget *parent)
    : QDialog(parent),
    btnClose(new QPushButton("关闭", this)),
    layout(new QVBoxLayout(this))
{
    setWindowTitle("帮助文档");
    resize(400, 100);

    layout->addWidget(btnClose);
    setLayout(layout);

    connect(btnClose, &QPushButton::clicked, this, &QDialog::close);

    // 使用QApplication::instance()替代qApp
    QTimer::singleShot(100, this, &DocumentDialog::openWordDocument);
}

void DocumentDialog::openWordDocument()
{
    // 1. 验证资源文件
    const QString resourcePath = ":/docs/help.docx";
    if(!QFile(resourcePath).exists()){
        QMessageBox::warning(this, "错误",
                             "内置帮助文档不存在！\n"
                             "请检查：\n"
                             "1. resources.qrc配置\n"
                             "2. 文件是否在/docs目录下");
        return;
    }

    // 2. 创建带随机UUID的临时文件
    QString tempPath = QDir::toNativeSeparators(
        QDir::tempPath() + "/piano_help_" +
        QUuid::createUuid().toString(QUuid::Id128) + ".docx"
        );

    // 3. 复制资源文件
    QFile resFile(resourcePath);
    QFile tempFile(tempPath);

    if(!resFile.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this, "错误", "无法读取内置文档！");
        return;
    }

    if(!tempFile.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this, "错误", "无法创建临时文件！");
        return;
    }

    tempFile.write(resFile.readAll());
    tempFile.close();
    resFile.close();

    // 4. 设置文件权限（Windows需要）
    tempFile.setPermissions(
        QFile::ReadOwner | QFile::WriteOwner |
        QFile::ReadUser | QFile::WriteUser |
        QFile::ReadOther | QFile::WriteOther
        );

    // 5. 跨平台打开方式（三重保障）
    bool opened = false;
#ifdef Q_OS_WIN
    // 方式1：使用默认关联
    opened = QDesktopServices::openUrl(QUrl::fromLocalFile(tempPath));

    // 方式2：使用Windows API
    if(!opened){
        opened = QProcess::startDetached("rundll32.exe",
                                         {"url.dll,FileProtocolHandler", tempPath});
    }

    // 方式3：直接调用Word
    if(!opened){
        opened = QProcess::startDetached("winword.exe", {tempPath});
    }
#else
    opened = QDesktopServices::openUrl(QUrl::fromLocalFile(tempPath));
#endif

    // 6. 错误处理
    if(!opened){
        QMessageBox::critical(this, "错误",
                              QString("无法打开Word文档！\n"
                                      "可能原因：\n"
                                      "1. 未安装Office/WPS\n"
                                      "2. 文件关联错误\n"
                                      "3. 权限不足\n\n"
                                      "临时文件路径：\n%1").arg(tempPath));
    }

    // 7. 程序退出时清理临时文件
    connect(QApplication::instance(), &QCoreApplication::aboutToQuit, [tempPath](){
        if(QFile::exists(tempPath)){
            QFile::remove(tempPath);
        }
    });
}
