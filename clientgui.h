#ifndef CLIENTGUI_H
#define CLIENTGUI_H

#include <QObject>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QSplitter>
#include <QDialog>
#include <QToolBox>
#include <QKeyEvent>
#include <QEvent>
#include <QScrollBar>
#include <QString>
#include <QAbstractTextDocumentLayout>
#include <QTextStream>
#include <QTabWidget>
#include <QSocketNotifier>
#include <QtGui/QMainWindow>
#include "gaosocket.h"
#include "client.h"

static QString const ROOTTAB = "ROOT";

class Tab : public QWidget {
	Q_OBJECT

public:
	Tab(QWidget *parent=0);

    bool eventFilter(QObject *watched, QEvent *e);
	void appendChat(QString html);
	void passClient(Client *c);
private:
    QTextEdit *chatInput;
    QTextBrowser *chat;

    void submitChatInput();
	QString msgToHtml(QString nick, QString text);
protected:
	QSplitter *outputSplitter;
	QSplitter *inputSplitter;
	Client *client;
private slots:
};



class GroupTab : public Tab {
	Q_OBJECT

public:
	GroupTab(QWidget *parent=0);
	void consolidatePeers(std::map<std::string,std::string> peers);
private:
	QListWidget *online;
};



class ClientGUI : public QWidget
{
    Q_OBJECT

public:
    ClientGUI(QWidget *parent = 0);
    ~ClientGUI();
private:
    Client *client;
	QTabWidget *tabs;
	std::map<QString,Tab*> tabPt;

    QLineEdit *serverAddr;
    QSpinBox *serverPort;
    QLineEdit *nick;
    QDialog *configPrompt;

	QSocketNotifier *socketEvent;

	GroupTab* getRootTab();
private slots:
    void quit();
    void serverConnect();
	void processMsg();
	void msgPeer(QString nick);
};



class PeerTab : public Tab {
	Q_OBJECT

public:
private:
private slots:
};

#endif // CLIENTGUI_H
