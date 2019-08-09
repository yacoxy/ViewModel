#include "SpinBoxDelegate.h"

#include <QSpinBox>
#include <QDoubleSpinBox>

SpinBoxDelegate::SpinBoxDelegate(bool isDoubleSpinBox,double min,double max,QObject *parent)
    : m_isDoubleSpinBox(isDoubleSpinBox)
    , m_min(min)
    , m_max(max)
    , QStyledItemDelegate(parent)
{
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    if(m_isDoubleSpinBox){
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setFrame(false);
        editor->setMinimum(m_min);
        editor->setMaximum(m_max);
        return editor;
    }else{
        QSpinBox *editor = new QSpinBox(parent);
        editor->setFrame(false);
        editor->setMinimum(static_cast<int>(m_min));
        editor->setMaximum(static_cast<int>(m_max));
        return editor;
    }
}

void SpinBoxDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
    if(m_isDoubleSpinBox){
        double value = index.model()->data(index, Qt::EditRole).toDouble();
        QDoubleSpinBox *spinBox= static_cast<QDoubleSpinBox *>(editor);
        spinBox->setValue(value);
    }else{
        int value = index.model()->data(index, Qt::EditRole).toInt();
        QSpinBox *spinBox = static_cast<QSpinBox *>(editor);
        spinBox->setValue(value);
    }
}

void SpinBoxDelegate::setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const
{
    if(m_isDoubleSpinBox){
        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox *>(editor);
        spinBox->interpretText();
        double value = spinBox->value();

        model->setData(index, value, Qt::EditRole);
    }else{
        QSpinBox *spinBox = static_cast<QSpinBox *>(editor);
        spinBox->interpretText();
        int value = spinBox->value();

        model->setData(index, value, Qt::EditRole);
    }
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
