#include "mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QPushButton>
#include <QSlider>
#include <QSpacerItem>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    createActions();
    createMenus();

    createLabels();
    createDropDownMenus();
    createInteractives();
    createLayouts();

    QWidget *widget = new QWidget();
    widget->setLayout(mainVLayout);
    setCentralWidget(widget);
    /*setLayout(mainVLayout);   /* Investigate to understand why this
                                 * is not working */

    setWindowTitle("LEDs Display Creator");
    setMinimumSize(300, 300);
    resize(480, 320);
}

MainWindow::~MainWindow() {}

/* *** File actions ******************************************************** */
void MainWindow::saveDesign() {
    /* TODO */
}

void MainWindow::loadDesign() {
    /* TODO */
}

/* *** Design actions ****************************************************** */
void MainWindow::undoAction() {
    /* TODO */
}

void MainWindow::emptyDesign() {
    /* TODO */
}

void MainWindow::infoLedsCount() {
    /* TODO */
}

void MainWindow::infoSizeIrl() {
    /* TODO */
}

/* *** TCP Socket actions ************************************************** */
void MainWindow::cfgSocketInfos() {
    /* TODO */
}

void MainWindow::startServer() {
    /* TODO */

    startSvrAct->setEnabled(socketStatus);
    socketStatus = true;
    socketLabel->setText(QString("Socket status : On"));
    stopSvrAct->setEnabled(socketStatus);
}

void MainWindow::stopServer() {
    /* TODO */

    /* Set true, as at start, socketStatus == false */
    startSvrAct->setEnabled(true);
    socketStatus = false;
    socketLabel->setText(QString("Socket status : Off"));
    stopSvrAct->setEnabled(socketStatus);
}

void MainWindow::createActions() {
    /* File actions *************************************************** */
    /* '-> Save ****** */
    saveAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew),
                          tr("&Save design"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save current design"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveDesign);

    /* '-> Load ****** */
    loadAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew),
                          tr("&Load design"), this);
    loadAct->setShortcuts(QKeySequence::Open);
    loadAct->setStatusTip(tr("Load existing design"));
    connect(loadAct, &QAction::triggered, this, &MainWindow::loadDesign);

    /* '-> Quit ****** */
    exitAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew),
                          tr("&Quit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Quit application"));
    connect(exitAct, &QAction::triggered, this, &MainWindow::close);

    /* Design actions ************************************************* */
    /** Undo last action ****** */
    undoAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::Battery),
                          tr("&Undo "), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo last design's action"));
    connect(undoAct, &QAction::triggered,
            this, &MainWindow::undoAction);

    /** Empty design ****** */
    emptyDesignAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::Battery),
                                 tr("&Empty design"), this);
    emptyDesignAct->setStatusTip(tr("Remove all LEDs in design"));
    connect(emptyDesignAct, &QAction::triggered,
            this, &MainWindow::emptyDesign);

    /* '-> Infos ****** */
    /*    '-> Number of LEDs ****** */
    infoLedCountAct = new QAction(QIcon::fromTheme(
                                      QIcon::ThemeIcon::DocumentNew),
                                  tr("# of LEDs"), this);
    infoLedCountAct->setStatusTip(tr("Get number of LEDs in design"));
    connect(infoLedCountAct, &QAction::triggered,
            this, &MainWindow::infoLedsCount);

    /*    '-> Lifesized dims of design ****** */
    infoSizeIrlAct = new QAction(QIcon::fromTheme(
                                    QIcon::ThemeIcon::DocumentNew),
                                 tr("Real life size of design"), this);
    infoSizeIrlAct->setStatusTip(tr("Get lifesized dimensions of design"));
    connect(infoSizeIrlAct, &QAction::triggered,
            this, &MainWindow::infoSizeIrl);

    /* TCP Socket actions ********************************************* */
    /* '-> Start socket ****** */
    startSvrAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew),
                              tr("St&art server"), this);
    startSvrAct->setStatusTip(tr("Start server socket"));
    startSvrAct->setEnabled(true);
    connect(startSvrAct, &QAction::triggered, this, &MainWindow::startServer);

    /* '-> Stop socket ****** */
    stopSvrAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew),
                             tr("St&op server"), this);
    stopSvrAct->setStatusTip(tr("Stop server socket"));
    stopSvrAct->setEnabled(false);
    connect(stopSvrAct, &QAction::triggered, this, &MainWindow::stopServer);

    /* '-> Customize socket's params ****** */
    cfgSocketAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew),
                               tr("&Configure socket"), this);
    cfgSocketAct->setStatusTip(tr("Configure socket's IP & Port"));
    connect(cfgSocketAct, &QAction::triggered,
            this, &MainWindow::cfgSocketInfos);
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(saveAct);
    fileMenu->addAction(loadAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    designMenu = menuBar()->addMenu(tr("&Design"));
    designMenu->addAction(undoAct);
    designMenu->addAction(emptyDesignAct);
    /* Infos sub-menu */
    infosSubMenu = designMenu->addMenu(tr("&Infos"));
    infosSubMenu->addAction(infoLedCountAct);
    infosSubMenu->addAction(infoSizeIrlAct);

    tcpSocketMenu = menuBar()->addMenu(tr("&TCP Socket"));
    tcpSocketMenu->addAction(startSvrAct);
    tcpSocketMenu->addAction(stopSvrAct);
    tcpSocketMenu->addSeparator();
    tcpSocketMenu->addAction(cfgSocketAct);
}

