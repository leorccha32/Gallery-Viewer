#ifndef BILDER_H
#define BILDER_H

#include <QImage>

class Bilder : public QImage
{
public:
    Bilder( QString strFileName );
    ~Bilder();

protected:
//    QImage smoothScaled(int w, int h) const;
};

#endif // BILDER_H
