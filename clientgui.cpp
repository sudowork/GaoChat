#include "common.h"
#include "clientgui.h"

using std::string;
using std::vector;
using std::map;

ClientGUI::ClientGUI(QWidget *parent)
    : QWidget(parent)
{
	/**********
	 * Initialize NULL pointer
	 **********/

	client = NULL;

    /**********
     * Build base application window
     **********/

    setWindowTitle("GaoChat Client");

    // Will contain chat window and list of online peers
    QSplitter *outputSplitter = new QSplitter;
    // Will split the outputSplitter from the input line
    QSplitter *inputSplitter = new QSplitter;
    inputSplitter->setOrientation(Qt::Vertical);
    // Contains the top portion (output) and bottom (input)
    QLayout *groupLayout = new QVBoxLayout;

    // Chat input
    chatInput = new QTextEdit;
    chatInput->setMinimumHeight(20);
    chatInput->setMaximumHeight(100);
    chatInput->installEventFilter(this);

    // Add chat viewer and chat input to main splitter
    inputSplitter->addWidget(outputSplitter);
    inputSplitter->addWidget(chatInput);

    // Configure sizing for main vertical splitter
    inputSplitter->setStretchFactor(0,99);
    inputSplitter->setStretchFactor(1,1);
    inputSplitter->setCollapsible(1,false);

    // Populate chat container
    QWidget *chatContainer = new QWidget;
    QLayout *chatContainerLayout = new QVBoxLayout;
    chat = new QTextBrowser;
    chatContainerLayout->addWidget(new QLabel("Chat:"));
    chatContainerLayout->addWidget(chat);
    chatContainerLayout->setMargin(0);
    chatContainerLayout->setSpacing(2);
    chatContainer->setLayout(chatContainerLayout);

    // Populate online list container
    QWidget *onlineContainer = new QWidget;
    QLayout *onlineContainerLayout = new QVBoxLayout();
    online = new QListWidget;
	online->setSortingEnabled(true);
    onlineContainer->setMinimumWidth(50);
    onlineContainer->setMaximumWidth(175);
    onlineContainerLayout->addWidget(new QLabel("Online:"));
    onlineContainerLayout->addWidget(online);
    onlineContainerLayout->setMargin(0);
    onlineContainerLayout->setSpacing(2);
    onlineContainer->setLayout(onlineContainerLayout);

    // Add chat viewer and online list to splitter
    outputSplitter->addWidget(chatContainer);
    outputSplitter->addWidget(onlineContainer);
    outputSplitter->setStretchFactor(0,7);
    outputSplitter->setStretchFactor(1,3);

	// Construct tabs
	tabs = new QTabWidget;
	tabs->setDocumentMode(true);
	QWidget *groupChat = new QWidget;

    groupLayout->addWidget(inputSplitter);
    groupLayout->setMargin(10);
    groupLayout->setSpacing(5);

	groupChat->setLayout(groupLayout);

	tabs->addTab(groupChat,"Group Chat");

	QLayout *mainLayout = new QVBoxLayout;
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	mainLayout->addWidget(tabs);

	setLayout(mainLayout);

    /**********
     * Dialog box prompting for server,port and nickname
     **********/
    configPrompt = new QDialog;
    configPrompt->setWindowTitle("Settings");
    configPrompt->setFixedSize(configPrompt->sizeHint());
    configPrompt->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QLayout *serverSettings = new QVBoxLayout;

    serverAddr = new QLineEdit;
    serverAddr->setFixedWidth(125);
    serverAddr->setText("127.0.0.1");

    serverPort = new QSpinBox;
    serverPort->setMinimum(1024);
    serverPort->setMaximum(65535);
    serverPort->setValue(S_PORT);

    // Radio buttons for UDP/TCP

    serverSettings->addWidget(new QLabel("Server:"));
    serverSettings->addWidget(serverAddr);
    serverSettings->addWidget(new QLabel("Port:"));
    serverSettings->addWidget(serverPort);
    serverSettings->setSpacing(1);
    serverSettings->setMargin(0);

    QLayout *userSettings = new QVBoxLayout;

    nick = new QLineEdit;
	nick->setText("sudowork");
    userSettings->addWidget(new QLabel("Nickname:"));
    userSettings->addWidget(nick);

    QPushButton *save = new QPushButton("Save");
    connect(save,SIGNAL(clicked()),this,SLOT(saveSettings()));
    QPushButton *quit = new QPushButton("Quit");
    connect(quit,SIGNAL(clicked()),this,SLOT(quit()));

    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->addLayout(serverSettings);
    configLayout->addLayout(userSettings);
    configLayout->addWidget(save);
    configLayout->addWidget(quit);

    configPrompt->setLayout(configLayout);

    configPrompt->exec();
}

