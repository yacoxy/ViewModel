#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <QMainWindow>

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
    void loadBackgroudImage();
    void loadImageItemImage();

    void imageItemParamChanged();

    void imagePosChanged(QPointF pos);
    void imageSpinChanged(QPointF spinCenter,double spinAngle);

    /***************ShapeItem slots********************/
    void switchShapeItem();

    void shapeDataChanged_FromWidget();
    void shapeDataChanged_FromItem();

    /***************PointItem slots********************/
    void addPoint(QPointF pos = QPointF(100,100));
    void delPoint();

    void blockPosChanged(int id,QPointF pos);
    void selectionChanged();

    void posValueChanged(int row,int column);

    /***************ScenModel slots********************/
    void mousePoschanged(QPointF mousePos,QRgb rgb,bool hasRgb);

private:
    Ui::ViewModel *ui;

    SceneModel *m_sceneModel;           //Scene

    ImageItem *m_bgImageItem;           //Backgroud image item
    ImageItem *m_opImageItem;           //Image item for operation

    ShapeItem *m_shapeItem;             //ShapeItem

    QList<PointItem *> m_poitnItemList; //PointItem list
};

#endif // VIEWMODEL_H
