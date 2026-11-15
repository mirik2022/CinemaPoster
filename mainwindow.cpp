#include "mainwindow.h"
#include "logindialog.h"
#include "bookingdialog.h"
#include "movieinfodialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QScrollBar>
#include <QMessageBox>
#include <QToolButton>
#include <QStyle>
#include <QFile>
#include <QInputDialog>
#include <QSettings>
#include <QMenu>
#include <algorithm>
#include <QGraphicsDropShadowEffect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("🎬 CINEMA VISION | Афиша");
    setMinimumSize(1400, 900);

    // Загружаем стиль
    QFile styleFile(":style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        setStyleSheet(styleSheet);
    }

    setupUI();
    displayMovies();
}

void MainWindow::displayMovies()
{
    QList<Movie> filtered = filterMovies(searchEdit->text());
    displaySortedMovies(filtered);
}

void MainWindow::setupUI()
{
    // Центральный виджет с градиентом
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Верхняя панель с градиентом
    QWidget *headerWidget = new QWidget();
    headerWidget->setObjectName("headerWidget");
    headerWidget->setFixedHeight(120);

    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(40, 0, 40, 0);

    // Логотип
    QLabel *logoLabel = new QLabel("🎬 CINEMA VISION");
    logoLabel->setObjectName("logoLabel");
    headerLayout->addWidget(logoLabel);

    headerLayout->addStretch();

    // Приветствие пользователя
    welcomeLabel = new QLabel("Добро пожаловать!");
    welcomeLabel->setObjectName("welcomeLabel");
    headerLayout->addWidget(welcomeLabel);

    // Баланс
    balanceLabel = new QLabel("Баланс: 0 ₽");
    balanceLabel->setObjectName("balanceLabel");
    headerLayout->addWidget(balanceLabel);

    // Кнопки
    ticketsBtn = new QPushButton("🎫 Мои билеты");
    ticketsBtn->setObjectName("headerBtn");
    connect(ticketsBtn, &QPushButton::clicked, this, &MainWindow::showMyTickets);
    headerLayout->addWidget(ticketsBtn);

    profileBtn = new QPushButton("👤 Профиль");
    profileBtn->setObjectName("headerBtn");
    connect(profileBtn, &QPushButton::clicked, this, &MainWindow::showProfile);
    headerLayout->addWidget(profileBtn);

    mainLayout->addWidget(headerWidget);

    // Панель фильтров
    QWidget *filterWidget = new QWidget();
    filterWidget->setObjectName("filterWidget");
    filterWidget->setFixedHeight(100);

    QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
    filterLayout->setContentsMargins(40, 15, 40, 15);
    filterLayout->setSpacing(20);

    // Поиск с иконкой
    QWidget *searchContainer = new QWidget();
    searchContainer->setObjectName("searchContainer");
    QHBoxLayout *searchLayout = new QHBoxLayout(searchContainer);
    searchLayout->setContentsMargins(15, 0, 15, 0);

    QLabel *searchIcon = new QLabel("🔍");
    searchIcon->setObjectName("searchIcon");

    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Поиск фильмов...");
    searchEdit->setObjectName("searchEdit");
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    searchLayout->addWidget(searchIcon);
    searchLayout->addWidget(searchEdit);

    filterLayout->addWidget(searchContainer, 2);

    // Фильтры
    genreCombo = new QComboBox();
    genreCombo->addItem("Все жанры");
    genreCombo->addItems({
        "Фантастика", "Драма", "Боевик", "Комедия",
        "Приключения", "Триллер", "Ужасы", "Криминал",
        "Биография", "Мультфильм", "Детектив"
    });
    genreCombo->setObjectName("filterCombo");
    connect(genreCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFilterChanged);
    filterLayout->addWidget(genreCombo);

    dateEdit = new QDateEdit(QDate::currentDate());
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd.MM.yyyy");
    dateEdit->setObjectName("dateEdit");
    connect(dateEdit, &QDateEdit::dateChanged, this, &MainWindow::onDateFilterChanged);
    filterLayout->addWidget(dateEdit);

    // Кнопка сортировки
    sortBtn = new QPushButton("📊 Сортировка");
    sortBtn->setObjectName("filterBtn");
    filterLayout->addWidget(sortBtn);

    // Кнопка обновления
    refreshBtn = new QPushButton("🔄");
    refreshBtn->setObjectName("refreshBtn");
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshMovies);
    filterLayout->addWidget(refreshBtn);

    mainLayout->addWidget(filterWidget);

    // Меню сортировки
    sortMenu = new QMenu(this);
    sortMenu->setObjectName("sortMenu");

    QAction *sortTitleAction = new QAction("📝 По названию (А-Я)", this);
    QAction *sortRatingAction = new QAction("⭐ По рейтингу (высокий)", this);
    QAction *sortDurationAction = new QAction("⏰ По длительности (короткие)", this);
    QAction *sortPriceAscAction = new QAction("💰 По цене (дешевые)", this);
    QAction *sortPriceDescAction = new QAction("💰 По цене (дорогие)", this);

    sortMenu->addAction(sortTitleAction);
    sortMenu->addAction(sortRatingAction);
    sortMenu->addAction(sortDurationAction);
    sortMenu->addSeparator();
    sortMenu->addAction(sortPriceAscAction);
    sortMenu->addAction(sortPriceDescAction);

    sortBtn->setMenu(sortMenu);

    connect(sortTitleAction, &QAction::triggered, this, &MainWindow::sortByTitle);
    connect(sortRatingAction, &QAction::triggered, this, &MainWindow::sortByRating);
    connect(sortDurationAction, &QAction::triggered, this, &MainWindow::sortByDuration);
    connect(sortPriceAscAction, &QAction::triggered, this, &MainWindow::sortByPriceAsc);
    connect(sortPriceDescAction, &QAction::triggered, this, &MainWindow::sortByPriceDesc);

    // Область с фильмами
    scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollWidget = new QWidget();
    scrollWidget->setObjectName("scrollWidget");

    gridLayout = new QGridLayout(scrollWidget);
    gridLayout->setSpacing(30);
    gridLayout->setContentsMargins(40, 30, 40, 40);
    gridLayout->setAlignment(Qt::AlignTop);

    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);
}

