#include <QCheckBox>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItem>
#include <QTableView>
#include <QLineEdit>
#include <QTextStream>
#include "bilderftpdlg.h"
#include "bilderapp.h"
#include "bilderfile.h"
#include "bilderuploader.h"
#include "resource.h"
#include "qftp.h"

#define DLG_WIDTH   480
#define DLG_HEIGHT  485

BilderFtpDlg::BilderFtpDlg( QWidget* pParentWnd )
    : QDialog( pParentWnd )
{
    m_pApp = (BilderApp*)qApp;
    m_pMainWnd  = (Bilderuploader*)pParentWnd;

    initWnd();
    loadAccountInfo();

    connect( this, SIGNAL(updateMainUI()), m_pMainWnd, SLOT(onUpdateItem()) );
}

BilderFtpDlg::~BilderFtpDlg()
{

}

void
BilderFtpDlg::initWnd()
{
    m_pTabWnd = new QTabWidget( this );
    m_pTabWnd->setGeometry( 10, 10, DLG_WIDTH - 20, DLG_HEIGHT - 20 );

    initLoginTab();
    initAdminTab();

    QString strQssContent;
    strQssContent += GetQssContentFromFile( IDS_QSS_COMMONCTRL );
    setStyleSheet( strQssContent );

    setWindowTitle("Einstellungen");
    setWindowIcon( QIcon( IDP_FTP ) );
    setWindowFlags(Qt::Dialog |
                   Qt::WindowMinimizeButtonHint |
                   Qt::WindowCloseButtonHint |
                   Qt::MSWindowsFixedSizeDialogHint |
                   Qt::WindowStaysOnTopHint );

    setFixedSize( DLG_WIDTH, DLG_HEIGHT );
    setMaximumSize( DLG_WIDTH, DLG_HEIGHT );
}

void
BilderFtpDlg::initLoginTab()
{
    m_pDlgLogin = new QWidget;
    m_pTabWnd->addTab( m_pDlgLogin, "Login" );

    ////////////////////////////////////// LoginTab's Controls Initialize /////////////////////////////////////////
    m_pBtnStandardPath      = new QPushButton( "Standardpfad für Bilder...", m_pDlgLogin );
    m_pBtnStandardPath->setGeometry( 108, 30, 240, 26 );
    m_pBtnStandardPath->setStyleSheet("background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 #fafafa, stop:1 #ccc, stop:2 #d8d8d8);}");

    m_pBtnLoadSetupFile     = new QPushButton( "Einstellungsdatei laden...", m_pDlgLogin );
    m_pBtnLoadSetupFile->setGeometry( 108, 80, 240, 26 );
    m_pBtnLoadSetupFile->setStyleSheet("background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 #fafafa, stop:1 #ccc, stop:2 #d8d8d8);}");

    m_pBtnImportFromCamera  = new QPushButton( "Standardpfad für Bildimport...", m_pDlgLogin );
    m_pBtnImportFromCamera->setGeometry( 108, 130, 240, 26 );
    m_pBtnImportFromCamera->setStyleSheet("background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 #fafafa, stop:1 #ccc, stop:2 #d8d8d8);}");

    m_pChkWindowsAlwaysInForeground = new QCheckBox( "Fenster immer im Vordergrund", m_pDlgLogin );
    m_pChkWindowsAlwaysInForeground->setGeometry( 121, 185, 240, 26 );

    m_pChkAutoImportFromCamera      = new QCheckBox( "Bilder automatisch importieren", m_pDlgLogin );
    m_pChkAutoImportFromCamera->setGeometry( 121, 225, 240, 26 );

    m_plblPasswd    = new QLabel( "Password:", m_pDlgLogin );
    m_plblPasswd->setGeometry( 108, 275, 240, 26 );

    m_pTxtPasswd    = new QLineEdit( "", m_pDlgLogin );
    m_pTxtPasswd->setGeometry(108, 305, 240, 25);
    m_pTxtPasswd->setEchoMode( QLineEdit::Password );

    m_pBtnLogin     = new QPushButton( "Login", m_pDlgLogin );
    m_pBtnLogin->setGeometry( 248, 380, 100, 26 );
    m_pBtnLogin->setStyleSheet("background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 #fafafa, stop:1 #ccc, stop:2 #d8d8d8);}");

    connect( m_pBtnLogin, SIGNAL(clicked()), SLOT(onLogin()) );
    connect( m_pBtnStandardPath, SIGNAL(clicked()), SLOT(onStandardPathInfo()) );
    connect( m_pBtnLoadSetupFile, SIGNAL(clicked()), SLOT(onLoadAccountInfo()) );
    connect( m_pBtnImportFromCamera, SIGNAL(clicked()), SLOT(onCameraImportPath()) );
    connect( m_pChkWindowsAlwaysInForeground, SIGNAL(clicked()), SLOT(onAlwaysTopWindow()) );
    connect( m_pChkAutoImportFromCamera, SIGNAL(clicked()), SLOT(onAutoCopyImagesFromCamera()) );
    ///////////////////////////////////////////////////////////////////////////////
}

