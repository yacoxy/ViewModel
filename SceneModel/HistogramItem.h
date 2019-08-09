#ifndef HISTOGRAMITEM_H
#define HISTOGRAMITEM_H

#include "BaseItem.h"

class HistogramItem : public BaseItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit HistogramItem(QMap<double,double> valMap);

    void setWidthAndLength(double width,double height);

    void insertVal(double xValue,double yValue);
    void setValMap(QMap<double,double> &valMap);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    void refresh();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QMap<double,double> m_valMap;

    double m_width;
    double m_height;
};

#endif // HISTOGRAMITEM_H
