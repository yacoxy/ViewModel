#include "ViewModel.h"
#include "ui_ViewModel.h"

#include <QFileDialog>
#include <QPixmap>
#include <QMessageBox>

#include "ImageItem.h"
#include "ShapeItem.h"
#include "PointItem.h"
#include "SceneModel.h"

ViewModel::ViewModel(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ViewModel)
    , m_sceneModel(new SceneModel)
    , m_bgImageItem(nullptr)
    , m_opImageItem(nullptr)
    , m_shapeItem(nullptr)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(m_sceneModel);
    connect(m_sceneModel,SIGNAL(mousePosChanged(QPointF,QRgb,bool)),this,SLOT(mousePoschanged(QPointF,QRgb,bool)));

    //ImageItem page
    connect(ui->loadBackgroudBut,SIGNAL(clicked(bool)),this,SLOT(loadImage()));
    connect(ui->loadImageItemBut,SIGNAL(clicked(bool)),this,SLOT(loadImage()));
    connect(ui->imageItemPosXSBox,SIGNAL(valueChanged(double)),this,SLOT(imageItemParamChanged()));
    connect(ui->imageItemPosYSBox,SIGNAL(valueChanged(double)),this,SLOT(imageItemParamChanged()));
    connect(ui->imageItemSpinCenterXSBox,SIGNAL(valueChanged(double)),this,SLOT(imageItemParamChanged()));
    connect(ui->imageItemSpinCenterYSBox,SIGNAL(valueChanged(double)),this,SLOT(imageItemParamChanged()));
    connect(ui->imageItemSpinAngleSBox,SIGNAL(valueChanged(double)),this,SLOT(imageItemParamChanged()));

    //ShapeItem page
    connect(ui->shapeSelGroupBox,SIGNAL(shapeTypeSwitched(int)),this,SLOT(switchShapeItem(int)));
    connect(ui->shapeSelGroupBox,SIGNAL(shapeDataEdited(QList<double>&)),this,SLOT(shapeDataChanged_FromWidget(QList<double>&)));

    //PointItem page
    ui->pointItemTablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(ui->pointItemTablewidget,SIGNAL(itemSelectionChanged()),this,SLOT(selectionChanged_FromTableWidget()));
    connect(ui->pointItemTablewidget,SIGNAL(cellChanged(int,int)),this,SLOT(posValueChanged_FromTableWidget(int,int)));
    connect(ui->addPointItemBut,SIGNAL(clicked(bool)),this,SLOT(addPoint()));
    connect(ui->delPointItemBut,SIGNAL(clicked(bool)),this,SLOT(delPoint()));
}

ViewModel::~ViewModel()
{
    delete ui;
}

void ViewModel::loadImage()
{
    if(!m_sceneModel){
        return;
    }

    QString imageFile = QFileDialog::getOpenFileName(this,"Load backgroud image","",tr("Images (*.png *.bmp *.jpg)"));
    if(imageFile.isEmpty()){
        return;
    }

    QImage bgImage(imageFile);
    if(bgImage.isNull()){
        QMessageBox::critical(this,"Error","Image is invalid.");
        return;
    }
    QPixmap bgPixmap = QPixmap::fromImage(bgImage);

    QString sender = QObject::sender()->objectName();
    if(sender == ui->loadBackgroudBut->objectName()){
        if(m_bgImageItem){
            m_sceneModel->delItem(m_bgImageItem);
        }
        m_bgImageItem = m_sceneModel->addImageItem(bgPixmap,true);
    }else if(sender == ui->loadImageItemBut->objectName()){
        if(m_opImageItem){
            m_sceneModel->delItem(m_opImageItem);
        }
        m_opImageItem = m_sceneModel->addImageItem(bgPixmap,false);
        connect(m_opImageItem,SIGNAL(posChanged(QPointF)),this,SLOT(imagePosChanged()));
        connect(m_opImageItem,SIGNAL(spinDataChanged(QPointF,double)),this,SLOT(imageSpinChanged(QPointF,double)));

        imageItemUIOperation(true);
    }
}

