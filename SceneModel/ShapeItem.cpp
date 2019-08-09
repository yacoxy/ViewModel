#include "ShapeItem.h"

#include <QGraphicsView>
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QPainter>
#include <QPen>
#include <QRectF>
#include <QPointF>
#include <QSizeF>
#include <QGraphicsSceneWheelEvent>
#include <QLineF>
#include <qmath.h>

#include <QDebug>

//全局特征点大小，拉伸区域宽度变量
const double Side = 3;
const double DoubleSide = Side * 2;

ShapeItem::ShapeItem(ShapeType shape,bool isFixed,QColor penColor,ShapeData shapeData)
    : BaseItem(penColor,isFixed)
    , m_shape(shape)
    , m_shapeData(shapeData)
    , m_drawDots(true)
{
    setFlag(QGraphicsItem::ItemIsSelectable);

    setAcceptDrops(true);
    setAcceptHoverEvents(true);

    if(m_shape == Shape_Circle){
        m_shapeData.m_lLength = m_shapeData.m_sLength;
    }

    if(m_shape == Shape_Sector || m_shape == Shape_MultiSector){
        sectorRefresh();
    }else{
        refresh();
    }
}

ShapeItem::~ShapeItem()
{

}

ShapeType ShapeItem::itemShape()
{
    return m_shape;
}

ShapeData ShapeItem::getShapeData()
{
    return m_shapeData;
}

void ShapeItem::setShapeData(ShapeData data)
{
    setShapeData(data.m_sLength,data.m_lLength,data.m_sAngle,data.m_lAngle,data.m_partCount,data.m_partLength);
}

void ShapeItem::setShapeData(double sLength, double lLength, double sAngle, double lAngle, int partCount, double partLength)
{
    m_shapeData.m_sLength = sLength;
    m_shapeData.m_lLength = lLength;
    m_shapeData.m_sAngle = sAngle;
    m_shapeData.m_lAngle = lAngle;
    m_shapeData.m_partCount = partCount;
    m_shapeData.m_partLength = partLength;

    //传入数据在构造绘图数据之前的预处理
    switch(m_shape){
    case Shape_Circle:
        if(m_shapeData.m_sLength > m_shapeData.m_lLength){
            m_shapeData.m_lLength = m_shapeData.m_sLength;
        }else{
            m_shapeData.m_sLength = m_shapeData.m_lLength;
        }
        break;
    }

    if(m_shape == Shape_Sector || m_shape == Shape_MultiSector){
        sectorRefresh();
    }else{
        refresh();
    }
}

void ShapeItem::setShapeData(const QList<double> data)
{
    if(data.length() != 7){
        return;
    }
    QPointF pos(data[0],data[1]);
    this->setPos(pos);

    switch (itemShape()){
    case Shape_Rectangle:
        this->setShapeData(data[2] - data[0],data[3] - data[1]);
        break;
    case Shape_RotaryRectangle:
        this->setShapeData(data[2],data[3],data[4]);
        break;
    case Shape_Circle:
        this->setShapeData(data[2],data[2]);
        break;
    case Shape_Ellipse:
        this->setShapeData(data[2],data[3],data[4]);
        break;
    case Shape_Sector:
        this->setShapeData(data[2],data[3],data[5],data[6]);
        break;
    }
}

void ShapeItem::setDrawDots(bool state)
{
    m_drawDots = state;
}

void ShapeItem::addText(const QPointF &pos, const QString &text)
{
    m_textList.append(QPair<QPointF,QString>(pos,text));
}

void ShapeItem::removeText(int index)
{
    if(index < 0){
        m_textList.clear();
        return;
    }
    m_textList.removeAt(index);
}