void MainWindow::createMovieCard(const Movie& movie)
{
    // Создаем контейнер для карточки
    QWidget *movieWidget = new QWidget();
    movieWidget->setObjectName("movieWidget");
    movieWidget->setMinimumSize(320, 480);
    movieWidget->setMaximumSize(320, 480);

    // Добавляем тень
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 60));
    shadow->setOffset(0, 5);
    movieWidget->setGraphicsEffect(shadow);

    QVBoxLayout *mainLayout = new QVBoxLayout(movieWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Верхняя часть с изображением (кликабельная кнопка)
    QPushButton *imageButton = new QPushButton();
    imageButton->setFixedSize(320, 200);
    imageButton->setFlat(true);
    imageButton->setCursor(Qt::PointingHandCursor);
    imageButton->setStyleSheet(
        "QPushButton {"
        "    border: none;"
        "    border-bottom: 1px solid rgba(255, 255, 255, 0.05);"
        "    padding: 0;"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #1a1a2e, stop:1 #16213e);"
        "}"
        "QPushButton:hover {"
        "    background: rgba(139, 92, 246, 0.1);"
        "}"
        );

    // Загружаем изображение
    QPixmap pixmap;
    if (QFile::exists(movie.imagePath)) {
        pixmap.load(movie.imagePath);
    } else {
        // Пробуем загрузить из ресурсов
        pixmap.load(":/" + movie.imagePath);
    }

    if (!pixmap.isNull()) {
        QPixmap scaledPixmap = pixmap.scaled(300, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Создаем QLabel для изображения внутри кнопки
        QLabel *imageLabel = new QLabel();
        imageLabel->setPixmap(scaledPixmap);
        imageLabel->setAlignment(Qt::AlignCenter);

        QVBoxLayout *imageLayout = new QVBoxLayout(imageButton);
        imageLayout->setContentsMargins(0, 0, 0, 0);
        imageLayout->addWidget(imageLabel);
    } else {
        // Если изображение не загрузилось
        imageButton->setText("🎬");
        imageButton->setStyleSheet(
            "QPushButton {"
            "    border: none;"
            "    border-bottom: 1px solid rgba(255, 255, 255, 0.05);"
            "    padding: 0;"
            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "    stop:0 #1a1a2e, stop:1 #16213e);"
            "    font-size: 50px;"
            "    color: #8b5cf6;"
            "}"
            "QPushButton:hover {"
            "    background: rgba(139, 92, 246, 0.1);"
            "}"
            );
    }

    // Подключаем клик по изображению
    connect(imageButton, &QPushButton::clicked, this, [this, movie]() {
        showMovieDetails(movie.id);
    });

    mainLayout->addWidget(imageButton);

    // Нижняя часть с информацией
    QWidget *infoWidget = new QWidget();
    infoWidget->setObjectName("movieContent");

    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(20, 20, 20, 20);
    infoLayout->setSpacing(12);

    // Заголовок и кнопка информации
    QHBoxLayout *titleLayout = new QHBoxLayout();

    QLabel *titleLabel = new QLabel(movie.title);
    titleLabel->setObjectName("movieTitle");
    titleLabel->setWordWrap(true);
    titleLayout->addWidget(titleLabel, 1);

    QPushButton *infoBtn = new QPushButton("ℹ️");
    infoBtn->setObjectName("detailsBtn");
    infoBtn->setFixedSize(30, 30);
    infoBtn->setToolTip("Подробнее о фильме");
    connect(infoBtn, &QPushButton::clicked, this, [this, movie]() {
        showMovieDetails(movie.id);
    });
    titleLayout->addWidget(infoBtn);

    infoLayout->addLayout(titleLayout);

    // Жанр и возраст
    QStringList genres = movie.genre.split(",");
    QString genresText;
    for (int i = 0; i < genres.size(); i++) {
        genresText += genres[i].trimmed();
        if (i < genres.size() - 1) {
            genresText += ", ";
        }
    }

    QLabel *genreLabel = new QLabel(
        QString("🎭 %1  |  📊 %2")
            .arg(genresText)
            .arg(movie.ageRating)
        );
    genreLabel->setObjectName("movieGenre");
    infoLayout->addWidget(genreLabel);


    // Рейтинг и длительность
    QLabel *ratingLabel = new QLabel(
        QString("⭐ %1/10  |  ⏰ %2 мин")
            .arg(movie.rating)
            .arg(movie.duration)
        );
    ratingLabel->setObjectName("movieRating");
    infoLayout->addWidget(ratingLabel);

    // Режиссер
    QLabel *directorLabel = new QLabel(
        QString("🎬 %1").arg(movie.director)
        );
    directorLabel->setObjectName("movieDirector");
    infoLayout->addWidget(directorLabel);

    // Кнопка "Смотреть детали"
    QPushButton *detailsBtn = new QPushButton("👁 СМОТРЕТЬ ДЕТАЛИ");
    detailsBtn->setObjectName("watchBtn");
    connect(detailsBtn, &QPushButton::clicked, this, [this, movie]() {
        showMovieDetails(movie.id);
    });
    infoLayout->addWidget(detailsBtn);

    mainLayout->addWidget(infoWidget);

    // Добавляем в сетку
    int count = gridLayout->count();
    int row = count / 4;
    int col = count % 4;
    gridLayout->addWidget(movieWidget, row, col);
}

void MainWindow::showMovieDetails(int movieId)
{
    qDebug() << "Opening movie details for ID:" << movieId;

    // Проверяем, существует ли фильм
    Movie movie = MovieDatabase::instance().getMovieById(movieId);
    if (movie.id == 0) {
        QMessageBox::warning(this, "Ошибка", "Фильм не найден!");
        return;
    }

    MovieInfoDialog *dialog = new MovieInfoDialog(movieId, currentUser, this);

    // ИСПРАВИТЬ подключение сигнала:
    connect(dialog, &MovieInfoDialog::bookingRequested,
            this, [this](int sessionId, const QList<int>& seats, double totalPrice) {
                // Вызываем showBookingDialog для конкретного сеанса
                this->showBookingDialog(sessionId);
            });

    dialog->exec();
    dialog->deleteLater();
}

void MainWindow::displaySortedMovies(const QList<Movie>& movies)
{
    // Очищаем старые карточки
    QLayoutItem* child;
    while ((child = gridLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    if (movies.isEmpty()) {
        QLabel *emptyLabel = new QLabel("🎬 Фильмы не найдены\nПопробуйте изменить фильтры");
        emptyLabel->setObjectName("emptyState");
        emptyLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(emptyLabel, 0, 0, 1, 4);
    } else {
        for (const Movie& movie : movies) {
            createMovieCard(movie);
        }
    }

    gridLayout->setRowStretch(gridLayout->rowCount(), 1);
}

QList<Movie> MainWindow::filterMovies(const QString& searchText)
{
    QList<Movie> allMovies = MovieDatabase::instance().getMovies();
    QList<Movie> filteredMovies;

    QString selectedGenre = genreCombo->currentText();
    QDate selectedDate = dateEdit->date();

    for (const Movie& movie : allMovies) {
        // Поиск по всем полям (название, жанры, режиссер)
        bool matchesSearch = searchText.isEmpty() ||
                             movie.title.contains(searchText, Qt::CaseInsensitive) ||
                             movie.genre.contains(searchText, Qt::CaseInsensitive) ||
                             movie.director.contains(searchText, Qt::CaseInsensitive) ||
                             movie.description.contains(searchText, Qt::CaseInsensitive);

        // Фильтрация по жанру - проверяем все жанры фильма
        bool matchesGenre = true;
        if (selectedGenre != "Все жанры" && selectedGenre != "") {
            matchesGenre = false;

            // Разделяем жанры фильма (они могут быть разделены запятыми)
            QStringList movieGenres = movie.genre.split(",", Qt::SkipEmptyParts);
            for (QString &genre : movieGenres) {
                QString trimmedGenre = genre.trimmed();
                if (trimmedGenre == selectedGenre) {
                    matchesGenre = true;
                    break;
                }
            }

            // Также проверяем полное совпадение строки жанра
            if (!matchesGenre && movie.genre.contains(selectedGenre, Qt::CaseInsensitive)) {
                matchesGenre = true;
            }
        }

        // Фильтрация по дате - проверяем наличие сеансов на выбранную дату
        bool matchesDate = true;
        if (selectedDate != QDate::currentDate()) {
            matchesDate = false;
            QList<CinemaSession> sessions = MovieDatabase::instance().getSessionsForMovie(movie.id);
            for (const CinemaSession& session : sessions) {
                if (session.dateTime.date() == selectedDate) {
                    matchesDate = true;
                    break;
                }
            }
        }

        // Если фильм проходит все фильтры - добавляем
        if (matchesSearch && matchesGenre && matchesDate) {
            filteredMovies.append(movie);
        }
    }

    return filteredMovies;
}

QList<Movie> MainWindow::sortMovies(const QList<Movie>& movies, int sortType)
{
    QList<Movie> sortedMovies = movies;

    switch (sortType) {
    case 0: // По названию
        std::sort(sortedMovies.begin(), sortedMovies.end(),
                  [](const Movie& a, const Movie& b) {
                      return a.title.toLower() < b.title.toLower();
                  });
        break;

    case 1: // По рейтингу (убывание)
        std::sort(sortedMovies.begin(), sortedMovies.end(),
                  [](const Movie& a, const Movie& b) {
                      return a.rating > b.rating;
                  });
        break;

    case 2: // По длительности (возрастание)
        std::sort(sortedMovies.begin(), sortedMovies.end(),
                  [](const Movie& a, const Movie& b) {
                      return a.duration < b.duration;
                  });
        break;

    case 3: // По минимальной цене (возрастание)
        std::sort(sortedMovies.begin(), sortedMovies.end(),
                  [this](const Movie& a, const Movie& b) {
                      double priceA = 99999.0;
                      double priceB = 99999.0;

                      QList<CinemaSession> sessionsA = MovieDatabase::instance().getSessionsForMovie(a.id);
                      QList<CinemaSession> sessionsB = MovieDatabase::instance().getSessionsForMovie(b.id);

                      for (const CinemaSession& session : sessionsA) {
                          if (session.price < priceA) priceA = session.price;
                      }
                      for (const CinemaSession& session : sessionsB) {
                          if (session.price < priceB) priceB = session.price;
                      }

                      return priceA < priceB;
                  });
        break;

    case 4: // По минимальной цене (убывание)
        std::sort(sortedMovies.begin(), sortedMovies.end(),
                  [this](const Movie& a, const Movie& b) {
                      double priceA = 0.0;
                      double priceB = 0.0;

                      QList<CinemaSession> sessionsA = MovieDatabase::instance().getSessionsForMovie(a.id);
                      QList<CinemaSession> sessionsB = MovieDatabase::instance().getSessionsForMovie(b.id);

                      for (const CinemaSession& session : sessionsA) {
                          if (session.price > priceA) priceA = session.price;
                      }
                      for (const CinemaSession& session : sessionsB) {
                          if (session.price > priceB) priceB = session.price;
                      }

                      return priceA > priceB;
                  });
        break;

    case 5: // По ближайшей дате сеанса
        std::sort(sortedMovies.begin(), sortedMovies.end(),
                  [this](const Movie& a, const Movie& b) {
                      QDateTime earliestA = QDateTime::currentDateTime().addYears(1);
                      QDateTime earliestB = QDateTime::currentDateTime().addYears(1);

                      QList<CinemaSession> sessionsA = MovieDatabase::instance().getSessionsForMovie(a.id);
                      QList<CinemaSession> sessionsB = MovieDatabase::instance().getSessionsForMovie(b.id);

                      for (const CinemaSession& session : sessionsA) {
                          if (session.dateTime > QDateTime::currentDateTime() &&
                              session.dateTime < earliestA) {
                              earliestA = session.dateTime;
                          }
                      }
                      for (const CinemaSession& session : sessionsB) {
                          if (session.dateTime > QDateTime::currentDateTime() &&
                              session.dateTime < earliestB) {
                              earliestB = session.dateTime;
                          }
                      }

                      return earliestA < earliestB;
                  });
        break;
    }

    return sortedMovies;
}

void MainWindow::sortByTitle()
{
    QList<Movie> filtered = filterMovies(searchEdit->text());
    QList<Movie> sorted = sortMovies(filtered, 0);
    displaySortedMovies(sorted);
    sortBtn->setText("📊 По названию");
}

void MainWindow::sortByRating()
{
    QList<Movie> filtered = filterMovies(searchEdit->text());
    QList<Movie> sorted = sortMovies(filtered, 1);
    displaySortedMovies(sorted);
    sortBtn->setText("📊 По рейтингу");
}

void MainWindow::sortByDuration()
{
    QList<Movie> filtered = filterMovies(searchEdit->text());
    QList<Movie> sorted = sortMovies(filtered, 2);
    displaySortedMovies(sorted);
    sortBtn->setText("📊 По длительности");
}

void MainWindow::sortByPriceAsc()
{
    QList<Movie> filtered = filterMovies(searchEdit->text());
    QList<Movie> sorted = sortMovies(filtered, 3);
    displaySortedMovies(sorted);
    sortBtn->setText("📊 По цене (↑)");
}

void MainWindow::sortByPriceDesc()
{
    QList<Movie> filtered = filterMovies(searchEdit->text());
    QList<Movie> sorted = sortMovies(filtered, 4);
    displaySortedMovies(sorted);
    sortBtn->setText("📊 По цене (↓)");
}

void MainWindow::sortByDate()
{
    QList<Movie> filtered = filterMovies(searchEdit->text());
    QList<Movie> sorted = sortMovies(filtered, 5);
    displaySortedMovies(sorted);
    sortBtn->setText("📊 По дате");
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    displayMovies();
}

void MainWindow::onFilterChanged()
{
    displayMovies();
}

void MainWindow::onDateFilterChanged()
{
    displayMovies();
}

void MainWindow::showBookingDialog(int sessionId)
{
    if (currentUser.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Для бронирования необходимо войти в систему");
        return;
    }

    qDebug() << "\n=== ОТКРЫТИЕ БРОНИРОВАНИЯ ИЗ ГЛАВНОГО ОКНА ===";

    // Принудительно загружаем данные ПЕРЕД открытием диалога
    MovieDatabase::instance().loadBookedSeats();

    // Проверяем файл
    QFile file("booked_seats.txt");
    if (file.exists()) {
        qDebug() << "Файл booked_seats.txt существует, размер:" << file.size() << "байт";
        if (file.open(QIODevice::ReadOnly)) {
            QString content = file.readAll();
            qDebug() << "Содержимое файла:";
            qDebug() << content;
            file.close();
        }
    } else {
        qDebug() << "Файл booked_seats.txt НЕ существует!";
    }

    BookingDialog *dialog = new BookingDialog(sessionId, currentUser, this);
    connect(dialog, &BookingDialog::bookingCompleted, this, &MainWindow::refreshMovies);
    dialog->exec();
    dialog->deleteLater();
}

void MainWindow::showMyTickets()
{
    if (currentUser.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Войдите в систему для просмотра билетов");
        return;
    }

    User* user = MovieDatabase::instance().findUser(currentUser);
    if (!user) return;

    QList<int> ticketIds = user->getBookedTickets();
    if (ticketIds.isEmpty()) {
        QMessageBox::information(this, "Мои билеты", "У вас нет забронированных билетов");
        return;
    }

    // Создаем диалог для красивого отображения билетов
    QDialog *ticketsDialog = new QDialog(this);
    ticketsDialog->setWindowTitle("🎫 Мои билеты");
    ticketsDialog->setMinimumSize(500, 600);
    ticketsDialog->setStyleSheet(
        "QDialog {"
        "    background: #1a1a2e;"
        "    border: 2px solid #2a2a4a;"
        "    border-radius: 15px;"
        "}"
        "QLabel {"
        "    color: white;"
        "}"
        "QPushButton {"
        "    background: #00adb5;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 10px 20px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: #009ca3;"
        "}"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(ticketsDialog);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QLabel *titleLabel = new QLabel("🎫 ВАШИ БИЛЕТЫ");
    titleLabel->setStyleSheet("color: #8b5cf6; font-size: 24px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Область с билетами
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    QWidget *ticketsWidget = new QWidget();
    QVBoxLayout *ticketsLayout = new QVBoxLayout(ticketsWidget);
    ticketsLayout->setSpacing(15);
    ticketsLayout->setContentsMargins(10, 10, 10, 10);

    for (int ticketId : ticketIds) {
        int sessionId = ticketId / 1000;
        int seatNumber = ticketId % 1000;

        CinemaSession session = MovieDatabase::instance().getSessionById(sessionId);
        Movie movie = MovieDatabase::instance().getMovieById(session.movieId);

        // Карточка билета
        QWidget *ticketCard = new QWidget();
        ticketCard->setStyleSheet(
            "QWidget {"
            "    background: rgba(139, 92, 246, 0.1);"
            "    border: 2px solid rgba(139, 92, 246, 0.3);"
            "    border-radius: 10px;"
            "    padding: 15px;"
            "}"
            );

        QVBoxLayout *ticketLayout = new QVBoxLayout(ticketCard);
        ticketLayout->setSpacing(8);

        // Заголовок фильма
        QLabel *movieLabel = new QLabel(movie.title);
        movieLabel->setStyleSheet("color: #8b5cf6; font-size: 16px; font-weight: bold;");
        ticketLayout->addWidget(movieLabel);

        // Детали сеанса
        QLabel *dateLabel = new QLabel("📅 " + session.dateTime.toString("dd.MM.yyyy hh:mm"));
        dateLabel->setStyleSheet("color: white; font-size: 14px;");
        ticketLayout->addWidget(dateLabel);

        QLabel *hallLabel = new QLabel("🎭 " + session.hall + " | 💺 Место: " + QString::number(seatNumber));
        hallLabel->setStyleSheet("color: white; font-size: 14px;");
        ticketLayout->addWidget(hallLabel);

        QLabel *priceLabel = new QLabel("💰 " + QString::number(session.price, 'f', 2) + " BYN");
        priceLabel->setStyleSheet("color: #00ff00; font-size: 14px; font-weight: bold;");
        ticketLayout->addWidget(priceLabel);

        // Кнопка отмены
        QPushButton *cancelBtn = new QPushButton("❌ Отменить бронь");
        cancelBtn->setStyleSheet(
            "QPushButton {"
            "    background: rgba(255, 77, 141, 0.2);"
            "    color: #ff4d8d;"
            "    border: 1px solid rgba(255, 77, 141, 0.4);"
            "    border-radius: 5px;"
            "    padding: 8px;"
            "    font-size: 12px;"
            "    margin-top: 5px;"
            "}"
            "QPushButton:hover {"
            "    background: rgba(255, 77, 141, 0.3);"
            "}"
            );
        connect(cancelBtn, &QPushButton::clicked, this, [this, ticketId, user, session, ticketCard]() {
            // Возвращаем деньги
            user->addBalance(session.price);
            int seatNumber = ticketId % 1000;

            // Удаляем бронирование места из базы
            MovieDatabase::instance().unbookSeat(session.id, seatNumber);
            user->removeBookedTicket(ticketId);

            // Освобождаем место
            CinemaSession updatedSession = session;
            updatedSession.availableSeats++;
            MovieDatabase::instance().updateSession(updatedSession);

            // Сохраняем изменения
            MovieDatabase::instance().saveBookedTickets(currentUser);
            MovieDatabase::instance().saveUsers();

            // Удаляем карточку
            ticketCard->deleteLater();
            updateBalanceDisplay();

            QMessageBox::information(this, "Успех",
                                     "Бронь отменена!\n" +
                                         QString("Возвращено: %1 BYN").arg(QString::number(session.price, 'f', 2)) +
                                         QString("\nНовый баланс: %1 BYN").arg(QString::number(user->getBalance(), 'f', 2)));
        });

        ticketLayout->addWidget(cancelBtn);
        ticketsLayout->addWidget(ticketCard);
    }

    ticketsLayout->addStretch();
    scrollArea->setWidget(ticketsWidget);
    mainLayout->addWidget(scrollArea);

    // Кнопка закрытия
    QPushButton *closeBtn = new QPushButton("Закрыть");
    connect(closeBtn, &QPushButton::clicked, ticketsDialog, &QDialog::accept);
    mainLayout->addWidget(closeBtn);

    ticketsDialog->exec();
    ticketsDialog->deleteLater();
}

void MainWindow::showProfile()
{
    if (currentUser.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Войдите в систему");
        return;
    }

    User* user = MovieDatabase::instance().findUser(currentUser);
    if (!user) return;

    QString profileInfo = QString("👤 <b style='color:#8b5cf6; font-size:16px;'>%1</b><br><br>"
                                  "📧 <span style='color:#cccccc;'>%2</span><br><br>"
                                  "💰 <span style='color:#3b82f6; font-size:18px; font-weight:bold;'>Баланс: %3 BYN</span><br><br>"
                                  "🎟 <span style='color:#cccccc;'>Билетов: %4 шт.</span><br><br>"
                                  "🔐 <span style='color:#ff4d8d;'>Код для пополнения: <b>%5</b></span><br>"
                                  "<small style='color:#888888;'>Сообщите этот код администратору для пополнения баланса</small>")
                              .arg(user->getUsername())
                              .arg(user->getEmail())
                              .arg(QString::number(user->getBalance(), 'f', 2))
                              .arg(user->getBookedTickets().size())
                              .arg(user->getRechargeCode());

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("👤 Профиль пользователя");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(profileInfo);
    msgBox.setIcon(QMessageBox::Information);

    QPushButton *addFundsBtn = msgBox.addButton("💰 Пополнить баланс", QMessageBox::ActionRole);
    QPushButton *closeBtn = msgBox.addButton("✕ Закрыть", QMessageBox::AcceptRole);

    // Стилизация кнопок
    addFundsBtn->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #3b82f6, stop:1 #8b5cf6);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 10px;"
        "    padding: 12px 25px;"
        "    font-size: 14px;"
        "    font-weight: 600;"
        "    min-width: 180px;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #2563eb, stop:1 #7c3aed);"
        "}"
        );

    closeBtn->setStyleSheet(
        "QPushButton {"
        "    background: rgba(255, 255, 255, 0.1);"
        "    color: white;"
        "    border: 2px solid rgba(255, 255, 255, 0.2);"
        "    border-radius: 10px;"
        "    padding: 12px 25px;"
        "    font-size: 14px;"
        "    font-weight: 600;"
        "    min-width: 100px;"
        "}"
        "QPushButton:hover {"
        "    background: rgba(255, 255, 255, 0.15);"
        "    border-color: rgba(255, 255, 255, 0.3);"
        "}"
        );

    msgBox.setStyleSheet(
        "QMessageBox {"
        "    background: #1a1a2e;"
        "    border: 2px solid #2a2a4a;"
        "    border-radius: 20px;"
        "}"
        "QMessageBox QLabel {"
        "    color: white;"
        "    font-size: 14px;"
        "    padding: 25px;"  // Оставили только padding
        "}"
        );

    msgBox.exec();

    if (msgBox.clickedButton() == addFundsBtn) {
        addFunds();
    }
}

void MainWindow::addFunds()
{
    User* user = MovieDatabase::instance().findUser(currentUser);
    if (!user) {
        QMessageBox::warning(this, "Ошибка", "Пользователь не найден!");
        return;
    }

    // Диалог для администратора
    bool ok;
    QString adminPassword = QInputDialog::getText(this, "Администратор",
                                                  "Введите пароль администратора:",
                                                  QLineEdit::Password, "", &ok);

    if (!ok) return;

    // Проверяем пароль администратора
    User* admin = MovieDatabase::instance().findUser("admin");
    if (!admin || admin->getPassword() != adminPassword) {
        QMessageBox::warning(this, "Ошибка", "Неверный пароль администратора!");
        return;
    }

    // Получаем код пополнения для пользователя
    QString rechargeCode = QInputDialog::getText(this, "Пополнение баланса",
                                                 QString("Введите код пополнения для пользователя %1:")
                                                     .arg(currentUser),
                                                 QLineEdit::Normal, "", &ok);

    if (!ok) return;

    // Проверяем код пополнения
    if (!user->validateRechargeCode(rechargeCode)) {
        QMessageBox::warning(this, "Ошибка", "Неверный код пополнения!");
        return;
    }

    // Получаем сумму для пополнения
    double amount = QInputDialog::getDouble(this, "Пополнение баланса",
                                            "Введите сумму для пополнения (BYN):",
                                            10.0, 1.0, 1000.0, 2, &ok);
    if (ok && amount > 0) {
        user->addBalance(amount);

        // Генерируем новый код после пополнения (для безопасности)
        user->generateRechargeCode();

        MovieDatabase::instance().saveUsers();
        updateBalanceDisplay();

        QMessageBox::information(this, "Успех",
                                 QString("Баланс пополнен на %1 BYN.\n"
                                         "Текущий баланс: %2 BYN\n\n"
                                         "🔐 <b>Новый код для пополнения: %3</b>")
                                     .arg(QString::number(amount, 'f', 2))
                                     .arg(QString::number(user->getBalance(), 'f', 2))
                                     .arg(user->getRechargeCode()));
    }
}

void MainWindow::updateBalanceDisplay()
{
    if (!currentUser.isEmpty()) {
        User* user = MovieDatabase::instance().findUser(currentUser);
        if (user) {
            balanceLabel->setText(QString("Баланс: %1 BYN").arg(QString::number(user->getBalance(), 'f', 2)));
        }
    }
}

void MainWindow::refreshMovies()
{
    displayMovies();
    updateBalanceDisplay();
}

void MainWindow::setCurrentUser(const QString& username)
{
    currentUser = username;
    updateBalanceDisplay();
    MovieDatabase::instance().loadBookedTickets(username);

    if (!username.isEmpty()) {
        welcomeLabel->setText("Привет, " + username + "!");
    }
}

MainWindow::~MainWindow() {}
