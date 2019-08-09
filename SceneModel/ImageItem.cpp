#include "ImageItem.h"

#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <qmath.h>
#include <QDebug>
#include <QMenu>

const static double MinCoordinateLength = 50;
const static double ArrowWidth = 8;

ImageItem::ImageItem(const QPixmap &pixmap, bool isFixed, QGraphicsItem *parent)
    : BaseItem(pixmap,isFixed,parent)
    , m_spinCenter(QPointF(0,0))
    , m_spinAngle(0)
    , m_drawCoordinate(!isFixed)
    , m_imageWidth(pixmap.width())
    , m_imageHeight(pixmap.height())
    , m_pressState(NO_PRESS)
    , m_sceneMenu(nullptr)
{
    if(!m_fixed){
        setFlag(QGraphicsItem::ItemIsSelectable);
    }

    setAcceptHoverEvents(true);

    refresh();
}

ImageItem::~ImageItem()
{
    if(m_sceneMenu){
        delete m_sceneMenu;
    }
}

QPointF ImageItem::spinCenter()
{
    return m_spinCenter;
}

void ImageItem::setSpinCenter(const QPointF &center)
{
    m_spinAngle = 0;
    setRotation(m_spinAngle);
    m_spinCenter = center;
    setTransformOriginPoint(m_spinCenter);
//    repaint();
}

double ImageItem::spinAngle()
{
    return m_spinAngle;
}

void ImageItem::setSpinAngle(double angle)
{
    m_spinAngle = angle;
    this->setRotation(m_spinAngle);
//    repaint();
}

bool ImageItem::hasCoordinate()
{
    return m_drawCoordinate;
}

void ImageItem::setDrawCoordinate(bool state)
{
    m_drawCoordinate = state;
}

QSizeF ImageItem::imageSize()
{
    return QSizeF(m_imageWidth,m_imageHeight);
}

void ImageItem::refresh()
{
    double arrowWidth = ArrowWidth / m_scale;
    double minCoordinateLength = MinCoordinateLength / m_scale;

    //boundingRect
    m_realRect = QRectF(0,0,m_imageWidth - 1,m_imageHeight - 1);
    if(!m_drawCoordinate){
        m_boundRect = QRectF(0,0,m_imageWidth,m_imageHeight);
        return;
    }else{
        QPointF topleft = QPointF(-arrowWidth / 2,-arrowWidth / 2);
        double boundingWidth = minCoordinateLength + arrowWidth;
        QPointF bottomRight = m_spinCenter + QPointF(boundingWidth,boundingWidth);
        double boundingHeight = (m_imageHeight > boundingWidth ? m_imageHeight : boundingWidth) + arrowWidth / 2;
        boundingWidth = (boundingWidth > m_imageWidth ? boundingWidth : m_imageWidth) + arrowWidth / 2;
        if(m_spinCenter.x() < 0){
            topleft.setX(m_spinCenter.x() - arrowWidth / 2);
            boundingWidth -= m_spinCenter.x();
        }
        if(m_spinCenter.y() < 0){
            topleft.setY(m_spinCenter.y() - arrowWidth / 2);
            boundingHeight -= m_spinCenter.y();
        }

        if(bottomRight.x() > m_imageWidth){
            boundingWidth = bottomRight.x() + arrowWidth;
        }
        if(bottomRight.y() > m_imageHeight){
            boundingHeight = bottomRight.y() + arrowWidth;
        }

        m_boundRect = QRectF(topleft,QSizeF(boundingWidth,boundingHeight));
    }

    //coordinate
    QPolygonF horizontalArrow,verticalArrow;
    horizontalArrow.append(QPointF(minCoordinateLength,-arrowWidth/2) + m_spinCenter);
    horizontalArrow.append(QPointF(minCoordinateLength,arrowWidth/2) + m_spinCenter);
    horizontalArrow.append(QPointF(minCoordinateLength + arrowWidth,0) + m_spinCenter);
    verticalArrow.append(QPointF(-arrowWidth/2,minCoordinateLength) + m_spinCenter);
    verticalArrow.append(QPointF(arrowWidth/2,minCoordinateLength) + m_spinCenter);
    verticalArrow.append(QPointF(0,minCoordinateLength + arrowWidth) + m_spinCenter);

    QPainterPath hArrow,vArrow;
    hArrow.addPolygon(horizontalArrow);
    m_hArrow.swap(hArrow);
    vArrow.addPolygon(verticalArrow);
    m_vArrow.swap(vArrow);

    m_hLine = QLineF(m_spinCenter,QPointF(minCoordinateLength,0) + m_spinCenter);
    m_vLine = QLineF(m_spinCenter,QPointF(0,minCoordinateLength) + m_spinCenter);
    m_originRect = QRectF(m_spinCenter - QPointF(arrowWidth / 8, arrowWidth / 8),
                          QSizeF(arrowWidth / 4, arrowWidth / 4));

    //operation area
    m_transArea = QRectF(m_spinCenter - QPointF(arrowWidth / 2, arrowWidth / 2),QSizeF(arrowWidth,arrowWidth));
    m_rotateArea = QRectF(m_spinCenter - QPointF(-arrowWidth / 2, arrowWidth / 2),
                          QSizeF(arrowWidth + minCoordinateLength,arrowWidth));
}

void ImageItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF mousePos = event->pos();
    QCursor currentCursor(Qt::ArrowCursor);
    if(m_transArea.contains(mousePos)){
        currentCursor.setShape(Qt::SizeAllCursor);
    }else if(m_rotateArea.contains(mousePos)){
        currentCursor.setShape(Qt::OpenHandCursor);
    }

    setCursor(currentCursor);

    QGraphicsPixmapItem::hoverMoveEvent(event);
}

void ImageItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF mousePos = event->pos();
    if(m_transArea.contains(mousePos)){
        m_pressState = TRANS_PRESS;
    }else if(m_rotateArea.contains(mousePos)){
        m_pressState = ROTATE_PRESS;
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsPixmapItem::mousePressEvent(event);
}

void ImageItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_fixed){
        BaseItem::mouseMoveEvent(event);
        return;
    }

    switch(m_pressState){
    case NO_PRESS:
        QGraphicsPixmapItem::mouseMoveEvent(event);
        return;
    case TRANS_PRESS:
    {
        m_spinAngle = 0;
        setRotation(m_spinAngle);
        m_spinCenter = event->pos();
        setTransformOriginPoint(m_spinCenter);
        break;
    }
    case ROTATE_PRESS:
    {
        QPointF mousePos = event->pos();
        mousePos = mapToScene(mousePos);
        QPointF delta = mousePos - mapToScene(m_spinCenter);

        double deltaX = delta.x();
        double deltaY = delta.y();
        double radius = sqrt(deltaX * deltaX + deltaY * deltaY);

        if(deltaY < 0){
            m_spinAngle = -qRadiansToDegrees(qAcos(deltaX / radius));
        }else if(deltaY > 0){
            m_spinAngle = qRadiansToDegrees(qAcos(deltaX / radius));
        }else{
            if(deltaX > 0){
                m_spinAngle = 0;
            }else{
                m_spinAngle = 180;
            }
        }
        setRotation(m_spinAngle);
        break;
    }
    }

    prepareGeometryChange();

    emit spinDataChanged(m_spinCenter,m_spinAngle);

    event->accept();
}

void ImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_pressState = NO_PRESS;
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void ImageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPixmapItem::paint(painter,option,widget);
    if(!m_drawCoordinate){
        return;
    }

    refresh();
    painter->setPen(QPen(m_penColor,0));

    painter->drawLine(m_hLine);
    painter->fillPath(m_hArrow,QBrush(m_penColor));
    painter->drawLine(m_vLine);
    painter->fillPath(m_vArrow,QBrush(m_penColor));

    painter->setBrush(m_penColor);
    painter->drawEllipse(m_originRect);
}

void ImageItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if(m_drawCoordinate){
        BaseItem::contextMenuEvent(event);
        return;
    }

    QGraphicsView *parentView = scene()->views().isEmpty() ? nullptr : scene()->views().first();
    if(parentView == nullptr){
        QGraphicsItem::contextMenuEvent(event);
        return;
    }

    if(!m_sceneMenu){
        m_sceneMenu = new QMenu(parentView);
        m_arrowAction = m_sceneMenu->addAction("Arrow",this,SLOT(enableArrow()));
        m_arrowAction->setCheckable(true);

        m_dragAction = m_sceneMenu->addAction("HandDrag",this,SLOT(enableHandDrag()));
        m_dragAction->setCheckable(true);

        m_rubberBandAction = m_sceneMenu->addAction("RubberBand",this,SLOT(enableRubberBand()));
        m_rubberBandAction->setCheckable(true);
    }

    QGraphicsView::DragMode dragMode = parentView->dragMode();
    m_arrowAction->setChecked(false);
    m_dragAction->setChecked(false);
    m_rubberBandAction->setChecked(false);
    switch (dragMode) {
    case QGraphicsView::NoDrag:
        m_arrowAction->setChecked(true);
        break;
    case QGraphicsView::ScrollHandDrag:
        m_dragAction->setChecked(true);
        break;
    case QGraphicsView::RubberBandDrag:
        m_rubberBandAction->setChecked(true);
        break;
    }

    m_sceneMenu->move(static_cast<int>(event->screenPos().x()),static_cast<int>(event->screenPos().y()));

    m_sceneMenu->show();

    event->accept();
}

void ImageItem::enableArrow()
{
    QGraphicsView *parentView = scene()->views().isEmpty() ? nullptr : scene()->views().first();
    if(parentView == nullptr){
        return;
    }
    parentView->setDragMode(QGraphicsView::NoDrag);
}

void ImageItem::enableHandDrag()
{
    QGraphicsView *parentView = scene()->views().isEmpty() ? nullptr : scene()->views().first();
    if(parentView == nullptr){
        return;
    }
    parentView->setDragMode(QGraphicsView::ScrollHandDrag);
}

void ImageItem::enableRubberBand()
{
    QGraphicsView *parentView = scene()->views().isEmpty() ? nullptr : scene()->views().first();
    if(parentView == nullptr){
        return;
    }
    parentView->setDragMode(QGraphicsView::RubberBandDrag);
}
