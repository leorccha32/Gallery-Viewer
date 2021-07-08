#ifndef BILDERAPP_H
#define BILDERAPP_H

#include <QApplication>

class BilderApp : public QApplication
{
public:
    enum eBuilderView{
        eStandard       = 0x00,
        eOriginal       = 0x01,
        ePhotoStandard  = 0x02,
        ePhotoOriginal  = 0x03
    };

    BilderApp( int argc, char** argv );
    ~BilderApp();

    bool initConfig(QWidget* pParentWnd);

    void setOptionMode( eBuilderView nOptMode = eStandard );
    void setFtpServer( QString&, int );
    void setFtpAccount( QString&, QString& );
    void setBaseUrl( QString& );
    void setEditerName( QString& );
    void setAdminPasswd( QString& );
    void setMaxRatio( int nWidth, int nHeight );
    void setMaxThumbRatio( int nWidth, int nHeight );
    void setComment( QString strComment );

    eBuilderView     getOptMode();
    QString getFtpServerIP();
    int     getFtpPort();
    QString getFtpUserName();
    QString getFtpUserPasswd();
    QString getBaseUrl();
    QString getEditerName();
    QString getAdminPasswd();
    QString getComment();

    int     getMaxWidth() { return m_nMaxImageWidth; }
    int     getMaxHeight() { return m_nMaxImageHeight; }

    int     getMaxThumbWidth() { return m_nThumbImageWidth; }
    int     getMaxThumbHeight() { return m_nThumbImageHeight; }

    bool    isFtpConnected() { return m_bFtpConnected; }
    void    setFtpConnected( bool bFtpConnected ){ m_bFtpConnected = bFtpConnected; }

    bool    isOverwrie(){ return m_bOverwrite; }
    void    setOverwrie( bool bOverwrite ){ m_bOverwrite = bOverwrite; }

    bool    isAlwaysTopWindow(){ return m_bAlwaysTopWindow; }
    void    setAlwaysTopWindow( bool bAlwaysTopWindow ){ m_bAlwaysTopWindow = bAlwaysTopWindow; }

    QString getStandardPath() { return m_strStandardPath; }
    void    setStandardPath( QString strImagePath ) { m_strStandardPath = strImagePath; }

    QString getCameraImportPath() { return m_strCameraImportPath; }
    void    setCameraImportPath( QString strImagePath ) { m_strCameraImportPath = strImagePath; }

    bool    isAutoCameraImport() { return m_bAutoCameraImport; }
    void    setAutoCameraImport( bool bAutoCameraImport ){ m_bAutoCameraImport = bAutoCameraImport; }

private:
    bool    m_bFtpConnected;
    eBuilderView m_eOptMode;
    bool    m_bOverwrite;
    bool    m_bAlwaysTopWindow;
    bool    m_bAutoCameraImport;

    QString m_strServerIp;
    int     m_nFtpPort;

    QString m_strComment;
    QString m_strUserName;
    QString m_strUserPasswd;
    QString m_strBaseUrl;
    QString m_strEditerName;
    QString m_strAdminPasswd;
    int     m_nMaxImageWidth;
    int     m_nMaxImageHeight;
    int     m_nThumbImageWidth;
    int     m_nThumbImageHeight;

    QString m_strStandardPath;
    QString m_strCameraImportPath;
};

#endif // BILDERAPP_H
