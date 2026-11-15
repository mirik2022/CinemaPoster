#include "logindialog.h"
#include <QFile>
#include <QTextStream>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QFont>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("🎬 Вход в кинотеатр");
    setFixedSize(500, 400); // Увеличили высоту

    // Стильный дизайн
    setStyleSheet(
        "QDialog {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "    stop:0 #0a0a1f, stop:1 #15152b);"
        "    border: 2px solid #2a2a4a;"
        "    border-radius: 20px;"
        "}"
        "QLabel#titleLabel {"
        "    color: #ffffff;"
        "    font-size: 28px;"
        "    font-weight: bold;"
        "    margin-bottom: 30px;"
        "    qproperty-alignment: AlignCenter;"
        "}"
        "QLineEdit {"
        "    background: rgba(255, 255, 255, 0.1);"
        "    border: 2px solid rgba(255, 255, 255, 0.2);"
        "    border-radius: 10px;"
        "    padding: 15px 20px;"
        "    font-size: 16px;"  // Увеличили размер текста
        "    color: white;"
        "    margin: 5px 0;"
        //"    min-width: 300px;"  // Увеличили ширину
        "    min-height: 25px;"  // Увеличили высоту
        "}"
        "QLineEdit:focus {"
        "    border-color: #00adb5;"
        "    background: rgba(255, 255, 255, 0.15);"
        "}"
        "QLineEdit::placeholder {"
        "    color: rgba(255, 255, 255, 0.5);"
        "    font-size: 14px;"
        "}"
        "QPushButton {"
        "    min-width: 180px;"  // Уменьшили ширину кнопок
        "    min-height: 45px;"
        "    border-radius: 10px;"
        "    padding: 12px 20px;"  // Уменьшили padding
        "    font-size: 15px;"
        "    font-weight: bold;"
        "    margin: 5px;"  // Уменьшили отступы
        "}"
        "#loginBtn {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #00adb5, stop:1 #009ca3);"
        "    color: white;"
        "    border: none;"
        "}"
        "#loginBtn:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #009ca3, stop:1 #008b92);"
        "}"
        "#loginBtn:pressed {"
        "    background: #007b83;"
        "}"
        "#registerBtn {"
        "    background: rgba(57, 62, 70, 0.8);"
        "    color: white;"
        "    border: 2px solid rgba(255, 255, 255, 0.2);"
        "}"
        "#registerBtn:hover {"
        "    background: rgba(57, 62, 70, 0.9);"
        "    border-color: #00adb5;"
        "}"
        "#registerBtn:pressed {"
        "    background: rgba(57, 62, 70, 1.0);"
        "}"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 30, 40, 30); // Уменьшили отступы
    mainLayout->setSpacing(20);

    QLabel *titleLabel = new QLabel("🎬 CINEMA CITY");
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    // Используем QFormLayout для лучшего выравнивания
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight);
    formLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);

    usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("Введите имя пользователя");
    usernameEdit->setMinimumWidth(320); // Увеличили ширину
    usernameEdit->setMinimumHeight(40); // Увеличили высоту
    formLayout->addRow("👤 Имя:", usernameEdit);

    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("Введите пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumWidth(320);
    passwordEdit->setMinimumHeight(40);
    formLayout->addRow("🔑 Пароль:", passwordEdit);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch(1);

    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    loginButton = new QPushButton("Войти");
    loginButton->setObjectName("loginBtn");
    registerButton = new QPushButton("Регистрация");
    registerButton->setObjectName("registerBtn");

    buttonLayout->addStretch();
    buttonLayout->addWidget(loginButton);
    buttonLayout->addSpacing(10); // Добавили промежуток между кнопками
    buttonLayout->addWidget(registerButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(1);

    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &LoginDialog::onRegisterClicked);

    MovieDatabase::instance().loadUsers();
}

