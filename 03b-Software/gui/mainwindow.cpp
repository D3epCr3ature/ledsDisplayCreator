#include "mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QMovie>
#include <QPushButton>
#include <QSlider>
#include <QSpacerItem>
#include <QTextEdit>

#include "structure/display.h"

#define HEXA_16BITS_NDIGITS  2*sizeof(uint16_t)
#define HEXA_32BITS_NDIGITS  2*sizeof(uint32_t)
#define NUMERICAL_BASE_16   16
#define TO_UINT8(DIGIT) (DIGIT) - '0'

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    display = new DynamicDisplay;

    display->setSceneRect(0, 0, 5000, 5000);
    display->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    display->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    display->setMinimumSize(300, 300);

    createMenus();
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
    saveDisplay(display->getDisplay());
}

void MainWindow::loadDesign() {
    struct LEDDisplay tmp;

    if ( ! openDisplay(tmp) ) {
        return;
    }

    display->setDisplay(tmp);
    display->updateScene();
}

/* *** Design actions ****************************************************** */
void MainWindow::undoAction() {
    /* TODO */
}

void MainWindow::emptyDesign() {
    display->clearScene();
}

void MainWindow::infoLedsCount() {
    /* TODO */
    if (logsTxtBox->isEnabled())
        logsTxtBox->append(
            QString("# of LEDs: %1").arg(display->getNumberOfLeds()) );
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
    if ( ! tcpServer->listen(QHostAddress("127.0.0.1"), 5000) ) {
        QMessageBox::critical(this, tr("Socket's server"),
                              tr("Unable to start the server: %1.")
                                  .arg(tcpServer->errorString()));
        close();
        return;
    }

    startSvrAct->setEnabled(false);
    scktStatus = true;
    scktLbl->setText(QString("Socket status : %1").arg("On", 15));
    stopSvrAct->setEnabled( ! startSvrAct->isEnabled() );

    replaceSocketMovieWith(scktMovConnect);
}

void MainWindow::stopServer() {
    /* TODO */
    if (cltConnection) {
        /* Send a leaving message to inform the client and
             * allow it to reset its state as
             * QTcpSocket::disconnectFromHost() is used below */
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setByteOrder(QDataStream::LittleEndian);

        /* /!\ Does not work properly using C string (out << "Hi";) /!\
             * Answer: It might be because a C-string is then cast as
             *         a QString and this stores each data
             *         as uint16_t instead of uint8_t */
        out << QByteArray("Leaving");

        cltConnection->write(block);
        cltConnection->flush();

        cltConnection->disconnectFromHost();
        cltConnection = nullptr;
    }
    tcpServer->close();

    /* Set true because, at start, socketStatus == false */
    startSvrAct->setEnabled(true);
    scktStatus = false;
    scktLbl->setText(QString("Socket status : %1").arg("Off", 15));
    stopSvrAct->setEnabled( ! startSvrAct->isEnabled() );

    replaceSocketMovieWith(scktMovDisconn);
}

