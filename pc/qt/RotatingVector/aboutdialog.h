#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

protected:
    void paintEvent(QPaintEvent *pe);

private:
    Ui::AboutDialog *ui;
    QImage *aboutImage = nullptr;

    QImage* locateAndInstantiateImage(QString filename);
};

#endif // ABOUTDIALOG_H
