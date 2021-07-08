#ifndef BILDERFTPDLG_H
#define BILDERFTPDLG_H

#include <QDialog>
#include <QTabWidget>
#include "qurlinfo.h"

class QCheckBox;
class QLabel;
class QTableView;
class QLineEdit;
class QFtp;
class BilderApp;
class Bilderuploader;

class BilderFtpDlg : public QDialog
{
    Q_OBJECT

public:
    explicit BilderFtpDlg( QWidget* pParentWnd = 0 );
    ~BilderFtpDlg();

    void initWnd();
    void initLoginTab();
    void initAdminTab();

    bool isAlwaysTopWindow();

signals:
    void updateMainUI();

private slots:
    void onLogin();
    void onStandardPathInfo();
    void onCameraImportPath();
    void onLoadAccountInfo();
    void onSaveAccountInfo();
    void onAlwaysTopWindow();
    void onAutoCopyImagesFromCamera();

private:
    bool loadAccountInfo( QString strFileName = "config.dat" );
    bool saveAccountInfo();
    bool saveConfigPath( QString strFilePath );

private:
    Bilderuploader* m_pMainWnd;

//////////////// FTP Class /////////////////
    BilderApp*  m_pApp;

    QString m_strComment;
    QString m_strFtpServerIp;
    int     m_nFtpServerPort;
    QString m_strFtpUserName;
    QString m_strFtpUserPasswd;
    QString m_strEditerName;
    QString m_strAdminPasswd;
    QString m_strBaseUrl;
    int     m_nMaxWidth;
    int     m_nMaxHeight;
    bool    m_bAlwaysTopWindow;
    QString m_strStandardPath;
    QString m_strCameraImportPath;
    bool    m_bAutoCameraImport;
    int     m_nThumbImageWidth;
    int     m_nThumbImageHeight;

////////////////  Main UI Tab //////////////////

    QTabWidget* m_pTabWnd;
    QWidget*    m_pDlgLogin;
    QWidget*    m_pDlgAdmin;

///////////////// Login Tab Part /////////////////
    QPushButton*    m_pBtnStandardPath;
    QPushButton*    m_pBtnLoadSetupFile;
    QPushButton*    m_pBtnImportFromCamera;
    QCheckBox*      m_pChkWindowsAlwaysInForeground;
    QCheckBox*      m_pChkAutoImportFromCamera;
    QLabel*         m_plblPasswd;
    QLineEdit*      m_pTxtPasswd;
    QPushButton*    m_pBtnLogin;
//////////////////////////////////////////////////

///////////// Administration Tab Part ////////////
    QLabel*         m_pLblKommentar;
    QLineEdit*      m_pTxtComment;
    QLabel*         m_pLblFtpServer;
    QLineEdit*      m_pTxtFtpServer;
    QLabel*         m_pLblFtpUser;
    QLineEdit*      m_pTxtFtpUser;
    QLabel*         m_pLblFtpPasswd;
    QLineEdit*      m_pTxtFtpPasswd;
    QLabel*         m_pLblBaseURL;
    QLineEdit*      m_pTxtBaseURL;
    QLabel*         m_pLblBearbeiterName;
    QLineEdit*      m_pTxtBearbeiterName;
    QLabel*         m_pLblAdminPasswd;
    QLineEdit*      m_pTxtAdminPasswd;
    QCheckBox*      m_pChkEntfemen;
    QLabel*         m_pLblMaxBreite;
    QLineEdit*      m_pTxtMaxBreite;
    QLabel*         m_pLblMaxHohe;
    QLineEdit*      m_pTxtMaxHohe;
    QLabel*         m_pLblDaumenBreite;
    QLineEdit*      m_pTxtDaumenBreite;
    QLabel*         m_pLblDaumenHohe;
    QLineEdit*      m_pTxtDaumenHohe;
    QPushButton*    m_pBtnSaveSteupFiles;
//////////////////////////////////////////////////
};

#endif // BILDERFTPDLG_H
