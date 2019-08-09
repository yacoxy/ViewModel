#ifndef SHAPESELGROUPBOX_H
#define SHAPESELGROUPBOX_H

#include <QWidget>
#include <QIcon>
#include <QtUiPlugin/QDesignerExportWidget>

namespace Ui {
class ShapeSelGroupBox;
}

class QDESIGNER_WIDGET_EXPORT ShapeSelGroupBox : public QWidget
{
    Q_OBJECT

public:
    explicit ShapeSelGroupBox(QWidget *parent = 0);
    ~ShapeSelGroupBox();

    QList<double> getEditorData();

signals:
    void shapeTypeSwitched(int type);
    void shapeDataEdited(QList<double> &data);

public slots:
    void setShapeType(int type);
    void setShapeEditor(const QList<double> &data);

private slots:
    void on_clearShapeBut_clicked();
    void on_rectBut_clicked();
    void on_rotaryRectBut_clicked();
    void on_circleBut_clicked();
    void on_ellipseBut_clicked();
    void on_sectorBut_clicked();
    void editorValueChanged();

private:
    void connectSignal(bool isDisconnect);
    void setSelectBoxItem(int type);

private:
    Ui::ShapeSelGroupBox *ui;
    int m_type;
};

#endif // SHAPESELGROUPBOX_H
