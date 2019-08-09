#include "ShapeSelGroupBox.h"
#include "ui_ShapeSelGroupBox.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>

#include <QTextCodec>

QTextCodec *codec = QTextCodec::codecForLocale();

ShapeSelGroupBox::ShapeSelGroupBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShapeSelGroupBox)
    , m_type(-1)
{
    ui->setupUi(this);

    connectSignal(false);
    setSelectBoxItem(m_type);
}

ShapeSelGroupBox::~ShapeSelGroupBox()
{
    delete ui;
}

QList<double> ShapeSelGroupBox::getEditorData()
{
    QList<double> data;
    data << ui->centerXSBox->value() << ui->centerYSBox->value() << ui->widthSBox->value()
         << ui->heightSBox->value() << ui->slopeAngleSBox->value() << ui->startAngleSBox->value()
         << ui->endAngleSBox->value();
    return data;
}

void ShapeSelGroupBox::setShapeType(int type)
{
    m_type = type;
    setSelectBoxItem(m_type);
}

void ShapeSelGroupBox::setShapeEditor(const QList<double> &data)
{
    if(data.length() != 7){
        return;
    }
    connectSignal(true);
    ui->centerXSBox->setValue(data[0]);
    ui->centerYSBox->setValue(data[1]);
    ui->widthSBox->setValue(data[2]);
    ui->heightSBox->setValue(data[3]);
    ui->slopeAngleSBox->setValue(data[4]);
    ui->startAngleSBox->setValue(data[5]);
    ui->endAngleSBox->setValue(data[6]);
    connectSignal(false);
}

void ShapeSelGroupBox::on_clearShapeBut_clicked()
{
    m_type = -1;
    setSelectBoxItem(m_type);
    emit shapeTypeSwitched(m_type);
}

void ShapeSelGroupBox::on_rectBut_clicked()
{
    m_type = 0;
    setSelectBoxItem(m_type);
    emit shapeTypeSwitched(m_type);
}

void ShapeSelGroupBox::on_rotaryRectBut_clicked()
{
    m_type = 1;
    setSelectBoxItem(m_type);
    emit shapeTypeSwitched(m_type);
}

void ShapeSelGroupBox::on_circleBut_clicked()
{
    m_type = 2;
    setSelectBoxItem(m_type);
    emit shapeTypeSwitched(m_type);
}

void ShapeSelGroupBox::on_ellipseBut_clicked()
{
    m_type = 3;
    setSelectBoxItem(m_type);
    emit shapeTypeSwitched(m_type);
}

void ShapeSelGroupBox::on_sectorBut_clicked()
{
    m_type = 4;
    setSelectBoxItem(m_type);
    emit shapeTypeSwitched(m_type);
}

void ShapeSelGroupBox::editorValueChanged()
{
    QList<double> data;
    data << ui->centerXSBox->value() << ui->centerYSBox->value() << ui->widthSBox->value()
         << ui->heightSBox->value() << ui->slopeAngleSBox->value() << ui->startAngleSBox->value()
         << ui->endAngleSBox->value();
    emit shapeDataEdited(data);
}

void ShapeSelGroupBox::connectSignal(bool isDisconnect)
{
    if(isDisconnect){
        disconnect(ui->centerXSBox,SIGNAL(valueChanged(double)),this,SLOT(editorValueChanged()));
        disconnect(ui->centerYSBox,SIGNAL(valueChanged(double)),this,SLOT(editorValueChanged()));
        disconnect(ui->widthSBox,SIGNAL(valueChanged(double)),this,SLOT(editorValueChanged()));
        disconnect(ui->heightSBox,SIGNAL(valueChanged(double)),this,SLOT(editorValueChanged()));
        disconnect(ui->slopeAngleSBox,SIGNAL(valueChanged(double)),this,SLOT(editorValueChanged()));
        disconnect(ui->startAngleSBox,SIGNAL(valueChanged(double)),this,SLOT(editorValueChanged()));
        disconnect(ui->endAngleSBox,SIGNAL(valueChanged(double)),this,SLOT(editorValueChanged()));
    }else{
        connect(ui->centerXSBox,SIGNAL(valueChanged(double)),this,SLOT(editorValueChanged()));
        connect(ui->centerYSBox,SIGNAL(valueChanged(double)),this,SLOT(editorValueChanged()));
        connect(ui->widthSBox,SIGNAL(valueChanged(double)),this,SLOT(editorValueChanged()));
        connect(ui->heightSBox,SIGNAL(valueChanged(double)),this,SLOT(editorValueChanged()));
        connect(ui->slopeAngleSBox,SIGNAL(valueChanged(double)),this,SLOT(editorValueChanged()));
        connect(ui->startAngleSBox,SIGNAL(valueChanged(double)),this,SLOT(editorValueChanged()));
        connect(ui->endAngleSBox,SIGNAL(valueChanged(double)),this,SLOT(editorValueChanged()));
    }
}

