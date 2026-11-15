
#include "bookingdialog.h"
#include <QScrollArea>
#include "moviedatabase.h"
#include <QGroupBox>

BookingDialog::BookingDialog(int sessionId, const QString& username, QWidget *parent)
    : QDialog(parent), sessionId(sessionId), username(username),
    seatContainer(nullptr), refreshBtn(nullptr)
{
    qDebug() << "\n=== ОТКРЫТИЕ DIALOG БРОНИРОВАНИЯ ===";

    // ШАГ 1: Принудительно загружаем ВСЕ данные
    MovieDatabase::instance().loadBookedSeats(); // Загружаем из файла
    MovieDatabase::instance().updateSessionAvailableSeats(); // Обновляем счетчики

    qDebug() << "Данные загружены, проверяем места...";

    // Проверяем, что данные загрузились
    auto bookedSeats = MovieDatabase::instance().getAllBookedSeats();
    qDebug() << "Всего забронированных мест в системе:" << bookedSeats.size();

    for (const auto& seat : bookedSeats) {
        if (seat.sessionId == sessionId) {
            qDebug() << "  -> Сеанс" << sessionId << "место" << seat.seatNumber
                     << "забронировано пользователем" << seat.username;
        }
    }

    // ШАГ 2: Получаем сеанс и фильм
    session = MovieDatabase::instance().getSessionById(sessionId);
    movie = MovieDatabase::instance().getMovieById(session.movieId);

    qDebug() << "Сеанс ID:" << sessionId;
    qDebug() << "Фильм:" << movie.title;
    qDebug() << "Доступно мест в сеансе:" << session.availableSeats;

    // Проверяем конкретные места
    for (int i = 1; i <= 10; i++) { // Первые 10 мест для проверки
        bool isBooked = MovieDatabase::instance().isSeatBooked(sessionId, i);
        qDebug() << "Место" << i << "- забронировано:" << isBooked;
    }

    setWindowTitle("Бронирование билетов - " + movie.title);
    setFixedSize(600, 700);

    setStyleSheet(
        "QDialog {"
        "    background: #1a1a2e;"
        "    border-radius: 15px;"
        "}"
        "QLabel {"
        "    color: white;"
        "    font-size: 14px;"
        "}"
        "QLabel#title {"
        "    font-size: 20px;"
        "    font-weight: bold;"
        "    color: #00adb5;"
        "}"
        "QLabel#info {"
        "    color: #eeeeee;"
        "    font-size: 13px;"
        "}"
        "QPushButton {"
        "    border-radius: 8px;"
        "    padding: 8px 15px;"
        "    font-size: 13px;"
        "    font-weight: bold;"
        "}"
        "QPushButton#bookBtn {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #00adb5, stop:1 #009ca3);"
        "    color: white;"
        "    border: none;"
        "    padding: 12px 25px;"
        "    font-size: 14px;"
        "}"
        "QPushButton#bookBtn:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #009ca3, stop:1 #008b92);"
        "}"
        "QPushButton#bookBtn:disabled {"
        "    background: #555555;"
        "    color: #888888;"
        "}"
        );

    setupUI();
}

void BookingDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(20);

    // Информация о фильме
    QLabel *titleLabel = new QLabel(movie.title);
    titleLabel->setObjectName("title");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Подсчитываем занятые места
    int bookedCount = 0;
    for (int i = 1; i <= 80; i++) {
        if (MovieDatabase::instance().isSeatBooked(sessionId, i)) {
            bookedCount++;
        }
    }
    int freeSeats = 80 - bookedCount;

    QLabel *infoLabel = new QLabel(
        QString("🎬 %1\n"
                "⏰ %2\n"
                "📍 %3\n"
                "💵 %4 BYN\n"
                "💺 Свободно: %5 мест | 🔴 Занято: %6 мест")
            .arg(session.dateTime.toString("dd.MM.yyyy hh:mm"))
            .arg(movie.duration)
            .arg(session.hall)
            .arg(QString::number(session.price, 'f', 2))
            .arg(freeSeats)
            .arg(bookedCount)
        );
    infoLabel->setObjectName("info");
    mainLayout->addWidget(infoLabel);

    // Легенда
    QLabel *legend = new QLabel("🔴 Занято | 🔵 Свободно (номер места) | 🔷 Вы выбрали");
    legend->setStyleSheet("color: #cccccc; font-size: 12px; text-align: center;");
    mainLayout->addWidget(legend);

    // Создаем ScrollArea
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");

    // Контейнер для схемы мест
    seatContainer = new QWidget();
    seatContainer->setObjectName("seatContainer");
    QVBoxLayout *containerLayout = new QVBoxLayout(seatContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);

    // Сразу создаем схему мест
    createSeatLayout();

    scrollArea->setWidget(seatContainer);
    mainLayout->addWidget(scrollArea);

    // Общая сумма
    totalLabel = new QLabel("Выберите свободные места (синие)");
    totalLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #00adb5;");
    mainLayout->addWidget(totalLabel);

    // Кнопка бронирования
    bookButton = new QPushButton("Забронировать");
    bookButton->setObjectName("bookBtn");
    bookButton->setEnabled(false);
    connect(bookButton, &QPushButton::clicked, this, &BookingDialog::onBookClicked);
    mainLayout->addWidget(bookButton);

    // Кнопки управления
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *updateBtn = new QPushButton("🔄 Обновить");
    updateBtn->setStyleSheet(
        "QPushButton {"
        "    background: rgba(59, 130, 246, 0.2);"
        "    color: #3b82f6;"
        "    border: 2px solid rgba(59, 130, 246, 0.4);"
        "    border-radius: 8px;"
        "    padding: 8px 15px;"
        "    font-size: 12px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: rgba(59, 130, 246, 0.3);"
        "    border-color: #3b82f6;"
        "}"
        );
    connect(updateBtn, &QPushButton::clicked, this, [this]() {
        MovieDatabase::instance().loadBookedSeats();
        createSeatLayout();
        QMessageBox::information(this, "Обновлено", "Схема мест обновлена!");
    });

    QPushButton *closeBtn = new QPushButton("Закрыть");
    closeBtn->setStyleSheet(
        "QPushButton {"
        "    background: rgba(255, 77, 141, 0.2);"
        "    color: #ff4d8d;"
        "    border: 2px solid rgba(255, 77, 141, 0.4);"
        "    border-radius: 8px;"
        "    padding: 8px 15px;"
        "    font-size: 12px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: rgba(255, 77, 141, 0.3);"
        "    border-color: #ff4d8d;"
        "}"
        );
    connect(closeBtn, &QPushButton::clicked, this, &BookingDialog::accept);

    buttonLayout->addStretch();
    buttonLayout->addWidget(updateBtn);
    buttonLayout->addWidget(closeBtn);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
}

void BookingDialog::checkAndUpdateSeats()
{
    qDebug() << "\n=== ПРОВЕРКА И ОБНОВЛЕНИЕ МЕСТ ===";

    // Перезагружаем данные
    MovieDatabase::instance().loadBookedSeats();

    // Создаем временную карту для проверки
    QMap<int, bool> seatStatus;
    for (int i = 1; i <= 80; i++) {
        seatStatus[i] = MovieDatabase::instance().isSeatBooked(sessionId, i);
    }

    qDebug() << "Статус мест (первые 20):";
    for (int i = 1; i <= 20; i++) {
        qDebug() << "  Место" << i << ":" << (seatStatus[i] ? "ЗАНЯТО" : "СВОБОДНО");
    }

    // Обновляем отображение
    createSeatLayout();
}


