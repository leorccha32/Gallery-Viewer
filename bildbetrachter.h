#ifndef BILDBETRACHTER_H
#define BILDBETRACHTER_H

#include <QScrollArea>

class QTextEdit;
class QLabel;
class QPaintEvent;
class BilderApp;

class Bildbetrachter : public QScrollArea
{
 public:
    Bildbetrachter ( QWidget* pParentWnd );
    ~Bildbetrachter();

    void onSelectedImage( QString strFileName );
    void onSelectedImage( QImage& imgSelectedItem );

 private:
    QString         m_strFilePath;
    QImage          m_imgItem;
    QTextEdit*      m_pTxtFTPComment;
    QTextEdit*      m_pTxtFileComment;
    QLabel*         m_pLblImage;
    BilderApp*      m_pApp;
};

#endif // BILDBETRACHTER_H
