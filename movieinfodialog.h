#ifndef MOVIEINFODIALOG_H
#define MOVIEINFODIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QFrame>
#include <QButtonGroup>
#include <QGroupBox>
#include "moviedatabase.h"
#include "seatbutton.h"

class MovieInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MovieInfoDialog(int movieId, const QString& username = "", QWidget *parent = nullptr);

signals:
    void bookingRequested(int sessionId, const QList<int>& seats, double totalPrice);

private slots:
    void onSessionSelected(int sessionId);
    void onSeatClicked(int seatNumber);
    void onBookClicked();
    void updateBookingInfo();

private:
    void setupUI();
    QWidget* createMovieInfo();
    QWidget*  createSessionSelector();
    QWidget* createSeatMap();
    void clearSeatSelection();

    int movieId;
    QString username;
    Movie movie;
    QList<CinemaSession> sessions;

    // UI элементы
    QLabel *selectedSessionLabel;
    QLabel *selectedSeatsLabel;
    QLabel *totalPriceLabel;
    QPushButton *bookButton;

    // Текущий выбранный сеанс
    CinemaSession currentSession;

    // Выбранные места
    QList<int> selectedSeats;

    // Карта мест
    QList<SeatButton*> seatButtons;
    QButtonGroup *seatButtonGroup;
};

#endif // MOVIEINFODIALOG_H