void LoginDialog::onLoginClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("⚠️ Ошибка входа");
        msgBox.setText("<div style='color: white; font-size: 14px;'>"
                       "❌ <b>Заполните все поля</b><br>"
                       "Введите имя пользователя и пароль"
                       "</div>");
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setIcon(QMessageBox::Warning);
        // В методе onLoginClicked() замените стиль для QMessageBox:
        msgBox.setStyleSheet(
            "QMessageBox {"
            "    background: #1a1a2e;"
            "    border: 2px solid #ff4d8d;"
            "    border-radius: 15px;"
            "    padding: 10px;"
            "}"
            "QMessageBox QLabel {"
            "    color: white;"
            "    font-size: 14px;"
            "}"
            "QMessageBox QPushButton {"
            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "    stop:0 #ff4d8d, stop:1 #8b5cf6);"
            "    color: white;"
            "    border: none;"
            "    border-radius: 8px;"
            "    padding: 10px 25px;"
            "    font-size: 13px;"
            "    font-weight: bold;"
            "}"
            "QMessageBox QPushButton:hover {"
            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "    stop:0 #ff2d7a, stop:1 #7c3aed);"
            "}"
            );
        msgBox.exec();
        return;
    }

    if (MovieDatabase::instance().validateUser(username, password)) {
        User* user = MovieDatabase::instance().findUser(username);
        if (user) {
            currentUser = *user;
            loggedIn = true;
            loggedUsername = username; // Сохраняем имя пользователя

            MovieDatabase::instance().loadBookedTickets(username);

            QMessageBox msgBox;
            msgBox.setWindowTitle("✅ Успешный вход");
            msgBox.setText(QString("<div style='color: white; font-size: 14px; text-align: center;'>"
                                   "✨ <b style='color: #8b5cf6; font-size: 16px;'>Добро пожаловать, %1!</b><br><br>"
                                   "🎬 Рады видеть вас в Cinema Vision!"
                                   "</div>").arg(username));
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStyleSheet(
                "QMessageBox {"
                "    background: #1a1a2e;"
                "    border: 2px solid #2a2a4a;"
                "    border-radius: 15px;"
                "}"
                "QMessageBox QLabel {"
                "    color: white;"
                "    font-size: 14px;"
                "}"
                "QMessageBox QPushButton {"
                "    background: #00adb5;"
                "    color: white;"
                "    border: none;"
                "    border-radius: 8px;"
                "    padding: 8px 20px;"
                "    font-size: 13px;"
                "    font-weight: bold;"
                "}"
                "QMessageBox QPushButton:hover {"
                "    background: #009ca3;"
                "}"
                );
            msgBox.exec();
            accept();
        }
    } else {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка");
        msgBox.setText("Неверное имя пользователя или пароль");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet(
            "QMessageBox {"
            "    background: #1a1a2e;"
            "    border: 2px solid #2a2a4a;"
            "    border-radius: 15px;"
            "}"
            "QMessageBox QLabel {"
            "    color: white;"
            "    font-size: 14px;"
            "}"
            "QMessageBox QPushButton {"
            "    background: #00adb5;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 8px;"
            "    padding: 8px 20px;"
            "    font-size: 13px;"
            "    font-weight: bold;"
            "}"
            "QMessageBox QPushButton:hover {"
            "    background: #009ca3;"
            "}"
            );
        msgBox.exec();
    }
}


