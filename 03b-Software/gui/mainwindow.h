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
    void updateSocketMovie(QString filename);
    void updateSocketMovie(const char *filename);

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
    QHBoxLayout *zoomHLayout       = nullptr;

    /* Interactives */
    QCheckBox   *logsCheckBox = nullptr;
    QPushButton *logsClearBtn = nullptr;

    QComboBox *ledTypeDropDown      = nullptr;
    QComboBox *ledPkgDropDown       = nullptr;
    QLineEdit *ledPkgGapLineEdit    = nullptr;
    QComboBox *ledPkgUnitDropDown   = nullptr;
    QComboBox *ledPlacementDropDown = nullptr;

    QSlider *zoomSlider;

    QPushButton *btn;

    /* Labels, Datas & Stuffs */
    QLabel *ipLabel = nullptr;
    QString ipStr   = "127.0.0.1";

    QLabel *portLabel = nullptr;
    int     port      = 5000;

    QLabel *socketLabel      = nullptr;
    bool    socketStatus     = false;
    QLabel *socketMovieLabel = nullptr;
    QMovie *socketMovie      = nullptr;

    QTextEdit *logsTxtBox        = nullptr;
    /* Clickable Label implemented as QPushButton for the clicked event */
    QPushButton *zoomPlusLabel   = nullptr;
    QPushButton *zoomMinusLabel  = nullptr;

    /* Spacers */
    QSpacerItem *rightJustifSpacers[3];
    QSpacerItem *logsAlignmentSpacer;
    QSpacerItem *socketMovieSpacer;
};
#endif // MAINWINDOW_H
