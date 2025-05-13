#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

#include "dynamicdisplay.h"

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
    void createDropDownMenus();
    void createInteractives();
    void createLayouts();
    void createQMovies(void);
    void replaceSocketMovieWith(QMovie *movie);

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
    QVBoxLayout *mainVLayout     = nullptr;
    QHBoxLayout *toolsHLayout    = nullptr;
    QHBoxLayout *ledHLayout      = nullptr;
    QVBoxLayout *scktLogsVLayout = nullptr;
    QHBoxLayout *logsHLayout     = nullptr;
    QHBoxLayout *zoomHLayout     = nullptr;

    /* Interactives */
    QCheckBox   *logsCheckBox = nullptr;
    QCheckBox   *xRayCheckBox = nullptr;
    QPushButton *logsClearBtn = nullptr;

    QComboBox *ledTypeDrpDn      = nullptr;
    QComboBox *ledPkgDrpDn       = nullptr;
    QLineEdit *ledPkgGapLineEdit = nullptr;
    QComboBox *ledPkgUnitDrpDn   = nullptr;
    QComboBox *ledPlacementDrpDn = nullptr;

    /* Labels, Datas & Stuffs */
    QLabel *ipLbl = nullptr;
    QString ipStr = "127.0.0.1";

    QLabel *portLbl = nullptr;
    int     port    = 5000;

    QLabel *scktLbl        = nullptr;
    bool    scktStatus     = false;
    QLabel *scktMovieLbl   = nullptr;
    QMovie *scktMovConnect = nullptr;
    QMovie *scktMovDisconn = nullptr;
    QMovie *scktMovWaiting = nullptr;

    QTextEdit *logsTxtBox  = nullptr;

    QSlider *zoomSlider       = nullptr;;
    /* Clickable Label implemented as QPushButton for the clicked event */
    QPushButton *zoomPlusLbl  = nullptr;
    QPushButton *zoomMinusLbl = nullptr;

    /* Spacers */
    QSpacerItem *rightJustifSpacers[3];
    QSpacerItem *logsAlignSpacer;
    QSpacerItem *socketMovieSpacer;

    /* Button still present for drawing area position */
    //QPushButton *btn;
    DynamicDisplay *display;
};
#endif // MAINWINDOW_H
