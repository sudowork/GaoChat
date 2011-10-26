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
class Tab;
class GroupTab;
class PeerTab;
class CloseableTabWidget;
class ClientGUI;

QString msgToHtml(QString nick, QString text, bool fromPeer=false);

class ClientGUI : public QWidget
{
    Q_OBJECT

public:
    ClientGUI(QWidget *parent = 0);
    ~ClientGUI();

	QTabWidget* getTabs();
private:
    Client *client;
	CloseableTabWidget *tabs;
	std::map<QString,Tab*> *tabPt;

    QLineEdit *serverAddr;
    QSpinBox *serverPort;
    QLineEdit *nick;
    QDialog *configPrompt;

	QSocketNotifier *socketEvent;

	GroupTab* getRootTab();
	PeerTab* newPeerTab(QString ipp,QString peer);
private slots:
    void quit();
    void serverConnect();
	void processMsg();
	void startPeerChat(QListWidgetItem* peer);
};



class Tab : public QWidget {
	Q_OBJECT

public:
	Tab(QWidget *parent=0);
	//~Tab();

    bool eventFilter(QObject *watched, QEvent *e);
	void appendChat(QString html);
	void passClient(Client *c);
protected:
    QTextEdit *chatInput;
    QTextBrowser *chat;

	QSplitter *outputSplitter;
	QSplitter *inputSplitter;
	Client *client;

    void submitChatInput();
private slots:
};



class GroupTab : public Tab {
	Q_OBJECT

public:
	GroupTab(QWidget *parent=0);
	//~GroupTab();
	void consolidatePeers(std::map<std::string,std::string> peers,std::map<QString,Tab*> *tabPt, QTabWidget* tabs);
	QString peerFromList(QString nick);
private:
	QListWidget *online;
	int hueSeed;

	int nextHue();
};



class PeerTab : public Tab {
	Q_OBJECT

public:
	PeerTab(QString ipp, QString peer, QWidget *parent=0);
	//~PeerTab();

    bool eventFilter(QObject *watched, QEvent *e);

	QString ip();
	void ip(QString ip);
	unsigned short port();
	void port(unsigned short port);
	QString peer();
	void peer(QString peer);
private:
	QString _ip;
	unsigned short _port;
	QString _peer;

	// Override Tab
	void submitChatInput();
private slots:
};



class CloseableTabWidget : public QTabWidget {
	Q_OBJECT

public:
	CloseableTabWidget(std::map<QString,Tab*> *t,QWidget* parent=0);
	//~CloseableTabWidget();

private:
	std::map<QString,Tab*> *tabPt;

private slots:
	void closeTab(int i);
};

#endif // CLIENTGUI_H