void ShapeItem::refresh()
{
    m_shapeData.m_sLength = qAbs(m_shapeData.m_sLength);
    m_shapeData.m_lLength = qAbs(m_shapeData.m_lLength);

    double curSide = Side / m_scale;
    double curDoubleSide = 2 * curSide;

    //除扇形之外的其它图形用到的计算数据
    const double cutXPos = m_shapeData.m_sLength - curSide;
    const double cutYPos = m_shapeData.m_lLength - curSide;
    const double halfWidth = m_shapeData.m_sLength / 2;
    const double halfHeight = m_shapeData.m_lLength / 2;

    //拉伸判定区域（共用）
    m_top = QRectF(halfWidth - curSide,-curSide,curDoubleSide,curDoubleSide);
    m_bottom = QRectF(halfWidth - curSide,cutYPos,curDoubleSide,curDoubleSide);

    m_left = QRectF(-curSide,halfHeight - curSide,curDoubleSide,curDoubleSide);
    m_right = QRectF(cutXPos,halfHeight - curSide,curDoubleSide,curDoubleSide);

    //特征点获取（共用）
    m_dotList.clear();
    m_dotList.append(QPointF(0,halfHeight));
    m_dotList.append(QPointF(halfWidth,0));
    m_dotList.append(QPointF(m_shapeData.m_sLength,halfHeight));
    m_dotList.append(QPointF(halfWidth,m_shapeData.m_lLength));

    //boundRect更新（共用）
    m_realRect = QRectF(0,0,m_shapeData.m_sLength,m_shapeData.m_lLength);
    m_boundRect = QRectF(-curDoubleSide,-curDoubleSide,m_shapeData.m_sLength + 2 * curDoubleSide,m_shapeData.m_lLength + 2 * curDoubleSide);

    //矩形无须进行变换，其它的图形需要一些独有的变换
    if(m_shape == Shape_RotaryRectangle || m_shape == Shape_Ellipse || m_shape == Shape_MultiRotaryRectangle){
        m_transform.reset();
        m_transform.rotate(-m_shapeData.m_sAngle);
        m_transform.translate(-m_shapeData.m_sLength / 2,-m_shapeData.m_lLength / 2);
        setTransform(m_transform);
    }else if(m_shape == Shape_Circle){
        m_transform.reset();
        m_transform.translate(-m_shapeData.m_sLength / 2,-m_shapeData.m_lLength / 2);
        setTransform(m_transform);
    }

    //不同图形需要的特有数据
    switch(m_shape){
    case Shape_Rectangle:
        //倾斜拉伸区域
        m_topRight = QRectF(cutXPos,0,curDoubleSide,curDoubleSide);
        m_bottomLeft = QRectF(0,cutYPos,curDoubleSide,curDoubleSide);

        m_topLeft = QRectF(0,0,curDoubleSide,curDoubleSide);
        m_bottomRight = QRectF(cutXPos,cutYPos,curDoubleSide,curDoubleSide);

        //矩形四个角的特征点
        m_dotList.append(QPointF(0,0));
        m_dotList.append(QPointF(m_shapeData.m_sLength,0));
        m_dotList.append(QPointF(m_shapeData.m_sLength,m_shapeData.m_lLength));
        m_dotList.append(QPointF(0,m_shapeData.m_lLength));
        break;
    case Shape_RotaryRectangle:
        //旋转控制区域
        m_rotary = m_right;
        m_right = QRectF();
        break;
    case Shape_Ellipse:
        //旋转控制区域
        m_rotary = m_right;
        m_right = QRectF();
        break;
    case Shape_MultiRotaryRectangle:
        //旋转控制区域
        m_rotary = m_right;
        m_right = QRectF();
        //仅组合图形需要更新分割各部分的特征点
        m_partPoints.clear();
        if(m_shapeData.m_partCount > 0){
            double splitLength = m_shapeData.m_lLength / m_shapeData.m_partCount;//每一个分割部分的高度
            if(qAbs(splitLength) < m_shapeData.m_partLength){
                m_shapeData.m_partLength = qAbs(splitLength);
            }
            for(int i=0;i<m_shapeData.m_partCount;i++){
                double posX = m_realRect.topLeft().x();
                double posY = m_realRect.topLeft().y() + ((double)i + 0.5) * splitLength - m_shapeData.m_partLength / 2;
                m_partPoints.append(QPointF(posX,posY));
            }
        }
    }

    prepareGeometryChange();
}

