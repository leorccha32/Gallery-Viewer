#ifndef BILDERUPLOADER_H
#define BILDERUPLOADER_H

#include <QMainWindow>
#include <QMenuBar>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QStatusBar>
#include <QList>
#include <QProgressBar>
#include "qurlinfo.h"

#define MAX_WIDTH       438
#define MAX_HEIGHT      914

class BilderApp;
class Bildbetrachter;
class BilderFtpDlg;
class BilderFile;
class QDeviceWatcher;
class QFtp;
class QScrollArea;

class Bilderuploader : public QMainWindow
{
    Q_OBJECT

public:
    explicit Bilderuploader(QWidget *parent = 0);
    ~Bilderuploader();

    void onConnectFtp();

private:
    void initWnd();
    void initMenuAndStatus();
    void initCtrls();
    void checkCameras();
    void onScanDevice(const QString&);

    QString getFileNameForFtp();
    QString getThumbFileNameForFtp();

    void reduceStandardImage();
    void reduceThumbnailImage();
    void _reduceImage( QString strNewFileName, int nMaxWidth, int nMaxHeight );

    QString _ToSpecialEncoding(const QString &InputStr);
    QString _FromSpecialEncoding(const QString &InputStr);

    bool eventFilter(QObject *obj, QEvent *ev);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

signals:
    void uploadEvent();
    void updateImage(QImage&);

private slots:
    void onOpenFile();
    void onOpenHelp();
    void onOpenFtpDialog();
    void onOpenAbout();
    void onCloseApp();
    void onReset();
    void onSelectedImage();
    void onUploadAllFilesFtp();
    void onUploadFilesToFtp();
    void onLeftRotate();
    void onRightRotate();
    void onUpdateItem();
    void onHtmlClipboard();

    void ftpCommandStarted(int);
    void ftpCommandFinished(int, bool error);
    void addToList(const QUrlInfo &urlInfo);
    void updateDataTransferProgress(qint64 readBytes, qint64 totalBytes);

    void onDeviceAdded(const QString&);
    void onDeviceChanged(const QString&);
    void onDeviceRemoved(const QString&);

    void onImageViewMode();
    void onFileOverWriting(int);
    void onRenameImageFiles();

private:
    QMenuBar*       m_menubar;
    QStatusBar*     m_statusbar;

    QWidget*        m_pBackPanel;
    QPushButton*    m_pBtnAddImages;
    QPushButton*    m_pBtnFtpUpload;
    QPushButton*    m_pBtnReset;

    QRadioButton*   m_pRBStandardImage;
    QRadioButton*   m_pRBOriginalImage;
    QRadioButton*   m_pRBOriginPhotoImage;
    QRadioButton*   m_pRBReducedPhotoImage;

    QPushButton*    m_pBtnHTMLProduce;
    QCheckBox*      m_pChkFileOverwriting;

    QLabel*         m_pLblEditor;
    QLabel*         m_pLblEditorName;

    QListWidget*    m_listbox;
    QPushButton*    m_pBtnFileRename;

    QPushButton*    m_pBtnLeftRotate;
    QPushButton*    m_pBtnRightRotate;
    QPushButton*    m_pBtnExistFileInFTP;
    QProgressBar*   m_pProgressBar;

    Bildbetrachter* m_pWndBilderuploader;
    BilderFtpDlg*   m_pDlgFTPUpload;

    QStringList     m_lstImages;
    QStringList     m_lstFtpImages;

    QFtp*           m_ftpServer;
    BilderApp*      m_pApp;

    int             m_nUploadItem;
    QImage          m_imgSelectedItem;
    int             m_nRotateAgnle;

    QDeviceWatcher* m_pUsbDeviceMoniter;
    QHash<QString, bool> isDirectory;
    BilderFile*     m_ftpUploadFile;
    BilderFile*     m_ftpUploadThumbFile;
    QStringList     m_lstFtpPath;
    int             m_nFtpPathIndex;
    int             m_nNewFileUpload;
};

#endif // BILDERUPLOADER_H
