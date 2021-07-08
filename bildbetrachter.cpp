#include <QTextEdit>
#include <QLabel>
#include <QClipboard>
#include <QPaintEvent>
#include <QPainter>
#include <QIcon>
#include <QDate>
#include <QImageReader>

#include "bilderapp.h"
#include "bilderfile.h"
#include "bildbetrachter.h"
#include "resource.h"

Bildbetrachter::Bildbetrachter( QWidget* pParentWnd )
 : QScrollArea( pParentWnd )
{
    m_pApp = (BilderApp*)qApp;
    m_pTxtFTPComment = new QTextEdit( pParentWnd );
    m_pTxtFTPComment->setGeometry( 16, 420, 400, 44 );
    m_pTxtFTPComment->setStyleSheet( "background-color: white;" );
    m_pTxtFTPComment->setReadOnly(true);
    m_pTxtFTPComment->setVisible(false);

    m_pTxtFileComment = new QTextEdit( pParentWnd );
    m_pTxtFileComment->setGeometry( 16, 806, 400, 36 );
    m_pTxtFileComment->setStyleSheet( "background-color: white;" );
    m_pTxtFileComment->setReadOnly(true);
    m_pTxtFileComment->setVisible(false);

    m_pLblImage  = new QLabel( pParentWnd );
    m_pLblImage->setStyleSheet( "background-color:black" );
    m_pLblImage->setBackgroundRole(QPalette::Base);
    m_pLblImage->setScaledContents(true);
    m_pLblImage->setPixmap(QPixmap::fromImage( QImage( IDP_MAIN_BK_IMG ) ));
    m_pLblImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_pLblImage->adjustSize();

    setWidgetResizable(true);
    setBackgroundRole(QPalette::Dark);
    setWidget(m_pLblImage);
    setVisible(true);
    setGeometry( 16, 420, 400, 416 );

    QString strQssContent;
    strQssContent += GetQssContentFromFile( IDS_QSS_COMMONCTRL );
    setStyleSheet( strQssContent );
    m_strFilePath   = "";
}

Bildbetrachter::~Bildbetrachter()
{

}

void
Bildbetrachter::onSelectedImage( QString strFileName )
{
    BilderFile file(strFileName);

    if( file.exists() )
    {
        QImageReader reader(strFileName);
        reader.setAutoTransform(true);
        m_imgItem = reader.read();
        setWidgetResizable(true);

        if( m_pApp->getOptMode() == BilderApp::eStandard ||
            m_pApp->getOptMode() == BilderApp::ePhotoStandard )
        {
            m_pLblImage->setScaledContents(false);

            int nW = m_imgItem.width();
            int nH = m_imgItem.height();

            if( nW > nH )
            {
                float nRateH = nH * 1.0f /nW;
                m_pLblImage->setPixmap(QPixmap::fromImage(m_imgItem).scaled( 398, (int)(398*nRateH) ));
            }
            else
            {
                float nRateW = nW * 1.0f /nH;
                m_pLblImage->setPixmap(QPixmap::fromImage(m_imgItem).scaled( (int)345*nRateW, 345 ));
            }

            m_pLblImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            m_pLblImage->setAlignment( Qt::AlignCenter );
        }
        else if( m_pApp->getOptMode() == BilderApp::eOriginal ||
                 m_pApp->getOptMode() == BilderApp::ePhotoOriginal )
        {
            m_pLblImage->setPixmap(QPixmap::fromImage( QImage( m_imgItem ) ));
            m_pLblImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        }

        m_strFilePath   = strFileName;
        m_pLblImage->adjustSize();
        m_pTxtFTPComment->setVisible(true);
        m_pTxtFileComment->setVisible(true);
        m_pTxtFileComment->setText( m_strFilePath );

        QStringList lstFileSection = strFileName.split( "/" );
        strFileName = lstFileSection.at( lstFileSection.size() - 1 );

        QString strServerAddr = m_pApp->getFtpServerIP();
        if( strServerAddr.left(4) == "ftp." )
            strServerAddr = "www." + m_pApp->getFtpServerIP().mid(4);

        int nY = 0, nM = 0, nD = 0;
        QDate::currentDate().getDate( &nY, &nM, &nD );
        QString strFtpCurrentDateDir = QString("%1-%2").arg(nY)
                                                       .arg( nM > 9 ? QString("%1").arg(nM) : "0" + QString("%1").arg(nM) );

        m_pTxtFTPComment->setText( "http://" + strServerAddr + "/" +
                                   m_pApp->getBaseUrl() + "/" +
                                   m_pApp->getEditerName() + "/" +
                                   strFtpCurrentDateDir + "/" + strFileName );

        qApp->clipboard()->setText( "http://" + strServerAddr + "/" +
                                    m_pApp->getBaseUrl() + "/" +
                                    m_pApp->getEditerName() + "/" +
                                    strFtpCurrentDateDir + "/" + strFileName );

        setGeometry( 16, 462, 400, 346 );
        setVisible( true );
    }
    else
    {
        m_strFilePath   = "";
        m_imgItem = QImage( IDP_MAIN_BK_IMG );
        setWidgetResizable(false);

         m_pTxtFTPComment->setVisible(false);
        m_pTxtFileComment->setVisible(false);
        m_pTxtFileComment->setText( m_strFilePath );

        m_pLblImage->setPixmap(QPixmap::fromImage( QImage( m_imgItem ) ));
        m_pLblImage->setScaledContents(true);
        m_pLblImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        m_pLblImage->adjustSize();

        setWidgetResizable(true);
        setGeometry( 16, 420, 400, 416 );
    }
}

void
Bildbetrachter::onSelectedImage( QImage& imgItem )
{
    if( !imgItem.isNull() )
    {
        m_imgItem = imgItem;
        m_pLblImage->setPixmap(QPixmap::fromImage( QImage( m_imgItem ) ));
        m_pLblImage->adjustSize();

        if( m_pApp->getOptMode() == BilderApp::eStandard ||
            m_pApp->getOptMode() == BilderApp::ePhotoStandard )
        {
            m_pLblImage->setScaledContents(false);

            int nW = m_imgItem.width();
            int nH = m_imgItem.height();

            if( nW > nH )
            {
                float nRateH = nH * 1.0f /nW;
                m_pLblImage->setPixmap(QPixmap::fromImage(m_imgItem).scaled( 398, (int)(398*nRateH) ));
            }
            else
            {
                float nRateW = nW * 1.0f /nH;
                m_pLblImage->setPixmap(QPixmap::fromImage(m_imgItem).scaled( (int)345*nRateW, 345 ));
            }

            m_pLblImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            m_pLblImage->setAlignment( Qt::AlignCenter );
        }
        else if( m_pApp->getOptMode() == BilderApp::eOriginal ||
                 m_pApp->getOptMode() == BilderApp::ePhotoOriginal )
        {
            m_pLblImage->setPixmap(QPixmap::fromImage( QImage( m_imgItem ) ));
            m_pLblImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        }

        m_pTxtFTPComment->setVisible(true);
        m_pTxtFileComment->setVisible(true);
        m_pTxtFileComment->setText( m_strFilePath );
        setGeometry( 16, 462, 400, 346 );
        setVisible( true );
    }
}