void ShapeItem::sectorRefresh()
{
    //首先对数据进行预处理，保证sLength短半径<lLength长半径;sAngle起始角度<lAngle结束角度
    swapIfLess(m_shapeData.m_lLength,m_shapeData.m_sLength);
    swapIfLess(m_shapeData.m_lAngle,m_shapeData.m_sAngle);

    const double delta = m_shapeData.m_lLength;
    const double halfAngle = (m_shapeData.m_lAngle + m_shapeData.m_sAngle) / 2;
    const double halfRadius = (m_shapeData.m_lLength + m_shapeData.m_sLength) / 2;

    double curSide = Side / m_scale;
    double curDoubleSide = curSide * 2;

    //扇形的特征点，用来绘制图形上的突出操作点
    m_dotList.clear();
    m_dotList.append(QPointF(m_shapeData.m_lLength * qCos(qDegreesToRadians(halfAngle)) + delta,-m_shapeData.m_lLength * qSin(qDegreesToRadians(halfAngle)) + delta));
    m_dotList.append(QPointF(m_shapeData.m_sLength * qCos(qDegreesToRadians(halfAngle)) + delta,-m_shapeData.m_sLength * qSin(qDegreesToRadians(halfAngle)) + delta));
    m_dotList.append(QPointF(halfRadius * qCos(qDegreesToRadians(m_shapeData.m_lAngle)) + delta,-halfRadius * qSin(qDegreesToRadians(m_shapeData.m_lAngle)) + delta));
    m_dotList.append(QPointF(halfRadius * qCos(qDegreesToRadians(m_shapeData.m_sAngle)) + delta,-halfRadius * qSin(qDegreesToRadians(m_shapeData.m_sAngle)) + delta));
    //圆心点
    m_dotList.append(QPointF(m_shapeData.m_lLength,m_shapeData.m_lLength));

    //扇形的拉伸操作判定区域
    m_top = QRectF(m_dotList.at(0) - QPointF(curSide,curSide),m_dotList.at(0) + QPointF(curSide,curSide));
    m_bottom = QRectF(m_dotList.at(1) - QPointF(curSide,curSide),m_dotList.at(1) + QPointF(curSide,curSide));
    m_left = QRectF(m_dotList.at(2) - QPointF(curSide,curSide),m_dotList.at(2) + QPointF(curSide,curSide));

    //boundRect更新
    m_realRect = QRectF(0,0,m_shapeData.m_lLength * 2,m_shapeData.m_lLength * 2);
    m_boundRect = QRectF(-curDoubleSide,-curDoubleSide,m_realRect.width() + 2 * curDoubleSide,m_realRect.height() + 2 * curDoubleSide);

    //旋转控制区域
    m_rotary = QRectF(m_dotList.at(3) - QPointF(curSide,curSide),m_dotList.at(3) + QPointF(curSide,curSide));

    //需要进行的变换
    m_transform.reset();
    m_transform.translate(-m_shapeData.m_lLength,-m_shapeData.m_lLength);
    setTransform(m_transform);

    //仅组合图形需要更新分割各部分的特征点
    m_partLines.clear();
    if(m_shape == Shape_MultiSector && m_shapeData.m_partCount > 0){
        double splitAngle = (m_shapeData.m_lAngle - m_shapeData.m_sAngle) / m_shapeData.m_partCount;
        for(int i=0;i<m_shapeData.m_partCount;i++){
            //求中心点
            double goalAngle = qDegreesToRadians(m_shapeData.m_sAngle + splitAngle * (0.5 + i));
            QPointF centerPoint = QPointF(halfRadius * qCos(goalAngle) + delta,-halfRadius * qSin(goalAngle) + delta);
            //求中心往四个顶点走的移动向量
            double midRectWidth = (m_shapeData.m_lLength - m_shapeData.m_sLength) / 2;
            double midRectHeight = m_shapeData.m_partLength / 2;
            QPointF horizontalVector = QPointF(midRectWidth * qCos(goalAngle),-midRectWidth * qSin(goalAngle));
            QPointF verticalVector = QPointF(-midRectHeight * qSin(goalAngle),-midRectHeight * qCos(goalAngle));

            //矩形四个顶点
            QPointF topRight = centerPoint + horizontalVector + verticalVector;
            QPointF topLeft = centerPoint - horizontalVector + verticalVector;
            QPointF bottomLeft = centerPoint - horizontalVector - verticalVector;
            QPointF bottomRight = centerPoint + horizontalVector - verticalVector;

            //添加矩形四条边到要补充绘制的线段中
            m_partLines.append(QLineF(topLeft,topRight));
            m_partLines.append(QLineF(topRight,bottomRight));
            m_partLines.append(QLineF(bottomRight,bottomLeft));
            m_partLines.append(QLineF(bottomLeft,topLeft));
        }
    }

    prepareGeometryChange();
}

