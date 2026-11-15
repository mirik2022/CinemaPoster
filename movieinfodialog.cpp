#include "movieinfodialog.h"
#include "seatbutton.h"
#include <QScrollArea>
#include <QGroupBox>
#include <QButtonGroup>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QFile>

MovieInfoDialog::MovieInfoDialog(int movieId, const QString& username, QWidget *parent)
    : QDialog(parent), movieId(movieId), username(username), seatButtonGroup(new QButtonGroup(this))
{
    movie = MovieDatabase::instance().getMovieById(movieId);
    sessions = MovieDatabase::instance().getSessionsForMovie(movieId);

    if (movie.id == 0) {
        QMessageBox::warning(this, "Ошибка", "Фильм не найден!");
        return;
    }

    // Выбираем первый доступный сеанс по умолчанию
    if (!sessions.isEmpty()) {
        currentSession = sessions.first();
    }

    setWindowTitle(movie.title + " | Бронирование");
    setMinimumSize(1000, 800);
    setMaximumSize(1200, 900);

    // Стиль
    setStyleSheet(
        "QDialog {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "    stop:0 #0a0a1f, stop:1 #15152b);"
        "    border: 2px solid #2a2a4a;"
        "    border-radius: 20px;"
        "}"
        "QLabel {"
        "    color: #ffffff;"
        "}"
        "QPushButton {"
        "    border-radius: 10px;"
        "    font-weight: bold;"
        "    padding: 12px 25px;"
        "    font-size: 14px;"
        "    border: 2px solid transparent;"
        "}"
        "QScrollArea {"
        "    border: none;"
        "    background: transparent;"
        "}"
        "QGroupBox {"
        "    color: #ffffff;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    border: 2px solid #3b82f6;"
        "    border-radius: 10px;"
        "    margin-top: 10px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 10px;"
        "}"
        );

    setupUI();
}

void MovieInfoDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(20);

    // Заголовок
    QLabel *titleLabel = new QLabel("🎬 " + movie.title);
    titleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 28px;"
        "    font-weight: bold;"
        "    color: #8b5cf6;"
        "    padding: 10px 0;"
        "}"
        );
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Основной контент в QScrollArea
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(25);

    // Информация о фильме
    contentLayout->addWidget(createMovieInfo());

    // Выбор сеанса
    contentLayout->addWidget(createSessionSelector());

    // Карта мест
    contentLayout->addWidget(createSeatMap());

    // Информация о бронировании
    QGroupBox *bookingInfoGroup = new QGroupBox("📋 ИНФОРМАЦИЯ О БРОНИРОВАНИИ");
    bookingInfoGroup->setStyleSheet(
        "QGroupBox {"
        "    border-color: #ff4d8d;"
        "}"
        );

    QVBoxLayout *bookingLayout = new QVBoxLayout(bookingInfoGroup);
    bookingLayout->setSpacing(10);

    selectedSessionLabel = new QLabel("Сеанс: не выбран");
    selectedSessionLabel->setStyleSheet("color: #8b5cf6; font-size: 14px;");

    selectedSeatsLabel = new QLabel("Выбранные места: нет");
    selectedSeatsLabel->setStyleSheet("color: #3b82f6; font-size: 14px;");

    totalPriceLabel = new QLabel("Общая стоимость: 0 ₽");
    totalPriceLabel->setStyleSheet("color: #00ff00; font-size: 16px; font-weight: bold;");

    bookingLayout->addWidget(selectedSessionLabel);
    bookingLayout->addWidget(selectedSeatsLabel);
    bookingLayout->addWidget(totalPriceLabel);

    // Кнопка бронирования
    bookButton = new QPushButton("🎫 ЗАБРОНИРОВАТЬ БИЛЕТЫ");
    bookButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #ff4d8d, stop:1 #8b5cf6);"
        "    color: white;"
        "    font-size: 16px;"
        "    min-height: 50px;"
        "    margin-top: 15px;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #ff2d7a, stop:1 #7c3aed);"
        "}"
        "QPushButton:disabled {"
        "    background: #555555;"
        "    color: #888888;"
        "}"
        );
    bookButton->setEnabled(false);
    connect(bookButton, &QPushButton::clicked, this, &MovieInfoDialog::onBookClicked);

    bookingLayout->addWidget(bookButton);

    contentLayout->addWidget(bookingInfoGroup);

    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);

    // Обновляем информацию при изменении
    updateBookingInfo();
}

