#ifndef SEATBUTTON_H
#define SEATBUTTON_H

#include <QPushButton>

class SeatButton : public QPushButton
{
    Q_OBJECT
public:
    SeatButton(int seatNumber, QWidget *parent = nullptr);
    int getSeatNumber() const { return seatNumber; }
    void setSeatState(bool available, bool selected = false);

private:
    int seatNumber;
};

#endif // SEATBUTTON_H