void LoginDialog::onRegisterClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка");
        msgBox.setText("Заполните все поля");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet(
            "QMessageBox {"
            "    background: #1a1a2e;"
            "    border: 2px solid #2a2a4a;"
            "    border-radius: 15px;"
            "}"
            "QMessageBox QLabel {"
            "    color: white;"
            "    font-size: 14px;"
            "}"
            "QMessageBox QPushButton {"
            "    background: #00adb5;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 8px;"
            "    padding: 8px 20px;"
            "    font-size: 13px;"
            "    font-weight: bold;"
            "}"
            "QMessageBox QPushButton:hover {"
            "    background: #009ca3;"
            "}"
            );
        msgBox.exec();
        return;
    }

    if (username.length() < 3) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка");
        msgBox.setText("Имя пользователя должно быть не менее 3 символов");
        msgBox.setIcon(QMessageBox::Warning);
        // После строки 327 (ошибка входа при пустых полях):
        msgBox.setStyleSheet(
            "QMessageBox {"
            "    background: #1a1a2e;"
            "    border: 2px solid #2a2a4a;"
            "    border-radius: 15px;"
            "}"
            "QMessageBox QLabel {"
            "    color: white;"
            "    font-size: 14px;"
            "}"
            "QMessageBox QPushButton {"
            "    background: #00adb5;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 8px;"
            "    padding: 8px 20px;"
            "    font-size: 13px;"
            "    font-weight: bold;"
            "}"
            "QMessageBox QPushButton:hover {"
            "    background: #009ca3;"
            "}"
            );
        msgBox.exec();
        return;
    }

    for (QChar ch : username) {
        if (!ch.isLetterOrNumber() && ch != '_') {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Ошибка");
            msgBox.setText("Имя пользователя может содержать только буквы, цифры и подчеркивание");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStyleSheet(
                "QMessageBox {"
                "    background: #1a1a2e;"
                "    border: 2px solid #2a2a4a;"
                "    border-radius: 15px;"
                "}"
                "QMessageBox QLabel {"
                "    color: white;"
                "    font-size: 14px;"
                "}"
                "QMessageBox QPushButton {"
                "    background: #00adb5;"
                "    color: white;"
                "    border: none;"
                "    border-radius: 8px;"
                "    padding: 8px 20px;"
                "    font-size: 13px;"
                "    font-weight: bold;"
                "}"
                "QMessageBox QPushButton:hover {"
                "    background: #009ca3;"
                "}"
                );
            msgBox.exec();
            return;
        }
    }

    if (password.length() < 6) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка");
        msgBox.setText("Пароль должен быть не менее 6 символов");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet(
            "QMessageBox {"
            "    background: #1a1a2e;"
            "    border: 2px solid #2a2a4a;"
            "    border-radius: 15px;"
            "}"
            "QMessageBox QLabel {"
            "    color: white;"
            "    font-size: 14px;"
            "}"
            "QMessageBox QPushButton {"
            "    background: #00adb5;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 8px;"
            "    padding: 8px 20px;"
            "    font-size: 13px;"
            "    font-weight: bold;"
            "}"
            "QMessageBox QPushButton:hover {"
            "    background: #009ca3;"
            "}"
            );
        msgBox.exec();
        return;
    }

    if (MovieDatabase::instance().findUser(username)) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка");
        msgBox.setText("Это имя пользователя уже занято");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet(
            "QMessageBox {"
            "    background: #1a1a2e;"
            "    border: 2px solid #2a2a4a;"
            "    border-radius: 15px;"
            "}"
            "QMessageBox QLabel {"
            "    color: white;"
            "    font-size: 14px;"
            "}"
            "QMessageBox QPushButton {"
            "    background: #00adb5;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 8px;"
            "    padding: 8px 20px;"
            "    font-size: 13px;"
            "    font-weight: bold;"
            "}"
            "QMessageBox QPushButton:hover {"
            "    background: #009ca3;"
            "}"
            );
        msgBox.exec();
        return;
    }

    // Создаем пользователя с начальным балансом 10 BYN
    User newUser(username, password, username + "@cinema.com", 10.0);

    MovieDatabase::instance().addUser(newUser);
    currentUser = newUser;
    loggedIn = true;
    loggedUsername = username; // Сохраняем имя пользователя

    MovieDatabase::instance().saveBookedTickets(username);

    QMessageBox msgBox;
    msgBox.setWindowTitle("Успех");
    msgBox.setText(QString("Регистрация выполнена!\n"
                           "На ваш счёт начислено 10 BYN\n"
                           "Добро пожаловать, %1!").arg(username));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet(
        "QMessageBox {"
        "    background: #1a1a2e;"
        "    border: 2px solid #2a2a4a;"
        "    border-radius: 15px;"
        "}"
        "QMessageBox QLabel {"
        "    color: white;"
        "    font-size: 14px;"
        "}"
        "QMessageBox QPushButton {"
        "    background: #00adb5;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 8px 20px;"
        "    font-size: 13px;"
        "    font-weight: bold;"
        "}"
        "QMessageBox QPushButton:hover {"
        "    background: #009ca3;"
        "}"
        );
    msgBox.exec();

    // Автоматически входим после регистрации
    accept();
}
