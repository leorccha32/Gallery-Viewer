#include "bilderapp.h"
#include "bilderuploader.h"
#include "bildbetrachter.h"
#include "bilderftpdlg.h"
#include "resource.h"
#include "qftp.h"
#include "qdevicewatcher.h"
#include "bilderuploader.h"
#include "bilderfile.h"

#include <QClipboard>
#include <QScrollArea>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QDir>
#include <QDirIterator>
#include <QImageReader>
#include <QTextStream>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
#include <QMimeData>

Bilderuploader::Bilderuploader(QWidget *parent)
    : QMainWindow(parent)
{
    m_pApp = (BilderApp*)qApp;
    m_nUploadItem       = 0;
    m_nRotateAgnle      = 0;
    m_pDlgFTPUpload     = 0;
    m_nNewFileUpload    = 0;

    m_ftpServer = new QFtp(this);

    if( m_pApp->initConfig( this ) )
        onConnectFtp();

    checkCameras();
    initWnd();

    connect(m_ftpServer, SIGNAL(commandStarted(int)), this, SLOT(ftpCommandStarted(int)));
    connect(m_ftpServer, SIGNAL(commandFinished(int,bool)), SLOT(ftpCommandFinished(int,bool)));
    connect(m_ftpServer, SIGNAL(listInfo(QUrlInfo)), this, SLOT(addToList(QUrlInfo)));
    connect(m_ftpServer, SIGNAL(dataTransferProgress(qint64,qint64)), SLOT(updateDataTransferProgress(qint64,qint64)));
    connect( this, SIGNAL(uploadEvent()), SLOT( onUploadFilesToFtp() ) );
}

Bilderuploader::~Bilderuploader()
{
    if (m_pUsbDeviceMoniter->isRunning()) {
        if (!m_pUsbDeviceMoniter->stop()) {
            m_statusbar->showMessage(tr("Failed to stop"));
        }
    }
}

void
Bilderuploader::initWnd()
{
    initMenuAndStatus();
    initCtrls();

    QString strQssContent;
    strQssContent += GetQssContentFromFile( IDS_QSS_COMMONCTRL );

    setStyleSheet( strQssContent );
    setFixedSize( MAX_WIDTH, MAX_HEIGHT );
    setMaximumSize( MAX_WIDTH, MAX_HEIGHT );
    setWindowIcon( QIcon( IDP_ICON ) );
    setAcceptDrops(true);
    resize(MAX_WIDTH, MAX_HEIGHT);
}

void
Bilderuploader::initMenuAndStatus()
{
    m_menubar   = new QMenuBar(this);
    setMenuBar( m_menubar );

    QMenu* mnFile = this->menuBar()->addMenu( "Datei" );
    QMenu* mnHelp = this->menuBar()->addMenu( "Hilfe" );
    QMenu* mnSetting = this->menuBar()->addMenu( "Einstellugen" );

    QAction* openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open files"));
    connect(openAct, &QAction::triggered, this, &Bilderuploader::onOpenFile);

    QAction* closeAct = new QAction(tr("&Close"), this);
    closeAct->setShortcuts(QKeySequence::Close);
    closeAct->setStatusTip(tr("Close application"));
    connect(closeAct, &QAction::triggered, this, &Bilderuploader::onCloseApp);

    QAction* helpAct = new QAction(tr("&Help"), this);
    helpAct->setShortcuts(QKeySequence::HelpContents);
    helpAct->setStatusTip(tr("Help"));
    connect(helpAct, &QAction::triggered, this, &Bilderuploader::onOpenHelp);

    QAction* aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("About"));
    connect(aboutAct, &QAction::triggered, this, &Bilderuploader::onOpenAbout);

    QAction* settingAct = new QAction(tr("&Einstellugen"), this);
    settingAct->setStatusTip(tr("Einstellugen"));
    connect(settingAct, &QAction::triggered, this, &Bilderuploader::onOpenFtpDialog);

    mnFile->addAction( openAct );
    mnFile->addAction( closeAct );
    mnHelp->addAction( helpAct );
    mnHelp->addAction( aboutAct );
    mnSetting->addAction( settingAct );

    m_statusbar     = new QStatusBar(this);
    setStatusBar(m_statusbar);
}

