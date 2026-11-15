#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include "moviedatabase.h"
#include "user.h"

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    User getCurrentUser() const { return currentUser; }
    bool isLoggedIn() const { return loggedIn; }
    QString getLoggedUsername() const { return loggedUsername; } // Добавляем метод

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    User currentUser;
    bool loggedIn = false;
    QString loggedUsername; // Добавляем поле для имени пользователя

    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *registerButton;
    bool isPasswordAlreadyUsed(const QString& password);
};

#endif // LOGINDIALOG_H
