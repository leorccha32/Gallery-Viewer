#include <QDesktopWidget>
#include "bilderuploader.h"
#include "bilderapp.h"

int main(int argc, char *argv[])
{
    BilderApp app(argc, argv);
    Bilderuploader w;

    QDesktopWidget deskWnd;
    w.move((deskWnd.width() - MAX_WIDTH)/2, (deskWnd.height() - MAX_HEIGHT)/2 );
    w.show();

    return app.exec();
}