void
Bilderuploader::initCtrls()
{
    m_pBackPanel    = new QWidget(this);
    m_pBackPanel->setGeometry( 0, m_menubar->height()-10, MAX_WIDTH, MAX_HEIGHT-10-m_statusbar->height() );
    m_pBackPanel->setStyleSheet("background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 #7a7a7a, stop:1 #d8d8d8);}");

    QString strQssContent;
    strQssContent += GetQssContentFromFile( IDS_QSS_COMMONCTRL );

    m_pBtnAddImages     = new QPushButton( "Bilder hinzufügen", m_pBackPanel );
    m_pBtnAddImages->setGeometry( 16, 20, 152, 26 );
    m_pBtnAddImages->setStyleSheet( strQssContent );

    m_pBtnFtpUpload     = new QPushButton( "Bilder uploaden", m_pBackPanel );
    m_pBtnFtpUpload->setGeometry( 16, 56, 152, 26 );
    m_pBtnFtpUpload->setStyleSheet( strQssContent );
    m_pBtnFtpUpload->setDisabled(true);

    m_pBtnReset         = new QPushButton( "Reset", m_pBackPanel );
    m_pBtnReset->setGeometry( 16, 92, 152, 26 );
    m_pBtnReset->setStyleSheet( strQssContent );

    QFrame* line = new QFrame( m_pBackPanel );
    line->setGeometry(16, 132, 152, 1);
    line->setStyleSheet("background-color: #555;");

    m_pRBStandardImage      = new QRadioButton( "Standard", m_pBackPanel );
    m_pRBStandardImage->setStyleSheet("background: transparent;");
    m_pRBStandardImage->setGeometry( 16, 150, 140, 22 );
    m_pRBStandardImage->setChecked(true);

    m_pRBOriginalImage      = new QRadioButton( "Originalbild", m_pBackPanel );
    m_pRBOriginalImage->setStyleSheet("background: transparent;");
    m_pRBOriginalImage->setGeometry( 16, 180, 140, 22 );

    m_pRBOriginPhotoImage   = new QRadioButton( "Fotoalbum (Original)", m_pBackPanel );
    m_pRBOriginPhotoImage->setStyleSheet("background: transparent;");
    m_pRBOriginPhotoImage->setGeometry( 16, 210, 140, 22 );

    m_pRBReducedPhotoImage   = new QRadioButton( "Fotoalbum (verkeleinert)", m_pBackPanel );
    m_pRBReducedPhotoImage->setStyleSheet("background: transparent;");
    m_pRBReducedPhotoImage->setGeometry( 16, 240, 140, 22 );

    m_pBtnHTMLProduce   = new QPushButton( "HTML erzeugen", m_pBackPanel );
    m_pBtnHTMLProduce->setGeometry( 16, 287, 152, 26 );
    m_pBtnHTMLProduce->setStyleSheet("background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 #fafafa, stop:1 #ccc, stop:2 #d8d8d8);}");

    m_pChkFileOverwriting   = new QCheckBox( "Dateien überschreiben", m_pBackPanel );
    m_pChkFileOverwriting->setGeometry( 16, 317, 152, 25 );
    m_pChkFileOverwriting->setStyleSheet("background: transparent;");
    m_pChkFileOverwriting->setChecked( m_pApp->isOverwrie() );

    QFrame* line2 = new QFrame( m_pBackPanel );
    line2->setGeometry(16, 357, 152, 1);
    line2->setStyleSheet("background-color: #555;");

    QFrame* line3 = new QFrame( m_pBackPanel );
    line3->setGeometry(200, 20, 1, 390);
    line3->setStyleSheet("background-color: #555;");

    m_pLblEditor            = new QLabel( "Bearbeiter:", this );
    m_pLblEditor->setGeometry( 16, 382, 152, 25 );
    m_pLblEditor->setStyleSheet("background: transparent;");

    m_pLblEditorName                = new QLabel( m_pApp->getEditerName(), this );
    m_pLblEditorName->setGeometry( 16, 402, 152, 25 );
    m_pLblEditorName->setStyleSheet("background: transparent;");

    m_listbox               = new QListWidget( m_pBackPanel );
    m_listbox->setGeometry( 235, 20, 180, 330 );
    m_listbox->setStyleSheet("background-color: white;");

    m_pProgressBar          = new QProgressBar( m_pBackPanel );
    m_pProgressBar->setGeometry( 235, 355, 180, 15 );
    m_pProgressBar->setStyleSheet("background-color: white;");
    m_pProgressBar->setValue(0);

    m_pBtnFileRename        = new QPushButton( "Dateien jetzt umbenennen", m_pBackPanel );
    m_pBtnFileRename->setGeometry( 235, 382, 180, 26 );
    m_pBtnFileRename->setStyleSheet("background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 #fafafa, stop:1 #ccc, stop:2 #d8d8d8);}");

    m_pWndBilderuploader    = new Bildbetrachter( m_pBackPanel );
    m_pWndBilderuploader->setStyleSheet("background-color: white;");

    m_pBtnLeftRotate        = new QPushButton( "", m_pBackPanel );
    m_pBtnLeftRotate->setGeometry( 150, 846, 35, 25 );
    m_pBtnLeftRotate->setObjectName("LEFT_ROTATE");
    m_pBtnLeftRotate->setStyleSheet( strQssContent );
    m_pBtnLeftRotate->setToolTip(tr("Left Rotate"));
    m_pBtnLeftRotate->setVisible(false);

    m_pBtnRightRotate        = new QPushButton( "", m_pBackPanel );
    m_pBtnRightRotate->setGeometry( 256, 846, 35, 25 );
    m_pBtnRightRotate->setObjectName("RIGHT_ROTATE");
    m_pBtnRightRotate->setStyleSheet( strQssContent );
    m_pBtnRightRotate->setToolTip(tr("Right Rotate"));
    m_pBtnRightRotate->setVisible(false);

    m_pBtnExistFileInFTP    = new QPushButton( "", m_pBackPanel );
    m_pBtnExistFileInFTP->setGeometry( 645, 486, 35, 25 );
    m_pBtnExistFileInFTP->setObjectName("RESET");
    m_pBtnExistFileInFTP->setVisible(false);

    if( m_pApp->isAlwaysTopWindow() )
        setWindowFlags( Qt::Window | Qt::WindowStaysOnTopHint );

    installEventFilter(this);

    connect( m_pBtnAddImages, SIGNAL(clicked()), SLOT(onOpenFile()) );
    connect( m_pBtnReset, SIGNAL(clicked()), SLOT(onReset()) );
    connect( m_pBtnFtpUpload, SIGNAL(clicked()), SLOT(onUploadAllFilesFtp()) );
    connect( m_listbox, SIGNAL(itemSelectionChanged()), SLOT(onSelectedImage()));
    connect( m_pBtnLeftRotate, SIGNAL(clicked()), SLOT(onLeftRotate()) );
    connect( m_pBtnRightRotate, SIGNAL(clicked()), SLOT(onRightRotate()) );
    connect( m_pRBStandardImage, SIGNAL(clicked()), SLOT(onImageViewMode()) );
    connect( m_pRBOriginalImage, SIGNAL(clicked()), SLOT(onImageViewMode()) );
    connect( m_pRBOriginPhotoImage, SIGNAL(clicked()), SLOT(onImageViewMode()) );
    connect( m_pRBReducedPhotoImage, SIGNAL(clicked()), SLOT(onImageViewMode()) );
    connect( m_pChkFileOverwriting, SIGNAL(stateChanged(int)), SLOT(onFileOverWriting(int)) );
    connect( m_pBtnFileRename, SIGNAL(clicked()), SLOT( onRenameImageFiles() ) );
    connect( m_pBtnHTMLProduce, SIGNAL(clicked()), SLOT( onHtmlClipboard() ) );
}