QWidget* MovieInfoDialog::createMovieInfo()
{
    QWidget *infoWidget = new QWidget();
    infoWidget->setStyleSheet(
        "QWidget {"
        "    background: rgba(26, 26, 46, 0.7);"
        "    border-radius: 15px;"
        "    border: 1px solid rgba(139, 92, 246, 0.3);"
        "}"
        );

    QHBoxLayout *layout = new QHBoxLayout(infoWidget);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(25);

    // Изображение фильма
    QLabel *imageLabel = new QLabel();
    imageLabel->setFixedSize(220, 330);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet(
        "QLabel {"
        "    background: rgba(26, 26, 46, 0.8);"
        "    border-radius: 10px;"
        "    border: 2px solid rgba(139, 92, 246, 0.4);"
        "}"
        );

    QPixmap pixmap;
    if (QFile::exists(movie.imagePath)) {
        pixmap.load(movie.imagePath);
    } else {
        pixmap.load(":/" + movie.imagePath);
    }

    if (!pixmap.isNull()) {
        QPixmap scaledPixmap = pixmap.scaled(200, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(scaledPixmap);
    } else {
        imageLabel->setText("🎬");
        imageLabel->setStyleSheet(
            "QLabel {"
            "    background: rgba(26, 26, 46, 0.8);"
            "    border-radius: 10px;"
            "    border: 2px solid rgba(139, 92, 246, 0.4);"
            "    font-size: 80px;"
            "    color: #8b5cf6;"
            "}"
            );
    }

    layout->addWidget(imageLabel);

    // Детальная информация о фильме
    QWidget *detailsWidget = new QWidget();
    QVBoxLayout *detailsLayout = new QVBoxLayout(detailsWidget);
    detailsLayout->setSpacing(15);
    detailsLayout->setContentsMargins(0, 0, 0, 0);

    // Заголовок
    QLabel *movieTitle = new QLabel(movie.title);
    movieTitle->setStyleSheet(
        "QLabel {"
        "    color: #ff4d8d;"
        "    font-size: 24px;"
        "    font-weight: bold;"
        "    margin-bottom: 10px;"
        "}"
        );
    detailsLayout->addWidget(movieTitle);

    // Основная информация
    QWidget *infoGridWidget = new QWidget();
    QGridLayout *infoGrid = new QGridLayout(infoGridWidget);
    infoGrid->setSpacing(12);
    infoGrid->setContentsMargins(0, 0, 0, 0);

    // Жанры - показываем все
    QLabel *genreTitle = new QLabel("🎭 ЖАНРЫ:");
    genreTitle->setStyleSheet("color: #8b5cf6; font-weight: bold; font-size: 14px;");

    QLabel *genreValue = new QLabel(movie.genre);
    genreValue->setStyleSheet("color: #cccccc; font-size: 14px;");
    genreValue->setWordWrap(true);

    infoGrid->addWidget(genreTitle, 0, 0);
    infoGrid->addWidget(genreValue, 0, 1, 1, 3);

    // Режиссер
    QLabel *directorTitle = new QLabel("🎬 РЕЖИССЕР:");
    directorTitle->setStyleSheet("color: #8b5cf6; font-weight: bold; font-size: 14px;");

    QLabel *directorValue = new QLabel(movie.director);
    directorValue->setStyleSheet("color: #cccccc; font-size: 14px;");

    infoGrid->addWidget(directorTitle, 1, 0);
    infoGrid->addWidget(directorValue, 1, 1);

    // Длительность
    QLabel *durationTitle = new QLabel("⏰ ДЛИТЕЛЬНОСТЬ:");
    durationTitle->setStyleSheet("color: #8b5cf6; font-weight: bold; font-size: 14px;");

    QLabel *durationValue = new QLabel(QString("%1 мин.").arg(movie.duration));
    durationValue->setStyleSheet("color: #cccccc; font-size: 14px;");

    infoGrid->addWidget(durationTitle, 1, 2);
    infoGrid->addWidget(durationValue, 1, 3);

    // Рейтинг
    QLabel *ratingTitle = new QLabel("⭐ РЕЙТИНГ:");
    ratingTitle->setStyleSheet("color: #8b5cf6; font-weight: bold; font-size: 14px;");

    QLabel *ratingValue = new QLabel(QString("%1/10").arg(movie.rating));
    ratingValue->setStyleSheet("color: #ffd700; font-size: 14px; font-weight: bold;");

    infoGrid->addWidget(ratingTitle, 2, 0);
    infoGrid->addWidget(ratingValue, 2, 1);

    // Возрастной рейтинг
    QLabel *ageTitle = new QLabel("📊 ВОЗРАСТ:");
    ageTitle->setStyleSheet("color: #8b5cf6; font-weight: bold; font-size: 14px;");

    QLabel *ageValue = new QLabel(movie.ageRating);
    ageValue->setStyleSheet(
        QString("color: white; font-size: 14px; font-weight: bold; "
                "background: %1; padding: 3px 8px; border-radius: 5px;")
            .arg(movie.ageRating == "18+" ? "#ff4d8d" :
                     movie.ageRating == "16+" ? "#f59e0b" :
                     movie.ageRating == "12+" ? "#10b981" : "#3b82f6")
        );

    infoGrid->addWidget(ageTitle, 2, 2);
    infoGrid->addWidget(ageValue, 2, 3);

    detailsLayout->addWidget(infoGridWidget);

    // Разделитель
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("background: rgba(139, 92, 246, 0.3); margin: 10px 0;");
    detailsLayout->addWidget(separator);

    // Описание фильма
    QLabel *descriptionTitle = new QLabel("📖 ОПИСАНИЕ:");
    descriptionTitle->setStyleSheet("color: #3b82f6; font-size: 16px; font-weight: bold; margin-bottom: 8px;");
    detailsLayout->addWidget(descriptionTitle);

    QLabel *descriptionLabel = new QLabel(movie.description);
    descriptionLabel->setStyleSheet("color: #cccccc; font-size: 13px; line-height: 1.5;");
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setTextFormat(Qt::PlainText);
    detailsLayout->addWidget(descriptionLabel);

    // Разделитель для оценки фильма
    separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("background: rgba(139, 92, 246, 0.3); margin: 15px 0;");
    detailsLayout->addWidget(separator);

    // Оценка фильма
    QLabel *ratingTitle2 = new QLabel("⭐ ВАША ОЦЕНКА ФИЛЬМУ:");
    ratingTitle2->setStyleSheet("color: #10b981; font-size: 16px; font-weight: bold; margin-bottom: 10px;");
    detailsLayout->addWidget(ratingTitle2);

    // Виджет со звездами для оценки
    QWidget *starsWidget = new QWidget();
    QHBoxLayout *starsLayout = new QHBoxLayout(starsWidget);
    starsLayout->setSpacing(5);
    starsLayout->setAlignment(Qt::AlignCenter);
    starsLayout->setContentsMargins(0, 0, 0, 0);

    // Создаем 10 звезд
    for (int i = 1; i <= 10; i++) {
        QPushButton *starBtn = new QPushButton("★");
        starBtn->setFixedSize(32, 32);
        starBtn->setCheckable(true);
        starBtn->setProperty("rating", i);
        starBtn->setCursor(Qt::PointingHandCursor);

        starBtn->setStyleSheet(
            "QPushButton {"
            "    background: transparent;"
            "    color: #555555;"
            "    font-size: 20px;"
            "    border: none;"
            "    padding: 0;"
            "    margin: 0;"
            "}"
            "QPushButton:hover {"
            "    color: #ffcc00;"
            "    transform: scale(1.1);"
            "}"
            "QPushButton:checked {"
            "    color: #ffd700;"
            "}"
            );

        // Подключаем нажатие на звезду
        connect(starBtn, &QPushButton::clicked, [this, i, starsWidget]() {
            // Устанавливаем все звезды до i включительно как выбранные
            QLayout *layout = starsWidget->layout();
            for (int j = 0; j < layout->count(); j++) {
                QPushButton *btn = qobject_cast<QPushButton*>(layout->itemAt(j)->widget());
                if (btn) {
                    int btnRating = btn->property("rating").toInt();
                    btn->setChecked(btnRating <= i);
                }
            }

            // Сохраняем оценку (здесь можно добавить сохранение в базу данных)
            QMessageBox msgBox;
            msgBox.setWindowTitle("✨ Спасибо за оценку!");
            msgBox.setText(QString("<div style='text-align: center;'>"
                                   "<p style='color: white; font-size: 14px;'>"
                                   "Вы поставили фильму оценку <b style='color: #ffd700; font-size: 16px;'>%1/10</b>"
                                   "</p></div>").arg(i));
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setStyleSheet(
                "QMessageBox {"
                "    background: #1a1a2e;"
                "    border: 2px solid #2a2a4a;"
                "    border-radius: 15px;"
                "}"
                "QMessageBox QLabel {"
                "    color: white;"
                "    font-size: 14px;"
                "    padding: 15px;"
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
        });

        starsLayout->addWidget(starBtn);
    }

    detailsLayout->addWidget(starsWidget);

    // Подпись под звездами
    QLabel *ratingHint = new QLabel("(Нажмите на звезду, чтобы поставить оценку)");
    ratingHint->setStyleSheet("color: #888888; font-size: 11px; font-style: italic; text-align: center; margin-top: 5px;");
    detailsLayout->addWidget(ratingHint);

    detailsLayout->addStretch();

    layout->addWidget(detailsWidget, 1);

    return infoWidget;
}

QWidget* MovieInfoDialog::createSessionSelector()
{
    QGroupBox *sessionGroup = new QGroupBox("📅 ВЫБЕРИТЕ СЕАНС");

    QVBoxLayout *groupLayout = new QVBoxLayout(sessionGroup);
    groupLayout->setSpacing(15);

    if (sessions.isEmpty()) {
        QLabel *noSessionsLabel = new QLabel("🎬 На данный момент сеансов нет");
        noSessionsLabel->setStyleSheet("color: #888888; font-style: italic; padding: 20px; text-align: center;");
        groupLayout->addWidget(noSessionsLabel);
        return sessionGroup;
    }

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(10);

    int row = 0, col = 0;
    const int COLUMNS = 2; // 2 столбца для лучшего отображения

    for (int i = 0; i < sessions.size(); i++) {
        const CinemaSession &session = sessions[i];

        // Упрощенный текст без количества мест
        QPushButton *sessionBtn = new QPushButton(
            QString("%1\n%2\n📍 %3\n💰 %4 BYN")
                .arg(session.dateTime.toString("dd.MM.yyyy"))
                .arg(session.dateTime.toString("hh:mm"))
                .arg(session.hall)
                .arg(QString::number(session.price, 'f', 2))
            );

        sessionBtn->setCheckable(true);
        sessionBtn->setFixedHeight(80); // Уменьшили высоту
        sessionBtn->setMinimumWidth(220); // Увеличили ширину

        if (session.availableSeats > 0) {
            sessionBtn->setStyleSheet(
                "QPushButton {"
                "    background: rgba(59, 130, 246, 0.2);"
                "    color: #3b82f6;"
                "    border: 2px solid rgba(59, 130, 246, 0.4);"
                "    border-radius: 8px;"
                "    font-size: 12px;" // Увеличили шрифт
                "    padding: 5px;"
                "}"
                "QPushButton:hover {"
                "    background: rgba(59, 130, 246, 0.3);"
                "    border-color: #3b82f6;"
                "}"
                "QPushButton:checked {"
                "    background: #3b82f6;"
                "    color: white;"
                "    border-color: #2563eb;"
                "}"
                );
        } else {
            sessionBtn->setEnabled(false);
            sessionBtn->setStyleSheet(
                "QPushButton {"
                "    background: #555555;"
                "    color: #888888;"
                "    border: 2px solid #444444;"
                "    border-radius: 8px;"
                "    font-size: 12px;"
                "    padding: 5px;"
                "}"
                );
        }

        connect(sessionBtn, &QPushButton::clicked, this, [this, session]() {
            onSessionSelected(session.id);
        });

        // Выбираем первый доступный сеанс
        if (i == 0 && session.availableSeats > 0) {
            sessionBtn->setChecked(true);
            currentSession = session;
        }

        gridLayout->addWidget(sessionBtn, row, col);

        col++;
        if (col >= COLUMNS) {
            col = 0;
            row++;
        }
    }

    groupLayout->addLayout(gridLayout);
    return sessionGroup;
}

QWidget* MovieInfoDialog::createSeatMap()
{
    QGroupBox *seatGroup = new QGroupBox("🎭 ВЫБЕРИТЕ МЕСТА");
    seatGroup->setStyleSheet(
        "QGroupBox {"
        "    border-color: #00ff00;"
        "}"
        );

    QVBoxLayout *groupLayout = new QVBoxLayout(seatGroup);

    // Легенда
    QWidget *legendWidget = new QWidget();
    QHBoxLayout *legendLayout = new QHBoxLayout(legendWidget);
    legendLayout->setSpacing(15);

    // Создаем цветные квадраты для легенды
    QLabel *availableColor = new QLabel();
    availableColor->setFixedSize(20, 20);
    availableColor->setStyleSheet("background: #3b82f6; border: 1px solid #2563eb; border-radius: 3px;");

    QLabel *selectedColor = new QLabel();
    selectedColor->setFixedSize(20, 20);
    selectedColor->setStyleSheet("background: #00ff00; border: 1px solid #00cc00; border-radius: 3px;");

    QLabel *occupiedColor = new QLabel();
    occupiedColor->setFixedSize(20, 20);
    occupiedColor->setStyleSheet("background: #555555; border: 1px solid #444444; border-radius: 3px;");

    QLabel *availableLabel = new QLabel("Свободно");
    availableLabel->setStyleSheet("color: #3b82f6;");

    QLabel *selectedLabel = new QLabel("Выбрано");
    selectedLabel->setStyleSheet("color: #00ff00;");

    QLabel *occupiedLabel = new QLabel("Занято");
    occupiedLabel->setStyleSheet("color: #555555;");

    legendLayout->addWidget(availableColor);
    legendLayout->addWidget(availableLabel);
    legendLayout->addSpacing(15);
    legendLayout->addWidget(selectedColor);
    legendLayout->addWidget(selectedLabel);
    legendLayout->addSpacing(15);
    legendLayout->addWidget(occupiedColor);
    legendLayout->addWidget(occupiedLabel);
    legendLayout->addStretch();

    groupLayout->addWidget(legendWidget);

    // "Экран"
    QLabel *screenLabel = new QLabel("────────── 🎬 ЭКРАН 🎬 ──────────");
    screenLabel->setAlignment(Qt::AlignCenter);
    screenLabel->setStyleSheet(
        "QLabel {"
        "    color: #ffd700;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    background: rgba(255, 215, 0, 0.1);"
        "    border-radius: 5px;"
        "    padding: 10px;"
        "    margin: 20px 0;"
        "}"
        );
    groupLayout->addWidget(screenLabel);

    // Сетка мест
    QWidget *seatMapWidget = new QWidget();
    QGridLayout *seatLayout = new QGridLayout(seatMapWidget);
    seatLayout->setSpacing(5);
    seatLayout->setHorizontalSpacing(10);

    int rows = 8;
    int seatsPerRow = 10;
    int totalSeats = rows * seatsPerRow;

    // Очищаем предыдущие кнопки
    clearSeatSelection();
    seatButtons.clear();

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < seatsPerRow; col++) {
            int seatNumber = row * seatsPerRow + col + 1;
            SeatButton *seatBtn = new SeatButton(seatNumber);

            // ПРОВЕРКА: занято ли место в БАЗЕ ДАННЫХ
            bool isBooked = MovieDatabase::instance().isSeatBooked(currentSession.id, seatNumber);

            qDebug() << "MovieInfoDialog: Место" << seatNumber
                     << "для сеанса" << currentSession.id
                     << "- забронировано:" << isBooked;

            seatBtn->setSeatState(!isBooked, selectedSeats.contains(seatNumber));
            seatBtn->setEnabled(!isBooked);

            if (!isBooked) {
                connect(seatBtn, &QPushButton::clicked, this, [this, seatNumber]() {
                    onSeatClicked(seatNumber);
                });
            }

            seatLayout->addWidget(seatBtn, row, col + 1);
            seatButtons.append(seatBtn);
        }
    }

    groupLayout->addWidget(seatMapWidget);
    return seatGroup;
}

