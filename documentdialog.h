#ifndef DOCUMENTDIALOG_H
#define DOCUMENTDIALOG_H

#include <QDialog>

class QPushButton;
class QVBoxLayout;

class DocumentDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DocumentDialog(QWidget *parent = nullptr);
    void openWordDocument();

private:
    QPushButton *btnClose;
    QVBoxLayout *layout;
};

#endif // DOCUMENTDIALOG_H
