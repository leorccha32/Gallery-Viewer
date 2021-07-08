#ifndef BLIDERBUTTON_H
#define BLIDERBUTTON_H

#include <QMutex>
#include <QPushButton>

class QPaintEvent;

class BliderButton : public QPushButton
{
public:
    BliderButton(QString title, QWidget* parent = NULL);
    BliderButton(QWidget* parent, QRect r, QString title = QString() );
    BliderButton(QWidget* parent, QPoint pos, QSize size, QString title = QString());
    BliderButton(QWidget* parent, int x, int y, int width, int height, QString title = QString());

    ~BliderButton();

    void setImage( QImage imgButton );

public:
        void setSkin(QString strNormal, QString strOver = QString(), QString strDown = QString(), QString strDisable = QString(), QString strFocus = QString());
        void setSkin(QPixmap normal, QPixmap over, QPixmap down, QPixmap disable, QPixmap focus);

        QColor      normalTextColor() { return m_normalColor; }
        QColor      disableTextColor() { return m_disableColor; }

        void        setTextImage(QString normal, QString disable= QString());
        void        setTextImage(QPixmap normal, QPixmap disable);

        void        setTextColor(QColor textColor, QColor shadowColor);
        void        setTextColor(QColor normalColor, QColor shadowNormalColor,
                                 QColor disableColor, QColor shadowDisableColor);
        void		SetFont( QFont& f ) { m_font = f; }

        void		SetBackImg( QPixmap pix );
signals:

private:
        void init();

protected:
        virtual void paintEvent(QPaintEvent *);
        virtual void resizeEvent(QResizeEvent *);

protected slots:

private:
        bool		m_bChecked;
        QMutex      m_mutex;

        QPixmap     m_normal;
        QPixmap     m_focus;
        QPixmap     m_over;
        QPixmap     m_down;
        QPixmap     m_disable;

        QPixmap     m_normalText;
        QPixmap     m_disableText;

        QPixmap		m_back;

        QColor      m_normalColor;
        QColor      m_disableColor;
        QColor      m_shadowNormalColor;
        QColor      m_shadowDisableColor;

        QFont		m_font;
};

#endif // BLIDERBUTTON_H