bool ClientGUI::eventFilter(QObject *dist, QEvent *event) {
    // Check for keypress
    if (event->type() == QEvent::KeyPress) {
        // Cast to keyEvent
        QKeyEvent *e = static_cast<QKeyEvent *>(event);
        // Check if return is pressed
        if ((e->key() == Qt::Key_Return)) {
            // Make sure enter is not escaped w/ alt modifier
            if (e->modifiers() != Qt::AltModifier) {
		        submitChatInput();
				return true;
            } else {
                // if alt is used, then just insert a return
				chatInput->insertPlainText(QString("\n"));
				chatInput->ensureCursorVisible();
				return true;
            }
        }
        //chatInput->document()->documentLayout()->documentSize().toSize();
    }
    return false;
}

void ClientGUI::submitChatInput() {
    //string input = chatInput->toPlainText().toUtf8().constData();
	QString input = chatInput->toPlainText();
	QString inputHtml = msgToHtml(QString::fromStdString(client->nick()),chatInput->toPlainText());

	client->sendServerMsg(string(input.toUtf8().constData()));
	appendChat(inputHtml);

    // Clear text input
    chatInput->clear();
}

ClientGUI::~ClientGUI()
{

}

void ClientGUI::quit() {
	// If client has not been instantiated, just exit
	if (client == NULL) {
		exit(0);
	} else {
		// Otherwise, check if connected to server, and quit cleanly
		if (client->isConnected()) {
			client->sendServerMsg(CMD_ESCAPE + QUIT);
		}
	}
}

void ClientGUI::saveSettings() {
    // Instantiate new Client object with given server and port settings
    client = new Client(serverAddr->text().toStdString(),(unsigned short)serverPort->value());
    client->nick(nick->text().replace(QRegExp("[\\s\t]"),"").toStdString());
	client->bootstrap();

	// Connect listening socket event to action
	socketEvent = new QSocketNotifier(client->sockFd(), QSocketNotifier::Read);
	connect(socketEvent,SIGNAL(activated(int)),this,SLOT(processMsg()));

	// Rename server title
	tabs->setTabText(0,serverAddr->text());

    configPrompt->accept();
}

void ClientGUI::processMsg() {
	string msg = client->readSocket();
	// If command, perform action
	//if (isCmd(msg) && (str2cmd(msg)).isValid) {
	if (isCmd(msg)) {
		print("Response: " + msg);
		// Split cmd
		string cmd = msg.substr(1,msg.find(CMD_DELIM)-1);

		// Handle bootstrap return
		if (cmd.compare(RETPEERS) == 0) {
			// Clear old list of peers
			client->clearPeers();
			// string is formatted as IP:PORT,NICK;
			// explode command argument and insert into map of peers
			vector<string> v = explode(msg.substr(msg.find(CMD_DELIM)+1),';');
			// Enter each peer into map
			for (unsigned int i = 0; i < v.size(); i++) {
				vector<string> p = explode(v[i],',');
				client->addPeer(p[0],p[1]);
				consolidatePeers(client->getPeers());
			}
		}
	} else {
		appendChat(QString::fromStdString(msg));
	}
}

void ClientGUI::consolidatePeers(map<string,string> peers) {
	online->clear();
	map<string,string>::iterator it;
	for (it=peers.begin(); it != peers.end(); it++) {
		online->addItem(QString::fromStdString(it->first));
	}
}

void ClientGUI::msgPeer(QString nick) {
}

QString ClientGUI::msgToHtml(QString nick,QString text) {
	QString html;
	html = "<div><span style=\"color:#FF0000;\">" +
	        nick + ":&nbsp;</span>" +
	        "<span>" + text + "</span></div>";
	html.replace(QRegExp("\n"),"<br />");
	return html;
}

void ClientGUI::appendChat(QString html) {
    // Write to chat and scroll chat to end
    chat->setHtml(chat->toHtml() + html);
    chat->moveCursor(QTextCursor::End);
}