#include "mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMovie>
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
    /*setLayout(mainVLayout);   /* TODO: Investigate to understand why
                                 *       this is not working */

    setWindowTitle("LEDs Display Creator");
    setMinimumSize(mainVLayout->geometry().size().width(),
                   mainVLayout->geometry().size().height());
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
    updateSocketMovie("../../assets/someKindOfPortal.gif");

    startSvrAct->setEnabled(socketStatus);
    socketStatus = true;
    socketLabel->setText(QString("Socket status : %1").arg("On", 15));
    stopSvrAct->setEnabled(socketStatus);
}

void MainWindow::stopServer() {
    /* TODO */
    updateSocketMovie("../../assets/loading_wo_bg.gif");

    /* Set true, as at start, socketStatus == false */
    startSvrAct->setEnabled(true);
    socketStatus = false;
    socketLabel->setText(QString("Socket status : %1").arg("Off", 15));
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
    ipLabel->setText(QString("IP            : %1").arg(ipStr, 15));

    portLabel = new QLabel(QString("Port          : %1").arg(port, 15));
    portLabel->setAlignment(Qt::AlignLeft);
    portLabel->setFont({ "Source Code Pro" });
    portLabel->setGeometry(0, 0, portLabel->sizeHint().width(), 10);

    socketLabel = new QLabel(QString("Socket status : %1").arg(
                             socketStatus ? "On" : "Off", 15));
    socketLabel->setAlignment(Qt::AlignLeft);
    socketLabel->setFont({ "Source Code Pro" });
    socketLabel->setGeometry(0, 0, socketLabel->sizeHint().width(), 10);

    zoomPlusLabel = new QPushButton("+");
    zoomPlusLabel->setFont({ "Source Code Pro" });
    zoomPlusLabel->setFixedSize(20, 20);
    connect(zoomPlusLabel, &QPushButton::clicked,
            [=](bool clicked) {
                zoomSlider->setValue(zoomSlider->value() +
                                     zoomSlider->tickInterval());
            } );

    zoomMinusLabel = new QPushButton("-");
    zoomMinusLabel->setFont({ "Source Code Pro" });
    zoomMinusLabel->setFixedSize(20, 20);
    connect(zoomMinusLabel, &QPushButton::clicked,
            [=](bool clicked) {
                zoomSlider->setValue(zoomSlider->value() -
                                     zoomSlider->tickInterval());
            } );
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

    btn = new QPushButton;
    btn->setFixedSize(500, 500);
    btn->setStyleSheet("background-color: purple");
    connect(btn, &QPushButton::clicked, this, &MainWindow::close);

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
    logsTxtBox->setFixedWidth(ipLabel->size().width());
    logsTxtBox->clear();

    logsClearBtn = new QPushButton("Clear");
    logsClearBtn->setFixedWidth(100);
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
    zoomSlider->setTickInterval(10);
    zoomSlider->setMinimum(0);
    zoomSlider->setMaximum(200);
    zoomSlider->setValue(100);
    zoomSlider->setOrientation(Qt::Orientation::Horizontal);
    zoomSlider->setFixedSize(btn->size().width()/2, 50);

    socketMovieLabel = new QLabel;
    socketMovieLabel->setAlignment(Qt::AlignCenter);
    socketMovieLabel->setFixedSize(ipLabel->size().width(), 125);
    socketMovieLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    socketMovieLabel->setAutoFillBackground(false);
    socketMovie = new QMovie;
    socketMovie->setCacheMode(QMovie::CacheAll);
    socketMovieLabel->setMovie(socketMovie);
    updateSocketMovie("../../assets/loading_wo_bg.gif");

    rightJustifSpacers[0] = new QSpacerItem(50, 0, QSizePolicy::Expanding,
                                            QSizePolicy::Minimum);
    rightJustifSpacers[1] = new QSpacerItem(50, 0, QSizePolicy::Expanding,
                                          QSizePolicy::Minimum);
    rightJustifSpacers[2] = new QSpacerItem(50, 0, QSizePolicy::Expanding,
                                            QSizePolicy::Minimum);
    logsAlignmentSpacer   = new QSpacerItem(ipLabel->size().width()        -
                                              logsCheckBox->size().width() -
                                              logsClearBtn->size().width(), 0,
                                            QSizePolicy::Minimum,
                                            QSizePolicy::Minimum);
    socketMovieSpacer     = new QSpacerItem(0,
                                            socketMovieLabel->size().height(),
                                            QSizePolicy::Minimum,
                                            QSizePolicy::Minimum);
}

