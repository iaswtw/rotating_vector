#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <QDir>
#include <QPainter>

static int MAJOR = 0;
static int MINOR = 5;
static int REVISION = 0;
//static int BUILD = 0;

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    setWindowTitle("About Rotating Vector");

    QString versionString;
    versionString.sprintf("%d.%d.%d", MAJOR, MINOR, REVISION);
    ui->nameAndVersion_label->setStyleSheet("font-weight: bold; color: black; font-size: 20px;");
    ui->nameAndVersion_label->setText(QString("Rotating Vector ") + versionString);

    ui->mainTextEdit->setText("This program simulates the generation of sine and cosine functions from the projections of a rotating vector.\n\n"
                              "It can be used as a teaching tool for anyone interested to understand sine and cosine in a more fun, intuituive and satisfying way.");

    aboutImage = locateAndInstantiateImage("about_icon.png");

    setFixedSize(600, 400);
}

void AboutDialog::paintEvent(QPaintEvent *pe)
{
    QWidget::paintEvent(pe);

    QPainter p(this);

    QRect target;
    QRect source;
    QImage image;

    //------------------------------------------------------------
    // Load and draw Alice
    if (aboutImage)
    {
        target = QRect(5,
                       5,
                       205,
                       216);
        source = QRect(0,
                       0,
                       205,
                       216);
        p.drawImage(target, *aboutImage, source);
    }

}



QImage* AboutDialog::locateAndInstantiateImage(QString filename)
{
    QImage *image = nullptr;
    QString curDir = QDir::currentPath();
    QFile file(filename);

    if (file.exists())
    {
        image = new QImage(file.fileName());
    }
    else
    {
        file.setFileName(curDir + "\\..\\RotatingVector\\" + filename);
        if (file.exists())
        {
            image = new QImage(file.fileName());
        }
    }
    return image;
}


AboutDialog::~AboutDialog()
{
    delete ui;
}