void
BilderFtpDlg::initAdminTab()
{
    //////////////////////////////////// AdminTab' Controls Intialize ///////////////////////////////////////////

    m_pDlgAdmin = new QWidget;

    QString fileName = "config.dat";
    BilderFile fConfig( fileName );
    bool bAddAdminTab = false;

    if( fConfig.exists() )
    {
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

            BilderFile file( fileName );

            if( !file.exists() ) bAddAdminTab = true;

        }
        else bAddAdminTab = true;

        fConfig.close();
    }
    else bAddAdminTab = true;

    if( bAddAdminTab ) m_pTabWnd->addTab( m_pDlgAdmin, "Administrator" );
    m_pLblKommentar     = new QLabel( "Kommentar:", m_pDlgAdmin );
    m_pLblKommentar->setGeometry( 20, 20, 125, 25 );

    m_pTxtComment       = new QLineEdit( "", m_pDlgAdmin );
    m_pTxtComment->setGeometry( 140, 20, 300, 25 );

    m_pLblFtpServer     = new QLabel( "FTP-Server:", m_pDlgAdmin );
    m_pLblFtpServer->setGeometry( 20, 55, 125, 25 );

    m_pTxtFtpServer     = new QLineEdit( "", m_pDlgAdmin );
    m_pTxtFtpServer->setGeometry( 140, 55, 300, 25 );

    m_pLblFtpUser       = new QLabel( "FTP-User:", m_pDlgAdmin );
    m_pLblFtpUser->setGeometry( 20, 90, 125, 25 );

    m_pTxtFtpUser       = new QLineEdit( "", m_pDlgAdmin );
    m_pTxtFtpUser->setGeometry( 140, 90, 300, 25 );

    m_pLblFtpPasswd     = new QLabel( "FTP-Password", m_pDlgAdmin );
    m_pLblFtpPasswd->setGeometry( 20, 125, 125, 25 );

    m_pTxtFtpPasswd     = new QLineEdit( "", m_pDlgAdmin );
    m_pTxtFtpPasswd->setGeometry( 140, 125, 300, 25 );
    m_pTxtFtpPasswd->setEchoMode( QLineEdit::Password );

    m_pLblBaseURL       = new QLabel( "Base-URL", m_pDlgAdmin );
    m_pLblBaseURL->setGeometry( 20, 160, 125, 25 );

    m_pTxtBaseURL       = new QLineEdit( "", m_pDlgAdmin );
    m_pTxtBaseURL->setGeometry( 140, 160, 300, 25 );

    m_pLblBearbeiterName    = new QLabel( "Bearbeiter-Name", m_pDlgAdmin );
    m_pLblBearbeiterName->setGeometry( 20, 195, 125, 25 );

    m_pTxtBearbeiterName    = new QLineEdit( "", m_pDlgAdmin );
    m_pTxtBearbeiterName->setGeometry( 140, 195, 300, 25 );

    m_pLblAdminPasswd   = new QLabel( "Admin-Password", m_pDlgAdmin );
    m_pLblAdminPasswd->setGeometry( 20, 230, 125, 25 );

    m_pTxtAdminPasswd   = new QLineEdit( "", m_pDlgAdmin );
    m_pTxtAdminPasswd->setGeometry( 140, 230, 165, 25 );
    m_pTxtAdminPasswd->setEchoMode( QLineEdit::Password );

    m_pChkEntfemen      = new QCheckBox( "Entfemen", m_pDlgAdmin );
    m_pChkEntfemen->setGeometry( 320, 230, 125, 25 );

    m_pLblMaxBreite     = new QLabel( "max.Breite", m_pDlgAdmin );
    m_pLblMaxBreite->setGeometry( 20, 265, 125, 25 );

    m_pTxtMaxBreite     = new QLineEdit( "", m_pDlgAdmin );
    m_pTxtMaxBreite->setGeometry( 140, 265, 165, 25 );

    m_pLblMaxHohe       = new QLabel( "max.Höhe", m_pDlgAdmin );
    m_pLblMaxHohe->setGeometry( 20, 300, 125, 25 );

    m_pTxtMaxHohe       = new QLineEdit( "", m_pDlgAdmin );
    m_pTxtMaxHohe->setGeometry( 140, 300, 165, 25 );

    m_pLblDaumenBreite  = new QLabel( "Daumen Breite", m_pDlgAdmin );
    m_pLblDaumenBreite->setGeometry( 20, 335, 125, 25 );

    m_pTxtDaumenBreite  = new QLineEdit( "", m_pDlgAdmin );
    m_pTxtDaumenBreite->setGeometry( 140, 335, 165, 25 );

    m_pLblDaumenHohe    = new QLabel( "Daumen Höhe", m_pDlgAdmin );
    m_pLblDaumenHohe->setGeometry( 20, 370, 125, 25 );

    m_pTxtDaumenHohe    = new QLineEdit( "", m_pDlgAdmin );
    m_pTxtDaumenHohe->setGeometry( 140, 370, 165, 25 );

    m_pBtnSaveSteupFiles= new QPushButton( "Einstellungsdatei speichem...", m_pDlgAdmin );
    m_pBtnSaveSteupFiles->setGeometry( 240, 405, 200, 25 );
    m_pBtnSaveSteupFiles->setStyleSheet("background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 #fafafa, stop:1 #ccc, stop:2 #d8d8d8);}");

    connect( m_pBtnSaveSteupFiles, SIGNAL(clicked()), SLOT(onSaveAccountInfo()) );
    /////////////////////////////////////////////////////////////////////////////////////////////
}