void
Bilderuploader::checkCameras()
{
    m_pUsbDeviceMoniter = new QDeviceWatcher;
    m_pUsbDeviceMoniter->appendEventReceiver(this);

    connect(m_pUsbDeviceMoniter, SIGNAL(deviceAdded(QString)), this, SLOT(onDeviceAdded(QString)), Qt::DirectConnection);
    connect(m_pUsbDeviceMoniter, SIGNAL(deviceChanged(QString)), this, SLOT(onDeviceChanged(QString)), Qt::DirectConnection);
    connect(m_pUsbDeviceMoniter, SIGNAL(deviceRemoved(QString)), this, SLOT(onDeviceRemoved(QString)), Qt::DirectConnection);

    m_pUsbDeviceMoniter->start();
}

void
Bilderuploader::onConnectFtp()
{
    QString strServerIpAddr = "127.0.0.1";
    int nServerPort = 21;
    QString strUserName = "anonymous";
    QString strUserPasswd   = "";

    if( m_pApp )
    {
        strServerIpAddr     = m_pApp->getFtpServerIP();
        nServerPort         = m_pApp->getFtpPort();
        strUserName         = m_pApp->getFtpUserName();
        strUserPasswd       = m_pApp->getFtpUserPasswd();

        m_ftpServer->connectToHost( strServerIpAddr, nServerPort );
        m_ftpServer->login( strUserName, strUserPasswd );

        int nY = 0, nM = 0, nD = 0;
        QDate::currentDate().getDate( &nY, &nM, &nD );
        QString strFtpCurrentDateDir = QString("%1-%2").arg(nY).arg( nM > 9 ? QString("%1").arg(nM) : "0" + QString("%1").arg(nM) );
        QString strFtpServerPath = m_pApp->getBaseUrl()+"/" + m_pApp->getEditerName() + "/" + strFtpCurrentDateDir;
        m_lstFtpPath.clear();
        m_lstFtpPath    = strFtpServerPath.split("/");
        m_nFtpPathIndex = 0;
    }
}

void
Bilderuploader::onOpenFile()
{
    QStringList lstAddedImages = QFileDialog::getOpenFileNames(this, tr("Bitte wählen Sie Bilddateien aus"), m_pApp->getStandardPath(),
                                                      tr("Jpeg (*.jpg);; Alle Dateien (*.*)"));

    if( lstAddedImages.size() )
    {
        m_lstImages << lstAddedImages;
        m_listbox->setFocus( Qt::ActiveWindowFocusReason );

        if( m_lstImages.count() )
        {
            m_pProgressBar->setValue(0);

            for( int i = 0; i < lstAddedImages.count(); i++ )
            {
                QString strFileName = lstAddedImages.at(i);
                QStringList lstFileSection = strFileName.split( "/" );
                strFileName = lstFileSection.at( lstFileSection.size() - 1 );
                m_listbox->addItem( strFileName );
            }

            QString strFileName = m_lstImages.at(0);
            if( BilderFile(strFileName).exists() )
            {
                QImageReader reader(strFileName);
                reader.setAutoTransform(true);
                m_imgSelectedItem = reader.read();
                m_pWndBilderuploader->onSelectedImage( strFileName );

                m_pBtnLeftRotate->setVisible(true);
                m_pBtnRightRotate->setVisible(true);
            }
        }
    }
}

void
Bilderuploader::onOpenHelp()
{

}

void
Bilderuploader::onOpenFtpDialog()
{
    m_pDlgFTPUpload = new BilderFtpDlg( this );
    m_pDlgFTPUpload->exec();
}

void
Bilderuploader::onOpenAbout()
{
    QMessageBox::about( this, tr("About"), tr("Bilderuploader Application\n"
                                              "Version 1.0\n "
                                              "Built on March 8, 2017 \n "
                                              "\n "
                                              "The program copy images from camera and upload them in ftp-server. \n"
                                              "Also photo can be resized and rotated") );
}

void
Bilderuploader::onReset()
{
    m_lstImages.clear();
    m_listbox->clear();
    m_pWndBilderuploader->onSelectedImage( "" );
}