void MainWindow::createActions() {
    /* File actions *************************************************** */
    /** Save ****** */
    saveAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew),
                          tr("&Save design"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save current design"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveDesign);

    /** Load ****** */
    loadAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew),
                          tr("&Load design"), this);
    loadAct->setShortcuts(QKeySequence::Open);
    loadAct->setStatusTip(tr("Load existing design"));
    connect(loadAct, &QAction::triggered, this, &MainWindow::loadDesign);

    /** Quit ****** */
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

    /** Infos ****** */
    /*** Number of LEDs ****** */
    infoLedCountAct = new QAction(QIcon::fromTheme(
                                      QIcon::ThemeIcon::DocumentNew),
                                  tr("# of LEDs"), this);
    infoLedCountAct->setStatusTip(tr("Get number of LEDs in design"));
    connect(infoLedCountAct, &QAction::triggered,
            this, &MainWindow::infoLedsCount);

    /*** Lifesized dims of design ****** */
    infoSizeIrlAct = new QAction(QIcon::fromTheme(
                                    QIcon::ThemeIcon::DocumentNew),
                                 tr("Real life size of design"), this);
    infoSizeIrlAct->setStatusTip(tr("Get lifesized dimensions of design"));
    connect(infoSizeIrlAct, &QAction::triggered,
            this, &MainWindow::infoSizeIrl);

    /* TCP Socket actions ********************************************* */
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection,
            this, &MainWindow::connectionSucessToClient);

    inStream.setByteOrder(QDataStream::LittleEndian);

    /** Start socket ****** */
    startSvrAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew),
                              tr("St&art server"), this);
    startSvrAct->setStatusTip(tr("Start server socket"));
    startSvrAct->setEnabled(true);
    connect(startSvrAct, &QAction::triggered, this, &MainWindow::startServer);

    /** Stop socket ****** */
    stopSvrAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew),
                             tr("St&op server"), this);
    stopSvrAct->setStatusTip(tr("Stop server socket"));
    stopSvrAct->setEnabled(false);
    connect(stopSvrAct, &QAction::triggered, this, &MainWindow::stopServer);

    /** Customize socket's params ****** */
    cfgSocketAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew),
                               tr("&Configure socket"), this);
    cfgSocketAct->setStatusTip(tr("Configure socket's IP & Port"));
    connect(cfgSocketAct, &QAction::triggered,
            this, &MainWindow::cfgSocketInfos);
}

