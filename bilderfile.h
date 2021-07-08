#ifndef BILDERFILE_H
#define BILDERFILE_H

#include <QFile>
#include <QString>

class BilderFile : public QFile
{
public:
    explicit BilderFile( QString strFileName );
    ~BilderFile();

    void setValue( QString strKey, QString strValue );
    void setValue( QString strKey, bool bValue );

private:
    QString m_strKey;
    QString m_strValue;
    bool    m_bValue;
};

#endif // BILDERFILE_H