void
Bilderuploader::onCloseApp()
{
    close();
}

void
Bilderuploader::onSelectedImage()
{
    if( !m_listbox->selectedItems().size() ) return;
    if( !m_listbox->count() ) {
        m_pWndBilderuploader->onSelectedImage( "" );
        return;
    }

    QListWidgetItem* pSelectedItem = m_listbox->selectedItems().at(0);
    int nSelNo = m_listbox->row( pSelectedItem );
    QString strFileName = m_lstImages.at(nSelNo);

    BilderFile imgFile(strFileName);
    if( imgFile.exists() )
    {
        m_nRotateAgnle = 0;
        QImageReader reader(strFileName);
        reader.setAutoTransform(true);
        m_imgSelectedItem   = reader.read();
        m_pWndBilderuploader->onSelectedImage( strFileName );
        m_pBtnLeftRotate->setVisible(true);
        m_pBtnRightRotate->setVisible(true);
    }
}

void
Bilderuploader::onUploadAllFilesFtp()
{
    if( !m_listbox->count() )
    {
        QMessageBox::information( this, tr("Error"), tr("Please select images to upload on FTP.") );
        return;
    }

    if( m_pApp->isFtpConnected() &&
        m_listbox->count() > 0 )
    {
        m_statusbar->showMessage( tr("Started uploading file") );
        m_pProgressBar->setValue(0);
        isDirectory.clear();
        m_nUploadItem   = 0;
        m_nFtpPathIndex = 0;

        emit uploadEvent();
    }
}

void
Bilderuploader::onUploadFilesToFtp()
{
    if( m_lstImages.size() > 0 &&
        m_listbox->count() > 0 )
    {
        QString strFtpFileName = m_listbox->item(m_nUploadItem)->text();
        bool bExistFile = false;

        for( int i = 0; i < m_lstFtpImages.size(); i++ ) {
            if( m_lstFtpImages.at(i) == strFtpFileName )
            {
                bExistFile = true;
                break;
            }
        }

        if( m_pApp->getOptMode() == BilderApp::ePhotoStandard ||
            m_pApp->getOptMode() == BilderApp::ePhotoOriginal )
        {
            if( !(m_nNewFileUpload % 2) ) {
                if( bExistFile && !m_pApp->isOverwrie() ) {
                    m_listbox->item( m_nUploadItem )->setForeground( Qt::red );
                    m_nNewFileUpload += 2;
                    m_nUploadItem++;

                    if( m_nUploadItem < m_lstImages.size() ) emit uploadEvent();

                } else {
                    reduceThumbnailImage();

                    QString strUploadThumbFileName = getThumbFileNameForFtp();
                    m_ftpUploadThumbFile = new BilderFile(strUploadThumbFileName);
                    if (!m_ftpUploadThumbFile->open(QIODevice::ReadOnly))
                    {
                        m_statusbar->showMessage( tr("Can't read image file") );
                        m_ftpUploadThumbFile->close();
                        m_ftpUploadThumbFile->deleteLater();

                        m_nNewFileUpload += 2;
                        m_nUploadItem++;

                        if( m_nUploadItem < m_lstImages.size() ) emit uploadEvent();
                        return;
                    }

                    strUploadThumbFileName = strFtpFileName.mid(0, strFtpFileName.length()-4) + "_thumb.jpg";
                    m_ftpServer->put( m_ftpUploadThumbFile, strUploadThumbFileName );
                }

                return;
            }
        }

        if( bExistFile && !m_pApp->isOverwrie() )
        {
            m_listbox->item( m_nUploadItem )->setForeground( Qt::red );
            m_nUploadItem++;

            if( m_nUploadItem < m_lstImages.size() ) emit uploadEvent();
            return;
        }

        QString strUploadFileName = m_lstImages.at(m_nUploadItem);

        if( m_pApp->getOptMode() == BilderApp::eStandard ||
            m_pApp->getOptMode() == BilderApp::ePhotoStandard )
        {
            reduceStandardImage();
            strUploadFileName = getFileNameForFtp();
        }

        m_ftpUploadFile = new BilderFile(strUploadFileName);
        if (!m_ftpUploadFile->open(QIODevice::ReadOnly)) {
            m_statusbar->showMessage( tr("Can't read image file") );
            m_ftpUploadFile->deleteLater();
            return;
        }

        if( !bExistFile ) m_ftpServer->put( m_ftpUploadFile, strFtpFileName );
        else
        {
            if( m_pApp->isOverwrie() ) m_ftpServer->put( m_ftpUploadFile, strFtpFileName );
            else {
                QString strUploadFileName = getFileNameForFtp();
                if( strUploadFileName.right(3).toLower() == "jpg" )
                {
                    QFile aFile( strUploadFileName );
                    if( aFile.exists() )
                        aFile.remove();
                }

                if( m_ftpUploadFile ) {
                    m_ftpUploadFile->close();
                    m_ftpUploadFile->deleteLater();
                }

                m_nUploadItem++;
                if( m_nUploadItem < m_lstImages.size() ) emit uploadEvent();
                else {
                    isDirectory.clear();
                    m_ftpServer->list();
                }
            }
        }
    }
}

