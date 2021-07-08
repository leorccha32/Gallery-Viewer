#include <QTextStream>
#include "bilderfile.h"

BilderFile::BilderFile( QString strFileName )
    : QFile( strFileName )
{
    m_strKey    = "";
    m_strValue  = "";
}

BilderFile::~BilderFile()
{

}

void
BilderFile::setValue( QString strKey, QString strValue )
{
    m_strKey    = strKey;
    m_strValue  = strValue;

    bool bKeyExist = false;
    QTextStream in(this);

    while (!atEnd()) {
        QString line = in.readLine();

        QString strFileContent;
        if (line.startsWith( m_strKey ) )
        {
            strFileContent =
            bKeyExist = true;
            break;
        }
    }
}

void
BilderFile::setValue( QString strKey, bool bValue )
{
    m_strKey    = strKey;
    m_bValue  = bValue;

    bool bKeyExist = false;
    QTextStream in(this);

    while (!atEnd()) {
        QString line = in.readLine();

        QString strFileContent;
        if (line.startsWith( m_strKey ) )
        {
            strFileContent =
            bKeyExist = true;
            break;
        }
    }
}


