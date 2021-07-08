#include <QPainter>
#include <QStyle>
#include <QStyleOptionFocusRect>
#include "bliderbutton.h"
#include "resource.h"

BliderButton::BliderButton(QString title, QWidget* parent) : QPushButton(title, parent)
{
    init();
}

BliderButton::BliderButton(QWidget* parent, QRect r, QString title)
    : QPushButton(title, parent)
{
    init();

    setGeometry(r);
}

BliderButton::BliderButton(QWidget* parent, QPoint pos, QSize size, QString title)
    : QPushButton(title, parent)
{
    init();

    setGeometry(QRect(pos, size));
}


BliderButton::BliderButton(QWidget* parent, int x, int y, int width, int height, QString title)
    : QPushButton(title, parent)
{
    init();

    setGeometry(x, y, width, height);
}

BliderButton::~BliderButton()
{

}

void BliderButton::init()
{
    m_bChecked = false;
    setAutoDefault(false);

    setCursor(QCursor(Qt::PointingHandCursor));

    m_normalColor = Qt::black;
    m_disableColor  = Qt::gray;

}

void BliderButton::setSkin(QString strNormal, QString strOver, QString strDown, QString strDisable, QString strFocus)
{
    setSkin( QPixmap(strNormal), QPixmap(strOver), QPixmap(strDown), QPixmap(strDisable), QPixmap(strFocus) );
}

void BliderButton::setSkin(QPixmap normal, QPixmap over, QPixmap down, QPixmap disable, QPixmap focus)
{
    m_mutex.lock();

    resize( normal.size() );
    int w = width(), h = height();

    if( !normal.isNull() )
        m_normal = normal.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if( !over.isNull() )
        m_over = over.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if( !down.isNull() )
        m_down = down.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if( !disable.isNull() )
        m_disable = disable.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if( !focus.isNull() )
        m_focus = focus.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if(m_over.isNull())     m_over = m_normal;
    if(m_down.isNull())     m_down = m_normal;
    if(m_disable.isNull())  m_disable = m_normal;
    if(m_focus.isNull())    m_focus = m_normal;

    if( m_font.pixelSize() < 0 )
    {
        QFont f(DEFAULT_BUTTON_FONT_NAME );
        f.setBold(true);
        f.setPixelSize( (int)(height() * 0.27) );
        setFont( f );
    }
    else
        setFont( m_font );

    m_mutex.unlock();
}

void BliderButton::setTextImage(QString normal, QString disable)
{
    setTextImage(QPixmap(normal), QPixmap(disable));
}

void BliderButton::setTextImage(QPixmap normal, QPixmap disable)
{
    m_mutex.lock();

    int w = width(), h = height();
    if( !normal.isNull() )
        m_normalText = normal.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if( !disable.isNull() )
        m_disableText = disable.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if(m_disableText.isNull())  m_disableText = m_normalText;

    m_mutex.unlock();
}

void BliderButton::setTextColor(QColor textColor, QColor shadowColor)
{
    m_normalColor = m_disableColor = textColor;

    m_shadowNormalColor = m_shadowDisableColor = shadowColor;
}

void BliderButton::setTextColor(QColor normalColor, QColor shadowNormalColor,
                              QColor disableColor, QColor shadowDisableColor)
{
    m_normalColor       = normalColor;
    m_shadowNormalColor = shadowNormalColor;
    m_disableColor      = disableColor;
    m_shadowDisableColor= shadowDisableColor;
}

void BliderButton::resizeEvent(QResizeEvent *)
{
    if( m_normal.isNull() ) return;
    setSkin( m_normal, m_over, m_down, m_disable, m_focus );
}

void BliderButton::paintEvent ( QPaintEvent * )
{
    QPainter p(this);
    QStyleOptionFocusRect optFocus;

    optFocus.initFrom(this);

    m_mutex.lock();
    if( testAttribute( Qt::WA_OpaquePaintEvent ) && !m_back.isNull() )
        p.drawPixmap( 0, 0, m_back );

    if( isEnabled() )
    {
        if( isCheckable() )
        {
            if( isChecked() )
                p.drawPixmap(0, 0, m_focus);
            else
            {
                if( optFocus.state & QStyle::State_MouseOver )
                    p.drawPixmap(0, 0, m_over);
                else
                    p.drawPixmap(0, 0, m_normal);
            }
            goto l1;
        }
        if( isDown() )
        {
            if( optFocus.state & QStyle::State_MouseOver )
                p.drawPixmap(0, 0, m_down);
            else
                p.drawPixmap(0, 0, m_over);
        }
        else
        {
            if( optFocus.state & QStyle::State_MouseOver )
                p.drawPixmap(0, 0, m_over);
            else if( hasFocus() )
                p.drawPixmap( 0, 0, m_focus);
            else
                p.drawPixmap(0, 0, m_normal);
        }
    }
    else
    {
        p.drawPixmap(0, 0, m_disable);
    }

l1: QString strText = text();
    QRect   rt = rect();

    if( isDown() )
        rt = QRect(0, 0, width(), height());

    if( !m_normalText.isNull() )
    {
        if( isEnabled() )
            p.drawPixmap(rt, m_normalText);
        else
            p.drawPixmap(rt, m_disableText);
    }

    if( !strText.isEmpty() )
    {
        if( isEnabled() )
            draw3dText(&p, rt, strText, m_normalColor, m_shadowNormalColor);
        else
            draw3dText(&p, rt, strText, m_disableColor, m_shadowDisableColor);
    }

    m_mutex.unlock();
}

void BliderButton::SetBackImg( QPixmap pix )
{
    m_mutex.lock();
    m_back = pix;
    m_mutex.unlock();
}