void
Bilderuploader::onLeftRotate()
{
    m_nRotateAgnle -= 90;
    QTransform imgRotate;
    imgRotate.rotate(m_nRotateAgnle);

    QImage imgRotatedImage = m_imgSelectedItem.transformed(imgRotate);

    if( m_listbox->selectedItems().count() ) {
         int idx = m_listbox->row( m_listbox->selectedItems().at(0) );
        imgRotatedImage.save( m_lstImages.at(idx), "JPG", 97 );
    } else {
        if( m_listbox->count() ) imgRotatedImage.save( m_lstImages.at(0), "JPG", 97 );
    }

    m_pWndBilderuploader->onSelectedImage(imgRotatedImage);
    m_listbox->setFocus();
}

void
Bilderuploader::onRightRotate()
{
    m_nRotateAgnle += 90;
    QTransform imgRotate;
    imgRotate.rotate(m_nRotateAgnle);

    QImage imgRotatedImage = m_imgSelectedItem.transformed(imgRotate);

    if( m_listbox->selectedItems().count() ) {
         int idx = m_listbox->row( m_listbox->selectedItems().at(0) );
        imgRotatedImage.save( m_lstImages.at(idx), "JPG", 97 );
    } else {
        if( m_listbox->count() ) imgRotatedImage.save( m_lstImages.at(0), "JPG", 97 );
    }

    m_pWndBilderuploader->onSelectedImage(imgRotatedImage);
    m_listbox->setFocus();
}

void
Bilderuploader::onUpdateItem()
{
    m_pLblEditorName->setText( m_pApp->getEditerName() );
    m_pChkFileOverwriting->setChecked( m_pApp->isOverwrie() );
}

void
Bilderuploader::onHtmlClipboard()
{
    if( m_pApp->getOptMode() == BilderApp::ePhotoStandard ||
        m_pApp->getOptMode() == BilderApp::ePhotoOriginal )
    {

        int nY = 0, nM = 0, nD = 0;
        QDate::currentDate().getDate( &nY, &nM, &nD );
        QString strFtpCurrentDateDir = QString("%1-%2").arg(nY)
                                                       .arg( nM > 9 ? QString("%1").arg(nM) : "0" + QString("%1").arg(nM) );

        QString strFtpUrl = m_pApp->getFtpServerIP() + "/" +
                            m_pApp->getBaseUrl() + "/" +
                            m_pApp->getEditerName() + "/" +
                            strFtpCurrentDateDir + "/";

        if( strFtpUrl.left(4) == "ftp." )
            strFtpUrl = "http://www." + strFtpUrl.mid(4, strFtpUrl.length() - 4);

        QString strListItems;

        for( int i = 0; i < m_listbox->count(); i++ )
        {
            QString strImageFtpUrl = strFtpUrl + m_listbox->item(i)->text();
            QString strThumbFtpUrl = strFtpUrl + m_listbox->item(i)->text();

            if( strThumbFtpUrl.right(3).toLower() == "jpg" )
            {
                strThumbFtpUrl = strThumbFtpUrl.mid( 0, strThumbFtpUrl.length() - 4 );
                strThumbFtpUrl += "_thumb.jpg";
            }

            QString strWebUrl = "<a href='" + strImageFtpUrl + "' target='_blank'><img src='" + strThumbFtpUrl + "'/></a>";
            strListItems += strWebUrl + "\n";
        }

        m_pApp->clipboard()->setText(strListItems);
    }
}

void
Bilderuploader::ftpCommandStarted(int)
{
    int id = m_ftpServer->currentCommand();
    switch (id)
    {
        case QFtp::ConnectToHost :
            m_statusbar->showMessage(tr("Connecting to server ..."));
            break;
        case QFtp::Login :
            m_statusbar->showMessage(tr("logging in…"));
            break;
        case QFtp::Get :
            m_statusbar->showMessage(tr("downloading…"));
            break;
        case QFtp::Close :
            m_statusbar->showMessage(tr("Closing connection ..."));
    }
}

