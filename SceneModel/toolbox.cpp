#include "toolbox.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QDebug>

ToolBox::ToolBox(QWidget *parent)
    : QWidget(parent)
    , m_vLayout(new QVBoxLayout(this))
    , m_lastIndex(-1)
    , m_currentIndex(-1)
    , m_animation(nullptr)
{
    setLayout(m_vLayout);
    m_vLayout->setSpacing(0);
    m_vLayout->setContentsMargins(0, 0, 0, 0);

    setContentsMargins(0, 0, 0, 0);
}

ToolBox::~ToolBox()
{
    if(!m_animation){
        m_animation->deleteLater();
    }
}

int ToolBox::addItem(QWidget *item, const QString &text)
{
    return insertItem(-1, item, QIcon(), text);
}

int ToolBox::addItem(QWidget *item, const QIcon &iconSet, const QString &text)
{
    return insertItem(-1, item, iconSet, text);
}

int ToolBox::insertItem(int index, QWidget *item, const QString &text)
{
    return insertItem(index, item, QIcon(), text);
}

int ToolBox::insertItem(int index, QWidget *widget, const QIcon &icon, const QString &text)
{
    if(index >= m_widgetList.length() || index < 0){
        index = m_widgetList.length();
    }

    QPushButton *but = new QPushButton(icon, text, this);
    connect(but, SIGNAL(clicked(bool)), this, SLOT(changeCurrent()));

    if(index >= m_widgetList.length()){
        m_vLayout->addWidget(but);
        m_vLayout->addWidget(widget);
        m_widgetList.append(widget);
        m_butList.append(but);
    }else{
        m_vLayout->insertWidget(index * 2, but);
        m_vLayout->insertWidget(index * 2 + 1, widget);
        m_widgetList.insert(index, widget);
        m_butList.insert(index, but);
    }

    widget->setMaximumHeight(m_currentIndex == -1 ? 16777215 : 0);

    if(index <= m_currentIndex || m_currentIndex == -1){
        m_currentIndex++;
        emit currentChanged(m_currentIndex);
    }
    return index;
}

void ToolBox::setCurrentIndex(int index)
{
    if(index == m_currentIndex || m_widgetList.isEmpty() || m_currentIndex < 0){
        return;
    }

    if(index >= m_widgetList.length()){
        index = m_widgetList.length() - 1;
    }else if(index <= 0){
        index = 0;
    }
    m_lastIndex = m_currentIndex;
    m_currentIndex = index;

    if(!m_animation){
        m_animation = new QPropertyAnimation(this, "switchHeight");
        m_animation->setStartValue(0);
        m_animation->setEndValue(m_maxHeight);
        m_animation->setEasingCurve(QEasingCurve::InOutCubic);
        m_animation->setDuration(500);
    }
    m_animation->start();
    emit currentChanged(m_currentIndex);
}

void ToolBox::changItemText(int index, const QString &text)
{
    if(index >= m_butList.length()){
        return;
    }

    m_butList[index]->setText(text);
}

void ToolBox::resizeEvent(QResizeEvent *event)
{
    m_maxHeight = event->size().height();
    if(m_currentIndex >= 0 && m_currentIndex < m_widgetList.length()){
        m_widgetList[m_currentIndex]->setMaximumHeight(m_maxHeight);
    }
    if(m_animation){
        m_animation->setEndValue(m_maxHeight);
    }
    QWidget::resizeEvent(event);
}

int ToolBox::switchHeight()
{
    if(m_currentIndex >= 0 && m_currentIndex < m_widgetList.length()){
        return m_widgetList[m_currentIndex]->height();
    }
    return m_maxHeight;
}

void ToolBox::setSwitchHeight(int height)
{
    if(m_lastIndex >= 0 && m_lastIndex < m_widgetList.length()){
        m_widgetList[m_lastIndex]->setMaximumHeight(m_maxHeight - height);
    }
    if(m_currentIndex >= 0 && m_currentIndex < m_widgetList.length()){
        m_widgetList[m_currentIndex]->setMaximumHeight(height);
    }
}

void ToolBox::changeCurrent()
{
    QPushButton *but = qobject_cast<QPushButton *>(QObject::sender());
    if(!but){
        return;
    }
    for(int i=0;i<m_butList.length();i++){
        if(but == m_butList[i]){
            setCurrentIndex(i);
        }
    }
}
