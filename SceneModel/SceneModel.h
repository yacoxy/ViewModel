#ifndef SCENEMODEL_H
#define SCENEMODEL_H

#include <QGraphicsScene>

#include "SceneModelGloabl.h"

#ifndef SCENE_INIT_WIDTH
#define SCENE_INIT_WIDTH 800.0
#endif

#ifndef SCENE_INIT_HEIGHT
#define SCENE_INIT_HEIGHT 800.0
#endif


class BaseItem;
class PointItem;
class ShapeItem;
class ImageItem;
class HistogramItem;

class SceneModel : public QGraphicsScene
{
    Q_OBJECT
public:
    SceneModel(double sceneWidth = SCENE_INIT_WIDTH,double sceneHeight = SCENE_INIT_HEIGHT);
    virtual ~SceneModel();

    double getScale();
    void setScale(double scale);

    QSize sceneSize();
    void setSceneSize(double width,double height);

    PointItem * addPointItem(QPointF point,bool isFixed);
    ShapeItem * addShapeItem(ShapeType shape);
    ImageItem * addImageItem(const QPixmap &image,bool isBackgroud);
    HistogramItem *addHistogramItem(QMap<double,double> valMap);

    QVector<PointID> delSelectedPoints();

    void delItem(BaseItem *);
    void clearAll();

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);

signals:
    void mousePosChanged(QPointF mousePos,QRgb rgb,bool hasRGB);
    void mouseClicked(QPointF clickPos);

    void scaleChanged(double scale);

private:
    double m_sceneWidth;
    double m_sceneHeight;

    QList<PointItem *> m_pointItemList;
    QList<ShapeItem *> m_shapeItemList;
    QList<ImageItem *> m_imageItemList;

    HistogramItem *m_histogramItem;

    double m_scale;

    bool m_pressed;
};

#endif // SCENEMODEL_H
