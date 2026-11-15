#include "seatbutton.h"
#include <QStyle>

SeatButton::SeatButton(int seatNumber, QWidget *parent)
    : QPushButton(QString::number(seatNumber), parent), seatNumber(seatNumber)
{
    setFixedSize(40, 40); // Увеличили размер
    setText(QString::number(seatNumber));
}

void SeatButton::setSeatState(bool available, bool selected)
{
    if (!available) {
        setEnabled(false);
        setStyleSheet(
            "QPushButton {"
            "    background: #555555;"  // Серый - занято
            "    color: #888888;"
            "    border: 2px solid #444444;"
            "    border-radius: 6px;"
            "    font-weight: bold;"
            "    font-size: 12px;"
            "}"
            );
    } else if (selected) {
        setChecked(true);
        setStyleSheet(
            "QPushButton {"
            "    background: #00ff00;"  // Зеленый - выбрано
            "    color: #003300;"
            "    border: 2px solid #00cc00;"
            "    border-radius: 6px;"
            "    font-weight: bold;"
            "    font-size: 12px;"
            "}"
            "QPushButton:hover {"
            "    background: #00cc00;"
            "}"
            );
    } else {
        setChecked(false);
        setStyleSheet(
            "QPushButton {"
            "    background: #3b82f6;"  // Синий - свободно
            "    color: white;"
            "    border: 2px solid #2563eb;"
            "    border-radius: 6px;"
            "    font-weight: bold;"
            "    font-size: 12px;"
            "}"
            "QPushButton:hover {"
            "    background: #2563eb;"
            "}"
            "QPushButton:checked {"
            "    background: #00ff00;"
            "    color: #003300;"
            "    border-color: #00cc00;"
            "}"
            );
    }
}
