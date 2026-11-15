#ifndef USER_H
#define USER_H

#include <QString>
#include <QList>
#include <QDateTime>

class User
{
public:
    User();
    //User(QString username, QString password, QString email);
    User(QString username, QString password, QString email, double initialBalance);

    QString getUsername() const { return username; }
    QString getPassword() const { return password; }
    QString getEmail() const { return email; }
    QString getRechargeCode() const { return rechargeCode; }
    QList<int> getBookedTickets() const { return bookedTickets; }
    double getBalance() const { return balance; }

    void addBookedTicket(int sessionId);
    void removeBookedTicket(int sessionId);
    bool hasBookedTicket(int sessionId) const;
    void setBookedTickets(const QList<int>& newTickets);
    void addBalance(double amount);
    bool deductBalance(double amount);

    // Новый метод для генерации кода пополнения
    void generateRechargeCode();
    bool validateRechargeCode(const QString& code) const;

private:
    QString username;
    QString password;
    QString email;
    QString rechargeCode; // Уникальный код для пополнения баланса
    QList<int> bookedTickets;
    double balance = 0.0;
};

#endif // USER_H