void MainWindow::createLabels() {
    /* Hardcoded IP for sizeHint.width() to be to the longest possible value */
    ipLabel = new QLabel("IP            : 255.255.255.255");
    ipLabel->setAlignment(Qt::AlignLeft);
    /*ipLabel->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    ipLabel->setBackgroundRole(QPalette::Dark);
    ipLabel->setAutoFillBackground(true);*/
    ipLabel->setFont({ "Source Code Pro" });
    ipLabel->setGeometry(0, 0, ipLabel->sizeHint().width(), 10);
    ipLabel->setText(QString("IP            : %1").arg(ipStr));

    portLabel = new QLabel(QString("Port          : %1").arg(port));
    portLabel->setAlignment(Qt::AlignLeft);
    portLabel->setFont({ "Source Code Pro" });
    portLabel->setGeometry(0, 0, portLabel->sizeHint().width(), 10);

    socketLabel = new QLabel(QString("Socket status : %1").arg(
                             socketStatus ? "On" : "Off"));
    socketLabel->setAlignment(Qt::AlignLeft);
    socketLabel->setFont({ "Source Code Pro" });
    socketLabel->setGeometry(0, 0, socketLabel->sizeHint().width(), 10);
}
void MainWindow::createDropDownMenus() {
    ledTypeDropDown      = new QComboBox;
    ledPkgDropDown       = new QComboBox;
    ledPkgUnitDropDown   = new QComboBox;
    ledPlacementDropDown = new QComboBox;

    ledTypeDropDown->addItem("WS2812");
    ledTypeDropDown->addItem("SK6812");
    ledTypeDropDown->setFixedSize(ledTypeDropDown->sizeHint().width(),
                                  ledTypeDropDown->sizeHint().height());
    connect(ledTypeDropDown, &QComboBox::currentIndexChanged,
            [=](int index) {
                if (logsTxtBox->isEnabled())
                    logsTxtBox->append(
                        QString("Drop-down \"LED Type\": [%1] %2").arg(
                                ledTypeDropDown->currentIndex()).arg(
                                ledTypeDropDown->currentText())
                    );
            } );

    ledPkgDropDown->addItem("5050");
    ledPkgDropDown->addItem("2020");
    ledPkgDropDown->setFixedSize(ledPkgDropDown->sizeHint().width(),
                                 ledPkgDropDown->sizeHint().height());
    connect(ledPkgDropDown, &QComboBox::currentIndexChanged,
            [=](int index) {
                if (logsTxtBox->isEnabled())
                    logsTxtBox->append(
                        QString("Drop-down \"LED Packaging\": [%1] %2").arg(
                                ledPkgDropDown->currentIndex()).arg(
                                ledPkgDropDown->currentText())
                    );
            } );

    ledPkgUnitDropDown->addItem("[mm]");
    ledPkgUnitDropDown->addItem("[inch]");
    ledPkgUnitDropDown->setFixedSize(ledPkgUnitDropDown->sizeHint().width(),
                                     ledPkgUnitDropDown->sizeHint().height());
    connect(ledPkgUnitDropDown, &QComboBox::currentIndexChanged,
            [=](int index) {
                if (index == 0) {
                    ledPkgGapLineEdit->setText(QString("%1").arg(
                        ledPkgGapLineEdit->text().toFloat() * 25.4));
                } else {
                    ledPkgGapLineEdit->setText(QString("%1").arg(
                        ledPkgGapLineEdit->text().toFloat() / 25.4));
                }

                if (logsTxtBox->isEnabled())
                    logsTxtBox->append(
                        QString("Drop-down \"LED Gap unit\": [%1] %2 | "
                                "New value: %3").arg(
                                ledPkgUnitDropDown->currentIndex()).arg(
                                ledPkgUnitDropDown->currentText()).arg(
                                ledPkgGapLineEdit->text())
                );
            } );

    ledPlacementDropDown->addItem("Single");
    ledPlacementDropDown->addItem("Strip");
    ledPlacementDropDown->setFixedSize(
        ledPlacementDropDown->sizeHint().width(),
        ledPlacementDropDown->sizeHint().height());
    connect(ledPlacementDropDown, &QComboBox::currentIndexChanged,
            [=](int index) {
                if (logsTxtBox->isEnabled())
                    logsTxtBox->append(
                        QString("Drop-down \"LED Placement\": [%1] %2").arg(
                                ledPlacementDropDown->currentIndex()).arg(
                                ledPlacementDropDown->currentText())
                    );
            } );
}