/**
 * @brief 画矩形
 * @param painter
 */
void ShapeItem::drawRect(QPainter *painter)
{
    painter->drawRect(m_realRect);
}

/**
 * @brief 画旋转矩形
 * @param painter
 */
void ShapeItem::drawRotaryRect(QPainter *painter)
{
    painter->drawRect(m_realRect);
}

/**
 * @brief 画椭圆
 * @param painter
 */
void ShapeItem::drawEllipse(QPainter *painter)
{
    painter->drawEllipse(m_realRect);
}

/**
 * @brief 画圆
 * @param painter
 */
void ShapeItem::drawCircle(QPainter *painter)
{
    painter->drawEllipse(m_realRect);
}

/**
 * @brief 画扇形
 * @param painter
 */
void ShapeItem::drawSector(QPainter *painter)
{
    const double delta = m_shapeData.m_lLength - m_shapeData.m_sLength;
    QRectF outerRect = QRectF(0,0,m_shapeData.m_lLength * 2,m_shapeData.m_lLength * 2);
    QRectF innerRect = QRectF(delta,delta,m_shapeData.m_sLength * 2,m_shapeData.m_sLength * 2);
    painter->drawArc(outerRect,m_shapeData.m_sAngle * 16,(m_shapeData.m_lAngle - m_shapeData.m_sAngle) * 16);
    painter->drawArc(innerRect,m_shapeData.m_sAngle * 16,(m_shapeData.m_lAngle - m_shapeData.m_sAngle) * 16);

    const double sCosVal = qCos(qDegreesToRadians(m_shapeData.m_sAngle));
    const double sSinVal = -qSin(qDegreesToRadians(m_shapeData.m_sAngle));
    const double lCosVal = qCos(qDegreesToRadians(m_shapeData.m_lAngle));
    const double lSinVal = -qSin(qDegreesToRadians(m_shapeData.m_lAngle));
    QPointF sStart(m_shapeData.m_sLength * sCosVal,m_shapeData.m_sLength * sSinVal);
    QPointF sEnd(m_shapeData.m_lLength * sCosVal,m_shapeData.m_lLength * sSinVal);
    QPointF lStart(m_shapeData.m_sLength * lCosVal,m_shapeData.m_sLength * lSinVal);
    QPointF lEnd(m_shapeData.m_lLength * lCosVal,m_shapeData.m_lLength * lSinVal);
    QPointF center = QPointF(m_shapeData.m_lLength,m_shapeData.m_lLength);
    painter->drawLine(sStart + center,sEnd + center);
    painter->drawLine(lStart + center,lEnd + center);
}

/**
 * @brief 绘制组合图形中的分割部分
 * @param painter
 */
void ShapeItem::drawPartRect(QPainter *painter)
{
    if(m_shapeData.m_partLength <= 0){
        return;
    }
    if(m_shape == Shape_MultiRotaryRectangle && !m_partPoints.isEmpty()){
        for(int i=0;i<m_partPoints.length();i++){
            painter->drawRect(m_partPoints[i].x(),m_partPoints[i].y(),m_shapeData.m_sLength,m_shapeData.m_partLength);
        }
    }else if(m_shape == Shape_MultiSector && !m_partLines.isEmpty()){
        painter->drawLines(m_partLines);
    }
}

/**
 * @brief 处理除扇形外其它图形鼠标按下之后的移动动作，这里主要处理的是放大和缩小的动作
 * @param mousePosState 按下时的位置状态，即根据按下时位置所在区域
 * @param mousePos 鼠标移动到了哪个位置
 */
