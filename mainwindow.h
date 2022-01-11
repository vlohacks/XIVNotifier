#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QTime>
#include <QSettings>
#include "xivloginobserver.h"
#include "notificationsettings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void timer_timeout();
    void on_btnNotificationSettings_clicked();

    void on_btnExit_clicked();

private:
    int queue_init;
    int queue_last;
    int queue_current;

    const int REFRESH_INTERVAL_IDLE = 10000;
    const int REFRESH_INTERVAL_RUN = 5000;

    const size_t EST_AVG_POINTS_NO = 3;
    std::vector<int> est_avg_points;

    QTime time_init;
    QTime time_last;

    Ui::MainWindow *ui;
    bool running;
    XIVLoginObserver observer;
    QTimer timer;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;

    QSettings settings;
};
#endif // MAINWINDOW_H