bool
BilderFtpDlg::isAlwaysTopWindow()
{
    return m_pChkWindowsAlwaysInForeground->isChecked();
}

void
BilderFtpDlg::onLogin()
{
    loadAccountInfo();

    if( m_strAdminPasswd == m_pTxtPasswd->text() )
    {
        if( m_pChkWindowsAlwaysInForeground->isChecked())
        {
            m_pMainWnd->setWindowFlags( Qt::Window | Qt::WindowStaysOnTopHint );
            m_pMainWnd->show();
        }

        m_pTabWnd->addTab( m_pDlgAdmin, "Administrator" );
    }
    else
        QMessageBox::information( this, tr("Error"), tr("Don't be matched password.") );
#if 0
    m_pMainWnd->onConnectFtp();
#endif
}

void
BilderFtpDlg::onStandardPathInfo()
{
    QString strDirPath = QFileDialog::getExistingDirectory(this, tr("P[em dorectpru"), m_pApp->getStandardPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    m_pApp->setStandardPath( strDirPath );
    saveAccountInfo();
}

void
BilderFtpDlg::onCameraImportPath()
{
    QString strDirPath = QFileDialog::getExistingDirectory(this, tr("P[em dorectpru"), m_pApp->getCameraImportPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    m_pApp->setCameraImportPath( strDirPath );
    saveAccountInfo();
}

void
BilderFtpDlg::onLoadAccountInfo()
{
    QString strAccountFileInfo = QFileDialog::getOpenFileName(this, tr("Bitte wählen Sie die Setup-Datei"), "",
                                                      tr("Setup (*.usp);; Alle Dateien (*.*)"));

    if( strAccountFileInfo == "" ) return;

    saveConfigPath( strAccountFileInfo );

    if( loadAccountInfo() )
    {
        m_pApp->setComment( m_strComment );
        m_pApp->setFtpServer( m_strFtpServerIp, m_nFtpServerPort );
        m_pApp->setFtpAccount( m_strFtpUserName, m_strFtpUserPasswd );
        m_pApp->setBaseUrl( m_strBaseUrl );
        m_pApp->setEditerName( m_strEditerName );
        m_pApp->setAdminPasswd( m_strAdminPasswd );
        m_pApp->setAlwaysTopWindow( m_bAlwaysTopWindow );
        m_pApp->setMaxRatio( m_nMaxWidth, m_nMaxHeight );
        m_pApp->setStandardPath( m_strStandardPath );
        m_pApp->setCameraImportPath( m_strCameraImportPath );

        emit updateMainUI();
        QMessageBox::information( this, tr("User Account"), tr("Loaded account information successfully") );
    }
}

void
BilderFtpDlg::onSaveAccountInfo()
{
    QString strComment;
    QString strFtpServer;
    QString strFtpUser;
    QString strFtpPasswd;
    QString strBaseUrl;
    QString strEditerName;
    QString strAdminPasswd;
    QString strMaxWidth;
    QString strMaxHeight;
    QString strThumbWidth;
    QString strThumbHeight;

    strComment      = m_pTxtComment->text();
    strFtpServer    = m_pTxtFtpServer->text();
    strFtpUser      = m_pTxtFtpUser->text();
    strFtpPasswd    = m_pTxtFtpPasswd->text();
    strBaseUrl      = m_pTxtBaseURL->text();
    strEditerName   = m_pTxtBearbeiterName->text();
    strAdminPasswd  = m_pTxtAdminPasswd->text();
    strMaxWidth     = m_pTxtMaxBreite->text();
    strMaxHeight    = m_pTxtMaxHohe->text();
    strThumbWidth   = m_pTxtDaumenBreite->text();
    strThumbHeight  = m_pTxtDaumenHohe->text();

    if( strMaxWidth.toInt() < 10 ||
        strMaxHeight.toInt() < 10 )
    {
        QMessageBox::information( this, "Warning", "Please input max width and height correctly." );
        return;
    }

    if( strThumbWidth.toInt() < 10 ||
        strThumbHeight.toInt() < 10 )
    {
        QMessageBox::information( this, "Warning", "Please input thumb width and height correctly." );
        return;
    }


    m_pApp->setComment( strComment );
    m_pApp->setFtpServer( strFtpServer, 22 );
    m_pApp->setFtpAccount( strFtpUser, strFtpPasswd );
    m_pApp->setBaseUrl( strBaseUrl );
    m_pApp->setEditerName( strEditerName );
    m_pApp->setAdminPasswd( strAdminPasswd );
    m_pApp->setMaxRatio( strMaxWidth.toInt(), strMaxHeight.toInt() );
    m_pApp->setMaxThumbRatio( strThumbWidth.toInt(), strThumbHeight.toInt() );

    QFile fConfig( "account.usp" );
    QFileInfo ffInfo(fConfig);

    if( !QFile::exists( "config.dat" ) )
        saveConfigPath( ffInfo.absoluteFilePath() );

    emit updateMainUI();

    if( saveAccountInfo() )
    {
        QMessageBox::information( this, "User Account", "User account was saved successfully." );
    }
}

void
BilderFtpDlg::onAlwaysTopWindow()
{
    if( m_pChkWindowsAlwaysInForeground->isChecked() )
    {
        m_pMainWnd->setWindowFlags( Qt::Window | Qt::WindowStaysOnTopHint );
        m_pApp->setAlwaysTopWindow( true );
    }
    else
    {
        m_pMainWnd->setWindowFlags( Qt::Window );
        m_pApp->setAlwaysTopWindow( false );
    }

    m_pMainWnd->show();
    saveAccountInfo();
}

void
BilderFtpDlg::onAutoCopyImagesFromCamera()
{
    if( m_pChkAutoImportFromCamera->isChecked() )
        m_pApp->setAutoCameraImport( true );
    else
        m_pApp->setAutoCameraImport( false );

    saveAccountInfo();
}

bool
BilderFtpDlg::loadAccountInfo( QString strFileName )
{
    QString fileName = strFileName;
    BilderFile fConfig( fileName );

    if( !fConfig.exists() ){
        QMessageBox::information( m_pMainWnd, tr("User Account"), tr("Not exist account information file!") );
        return false;
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
            QMessageBox::information( m_pMainWnd, tr("User Account"), tr("Can't read account information!") );
            return false;
        }

        fConfig.close();
    }

    BilderFile file( fileName );

    if( !file.exists() )
    {
        QMessageBox::information( this, tr("User Account"), tr("File not exist, Please save account information!") );
        return false;
    }

    if( file.open( QIODevice::ReadOnly ) )
    {
        QString strAccountContent;
        QString strFileContent;
        QTextStream stream( &file );

        while( !stream.atEnd() )
        {
            stream >> strFileContent;
            strAccountContent += strFileContent;
        }

        QStringList lstAccountInfo = strAccountContent.split("#");

        if( lstAccountInfo.size() > 10 )
        {
            m_strComment            = lstAccountInfo.at(0);
            m_strFtpServerIp        = lstAccountInfo.at(1);
            m_nFtpServerPort        = 21;
            m_strFtpUserName        = lstAccountInfo.at(2);
            m_strFtpUserPasswd      = lstAccountInfo.at(3);
            m_strBaseUrl            = lstAccountInfo.at(4);
            m_strEditerName         = lstAccountInfo.at(5);
            m_strAdminPasswd        = lstAccountInfo.at(6);
            m_nMaxWidth             = lstAccountInfo.at(7).toInt();
            m_nMaxHeight            = lstAccountInfo.at(8).toInt();
            m_bAlwaysTopWindow      = lstAccountInfo.at(9).toInt();
            m_strStandardPath       = lstAccountInfo.at(11);
            m_strCameraImportPath   = lstAccountInfo.at(12);
            m_bAutoCameraImport     = lstAccountInfo.at(13).toInt();
            m_nThumbImageWidth      = lstAccountInfo.at(14).toInt();
            m_nThumbImageHeight     = lstAccountInfo.at(15).toInt();

            m_pApp->setOverwrie( lstAccountInfo.at(10).toInt() );

            m_pTxtComment->setText( lstAccountInfo.at(0) );
            m_pTxtFtpServer->setText( m_strFtpServerIp );
            m_pTxtFtpUser->setText( m_strFtpUserName );
            m_pTxtFtpPasswd->setText( m_strFtpUserPasswd );
            m_pTxtBaseURL->setText( m_strBaseUrl );
            m_pTxtBearbeiterName->setText( m_strEditerName );
            m_pTxtAdminPasswd->setText( m_strAdminPasswd );
            m_pTxtMaxBreite->setText( QString("%1").arg(m_nMaxWidth) );
            m_pTxtMaxHohe->setText( QString("%1").arg(m_nMaxHeight) );
            m_pChkWindowsAlwaysInForeground->setChecked( lstAccountInfo.at(9).toInt() );
            m_pChkAutoImportFromCamera->setChecked( lstAccountInfo.at(13).toInt() );
            m_pTxtDaumenBreite->setText( QString("%1").arg(m_nThumbImageWidth) );
            m_pTxtDaumenHohe->setText( QString("%1").arg(m_nThumbImageHeight) );
        }

        if( file.isOpen() )
            file.close();
    }

    return true;
}

bool
BilderFtpDlg::saveAccountInfo()
{
    QString fileName = "config.dat";
    BilderFile fConfig( fileName );

    if( !fConfig.exists() ){
        QMessageBox::information( m_pMainWnd, tr("User Account"), tr("Not exist account information file!") );
        return false;
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
            QMessageBox::information( m_pMainWnd, tr("User Account"), tr("Can't save account information!") );
            return false;
        }

        fConfig.close();
    }

    BilderFile file( fileName );
    if( file.open( QIODevice::WriteOnly ) )
    {
        QString strAlwaysTopWnd = m_pApp->isAlwaysTopWindow() ? "1" : "0";
        QString strOverwrite = m_pApp->isOverwrie() ? "1" : "0";

        QTextStream stream( &file );
        stream << m_pApp->getComment() << "#";
        stream << m_pApp->getFtpServerIP() << "#";
        stream << m_pApp->getFtpUserName() << "#";
        stream << m_pApp->getFtpUserPasswd() << "#";
        stream << m_pApp->getBaseUrl() << "#";
        stream << m_pApp->getEditerName() << "#";
        stream << m_pApp->getAdminPasswd() << "#";
        stream << m_pApp->getMaxWidth() << "#";
        stream << m_pApp->getMaxHeight() << "#";
        stream << strAlwaysTopWnd << "#";
        stream << strOverwrite << "#";
        stream << m_pApp->getStandardPath() << "#";
        stream << m_pApp->getCameraImportPath() << "#";
        stream << m_pApp->isAutoCameraImport() << "#";
        stream << m_pApp->getMaxThumbWidth() << "#";
        stream << m_pApp->getMaxThumbHeight() << endl;

        file.close();
        return true;
    }

    file.close();
    return false;
}

bool
BilderFtpDlg::saveConfigPath( QString strFilePath )
{
    BilderFile file( "config.dat" );
    if( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file );
        stream << strFilePath << endl;
        file.close();
        return true;
    }

    return false;
}
