#include "SceneModel.h"

#include "PointItem.h"
#include "ShapeItem.h"
#include "ImageItem.h"
#include "HistogramItem.h"

#include <QGraphicsView>
#include <QGraphicsSceneWheelEvent>

#include <QDebug>

const double scaleMax = 8.0;
const double scaleMin = 1 / scaleMax;

SceneModel::SceneModel(double sceneWidth,double sceneHeight)
    : m_sceneWidth(sceneWidth)
    , m_sceneHeight(sceneHeight)
    , m_scale(1)
    , m_histogramItem(nullptr)
    , m_pressed(false)
{
    setBackgroundBrush(QBrush(QColor(70,70,70)));
}

SceneModel::~SceneModel()
{

}

double SceneModel::getScale()
{
    return m_scale;
}

void SceneModel::setSceneSize(double width, double height)
{
    m_sceneWidth = width;
    m_sceneHeight = height;
}

PointItem *SceneModel::addPointItem(QPointF point, bool isFixed)
{
    PointItem *pointItem = new PointItem(static_cast<PointID>(m_pointItemList.length()),isFixed);
    pointItem->setPos(point);
    m_pointItemList.append(pointItem);
    pointItem->setZValue(2);
    pointItem->setScaleValue(m_scale);
    addItem(pointItem);
    return pointItem;
}

ShapeItem *SceneModel::addShapeItem(ShapeType shape)
{
    ShapeItem *shapeItem = new ShapeItem(shape);
    m_shapeItemList.append(shapeItem);
    shapeItem->setZValue(1);
    shapeItem->setScaleValue(m_scale);
    addItem(shapeItem);
    return shapeItem;
}

ImageItem *SceneModel::addImageItem(const QPixmap &image,bool isBackgroud)
{
    ImageItem *imageItem = new ImageItem(image,isBackgroud);
    double zValue = m_imageItemList.isEmpty() ? -1000 : m_imageItemList.last()->zValue();
    addItem(imageItem);

    if(isBackgroud){
        imageItem->setZValue(-1000);
        setSceneRect(image.rect());
        m_imageItemList.prepend(imageItem);
    }else{
        imageItem->setZValue(zValue + 1);
        m_imageItemList.append(imageItem);
    }

    //使窗口适应图片大小
    QGraphicsView *parentView = views().isEmpty() ? nullptr : views().first();
    if(parentView && isBackgroud){
        m_scale = 1;
        parentView->resetMatrix();
        double scaleFactor = qMin(m_sceneWidth/image.width(),m_sceneHeight/image.height());
        parentView->scale(scaleFactor,scaleFactor);
        m_scale *= scaleFactor;
        parentView->centerOn(imageItem);
    }

    imageItem->setScaleValue(m_scale);

    return imageItem;
}

HistogramItem *SceneModel::addHistogramItem(QMap<double, double> valMap)
{
    m_histogramItem = new HistogramItem(valMap);
    addItem(m_histogramItem);

    //使直方图居中
    QGraphicsView *parentView = views().isEmpty() ? nullptr : views().first();
    if(parentView){
        m_histogramItem->setWidthAndLength(parentView->width() * 4 / 5,
                                           parentView->height() * 4 / 5);
        parentView->centerOn(m_histogramItem);
    }

    m_histogramItem->setScaleValue(m_scale);

    return m_histogramItem;
}

QVector<PointID> SceneModel::delSelectedPoints()
{
    //获取被选中的item的id并排序
    QList<QGraphicsItem *> selected = selectedItems();
    QVector<PointID> selectedIdList;
    for(int i=0;i<selected.length();i++){
        selectedIdList.append((dynamic_cast<PointItem *>(selected[i]))->id());
    }
    qSort(selectedIdList.begin(),selectedIdList.end(),qLess<PointID>());

    //移除被选中的item
    for(int i=selectedIdList.length() - 1;i>=0;i--){
        removeItem(selected[i]);
        delete selected[i];
        m_pointItemList.removeAt(selectedIdList[i]);
    }

    //重设剩下的item的id
    for(int i=0;i<m_pointItemList.length();i++){
        m_pointItemList[i]->resetId(i);
    }

    return selectedIdList;
}

