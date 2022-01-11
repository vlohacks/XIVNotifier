#ifndef NOTIFICATIONSETTINGS_H
#define NOTIFICATIONSETTINGS_H

#include <QDialog>

namespace Ui {
class NotificationSettings;
}

class NotificationSettings : public QDialog
{
    Q_OBJECT

public:
    explicit NotificationSettings(QWidget *parent = nullptr);
    ~NotificationSettings();

private slots:
    void on_buttonBox_accepted();

    void on_btnTestNotification_clicked();

    void on_cmbNotificationMethod_currentIndexChanged(int index);

private:
    Ui::NotificationSettings *ui;
};

#endif // NOTIFICATIONSETTINGS_H