void ShapeItem::dealMouseMove(MousePress mousePosState,QPointF mousePos)
{
    //移动时首先判断要往哪个方向对图形进行放大或缩小
    //再根据图形形状采用不同的计算方式，结果直接放入图形数据中，之后调用更新函数对其它相关参数及绘制部分进行更新
    switch(mousePosState){
    case TopPressed:
        switch (m_shape) {
        case Shape_Rectangle:
            m_shapeData.m_lLength -= mousePos.y();
            moveBy(0,mousePos.y());
            break;
        case Shape_RotaryRectangle:
        case Shape_Ellipse:
        case Shape_MultiRotaryRectangle:
            m_shapeData.m_lLength -= mousePos.y();
            break;
        case Shape_Circle:
            m_shapeData.m_lLength -= mousePos.y();
            m_shapeData.m_sLength = m_shapeData.m_lLength;
            break;
        case Shape_Sector:
        case Shape_MultiSector:
            break;
        default:
            break;
        }
        break;
    case BottomPressed:
        switch (m_shape) {
        case Shape_Rectangle:
        case Shape_RotaryRectangle:
        case Shape_Ellipse:
        case Shape_MultiRotaryRectangle:
            m_shapeData.m_lLength = mousePos.y();
            break;
        case Shape_Circle:
            m_shapeData.m_lLength = mousePos.y();
            m_shapeData.m_sLength = m_shapeData.m_lLength;
            break;
        case Shape_Sector:
            break;
        default:
            break;
        }
        break;
    case LeftPressed:
        switch (m_shape) {
        case Shape_Rectangle:
            m_shapeData.m_sLength -= mousePos.x();
            moveBy(mousePos.x(),0);
            break;
        case Shape_RotaryRectangle:
        case Shape_Ellipse:
        case Shape_MultiRotaryRectangle:
            m_shapeData.m_sLength -= mousePos.x();
            break;
        case Shape_Circle:
            m_shapeData.m_sLength -= mousePos.x();
            m_shapeData.m_lLength = m_shapeData.m_sLength;
            break;
        case Shape_Sector:
            break;
        default:
            break;
        }
        break;
    case RightPressed:
        switch (m_shape) {
        case Shape_Rectangle:
        case Shape_RotaryRectangle:
        case Shape_Ellipse:
        case Shape_MultiRotaryRectangle:
            m_shapeData.m_sLength = mousePos.x();
            break;
        case Shape_Circle:
            m_shapeData.m_sLength = mousePos.x();
            m_shapeData.m_lLength = m_shapeData.m_sLength;
            break;
        case Shape_Sector:
            break;
        default:
            break;
        }
        break;
    case TopRightPresed:
        switch (m_shape) {
        case Shape_Rectangle:
            m_shapeData.m_lLength -= mousePos.y();
            m_shapeData.m_sLength = mousePos.x();
            moveBy(0,mousePos.y());
            break;
        case Shape_RotaryRectangle:
        case Shape_Ellipse:
        case Shape_Circle:
        case Shape_MultiRotaryRectangle:
            break;
        case Shape_Sector:
            break;
        default:
            break;
        }
        break;
    case BottomLeftPressed:
        switch (m_shape) {
        case Shape_Rectangle:
        case Shape_RotaryRectangle:
        case Shape_Ellipse:
        case Shape_MultiRotaryRectangle:
            m_shapeData.m_lLength = mousePos.y();
            m_shapeData.m_sLength -= mousePos.x();
            moveBy(mousePos.x(),0);
            break;
        case Shape_Circle:
            break;
        case Shape_Sector:
            break;
        default:
            break;
        }
        break;
    case TopLeftPressed:
        switch (m_shape) {
        case Shape_Rectangle:
        case Shape_RotaryRectangle:
        case Shape_Ellipse:
        case Shape_MultiRotaryRectangle:
            m_shapeData.m_lLength -= mousePos.y();
            m_shapeData.m_sLength -= mousePos.x();
            moveBy(mousePos.x(),mousePos.y());
            break;
        case Shape_Circle:
            break;
        case Shape_Sector:
            break;
        default:
            break;
        }
        break;
    case BottomRightPressed:
        switch (m_shape) {
        case Shape_Rectangle:
        case Shape_RotaryRectangle:
        case Shape_Ellipse:
        case Shape_MultiRotaryRectangle:
            m_shapeData.m_lLength = mousePos.y();
            m_shapeData.m_sLength = mousePos.x();
            break;
        case Shape_Circle:
            break;
        case Shape_Sector:
            break;
        default:
            break;
        }
        break;
    case RotaryPressed:
        switch (m_shape) {
        case Shape_RotaryRectangle:
        case Shape_Ellipse:
        case Shape_MultiRotaryRectangle:
        {
            mousePos = mapToScene(mousePos);
            QPointF delta = mousePos - mapToScene(QPointF(m_realRect.width() / 2,m_realRect.height() / 2));

            double deltaX = delta.x();
            double deltaY = delta.y();
            double radius = sqrt(deltaX * deltaX + deltaY * deltaY);

            if(deltaY < 0){
                m_shapeData.m_sAngle = qRadiansToDegrees(qAcos(deltaX / radius));
            }else if(deltaY > 0){
                m_shapeData.m_sAngle = -qRadiansToDegrees(qAcos(deltaX / radius));
            }else{
                if(deltaX > 0){
                    m_shapeData.m_sAngle = 0;
                }else{
                    m_shapeData.m_sAngle = 180;
                }
            }
        }
            break;
        }
        break;
    }
}

