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


	// Construct tabs
	tabPt = new map<QString,Tab*>;
	tabs = new CloseableTabWidget(tabPt);
	tabs->setDocumentMode(true);
	tabs->setTabPosition(QTabWidget::North);
	tabs->setTabsClosable(true);

	Tab *groupChat = new GroupTab(this);
	groupChat->setDisabled(true);	// Disable until connected to server
	tabPt->insert(std::pair<QString,Tab*>(ROOTTAB,groupChat));
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

    QPushButton *conn = new QPushButton("Connect");
    connect(conn,SIGNAL(clicked()),this,SLOT(serverConnect()));
    QPushButton *quit = new QPushButton("Quit");
    connect(quit,SIGNAL(clicked()),this,SLOT(quit()));

    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->addLayout(serverSettings);
    configLayout->addLayout(userSettings);
    configLayout->addWidget(conn);
    configLayout->addWidget(quit);

    configPrompt->setLayout(configLayout);

    configPrompt->exec();
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

void ClientGUI::serverConnect() {
    // Instantiate new Client object with given server and port settings
    client = new Client(serverAddr->text().toStdString(),(unsigned short)serverPort->value());
	getRootTab()->passClient(client);

    client->nick(nick->text().replace(QRegExp("[\\s\t]"),"").toStdString());
	client->bootstrap();

	// Connect listening socket event to action
	socketEvent = new QSocketNotifier(client->sockFd(), QSocketNotifier::Read);
	connect(socketEvent,SIGNAL(activated(int)),this,SLOT(processMsg()));

	// Rename server title
	tabs->setTabText(0,serverAddr->text());

	// Enable server tab
	getRootTab()->setEnabled(true);

    configPrompt->accept();
}

void ClientGUI::processMsg() {
	string msg = client->readSocket();

	// If command, perform action
	//if (isCmd(msg) && (str2cmd(msg)).isValid) {
	if (isCmd(msg)) {
		print("Response: " + msg);
		// Split cmd
		//string cmd = msg.substr(1,msg.find(CMD_DELIM)-1);
		Command c = str2cmd(msg);

		// Check if from peer
		if (c.cmd.compare(FROM) == 0) {
			QString nick = QString::fromStdString(c.args[0]);
			QString peerMsg = QString::fromStdString(c.args[1]);
			map<QString,Tab*>::iterator it;
			// See if tab is open
			if ((it = tabPt->find(nick)) != tabPt->end()) {
				// Append peer's message
				it->second->appendChat(msgToHtml(nick,peerMsg,true));
			} else {
				// Open new tab
				// First, ensure that peer is in your peer list
				QString ipp = getRootTab()->peerFromList(nick);
				if (!ipp.isNull()) {
					// Make new peer tab and pass client
					PeerTab *pt = newPeerTab(ipp,nick);
					pt->passClient(client);
					pt->appendChat(msgToHtml(nick,peerMsg,true));
				}
			}
		}

		// Handle bootstrap return
		if (c.cmd.compare(RETPEERS) == 0) {
			// Reset nickname in case nickname was taken already
			client->nick(c.args[0]);

			// Clear old list of peers
			client->clearPeers();

			// Consolidate and add new peers
			// string is formatted as IP:PORT,NICK;
			// explode map argument and insert into map of peers
			vector<string> v = explode(c.args[1],';');
			// Enter each peer into map
			for (unsigned int i = 0; i < v.size(); i++) {
				vector<string> p = explode(v[i],',');
				client->addPeer(p[0],p[1]);
			}
			getRootTab()->consolidatePeers(client->getPeers(),tabPt,tabs);
		}
	} else {
		getRootTab()->appendChat(QString::fromStdString(msg));
	}
}