void BookingDialog::createSeatLayout()
{
    qDebug() << "\n=== СОЗДАНИЕ СХЕМЫ МЕСТ ===";

    // Очищаем старую схему
    if (seatContainer && seatContainer->layout()) {
        QLayoutItem* child;
        while ((child = seatContainer->layout()->takeAt(0)) != nullptr) {
            if (child->widget()) {
                child->widget()->deleteLater();
            }
            delete child;
        }
    }

    QWidget *seatWidget = new QWidget();
    seatWidget->setObjectName("seatWidget"); // Устанавливаем имя для поиска
    QGridLayout *seatLayout = new QGridLayout(seatWidget);
    seatLayout->setSpacing(6);
    seatLayout->setContentsMargins(15, 15, 15, 25);

    // Экран
    QLabel *screenLabel = new QLabel("────────── 🎬 ЭКРАН 🎬 ──────────");
    screenLabel->setAlignment(Qt::AlignCenter);
    screenLabel->setStyleSheet(
        "QLabel {"
        "    background: #222831;"
        "    color: #ffd700;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    padding: 15px;"
        "    border-radius: 10px;"
        "    margin-bottom: 20px;"
        "    border: 2px solid #ffd700;"
        "}"
        );
    seatLayout->addWidget(screenLabel, 0, 0, 1, 12);

    // Места
    const int TOTAL_ROWS = 8;
    const int SEATS_PER_ROW = 10;
    const int START_ROW = 2;

    for (int row = 0; row < TOTAL_ROWS; row++) {
        // Номер ряда слева
        QLabel *rowLabel = new QLabel(QString::number(row + 1));
        rowLabel->setAlignment(Qt::AlignCenter);
        rowLabel->setStyleSheet("color: #00adb5; font-weight: bold; font-size: 16px;");
        seatLayout->addWidget(rowLabel, START_ROW + row, 0);

        for (int col = 0; col < SEATS_PER_ROW; col++) {
            int seatNumber = row * SEATS_PER_ROW + col + 1;

            QPushButton *seatBtn = new QPushButton();
            seatBtn->setFixedSize(36, 36);
            seatBtn->setProperty("seatNumber", seatNumber); // Сохраняем номер места

            // ПРОВЕРЯЕМ базу данных - занято ли место
            bool isBooked = MovieDatabase::instance().isSeatBooked(sessionId, seatNumber);

            if (isBooked) {
                // ЗАНЯТО - красный БЕЗ НОМЕРА, НЕ кликабельно
                seatBtn->setEnabled(false);
                seatBtn->setText(""); // НЕ показываем номер
                seatBtn->setStyleSheet(
                    "QPushButton {"
                    "    background: #ff4444;"
                    "    color: white;"
                    "    border-radius: 6px;"
                    "    border: 2px solid #cc0000;"
                    "}"
                    );
                qDebug() << "Место" << seatNumber << "-> КРАСНЫЙ (занято, не кликабельно)";
            } else {
                // СВОБОДНО - показываем номер
                seatBtn->setText(QString::number(seatNumber));
                seatBtn->setCheckable(true);
                seatBtn->setCursor(Qt::PointingHandCursor);

                bool isSelected = selectedSeats.contains(seatNumber);

                if (isSelected) {
                    // ВЫБРАНО - голубой
                    seatBtn->setChecked(true);
                    seatBtn->setStyleSheet(
                        "QPushButton {"
                        "    background: #00adb5;"
                        "    color: white;"
                        "    border: 2px solid #009ca3;"
                        "    border-radius: 6px;"
                        "    font-weight: bold;"
                        "    font-size: 12px;"
                        "}"
                        );
                    qDebug() << "Место" << seatNumber << "-> ГОЛУБОЙ (выбрано)";
                } else {
                    // СВОБОДНО - синий
                    seatBtn->setChecked(false);
                    seatBtn->setStyleSheet(
                        "QPushButton {"
                        "    background: #393e46;"
                        "    color: white;"
                        "    border: 2px solid #555555;"
                        "    border-radius: 6px;"
                        "    font-weight: bold;"
                        "    font-size: 12px;"
                        "}"
                        "QPushButton:hover {"
                        "    background: #4a4f57;"
                        "    border-color: #666666;"
                        "}"
                        "QPushButton:checked {"
                        "    background: #00adb5;"
                        "    color: white;"
                        "    border-color: #009ca3;"
                        "}"
                        );
                    qDebug() << "Место" << seatNumber << "-> СИНИЙ (свободно)";
                }

                // Подключаем обработчик клика через лямбда-функцию
                connect(seatBtn, &QPushButton::clicked, this, [this, seatNumber, seatBtn]() {
                    qDebug() << "Клик по кнопке места:" << seatNumber;

                    // Проверяем, не занято ли место (двойная проверка)
                    if (MovieDatabase::instance().isSeatBooked(sessionId, seatNumber)) {
                        qDebug() << "Место занято, игнорируем клик";
                        seatBtn->setChecked(false);
                        return;
                    }

                    // Обрабатываем выбор места
                    onSeatClicked(seatNumber);
                });
            }

            seatLayout->addWidget(seatBtn, START_ROW + row, col + 1);
        }

        // Номер ряда справа
        QLabel *rowLabelRight = new QLabel(QString::number(row + 1));
        rowLabelRight->setAlignment(Qt::AlignCenter);
        rowLabelRight->setStyleSheet("color: #00adb5; font-weight: bold; font-size: 16px;");
        seatLayout->addWidget(rowLabelRight, START_ROW + row, SEATS_PER_ROW + 1);
    }

    if (seatContainer) {
        seatContainer->layout()->addWidget(seatWidget);
    }
}
void BookingDialog::onSeatClicked(int seatNumber)
{
    qDebug() << "Клик по месту:" << seatNumber;

    // ПРОВЕРЯЕМ, не занято ли место в БАЗЕ ДАННЫХ
    if (MovieDatabase::instance().isSeatBooked(sessionId, seatNumber)) {
        qDebug() << "Место" << seatNumber << "уже занято!";

        // Показываем предупреждение
        QMessageBox::warning(this, "Место занято",
                             QString("Место %1 уже забронировано другим пользователем.\n"
                                     "Выберите другое место.").arg(seatNumber));

        // НЕ добавляем в selectedSeats
        return;
    }

    // Только СВОБОДНЫЕ места можно добавлять/убирать
    if (selectedSeats.contains(seatNumber)) {
        qDebug() << "Убираем место" << seatNumber << "из выбранных";
        selectedSeats.removeAll(seatNumber);
    } else {
        qDebug() << "Добавляем место" << seatNumber << "в выбранные";
        selectedSeats.append(seatNumber);
    }

    // Пересчитываем стоимость
    double total = selectedSeats.size() * session.price;
    totalLabel->setText(QString("Выбрано мест: %1\nОбщая сумма: %2 BYN")
                            .arg(selectedSeats.size())
                            .arg(QString::number(total, 'f', 2)));

    bookButton->setEnabled(!selectedSeats.isEmpty());

    // Сразу обновляем отображение кнопки места
    updateSeatButton(seatNumber);
}
void BookingDialog::updateSeatButton(int seatNumber)
{
    // Находим кнопку места и обновляем её состояние
    if (!seatContainer) return;

    QWidget *seatWidget = seatContainer->findChild<QWidget*>("seatWidget");
    if (!seatWidget) return;

    // Ищем все кнопки в виджете
    QList<QPushButton*> buttons = seatWidget->findChildren<QPushButton*>();
    for (QPushButton *btn : buttons) {
        // Проверяем, содержит ли кнопка свойство с номером места
        if (btn->property("seatNumber").isValid()) {
            int btnSeatNumber = btn->property("seatNumber").toInt();
            if (btnSeatNumber == seatNumber) {
                // Обновляем стиль кнопки
                bool isBooked = MovieDatabase::instance().isSeatBooked(sessionId, seatNumber);
                bool isSelected = selectedSeats.contains(seatNumber);

                if (isBooked) {
                    // ЗАНЯТО - красный БЕЗ НОМЕРА
                    btn->setEnabled(false);
                    btn->setText("");
                    btn->setStyleSheet(
                        "QPushButton {"
                        "    background: #ff4444;"
                        "    color: white;"
                        "    border-radius: 6px;"
                        "    border: 2px solid #cc0000;"
                        "}"
                        );
                } else if (isSelected) {
                    // ВЫБРАНО - голубой
                    btn->setChecked(true);
                    btn->setText(QString::number(seatNumber));
                    btn->setStyleSheet(
                        "QPushButton {"
                        "    background: #00adb5;"
                        "    color: white;"
                        "    border: 2px solid #009ca3;"
                        "    border-radius: 6px;"
                        "    font-weight: bold;"
                        "    font-size: 12px;"
                        "}"
                        );
                } else {
                    // СВОБОДНО - синий
                    btn->setChecked(false);
                    btn->setText(QString::number(seatNumber));
                    btn->setStyleSheet(
                        "QPushButton {"
                        "    background: #393e46;"
                        "    color: white;"
                        "    border: 2px solid #555555;"
                        "    border-radius: 6px;"
                        "    font-weight: bold;"
                        "    font-size: 12px;"
                        "}"
                        "QPushButton:hover {"
                        "    background: #4a4f57;"
                        "    border-color: #666666;"
                        "}"
                        "QPushButton:checked {"
                        "    background: #00adb5;"
                        "    color: white;"
                        "    border-color: #009ca3;"
                        "}"
                        );
                }
                break;
            }
        }
    }
}

