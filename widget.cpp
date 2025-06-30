#include "widget.h"
#include "ui_widget.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QAudioFormat>
#include <QAudioSource>
#include <QMediaDevices>
#include <QFile>
#include<QMenu>
#include <QDebug>
#include<QSlider>
#include <QComboBox>
#include <QDial>
#include <QAudioOutput>
#include <QMediaPlayer>

int KeyBoardItem::getNoteFromKey(int key)
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
KeyBoardItem::KeyBoardItem(std::shared_ptr<VirtualKeyboard> myBoard_):myBoard(myBoard_){}

PianoKeyItem2::PianoKeyItem2(const QRectF &rect, bool isWhiteKey, std::shared_ptr<VirtualKeyboard> myBoard_)
    : QGraphicsRectItem(rect)
{
    normalColor = Qt::transparent;
    pressColor = isWhiteKey ? QColor(201, 199, 200, 100) : QColor(0, 0, 0, 150);
    setBrush(normalColor);
    setPen(Qt::NoPen);
    setAcceptedMouseButtons(Qt::LeftButton);
    setZValue(isWhiteKey ? 1 : 2);
    myBoard=myBoard_;
}

// 假设采样率 48000、通道数 2、采样格式 Float（对应 32 位）
void Widget2::writeWavHeader(const QAudioFormat &format)
{
    struct WavHeader {
        char riff[4] = {'R', 'I', 'F', 'F'};
        quint32 fileSize = 0;        // 文件总大小-8
        char wave[4] = {'W', 'A', 'V', 'E'};
        char fmt[4] = {'f', 'm', 't', ' '};
        quint32 fmtSize = 16;        // PCM格式大小
        quint16 audioFormat = 1;     // 1=PCM
        quint16 numChannels;
        quint32 sampleRate;
        quint32 byteRate;
        quint16 blockAlign;
        quint16 bitsPerSample;       // 这里用 bitsPerSample
        char data[4] = {'d', 'a', 't', 'a'};
        quint32 dataSize = 0;        // 音频数据大小
    };

    WavHeader header;
    header.numChannels = format.channelCount();
    header.sampleRate = format.sampleRate();

    // 根据 sampleFormat 计算 bitsPerSample
    switch (format.sampleFormat()) {
    case QAudioFormat::UInt8:   header.bitsPerSample = 8; break;
    case QAudioFormat::Int16:   header.bitsPerSample = 16; break;
    case QAudioFormat::Int32:   header.bitsPerSample = 32; break;
    case QAudioFormat::Float:   header.bitsPerSample = 32; break; // 浮点型也是32位
    default:                    header.bitsPerSample = 0; break;
    }

    header.byteRate = format.sampleRate() * format.channelCount() * (header.bitsPerSample / 8);
    header.blockAlign = format.channelCount() * (header.bitsPerSample / 8);

    m_audioFile->write(reinterpret_cast<const char*>(&header), sizeof(WavHeader));
}

void PianoKeyItem2::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    setBrush(pressColor);
    myBoard->input->getInput(key_number,0);
    myBoard->process();
}

void PianoKeyItem2::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    setBrush(normalColor);
    myBoard->input->getInput(key_number,1);
    myBoard->process();
}
//键盘按下
void KeyBoardItem::keyPressEvent(QKeyEvent *event)
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
void KeyBoardItem::keyReleaseEvent(QKeyEvent *event)
{
    int key=event->key();
    int note=getNoteFromKey(key);
    if(note!=-1 && (!event->isAutoRepeat()))
    {
        myBoard->input->getInput(note,1);
        myBoard->process();
    }
}
void Widget2::keyPressEvent(QKeyEvent *event) {
    myBoardItem->keyPressEvent(event);
}
void Widget2::keyReleaseEvent(QKeyEvent *event) {
    myBoardItem->keyReleaseEvent(event);
}

