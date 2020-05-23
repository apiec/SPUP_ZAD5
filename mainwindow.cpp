#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <array>
#include <algorithm>

using uchar = unsigned char;

constexpr std::array<uchar, 4> ZERO_ARRAY = {0, 0, 0, 0};

std::array<uchar, 4> floatToOwn(std::array<uchar, 4> floatNum)
{
    if (floatNum == ZERO_ARRAY) return ZERO_ARRAY;
    uchar exp = floatNum[0] << 1 | floatNum[1] >> 7;
    exp += 1;
    uchar byte1 = (floatNum[0] & 0b10000000) | (floatNum[1] & 0b01111111);
    return std::array<uchar, 4>{exp, byte1, floatNum[2], floatNum[3]};
}

std::array<uchar, 4> ownToFloat(std::array<uchar, 4> ownNum)
{
    if (ownNum == ZERO_ARRAY) return ZERO_ARRAY;
    uchar exp = ownNum[0] - 1;
    uchar byte0 = (ownNum[1] & 0b10000000) | (exp >> 1);
    uchar byte1 = exp << 7 | (ownNum[1] & 0b01111111);
    return std::array<uchar, 4>{byte0, byte1, ownNum[2], ownNum[3]};
}

float arrayToFloat(std::array<uchar, 4> floatNum)
{
    std::reverse(floatNum.begin(), floatNum.end());
    return *reinterpret_cast<float*>(&floatNum.front());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(360, 128);
    static const QString binaryMask = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB;_";
    static const QString hexMask = "HHHHHHHH;_";
    ui->ownEdit->setInputMask(binaryMask);
    ui->floatEdit->setInputMask(binaryMask);
    connect(ui->binHexBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [=](int index) {
            switch (index) {
                case 0:
                {
                    auto ownText   = ui->ownEdit->text();
                    auto floatText = ui->floatEdit->text();
                    ui->ownEdit  ->setInputMask(binaryMask);
                    ui->floatEdit->setInputMask(binaryMask);
                    bool ok;
                    ui->ownEdit  ->setText(QString("%1").arg(ownText  .toUInt(&ok, 16), 32, 2, QChar('0')));
                    ui->floatEdit->setText(QString("%1").arg(floatText.toUInt(&ok, 16), 32, 2, QChar('0')));
                }
                break;
                case 1:
                {
                    auto ownText   = ui->ownEdit->text();
                    auto floatText = ui->floatEdit->text();
                    ui->ownEdit  ->setInputMask(hexMask);
                    ui->floatEdit->setInputMask(hexMask);
                    bool ok;
                    ui->ownEdit  ->setText(QString("%1").arg(ownText  .toUInt(&ok, 2), 8, 16, QChar('0')));
                    ui->floatEdit->setText(QString("%1").arg(floatText.toUInt(&ok, 2), 8, 16, QChar('0')));
                }
                break;
            }
        });

    connect(ui->ownEdit, &QLineEdit::editingFinished,
        [=](){
            auto str = ui->ownEdit->text();
            bool isUsingBin = ui->binHexBox->currentIndex() == 0;
            if(isUsingBin)
            {
                bool ok;
                std::array<uchar, 4> input;
                input[0] = str.left(8).toUInt(&ok, 2);
                input[1] = str.left(16).right(8).toUInt(&ok, 2);
                input[2] = str.right(16).left(8).toUInt(&ok, 2);
                input[3] = str.right(8).toUInt(&ok, 2);
                auto output = ownToFloat(input);
                QString outStr;
                for(auto i : output)
                {
                    outStr.append(QString("%1").arg(i, 8, 2, QChar('0')));
                }
                ui->floatEdit->setText(outStr);
                ui->number->setText(QVariant{arrayToFloat(output)}.toString());
            }
            else //isUsingHex
            {
                bool ok;
                std::array<uchar, 4> input;
                input[0] = str.left(2).toUInt(&ok, 16);
                input[1] = str.left(4).right(2).toUInt(&ok, 16);
                input[2] = str.right(4).left(2).toUInt(&ok, 16);
                input[3] = str.right(2).toUInt(&ok, 16);
                auto output = ownToFloat(input);
                QString outStr;
                for(auto i : output)
                {
                    outStr.append(QString("%1").arg(i, 2, 16, QChar('0')));
                }
                ui->floatEdit->setText(outStr);
                ui->number->setText(QVariant{arrayToFloat(output)}.toString());
            }
        });
    connect(ui->floatEdit, &QLineEdit::editingFinished,
        [=](){
            auto str = ui->floatEdit->text();
            bool isUsingBin = ui->binHexBox->currentIndex() == 0;
            if(isUsingBin)
            {
                bool ok;
                std::array<uchar, 4> input;
                input[0] = str.left(8).toUInt(&ok, 2);
                input[1] = str.left(16).right(8).toUInt(&ok, 2);
                input[2] = str.right(16).left(8).toUInt(&ok, 2);
                input[3] = str.right(8).toUInt(&ok, 2);
                auto output = floatToOwn(input);
                QString outStr;
                for(auto i : output)
                {
                    outStr.append(QString("%1").arg(i, 8, 2, QChar('0')));
                }
                ui->ownEdit->setText(outStr);
                ui->number->setText(QVariant{arrayToFloat(input)}.toString());
            }
            else //isUsingHex
            {
                bool ok;
                std::array<uchar, 4> input;
                input[0] = str.left(2).toUInt(&ok, 16);
                input[1] = str.left(4).right(2).toUInt(&ok, 16);
                input[2] = str.right(4).left(2).toUInt(&ok, 16);
                input[3] = str.right(2).toUInt(&ok, 16);
                auto output = floatToOwn(input);
                QString outStr;
                for(auto i : output)
                {
                    outStr.append(QString("%1").arg(i, 2, 16, QChar('0')));
                }
                ui->ownEdit->setText(outStr);
                ui->number->setText(QVariant{arrayToFloat(input)}.toString());
            }
        });
}

MainWindow::~MainWindow()
{
    delete ui;
}

