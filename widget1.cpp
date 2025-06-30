#include "widget1.h"
#include "ui_widget1.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPushButton>
#include <QVBoxLayout> // 用于布局
#include<QMenu>
#include<QKeyEvent>
#include <QTimer>

int KeyBoardItem1::getNoteFromKey(int key)
{
    switch(key)
    {
        // QWERTY行（白键）
    case Qt::Key_Q: return 60; // C4
    case Qt::Key_W: return 62; // D4
    case Qt::Key_E: return 64; // E4
    case Qt::Key_R: return 65; // F4
    case Qt::Key_T: return 67; // G4
    case Qt::Key_Y: return 69; // A4
    case Qt::Key_U: return 71; // B4
    case Qt::Key_I: return 72; // C5
    case Qt::Key_O: return 74; // D5
    case Qt::Key_P: return 76; // E5

        // 数字行（黑键）
    case Qt::Key_2: return 61; // C#4
    case Qt::Key_3: return 63; // D#4
    case Qt::Key_5: return 66; // F#4
    case Qt::Key_6: return 68; // G#4
    case Qt::Key_7: return 70; // A#4
    case Qt::Key_9: return 73; // C#5
    case Qt::Key_0: return 75; // D#5

        // ZXCV行（白键）
    case Qt::Key_Z: return 48; // C3
    case Qt::Key_X: return 50; // D3
    case Qt::Key_C: return 52; // E3
    case Qt::Key_V: return 53; // F3
    case Qt::Key_B: return 55; // G3
    case Qt::Key_N: return 57; // A3
    case Qt::Key_M: return 59; // B3
    case Qt::Key_Comma: return 60; // C4
    case Qt::Key_Period: return 62; // D4
    case Qt::Key_Slash: return 64; // E4

        // ASDF行（黑键）
    case Qt::Key_S: return 49; // C#3
    case Qt::Key_D: return 51; // D#3
    case Qt::Key_G: return 54; // F#3
    case Qt::Key_H: return 56; // G#3
    case Qt::Key_J: return 58; // A#3
    case Qt::Key_L: return 61; // C#4
    case Qt::Key_Semicolon: return 63; // D#4

        // 无效按键
    default: return -1;
    }
}
KeyBoardItem1::KeyBoardItem1(std::shared_ptr<VirtualKeyboard> myBoard_):myBoard(myBoard_){}

// 自定义按键构造函数：设置矩形和初始颜色
PianoKeyItem::PianoKeyItem(const QRectF &rect,bool isWhiteKey, std::shared_ptr<VirtualKeyboard> myBoard_)
    : QGraphicsRectItem(rect) {
    normalColor = Qt::transparent; // 透明背景
    // 白键按压效果：半透明白色
    pressColor = isWhiteKey ? QColor(201, 199, 200, 100) :
                     QColor(0, 0, 0, 150);    // 黑键按压效果：半透明黑色

    setBrush(normalColor);             // 默认颜色透明
    setPen(Qt::NoPen);                 // 无边框线
    setAcceptedMouseButtons(Qt::LeftButton); // 接收鼠标左键事件
    setZValue(isWhiteKey ? 1 : 2);                      // 确保在图像之上
    myBoard=myBoard_;
}
//鼠标按下
void PianoKeyItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event);
    setBrush(pressColor);
    myBoard->input->getInput(key_number,0);
    myBoard->process();
}
//鼠标释放
void PianoKeyItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event);
    setBrush(normalColor);
    myBoard->input->getInput(key_number,1);
    myBoard->process();
}

//键盘按下
void KeyBoardItem1::keyPressEvent(QKeyEvent *event)
{
    int key=event->key();
    int note=getNoteFromKey(key);
    if(note!=-1 && (!event->isAutoRepeat()))
    {
        myBoard->input->getInput(note,0);
        myBoard->process();
    }
}
//键盘释放
void KeyBoardItem1::keyReleaseEvent(QKeyEvent *event)
{
    int key=event->key();
    int note=getNoteFromKey(key);
    if(note!=-1 && (!event->isAutoRepeat()))
    {
        myBoard->input->getInput(note,1);
        myBoard->process();
    }
}