void MovieInfoDialog::onSessionSelected(int sessionId)
{
    // Находим выбранный сеанс
    for (const CinemaSession &session : sessions) {
        if (session.id == sessionId) {
            currentSession = session;
            break;
        }
    }

    // Очищаем выбор мест
    clearSeatSelection();

    // Обновляем карту мест
    for (SeatButton *seatBtn : seatButtons) {
        int seatNumber = seatBtn->getSeatNumber();
        bool isAvailable = seatNumber <= currentSession.availableSeats;
        seatBtn->setSeatState(isAvailable, selectedSeats.contains(seatNumber));
        seatBtn->setEnabled(isAvailable);
    }

    updateBookingInfo();
}

void MovieInfoDialog::onSeatClicked(int seatNumber)
{
    if (selectedSeats.contains(seatNumber)) {
        selectedSeats.removeAll(seatNumber);
    } else {
        // Проверяем, не превышает ли выбранное количество доступных мест
        if (selectedSeats.size() >= currentSession.availableSeats) {
            QMessageBox::warning(this, "Внимание",
                                 QString("Вы можете выбрать не более %1 мест")
                                     .arg(currentSession.availableSeats));
            return;
        }
        selectedSeats.append(seatNumber);
    }

    // Обновляем состояние кнопки
    for (SeatButton *seatBtn : seatButtons) {
        if (seatBtn->getSeatNumber() == seatNumber) {
            bool isAvailable = seatNumber <= currentSession.availableSeats;
            seatBtn->setSeatState(isAvailable, selectedSeats.contains(seatNumber));
            break;
        }
    }

    updateBookingInfo();
}