void BookingDialog::refreshSeatMap()
{
    qDebug() << "=== ОБНОВЛЕНИЕ СХЕМЫ МЕСТ ===";

    // Перезагружаем данные
    MovieDatabase::instance().reloadBookedSeats();

    // Обновляем сеанс
    session = MovieDatabase::instance().getSessionById(sessionId);

    // Пересоздаем схему мест
    createSeatLayout();

    QMessageBox::information(this, "Обновлено", "Схема мест обновлена!");
}

void BookingDialog::onBookClicked()
{
    if (selectedSeats.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите хотя бы одно место");
        return;
    }

    User* user = MovieDatabase::instance().findUser(username);
    if (!user) {
        QMessageBox::critical(this, "Ошибка", "Пользователь не найден");
        return;
    }

    // ФИЛЬТРУЕМ места: убираем уже забронированные
    QList<int> seatsToBook;
    for (int seat : selectedSeats) {
        if (!MovieDatabase::instance().isSeatBooked(sessionId, seat)) {
            seatsToBook.append(seat);
        }
    }

    // Если все выбранные места уже заняты
    if (seatsToBook.isEmpty()) {
        QMessageBox::warning(this, "Внимание",
                             "Все выбранные места уже заняты другими пользователями.\n"
                             "Пожалуйста, выберите другие места.");

        // Обновляем схему мест
        createSeatLayout();
        return;
    }

    // Если некоторые места уже заняты
    if (seatsToBook.size() < selectedSeats.size()) {
        int occupiedCount = selectedSeats.size() - seatsToBook.size();
        QMessageBox::information(this, "Информация",
                                 QString("%1 из %2 выбранных мест уже заняты.\n"
                                         "Будут забронированы только свободные места.")
                                     .arg(occupiedCount)
                                     .arg(selectedSeats.size()));
    }

    double total = seatsToBook.size() * session.price;
    if (!user->deductBalance(total)) {
        QMessageBox::warning(this, "Недостаточно средств",
                             QString("Необходимо: %1 BYN\n"
                                     "На счету: %2 BYN")
                                 .arg(QString::number(total, 'f', 2))
                                 .arg(QString::number(user->getBalance(), 'f', 2)));
        return;
    }

    // Бронируем только свободные места
    for (int seat : seatsToBook) {
        // Добавляем билет пользователю (уникальный ID)
        int ticketId = sessionId * 1000 + seat;
        user->addBookedTicket(ticketId);

        // Сохраняем бронирование места в общей базе
        MovieDatabase::instance().bookSeat(sessionId, seat, username);

        qDebug() << "Забронировано место" << seat << "для пользователя" << username;
    }

    // Сохраняем все изменения
    MovieDatabase::instance().saveBookedSeats();
    MovieDatabase::instance().saveBookedTickets(username);
    MovieDatabase::instance().saveUsers();

    // Обновляем данные и перерисовываем схему
    MovieDatabase::instance().loadBookedSeats(); // Перезагружаем
    session = MovieDatabase::instance().getSessionById(sessionId); // Обновляем сеанс

    // Очищаем выбранные места
    selectedSeats.clear();

    // Обновляем UI
    createSeatLayout();
    totalLabel->setText("Выберите места");
    bookButton->setEnabled(false);

    // Успешное сообщение
    QMessageBox successMsg;
    successMsg.setWindowTitle("✅ Бронирование завершено");
    successMsg.setText(QString("<div style='text-align: center;'>"
                               "<h3 style='color: #8b5cf6;'>🎉 БРОНИРОВАНИЕ УСПЕШНО!</h3>"
                               "<p style='color: white; font-size: 14px;'>"
                               "🎬 <b>Фильм:</b> %1<br>"
                               "📅 <b>Дата:</b> %2<br>"
                               "🎭 <b>Зал:</b> %3<br>"
                               "💺 <b>Забронировано мест:</b> %4<br>"
                               "💰 <b>Сумма:</b> %5 BYN<br>"
                               "💳 <b>Остаток на счету:</b> %6 BYN"
                               "</p></div>")
                           .arg(movie.title)
                           .arg(session.dateTime.toString("dd.MM.yyyy hh:mm"))
                           .arg(session.hall)
                           .arg(seatsToBook.size())
                           .arg(QString::number(total, 'f', 2))
                           .arg(QString::number(user->getBalance(), 'f', 2)));
    successMsg.setTextFormat(Qt::RichText);
    successMsg.setStyleSheet(
        "QMessageBox {"
        "    background: #1a1a2e;"
        "    border: 2px solid #00ff00;"
        "    border-radius: 15px;"
        "}"
        "QMessageBox QLabel {"
        "    color: white;"
        "    font-size: 14px;"
        "    padding: 10px;"
        "}"
        "QMessageBox QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #8b5cf6, stop:1 #3b82f6);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 12px 30px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QMessageBox QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #7c3aed, stop:1 #2563eb);"
        "}"
        );
    successMsg.exec();

    emit bookingCompleted();
    // Диалог остается открытым для дальнейшего бронирования
}
