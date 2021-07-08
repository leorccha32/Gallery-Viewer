#ifndef RESOURCE_H
#define RESOURCE_H

#include <QFile>
#include <QString>
#include <QPainter>

#define	DEFAULT_BUTTON_FONT_NAME    tr("Arial")

#define IDP_MAIN_BK_IMG             QObject::tr(":/res/back.png")
#define IDP_ICON                    QObject::tr(":/res/icon2.png")
#define IDP_RESET                   QObject::tr(":/res/reset.png")
#define IDP_IMAGE                   QObject::tr(":/res/image.png")
#define IDP_FTP                     QObject::tr(":/res/ftp.png")

#define IDS_QSS_COMMONCTRL          QObject::tr(":/qss/CommonControl.qss")

#define GREEN_BTN_COLOR             QColor(0x0F, 0xAF, 0x0F)
#define BLUE_BTN_COLOR              QColor(0x0F, 0xBF, 0xAF)
#define TEXT_COLOR                  QColor(39, 40, 43)
#define TEXT_SHADOW_COLOR           QColor(220, 220, 220)

inline QString GetQssContentFromFile( QString strFileName )
{
    QString qss = "";
    QFile fQss( strFileName );
    fQss.open( QFile::ReadOnly );

    if( fQss.isOpen() )
    {
        qss = QLatin1String( fQss.readAll() );
        fQss.close();
    }

    return qss;
}

static inline void draw3dText(QPainter* p, QRect rt, QString text,
                              QColor textColor = Qt::black, QColor shadowColor = TEXT_SHADOW_COLOR,
                              int flags = Qt::AlignCenter, int lineWidth = 1, int delta = 1 )
{
    QRect rtShadow = rt;

    rtShadow.translate(delta, delta);

    p->setPen(QPen(shadowColor, lineWidth));
    p->drawText(rtShadow, flags, text);

    p->setPen(QPen(textColor, lineWidth));
    p->drawText(rt, flags, text);

    p->setPen(QPen(Qt::white, lineWidth));
    p->drawText(QPoint(rt.width()/4 + 15, 25), text);
}

static inline void draw3dText(QPainter* p, int x, int y, int w, int h, QString text,
                              QColor textColor = Qt::black, QColor shadowColor = TEXT_SHADOW_COLOR,
                              int flags = Qt::AlignCenter, int lineWidth = 1, int delta = 1 )
{
    p->setPen(QPen(shadowColor, lineWidth));
    p->drawText(x+delta, y+delta, w, h, flags, text);

    p->setPen(QPen(textColor, lineWidth));
    p->drawText(x, y, w, h, flags, text);
}

inline void draw3dRect(QPainter* p, QRect rt,
                              QColor midLight = Qt::lightGray, QColor midDark = Qt::darkGray,
                              QColor light = Qt::white, QColor dark = Qt::black,
                              int lineWidth = 1 )
{
    int x1 = rt.left();
    int y1 = rt.top();
    int x2 = rt.right();
    int y2 = rt.bottom();

    p->setPen(QPen(midDark, lineWidth));
    p->drawLine(x1, y1, x2, x1);
    p->drawLine(x1, y1, x1, y2);

    p->setPen(QPen(dark, lineWidth));
    p->drawLine(x1+1, y1+1, x2-1, y1+1);
    p->drawLine(x1+1, y1+1, x1+1, y2);

    p->setPen(QPen(midLight, lineWidth));
    p->drawLine(x1+1, y2-1, x2-1, y2-1);
    p->drawLine(x2-1, y1+1, x2-1, y2-1);

    p->setPen(QPen(light, lineWidth));
    p->drawLine(x1, y2, x2, y2);
    p->drawLine(x2, y1, x2, y2);

}

static inline void draw3dRect(QPainter* p, int x, int y, int w, int h,
                              QColor midLight = Qt::lightGray, QColor midDark = Qt::darkGray,
                              QColor light = Qt::white, QColor dark = Qt::black,
                              int lineWidth = 1 )
{
    int x1 = x;
    int y1 = y;
    int x2 = x+w-1;
    int y2 = y+h-1;

    p->setPen(QPen(midDark, lineWidth));
    p->drawLine(x1, y1, x2, x1);
    p->drawLine(x1, y1, x1, y2);

    p->setPen(QPen(dark, lineWidth));
    p->drawLine(x1+1, y1+1, x2-1, y1+1);
    p->drawLine(x1+1, y1+1, x1+1, y2);

    p->setPen(QPen(midLight, lineWidth));
    p->drawLine(x1+1, y2-1, x2-1, y2-1);
    p->drawLine(x2-1, y1+1, x2-1, y2-1);

    p->setPen(QPen(light, lineWidth));
    p->drawLine(x1, y2, x2, y2);
    p->drawLine(x2, y1, x2, y2);
}

#endif // RESOURCE_H
