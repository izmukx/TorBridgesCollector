#include "getbridgeswindow.h"
#include "ui_getbridgeswindow.h"

#include "showqrcode.h"

#include <QtWidgets>
#include <QDialog>
#include <QMainWindow>

GetBridgesWindow::GetBridgesWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GetBridgesWindow)
{
    ui->setupUi(this);
}

void GetBridgesWindow::showRetrievedBridges(QString bridges, QPixmap qrcode, QString message)
{
    show();
    ui->bridgesText->setPlainText(bridges);
    ui->retrievalMessage->setText(message);
    BridgesQRCode = qrcode;
}

void GetBridgesWindow::on_showQRCode_clicked()
{
    ShowQRCode *QRCodeWindow = new ShowQRCode(this);
    QRCodeWindow->setQRCode(BridgesQRCode);
    QRCodeWindow->show();
}

void GetBridgesWindow::on_copyToClipboard_clicked()
{
    ui->bridgesText->selectAll();
    ui->bridgesText->copy();
}

GetBridgesWindow::~GetBridgesWindow()
{
    delete ui;
}