void SceneModel::delItem(BaseItem *item)
{
    if(!item){
        return;
    }

    PointItem *pointItem = dynamic_cast<PointItem *>(item);
    if(pointItem){
        for(int i=0;i<m_pointItemList.length();i++){
            if(pointItem == m_pointItemList[i]){
                m_pointItemList.removeAt(i);
                this->removeItem(item);
                delete item;
                break;
            }
        }
        for(int i=0;i<m_pointItemList.length();i++){
            m_pointItemList[i]->resetId(static_cast<PointID>(i));
        }
        return;
    }
    ShapeItem *shapeItem = dynamic_cast<ShapeItem *>(item);
    if(shapeItem){
        for(int i=0;i<m_shapeItemList.length();i++){
            if(shapeItem == m_shapeItemList[i]){
                m_shapeItemList.removeAt(i);
                this->removeItem(shapeItem);
                delete item;
                return;
            }
        }
    }
    ImageItem *imageItem = dynamic_cast<ImageItem *>(item);
    if(imageItem){
        for(int i=0;i<m_imageItemList.length();i++){
            if(imageItem == m_imageItemList[i]){
                m_imageItemList.removeAt(i);
                this->removeItem(imageItem);
                if(imageItem){
                    delete imageItem;
                }
                return;
            }
        }
    }
    HistogramItem *histogramItem = dynamic_cast<HistogramItem *>(item);
    if(item){
        this->removeItem(histogramItem);
        delete item;
        return;
    }
}

void SceneModel::clearAll()
{
    QGraphicsScene::clear();

    m_pointItemList.clear();
    m_shapeItemList.clear();
    m_imageItemList.clear();

    m_scale = 1;
}

void SceneModel::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF mousePos = event->scenePos();
    QRgb rgb = 0;
    bool hasRGB = false;
    if(m_imageItemList.isEmpty()){
        emit mousePosChanged(mousePos,rgb,hasRGB);
        QGraphicsScene::mouseMoveEvent(event);
        return;
    }
    for(int i=m_imageItemList.length() - 1;i>=0;i--){
        QPointF itemPos = m_imageItemList[i]->mapFromScene(mousePos);
        if(m_imageItemList[i]->contains(itemPos) && m_imageItemList[i]->isVisible()){
            hasRGB = true;
            rgb = m_imageItemList[i]->pixmap().toImage().pixel(itemPos.toPoint());
            break;
        }
    }
    emit mousePosChanged(mousePos,rgb,hasRGB);
    QGraphicsScene::mouseMoveEvent(event);
}

void SceneModel::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_pressed = true;
    QGraphicsScene::mousePressEvent(event);
}

void SceneModel::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_pressed){
        m_pressed = false;
        emit mouseClicked(event->scenePos());
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void SceneModel::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QGraphicsView *graphicsView = views().isEmpty() ? nullptr : views().at(0);
    if(graphicsView == nullptr){
        event->accept();
        return;
    }

    double delta = 0.2;
    if(event->delta() < 0){
        delta *= -1;
    }

    double scaleFactor = pow((double)2, delta);

    if((graphicsView->matrix().m11() < scaleMax && scaleFactor > 1) ||
        (graphicsView->matrix().m11() > scaleMin && scaleFactor < 1)){
        //大致保存当前的放大倍数，用于画特征点时设置笔宽
        m_scale *= scaleFactor;
        QTransform transform = graphicsView->transform();
        transform.reset();
        transform.scale(m_scale,m_scale);
        graphicsView->setTransform(transform);

        for(int i=0;i<m_imageItemList.length();i++){
            if(!m_imageItemList[i]->isFixed()){
                m_imageItemList[i]->setScaleValue(m_scale);
            }
        }

        for(int i=0;i<m_pointItemList.length();i++){
            m_pointItemList[i]->setScaleValue(m_scale);
        }

        for(int i=0;i<m_shapeItemList.length();i++){
            m_shapeItemList[i]->setScaleValue(m_scale);
        }

        if(m_histogramItem){
            m_histogramItem->setScale(m_scale);
        }
    }

    event->accept();
}