void MainWindow::createInteractives() {

    for (auto& btn : btns) {
        btn = new QPushButton;
        btn->setFixedSize(50, 50);
    }

    btns[0]->setFixedSize(500, 500);
    btns[0]->setStyleSheet("background-color: purple");
    connect(btns[0], &QPushButton::clicked, this, &MainWindow::close);

    btns[1]->setStyleSheet("background-color: cyan");
    connect(btns[1], &QPushButton::clicked, this, [=](bool clikced) {
        if (logsTxtBox->isEnabled())
            logsTxtBox->append("BTN: Clicked");
    });

    ledPkgGapLineEdit = new QLineEdit;
    ledPkgGapLineEdit->setText(QString("0"));
    ledPkgGapLineEdit->setMaxLength(5);
    ledPkgGapLineEdit->setFixedSize(50,
                                    ledTypeDropDown->sizeHint().height());

    logsTxtBox = new QTextEdit;
    logsTxtBox->setEnabled(false);
    logsTxtBox->setVisible(false);
    logsTxtBox->setReadOnly(true);  // RO as it is used for logs
    logsTxtBox->setFont({ "Source Code Pro" });
    logsTxtBox->clear();

    logsClearBtn = new QPushButton("Clear");
    logsClearBtn->setEnabled(false);    /* Enable button, only
                                         * when checkbox checked */
    connect( logsClearBtn, &QPushButton::clicked,
             [=](bool checked){ logsTxtBox->clear(); } );

    logsCheckBox = new QCheckBox(QString("Show logs"));
    logsCheckBox->setCheckState(Qt::Unchecked);
    connect(logsCheckBox, &QCheckBox::stateChanged,
            [=](int checked) {
                logsClearBtn->setEnabled(checked);
                logsTxtBox->setEnabled(checked);
                logsTxtBox->setVisible(checked);
            } );

    zoomSlider = new QSlider;
    zoomSlider->setOrientation(Qt::Orientation::Horizontal);

    rowSpacers[0] = new QSpacerItem(50, 0, QSizePolicy::Expanding,
                                    QSizePolicy::Minimum);
    rowSpacers[1] = new QSpacerItem(50, 0, QSizePolicy::Expanding,
                                    QSizePolicy::Minimum);
}

void MainWindow::createLayouts() {
    /* *** Objects creation *** */
    /* '-> Layouts ****** */
    toolsHLayout      = new QHBoxLayout;
    socketLogsVLayout = new QVBoxLayout;
    logsHLayout       = new QHBoxLayout;
    ledHLayout        = new QHBoxLayout;
    mainVLayout       = new QVBoxLayout;
    //zoomHLayout     = new QHBoxLayout;

    /* *** Layouts filling *** */
    /* '-> Tools layouts (Drop-down menus to configure LED to put) ******* */
    toolsHLayout->addWidget(ledTypeDropDown);
    toolsHLayout->addWidget(ledPkgDropDown);
    toolsHLayout->addWidget(ledPkgGapLineEdit);
    toolsHLayout->addWidget(ledPkgUnitDropDown);
    toolsHLayout->addWidget(ledPlacementDropDown);
    toolsHLayout->addItem(rowSpacers[0]);   // Compact to the right with spacer

    /* '-> Socket infos + Logs ****** */
    socketLogsVLayout->addWidget(ipLabel);
    socketLogsVLayout->addWidget(portLabel);
    socketLogsVLayout->addWidget(socketLabel);
    socketLogsVLayout->addWidget(btns[1]/* QMovie for animated socket status */);
    logsHLayout->addWidget(logsCheckBox);
    logsHLayout->addWidget(logsClearBtn);
    socketLogsVLayout->addLayout(logsHLayout);
    socketLogsVLayout->addWidget(logsTxtBox);
    socketLogsVLayout->addStretch();

    /* '-> Display/Creation area + Socket & Logs ****** */
    ledHLayout->addWidget(btns[0]);
    ledHLayout->addLayout(socketLogsVLayout);
    ledHLayout->addItem(rowSpacers[1]);

    /* '-> Main Layout ****** */
    mainVLayout->addLayout(toolsHLayout, Qt::AlignmentFlag::AlignRight);
    mainVLayout->addLayout(ledHLayout);
    mainVLayout->addWidget(zoomSlider);
    mainVLayout->addStretch();
}
