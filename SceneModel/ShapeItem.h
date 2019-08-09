#ifndef SHAPEITEM_H
#define SHAPEITEM_H

#include "BaseItem.h"

class ShapeItem : public BaseItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

private:
    //状态枚举
    enum MousePress{
        Released = 0,

        NoPressed,
        TopPressed,
        BottomPressed,
        LeftPressed,
        RightPressed,
        TopRightPresed,
        BottomLeftPressed,
        TopLeftPressed,
        BottomRightPressed,
        RotaryPressed
    };

public:
    explicit ShapeItem(ShapeType shape,bool isFixed = false,QColor penColor = QColor(Qt::green),ShapeData shapeData = ShapeData());
    ~ShapeItem();

    ShapeType itemShape();

    ShapeData getShapeData();
    void setShapeData(ShapeData data);
    void setShapeData(double sLength = 100,double lLength = 200,double sAngle = 0,double lAngle = 90,
                      int partCount = -1, double partLength = -1);
    void setShapeData(const QList<double> data);

    void setDrawDots(bool state);

    void addText(const QPointF &pos,const QString &text);
    void removeText(int index = -1);

signals:
    void shapeDataChanged();

private:
    void refresh();
    void sectorRefresh();

    void drawRect(QPainter *painter);
    void drawRotaryRect(QPainter *painter);
    void drawEllipse(QPainter *painter);
    void drawCircle(QPainter *painter);
    void drawSector(QPainter *painter);

    void drawPartRect(QPainter *painter);

    void dealMouseMove(MousePress mousePosState,QPointF mousePos);
    void dealSectorMouseMove(MousePress mousePosState,QPointF mousePos);

private:
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
    static void getKeyShapeElement(ShapeType shapeType,QPointF pos,ShapeData shapeData,QVector<KeyShapeElement> &multiShapeDataList);

private:
    ShapeType m_shape;      //图形

    ShapeData m_shapeData;  //图形数据

    bool m_drawDots;        //是否绘制特征点

    QList<QPair<QPointF,QString>> m_textList;

private:
    //边界矩形，用来判定鼠标所在位置从而决定将要执行的功能，在设定图形数据的时候确定
    QRectF m_top;           //上下方向
    QRectF m_bottom;
    QRectF m_left;          //左右方向
    QRectF m_right;
    QRectF m_topRight;      //斜杠方向
    QRectF m_bottomLeft;
    QRectF m_topLeft;       //反斜杠方向
    QRectF m_bottomRight;
    QRectF m_rotary;        //旋转区域

    MousePress m_mousePosState;     //状态变量

    QTransform m_transform;         //变换

    QVector<QPointF> m_dotList;     //图形上的特征点集合

    QVector<QPointF> m_partPoints;  //MultiRotaryRectangle图形中各部分的特征点

    QVector<QLineF> m_partLines;    //MultiSector图形中需要补充绘制的线段
};

#endif // SHAPEITEM_H
