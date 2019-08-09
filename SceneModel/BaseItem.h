#ifndef BASEITEM_H
#define BASEITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>

#include "SceneModelGloabl.h"

class QAction;
class BaseItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    BaseItem(QColor penColor,bool isFixed,QGraphicsItem *parent = Q_NULLPTR);
    BaseItem(const QPixmap &pixmap,bool isFixed,QGraphicsItem *parent = Q_NULLPTR);
    virtual ~BaseItem() = 0;

    double scaleValue();
    void setScaleValue(double scale);

    bool isFixed();
    void setFixed(bool fixState);

    QColor penColor();
    void setPenColor(const QColor &penColor);

    bool contains(const QPointF &point) const;

    virtual QPointF originPoint();

    virtual QRectF realRect() const;
    QRectF boundingRect() const;

protected:
    QPainterPath shape() const;

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void repaint();

protected slots:
    void changeFixState();
    void changePenColor();

signals:
    void posChanged(QPointF);

protected:
    double m_scale;

    bool m_fixed;

    QColor m_lastColor;
    QColor m_penColor;

    QRectF m_boundRect;
    QRectF m_realRect;

private:
    QMenu *m_menu;
    QAction *m_fixAction;
    QAction *m_colorAction;
};

#endif // BASEITEM_H