/**
 * @brief 处理在扇形中鼠标按下之后的移动动作，这里主要处理的时放大和缩小的动作
 * @param mousePosState 按下时的位置状态，即根据按下时位置所在区域
 * @param mousePos 鼠标移动到了哪个位置
 */
void ShapeItem::dealSectorMouseMove(MousePress mousePosState,QPointF mousePos)
{
    //移动时首先判断要往哪个方向对图形进行放大或缩小
    //再根据图形形状采用不同的计算方式，结果直接放入图形数据中，之后调用更新函数对其它相关参数及绘制部分进行更新
    double radius;
    double deltaY,deltaX;
    double goalAngle;
    switch(mousePosState){
    case TopPressed:
        m_shapeData.m_lLength = sqrt((mousePos.x() - m_shapeData.m_lLength) * (mousePos.x() - m_shapeData.m_lLength)
                      + (mousePos.y() - m_shapeData.m_lLength) * (mousePos.y() - m_shapeData.m_lLength));
        break;
    case BottomPressed:
        m_shapeData.m_sLength = sqrt((mousePos.x() - m_shapeData.m_lLength) * (mousePos.x() - m_shapeData.m_lLength)
                      + (mousePos.y() - m_shapeData.m_lLength) * (mousePos.y() - m_shapeData.m_lLength));
        break;
    case LeftPressed:
        deltaX = mousePos.x() - m_shapeData.m_lLength;
        deltaY = mousePos.y() - m_shapeData.m_lLength;
        radius = sqrt(deltaX * deltaX + deltaY * deltaY);

        if(deltaY < 0){
            m_shapeData.m_lAngle = qRadiansToDegrees(qAcos(deltaX / radius));
        }else if(deltaY > 0){
            m_shapeData.m_lAngle = -qRadiansToDegrees(qAcos(deltaX / radius));
        }else{
            if(deltaX > 0){
                m_shapeData.m_lAngle = 0;
            }else{
                m_shapeData.m_lAngle = 180;
            }
        }
        if(m_shapeData.m_lAngle < m_shapeData.m_sAngle){
            m_shapeData.m_lAngle += 360;
        }
        break;
    case RightPressed:
        deltaX = mousePos.x() - m_shapeData.m_lLength;
        deltaY = mousePos.y() - m_shapeData.m_lLength;
        radius = sqrt(deltaX * deltaX + deltaY * deltaY);

        if(deltaY < 0){
            m_shapeData.m_sAngle = qRadiansToDegrees(qAcos(deltaX / radius));
        }else if(deltaY > 0){
            m_shapeData.m_sAngle = -qRadiansToDegrees(qAcos(deltaX / radius));
        }else{
            if(deltaX > 0){
                m_shapeData.m_sAngle = 0;
            }else{
                m_shapeData.m_sAngle = 180;
            }
        }
        if(m_shapeData.m_sAngle > m_shapeData.m_lAngle){
            m_shapeData.m_sAngle -= 360;
        }
        break;
    case RotaryPressed:
        deltaX = mousePos.x() - m_shapeData.m_lLength;
        deltaY = mousePos.y() - m_shapeData.m_lLength;
        radius = sqrt(deltaX * deltaX + deltaY * deltaY);

        if(deltaY < 0){
            goalAngle = qRadiansToDegrees(qAcos(deltaX / radius));
        }else if(deltaY > 0){
            goalAngle = -qRadiansToDegrees(qAcos(deltaX / radius));
        }else{
            if(deltaX > 0){
                goalAngle = 0;
            }else{
                goalAngle = 180;
            }
        }
        m_shapeData.m_lAngle += goalAngle - m_shapeData.m_sAngle;
        m_shapeData.m_sAngle = goalAngle;
        break;
    }
}

