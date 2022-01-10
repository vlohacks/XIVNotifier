#include "mainwindow.h"
#include "ui_mainwindow.h"
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

    // set up notfication options
    ui->cmbNotificationMethod->addItem("WhatsApp", 0);
    ui->cmbNotificationMethod->addItem("Signal", 1);
    //ui->cmbNotificationMethod->addItem("Telegram", 2);
    QSettings settings("Cyndi", "FFXIVNotifier");

    // load notification settings
    int data = settings.value("NotificationMethod").toInt();
    int index = ui->cmbNotificationMethod->findData(data);
    if (index != -1)
        ui->cmbNotificationMethod->setCurrentIndex(index);

    ui->txtApiKey->setText(settings.value("ApiKey").toString());
    ui->txtPhoneNumber->setText(settings.value("PhoneNumber").toString());
    ui->txtNotificationThreshold->setText(settings.value("NotificationThreshold", "250").toString());
    ui->chkNotificationEnabled->setChecked(settings.value("NotificationEnabled", false).toBool());

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    if (running) {
        timer.stop();
        observer.reset();
        ui->pushButton->setText("Start");
        running = false;
    } else {
        // save settings
        QSettings settings("Cyndi", "FFXIVNotifier");
        settings.setValue("NotificationEnabled", ui->chkNotificationEnabled->isChecked());
        settings.setValue("NotificationThreshold", ui->txtNotificationThreshold->text());
        settings.setValue("PhoneNumber", ui->txtPhoneNumber->text());
        settings.setValue("ApiKey", ui->txtApiKey->text());
        settings.setValue("NotificationMethod", ui->cmbNotificationMethod->currentData());

        // hook game process
        ui->pushButton->setEnabled(false);
        running = observer.hookGame();
        ui->pushButton->setEnabled(true);

        if (running) {
            time_init = QTime::currentTime();

            queue_current = observer.getQueue();
            queue_last = queue_current;
            queue_init = -1;

            ui->lcd_ppl_ahead->display(queue_current);
            ui->lcd_time_h->display(0);
            ui->lcd_time_m->display(0);

            timer.start(REFRESH_INTERVAL);
            ui->pushButton->setText("Stop");
        } else {
            QMessageBox::information(this, "XIVNotifier", "Could not find valid login. Make sure the game is running and you are in the login queue", QMessageBox::Ok);
        }
    }
    ui->grpNotificationOptions->setEnabled(!running);
}


void MainWindow::timer_timeout()
{
    QString UrlSpec;
    QString Message;

    queue_current = observer.getQueue();
    ui->lcd_ppl_ahead->display(queue_current);

    if (queue_current == 0) {
        timer.stop();
        running = false;
        ui->pushButton->setText("Start");
        ui->lcd_time_h->display(0);
        ui->lcd_time_m->display(0);
        Message = "You are logged in!";
    } else {
        Message = tr("There %1 %2 guy%3 ahead.").arg(queue_current == 1 ? "is" : "are",  QString::number(queue_current), queue_current == 1 ? "" : "s");
    }

    if (queue_current != queue_last) {
        QTime est;

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

            est = QTime(0, 0).addSecs(secs_est);

            ui->lcd_time_h->display(est.hour());
            ui->lcd_time_m->display(est.minute());

            time_last = now;
        }

        queue_last = queue_current;

        if (est.isValid())
            Message += tr("\nEstaminated login in %1 hour%2 and %3 minute%4").arg(QString::number(est.hour()), est.hour() == 1 ? "" : "s", QString::number(est.minute()), est.minute() == 1 ? "" : "s");

        qDebug() << Message;

        if (ui->chkNotificationEnabled->isChecked() && (queue_current < ui->txtNotificationThreshold->text().toInt())) {
            switch (ui->cmbNotificationMethod->currentData().toInt()) {
            case 0: // Whatsapp
                UrlSpec = tr("https://api.callmebot.com/whatsapp.php?phone=%1&apikey=%2&text=%3").arg(ui->txtPhoneNumber->text(), ui->txtApiKey->text(), QUrl::toPercentEncoding(Message));
                break;
            case 1: // Signal
                UrlSpec = tr("https://api.callmebot.com/signal/send.php?phone=%1&apikey=%2&text=%3").arg(ui->txtPhoneNumber->text(), ui->txtApiKey->text(), QUrl::toPercentEncoding(Message));
                break;
            //case 2: // Telegram

                //break;
            default:
                return;
            }

            qDebug() << UrlSpec;

            qnam.get(QNetworkRequest(QUrl::fromUserInput(UrlSpec)));
        }
   }
}