void Widget1::keyPressEvent(QKeyEvent *event) {
    myBoardItem->keyPressEvent(event);
}
void Widget1::keyReleaseEvent(QKeyEvent *event) {
    myBoardItem->keyReleaseEvent(event);
}


Widget1::Widget1(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget1),
    view(new QGraphicsView(this)),
    scene(new QGraphicsScene(this))
{
    ui->setupUi(this);
    qDebug() << "Widget constructor started";

    //创建键盘，输入输出
    std::shared_ptr<InputHandler> myMouse = std::make_shared<MouseInput>();
    std::shared_ptr<OutputHandler> mySynth = std::make_shared<SynthOutput>();
    myKeyboard = std::make_shared<VirtualKeyboard>(myMouse, mySynth);
    myBoardItem = std::make_shared<KeyBoardItem1>(myKeyboard);

    //创建图形

    view->setScene(scene);
    view->setFixedSize(902,215);//设置大小
    view->move(0,290);//设置位置
    //连接主界面
    connect(ui->back,&QPushButton::clicked,this,[this](){
        emit request();
        this->close();
    },Qt::QueuedConnection);

    // // 假设 UI 里没有默认布局，手动创建垂直布局
    // QVBoxLayout *mainLayout = new QVBoxLayout(this);
    // // 添加钢琴键盘的 QGraphicsView 到布局
    // mainLayout->addWidget(view);
    // // 创建一个按钮
    // QPushButton *myButton = new QPushButton("测试按钮", this);
    // // 连接按钮点击信号与槽（这里简单打印，你可替换成实际逻辑）
    // connect(myButton, &QPushButton::clicked, this, []() {
    //     qDebug() << "按钮被点击啦";
    // });
    // // 将按钮添加到布局中，会显示在钢琴键盘视图下方
    // mainLayout->addWidget(myButton);
    //背景
    QPixmap b1(":/images/b1.jpg");
    QPixmap b2(":/images/b2.jpg");
    QPixmap b3(":/images/b3.jpg");
    QPixmap b4(":/images/b4.jpg");
    QPixmap big(":/images/keyboard_image.png");
    if (!b1.isNull()) {
        QPalette pal = palette();
        pal.setBrush(QPalette::Window, b1.scaled(size(), Qt::KeepAspectRatioByExpanding));
        setPalette(pal);
    } else {
        qWarning() << "图片加载失败！路径:" << "image/keyboard_image.png";
    }
    //加载图片
    QPixmap pianoImage(":/images/keyboard.png");
    QGraphicsPixmapItem *bg = new QGraphicsPixmapItem(pianoImage.scaled(view->size(), Qt::KeepAspectRatio));
    bg->setZValue(0);
    scene->addItem(bg);

    ui->musicnote->setWordWrap(true);

    QTimer *timer=new QTimer(this);
    connect(timer, &QTimer::timeout, this,[=](){
        const std::set<int>& notes = myKeyboard->getNoteList();
        QString noteText = "Playing Notes: ";
        for (int note : notes) {
            noteText += VirtualKeyboard::noteNames[note] + " ";
        }
        ui->musicnote->setText(noteText);
    });
    timer->start(100);

    // 白键区域坐标
    QList<int> whiteKeyX = {
        0, 43, 86, 129, 172, 215, 258,
        301, 344, 387, 430, 473, 516, 559,
        602, 645, 688, 731, 774, 817, 860
    };
    QList<int> whiteKeyN =
        {
            36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71
        };
    for(int i = 0;i < 21;i ++)
    {
        int x=whiteKeyX[i];
        int n=whiteKeyN[i];
        PianoKeyItem *key = new PianoKeyItem(QRectF(x,0,40,206),true, myKeyboard);
        key->key_number=n;
        scene->addItem(key);
    }
    // 黑键区域坐标（相对偏移手动估算）
    QList<int> blackKeyX = {
                            28,  73, 157, 200, 245,
                            329, 372,458, 501,
                            544,628,  673,757,  801,844
    };
    QList<int> blackKeyN=
        {
            37, 39, 42, 44, 46, 49, 51, 54, 56, 58, 61, 63, 66, 68, 70
        };

    for (int i = 0;i < 15;i ++) {
        int x=blackKeyX[i];
        int n=blackKeyN[i];
        PianoKeyItem *blackKey = new PianoKeyItem(QRectF(x, 0, 24, 120),false, myKeyboard);
        blackKey->key_number=n;
        scene->addItem(blackKey);
    }
    scene->setSceneRect(pianoImage.rect());

    //以下为基础功能的实现
    //1.基础认音
    //载入曲库图片
    QPixmap music1(":/images/star.png");
    QPixmap music2(":/images/tiger.png");
    QPixmap music3(":/images/birthday.png");

    ui->musicnote->setVisible(false);
    ui->music->setScaledContents(true);
    connect(ui->basicrecognition,&QPushButton::clicked,this,[=](){
        ui->musicnote->setVisible(true);
        ui->musicnote->setText("pressed");
        ui->music->setVisible(false);
    });
    //2.曲库
    //创建菜单
    QMenu *musiclibrary = new QMenu(this);
    const int width=1000;
    const int height=290;
    //菜单项

    if (!music1.isNull()) {
        QPixmap scaledMusic1 = music1.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->music->setPixmap(scaledMusic1);
        ui->music->setScaledContents(false);
        ui->music->setFixedSize(1000, 290);
        ui->music->move(120, 0);
        ui->music->setVisible(true);
        // 若要置顶，可加上
        ui->music->raise();
    } else {
        qDebug() << "图片加载失败，检查资源路径222";
    }
    ui->music->setVisible(false);

    musiclibrary->addAction("小星星",this,[this,music1](){
        if(ui->musicnote){
            ui->musicnote->setVisible(false);
        }
        QPixmap scaledMusic1 = music1.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->music->setPixmap(scaledMusic1);
        ui->music->setVisible(true);
    });
    musiclibrary->addAction("两只老虎",this,[this,music2](){
        if(ui->musicnote){
            ui->musicnote->setVisible(false);
        }
        QPixmap scaledMusic2 = music2.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->music->setPixmap(scaledMusic2);
        ui->music->setVisible(true);
    });
    musiclibrary->addAction("生日歌",this,[this,music3](){
        if(ui->musicnote){
            ui->musicnote->setVisible(false);
        }
        QPixmap scaledMusic3 = music3.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->music->setPixmap(scaledMusic3);
        ui->music->setVisible(true);
    });
    ui->menu->setMenu(musiclibrary);
    ui->menu->setPopupMode(QToolButton::InstantPopup);
    //背景
    QMenu *bl = new QMenu(this);
    bl->addAction("默认",this,[this,b1](){
        if (!b1.isNull()) {
            QPalette pal = palette();
            pal.setBrush(QPalette::Window, b1.scaled(size(), Qt::KeepAspectRatioByExpanding));
            setPalette(pal);
        } else {
            qWarning() << "图片加载失败！路径:" << "image/keyboard_image.png";
        }
    });
    bl->addAction("淡雅",this,[this,b2](){
        if (!b2.isNull()) {
            QPalette pal = palette();
            pal.setBrush(QPalette::Window, b2.scaled(size(), Qt::KeepAspectRatioByExpanding));
            setPalette(pal);
        } else {
            qWarning() << "图片加载失败！路径:" << "image/keyboard_image.png";
        }
    });
    bl->addAction("清新",this,[this,b3](){
        if (!b3.isNull()) {
            QPalette pal = palette();
            pal.setBrush(QPalette::Window, b3.scaled(size(), Qt::KeepAspectRatioByExpanding));
            setPalette(pal);
        } else {
            qWarning() << "图片加载失败！路径:" << "image/keyboard_image.png";
        }
    });
    bl->addAction("高贵",this,[this,b4](){
        if (!b4.isNull()) {
            QPalette pal = palette();
            pal.setBrush(QPalette::Window, b4.scaled(size(), Qt::KeepAspectRatioByExpanding));
            setPalette(pal);
        } else {
            qWarning() << "图片加载失败！路径:" << "image/keyboard_image.png";
        }
    });

    ui->switch_2->setMenu(bl);
    ui->switch_2->setPopupMode(QToolButton::InstantPopup);


}

Widget1::~Widget1()
{
    delete ui;
    delete view;
    delete scene;
}
