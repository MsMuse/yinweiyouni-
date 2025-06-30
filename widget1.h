#ifndef WIDGET1_H
#define WIDGET1_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include"VirtualKeyboard.h"

QT_BEGIN_NAMESPACE
//定义钢琴块类
class PianoKeyItem : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    PianoKeyItem(const QRectF &rect,bool isWhiteKey, std::shared_ptr<VirtualKeyboard> myBoard_);
    int key_number;
    std::shared_ptr<VirtualKeyboard> myBoard;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QColor normalColor = QColor(0, 0, 0, 0);       // 默认透明
    QColor pressColor = QColor(0, 0, 255, 80);     // 点击时蓝色半透明
    //QGraphicsPixmapItem *pressEffect; // 按压效果图片
};

//定义电脑键盘琴键类
class KeyBoardItem1: public QObject
{
    Q_OBJECT
public:
    KeyBoardItem1(std::shared_ptr<VirtualKeyboard> myBoard_);
    std::shared_ptr<VirtualKeyboard> myBoard;
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    static int getNoteFromKey(int key);
private:
    QSet<int> pressedKeys; // 用于跟踪当前按下的键
};
namespace Ui {
class Widget1;
}
QT_END_NAMESPACE

class Widget1 : public QWidget
{
    Q_OBJECT

public:
    explicit Widget1(QWidget *parent = nullptr);
    ~Widget1();
    std::shared_ptr<VirtualKeyboard> myKeyboard;
    std::shared_ptr<KeyBoardItem1> myBoardItem;

private:
    Ui::Widget1 *ui;
    QGraphicsView *view;
    QGraphicsScene *scene;
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
signals:
    void request();
};
#endif // WIDGET1_H
