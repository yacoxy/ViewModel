#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <QMainWindow>

#include "SceneModelGloabl.h"

namespace Ui {
class ViewModel;
}

class BaseItem;
class PointItem;
class ShapeItem;
class ImageItem;
class HistogramItem;
class SceneModel;

class ViewModel : public QMainWindow
{
    Q_OBJECT

public:
    explicit ViewModel(QWidget *parent = 0);
    ~ViewModel();

public slots:
    /**************ImageItem slots*********************/
    void loadImage();

    void imageItemParamChanged();

    void imagePosChanged();
    void imageSpinChanged(QPointF spinCenter,double spinAngle);

    /***************ShapeItem slots********************/
    void switchShapeItem(int type);

    void shapeDataChanged_FromWidget(QList<double> &data);
    void shapeDataChanged_FromItem();

    /***************PointItem slots********************/
    void addPoint(QPointF point = QPointF(100,100));
    void delPoint();

    void pointItemPosChanged(PointID id,QPointF pos);
    void selectionChangedFromItem();
    void selectionChangedFromWidget();

    /***************ScenModel slots********************/
    void mousePoschanged(QPointF mousePos,QRgb rgb,bool hasRgb);

private:
    void imageItemUIOperation(bool enable);

private:
    Ui::ViewModel *ui;

    SceneModel *m_sceneModel;           //Scene

    ImageItem *m_bgImageItem;           //Backgroud image item
    ImageItem *m_opImageItem;           //Image item for operation

    ShapeItem *m_shapeItem;             //ShapeItem

    QList<PointItem *> m_pointItemList; //PointItem list
};

#endif // VIEWMODEL_H