PeerTab* ClientGUI::newPeerTab(QString ipp, QString peerNick) {
	// If tab is open, give it focus
	if (tabPt->find(peerNick) != tabPt->end()) {
		tabs->setCurrentIndex(tabs->indexOf(tabPt->find(peerNick)->second));
		return (PeerTab *)tabPt->find(peerNick)->second;
	} else {
		// Create new tab, pass it the client, and keep track of its pointer
		PeerTab *pt = new PeerTab(ipp,peerNick);
		pt->passClient(client);
		pt->setToolTip(ipp);
		tabPt->insert(std::pair<QString,Tab*>(peerNick,pt));

		// Add tab to tab bar
		tabs->addTab(pt,peerNick);
		return pt;
	}
}

void ClientGUI::startPeerChat(QListWidgetItem* peer) {
	// Make sure you're not trying to chat with yourself
	if (peer->text().toStdString().compare(client->nick()) != 0) {
		// If tab is open, give it focus
		if (tabPt->find(peer->text()) != tabPt->end()) {
			tabs->setCurrentIndex(tabs->indexOf(tabPt->find(peer->text())->second));
		} else {
			// Create new tab and add to tab bar
			PeerTab* pt = newPeerTab(peer->toolTip(),peer->text());
			// Give tab focus
			tabs->setCurrentIndex(tabs->indexOf(pt));
		}
	}
}

GroupTab* ClientGUI::getRootTab() {
	return (GroupTab *) tabPt->find(ROOTTAB)->second;
}

QTabWidget* ClientGUI::getTabs() {
	return tabs;
}



Tab::Tab(QWidget *parent) : QWidget (parent) {
    // Will contain chat window and list of online peers
	outputSplitter = new QSplitter;
    // Will split the outputSplitter from the input line
    inputSplitter = new QSplitter;
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

    // Add chat viewer and online list to splitter
    outputSplitter->addWidget(chatContainer);

    groupLayout->addWidget(inputSplitter);
    groupLayout->setMargin(10);
    groupLayout->setSpacing(5);

	setLayout(groupLayout);

	this->setParent(parent);
}

void Tab::passClient(Client *c) {
	client = c;
}

