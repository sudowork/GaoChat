#include <QtGui/QApplication>
#include "clientgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ClientGUI cgui;

	QObject::connect(&a,SIGNAL(aboutToQuit()),&cgui,SLOT(quit()));
    cgui.show();

    return a.exec();
}