void ViewModel::imageItemParamChanged()
{
    if(!m_opImageItem){
        return;
    }
    QString senderName = QObject::sender()->objectName();
    if(senderName == ui->imageItemPosXSBox->objectName() || senderName == ui->imageItemPosYSBox->objectName()){
        disconnect(m_opImageItem,SIGNAL(posChanged(QPointF)),this,SLOT(imagePosChanged()));
        m_opImageItem->setPos(ui->imageItemPosXSBox->value(),ui->imageItemPosYSBox->value());
        connect(m_opImageItem,SIGNAL(posChanged(QPointF)),this,SLOT(imagePosChanged()));
    }else if(senderName == ui->imageItemSpinAngleSBox->objectName()){
        m_opImageItem->setSpinAngle(ui->imageItemSpinAngleSBox->value());
    }else if(senderName == ui->imageItemSpinCenterXSBox->objectName()
             || senderName == ui->imageItemSpinCenterYSBox->objectName()){
        m_opImageItem->setSpinCenter(QPointF(ui->imageItemSpinCenterXSBox->value(),ui->imageItemSpinCenterYSBox->value()));
        ui->imageItemSpinAngleSBox->setValue(0);
    }
}

void ViewModel::imagePosChanged()
{
    if(!m_opImageItem){
        return;
    }

    disconnect(ui->imageItemPosXSBox,SIGNAL(valueChanged(double)),this,SLOT(imageItemParamChanged()));
    disconnect(ui->imageItemPosYSBox,SIGNAL(valueChanged(double)),this,SLOT(imageItemParamChanged()));

    QPointF delta = m_opImageItem->mapToScene(m_opImageItem->transformOriginPoint())
            - m_opImageItem->transformOriginPoint();

    ui->imageItemPosXSBox->setValue(delta.x());
    ui->imageItemPosYSBox->setValue(delta.y());

    connect(ui->imageItemPosXSBox,SIGNAL(valueChanged(double)),this,SLOT(imageItemParamChanged()));
    connect(ui->imageItemPosYSBox,SIGNAL(valueChanged(double)),this,SLOT(imageItemParamChanged()));
}

void ViewModel::imageSpinChanged(QPointF spinCenter, double spinAngle)
{
    ui->imageItemSpinCenterXSBox->setValue(spinCenter.x());
    ui->imageItemSpinCenterYSBox->setValue(spinCenter.y());

    imagePosChanged();

    ui->imageItemSpinAngleSBox->setValue(spinAngle);
}

void ViewModel::switchShapeItem(int type)
{
    if(!m_sceneModel){
        return;
    }

    if(m_shapeItem){
        m_sceneModel->delItem(m_shapeItem);
        m_shapeItem = nullptr;
    }

    if(type != -1){
        m_shapeItem = m_sceneModel->addShapeItem(static_cast<ShapeType>(type));
        connect(m_shapeItem,SIGNAL(shapeDataChanged()),this,SLOT(shapeDataChanged_FromItem()));
    }
}

void ViewModel::shapeDataChanged_FromWidget(QList<double> &data)
{
    if(!m_sceneModel || !m_shapeItem || data.length() != 7){
        return;
    }

    disconnect(m_shapeItem,SIGNAL(shapeDataChanged()),this,SLOT(shapeDataChanged_FromItem()));

    m_shapeItem->setShapeData(data);

    connect(m_shapeItem,SIGNAL(shapeDataChanged()),this,SLOT(shapeDataChanged_FromItem()));
}

void ViewModel::shapeDataChanged_FromItem()
{
    if(!m_sceneModel || !m_shapeItem){
        return;
    }

    QPointF pos = m_shapeItem->pos();
    ShapeData shapeData = m_shapeItem->getShapeData();

    QList<double> data;
    data << pos.x() << pos.y() << shapeData.m_sLength << shapeData.m_lLength
         << shapeData.m_sAngle << shapeData.m_sAngle << shapeData.m_lAngle;
    if(m_shapeItem->itemShape() == Shape_Rectangle){
        data[2] += data[0];
        data[3] += data[1];
    }
    ui->shapeSelGroupBox->setShapeEditor(data);
}

void ViewModel::addPoint(QPointF point)
{
    if(!m_sceneModel){
        return;
    }

    int row = ui->pointItemTablewidget->rowCount();
    ui->pointItemTablewidget->setRowCount(row + 1);
    ui->pointItemTablewidget->setItem(row,0,new QTableWidgetItem(QString::number(point.x())));
    ui->pointItemTablewidget->setItem(row,1,new QTableWidgetItem(QString::number(point.y())));

    PointItem *item = m_sceneModel->addPointItem(point,false);
    m_pointItemList.append(item);
    connect(item,SIGNAL(posChanged(PointID,QPointF)),this,SLOT(pointItemPosChanged(PointID,QPointF)));
    connect(item,SIGNAL(selectStateChanged(PointID,bool)),this,SLOT(selectionChangedFromItem()));
}

