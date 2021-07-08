#include "bilder.h"

Bilder::Bilder( QString strFileName )
    : QImage(strFileName )
{

}

Bilder::~Bilder()
{

}

/*QImage
Bilder::smoothScaled(int w, int h)
{
    return QImage::smoothScaled(w, h);
}*/
