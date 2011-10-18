#ifndef CLIENTGUI_H
#define CLIENTGUI_H

#include <QtGui/QMainWindow>

class ClientGUI : public QMainWindow
{
    Q_OBJECT

public:
    ClientGUI(QWidget *parent = 0);
    ~ClientGUI();
};

#endif // CLIENTGUI_H
