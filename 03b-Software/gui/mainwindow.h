#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>  /* Also include {QHBox,QGrid}Layout */

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    /* ... */

private slots:
    /* File actions */
    void saveDesign();
    void loadDesign();
    /* Design actions */
    void undoAction();
    void emptyDesign();
    void infoLedsCount();
    void infoSizeIrl();
    /* TCP Socket actions */
    void startServer();
    void stopServer();
    void cfgSocketInfos();

private:
    void createActions();
    void createMenus();
    void createLabels();
    void createInteractives();
    void createLayouts();

    /* Menus */
    QMenu *fileMenu = nullptr;
    QMenu *designMenu = nullptr;
    QMenu *infosSubMenu = nullptr;
    QMenu *tcpSocketMenu = nullptr;

    /* Actions */
    /** File actions */
    QAction *saveAct = nullptr;
    QAction *loadAct = nullptr;
    QAction *exitAct = nullptr;
    /** Design actions */
    QAction *undoAct         = nullptr;
    QAction *emptyDesignAct  = nullptr;
    QAction *infoLedCountAct = nullptr;
    QAction *infoSizeIrlAct  = nullptr;
    /** TCP Socket actions */
    QAction *startSvrAct  = nullptr;
    QAction *stopSvrAct   = nullptr;
    QAction *cfgSocketAct = nullptr;

    /* Layouts */
    QVBoxLayout *mainVLayout       = nullptr;
    QHBoxLayout *toolsHLayout      = nullptr;
    QHBoxLayout *ledHLayout        = nullptr;
    QVBoxLayout *socketLogsVLayout = nullptr;
    QHBoxLayout *logsHLayout       = nullptr;
    //QHBoxLayout *zoomHLayout     = nullptr; /* TODO: might be useless */

    QPushButton *btns[7];

    /* Labels, Datas & Stuffs */
    QLabel *ipLabel = nullptr;
    QString ipStr   = "127.0.0.1";

    QLabel *portLabel = nullptr;
    int     port      = 5000;

    QLabel *socketLabel  = nullptr;
    bool    socketStatus = false;

    QCheckBox *logsCheckBox   = nullptr;
    QPushButton *logsClearBtn = nullptr;

    /* Spacers */
    QSpacerItem *rowSpacers[3];
    QSpacerItem *colSpacers[3];
};
#endif // MAINWINDOW_H