void
Bilderuploader::ftpCommandFinished(int, bool error)
{
    if (m_ftpServer->currentCommand() == QFtp::ConnectToHost) {
        if (error) {
            QMessageBox::information(this, tr("FTP"),
                                     tr("Unable to connect to the FTP server "
                                        "at %1. Please check that the host "
                                        "name is correct.")
                                     .arg(m_pApp->getFtpServerIP()));
            return;
        }
        else
        {
            m_statusbar->showMessage(tr("Connected successfully"));
        }
    }

    if ( m_ftpServer->currentCommand() == QFtp::Login )
    {
        if( m_pDlgFTPUpload )
        {
            if( m_pDlgFTPUpload->isAlwaysTopWindow() )
                setWindowFlags( Qt::Window | Qt::WindowStaysOnTopHint );
            else
                setWindowFlags( Qt::Window );

            show();
        }

        m_ftpServer->mkdir(m_lstFtpPath.at(m_nFtpPathIndex));

        m_statusbar->showMessage(tr("Loggined successfully, please wait moment!"));
        m_pApp->setFtpConnected( true );
    }

    if ( m_ftpServer->currentCommand() == QFtp::Get ) {
        if (error) {

        } else {

        }
    } else if( m_ftpServer->currentCommand() == QFtp::List ) {
        if (error) {

        } else {

        }
    } else if ( m_ftpServer->currentCommand() == QFtp::Put ) {
        if (error) {
            m_statusbar->showMessage(tr("Upload error: check whether the file name!").arg(m_ftpServer->errorString()));
            m_listbox->item( m_nUploadItem )->setForeground( Qt::red );

            QString strFileName = getFileNameForFtp();
            QFile aFile( strFileName );
            if( aFile.exists() ) aFile.remove();

            QString strThumbFileName = getThumbFileNameForFtp();
            QFile aFile2( strThumbFileName );
            if( aFile2.exists() ) aFile2.remove();

            if( m_pApp->getOptMode() == BilderApp::ePhotoStandard ||
                m_pApp->getOptMode() == BilderApp::ePhotoOriginal )
            {
                if( !(m_nNewFileUpload%2) ) {
                    if( m_ftpUploadThumbFile )
                    {
                        m_ftpUploadThumbFile->close();
                        m_ftpUploadThumbFile->deleteLater();
                        m_nNewFileUpload+=2;
                    }
                } else {
                    if( m_ftpUploadFile ) {
                        m_ftpUploadFile->close();
                        m_ftpUploadFile->deleteLater();
                        m_nNewFileUpload++;
                    }
                }
            }
            else {
                if( m_ftpUploadFile ) {
                    m_ftpUploadFile->close();
                    m_ftpUploadFile->deleteLater();
                    m_nNewFileUpload++;
                }
            }

            m_nUploadItem++;
            if( m_nUploadItem < m_lstImages.size() ) emit uploadEvent();
            else {
                isDirectory.clear();
                m_ftpServer->list();
            }

        } else {

            if( m_pApp->getOptMode() == BilderApp::ePhotoStandard ||
                m_pApp->getOptMode() == BilderApp::ePhotoOriginal )
            {
                if( !(m_nNewFileUpload%2) )
                {
                    if( m_ftpUploadThumbFile )
                    {
                        m_ftpUploadThumbFile->close();
                        m_ftpUploadThumbFile->deleteLater();
                    }

                    QString strFileName = getThumbFileNameForFtp();
                    QFile aFile( strFileName );
                    if( aFile.exists() ) aFile.remove();

                    m_nNewFileUpload++;
                    if( m_nUploadItem < m_lstImages.size() ) emit uploadEvent();
                    else {
                        isDirectory.clear();
                        m_ftpServer->list();
                    }

                    return;
                }
            }

            if( m_ftpUploadFile )
            {
                m_ftpUploadFile->close();
                m_ftpUploadFile->deleteLater();
            }

            QString strFileName = getFileNameForFtp();
            QFile aFile( strFileName );
            if( aFile.exists() ) aFile.remove();

            m_listbox->item( m_nUploadItem )->setForeground( Qt::darkGreen );
            m_statusbar->showMessage(tr("Upload completed"));
                m_nUploadItem++;

            if( m_pApp->getOptMode() == BilderApp::ePhotoStandard ||
                m_pApp->getOptMode() == BilderApp::ePhotoOriginal )
                m_nNewFileUpload++;

            if( m_nUploadItem < m_lstImages.size() ) emit uploadEvent();
            else {
                isDirectory.clear();
                m_ftpServer->list();
            }
        }
    } else if (m_ftpServer->currentCommand() == QFtp::Mkdir){

        if( !error )
            m_statusbar->showMessage(tr("Created new folder successfully!"));

        isDirectory.clear();

        if( m_nFtpPathIndex+1 < m_lstFtpPath.length() )
        {
            m_ftpServer->cd(m_lstFtpPath.at(m_nFtpPathIndex));
            m_ftpServer->mkdir(m_lstFtpPath.at(m_nFtpPathIndex+1));
        }
        else
        {
           m_ftpServer->cd(m_lstFtpPath.at(m_nFtpPathIndex));
        }

        m_ftpServer->list();
        m_nFtpPathIndex++;
        m_statusbar->showMessage(tr("Searching ftp path now, moment"));
    }

}

void
Bilderuploader::addToList(const QUrlInfo &urlInfo)
{
    m_statusbar->showMessage(tr("Please upload files"));

    if( urlInfo.isFile() )
    {
        QString strFileExt = urlInfo.name().right(3).toLower();

        if( strFileExt == "jpg" )
        {
            isDirectory[_FromSpecialEncoding(urlInfo.name())] = false;
            m_lstFtpImages.append( urlInfo.name() );
        }
    }

    m_pBtnFtpUpload->setEnabled(true);
}

void
Bilderuploader::updateDataTransferProgress(qint64 readBytes, qint64 totalBytes)
{
    if( m_pApp->getOptMode() == BilderApp::ePhotoStandard ||
        m_pApp->getOptMode() == BilderApp::ePhotoOriginal )
    {
        if( !(m_nNewFileUpload%2) ) return;
    }

    m_pProgressBar->setMaximum(totalBytes);
    m_pProgressBar->setValue(readBytes);
}

//There are FTP-side code to the local code
QString
Bilderuploader::_FromSpecialEncoding(const QString &InputStr)
{
#ifdef Q_OS_WIN
    return  QString::fromLocal8Bit(InputStr.toLatin1());
#else
    QTextCodec *codec = QTextCodec::codecForName("gbk");
    if (codec)
    {
        return codec->toUnicode(InputStr.toLatin1());
    }
    else
    {
        return QString("");
    }
#endif
}

