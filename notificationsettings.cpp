#include "notificationsettings.h"
#include "ui_notificationsettings.h"

#include <QDebug>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QUrl>

NotificationSettings::NotificationSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NotificationSettings)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    ui->lblCallMeBotUrl->setTextFormat(Qt::RichText);
    ui->lblCallMeBotUrl->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->lblCallMeBotUrl->setOpenExternalLinks(true);

    ui->txtApiKey->setValidator(new QIntValidator(0, 999999, this));
    ui->txtNotificationThreshold->setValidator(new QIntValidator(0, 99999, this));
    ui->txtPhoneNumber->setValidator(new QRegularExpressionValidator(QRegularExpression("^\\+[0-9]*")));

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

NotificationSettings::~NotificationSettings()
{
    delete ui;
}

void NotificationSettings::on_buttonBox_accepted()
{
    // save settings on quit
    QSettings settings("Cyndi", "FFXIVNotifier");
    settings.setValue("NotificationEnabled", ui->chkNotificationEnabled->isChecked());
    settings.setValue("NotificationThreshold", ui->txtNotificationThreshold->text());
    settings.setValue("PhoneNumber", ui->txtPhoneNumber->text());
    settings.setValue("ApiKey", ui->txtApiKey->text());
    settings.setValue("NotificationMethod", ui->cmbNotificationMethod->currentData());
}

void NotificationSettings::on_btnTestNotification_clicked()
{
    QNetworkAccessManager qnam;
    QUrl url;

    QString Message = "Hello from XIVNotifier";
    QString UrlSpec;

    switch (ui->cmbNotificationMethod->currentData().toInt()) {
    case 0: // Whatsapp
        UrlSpec = tr("https://api.callmebot.com/whatsapp.php?phone=%1&apikey=%2&text=%3").arg(ui->txtPhoneNumber->text(), ui->txtApiKey->text(), QUrl::toPercentEncoding(Message));
        break;
    case 1: // Signal
        UrlSpec = tr("https://api.callmebot.com/signal/send.php?phone=%1&apikey=%2&text=%3").arg(ui->txtPhoneNumber->text(), ui->txtApiKey->text(), QUrl::toPercentEncoding(Message));
        break;
    default:
        return;
    }

    qnam.get(QNetworkRequest(QUrl::fromUserInput(UrlSpec)));

    QMessageBox::information(this, "Test Message", "The notification works if you received the following message:\n\n\"" + Message + "\"");
}


void NotificationSettings::on_cmbNotificationMethod_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->lblCallMeBotUrl->setText("<a href=\"https://www.callmebot.com/blog/free-api-whatsapp-messages/\">How to get an API key for WhatsApp...</a>");
        break;
    case 1:
        ui->lblCallMeBotUrl->setText("<a href=\"https://www.callmebot.com/blog/free-api-signal-send-messages/\">How to get an API key for Signal...</a>");
        break;
    }
}

