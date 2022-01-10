#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSettings>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QTime>
#include "xivloginobserver.h"

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
    void on_pushButton_clicked();
    void timer_timeout();


private:
    int queue_init;
    int queue_last;
    int queue_current;

    const int REFRESH_INTERVAL = 30000;

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

};
#endif // MAINWINDOW_H
