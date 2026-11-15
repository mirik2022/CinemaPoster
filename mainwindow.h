#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QDateEdit>
#include <QListWidget>
#include <QMenu>
#include "moviedatabase.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setCurrentUser(const QString& username);

private slots:
    void onSearchTextChanged(const QString &text);
    void onFilterChanged();
    void onDateFilterChanged();
    void displayMovies();
    void showBookingDialog(int sessionId);
    void showMyTickets();
    void showProfile();
    void addFunds();
    void refreshMovies();

    // Слоты для сортировки
    void sortByTitle();
    void sortByRating();
    void sortByDuration();
    void sortByPriceAsc();
    void sortByPriceDesc();
    void sortByDate();

    // Новый слот для открытия деталей фильма
    void showMovieDetails(int movieId);

private:
    void setupUI();
    void createMovieCard(const Movie& movie);
    void updateBalanceDisplay();
    void displaySortedMovies(const QList<Movie>& movies);

    // Функции фильтрации
    QList<Movie> filterMovies(const QString& searchText = "");
    QList<Movie> sortMovies(const QList<Movie>& movies, int sortType);

    QScrollArea *scrollArea;
    QWidget *scrollWidget;
    QGridLayout *gridLayout;
    QString currentUser;

    // UI элементы
    QLabel *balanceLabel;
    QLabel *welcomeLabel;
    QLineEdit *searchEdit;
    QComboBox *genreCombo;
    QComboBox *ageCombo;
    QDateEdit *dateEdit;
    QPushButton *ticketsBtn;
    QPushButton *profileBtn;
    QPushButton *refreshBtn;
    QPushButton *sortBtn;
    QMenu *sortMenu;
};

#endif // MAINWINDOW_H