bool
Bilderuploader::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) m_listbox->setFocus();

    if (obj == m_listbox)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

            if( keyEvent->key() == Qt::Key_Delete )
            {
                bool bRemove = false;
                int idx = -1;
                if( m_listbox->selectedItems().count() )
                {
                    idx = m_listbox->row( m_listbox->selectedItems().at(0) );

                    if( idx >= 0 )
                    {
                        qDeleteAll(m_listbox->selectedItems());
                        m_lstImages.removeAt( idx );

                        if( !m_listbox->count() ) m_pWndBilderuploader->onSelectedImage( "" );
                        bRemove = true;
                    }
                }

                return bRemove;
            }

            return false;
        }
        else
        {
            return false;
        }
    }
    else
    {
        // pass the event on to the parent class
        return QMainWindow::eventFilter(obj, event);
    }
}

void
Bilderuploader::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void
Bilderuploader::dropEvent(QDropEvent *event)
{
    QStringList lstAddedImages;
    QList<QUrl> droppedUrls = event->mimeData()->urls();
    int droppedUrlCnt = droppedUrls.size();

    for(int i = 0; i < droppedUrlCnt; i++) {
        QString localPath = droppedUrls[i].toLocalFile();
        QFileInfo fileInfo(localPath);

        if(fileInfo.isFile())
        {
            QString strImgFilePath = fileInfo.absoluteFilePath();
            if(strImgFilePath.right(3).toLower() == "jpg") lstAddedImages.append( strImgFilePath );
        }
    }

    if( lstAddedImages.size() )
    {
        m_lstImages << lstAddedImages;

        if( m_lstImages.count() )
        {
            m_pProgressBar->setValue(0);

            for( int i = 0; i < lstAddedImages.count(); i++ )
            {
                QString strFileName = lstAddedImages.at(i);
                QStringList lstFileSection = strFileName.split( "/" );
                strFileName = lstFileSection.at( lstFileSection.size() - 1 );
                m_listbox->addItem( strFileName );
            }

            QString strFileName = m_lstImages.at(0);
            if( BilderFile(strFileName).exists() )
            {
                QImageReader reader(strFileName);
                reader.setAutoTransform(true);
                m_imgSelectedItem = reader.read();
                m_pWndBilderuploader->onSelectedImage( strFileName );

                m_pBtnLeftRotate->setVisible(true);
                m_pBtnRightRotate->setVisible(true);
            }
        }
    }

    event->acceptProposedAction();
}

void
Bilderuploader::onDeviceAdded(const QString &dev)
{
    if( m_pApp->isAutoCameraImport() )
    {
        onScanDevice( dev );
    }
}

void
Bilderuploader::onDeviceChanged(const QString &)
{

}

void
Bilderuploader::onDeviceRemoved(const QString &)
{

}

void
Bilderuploader::onImageViewMode()
{
    if( m_pRBStandardImage->isChecked() )
        m_pApp->setOptionMode( BilderApp::eStandard );
    else if( m_pRBOriginalImage->isChecked() )
        m_pApp->setOptionMode( BilderApp::eOriginal );
    else if( m_pRBOriginPhotoImage->isChecked() )
        m_pApp->setOptionMode( BilderApp::ePhotoOriginal );
    else if( m_pRBReducedPhotoImage->isChecked() )
        m_pApp->setOptionMode( BilderApp::ePhotoStandard );

    if( m_nRotateAgnle )
    {
        QTransform imgRotate;
        imgRotate.rotate(m_nRotateAgnle);

        QImage imgRotatedImage = m_imgSelectedItem.transformed(imgRotate);
        m_pWndBilderuploader->onSelectedImage( imgRotatedImage );
    }
    else
        m_pWndBilderuploader->onSelectedImage( m_imgSelectedItem );
}

void
Bilderuploader::onFileOverWriting(int)
{
    m_pApp->setOverwrie( m_pChkFileOverwriting->isChecked() );
}

void
Bilderuploader::onRenameImageFiles()
{
    for( int i = 0; i < m_lstImages.count(); i++ )
    {
        QString strFileName = m_lstImages.at(i);
        if( BilderFile(strFileName).exists() )
        {
            QString strNewFile = strFileName;
            QStringList lstFileSection = strFileName.split( "/" );
            strNewFile = lstFileSection.at( lstFileSection.size() - 1 );

            int nY = 0, nM = 0, nD = 0;
            int h, m, s;
            QDate::currentDate().getDate( &nY, &nM, &nD );
            h = QTime::currentTime().hour();
            m = QTime::currentTime().minute();
            s = QTime::currentTime().second();

            QString strFtpCurrentDateDir = QString("-%1-%2-%3-%4-%5-%6").arg( nD > 9 ? QString("%1").arg(nD) : "0" + QString("%1").arg(nD) )
                                                              .arg( nM > 9 ? QString("%1").arg(nM) : "0" + QString("%1").arg(nM) )
                                                              .arg(nY)
                                                              .arg(h)
                                                              .arg(m)
                                                              .arg(s);

            strNewFile = strNewFile.remove( strNewFile.length() - 4, 4 ) + strFtpCurrentDateDir + ".jpg";

            QString strNewFilePath = "";
            if( lstFileSection.size() > 1 )
            {
                for( int i = 0; i < lstFileSection.size() - 1; i++ )
                    strNewFilePath += lstFileSection.at(i) + "/";

                strNewFilePath += strNewFile;
                if( QFile::rename( strFileName, strNewFilePath ) )
                {
                    m_lstImages.replace(i, strNewFilePath);
                    m_listbox->item(i)->setText( strNewFile );
                    m_listbox->item(i)->setForeground( Qt::black );
                }
            }
        }
    }
}

