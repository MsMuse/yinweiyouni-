#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include "volume.h"  // 包含音量控制类


namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();


private slots:
    void handleItemClicked(QListWidgetItem *item);
    void onVolumeChanged(int value);  // 音量滑块变化
    void toggleMute();               // 静音切换

private:
    Ui::SettingsDialog *ui;
    Volume m_volume;                 // 音量控制对象
    bool m_isMuted = false;          // 静音状态
};

#endif // SETTINGSDIALOG_H