void MainWindow::createLayouts() {
    /* *** Objects creation *** */
    /* '-> Layouts ****** */
    toolsHLayout      = new QHBoxLayout;
    socketLogsVLayout = new QVBoxLayout;
    logsHLayout       = new QHBoxLayout;
    ledHLayout        = new QHBoxLayout;
    zoomHLayout       = new QHBoxLayout;
    mainVLayout       = new QVBoxLayout;

    /* *** Layouts filling *** */
    /* '-> Tools layouts (Drop-down menus to configure LED to put) ******* */
    toolsHLayout->addWidget(ledTypeDropDown);
    toolsHLayout->addWidget(ledPkgDropDown);
    toolsHLayout->addWidget(ledPkgGapLineEdit);
    toolsHLayout->addWidget(ledPkgUnitDropDown);
    toolsHLayout->addWidget(ledPlacementDropDown);
    toolsHLayout->addItem(rightJustifSpacers[0]);

    /* '-> Socket infos + Logs ****** */
    socketLogsVLayout->addWidget(ipLabel);
    socketLogsVLayout->addWidget(portLabel);
    socketLogsVLayout->addWidget(socketLabel);
    socketLogsVLayout->addWidget(socketMovieLabel, 0, Qt::AlignmentFlag::AlignCenter);
    socketLogsVLayout->addSpacerItem(socketMovieSpacer);
    logsHLayout->addWidget(logsCheckBox);
    logsHLayout->addItem(logsAlignmentSpacer);
    logsHLayout->addWidget(logsClearBtn);
    socketLogsVLayout->addLayout(logsHLayout);
    socketLogsVLayout->addWidget(logsTxtBox);
    socketLogsVLayout->addStretch();

    /* '-> Display/Creation area + Socket & Logs ****** */
    ledHLayout->addWidget(btn/* Drawing area */);
    ledHLayout->addLayout(socketLogsVLayout);
    ledHLayout->addItem(rightJustifSpacers[1]);

    /* '-> Zoom layout with Slider & Clickable Labels */
    zoomHLayout->addWidget(zoomMinusLabel);
    zoomHLayout->addWidget(zoomSlider);
    zoomHLayout->addWidget(zoomPlusLabel);
    zoomHLayout->addItem(rightJustifSpacers[2]);

    /* '-> Main Layout ****** */
    mainVLayout->addLayout(toolsHLayout);
    mainVLayout->addLayout(ledHLayout);
    mainVLayout->addLayout(zoomHLayout);
    mainVLayout->addStretch();
}

void MainWindow::updateSocketMovie(QString filename) {
    static int lblW = socketMovieLabel->size().width(),
               lblH = socketMovieLabel->size().height();
    static int movieW = 0, movieH = 0;

    /* Stop to change filename */
    socketMovie->stop();
    socketMovie->setFileName(filename);

    /* Jump to 1st frame to get its dimension */
    socketMovie->jumpToFrame(0);
    movieW = socketMovie->currentImage().size().width();
    movieH = socketMovie->currentImage().size().height();

    if (movieW <= movieH) {
        socketMovie->setScaledSize({ movieW * lblH / movieH, lblH });
    } else {
        socketMovie->setScaledSize({ lblW, lblW * movieH / movieW });
    }

    socketMovie->start();
}

void MainWindow::updateSocketMovie(const char *filename) {
    updateSocketMovie(QString(filename));
}