/**
 * @brief QGraphicsItem::hoverLeaveEvent函数实现。确保鼠标形状能恢复成箭头。
 * @param event 事件参数
 */
void ShapeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(QCursor(Qt::ArrowCursor));
    QGraphicsItem::hoverLeaveEvent(event);
}

/**
 * @brief QGraphicsItem::hoverMoveEvent函数实现。当鼠标放松且在图形上移动时，根据其所处的位置改变其鼠标图标。
 * @param event 事件参数
 */
void ShapeItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!m_drawDots){
        QGraphicsItem::hoverMoveEvent(event);
        return;
    }

    QPointF mousePos = event->pos();
    QCursor currentCursor(Qt::ArrowCursor);
    if(m_top.contains(mousePos) || m_bottom.contains(mousePos)
            || m_left.contains(mousePos) || m_right.contains(mousePos)
            || m_topRight.contains(mousePos) || m_bottomLeft.contains(mousePos)
            || m_topLeft.contains(mousePos) || m_bottomRight.contains(mousePos)){
        currentCursor.setShape(Qt::SizeAllCursor);
    }else if(m_rotary.contains(mousePos)){
        QPixmap pic;
        pic.load(":/icon/rotate.png");
        pic = pic.scaled(QSize(16,16),Qt::KeepAspectRatio);
        QCursor cursor(pic);
        currentCursor.swap(cursor);
    }

    setCursor(currentCursor);

    QGraphicsItem::hoverMoveEvent(event);
}

/**
 * @brief QGraphicsItem::mouseMoveEvent函数实现。当鼠标按下并移动时，根据记录的状态执行相应的操作。
 * @param event 事件参数
 */
void ShapeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!m_drawDots){
        QGraphicsItem::mouseMoveEvent(event);
        return;
    }

    if(m_mousePosState > NoPressed){
        //由于扇形的处理方式和其它有较大区别，所以单独处理。
        if(m_shape == Shape_Sector || m_shape == Shape_MultiSector){
            dealSectorMouseMove(m_mousePosState,event->pos());
            sectorRefresh();
        }else{
            dealMouseMove(m_mousePosState,event->pos());
            refresh();
        }
        emit shapeDataChanged();
        return;
    }else if(m_mousePosState == NoPressed){
        emit shapeDataChanged();
    }
    QGraphicsItem::mouseMoveEvent(event);
}

/**
 * @brief QGraphicsItem::mousePressEvent函数实现。当鼠标按下时，根据鼠标所处位置来设定状态
 * @param event 事件参数
 */
void ShapeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_shape == Shape_Sector || m_shape == Shape_MultiSector){
        QPointF mousePos = event->pos();
        QPointF delta = mousePos - m_realRect.center();
        double deltaX = delta.x();
        double deltaY = delta.y();
        double radius = sqrt(deltaX * deltaX + deltaY * deltaY);
        if(radius > m_shapeData.m_lLength + 1){
            event->ignore();
            return;
        }

        double deltaAngel;
        if(deltaY < 0){
            deltaAngel = qRadiansToDegrees(qAcos(deltaX / radius));
        }else if(deltaY > 0){
            deltaAngel = -qRadiansToDegrees(qAcos(deltaX / radius));
        }else{
            if(deltaX > 0){
                deltaAngel = 0;
            }else{
                deltaAngel = 180;
            }
        }

        deltaAngel -= 360;
        bool inSector = false;
        for(int i=0;i<3;i++,deltaAngel += 360){
            if(deltaAngel > m_shapeData.m_sAngle - 3 && deltaAngel < m_shapeData.m_lAngle + 3){
                inSector = true;
                break;
            }
        }
        if(!inSector){
            event->ignore();
            return;
        }
    }

    m_mousePosState = NoPressed;

    QPointF mousePos = event->pos();
    if(m_top.contains(mousePos)){
        m_mousePosState = TopPressed;
    }else if(m_bottom.contains(mousePos)){
        m_mousePosState = BottomPressed;
    }else if(m_left.contains(mousePos)){
        m_mousePosState = LeftPressed;
    }else if(m_right.contains(mousePos)){
        m_mousePosState = RightPressed;
    }else if(m_topRight.contains(mousePos)){
        m_mousePosState = TopRightPresed;
    }else if(m_bottomLeft.contains(mousePos)){
        m_mousePosState = BottomLeftPressed;
    }else if(m_topLeft.contains(mousePos)){
        m_mousePosState = TopLeftPressed;
    }else if(m_bottomRight.contains(mousePos)){
        m_mousePosState = BottomRightPressed;
    }else if(m_rotary.contains(mousePos)){
        m_mousePosState = RotaryPressed;
    }

    QGraphicsItem::mousePressEvent(event);
}

/**
 * @brief QGraphicsItem::mouseReleaseEvent函数实现。当鼠标松开时，还原按下时设定的状态
 * @param event 事件参数
 */
void ShapeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_mousePosState = Released;
    QGraphicsItem::mouseReleaseEvent(event);
}

void ShapeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(m_penColor,0));

    //不同的图形单独实现不同的绘制函数
    switch(m_shape){
    case Shape_Rectangle:
        drawRect(painter);
        break;
    case Shape_RotaryRectangle:
        drawRotaryRect(painter);
        break;
    case Shape_Circle:
        drawCircle(painter);
        break;
    case Shape_Sector:
        drawSector(painter);
        break;
    case Shape_Ellipse:
        drawEllipse(painter);
        break;
    case Shape_MultiRotaryRectangle:
        drawRotaryRect(painter);
        drawPartRect(painter);
        break;
    case Shape_MultiSector:
        drawSector(painter);
        drawPartRect(painter);
        break;
    }

    painter->setPen(QColor(Qt::green));
    for(int i=0;i<m_textList.length();i++){
        QRectF rect(m_textList[i].first,QSizeF(m_realRect.size() / 2));
        painter->drawText(rect,m_textList[i].second);
    }

    if(!m_drawDots){
        return;
    }

    //获取当前背景的放大倍数，用来保持特征点在显示时看上去大小基本保持一致
    double scaleFactor = Side / m_scale;
    painter->setBrush(m_penColor);
    for(int i=0;i<m_dotList.length();i++){
        painter->drawEllipse(m_dotList[i],scaleFactor,scaleFactor);
    }
}

void ShapeItem::getKeyShapeElement(ShapeType shapeType, QPointF center, ShapeData shapeData, QVector<KeyShapeElement> &keyShapeElementList)
{
    if(shapeType == Shape_MultiSector){
        double halfRadius = (shapeData.m_lLength + shapeData.m_sLength) / 2;
        double splitAngle = (shapeData.m_lAngle - shapeData.m_sAngle) / shapeData.m_partCount;
        for(int i=0;i<shapeData.m_partCount;i++){
            //求中心点
            double goalAngle = qDegreesToRadians(shapeData.m_sAngle + splitAngle * (0.5 + i));
            QPointF centerPoint = QPointF(halfRadius * qCos(goalAngle),
                                          -halfRadius * qSin(goalAngle)) + center;
            keyShapeElementList.append(KeyShapeElement(centerPoint.x(),centerPoint.y(),goalAngle,shapeData.m_partLength,halfRadius));
        }
    }else if(shapeType == Shape_MultiRotaryRectangle){

    }
}