void MainWindow::createMenus() {
    /* *** Create actions that will fill menus *** */
    createActions();

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(saveAct);
    fileMenu->addAction(loadAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    designMenu = menuBar()->addMenu(tr("&Design"));
    designMenu->addAction(undoAct);
    designMenu->addAction(emptyDesignAct);
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
    ipLbl = new QLabel("IP            : 255.255.255.255");
    ipLbl->setAlignment(Qt::AlignLeft);
    ipLbl->setFont({ "Source Code Pro" });
    ipLbl->setGeometry(0, 0, ipLbl->sizeHint().width(), 10);
    ipLbl->setText(QString("IP            : %1").arg(ipStr, 15));

    portLbl = new QLabel(QString("Port          : %1").arg(port, 15));
    portLbl->setAlignment(Qt::AlignLeft);
    portLbl->setFont({ "Source Code Pro" });
    portLbl->setGeometry(0, 0, portLbl->sizeHint().width(), 10);

    scktLbl = new QLabel(QString("Socket status : %1").arg(
                            scktStatus ? "On" : "Off", 15));
    scktLbl->setAlignment(Qt::AlignLeft);
    scktLbl->setFont({ "Source Code Pro" });
    scktLbl->setGeometry(0, 0, scktLbl->sizeHint().width(), 10);

    scktMovieLbl = new QLabel;
    scktMovieLbl->setAlignment(Qt::AlignCenter);
    scktMovieLbl->setFixedSize(ipLbl->size().width(), 150);
    scktMovieLbl->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    scktMovieLbl->setAutoFillBackground(false);
    createQMovies();
    replaceSocketMovieWith(scktMovDisconn);

    zoomPlusLbl = new QPushButton("+");
    zoomPlusLbl->setFont({ "Source Code Pro" });
    zoomPlusLbl->setFixedSize(20, 20);
    connect(zoomPlusLbl, &QPushButton::clicked,
            [=](bool clicked) {
                zoomSlider->setValue(zoomSlider->value() +
                                     zoomSlider->tickInterval());
            } );

    zoomMinusLbl = new QPushButton("-");
    zoomMinusLbl->setFont({ "Source Code Pro" });
    zoomMinusLbl->setFixedSize(20, 20);
    connect(zoomMinusLbl, &QPushButton::clicked,
            [=](bool clicked) {
                zoomSlider->setValue(zoomSlider->value() -
                                     zoomSlider->tickInterval());
            } );
}

void MainWindow::createDropDownMenus() {
    ledTypeDrpDn = new QComboBox;
    ledTypeDrpDn->addItem("WS2812");
    ledTypeDrpDn->addItem("SK6812");
    ledTypeDrpDn->setFixedSize(ledTypeDrpDn->sizeHint().width(),
                               ledTypeDrpDn->sizeHint().height());
    connect(ledTypeDrpDn, &QComboBox::currentIndexChanged,
            [=](int index) {
                if (logsTxtBox->isEnabled())
                    logsTxtBox->append(
                        QString("Drop-down \"LED Type\": [%1] %2").arg(
                                ledTypeDrpDn->currentIndex()).arg(
                                ledTypeDrpDn->currentText())
                    );
            } );

    ledPkgDrpDn = new QComboBox;
    ledPkgDrpDn->addItem("5050");
    ledPkgDrpDn->addItem("2020");
    ledPkgDrpDn->setFixedSize(ledPkgDrpDn->sizeHint().width(),
                              ledPkgDrpDn->sizeHint().height());
    connect(ledPkgDrpDn, &QComboBox::currentIndexChanged,
            [=](int index) {
                if (logsTxtBox->isEnabled())
                    logsTxtBox->append(
                        QString("Drop-down \"LED Packaging\": [%1] %2").arg(
                                ledPkgDrpDn->currentIndex()).arg(
                                ledPkgDrpDn->currentText())
                    );
            } );

    ledPkgGapLineEdit = new QLineEdit;
    ledPkgGapLineEdit->setText(QString("0"));
    ledPkgGapLineEdit->setMaxLength(5);
    ledPkgGapLineEdit->setFixedSize(50, ledTypeDrpDn->sizeHint().height());

    ledPkgUnitDrpDn = new QComboBox;
    ledPkgUnitDrpDn->addItem("[mm]");
    ledPkgUnitDrpDn->addItem("[inch]");
    ledPkgUnitDrpDn->setFixedSize(ledPkgUnitDrpDn->sizeHint().width(),
                                  ledPkgUnitDrpDn->sizeHint().height());
    connect(ledPkgUnitDrpDn, &QComboBox::currentIndexChanged,
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
                                ledPkgUnitDrpDn->currentIndex()).arg(
                                ledPkgUnitDrpDn->currentText()).arg(
                                ledPkgGapLineEdit->text())
                );
            } );

    ledPlacementDrpDn = new QComboBox;
    ledPlacementDrpDn->addItem("Single");
    ledPlacementDrpDn->addItem("Strip");
    ledPlacementDrpDn->setFixedSize(ledPlacementDrpDn->sizeHint().width(),
                                    ledPlacementDrpDn->sizeHint().height());
    connect(ledPlacementDrpDn, &QComboBox::currentIndexChanged,
            [=](int index) {
                if (logsTxtBox->isEnabled())
                    logsTxtBox->append(
                        QString("Drop-down \"LED Placement\": [%1] %2").arg(
                                ledPlacementDrpDn->currentIndex()).arg(
                                ledPlacementDrpDn->currentText())
                    );
            } );
}