bool Tab::eventFilter(QObject *dist, QEvent *event) {
    // Check for keypress
    if (event->type() == QEvent::KeyPress) {
        // Cast to keyEvent
        QKeyEvent *e = static_cast<QKeyEvent *>(event);
        // Check if return is pressed
        if ((e->key() == Qt::Key_Return)) {
            // Make sure enter is not escaped w/ alt modifier
            if (e->modifiers() != Qt::AltModifier) {
		        this->submitChatInput();
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

void Tab::submitChatInput() {
    //string input = chatInput->toPlainText().toUtf8().constData();
	QString input = chatInput->toPlainText();
	QString inputHtml = msgToHtml(QString::fromStdString(client->nick()),chatInput->toPlainText());

	client->sendServerMsg(string(input.toUtf8().constData()));
	appendChat(inputHtml);

    // Clear text input
    chatInput->clear();
}

void Tab::appendChat(QString html) {
    // Write to chat and scroll chat to end
    chat->setHtml(chat->toHtml() + html);
    chat->moveCursor(QTextCursor::End);
}



GroupTab::GroupTab(QWidget *parent) : Tab(parent){
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

	// Enable double clicking on list items to message peer
	connect(online,SIGNAL(itemDoubleClicked(QListWidgetItem*)),parent,SLOT(startPeerChat(QListWidgetItem*)));

    outputSplitter->addWidget(onlineContainer);
    outputSplitter->setStretchFactor(0,7);
    outputSplitter->setStretchFactor(1,3);

	// Set base hue
	hueSeed = 0;
}

void GroupTab::consolidatePeers(map<string,string> peers, map<QString,Tab*> *tabPt, QTabWidget *tabs) {
	// Create a map of tabs open by <IP:Port,*Tab>
	map<QString,Tab*> ipMap;
	map<QString,Tab*>::iterator it1;
	for (it1 = tabPt->begin(); it1 != tabPt->end(); it1++) {
		ipMap.insert(std::pair<QString,Tab*>(it1->second->toolTip(),it1->second));
	}

	// Iterator for peer map
	map<string,string>::iterator it;

	// Clear old peers
	online->clear();

	for (it=peers.begin(); it != peers.end(); it++) {
		// Check if tab is open by IP
		map<QString,Tab*>::iterator oldTab = ipMap.find(QString::fromStdString(it->second));

		if (oldTab != ipMap.end()) {
			// If open, enable and set nickname
			oldTab->second->setEnabled(true);
			// Change peername in tabPt
			tabPt->erase(tabs->tabText(tabs->indexOf(oldTab->second)));
			tabPt->insert(std::pair<QString,Tab*>(QString::fromStdString(it->first),oldTab->second));
			// Set tab text to new peer name
			tabs->setTabText(tabs->indexOf(oldTab->second),QString::fromStdString(it->first));
			// Change peername in PeerTab
			(static_cast<PeerTab *>(oldTab->second))->peer(QString::fromStdString(it->first));
		}

		QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(it->first));

		// Set tooltip as IP:Port
		item->setToolTip(QString::fromStdString(it->second));

		online->addItem(item);
		// Check if it's you
		if (client->nick().compare(it->first) == 0) {
			// Set background color to red
			item->setBackgroundColor(QColor(255,0,0,20));
		} else {
			// Assign color to peers
			// Make it look good to (use scaling hue along HSV scale)
			item->setTextColor(QColor::fromHsv(nextHue(),255,175));
		}
	}
}

QString GroupTab::peerFromList(QString nick) {
	QList<QListWidgetItem*> items = online->findItems(nick,Qt::MatchExactly);
	if (items.size() == 1) {
		return items[0]->toolTip();
	} else {
		return QString();
	}
}

int GroupTab::nextHue() {
	hueSeed += 127;
	return hueSeed % 360;
}



PeerTab::PeerTab(QString ipp, QString peer, QWidget *parent) : Tab(parent) {
	this->_ip = ipp.split(":").at(0);
	this->_port = ipp.split(":").at(1).toUShort();
	this->_peer = peer;
	chatInput->installEventFilter(this);
}

void PeerTab::submitChatInput() {
	string fromCmd = CMD_ESCAPE + FROM + CMD_DELIM + client->nick() + CMD_DELIM;
	QString input =  QString::fromStdString(fromCmd) + chatInput->toPlainText();
	QString inputHtml = msgToHtml(QString::fromStdString(client->nick()),chatInput->toPlainText());

	client->sendPeerMsg(string(input.toUtf8().constData()),_peer.toStdString());
	appendChat(inputHtml);

    // Clear text input
    chatInput->clear();
}

bool PeerTab::eventFilter(QObject *dist, QEvent *event) {
    // Check for keypress
    if (event->type() == QEvent::KeyPress) {
        // Cast to keyEvent
        QKeyEvent *e = static_cast<QKeyEvent *>(event);
        // Check if return is pressed
        if ((e->key() == Qt::Key_Return)) {
            // Make sure enter is not escaped w/ alt modifier
            if (e->modifiers() != Qt::AltModifier) {
		        this->submitChatInput();
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

void PeerTab::peer(QString peer) {
	_peer = peer;
}


CloseableTabWidget::CloseableTabWidget(map<QString,Tab*> *t, QWidget* parent) : QTabWidget(parent) {
	this->tabPt = t;
	this->setParent(parent);
	this->setMovable(true);
	connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
}


void CloseableTabWidget::closeTab(int i) {
	// Make sure it's not the server chat window
	if (i != this->indexOf(tabPt->find(ROOTTAB)->second)) {
		// Erase from list of Tab pointers so it can be opened agaij
		tabPt->erase(this->tabText(i));
		this->removeTab(i);
	}
}



QString msgToHtml(QString nick,QString text, bool frompeer) {
	QString html;
	QString color = (frompeer) ? "#0000FF" : "#FF0000";
	html = "<div><span style=\"color:" + color + ";\">" +
			nick + ":&nbsp;</span>" +
	        "<span>" + text + "</span></div>";
	html.replace(QRegExp("\n"),"<br />");
	return html;
}