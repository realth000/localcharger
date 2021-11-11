#ifndef PROXYSTYLE_H
#define PROXYSTYLE_H

#include <QProxyStyle>
#include <QStyleOption>
#include <QPainter>
#include <QProxyStyle>
#include <QStyleOptionTab>
#include <QStyleOptionTabWidgetFrame>
#include <QStyleOptionButton>
#include <QStyleOptionComboBox>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QStyleHintReturn>

// ProxyStyle defines

#define TABBAR_LABEL_WIDTH             120
#define TABBAR_LABEL_HEIGHT             60
#define TABBAR_SHADOW_WIDTH              3
#define TABWIDGET_BUTTON_UP_HEIGHT      43                      // tabwidget menu的高度
#define TABBAR_BACKGROUND_COLOR               "#232323"     // tabwidget左侧选项卡的label的背景颜色，需要和qss里QTabWidget的背景颜色一致
#define TABWIDGET_MENU_BACKGROUND_COLOR       "#282828"     // 一横条的背景，宽度是button_up_height + offset
#define PUSHBUTTON_DISABLE_COLOR              "#282828"     // disable color 要比上边这个menu background color 浅
#define PUSHBUTTON_ON_WIDGET_BACKGROUND_COLOR "#333333"     // 设置在widget上而不在一横条背景上的PushButton在普通状态时的背景颜色
#define CHECKBOX_DIABLE_COLOR                 "#808080"     // QCheckBox disabled color
#define CHECKBOX_CHECK_WIDTH             4

#define TABWIDGET_MENU_HORIZONTAL_MARGIN 0     // 一横条的高与pushbutton的高度差，要比pushbutton宽。
#define TABWIDGET_MENU_VERTICAL_MARGIN   5     // 一横条的高与pushbutton的高度差，要比pushbutton宽。

#define PADDING_UP    4
#define PADDING_LEFT  4
#define PADDING_DOWN  4

class TabBarStyle : public QProxyStyle
{
public:
    virtual QSize sizeFromContents(ContentsType type, const QStyleOption *option,const QSize &size, const QWidget *widget) const override;
    virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
};

class TabWidgetStyle : public QProxyStyle
{
public:
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
    virtual QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const override;
    virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
};

class PushButtonStyle : public QProxyStyle
{
public:
    virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;

private:
    const QString normalColor;
};

class HorizontalScrollBarStyle : public QProxyStyle
{
public:
    virtual void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;

};

class VerticalScrollBarStyle : public QProxyStyle
{
public:
    virtual void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;

};

class CheckBoxStyle : public QProxyStyle
{
public:
    virtual void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

};

class RadioButtonStyle : public QProxyStyle
{
public:
    virtual void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

};

class ComboBoxStyle: public QProxyStyle
{
public:
    virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
    virtual void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;
//    virtual void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
};

class TreeViewStyle: public QProxyStyle
{
public:
    virtual void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
};
#endif // PROXYSTYLE_H