void MainWindow::createInteractives() {
    /*btn = new QPushButton;
    btn->setFixedSize(600, 600);
    btn->setStyleSheet("background-color: purple");
    connect(btn, &QPushButton::clicked, this, &MainWindow::close);*/

    xRayCheckBox = new QCheckBox(QString("X-Ray"));
    xRayCheckBox->setCheckState(Qt::Unchecked);
    xRayCheckBox->setFixedSize(100, 25);
    connect(xRayCheckBox, &QCheckBox::checkStateChanged,
            [=](Qt::CheckState checked) {
                display->toggleXRay();
                display->updateScene();
            } );

    logsCheckBox = new QCheckBox(QString("Show logs"));
    logsCheckBox->setCheckState(Qt::Unchecked);
    logsCheckBox->setFixedSize(100, 25);
    connect(logsCheckBox, &QCheckBox::checkStateChanged,
            [=](Qt::CheckState checked) {
                logsClearBtn->setEnabled(checked);
                logsTxtBox->setEnabled(checked);
                logsTxtBox->setVisible(checked);
            } );

    logsClearBtn = new QPushButton("Clear");
    logsClearBtn->setFixedSize(100, logsCheckBox->size().height());
    logsClearBtn->setEnabled(false);    /* Enable button, only
                                         * when checkbox checked */
    connect( logsClearBtn, &QPushButton::clicked,
            [=](bool checked){ logsTxtBox->clear(); } );

    logsTxtBox = new QTextEdit;
    logsTxtBox->setEnabled(false);
    logsTxtBox->setVisible(false);
    logsTxtBox->setReadOnly(true);  // RO as it is used for logs
    logsTxtBox->setFont({ "Source Code Pro" });
    logsTxtBox->setFixedWidth(ipLbl->size().width());
    /* ISSUE: When defining height, it'll create an empty space on Top
     *        of the drawing area (btn here) and I don't know WHY?! */
    /*logsTxtBox->setFixedSize(ipLbl->size().width(),
                             btn->size().height() - (
                                ipLbl->size().height() +
                                portLbl->size().height() +
                                scktLbl->size().height() +
                                scktMovieLbl->size().height() +
                                logsClearBtn->size().height()));*/
    logsTxtBox->clear();

    zoomSlider = new QSlider(Qt::Orientation::Horizontal);
    zoomSlider->setTickInterval(1);
    /* Set highest jump when clicking on slider and not sliding the handle */
    zoomSlider->setPageStep(zoomSlider->tickInterval());
    zoomSlider->setRange(1, 40);
    zoomSlider->setValue((zoomSlider->maximum() - zoomSlider->minimum()) / 2 + 1);
    zoomSlider->setStyleSheet("QSlider::groove:horizontal {"    /* Back bar */
                                "background: #DCDCDC;"
                                "border: 1px solid #999999;"
                                "height: 2.5px;"
                                "border-radius: 2px;"
                              "}"
                              "QSlider::handle:horizontal {"    /* Handle */
                                "background: white;"
                                "border: 1px solid #999999;"
                                "width:  12px;"
                                "margin: -5px 0;"   /* <=> groove's height */
                                "border-radius: 3px;"
                              "}");
    zoomSlider->setFixedSize(display->size().width()/2, 50);
    connect(zoomSlider, &QSlider::valueChanged, this, [=](int value) {
        static int oldValue = (zoomSlider->maximum() - zoomSlider->minimum()) / 2 + 1;
        static double newScale = 0.0;
        newScale = 1 + (zoomSlider->value() - oldValue) / 10.0;

        display->scale(newScale, newScale);
        display->show();

        if (logsTxtBox->isEnabled())
            logsTxtBox->append(
                QString("Slider: %1 | newScale = %2").arg(zoomSlider->value()).arg(newScale));

        oldValue = zoomSlider->value();
    });

    rightJustifSpacers[0] = new QSpacerItem(50, 0, QSizePolicy::Expanding,
                                            QSizePolicy::Minimum);
    rightJustifSpacers[1] = new QSpacerItem(50, 0, QSizePolicy::Expanding,
                                            QSizePolicy::Minimum);
    rightJustifSpacers[2] = new QSpacerItem(50, 0, QSizePolicy::Expanding,
                                            QSizePolicy::Minimum);
    logsAlignSpacer       = new QSpacerItem(ipLbl->size().width()        -
                                            logsCheckBox->size().width() -
                                            logsClearBtn->size().width() - 6, 0,
                                            QSizePolicy::Minimum,
                                            QSizePolicy::Minimum);
    socketMovieSpacer     = new QSpacerItem(0, scktMovieLbl->size().height(),
                                            QSizePolicy::Minimum,
                                            QSizePolicy::Minimum);
}