void ShapeSelGroupBox::setSelectBoxItem(int type)
{
    if(type < 0){
        ui->centerXSBox->setEnabled(false);
        ui->centerYSBox->setEnabled(false);
        ui->widthSBox->setEnabled(false);
        ui->heightSBox->setEnabled(false);
        ui->slopeAngleSBox->setEnabled(false);
        ui->startAngleSBox->setEnabled(false);
        ui->endAngleSBox->setEnabled(false);
        return;
    }else{
        ui->centerXSBox->setEnabled(true);
        ui->centerYSBox->setEnabled(true);
        ui->widthSBox->setEnabled(true);
        ui->heightSBox->setEnabled(true);
        ui->slopeAngleSBox->setEnabled(true);
        ui->startAngleSBox->setEnabled(true);
        ui->endAngleSBox->setEnabled(true);
    }

    switch(type){
    case 0://矩形
        ui->centerXLabel->setText(codec->toUnicode("左上顶点.X:"));
        ui->centerYLabel->setText(codec->toUnicode("左上顶点.Y:"));
        ui->widthLabel->setText(codec->toUnicode("右下顶点.X:"));

        ui->heightLabel->setVisible(true);
        ui->heightSBox->setVisible(true);
        ui->heightLabel->setText(codec->toUnicode("右下顶点.Y:"));

        ui->slopeAngleLabel->setVisible(false);
        ui->slopeAngleSBox->setVisible(false);

        ui->startAngleLabel->setVisible(false);
        ui->startAngleSBox->setVisible(false);

        ui->endAngleLabel->setVisible(false);
        ui->endAngleSBox->setVisible(false);
        break;
    case 1://旋转矩形
        ui->centerXLabel->setText(codec->toUnicode("中心.X:"));
        ui->centerYLabel->setText(codec->toUnicode("中心.Y:"));
        ui->widthLabel->setText(codec->toUnicode("宽:"));

        ui->heightLabel->setVisible(true);
        ui->heightSBox->setVisible(true);
        ui->heightLabel->setText(codec->toUnicode("高:"));

        ui->slopeAngleLabel->setVisible(true);
        ui->slopeAngleSBox->setVisible(true);
        ui->slopeAngleLabel->setText(codec->toUnicode("倾斜角度:"));

        ui->startAngleLabel->setVisible(false);
        ui->startAngleSBox->setVisible(false);

        ui->endAngleLabel->setVisible(false);
        ui->endAngleSBox->setVisible(false);
        break;
    case 2://圆
        ui->centerXLabel->setText(codec->toUnicode("圆心.X:"));
        ui->centerYLabel->setText(codec->toUnicode("圆心.Y:"));
        ui->widthLabel->setText(codec->toUnicode("半径:"));

        ui->heightLabel->setVisible(false);
        ui->heightSBox->setVisible(false);

        ui->slopeAngleLabel->setVisible(false);
        ui->slopeAngleSBox->setVisible(false);

        ui->startAngleLabel->setVisible(false);
        ui->startAngleSBox->setVisible(false);

        ui->endAngleLabel->setVisible(false);
        ui->endAngleSBox->setVisible(false);
        break;
    case 3 ://椭圆
        ui->centerXLabel->setText(codec->toUnicode("圆心.X:"));
        ui->centerYLabel->setText(codec->toUnicode("圆心.Y:"));
        ui->widthLabel->setText(codec->toUnicode("水平轴:"));

        ui->heightLabel->setVisible(true);
        ui->heightSBox->setVisible(true);
        ui->heightLabel->setText(codec->toUnicode("垂直轴:"));

        ui->slopeAngleLabel->setVisible(true);
        ui->slopeAngleSBox->setVisible(true);
        ui->slopeAngleLabel->setText(codec->toUnicode("倾斜角度:"));

        ui->startAngleLabel->setVisible(false);
        ui->startAngleSBox->setVisible(false);

        ui->endAngleLabel->setVisible(false);
        ui->endAngleSBox->setVisible(false);
        break;
    case 4://扇形
        ui->centerXLabel->setText(codec->toUnicode("圆心.X:"));
        ui->centerYLabel->setText(codec->toUnicode("圆心.Y:"));
        ui->widthLabel->setText(codec->toUnicode("内径:"));

        ui->heightLabel->setVisible(true);
        ui->heightSBox->setVisible(true);
        ui->heightLabel->setText(codec->toUnicode("外径:"));

        ui->slopeAngleLabel->setVisible(false);
        ui->slopeAngleSBox->setVisible(false);

        ui->startAngleLabel->setVisible(true);
        ui->startAngleSBox->setVisible(true);
        ui->startAngleLabel->setText(codec->toUnicode("起始角度"));

        ui->endAngleLabel->setVisible(true);
        ui->endAngleSBox->setVisible(true);
        ui->endAngleLabel->setText(codec->toUnicode("终止角度"));
        break;
    default:
        break;
    }
}
