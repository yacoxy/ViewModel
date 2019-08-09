#ifndef POINTITEM_H
#define POINTITEM_H

#include "BaseItem.h"

class PointItem : public BaseItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit PointItem(PointID id,bool fixed = false,QColor penColor = QColor(Qt::green));

    PointID id();
    void resetId(PointID);

    QPointF getCenter();

signals:
    void posChanged(PointID id,QPointF pos);
    void selectStateChanged(PointID id,bool state);

private:
    void refresh();

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    PointID m_id;           //用以表示各个点
    QPointF m_center;       //整个点所占区域的中心位置

    QTransform m_transform; //用于将原点平移至中心位置的变换

    double m_width;         //点区域的长度
};

#endif // POINTITEM_H