void MainWindow::createLayouts() {
    /* *** Create objects that will fill menus *** */
    createLabels();
    createDropDownMenus();
    createInteractives();

    /* *** Objects creation *** */
    /** Layouts ****** */
    toolsHLayout    = new QHBoxLayout;
    scktLogsVLayout = new QVBoxLayout;
    logsHLayout     = new QHBoxLayout;
    ledHLayout      = new QHBoxLayout;
    zoomHLayout     = new QHBoxLayout;
    mainVLayout     = new QVBoxLayout;

    /* *** Layouts filling *** */
    /** Tools layouts (Drop-down menus to configure LED to put) ******* */
    toolsHLayout->addWidget(ledTypeDrpDn);
    toolsHLayout->addWidget(ledPkgDrpDn);
    toolsHLayout->addWidget(ledPkgGapLineEdit);
    toolsHLayout->addWidget(ledPkgUnitDrpDn);
    toolsHLayout->addWidget(ledPlacementDrpDn);
    toolsHLayout->addItem(rightJustifSpacers[0]);

    /** Socket infos + Logs ****** */
    scktLogsVLayout->addWidget(ipLbl);
    scktLogsVLayout->addWidget(portLbl);
    scktLogsVLayout->addWidget(scktLbl);
    scktLogsVLayout->addWidget(scktMovieLbl, 0, Qt::AlignCenter);
    scktLogsVLayout->addSpacerItem(socketMovieSpacer);
    logsHLayout->addWidget(logsCheckBox);
    logsHLayout->addItem(logsAlignSpacer);
    logsHLayout->addWidget(logsClearBtn);
    scktLogsVLayout->addLayout(logsHLayout);
    scktLogsVLayout->addWidget(logsTxtBox);
    scktLogsVLayout->addStretch();

    /** Display/Creation area + Socket & Logs ****** */
    //ledHLayout->addWidget(btn/* Drawing area */);
    ledHLayout->addWidget(display);
    ledHLayout->addLayout(scktLogsVLayout);
    ledHLayout->addItem(rightJustifSpacers[1]);

    /** Zoom layout with Slider & Clickable Labels */
    zoomHLayout->addWidget(zoomMinusLbl);
    zoomHLayout->addWidget(zoomSlider);
    zoomHLayout->addWidget(zoomPlusLbl);
    zoomHLayout->addWidget(xRayCheckBox);
    zoomHLayout->addItem(rightJustifSpacers[2]);

    /** Main Layout ****** */
    mainVLayout->addLayout(toolsHLayout);
    mainVLayout->addLayout(ledHLayout);
    mainVLayout->addLayout(zoomHLayout);
    mainVLayout->addStretch();
}

void createQMovie(QString filename, QMovie **movie, int &lblW, int &lblH) {
    int movieW, movieH;

    *movie = new QMovie(filename);
    (*movie)->setCacheMode(QMovie::CacheAll);

    /* Jump to 1st frame to retrieve dimensions */
    (*movie)->jumpToFrame(0);
    movieW = (*movie)->currentImage().size().width();
    movieH = (*movie)->currentImage().size().height();

    if (movieW <= movieH)
        (*movie)->setScaledSize({ movieW * lblH / movieH, lblH });
    else
        (*movie)->setScaledSize({ lblW, lblW * movieH / movieW });
}

void MainWindow::createQMovies(void) {
    int lblW = scktMovieLbl->size().width(),
        lblH = scktMovieLbl->size().height();

    createQMovie("../../assets/loading_wo_bg.gif",    &scktMovDisconn,
                 lblW, lblH);
    createQMovie("../../assets/someKindOfPortal.gif", &scktMovConnect,
                 lblW, lblH);
    createQMovie("../../assets/waves.gif", &scktMovWaiting,
                 lblW, lblH);
}

void MainWindow::replaceSocketMovieWith(QMovie *movie) {
    if (scktMovDisconn->state() == QMovie::Running)
        scktMovDisconn->stop();
    else if (scktMovConnect->state() == QMovie::Running)
        scktMovConnect->stop();
    else /* (socketStatusMovieWait->state() == QMovie::Running) */
        scktMovWaiting->stop();

    scktMovieLbl->setMovie(movie);
    movie->start();
}

/** **************************************************************************
 * @brief Client's request reader
 *************************************************************************** */
