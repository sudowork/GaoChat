#include <QtGui/QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSplitter>
#include <QDialog>
#include <QToolBox>
#include "clientgui.h"

int main(int argc, char *argv[])
{


    /**********
     * Build base application window
     **********/
    QApplication a(argc, argv);

    QWidget *w = new QWidget;
    w->setWindowTitle("GaoChat Client");


    // Will contain input text and send button
    QHBoxLayout *inputLayout = new QHBoxLayout();
    // Will contain chat window and list of online peers
    QSplitter *outputSplitter = new QSplitter();
    // Contains the top portion (output) and bottom (input)
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QTextEdit *input = new QTextEdit();
    input->setMaximumHeight(20);
    input->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    QPushButton *send = new QPushButton("Send");
    send->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    inputLayout->addWidget(input);
    inputLayout->addWidget(send);

    QWidget *chatContainer = new QWidget();
    QVBoxLayout *chatContainerLayout = new QVBoxLayout();
    QTextBrowser *chat = new QTextBrowser();
    chatContainer->setMinimumHeight(400);
    chatContainerLayout->addWidget(new QLabel("Chat:"));
    chatContainerLayout->addWidget(chat);
    chatContainerLayout->setMargin(0);
    chatContainerLayout->setSpacing(2);
    chatContainer->setLayout(chatContainerLayout);

    QWidget *onlineContainer = new QWidget();
    QVBoxLayout *onlineContainerLayout = new QVBoxLayout();
    QListWidget *online = new QListWidget();
    onlineContainer->setMinimumWidth(50);
    onlineContainer->setMaximumWidth(175);
    onlineContainerLayout->addWidget(new QLabel("Online:"));
    onlineContainerLayout->addWidget(online);
    onlineContainerLayout->setMargin(0);
    onlineContainerLayout->setSpacing(2);
    onlineContainer->setLayout(onlineContainerLayout);

    outputSplitter->addWidget(chatContainer);
    outputSplitter->addWidget(onlineContainer);
    outputSplitter->setStretchFactor(0,7);
    outputSplitter->setStretchFactor(1,3);

    mainLayout->addWidget(outputSplitter);
    mainLayout->addLayout(inputLayout);
    mainLayout->setMargin(10);
    mainLayout->setSpacing(5);

    w->setLayout(mainLayout);


    /**********
     * Dialog box prompting for server,port and nickname
     **********/
    QDialog *configPrompt = new QDialog;
    configPrompt->setWindowTitle("Settings");
    configPrompt->setFixedSize(configPrompt->sizeHint());
    configPrompt->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QVBoxLayout *serverSettings = new QVBoxLayout();
    QLineEdit *serverAddr = new QLineEdit();
    serverAddr->setFixedWidth(125);
    QLineEdit *serverPort = new QLineEdit();
    // Radio buttons for UDP/TCP
    serverSettings->addWidget(new QLabel("Server:"));
    serverSettings->addWidget(serverAddr);
    serverSettings->addWidget(new QLabel("Port:"));
    serverSettings->addWidget(serverPort);
    serverSettings->setSpacing(1);
    serverSettings->setMargin(0);

    QVBoxLayout *configLayout = new QVBoxLayout();
    configLayout->addLayout(serverSettings);

    configPrompt->setLayout(configLayout);

    configPrompt->exec();

    w->show();
    return a.exec();
}
