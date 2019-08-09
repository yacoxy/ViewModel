#include "PointItem.h"

#include <QGraphicsView>

//全局特征点大小，拉伸区域宽度变量
const static double Side = 8;
const static double DoubleSide = Side * 2;

PointItem::PointItem(PointID id,bool isFixed,QColor penColor)
    : BaseItem(penColor,isFixed)
    , m_id(id)
{
    if(!m_fixed){
        setFlag(QGraphicsItem::ItemIsMovable);
    }else{
        m_penColor = QColor(Qt::blue);
    }
    setFlag(QGraphicsItem::ItemIsSelectable);

    setAcceptDrops(true);
    setAcceptHoverEvents(true);

    refresh();
}

PointID PointItem::id()
{
    return m_id;
}

void PointItem::resetId(PointID id)
{
    m_id = id;
}

QPointF PointItem::getCenter()
{
    return mapToScene(m_boundRect.center());
}

void PointItem::refresh()
{
    double doubleSide = DoubleSide / m_scale;
    m_boundRect = QRectF(0,0,doubleSide,doubleSide);
    m_realRect = m_boundRect;
    m_width = m_boundRect.width();
    m_center = m_boundRect.center();

    m_transform.reset();
    m_transform.translate(-m_width / 2,-m_width / 2);
    setTransform(m_transform);
}

QVariant PointItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change & QGraphicsItem::ItemPositionHasChanged && value.type() == QVariant::PointF){
        emit posChanged(m_id,value.toPointF());
    }
    if(change & QGraphicsItem::ItemSelectedChange){
        emit selectStateChanged(m_id,value.toBool());
    }

    return QGraphicsItem::itemChange(change,value);
}

/**
 * @brief QGraphicsItem::paint函数实现。所有的绘制操作均在这里执行
 * @param painter 绘制的QPainter对象
 * @param option UNUSE
 * @param widget UNUSE
 */
void PointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    refresh();

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(m_penColor,0));

    QPointF hVec = QPointF(m_width / 2,0);
    QPointF vVec = QPointF(0,m_width / 2);
    painter->drawLine(m_center - hVec,m_center + hVec);
    painter->drawLine(m_center - vVec,m_center + vVec);

    if(isSelected()){
        painter->drawEllipse(m_center,m_width / 2,m_width / 2);
    }
    painter->setBrush(m_penColor);
    if(isSelected() || m_fixed){
        painter->drawEllipse(m_center,m_width / 8,m_width / 8);
    }else{
        painter->drawEllipse(m_center,m_width / 16,m_width / 16);
    }
}