void MainWindow::readCltRequest(void) {
    inStream.startTransaction();

    static QByteArray streamAsBytes;
    inStream >> streamAsBytes;

    if ( ! inStream.commitTransaction() )   return;

    if (logsTxtBox->isEnabled()) {
        logsTxtBox->append(QString("Input           : %1").arg(streamAsBytes));
    }

    if ( cltConnection->bytesAvailable() ) {
        return;
    }

    /** Detect client's leave
     *  + toLower() Hypothesis:
     *  Upper -> Lower is optimized because UpperCase + offset = LowerCase
     *  instead of a substraction LowerCase - offset = UpperCase */
    if (streamAsBytes.toLower() == QByteArray("leaving")) {
        cltConnection = nullptr;
        return;
    }

    if (logsTxtBox->isEnabled()) {
        logsTxtBox->append(QString("bytesAvailable(): %1").arg(cltConnection->bytesAvailable()));
        logsTxtBox->append(QString("Input           : %1").arg(streamAsBytes));
    }

    /* REGEX part: ^\!
     *             C[3-4]
     *             N(([A-F]|[a-f]|[0-9]){4}),
     *             .{4})+
     *             \$$
     * Accept data, like:  1data  - !C3N0001,<uint32_val>$
                          10datas - !C3N000A,<uint32_val1>...<uint32_valN>$ */
    static QRegularExpression re("^\\!C[3-4]N(([A-F]|[a-f]|[0-9]){4}),"
                                 "(.{4})+\\$$");
    if (re.match(streamAsBytes).hasMatch()) {
        /* Remove starting "!C" & terminating '$' sequences */
        streamAsBytes.remove(0, 2);
        streamAsBytes.removeLast();

        uint16_t n;
        uint8_t  comp;
        uint8_t  r, g, b, w;

        /* Extract composants & # of data (16bits <=> 4 hexa digits) */
        bool okDbg = true;
        comp = TO_UINT8(streamAsBytes.at(0));   /* -'0': From char to uint8 */
        streamAsBytes.remove(0, 2);             /* Remove digit + 'N' */
        /* ISSUE: Below not working?!?!
         * Hypothesis: Not isolated, so doesn't work with suffix */
        //n = streamAsBytes.toUShort(&okDbg, NUMERICAL_BASE_16);
        n = streamAsBytes.first(HEXA_16BITS_NDIGITS)
                .toUShort(&okDbg, NUMERICAL_BASE_16);
        /* Remove digits + ',' before datas */
        streamAsBytes.remove(0, HEXA_16BITS_NDIGITS+1);

        /* Check MAX limit & overwrite value if needed */
        if (n > display->getNumberOfLeds())
            n = display->getNumberOfLeds();

        if (comp == 3) {
            for (uint16_t i = 0; i < n; i++) {
                r = streamAsBytes.at(0);
                g = streamAsBytes.at(1);
                b = streamAsBytes.at(2);
                display->setLedColor(i, QColor(r, g, b));

                streamAsBytes.remove(0, sizeof(uint32_t));
            }
        } else if (comp == 4) {
            /* Treat 4 components as one WHITE channel
             * and, for now, set all channels to this value */
            for (uint32_t i = 0; i < n; i++) {
                w = streamAsBytes.at(3);
                display->setLedColor(i, QColor(w, w, w));

                streamAsBytes.remove(0, sizeof(uint32_t));
            }
        } else  return; /* Do nothing and end function */

        display->updateScene();
        //display->update();
    } else {
        if (logsTxtBox->isEnabled()) {
            logsTxtBox->append(QString("readCltRequest: RegEx failed to pass"));
            logsTxtBox->append(QString("Input: %1").arg(streamAsBytes));
        }
    }
}

/** **************************************************************************
 * @brief Client's connection approval
 *************************************************************************** */
void MainWindow::connectionSucessToClient(void) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);

    /* /!\ Doesn't work properly using C string (out << "Hi";) /!\
     * Answer: It might be because a C-string is then cast as a QString and
     *         this stores each data as uint16_t instead of uint8_t */
    out << QByteArray("Connection successful");

    if ( ! cltConnection ) {
        cltConnection = tcpServer->nextPendingConnection();
        connect(cltConnection, &QAbstractSocket::disconnected,
                cltConnection, &QObject::deleteLater);

        inStream.setDevice(cltConnection);
        connect(cltConnection, &QIODevice::readyRead,
                this, &MainWindow::readCltRequest);
    }

    cltConnection->write(block);
    cltConnection->flush();
}