void
Bilderuploader::onScanDevice(const QString &dev)
{
    int nY = 0, nM = 0, nD = 0;
    QDate::currentDate().getDate( &nY, &nM, &nD );

    QString strLocalDir = QString("%1_%2_%3")
                                             .arg(nY)
                                             .arg( nM > 9 ? QString("%1").arg(nM) : "0" + QString("%1").arg(nM) )
                                             .arg( nD > 9 ? QString("%1").arg(nD) : "0" + QString("%1").arg(nD) );

    QString strCreatedDirPath = m_pApp->getCameraImportPath();
    strCreatedDirPath += "/" + strLocalDir;
    QDir dir;
    dir.mkpath( strCreatedDirPath );

    m_lstImages.clear();
    QDirIterator it(dev, QStringList() << "*.jpg", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString strDeviceImgFile = it.next();
        QStringList lstFileSection = strDeviceImgFile.split( "/" );
        QString strFileName = lstFileSection.at( lstFileSection.size() - 1 );
        QString strCutFilePath = strCreatedDirPath + "/" + strFileName;

        if( BilderFile::copy( strDeviceImgFile, strCutFilePath ) )
        {
            m_lstImages.append( strCutFilePath );
            QDir().remove( strDeviceImgFile );
        }
    }

    if( m_lstImages.count() )
    {
        m_listbox->clear();

        for( int i = 0; i < m_lstImages.count(); i++ )
        {
            QString strFileName = m_lstImages.at(i);
            QStringList lstFileSection = strFileName.split( "/" );
            strFileName = lstFileSection.at( lstFileSection.size() - 1 );
            m_listbox->addItem( strFileName );
        }

        QString strFileName = m_lstImages.at(0);
        if( BilderFile(strFileName).exists() )
        {
            QImageReader reader(strFileName);
            reader.setAutoTransform(true);
            m_imgSelectedItem = reader.read();

            m_pWndBilderuploader->onSelectedImage( strFileName );
            m_pBtnLeftRotate->setVisible(true);
            m_pBtnRightRotate->setVisible(true);
        }
    }

}

QString
Bilderuploader::getFileNameForFtp()
{
    QString strNewFileName = m_lstImages.at(m_nUploadItem);
    if( strNewFileName.right(3).toLower() == "jpg" )
    {
        int nExtPos = strNewFileName.indexOf( ".jpg" );
        strNewFileName = strNewFileName.mid(0, nExtPos) + "_resized.jpg";
    }

    return strNewFileName;
}

QString
Bilderuploader::getThumbFileNameForFtp()
{
    QString strThumbFileName = m_lstImages.at(m_nUploadItem);
    if( strThumbFileName.right(3).toLower() == "jpg" )
    {
        strThumbFileName = m_lstImages.at(m_nUploadItem).mid(0, strThumbFileName.length()-4) + "_thumb.jpg";
    }

    return strThumbFileName;
}

void
Bilderuploader::reduceStandardImage()
{
    int nMaxWidth = m_pApp->getMaxWidth();
    int nMaxHeight = m_pApp->getMaxHeight();

    QString strNewFileName;
    strNewFileName= getFileNameForFtp();

    _reduceImage( strNewFileName, nMaxWidth, nMaxHeight );
}

void
Bilderuploader::reduceThumbnailImage()
{
    int nMaxWidth = m_pApp->getMaxThumbWidth();
    int nMaxHeight = m_pApp->getMaxThumbHeight();

    QString strNewFileName;
    strNewFileName= getThumbFileNameForFtp();

    _reduceImage( strNewFileName, nMaxWidth, nMaxHeight );
}

void
Bilderuploader::_reduceImage( QString strNewFileName, int nMaxWidth, int nMaxHeight )
{
    QImageReader reader( m_lstImages.at(m_nUploadItem) );
    reader.setAutoTransform(true);

    QImage imgReduced = reader.read();

    if( m_nRotateAgnle )
    {
        QTransform imgRotate;
        imgRotate.rotate(m_nRotateAgnle);
        imgReduced = imgReduced.transformed(imgRotate);
    }

    if( imgReduced.width() > 10 &&
        imgReduced.height() > 10 )
    {
        if( imgReduced.width() >= imgReduced.height() )
        {
            if( nMaxWidth * imgReduced.height()*1.0f/imgReduced.width() < nMaxHeight )
                nMaxHeight = nMaxWidth * imgReduced.height()*1.0f/imgReduced.width();
        }
        else
        {
            if( nMaxHeight * imgReduced.width()*1.0f/imgReduced.height() < nMaxWidth )
                nMaxWidth = nMaxHeight * imgReduced.width()*1.0f/imgReduced.height();
        }

        imgReduced = imgReduced.scaled( nMaxWidth, nMaxHeight );
        imgReduced.save( strNewFileName, "JPG", 97 );
    }
}

QString
Bilderuploader::_ToSpecialEncoding(const QString &InputStr)
{
#ifdef Q_OS_WIN
    return QString::fromLatin1(InputStr.toLocal8Bit());
#else
    QTextCodec *codec= QTextCodec::codecForName("gbk");
    if (codec)
    {
        return QString::fromLatin1(codec->fromUnicode(InputStr));
    }
    else
    {
        return QString("");
    }
#endif
}
