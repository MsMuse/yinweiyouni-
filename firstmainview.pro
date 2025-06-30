QT += core gui widgets uitools # 统一配置 Qt 模块，涵盖必要内容
QT       += core gui
QT += core gui multimedia multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets  # 兼容旧版本写法，Qt 6 下也可保留
CONFIG += c++17
QT += core gui widgets network
# 可选：启用后，使用 Qt 6.0.0 之前弃用的 API 会编译报错，按需开启
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000


SOURCES += \
    InputHandler.cpp \
    MIDIMsg.cpp \
    OutputHandler.cpp \
    VirtualKeyboard.cpp \
    documentdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    volume.cpp \
    widget.cpp \
    widget1.cpp

HEADERS += \
    InputHandler.h \
    MIDIMsg.h \
    OutputHandler.h \
    VirtualKeyboard.h \
    VirtualKeyboard.h \
    VirtualKeyboard.h \
    documentdialog.h \
    mainwindow.h \
    settingsdialog.h \
    volume.h \
    widget.h \
    widget1.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui \
    widget.ui \
    widget1.ui

RESOURCES += \
    resources.qrc  # 确保 resources.qrc 实际存在，且路径正确
INCLUDEPATH += \
    $$PWD\lib\include
LIBS += \
    -LC:\Users\Housh\vcpkg\installed\x64-windows\lib

# 针对 Windows 平台的库依赖（按需保留，若不需要操作系统相关功能可去掉）
win32 {
    LIBS += -luuid
    LIBS += -lole32
    LIBS += -llibfluidsynth-3
}

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
