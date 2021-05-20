#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include "BaseItem.h"

class ImageItem : public BaseItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit ImageItem(const QPixmap &pixmap, bool fixed = true, QGraphicsItem *parent = nullptr);
    ~ImageItem();

    QPointF spinCenter();
    void setSpinCenter(const QPointF &center);

    double spinAngle();
    void setSpinAngle(double angle);

    bool hasCoordinate();
    void setDrawCoordinate(bool state);

    QSizeF imageSize();
    void setImage(const QPixmap &pixmap);

private:
    void refresh();

    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private slots:
    void enableArrow();
    void enableHandDrag();
    void enableRubberBand();

signals:
    void spinDataChanged(QPointF spinCenter,double spinAngle);

private:
    QPointF m_spinCenter;   //旋转中心X
    double m_spinAngle;     //旋转角度，顺时针为正，单位为°
    bool m_drawCoordinate;  //是否绘制坐标系

    double m_imageWidth;
    double m_imageHeight;

    QLineF m_hLine,m_vLine;
    QPainterPath m_hArrow,m_vArrow;
    QRectF m_originRect;

    QRectF m_transArea;
    QRectF m_rotateArea;

    enum PressState {
        NO_PRESS,
        TRANS_PRESS,
        ROTATE_PRESS
    };
    PressState m_pressState;

    QTransform m_transform;

private:
    QMenu *m_sceneMenu;
    QAction *m_arrowAction;
    QAction *m_dragAction;
    QAction *m_rubberBandAction;
};

#endif // IMAGEITEM_H
