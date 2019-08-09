#include "HistogramItem.h"

#include <QGraphicsView>

const static double HistogramWidth = 400;
const static double HistogramHeight = 200;

HistogramItem::HistogramItem(QMap<double,double> valMap)
    : BaseItem(QColor(Qt::green),false)
    , m_valMap(valMap)
    , m_width(HistogramWidth)
    , m_height(HistogramHeight)
{
    setPenColor(QColor(Qt::green));

    refresh();
}

void HistogramItem::setWidthAndLength(double width, double height)
{
    m_width = width;
    m_height = height;
    refresh();
}

void HistogramItem::insertVal(double xValue, double yValue)
{
    m_valMap[xValue] = yValue;
    repaint();
}

void HistogramItem::setValMap(QMap<double, double> &valMap)
{
    m_valMap = valMap;
    repaint();
}

void HistogramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QGraphicsItem::contextMenuEvent(event);
}

void HistogramItem::refresh()
{
    m_boundRect = QRectF(0,-m_height,m_width,m_height);
    m_realRect = m_boundRect;
}

void HistogramItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(m_penColor,0));

    painter->drawRect(m_boundRect);

    if(m_valMap.isEmpty()){
        BaseItem::paint(painter,option,widget);
        return;
    }

    painter->setBrush(QBrush(m_penColor));

    double xGap = m_width / (m_valMap.lastKey() - m_valMap.firstKey() + 1);

    double yMax = m_valMap.first();
    double yMin = m_valMap.first();
    QMapIterator<double,double> valIterator(m_valMap);
    while(valIterator.hasNext()){
        valIterator.next();
        if(valIterator.value() > yMax){
            yMax = valIterator.value();
            continue;
        }
        if(valIterator.value() < yMin){
            yMin = valIterator.value();
        }
    }
    double yGap = m_height / (yMax - yMin);

//    qDebug() << "gap : " << xGap << "  " << yGap;

    double xStart = m_valMap.firstKey();

    QMapIterator<double,double> valIterator1(m_valMap);
    while (valIterator1.hasNext()){
        valIterator1.next();
        double xPos = (valIterator1.key() - xStart) * xGap;
        double yLength = valIterator1.value() * yGap;
//        qDebug() << "xpos : " << valIterator1.key() - xStart << "  yPos : " << valIterator1.value();
        painter->drawRect(QRectF(xPos,-yLength,xGap,yLength));
    }
}
