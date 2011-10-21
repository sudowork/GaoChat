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

class ClientGUI : public QWidget
{
    Q_OBJECT

public:
    ClientGUI(QWidget *parent = 0);
    ~ClientGUI();
    bool eventFilter(QObject *watched, QEvent *e);
    void submitChatInput();
private:
    Client *client;
    QTextEdit *chatInput;
    QTextBrowser *chat;
	QListWidget *online;
    QLineEdit *serverAddr;
    QSpinBox *serverPort;
    QLineEdit *nick;
    QDialog *configPrompt;
	QTabWidget *tabs;

	QSocketNotifier *socketEvent;

	QString msgToHtml(QString nick, QString text);
	void appendChat(QString html);
	void consolidatePeers(std::map<std::string,std::string> peers);
private slots:
    void quit();
    void saveSettings();
	void processMsg();
	void msgPeer(QString nick);
};

#endif // CLIENTGUI_H
