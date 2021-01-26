#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QWidget>

class QVBoxLayout;
class QPropertyAnimation;
class QPushButton;

class ToolBox : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int switchHeight READ switchHeight WRITE setSwitchHeight)

public:
    explicit ToolBox(QWidget *parent = nullptr);
    ~ToolBox();

    int addItem(QWidget *widget, const QString &text);
    int addItem(QWidget *widget, const QIcon &icon, const QString &text);
    int insertItem(int index, QWidget *widget, const QString &text);
    int insertItem(int index, QWidget *widget, const QIcon &icon, const QString &text);

    void setCurrentIndex(int index);

    void changItemText(int index, const QString &text);

    void resizeEvent(QResizeEvent *event) override;

private:
    int switchHeight();
    void setSwitchHeight(int height);

signals:
    void currentChanged(int index);

private slots:
    void changeCurrent();

private:
    QVBoxLayout *m_vLayout;

    QList<QWidget *> m_widgetList;
    QList<QPushButton *> m_butList;
    int m_lastIndex;
    int m_currentIndex;

    int m_maxHeight;

    QPropertyAnimation *m_animation;
};

#endif // TOOLBOX_H
