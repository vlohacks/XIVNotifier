#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "notificationsettings.h"

#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , running(false)
{
    ui->setupUi(this);

    connect(&timer, &QTimer::timeout, this, &MainWindow::timer_timeout);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    timer.start(REFRESH_INTERVAL_IDLE);
}

MainWindow::~MainWindow()
{
    timer.stop();
    delete ui;
}

void MainWindow::timer_timeout()
{
    QString UrlSpec;
    QString Message;

    if (running) {
        queue_current = observer.getQueue();
        ui->lcd_ppl_ahead->display(queue_current);

        if (queue_current == 0) {
            timer.setInterval(REFRESH_INTERVAL_IDLE);
            running = false;
            ui->lcd_time_h->display(0);
            ui->lcd_time_m->display(0);
            Message = "You are logged in!";
        } else {
            Message = tr("There %1 %2 guy%3 ahead.").arg(queue_current == 1 ? "is" : "are",  QString::number(queue_current), queue_current == 1 ? "" : "s");
        }

        if (queue_current != queue_last) {
            QTime time_est;

            if (queue_init == -1) {
                queue_init = queue_current;
                time_last = QTime::currentTime();
            } else {
                int queue_delta = queue_last - queue_current;

                // calculate average of queue deltas
                est_avg_points.push_back(queue_delta);

                while (est_avg_points.size() > EST_AVG_POINTS_NO)
                    est_avg_points.erase(est_avg_points.begin());

                int queue_delta_avg = 0;
                for (auto it = est_avg_points.begin(); it != est_avg_points.end(); ++it)
                    queue_delta_avg += *it;

                queue_delta_avg /= est_avg_points.size();

                // calculate estaminated login time from average delta
                QTime now = QTime::currentTime();
                int secs_delta = time_last.secsTo(now);
                int secs_est = (int)(((double)secs_delta / (double)queue_delta_avg) * (double)queue_current);

                time_est = QTime(0, 0).addSecs(secs_est);

                ui->lblPleaseWait->setVisible(false);
                ui->lcd_time_h->display(time_est.hour());
                ui->lcd_time_m->display(time_est.minute());

                time_last = now;
            }

            queue_last = queue_current;

            if (time_est.isValid() && queue_current > 0)
                Message += tr("\nEstaminated login in %1 hour%2 and %3 minute%4").arg(QString::number(time_est.hour()), time_est.hour() == 1 ? "" : "s", QString::number(time_est.minute()), time_est.minute() == 1 ? "" : "s");

            if (settings.value("NotificationEnabled").toBool() && (queue_current < settings.value("NotificationThreshold").toInt())) {
                switch (settings.value("NotificationMethod").toInt()) {
                case 0: // Whatsapp
                    UrlSpec = tr("https://api.callmebot.com/whatsapp.php?phone=%1&apikey=%2&text=%3").arg(settings.value("PhoneNumber").toString(), settings.value("ApiKey").toString(), QUrl::toPercentEncoding(Message));
                    break;
                case 1: // Signal
                    UrlSpec = tr("https://api.callmebot.com/signal/send.php?phone=%1&apikey=%2&text=%3").arg(settings.value("PhoneNumber").toString(), settings.value("ApiKey").toString(), QUrl::toPercentEncoding(Message));
                    break;
                //case 2: // Telegram

                    //break;
                default:
                    return;
                }

                qnam.get(QNetworkRequest(QUrl::fromUserInput(UrlSpec)));
            }
       }
    } else {    // if (running)
        running = observer.hookGame();

        if (observer.hasPid()) {
            ui->lblGameInstanceCheck->setStyleSheet("image: url(:/icons/res/tick.png);");
            ui->lblGameInstanceInfo->setText( tr("%1 (%2)").arg(QString::fromWCharArray(observer.getExeName()), QString::number(observer.getPid())) );
        } else {
            ui->lblGameInstanceCheck->setStyleSheet("image: url(:/icons/res/cross.png);");
            ui->lblGameInstanceInfo->setText("...");
        }

        if (observer.hasPtr()) {
            ui->lblMemAddressCheck->setStyleSheet("image: url(:/icons/res/tick.png);");
            ui->lblMemAddressInfo->setText( tr("0x%1").arg(QString::number(observer.getPtrU64(), 16)) );
        } else {
            ui->lblMemAddressCheck->setStyleSheet("image: url(:/icons/res/cross.png);");
            ui->lblMemAddressInfo->setText("...");
        }

        if (running) {
            timer.setInterval(REFRESH_INTERVAL_RUN);
            time_init = QTime::currentTime();

            queue_current = observer.getQueue();
            queue_last = queue_current;
            queue_init = -1;

            ui->lcd_ppl_ahead->display(queue_current);
            ui->lcd_time_h->display(0);
            ui->lcd_time_m->display(0);
            ui->lblPleaseWait->setVisible(true);
            ui->lblGlobalState->setText("Running");
        } else {
            ui->lblGlobalState->setText("Waiting for login process...");
        }

    }
}

void MainWindow::on_btnNotificationSettings_clicked()
{
    timer.stop();
    NotificationSettings s(this);
    s.exec();
    timer.start();
}

void MainWindow::on_btnExit_clicked()
{
    this->close();
}

