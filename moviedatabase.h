#ifndef MOVIEDATABASE_H
#define MOVIEDATABASE_H

#include <QString>
#include <QList>
#include <QMap>
#include <QDateTime>
#include "user.h"

struct Movie {
    int id;
    QString title;
    QString genre;
    QString director;
    int duration; // в минутах
    QString description;
    QString imagePath;
    double rating;
    QString ageRating;
};

struct BookedSeat {
    int sessionId;
    int seatNumber;
    QString username;
    QDateTime bookingTime;
};

struct CinemaSession {
    int id;
    int movieId;
    QDateTime dateTime;
    QString hall;
    double price;
    int availableSeats;
    int totalSeats;
};

class MovieDatabase
{
public:
    int getBookedSeatsCountForSession(int sessionId) const;
    static MovieDatabase& instance();
    void reloadBookedSeats();

    // Работа с пользователями
    bool loadUsers();
    bool saveUsers();
    void addUser(const User& user);
    User* findUser(const QString& username);
    bool validateUser(const QString& username, const QString& password);

    // Работа с билетами
    bool loadBookedTickets(const QString& username);
    bool saveBookedTickets(const QString& username);
    void updateUserTickets(const QString& username, const QList<int>& tickets);

    // Фильмы
    QList<Movie> getMovies() const;
    Movie getMovieById(int id) const;

    // Сеансы
    QList<CinemaSession> getSessions() const;
    QList<CinemaSession> getSessionsForMovie(int movieId) const;
    CinemaSession getSessionById(int id) const;
    bool bookTicket(int sessionId, int seats = 1);  // ИЗМЕНЕНО: добавлен параметр seats
    void updateSession(const CinemaSession& session);
    void updateSessionAvailableSeats();
    void loadBookedSeats();
    void saveBookedSeats();
    bool isSeatBooked(int sessionId, int seatNumber) const;
    void bookSeat(int sessionId, int seatNumber, const QString& username);
    void unbookSeat(int sessionId, int seatNumber);
    int getBookedSeatsCount(int sessionId) const;
    QList<BookedSeat> getAllBookedSeats() const { return bookedSeats; }

private:
    MovieDatabase();
    void loadMovies();
    void loadSessions();

    QList<User> users;
    QList<Movie> movies;
    QList<CinemaSession> sessions;
    QList<BookedSeat> bookedSeats;

};

#endif // MOVIEDATABASE_H
