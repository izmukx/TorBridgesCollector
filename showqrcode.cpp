#include "showqrcode.h"
#include "ui_showqrcode.h"

ShowQRCode::ShowQRCode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowQRCode)
{
    ui->setupUi(this);
}

void ShowQRCode::setQRCode(QPixmap qrcode)
{
    ui->QRCode->setPixmap(qrcode);
}

ShowQRCode::~ShowQRCode()
{
    delete ui;
}