void MovieInfoDialog::clearSeatSelection()
{
    selectedSeats.clear();
    for (SeatButton *seatBtn : seatButtons) {
        int seatNumber = seatBtn->getSeatNumber();
        bool isAvailable = seatNumber <= currentSession.availableSeats;
        seatBtn->setSeatState(isAvailable, false);
    }
}

void MovieInfoDialog::updateBookingInfo()
{
    // Обновляем информацию о сеансе
    selectedSessionLabel->setText(
        QString("Сеанс: %1, %2, Зал: %3")
            .arg(currentSession.dateTime.toString("dd.MM.yyyy"))
            .arg(currentSession.dateTime.toString("hh:mm"))
            .arg(currentSession.hall)
        );

    // Обновляем информацию о местах
    if (selectedSeats.isEmpty()) {
        selectedSeatsLabel->setText("Выбранные места: нет");
    } else {
        QString seatsText;
        for (int seat : selectedSeats) {
            seatsText += QString::number(seat) + ", ";
        }
        seatsText.chop(2); // Убираем последнюю запятую и пробел
        selectedSeatsLabel->setText("Выбранные места: " + seatsText);
    }

    // Обновляем стоимость
    double totalPrice = selectedSeats.size() * currentSession.price;
    totalPriceLabel->setText(QString("Общая стоимость: %1 ₽").arg(totalPrice));

    // Активируем кнопку бронирования, если есть выбранные места
    bookButton->setEnabled(!selectedSeats.isEmpty() && !username.isEmpty());
}

