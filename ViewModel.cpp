#include "ViewModel.h"
#include "ui_ViewModel.h"

#include "ImageItem.h"
#include "ShapeItem.h"
#include "PointItem.h"
#include "SceneModel.h"

ViewModel::ViewModel(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ViewModel)
{
    ui->setupUi(this);

    connect(ui->loadBackgroudBut,SIGNAL(clicked(bool)),this,SLOT(loadBackgroudImage()));
    connect(ui->loadImageItemBut,SIGNAL(clicked(bool)),this,SLOT(loadImageItemImage()));

    connect(ui->addPointItemBut,SIGNAL(clicked(bool)),this,SLOT(addPoint()));
    connect(ui->delPointItemBut,SIGNAL(clicked(bool)),this,SLOT(delPoint()));
}

ViewModel::~ViewModel()
{
    delete ui;
}

void ViewModel::loadBackgroudImage()
{

}

void ViewModel::loadImageItemImage()
{

}

void ViewModel::imageItemParamChanged()
{

}

void ViewModel::imagePosChanged(QPointF pos)
{

}

void ViewModel::imageSpinChanged(QPointF spinCenter, double spinAngle)
{

}

void ViewModel::switchShapeItem()
{

}

void ViewModel::shapeDataChanged_FromWidget()
{

}

void ViewModel::shapeDataChanged_FromItem()
{

}

void ViewModel::addPoint(QPointF pos)
{

}

void ViewModel::delPoint()
{

}

void ViewModel::blockPosChanged(int id, QPointF pos)
{

}

void ViewModel::selectionChanged()
{

}

void ViewModel::posValueChanged(int row, int column)
{

}

void ViewModel::mousePoschanged(QPointF mousePos, QRgb rgb, bool hasRgb)
{

}