void ViewModel::delPoint()
{
    if(!m_sceneModel){
        return;
    }

    QVector<int> selectedPointsId;
    QList<QTableWidgetSelectionRange> selectRange = ui->pointItemTablewidget->selectedRanges();
    for(int i=0;i<selectRange.length();i++){
        for(int j=selectRange[i].topRow();j<=selectRange[i].bottomRow();j++){
           selectedPointsId.append(j);
        }
    }

    qSort(selectedPointsId.begin(),selectedPointsId.end());

    for(int i=selectedPointsId.length() - 1;i>=0;i--){
        m_sceneModel->delItem(m_pointItemList[selectedPointsId[i]]);
        m_pointItemList.removeAt(selectedPointsId[i]);
        ui->pointItemTablewidget->removeRow(selectedPointsId[i]);
    }
}

void ViewModel::pointItemPosChanged(PointID id, QPointF pos)
{
    if(id >= ui->pointItemTablewidget->rowCount()){
        return;
    }

    ui->pointItemTablewidget->item(id,0)->setText(QString::number(pos.x()));
    ui->pointItemTablewidget->item(id,1)->setText(QString::number(pos.y()));
}

void ViewModel::selectionChangedFromItem()
{
    disconnect(ui->pointItemTablewidget,SIGNAL(itemSelectionChanged()),this,SLOT(selectionChangedFromWidget()));
    QList<QGraphicsItem *> selected = m_sceneModel->selectedItems();
    QList<int> selectedIdList;
    for(int i=0;i<selected.length();i++){
        PointItem *item = dynamic_cast<PointItem *>(selected[i]);
        if(item){
            selectedIdList.append(item->id());
        }
    }

    //选中表格中item对应的行
    ui->pointItemTablewidget->clearSelection();
    ui->pointItemTablewidget->setSelectionMode(QAbstractItemView::MultiSelection);
    for(int i=0;i<selectedIdList.length();i++){
        ui->pointItemTablewidget->selectRow(selectedIdList[i]);
    }
    ui->pointItemTablewidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(ui->pointItemTablewidget,SIGNAL(itemSelectionChanged()),this,SLOT(selectionChangedFromWidget()));
}

void ViewModel::selectionChangedFromWidget()
{
    QList<int> selectedIdList;
    QList<QTableWidgetSelectionRange> selectRange = ui->pointItemTablewidget->selectedRanges();
    for(int i=0;i<selectRange.length();i++){
        for(int j=selectRange[i].topRow();j<=selectRange[i].bottomRow();j++){
            selectedIdList.append(j);
        }
    }

    for(int i=0;i<m_pointItemList.length();i++){
        PointItem *item = m_pointItemList[i];
        disconnect(item,SIGNAL(selectStateChanged(PointID,bool)),this,SLOT(selectionChangedFromItem()));
        item->setSelected(selectedIdList.contains(i));
        connect(item,SIGNAL(selectStateChanged(PointID,bool)),this,SLOT(selectionChangedFromItem()));
    }
}

void ViewModel::mousePoschanged(QPointF mousePos, QRgb rgb, bool hasRgb)
{
    ui->mousePosLabel->setText(QString("x=%1,y=%2").arg(mousePos.x()).arg(mousePos.y()));
    if(!hasRgb){
        ui->rgbLabel->clear();
        return;
    }
    if(rgb < 256){
        ui->rgbLabel->setText(QString("Gray=%1").arg(qGray(rgb)));
    }else{
        ui->rgbLabel->setText(QString("R=%1,G=%2,B=%3").arg(qRed(rgb)).arg(qGreen(rgb)).arg(qBlue(rgb)));
    }
}

void ViewModel::imageItemUIOperation(bool enable)
{
    ui->imageItemPosXSBox->setEnabled(enable);
    ui->imageItemPosYSBox->setEnabled(enable);
    ui->imageItemSpinCenterXSBox->setEnabled(enable);
    ui->imageItemSpinCenterYSBox->setEnabled(enable);
    ui->imageItemSpinAngleSBox->setEnabled(enable);
}
