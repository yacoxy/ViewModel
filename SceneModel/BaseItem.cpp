#include "BaseItem.h"

#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMenu>
#include <QAction>
#include <QColorDialog>

#include <QDebug>

BaseItem::BaseItem(QColor penColor, bool isFixed, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , m_scale(1)
    , m_lastColor(penColor)
    , m_penColor(penColor)
    , m_menu(nullptr)
    , m_fixAction(nullptr)
    , m_colorAction(nullptr)
{
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    setFixed(isFixed);
}

BaseItem::BaseItem(const QPixmap &pixmap, bool isFixed, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap,parent)
    , m_scale(1)
    , m_lastColor(QColor(Qt::green))
    , m_penColor(QColor(Qt::green))
    , m_menu(nullptr)
    , m_fixAction(nullptr)
    , m_colorAction(nullptr)
{
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    setFixed(isFixed);
}

BaseItem::~BaseItem()
{

}

double BaseItem::scaleValue()
{
    return m_scale;
}

void BaseItem::setScaleValue(double scale)
{
    m_scale = scale;
}

bool BaseItem::isFixed()
{
    return m_fixed;
}

void BaseItem::setFixed(bool fixState)
{
    m_fixed = fixState;
    if(m_fixed){
        m_lastColor = m_penColor;
        setPenColor(QColor(Qt::blue));
        setFlag(QGraphicsItem::ItemIsMovable,false);
    }else{
        setPenColor(m_lastColor);
        setFlag(QGraphicsItem::ItemIsMovable);
    }
}

QColor BaseItem::penColor()
{
    return m_penColor;
}

void BaseItem::setPenColor(const QColor &penColor)
{
    m_penColor = penColor;
}

bool BaseItem::contains(const QPointF &point) const
{
    return m_realRect.contains(point);
}

QPointF BaseItem::originPoint()
{
    return m_boundRect.center();
}

QRectF BaseItem::realRect() const
{
    return m_realRect;
}

QRectF BaseItem::boundingRect() const
{
    return m_boundRect;
}

QPainterPath BaseItem::shape() const
{
    QPainterPath path;
    path.addRect(m_boundRect);
    return path;
}

void BaseItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QGraphicsView *parentView = scene()->views().isEmpty() ? nullptr : scene()->views().first();
    if(parentView == nullptr){
        return;
    }

    if(!m_menu){
        m_menu = new QMenu(parentView);
        m_fixAction = m_menu->addAction("Fixed",this,SLOT(changeFixState()));
        m_fixAction->setCheckable(true);
        m_colorAction = m_menu->addAction("Color",this,SLOT(changePenColor()));
    }

    m_menu->move(static_cast<int>(event->screenPos().x()),static_cast<int>(event->screenPos().y()));

    m_fixAction->setChecked(m_fixed);

    QPixmap colorImage = QPixmap(16,16);
    colorImage.fill(m_penColor);
    m_colorAction->setIcon(QIcon(colorImage));

    m_menu->show();

    event->accept();
}

QVariant BaseItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()){
        emit posChanged(mapToScene(originPoint()));
        return value;
    }
    return QGraphicsPixmapItem::itemChange(change,value);
}

void BaseItem::repaint()
{
    QGraphicsScene *parentScene = scene();
    if(parentScene){
        parentScene->update();
    }
}

void BaseItem::changeFixState()
{
    this->setFixed(m_fixAction->isChecked());
}

void BaseItem::changePenColor()
{
    QGraphicsView *parentView = scene()->views().isEmpty() ? nullptr : scene()->views().first();
    if(parentView == nullptr){
        return;
    }

    QColor color = QColorDialog::getColor(m_penColor,parentView,"Select pen color");
    this->setPenColor(color);
}
