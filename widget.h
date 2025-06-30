#ifndef WIDGET_H
#define WIDGET_H

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QSet>
#include <QWidget>
#include <QGraphicsRectItem>
#include <QMediaDevices>
#include <QAudioSource>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QBuffer>
#include <QFile>
#include <QByteArray>
#include <QFile>
#include <QtGlobal>
#include <QMediaPlayer>  // 添加 QMediaPlayer 的头文件
#include "qaudioinput.h"
#include "VirtualKeyboard.h"

class PianoKeyItem2 : public QGraphicsRectItem
{
public:
    PianoKeyItem2(const QRectF &rect, bool isWhiteKey, std::shared_ptr<VirtualKeyboard> myBoard_);
    int key_number;
    std::shared_ptr<VirtualKeyboard> myBoard;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QColor normalColor;
    QColor pressColor;
};

//定义电脑键盘琴键类
class KeyBoardItem: public QObject
{
    Q_OBJECT
public:
    KeyBoardItem(std::shared_ptr<VirtualKeyboard> myBoard_);
    std::shared_ptr<VirtualKeyboard> myBoard;
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    static int getNoteFromKey(int key);
private:
    QSet<int> pressedKeys; // 用于跟踪当前按下的键
};

namespace Ui { class Widget; }

class Widget2 : public QWidget
{
    Q_OBJECT

public:
    explicit Widget2(QWidget *parent = nullptr);
    ~Widget2();
    std::shared_ptr<VirtualKeyboard> myKeyboard;
    std::shared_ptr<KeyBoardItem> myBoardItem;

private slots:
    void on_startrecord_clicked();
    void on_stoprecord_clicked();
    void on_play_clicked();

private:
    void stopRecording();  // 添加声明
    void writeWavHeader(const QAudioFormat &format);

    Ui::Widget *ui;
    QAudioSource *m_audioInput = nullptr;
    QAudioOutput *m_audioOutput = nullptr;  // 播放用
    QByteArray m_recordedData;  // 存储录制的音频数据
    QBuffer *m_audioBuffer = nullptr;
    QFile *m_audioFile = nullptr;
    QMediaPlayer *m_player = nullptr;
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
signals:
    void request2();
};


#endif // WIDGET_H