Widget2::Widget2(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->statusLabel->setVisible(false);
    connect(ui->back,&QPushButton::clicked,this,[this](){
        emit request2();
        this->close();
    });

    //创建键盘，输入输出
    std::shared_ptr<InputHandler> myMouse = std::make_shared<MouseInput>();
    std::shared_ptr<OutputHandler> mySynth = std::make_shared<SynthOutput>();
    myKeyboard = std::make_shared<VirtualKeyboard>(myMouse, mySynth);
    myBoardItem = std::make_shared<KeyBoardItem>(myKeyboard);

    // 设置钢琴键盘UI
    QGraphicsView *view = new QGraphicsView(this);
    QGraphicsScene *scene = new QGraphicsScene(this);
    view->setScene(scene);
    view->setFixedSize(902, 215);
    view->move(0, 290);

    // 背景图片
    QPixmap pianoImage(":/images/keyboard.png");
    QGraphicsPixmapItem *bg = new QGraphicsPixmapItem(pianoImage.scaled(view->size(), Qt::KeepAspectRatio));
    bg->setZValue(0);
    scene->addItem(bg);

    //左侧操作区背景
    QFrame *blackRect = new QFrame(this);
    blackRect->setStyleSheet("background-color: #404040;");
    blackRect->setFixedSize(120, 291);

    //Transpose
    QLabel *transposeLabel= new QLabel("TRANSPOSE",this);
    transposeLabel->setAlignment(Qt::AlignCenter);
    QFont transposeFont=transposeLabel->font();
    transposeFont.setPointSize(10);
    transposeLabel->setFont(transposeFont);
    transposeLabel->setStyleSheet("color: White;");
    transposeLabel->move(22,7);

    //octave transpose
    QFrame *grayRect = new QFrame(this);
    grayRect->setStyleSheet("background-color: gray; border-radius: 10px;");
    grayRect->setFixedSize(60, 20);
    grayRect->move(30,  30);
    QLabel *octaveLabel= new QLabel("oct",this);
    octaveLabel->setAlignment(Qt::AlignCenter); // 文本居中
    QFont octaveFont= octaveLabel->font();
    octaveFont.setPointSize(10);
    octaveFont.setWeight(QFont::Light);
    octaveLabel->setFont(octaveFont);
    octaveLabel->setStyleSheet("color: white;");
    octaveLabel->move(50,30);

    QPushButton *octaveMinusButton = new QPushButton(grayRect);
    octaveMinusButton->setParent(this);
    octaveMinusButton->setFixedSize(20, 20); // 设置按钮大小
    octaveMinusButton->move(30, 30); // 设置按钮位置
    octaveMinusButton->setStyleSheet("QPushButton {"
                                     "background-color: none; /* 按钮背景颜色 */"
                                     "color: white; /* 文字颜色 */"
                                     "border: none; /* 无边框 */"
                                     "padding-top: -5px; /* 文本向上调整5像素 */"
                                     "}"
                                     "QPushButton:pressed {"
                                     "background-color: #202020; /* 鼠标按下时的背景颜色 */"
                                     "}");
    QFont octaveMinusFont = octaveMinusButton->font();
    octaveMinusFont.setPointSize(20);
    octaveMinusButton->setFont(octaveMinusFont);
    octaveMinusButton->setText(" - "); // 设置按钮文字为减号
    connect(octaveMinusButton, &QPushButton::clicked, this, [=]() {
        myKeyboard->bOctave();
    });
    QPushButton *octavePlusButton = new QPushButton(grayRect);
    octavePlusButton->setParent(this);
    octavePlusButton->setFixedSize(20, 20); // 设置按钮大小
    octavePlusButton->move(70, 30); // 设置按钮位置
    octavePlusButton->setStyleSheet("QPushButton {"
                                    "background-color: none; /* 按钮背景颜色 */"
                                    "color: white; /* 文字颜色 */"
                                    "border: none; /* 无边框 */"
                                    "padding-top: -3px; /* 文本向上调整5像素 */"
                                    "}"
                                    "QPushButton:pressed {"
                                    "background-color: #202020; /* 鼠标按下时的背景颜色 */"
                                    "}");
    QFont octavePlusFont = octavePlusButton->font();
    octavePlusFont.setPointSize(18);
    octavePlusButton->setFont(octavePlusFont);
    octavePlusButton->setText(" + "); // 设置按钮文字为减号
    connect(octavePlusButton, &QPushButton::clicked, this, [=]() {
        myKeyboard->aOctave();
    });

    //semitone transpose
    QFrame *grayRect2 = new QFrame(this);
    grayRect2->setStyleSheet("background-color: gray; border-radius: 10px;");
    grayRect2->setFixedSize(60, 20);
    grayRect2->move(30, 55);
    QLabel *semiLabel= new QLabel("semi",this);
    semiLabel->setAlignment(Qt::AlignCenter); // 文本居中
    QFont semiFont= semiLabel->font();
    semiFont.setPointSize(10);
    semiFont.setWeight(QFont::Light);
    semiLabel->setFont(octaveFont);
    semiLabel->setStyleSheet("color: white;");
    semiLabel->move(46,55);
    QPushButton *semiMinusButton = new QPushButton(grayRect2);
    semiMinusButton->setParent(this);
    semiMinusButton->setFixedSize(20, 20); // 设置按钮大小
    semiMinusButton->move(30, 55); // 设置按钮位置
    semiMinusButton->setStyleSheet("QPushButton {"
                                   "background-color: none; /* 按钮背景颜色 */"
                                   "color: white; /* 文字颜色 */"
                                   "border: none; /* 无边框 */"
                                   "padding-top: -5px; /* 文本向上调整5像素 */"
                                   "}"
                                   "QPushButton:pressed {"
                                   "background-color: #202020; /* 鼠标按下时的背景颜色 */"
                                   "}");
    QFont semiMinusFont = semiMinusButton->font();
    semiMinusFont.setPointSize(20);
    semiMinusButton->setFont(octaveMinusFont);
    semiMinusButton->setText(" - "); // 设置按钮文字为减号
    connect(semiMinusButton, &QPushButton::clicked, this, [=]() {
        myKeyboard->bSemitone();
    });
    QPushButton *semiPlusButton = new QPushButton(grayRect2);
    semiPlusButton->setParent(this);
    semiPlusButton->setFixedSize(20, 20); // 设置按钮大小
    semiPlusButton->move(70, 55); // 设置按钮位置
    semiPlusButton->setStyleSheet("QPushButton {"
                                  "background-color: none; /* 按钮背景颜色 */"
                                  "color: white; /* 文字颜色 */"
                                  "border: none; /* 无边框 */"
                                  "padding-top: -3px; /* 文本向上调整5像素 */"
                                  "}"
                                  "QPushButton:pressed {"
                                  "background-color: #202020; /* 鼠标按下时的背景颜色 */"
                                  "}");
    QFont semiPlusFont = semiPlusButton->font();
    semiPlusFont.setPointSize(18);
    semiPlusButton->setFont(semiPlusFont);
    semiPlusButton->setText(" + "); // 设置按钮文字为减号
    connect(semiPlusButton, &QPushButton::clicked, this, [=]() {
        myKeyboard->aSemitone();
    });

    //Sustain Pedal
    QLabel *sustainLabel = new QLabel("SUSTAIN",this);
    sustainLabel->setAlignment(Qt::AlignCenter);
    QFont sustainFont=sustainLabel->font();
    sustainFont.setPointSize(10);
    sustainLabel->setFont(transposeFont);
    sustainLabel->setStyleSheet("color: White;");
    sustainLabel->move(32,82);
    QPushButton *sustainButton = new QPushButton(this);
    sustainButton->setParent(this);
    sustainButton->setFixedSize(60,20);
    sustainButton->move(30,100);
    sustainButton->setStyleSheet("QPushButton {"
                                 "border: 1px solid white;"
                                 "border-radius: 10px;"
                                 "background-color: gray;"
                                 "}"
                                 "QPushButton:hover {"
                                 "background-color: #606060;"
                                 "}"
                                 "QPushButton:pressed {"
                                 "background-color: #202020;"
                                 "}");
    connect(sustainButton, &QPushButton::clicked, this, [=]() {
        static bool isGreen = false; // 用于切换状态
        myKeyboard->changeSustain();
        if (isGreen) {
            // 如果当前是绿色，切换回白色
            sustainButton->setStyleSheet("QPushButton {"
                                         "border: 1px solid white;"
                                         "border-radius: 10px;"
                                         "background-color: gray;"
                                         "}"
                                         "QPushButton:hover {"
                                         "background-color: #606060;"
                                         "}"
                                         "QPushButton:pressed {"
                                         "background-color: #202020;"
                                         "}");
        } else {
            // 如果当前是白色，切换到绿色
            sustainButton->setStyleSheet("QPushButton {"
                                         "border: 2px solid green;"
                                         "border-radius: 10px;"
                                         "background-color: Lightgray;"
                                         "}"
                                         "QPushButton:hover {"
                                         "background-color: #606060;"
                                         "}");
        }
        // 切换状态
        isGreen = !isGreen;
    });

    //Reverb
    QLabel *reverbLabel = new QLabel("REVERB",this);
    reverbLabel->setAlignment(Qt::AlignCenter);
    QFont reverbFont=reverbLabel->font();
    reverbFont.setPointSize(10);
    reverbLabel->setFont(reverbFont);
    reverbLabel->setStyleSheet("color: White;");
    reverbLabel->move(35,124);

    QSlider *revlSlider = new QSlider(Qt::Horizontal,this);
    revlSlider->setRange(0, 100);
    revlSlider->setValue(0);
    revlSlider->move(20,150);
    QLabel *revlLabel=new QLabel("level: 0      ",this);
    revlLabel->setAlignment(Qt::AlignLeft);
    QFont revlFont=revlLabel->font();
    revlFont.setPointSize(6);
    revlLabel->setFont(revlFont);
    revlLabel->setStyleSheet("color: White;");
    revlLabel->move(10,142);
    connect(revlSlider, &QSlider::valueChanged, this, [=](double value) {
        revlLabel->setText(QString("level: %1      ").arg(value));
        myKeyboard->setReverbLevel(value/100.0);
    });

    QSlider *revrSlider = new QSlider(Qt::Horizontal,this);
    revrSlider->setRange(0, 100);
    revrSlider->setValue(50);
    revrSlider->move(20,170);
    QLabel *revrLabel=new QLabel("roomsize: 50     ",this);
    revrLabel->setAlignment(Qt::AlignLeft);
    revrLabel->setFont(revlFont);
    revrLabel->setStyleSheet("color: White;");
    revrLabel->move(10,162);
    connect(revrSlider, &QSlider::valueChanged, this, [=](double value) {
        revrLabel->setText(QString("roomsize: %1    ").arg(value));
        myKeyboard->setReverbRoom(value/100.0);
    });

    //Chorus
    QLabel *chorusLabel = new QLabel("CHORUS",this);
    chorusLabel->setAlignment(Qt::AlignCenter);
    chorusLabel->setFont(reverbFont);
    chorusLabel->setStyleSheet("color: White;");
    chorusLabel->move(30,185);
    QSlider *crsnSlider = new QSlider(Qt::Horizontal,this);
    crsnSlider->setRange(0, 60);
    crsnSlider->setValue(0);
    crsnSlider->move(20,210);
    QLabel *crsnLabel=new QLabel("count: 0     ",this);
    crsnLabel->setAlignment(Qt::AlignLeft);
    crsnLabel->setFont(revlFont);
    crsnLabel->setStyleSheet("color: White;");
    crsnLabel->move(10,202);
    connect(crsnSlider, &QSlider::valueChanged, this, [=](double value) {
        crsnLabel->setText(QString("count: %1    ").arg(value));
        myKeyboard->setChorusNum(value);
    });
    QSlider *crslSlider = new QSlider(Qt::Horizontal,this);
    crslSlider->setRange(0, 100);
    crslSlider->setValue(0);
    crslSlider->move(20,230);
    QLabel *crslLabel=new QLabel("level: 0     ",this);
    crslLabel->setAlignment(Qt::AlignLeft);
    crslLabel->setFont(revlFont);
    crslLabel->setStyleSheet("color: White;");
    crslLabel->move(10,222);
    connect(crslSlider, &QSlider::valueChanged, this, [=](double value) {
        crslLabel->setText(QString("level: %1    ").arg(value));
        myKeyboard->setChorusLevel(value/10.0);
    });
    QSlider *crssSlider = new QSlider(Qt::Horizontal,this);
    crssSlider->setRange(0, 100);
    crssSlider->setValue(0);
    crssSlider->move(20,250);
    QLabel *crssLabel=new QLabel("speed: 0     ",this);
    crssLabel->setAlignment(Qt::AlignLeft);
    crssLabel->setFont(revlFont);
    crssLabel->setStyleSheet("color: White;");
    crssLabel->move(10,242);
    connect(crssSlider, &QSlider::valueChanged, this, [=](double value) {
        if(value<2)
            value=2;
        crssLabel->setText(QString("speed: %1    ").arg(value));
        myKeyboard->setChorusSpeed(value/20.0);
    });
    QSlider *crsdSlider = new QSlider(Qt::Horizontal,this);
    crsdSlider->setRange(0, 100);
    crsdSlider->setValue(0);
    crsdSlider->move(20,270);
    QLabel *crsdLabel=new QLabel("depth: 0     ",this);
    crsdLabel->setAlignment(Qt::AlignLeft);
    crsdLabel->setFont(revlFont);
    crsdLabel->setStyleSheet("color: White;");
    crsdLabel->move(10,262);
    connect(crsdSlider, &QSlider::valueChanged, this, [=](double value) {
        crsdLabel->setText(QString("depth: %1    ").arg(value));
        myKeyboard->setChorusSpeed(value/5.0);
    });

    //右侧操作区背景
    QFrame *blackRect3 = new QFrame(this);
    blackRect3->setStyleSheet("background-color: #404040;");
    blackRect3->setFixedSize(161, 291);
    blackRect3->move(839,0);
    //Dual
    QLabel *dualLabel= new QLabel("DUAL",this);
    dualLabel->setAlignment(Qt::AlignCenter);
    dualLabel->setFont(transposeFont);
    dualLabel->setStyleSheet("color: White;");
    dualLabel->move(900,7);
    QLabel *upperLabel= new QLabel("Upper",this);
    upperLabel->setAlignment(Qt::AlignCenter);
    QFont upperFont = upperLabel->font();
    upperFont.setPointSize(7);
    upperLabel->setFont(upperFont);
    upperLabel->setStyleSheet("color: White;");
    upperLabel->move(842,30);
    QComboBox *uppBox = new QComboBox(this);
    for (const auto &instrument : VirtualKeyboard::instruments) {
        uppBox->addItem(instrument.name, QVariant::fromValue(instrument));
    }
    uppBox->move(870,25);
    uppBox->setFixedSize(130, 25);
    uppBox->setFont(upperFont);
    uppBox->setCurrentIndex(1);
    QObject::connect(uppBox, &QComboBox::currentIndexChanged, uppBox, [=]() {
        int index = uppBox->currentIndex();
        Instrument selectedInstrument = uppBox->itemData(index).value<Instrument>();
        myKeyboard->setTimbre1(selectedInstrument.bank,selectedInstrument.program);
    });
    QSlider *vol1Slider = new QSlider(Qt::Horizontal,this);
    vol1Slider->setRange(0, 127);
    vol1Slider->setValue(127);
    vol1Slider->move(860,48);
    vol1Slider->setFixedSize(120,30);
    QLabel *vol1Label=new QLabel("volume: 127     ",this);
    vol1Label->setAlignment(Qt::AlignLeft);
    vol1Label->setFont(revlFont);
    vol1Label->setStyleSheet("color: White;");
    vol1Label->move(855,49);
    connect(vol1Slider, &QSlider::valueChanged, this, [=](double value) {
        vol1Label->setText(QString("volume: %1    ").arg(value));
        myKeyboard->setVolume1(value);
    });
    QLabel *lowerLabel= new QLabel("Lower",this);
    lowerLabel->setAlignment(Qt::AlignCenter);
    lowerLabel->setFont(upperFont);
    lowerLabel->setStyleSheet("color: White;");
    lowerLabel->move(842,93);
    QComboBox *lowBox = new QComboBox(this);
    for (const auto &instrument : VirtualKeyboard::instruments) {
        lowBox->addItem(instrument.name, QVariant::fromValue(instrument));
    }
    lowBox->move(870,88);
    lowBox->setFixedSize(130, 25);
    lowBox->setFont(upperFont);
    lowBox->setCurrentIndex(0);
    QObject::connect(lowBox, &QComboBox::currentIndexChanged, lowBox, [=]() {
        int index = lowBox->currentIndex();
        Instrument selectedInstrument = lowBox->itemData(index).value<Instrument>();
        myKeyboard->setTimbre2(selectedInstrument.bank,selectedInstrument.program);
    });
    QSlider *vol2Slider = new QSlider(Qt::Horizontal,this);
    vol2Slider->setRange(0, 127);
    vol2Slider->setValue(127);
    vol2Slider->move(860,111);
    vol2Slider->setFixedSize(120,30);
    QLabel *vol2Label=new QLabel("volume: 127     ",this);
    vol2Label->setAlignment(Qt::AlignLeft);
    vol2Label->setFont(revlFont);
    vol2Label->setStyleSheet("color: White;");
    vol2Label->move(855,112);
    connect(vol2Slider, &QSlider::valueChanged, this, [=](double value) {
        vol2Label->setText(QString("volume: %1    ").arg(value));
        myKeyboard->setVolume2(value);
    });
    QLabel *spLabel = new QLabel("C4",this);
    spLabel->setAlignment(Qt::AlignLeft);
    spLabel->setFont(upperFont);
    spLabel->setStyleSheet("color: White;");
    spLabel->move(880,74);
    spLabel->setVisible(false);
    QSlider *splitSlider = new QSlider(Qt::Horizontal,this);
    splitSlider->setRange(2,126);
    splitSlider->setValue(60);
    splitSlider->move(900,72);
    splitSlider->setVisible(false);
    connect(splitSlider, &QSlider::valueChanged, this, [=](int value) {
        spLabel->setText(VirtualKeyboard::noteNames[value]);
        myKeyboard->set_split(value);
    });
    QLabel *splitLabel = new QLabel("Split",this);
    splitLabel->setAlignment(Qt::AlignCenter);
    splitLabel->setFont(upperFont);
    splitLabel->setStyleSheet("color: White;");
    splitLabel->move(842,73);
    QPushButton *splitButton = new QPushButton(this);
    splitButton->setFixedSize(14,14);
    splitButton->move(865,72);
    splitButton->setStyleSheet(
        "QPushButton {"
        "border: 1px solid white;"
        "border-radius: 7px;"
        "background-color: gray;"
        "}"
        "QPushButton:hover {"
        "background-color: #606060;"
        "}"
        "QPushButton:pressed {"
        "background-color: #202020;"
        "}"
        );
    connect(splitButton, &QPushButton::clicked, this, [=]() {
        static bool isGreen = false; // 用于切换状态
        myKeyboard->change_split();
        if (isGreen) {
            // 如果当前是绿色，切换回白色
            splitButton->setStyleSheet("QPushButton {"
                                       "border: 1px solid white;"
                                       "border-radius: 7px;"
                                       "background-color: gray;"
                                       "}"
                                       "QPushButton:hover {"
                                       "background-color: #606060;"
                                       "}"
                                       "QPushButton:pressed {"
                                       "background-color: #202020;"
                                       "}");
            spLabel->setVisible(false);
            splitSlider->setVisible(false);
        } else {
            // 如果当前是白色，切换到绿色
            splitButton->setStyleSheet("QPushButton {"
                                       "border: 2px solid green;"
                                       "border-radius: 7px;"
                                       "background-color: LightGray;"
                                       "}"
                                       "QPushButton:hover {"
                                       "background-color: #606060;"
                                       "}");
            spLabel->setVisible(true);
            splitSlider->setVisible(true);
        }
        // 切换状态
        isGreen = !isGreen;
    });

    //Pitchbend
    QLabel *pbLabel= new QLabel("PITCH BEND",this);
    pbLabel->setAlignment(Qt::AlignCenter);
    pbLabel->setFont(transposeFont);
    pbLabel->setStyleSheet("color: White;");
    pbLabel->move(884,135);
    QSlider *pbSlider = new QSlider(Qt::Horizontal,this);
    pbSlider->setRange(-8191, 8190);
    pbSlider->setValue(0);
    pbSlider->move(852,142);
    pbSlider->setFixedSize(138,30);
    pbSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "  height: 5px;"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);"
        "  margin: 1px 0;"
        "}"
        "QSlider::handle:horizontal {"
        "  border: 1px solid #5c5c5c;"
        "  width: 14px;"
        "  height: 18px;"
        "  margin: -2px 0;"
        "  border-radius: 9px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "  background: lightgray;"
        "}"
        "QSlider::add-page:horizontal {"
        "  background: lightgray;"
        "}"
        );
    connect(pbSlider, &QSlider::valueChanged, this, [=](int value) {
        myKeyboard->setPitchbend(value);
    });
    connect(pbSlider, &QSlider::sliderReleased, this, [=]() {
        pbSlider->setValue(0);
        myKeyboard->setPitchbend(0);
    });

    //Modulation Wheel
    QLabel *mdLabel= new QLabel("MODULATION",this);
    mdLabel->setAlignment(Qt::AlignCenter);
    mdLabel->setFont(transposeFont);
    mdLabel->setStyleSheet("color: White;");
    mdLabel->move(876,167);
    QSlider *mdSlider = new QSlider(Qt::Horizontal,this);
    mdSlider->setRange(0, 127);
    mdSlider->setValue(0);
    mdSlider->move(852,176);
    mdSlider->setFixedSize(138,30);
    mdSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "  height: 6px;"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);"
        "  margin: 1px 0;"
        "}"
        "QSlider::handle:horizontal {"
        "  border: 1px solid #5c5c5c;"
        "  width: 14px;"
        "  height: 18px;"
        "  margin: -2px 0;"
        "  border-radius: 9px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "  background: lightgray;"
        "}"
        "QSlider::add-page:horizontal {"
        "  background: lightgray;"
        "}"
        );
    connect(mdSlider, &QSlider::valueChanged, this, [=](int value) {
        myKeyboard->setModulation(value);
    });
    connect(mdSlider, &QSlider::sliderReleased, this, [=]() {
        mdSlider->setValue(0);
        myKeyboard->setModulation(0);
    });

    //Filter
    QLabel *flLabel= new QLabel("FILTER",this);
    flLabel->setAlignment(Qt::AlignCenter);
    flLabel->setFont(transposeFont);
    flLabel->setStyleSheet("color: White;");
    flLabel->move(860,205);
    //resonance
    QLabel *rvLabel = new QLabel("0",this);
    rvLabel->setAlignment(Qt::AlignCenter);
    rvLabel->setFont(revlFont);
    rvLabel->setStyleSheet("color: White;");
    rvLabel->setFixedWidth(20);
    rvLabel->move(848,220);
    QLabel *rsLabel = new QLabel("resonance",this);
    rsLabel->setAlignment(Qt::AlignCenter);
    rsLabel->setFont(revlFont);
    rsLabel->setStyleSheet("color: White;");
    rsLabel->move(839,260);
    QDial *rsDial = new QDial(this);
    rsDial->setFixedSize(40,40);
    rsDial->move(838,225);
    rsDial->setRange(0,300);
    connect(rsDial, &QDial::valueChanged, this, [=](int value) {
        rvLabel->setText(QString("%1").arg(value));
        myKeyboard->setResonance(value);
    });
    //cutoff
    QLabel *cvLabel = new QLabel("2000",this);
    cvLabel->setAlignment(Qt::AlignCenter);
    cvLabel->setFont(revlFont);
    cvLabel->setStyleSheet("color: White;");
    cvLabel->setFixedWidth(20);
    cvLabel->move(894,220);
    QLabel *coLabel = new QLabel("cutoff",this);
    coLabel->setAlignment(Qt::AlignCenter);
    coLabel->setFont(revlFont);
    coLabel->setStyleSheet("color: White;");
    coLabel->move(892,260);
    QDial *coDial = new QDial(this);
    coDial->setFixedSize(40,40);
    coDial->move(882,225);
    coDial->setRange(20,8000);
    coDial->setSingleStep(10);
    coDial->setValue(2000);
    connect(coDial, &QDial::valueChanged, this, [=](int value) {
        cvLabel->setText(QString("%1").arg(value));
        myKeyboard->setCutoff(value);
    });

    //ENV
    QLabel *egLabel= new QLabel("ENV",this);
    egLabel->setAlignment(Qt::AlignCenter);
    egLabel->setFont(transposeFont);
    egLabel->setStyleSheet("color: White;");
    egLabel->move(940,205);
    //Attack
    QLabel *avLabel = new QLabel("0",this);
    avLabel->setAlignment(Qt::AlignCenter);
    avLabel->setFont(revlFont);
    avLabel->setStyleSheet("color: White;");
    avLabel->setFixedWidth(20);
    avLabel->move(918,220);
    QLabel *atLabel = new QLabel("A",this);
    atLabel->setAlignment(Qt::AlignCenter);
    atLabel->setFont(revlFont);
    atLabel->setStyleSheet("color: White;");
    atLabel->move(926,280);
    QSlider *atSlider = new QSlider(Qt::Vertical, this);
    atSlider->setRange(0, 1000);
    atSlider->setValue(0);
    atSlider->setFixedHeight(53);
    atSlider->setSingleStep(10);
    atSlider->move(921,231);
    connect(atSlider, &QSlider::valueChanged, this, [=](int value) {
        avLabel->setText(QString("%1").arg(value));
        myKeyboard->setAttack(value);
    });
    //Decay
    QLabel *dvLabel = new QLabel("0",this);
    dvLabel->setAlignment(Qt::AlignCenter);
    dvLabel->setFont(revlFont);
    dvLabel->setStyleSheet("color: White;");
    dvLabel->setFixedWidth(20);
    dvLabel->move(937,220);
    QLabel *dcLabel = new QLabel("D",this);
    dcLabel->setAlignment(Qt::AlignCenter);
    dcLabel->setFont(revlFont);
    dcLabel->setStyleSheet("color: White;");
    dcLabel->move(945,280);
    QSlider *dcSlider = new QSlider(Qt::Vertical, this);
    dcSlider->setRange(0, 1000);
    dcSlider->setValue(0);
    dcSlider->setFixedHeight(53);
    dcSlider->setSingleStep(10);
    dcSlider->move(940,231);
    connect(dcSlider, &QSlider::valueChanged, this, [=](int value) {
        dvLabel->setText(QString("%1").arg(value));
        myKeyboard->setDecay(value);
    });
    //Sustain
    QLabel *svLabel = new QLabel("0",this);
    svLabel->setAlignment(Qt::AlignCenter);
    svLabel->setFont(revlFont);
    svLabel->setStyleSheet("color: White;");
    svLabel->setFixedWidth(20);
    svLabel->move(956,220);
    QLabel *ssLabel = new QLabel("S",this);
    ssLabel->setAlignment(Qt::AlignCenter);
    ssLabel->setFont(revlFont);
    ssLabel->setStyleSheet("color: White;");
    ssLabel->move(964,280);
    QSlider *ssSlider = new QSlider(Qt::Vertical, this);
    ssSlider->setRange(0, 1000);
    ssSlider->setValue(0);
    ssSlider->setFixedHeight(53);
    ssSlider->setSingleStep(10);
    ssSlider->move(959,231);
    connect(ssSlider, &QSlider::valueChanged, this, [=](int value) {
        svLabel->setText(QString("%1").arg(value));
        myKeyboard->setSustain(value);
    });
    //Release
    QLabel *rlvLabel = new QLabel("0",this);
    rlvLabel->setAlignment(Qt::AlignCenter);
    rlvLabel->setFont(revlFont);
    rlvLabel->setStyleSheet("color: White;");
    rlvLabel->setFixedWidth(20);
    rlvLabel->move(975,220);
    QLabel *rlLabel = new QLabel("R",this);
    rlLabel->setAlignment(Qt::AlignCenter);
    rlLabel->setFont(revlFont);
    rlLabel->setStyleSheet("color: White;");
    rlLabel->move(983,280);
    QSlider *rlSlider = new QSlider(Qt::Vertical, this);
    rlSlider->setRange(0, 1000);
    rlSlider->setValue(0);
    rlSlider->setFixedHeight(53);
    rlSlider->setSingleStep(10);
    rlSlider->move(978,231);
    connect(rlSlider, &QSlider::valueChanged, this, [=](int value) {
        rlvLabel->setText(QString("%1").arg(value));
        myKeyboard->setRelease(value);
    });


    // 白键
    const QList<int> whiteKeyX = {0, 43, 86, 129, 172, 215, 258, 301, 344, 387,
                                  430, 473, 516, 559, 602, 645, 688, 731, 774, 817, 860};
    QList<int> whiteKeyN =
        {
            36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71
        };
    for(int i = 0;i < 21;i ++)
    {
        int x=whiteKeyX[i];
        int n=whiteKeyN[i];
        PianoKeyItem2 *key = new PianoKeyItem2(QRectF(x,0,40,206),true, myKeyboard);
        key->key_number=n;
        scene->addItem(key);
    }

    // 黑键
    const QList<int> blackKeyX = {28, 73, 157, 200, 245, 329, 372, 458, 501,
                                  544, 628, 673, 757, 801, 844};
    QList<int> blackKeyN=
        {
            37, 39, 42, 44, 46, 49, 51, 54, 56, 58, 61, 63, 66, 68, 70
        };

    for (int i = 0;i < 15;i ++) {
        int x=blackKeyX[i];
        int n=blackKeyN[i];
        PianoKeyItem2 *blackKey = new PianoKeyItem2(QRectF(x, 0, 24, 120),false, myKeyboard);
        blackKey->key_number=n;
        scene->addItem(blackKey);
    }

    scene->setSceneRect(pianoImage.rect());

    // 初始化成员指针
    m_audioInput = nullptr;
    m_audioFile = nullptr;
    //背景
    QPixmap b1(":/images/b1.jpg");
    QPixmap b2(":/images/b2.jpg");
    QPixmap b3(":/images/b3.jpg");
    QPixmap b4(":/images/b4.jpg");
    if (!b1.isNull()) {
        QPalette pal = palette();
        pal.setBrush(QPalette::Window, b1.scaled(size(), Qt::KeepAspectRatioByExpanding));
        setPalette(pal);
    } else {
        qWarning() << "图片加载失败！路径:" << "image/keyboard_image.png";
    }//默认背景
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