void MovieInfoDialog::onBookClicked()
{
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Внимание",
                             "Для бронирования необходимо войти в систему.");
        return;
    }

    if (selectedSeats.isEmpty()) {
        QMessageBox::warning(this, "Внимание", "Выберите хотя бы одно место.");
        return;
    }

    // Проверяем баланс пользователя
    User* user = MovieDatabase::instance().findUser(username);
    if (!user) {
        QMessageBox::critical(this, "Ошибка", "Пользователь не найден.");
        return;
    }

    double totalPrice = selectedSeats.size() * currentSession.price;

    if (user->getBalance() < totalPrice) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Недостаточно средств");
        msgBox.setText(QString("Необходимо: %1 BYN\n"
                               "На вашем счету: %2 BYN")
                           .arg(QString::number(totalPrice, 'f', 2))
                           .arg(QString::number(user->getBalance(), 'f', 2)));
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
            "    padding: 15px;"
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

    // Подтверждение бронирования
    QMessageBox msgBox;
    msgBox.setWindowTitle("Подтверждение бронирования");
    msgBox.setText(QString("Вы уверены, что хотите забронировать?\n\n"
                           "🎬 Фильм: %1\n"
                           "📅 Дата: %2\n"
                           "🕒 Время: %3\n"
                           "📍 Зал: %4\n"
                           "🎭 Места: %5\n"
                           "💰 Стоимость: %6 BYN")
                       .arg(movie.title)
                       .arg(currentSession.dateTime.toString("dd.MM.yyyy"))
                       .arg(currentSession.dateTime.toString("hh:mm"))
                       .arg(currentSession.hall)
                       .arg(selectedSeats.size())
                       .arg(QString::number(totalPrice, 'f', 2)));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStyleSheet(
        "QMessageBox {"
        "    background: #1a1a2e;"
        "    border: 2px solid #2a2a4a;"
        "    border-radius: 15px;"
        "}"
        "QMessageBox QLabel {"
        "    color: white;"
        "    font-size: 14px;"
        "    padding: 15px;"
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

    int reply = msgBox.exec();

    if (reply == QMessageBox::Yes) {
        // Списываем деньги
        if (!user->deductBalance(totalPrice)) {
            QMessageBox::warning(this, "Ошибка", "Не удалось списать средства.");
            return;
        }

        // Бронируем места
        for (int seat : selectedSeats) {
            // Создаем уникальный ID билета: sessionId * 1000 + seat
            int ticketId = currentSession.id * 1000 + seat;
            user->addBookedTicket(ticketId);
        }

        // Обновляем доступные места в сеансе
        currentSession.availableSeats -= selectedSeats.size();
        MovieDatabase::instance().updateSession(currentSession);

        // Сохраняем изменения
        MovieDatabase::instance().saveBookedTickets(username);
        MovieDatabase::instance().saveUsers();

        // УВЕДОМЛЕНИЕ ОБ УСПЕХЕ (без лишних диалогов)
        QMessageBox successMsg;
        successMsg.setWindowTitle("✅ Успех");
        successMsg.setText(QString("Бронирование успешно завершено!\n\n"
                                   "🎬 Фильм: %1\n"
                                   "📅 Дата: %2\n"
                                   "🕒 Время: %3\n"
                                   "📍 Зал: %4\n"
                                   "🎭 Места: %5\n"
                                   "💰 Стоимость: %6 BYN\n"
                                   "💳 Остаток на счету: %7 BYN")
                               .arg(movie.title)
                               .arg(currentSession.dateTime.toString("dd.MM.yyyy"))
                               .arg(currentSession.dateTime.toString("hh:mm"))
                               .arg(currentSession.hall)
                               .arg(selectedSeats.size())
                               .arg(QString::number(totalPrice, 'f', 2))
                               .arg(QString::number(user->getBalance(), 'f', 2)));
        successMsg.setIcon(QMessageBox::Information);
        successMsg.setStandardButtons(QMessageBox::Ok);
        successMsg.setStyleSheet(
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

        successMsg.exec();

        // Закрываем диалог бронирования (без дополнительных окон)
        accept();
    }
}
