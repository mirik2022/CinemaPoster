#include "user.h"
#include <QDateTime>
#include <QCryptographicHash>

User::User() {}

User::User(QString username, QString password, QString email, double initialBalance)
    : username(username), password(password), email(email), balance(initialBalance)
{
    generateRechargeCode();
}

void User::addBookedTicket(int sessionId)
{
    if (!bookedTickets.contains(sessionId)) {
        bookedTickets.append(sessionId);
    }
}

void User::removeBookedTicket(int sessionId)
{
    bookedTickets.removeAll(sessionId);
}

bool User::hasBookedTicket(int sessionId) const
{
    return bookedTickets.contains(sessionId);
}

void User::setBookedTickets(const QList<int>& newTickets)
{
    bookedTickets = newTickets;
}

void User::addBalance(double amount)
{
    if (amount > 0) {
        balance += amount;
    }
}

bool User::deductBalance(double amount)
{
    if (balance >= amount && amount > 0) {
        balance -= amount;
        return true;
    }
    return false;
}

// Генерация уникального кода для пополнения баланса
void User::generateRechargeCode()
{
    // Генерируем код на основе имени пользователя и текущего времени
    QString data = username + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    QByteArray hash = QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Md5);
    rechargeCode = hash.toHex().left(8).toUpper(); // 8 символов в верхнем регистре
}

// Проверка кода пополнения
bool User::validateRechargeCode(const QString& code) const
{
    return (rechargeCode == code.toUpper());
}