Widget2::~Widget2()
{
    stopRecording();
    delete ui;
}

void Widget2::on_startrecord_clicked()
{
    ui->statusLabel->setVisible(true);
    ui->statusLabel->setText("录音中");

    m_audioFile = new QFile("recording.wav", this);
    if (!m_audioFile->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "无法打开音频文件：" << m_audioFile->errorString();
        return;
    }

    QAudioFormat format;
    format.setSampleRate(44100);        // CD标准采样率
    format.setChannelCount(2);          // 立体声
    format.setSampleFormat(QAudioFormat::Int16); // 16位整数（兼容性好）

    const QAudioDevice inputDevice = QMediaDevices::defaultAudioInput();

    qDebug() << "请求的格式："
             << "采样率：" << format.sampleRate()
             << "通道数：" << format.channelCount()
             << "采样格式：" << format.sampleFormat();

    if (!inputDevice.isNull()) {
        if (!inputDevice.isFormatSupported(format)) {
            qWarning() << "不支持的格式，使用设备推荐格式";
            format = inputDevice.preferredFormat();
            qDebug() << "实际使用格式："
                     << "采样率：" << format.sampleRate()
                     << "通道数：" << format.channelCount()
                     << "采样格式：" << format.sampleFormat();
        }

        writeWavHeader(format);  // 写入WAV头

        m_audioInput = new QAudioSource(inputDevice, format, this);
        m_audioInput->start(m_audioFile);
        qDebug() << "开始录音";
    } else {
        qWarning() << "未找到音频输入设备";
    }
}

void Widget2::on_stoprecord_clicked()
{
    stopRecording();
    ui->statusLabel->setVisible(false);
    qDebug() << "结束录音";
}

void Widget2::stopRecording()
{
    if (m_audioInput) {
        m_audioInput->stop();
        delete m_audioInput;
        m_audioInput = nullptr;
    }

    if (m_audioFile) {
        m_audioFile->close();
        delete m_audioFile;
        m_audioFile = nullptr;
    }
}


void Widget2::on_play_clicked(){
    return;
}
