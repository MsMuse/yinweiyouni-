#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->volumeControlPanel->setVisible(false);  // 初始隐藏

    connect(ui->listWidget, &QListWidget::itemClicked,
            this, &SettingsDialog::handleItemClicked);
    connect(ui->volumeSlider, &QSlider::valueChanged,
            this, &SettingsDialog::onVolumeChanged);
    connect(ui->pushButton, &QPushButton::clicked,
            this, &SettingsDialog::toggleMute);
}

void SettingsDialog::handleItemClicked(QListWidgetItem *item)
{
    if (item->text() == "音量控制") {
        ui->volumeControlPanel->setVisible(true);
        ui->volumeSlider->setValue(m_volume.GetCurrentVolume());
    }
}
SettingsDialog::~SettingsDialog()
{
    delete ui;  // 必须释放UI对象
}
void SettingsDialog::onVolumeChanged(int value)
{
    m_volume.SetSystemVolume(value);
    m_isMuted = false;
    ui->pushButton->setText("静音");
}

void SettingsDialog::toggleMute()
{
    m_isMuted = !m_isMuted;
    m_volume.SetSystemVolume(m_isMuted ? -2 : -1);
    ui->pushButton->setText(m_isMuted ? "取消静音" : "静音");
    if (!m_isMuted) {
        ui->volumeSlider->setValue(m_volume.GetCurrentVolume());
    }
}
