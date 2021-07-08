#include <QDate>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include "bilderapp.h"
#include "bilderfile.h"

BilderApp::BilderApp(int argc, char **argv)
    : QApplication( argc, argv ),
    m_eOptMode(eStandard),
    m_bFtpConnected ( false )
{
    m_bOverwrite        = false;
    m_bAlwaysTopWindow  = false;

    m_strComment        = "";
    m_strServerIp       = "";
    m_nFtpPort          = 22;
    m_strUserName       = "";
    m_strUserPasswd     = "";
    m_strBaseUrl        = "";
    m_strEditerName     = "";
    m_strAdminPasswd    = "";
    m_nMaxImageWidth    = m_nMaxImageHeight = 0;
    m_strStandardPath   = "";
    m_bAutoCameraImport = false;

    int nY = 0, nM = 0, nD = 0;
    QDate::currentDate().getDate( &nY, &nM, &nD );

    QString strLocalDir = QString("%1_%2_%3")
                                             .arg(nY)
                                             .arg( nM > 9 ? QString("%1").arg(nM) : "0" + QString("%1").arg(nM) )
                                             .arg( nD > 9 ? QString("%1").arg(nD) : "0" + QString("%1").arg(nD) );

    QDir dir;
    if( dir.mkpath( "images/" + strLocalDir ) ) dir = QDir("images/" + strLocalDir);

    m_strCameraImportPath = dir.absolutePath();
}

BilderApp::~BilderApp()
{

}

bool
BilderApp::initConfig( QWidget* pParentWnd )
{
    bool bRetVal = false;

    QString fileName = "config.dat";
    BilderFile fConfig( fileName );

    if( !fConfig.exists() ){
        QMessageBox::information( pParentWnd, tr("User Account"), tr("Please create account information!") );
        return bRetVal;
    }
    else{
        if( fConfig.open( QIODevice::ReadOnly ) )
        {

            QString strReadData;
            QString strFileContent;
            QTextStream stream( &fConfig );

            while( !stream.atEnd() )
            {
                stream >> strFileContent;
                strReadData += strFileContent;
            }

            fileName = strReadData;

        } else {
            QMessageBox::information( pParentWnd, tr("User Account"), tr("Can't read account information!") );
            return false;
        }

        fConfig.close();
    }

    BilderFile fAccount( fileName );

    if( !fAccount.exists() )
    {
        QMessageBox::information( pParentWnd, tr("User Account"), tr("Please create account information!") );
        return bRetVal;
    }

    if( fAccount.open( QIODevice::ReadOnly ) )
    {
        QString strAccountContent;
        QString strFileContent;
        QTextStream stream( &fAccount );

        while( !stream.atEnd() )
        {
            stream >> strFileContent;
            strAccountContent += strFileContent;
        }

        QStringList lstAccountInfo = strAccountContent.split("#");

        if( lstAccountInfo.size() > 11 )
        {
            m_strComment            = lstAccountInfo.at(0);
            m_strServerIp           = lstAccountInfo.at(1);
            m_nFtpPort              = 21;
            m_strUserName           = lstAccountInfo.at(2);
            m_strUserPasswd         = lstAccountInfo.at(3);
            m_strBaseUrl            = lstAccountInfo.at(4);
            m_strEditerName         = lstAccountInfo.at(5);
            m_strAdminPasswd        = lstAccountInfo.at(6);
            m_nMaxImageWidth        = lstAccountInfo.at(7).toInt();
            m_nMaxImageHeight       = lstAccountInfo.at(8).toInt();
            m_bAlwaysTopWindow      = lstAccountInfo.at(9).toInt();
            m_bOverwrite            = lstAccountInfo.at(10).toInt();
            m_strStandardPath       = lstAccountInfo.at(11);
            m_strCameraImportPath   = lstAccountInfo.at(12);
            m_bAutoCameraImport     = lstAccountInfo.at(13).toInt();
            m_nThumbImageWidth      = lstAccountInfo.at(14).toInt();
            m_nThumbImageHeight     = lstAccountInfo.at(15).toInt();
        }

        fAccount.close();
    }

    bRetVal = true;

    return bRetVal;
}

void
BilderApp::setOptionMode( eBuilderView eOptMode )
{
    m_eOptMode = eOptMode;
}

void
BilderApp::setFtpServer( QString& strServerIp, int nFtpPort )
{
    m_strServerIp = strServerIp;
    m_nFtpPort = nFtpPort;

    setFtpConnected( true );
}

void
BilderApp::setFtpAccount( QString& strUserName, QString& strUserPasswd )
{
    m_strUserName = strUserName;
    m_strUserPasswd = strUserPasswd;
}

void
BilderApp::setBaseUrl( QString& strBaseUrl )
{
    m_strBaseUrl = strBaseUrl;
}

void
BilderApp::setEditerName( QString& strEditerName )
{
    m_strEditerName = strEditerName;
}

void
BilderApp::setAdminPasswd( QString& strAdminPasswd )
{
    m_strAdminPasswd = strAdminPasswd;
}

void
BilderApp::setMaxRatio( int nWidth, int nHeight )
{
    m_nMaxImageWidth    = nWidth;
    m_nMaxImageHeight   = nHeight;
}

void
BilderApp::setMaxThumbRatio( int nWidth, int nHeight )
{
    m_nThumbImageWidth  = nWidth;
    m_nThumbImageHeight = nHeight;
}

void
BilderApp::setComment( QString strComment )
{
    m_strComment    = strComment;
}

QString
BilderApp::getFtpServerIP()
{
    return m_strServerIp;
}

int
BilderApp::getFtpPort()
{
    return m_nFtpPort;
}

QString
BilderApp::getFtpUserName()
{
    return m_strUserName;
}

QString
BilderApp::getFtpUserPasswd()
{
    return m_strUserPasswd;
}

QString
BilderApp::getBaseUrl()
{
    return m_strBaseUrl;
}

QString
BilderApp::getEditerName()
{
    return m_strEditerName;
}

QString
BilderApp::getAdminPasswd()
{
    return m_strAdminPasswd;
}

QString
BilderApp::getComment()
{
    return m_strComment;
}

BilderApp::eBuilderView
BilderApp::getOptMode()
{
    return  m_eOptMode;
}
