#ifndef SCENEMODELGLOABL_H
#define SCENEMODELGLOABL_H

#include <QDebug>

/**************PointItem*****************/
typedef int PointID;

/**************ShapeItem*****************/
//形状枚举
enum ShapeType{
    Shape_Rectangle = 0,
    Shape_RotaryRectangle,
    Shape_Circle,
    Shape_Ellipse,
    Shape_Sector,
    //组合图形
    Shape_MultiRotaryRectangle,
    Shape_MultiSector
};
//形状数据，上面的5种形状通用的数据结构
class ShapeData
{
public:
    ShapeData()
        : m_sLength(100)
        , m_lLength(200)
        , m_sAngle(0)
        , m_lAngle(90)
        , m_partCount(-1)
        , m_partLength(-1)
    {}

    ShapeData(double sLength,double lLength,double sAngle,double lAngle,int partCount,double partLength)
        : m_sLength(sLength)
        , m_lLength(lLength)
        , m_sAngle(sAngle)
        , m_lAngle(lAngle)
        , m_partCount(partCount)
        , m_partLength(partLength)
    {}

public:
    //下方四个变量在不同的图形中代表不同含义，依次描述如下。
    //矩形：水平方向宽度，垂直方向长度，后面两个没有用到
    //旋转矩形：水平方向宽度，垂直方向长度，顺时针旋转角度，最后一个没用到
    //圆：直径，直径，后面两个没有用到
    //椭圆：长直径/短直径，短直径/长直径，顺时针旋转角度，最后一个没用到
    //扇形：内半径，外半径，逆时针起始角度，逆时针结束角度
    double m_sLength;
    double m_lLength;
    double m_sAngle;
    double m_lAngle;
    //组合型图形的变量
    int m_partCount;//组合图形分成多少部分
    double m_partLength;//每一部分的宽度
};

class KeyShapeElement{
public:
    KeyShapeElement(double centerX=0,double centerY=0,double rotation=0,double width=0,double height=0)
        : m_centerX(centerX)
        , m_centerY(centerY)
        , m_rotation(rotation)
        , m_width(width)
        , m_height(height)
    {}
public:
    double m_centerX;
    double m_centerY;
    double m_rotation;
    double m_width;
    double m_height;
};

template<typename T>
void swapIfLess(T &left,T &right)
{
    if(left < right){
        T temp = left;
        left = right;
        right = temp;
    }
}


#endif // SCENEMODELGLOABL_H
