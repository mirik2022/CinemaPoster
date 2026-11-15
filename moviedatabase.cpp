#include "moviedatabase.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QDateTime>  // ДОБАВИТЬ

MovieDatabase& MovieDatabase::instance()
{
    static MovieDatabase instance;
    return instance;
}

MovieDatabase::MovieDatabase()
{
    loadUsers();
    loadMovies();
    loadSessions();
    loadBookedSeats();
    //updateSessionAvailableSeats();
}

int MovieDatabase::getBookedSeatsCount(int sessionId) const
{
    int count = 0;
    for (const BookedSeat &seat : bookedSeats) {
        if (seat.sessionId == sessionId) {
            count++;
        }
    }
    return count;
}

void MovieDatabase::loadMovies()
{
    movies.clear();

    // Существующие фильмы
    movies.append({
        1,
        "Интерстеллар",
        "Фантастика, Драма",
        "Кристофер Нолан",
        169,
        "Когда засуха, пыльные бури и вымирание растений приводят человечество к продовольственному кризису, коллектив исследователей и учёных отправляется сквозь червоточину в путешествие, чтобы превзойти прежние ограничения для космических путешествий человека и найти планеты с подходящими для человечества условиями.",
        ":/images/interstellar.jpg",
        8.6,
        "12+"
    });

    movies.append({
        2,
        "Начало",
        "Фантастика, Боевик",
        "Кристофер Нолан",
        148,
        "Кобб — талантливый вор, лучший из лучших в опасном искусстве извлечения: он крадет ценные секреты из глубин подсознания во время сна, когда человеческий разум наиболее уязвим. Ему предстоит выполнить последнее задание, которое может вернуть ему всё, но для этого нужно совершить невозможное — не украсть идею, а внедрить её.",
        ":/images/inception.jpg",
        8.7,
        "12+"
    });

    movies.append({
        3,
        "Матрица",
        "Фантастика, Боевик",
        "Лана и Лилли Вачовски",
        136,
        "Жизнь Томаса Андерсона разделена на две части: днём он — программист, а ночью — хакер по имени Нео. Он всегда задавался вопросом, что же такое Матрица. Однажды он получает ответ от самого загадочного хакера — Морфеуса. «Матрица» — это мир, который существует лишь в воображении, мир-иллюзия.",
        ":/images/matrix.jpg",
        8.7,
        "16+"
    });

    movies.append({
        4,
        "Джокер",
        "Драма, Криминал",
        "Тодд Филлипс",
        122,
        "Готэм, начало 1980-х годов. Комик Артур Флек живет с больной матерью, которая с детства учит его «ходить с улыбкой». Пытаясь нести в мир хорошее и дарить людям радость, Артур сталкивается с человеческой жестокостью и постепенно приходит к выводу, что этот мир получит от него не добрую улыбку, а ухмылку злодея Джокера.",
        ":/images/joker.jpg",
        8.4,
        "18+"
    });

    movies.append({
        5,
        "Аватар",
        "Фантастика, Приключения",
        "Джеймс Кэмерон",
        162,
        "Бывший морпех Джейк Салли прикован к инвалидному креслу. Несмотря на немощное тело, Джейк в душе по-прежнему остается воином. Он получает задание совершить путешествие в несколько световых лет к базе землян на планете Пандора, где корпорации добывают редкий минерал.",
        ":/images/avatar.jpg",
        7.9,
        "12+"
    });

    movies.append({
        6,
        "Оппенгеймер",
        "Биография, Драма",
        "Кристофер Нолан",
        180,
        "История жизни американского физика-теоретика Роберта Оппенгеймера, который в годы Второй мировой войны руководил Манхэттенским проектом — программой по созданию первой атомной бомбы.",
        ":/images/oppenheimer.jpg",
        8.3,
        "16+"
    });

    movies.append({
        7,
        "Барби",
        "Комедия, Приключения",
        "Грета Гервиг",
        114,
        "Кукла Барби из кукольного мира отправляется в реальный мир, чтобы найти истинное счастье.",
        ":/images/barbie.jpg",
        7.0,
        "12+"
    });

    movies.append({
        8,
        "Довод",
        "Фантастика, Боевик",
        "Кристофер Нолан",
        150,
        "Секретный агент получает задание предотвратить начало Третьей мировой войны. Для этого ему придётся использовать технологию «довод», позволяющую управлять течением времени.",
        ":/images/tenet.jpg",
        7.3,
        "12+"
    });

    // НОВЫЕ ФИЛЬМЫ
    movies.append({
        9,
        "Клаутрофобы",
        "Ужасы, Триллер",
        "Алексей Нужный",
        90,
        "Группа людей оказывается запертой в подземном бункере. Постепенно они понимают, что это не случайность, а часть чьего-то жуткого плана. Герои должны не только выбраться на свободу, но и сохранить рассудок в условиях полной изоляции.",
        ":/images/claustr.jpg",
        6.2,
        "18+"
    });

    movies.append({
        10,
        "Дивергент",
        "Фантастика, Боевик",
        "Нил Бергер",
        139,
        "В постапокалиптическом Чикаго общество разделено на пять фракций. Шестнадцатилетняя Беатрис Прайор оказывается Дивергентом — человеком, который не вписывается в рамки ни одной из фракций. Ей предстоит скрывать свою природу, чтобы выжить.",
        ":/images/divergent.jpg",
        6.6,
        "12+"
    });

    movies.append({
        11,
        "Дюна",
        "Фантастика, Драма",
        "Дени Вильнёв",
        155,
        "Путешествие Пола Атрейдеса, блестящего и одарённого молодого человека, рождённого в великой судьбе за пределами его понимания, который должен отправиться на самую опасную планету во вселенной, чтобы обеспечить будущее своей семьи и своего народа.",
        ":/images/duna.jpg",
        8.0,
        "12+"
    });

    movies.append({
        12,
        "Эйфория",
        "Драма",
        "Лиз Гарбас",
        98,
        "История подростков, которые пытаются найти своё место в мире, сталкиваясь с проблемами взросления, любви, дружбы и поиска смысла жизни.",
        ":/images/euphoria.jpg",
        7.8,
        "18+"
    });

    movies.append({
        13,
        "Гемини",
        "Боевик, Триллер",
        "Энг Ли",
        117,
        "Специальный агент получает задание найти и обезвредить опасного террориста, но вскоре понимает, что его противник — его собственный клон, созданный в секретной лаборатории.",
        ":/images/gem.jpg",
        6.8,
        "16+"
    });

    movies.append({
        14,
        "Храбрая сердцем",
        "Мультфильм, Приключения",
        "Марк Эндрюс, Бренда Чепмен",
        93,
        "Мерида, дочь короля Шотландии, отказывается следовать традициям и выходить замуж. Чтобы изменить свою судьбу, она обращается за помощью к ведьме, но её желание оборачивается неожиданными последствиями для всего королевства.",
        ":/images/hrabr.jpg",
        7.1,
        "6+"
    });

    movies.append({
        15,
        "Иллюзия обмана 3",
        "Триллер, Детектив",
        "Джон М. Чу",
        100,
        "Четыре мага-иллюзиониста, известные как «Всадники», возвращаются для своего самого опасного трюка, чтобы разоблачить технологического гения, который угрожает мировой безопасности.",
        ":/images/illusion.jpg",
        6.5,
        "12+"
    });

    movies.append({
        16,
        "Веном",
        "Фантастика, Боевик",
        "Рубен Фляйшер",
        112,
        "Журналист Эдди Брок получает сверхспособности после того, как его тело сливается с инопланетным симбиотом. Теперь он должен использовать свои новые силы, чтобы бороться с тёмными силами, угрожающими человечеству.",
        ":/images/venom.jpg",
        6.6,
        "16+"
    });

    movies.append({
        17,
        "Анчартед: На картах не значится",
        "Приключения, Боевик",
        "Рубен Фляйшер",
        116,
        "Молодой искатель приключений Нейтан Дрейк и его наставник Виктор Салли отправляются на поиски величайшего сокровища, которое когда-либо находилось, и одновременно пытаются найти пропавшего брата Дрейка.",
        ":/images/uncharted.jpg",
        6.3,
        "12+"
    });

    movies.append({
        18,
        "Мстители: Финал",
        "Фантастика, Боевик",
        "Энтони и Джо Руссо",
        181,
        "Оставшиеся в живых члены команды Мстителей и их союзники должны исправить разрушения, вызванные Таносом, и восстановить порядок во вселенной.",
        ":/images/mstit.jpg",
        8.4,
        "12+"
    });

    movies.append({
        19,
        "Однажды в Голливуде",
        "Драма, Комедия",
        "Квентин Тарантино",
        161,
        "История актёра Рика Далтона и его дублёра Клиффа Бута, которые пытаются найти своё место в стремительно меняющемся кинематографическом ландшафте Лос-Анджелеса 1969 года.",
        ":/images/odna.jpg",
        7.6,
        "18+"
    });
}

void MovieDatabase::loadSessions()
{
    // Сначала загружаем сеансы без учета бронирований
    QList<CinemaSession> newSessions;
    QDateTime now = QDateTime::currentDateTime();

    // Создаем сеансы для всех фильмов
    for (int movieId = 1; movieId <= 19; movieId++) {
        for (int day = 0; day < 3; day++) {
            for (int timeSlot = 0; timeSlot < 3; timeSlot++) {
                int sessionId = (movieId - 1) * 9 + day * 3 + timeSlot + 1;

                CinemaSession session;
                session.id = sessionId;
                session.movieId = movieId;
                session.dateTime = now.addDays(day).addSecs((10 + timeSlot * 4) * 3600); // 10:00, 14:00, 18:00
                session.hall = QString("Зал %1").arg((movieId % 4) + 1);
                session.price = 8.0 + (movieId % 5) * 2.0;  // Цены от 8 до 16 BYN
                session.totalSeats = 80;  // Всего мест (8 рядов * 10 мест)
                session.availableSeats = session.totalSeats;  // Временно все места доступны

                newSessions.append(session);
            }
        }
    }

    // Теперь применяем уже загруженные бронирования
    sessions = newSessions;

    // Если у нас уже загружены бронирования, обновляем доступные места
    if (!bookedSeats.isEmpty()) {
        updateSessionAvailableSeats();
    }
}

bool MovieDatabase::loadUsers()
{
    users.clear();
    QFile file("users.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Создаем администратора по умолчанию с балансом 10 BYN
        User admin("admin", "admin123", "admin@cinema.com", 10.0);
        users.append(admin);

        User user("user", "user123", "user@cinema.com", 10.0);
        users.append(user);
        saveUsers();
        return true;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split('|');
        if (parts.size() >= 4) {
            // Создаем пользователя с балансом из файла
            QString username = parts[0];
            QString password = parts[1];
            QString email = parts[2];
            double balance = 0.0;

            if (parts.size() > 3 && !parts[3].isEmpty()) {
                balance = parts[3].toDouble();
            } else {
                balance = 10.0; // По умолчанию 10 BYN для старых записей
            }

            User user(username, password, email, balance);

            // Загружаем код пополнения если есть
            if (parts.size() > 4 && !parts[4].isEmpty()) {
                // Здесь нужно добавить метод для установки кода
            }

            users.append(user);
        }
    }
    file.close();
    return true;
}

bool MovieDatabase::saveUsers()
{
    QFile file("users.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл users.txt для записи";
        return false;
    }

    QTextStream out(&file);
    for (User& user : users) {
        out << user.getUsername() << "|"
            << user.getPassword() << "|"
            << user.getEmail() << "|"
            << user.getBalance() << "|"
            << user.getRechargeCode() << "\n";
    }

    file.close();
    return true;
}

void MovieDatabase::updateSessionAvailableSeats()
{
    qDebug() << "\n=== ОБНОВЛЕНИЕ ДОСТУПНЫХ МЕСТ ===";
    qDebug() << "Всего сеансов:" << sessions.size();
    qDebug() << "Всего забронированных мест в БД:" << bookedSeats.size();

    // Создаем карту для подсчета
    QMap<int, int> bookedCountBySession;
    for (const BookedSeat &seat : bookedSeats) {
        bookedCountBySession[seat.sessionId]++;
        qDebug() << "  - Сеанс" << seat.sessionId
                 << "место" << seat.seatNumber
                 << "пользователь" << seat.username;
    }

    // Обновляем каждый сеанс
    for (CinemaSession &session : sessions) {
        int bookedCount = bookedCountBySession.value(session.id, 0);
        session.availableSeats = session.totalSeats - bookedCount;

        qDebug() << "Сеанс ID:" << session.id
                 << "| Фильм:" << getMovieById(session.movieId).title.left(20)
                 << "| Всего мест:" << session.totalSeats
                 << "| Забронировано:" << bookedCount
                 << "| Доступно:" << session.availableSeats;
    }
    qDebug() << "=== КОНЕЦ ОБНОВЛЕНИЯ ===";
}
void MovieDatabase::loadBookedSeats()
{
    bookedSeats.clear();
    QFile file("booked_seats.txt");

    qDebug() << "Загрузка забронированных мест...";
    qDebug() << "Файл booked_seats.txt существует:" << file.exists();

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл booked_seats.txt";
        // Создаем пустой файл
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        file.close();
        return;
    }

    QTextStream in(&file);
    int loadedCount = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split('|');
        if (parts.size() >= 3) {
            BookedSeat seat;
            seat.sessionId = parts[0].toInt();
            seat.seatNumber = parts[1].toInt();
            seat.username = parts[2];
            if (parts.size() > 3) {
                seat.bookingTime = QDateTime::fromString(parts[3], Qt::ISODate);
            } else {
                seat.bookingTime = QDateTime::currentDateTime();
            }
            bookedSeats.append(seat);
            loadedCount++;

            qDebug() << "Загружено место: сеанс" << seat.sessionId
                     << "место" << seat.seatNumber
                     << "пользователь" << seat.username;
        }
    }
    file.close();

    qDebug() << "Всего загружено забронированных мест:" << loadedCount;
}

void MovieDatabase::saveBookedSeats()
{
    qDebug() << "\n=== СОХРАНЕНИЕ ЗАБРОНИРОВАННЫХ МЕСТ ===";
    qDebug() << "Всего мест для сохранения:" << bookedSeats.size();

    QFile file("booked_seats.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "ОШИБКА: Не удалось открыть файл booked_seats.txt для записи";
        return;
    }

    QTextStream out(&file);
    int savedCount = 0;
    for (const BookedSeat &seat : bookedSeats) {
        out << seat.sessionId << "|"
            << seat.seatNumber << "|"
            << seat.username << "|"
            << seat.bookingTime.toString(Qt::ISODate) << "\n";
        savedCount++;
        qDebug() << "  Сохранено: сеанс" << seat.sessionId
                 << "место" << seat.seatNumber
                 << "пользователь" << seat.username;
    }
    file.close();

    qDebug() << "Сохранено мест:" << savedCount;
}

void MovieDatabase::reloadBookedSeats()
{
    qDebug() << "\n=== ПЕРЕЗАГРУЗКА ДАННЫХ О МЕСТАХ ===";
    loadBookedSeats();
    updateSessionAvailableSeats();
}

bool MovieDatabase::isSeatBooked(int sessionId, int seatNumber) const
{
    qDebug() << "[isSeatBooked] Проверка: сеанс" << sessionId << "место" << seatNumber;

    for (const BookedSeat &seat : bookedSeats) {
        if (seat.sessionId == sessionId && seat.seatNumber == seatNumber) {
            qDebug() << "  -> ЗАБРОНИРОВАНО! Пользователь:" << seat.username;
            return true;
        }
    }

    qDebug() << "  -> СВОБОДНО";
    return false;
}

int MovieDatabase::getBookedSeatsCountForSession(int sessionId) const
{
    int count = 0;
    for (const BookedSeat &seat : bookedSeats) {
        if (seat.sessionId == sessionId) {
            count++;
        }
    }
    return count;
}

void MovieDatabase::bookSeat(int sessionId, int seatNumber, const QString& username)
{
    // Двойная проверка
    if (isSeatBooked(sessionId, seatNumber)) {
        qDebug() << "ОШИБКА: Место" << seatNumber << "уже забронировано!";
        return;
    }

    BookedSeat seat;
    seat.sessionId = sessionId;
    seat.seatNumber = seatNumber;
    seat.username = username;
    seat.bookingTime = QDateTime::currentDateTime();

    bookedSeats.append(seat);
    saveBookedSeats(); // Немедленно сохраняем

    qDebug() << "УСПЕХ: Место" << seatNumber << "забронировано для" << username;
}

void MovieDatabase::unbookSeat(int sessionId, int seatNumber)
{
    for (int i = 0; i < bookedSeats.size(); i++) {
        if (bookedSeats[i].sessionId == sessionId &&
            bookedSeats[i].seatNumber == seatNumber) {
            bookedSeats.removeAt(i);
            saveBookedSeats();
            break;
        }
    }
}

void MovieDatabase::addUser(const User& user)
{
    users.append(user);
    saveUsers();
}

User* MovieDatabase::findUser(const QString& username)
{
    for (User& user : users) {
        if (user.getUsername() == username) {
            return &user;
        }
    }
    return nullptr;
}

bool MovieDatabase::validateUser(const QString& username, const QString& password)
{
    User* user = findUser(username);
    return user && user->getPassword() == password;  // ИСПРАВЛЕНО: -> вместо .
}

bool MovieDatabase::loadBookedTickets(const QString& username)
{
    QFile file(username + "_tickets.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    QList<int> tickets;
    while (!in.atEnd()) {
        QString line = in.readLine();
        bool ok;
        int ticketId = line.toInt(&ok);
        if (ok) {
            tickets.append(ticketId);
        }
    }
    file.close();

    User* user = findUser(username);
    if (user) {
        user->setBookedTickets(tickets);
        return true;
    }
    return false;
}

bool MovieDatabase::saveBookedTickets(const QString& username)
{
    User* user = findUser(username);
    if (!user) return false;

    QFile file(username + "_tickets.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    for (int ticketId : user->getBookedTickets()) {
        out << ticketId << "\n";
    }

    file.close();
    return true;
}

void MovieDatabase::updateUserTickets(const QString& username, const QList<int>& tickets)
{
    User* user = findUser(username);
    if (user) {
        user->setBookedTickets(tickets);
        saveBookedTickets(username);
    }
}

QList<Movie> MovieDatabase::getMovies() const { return movies; }

Movie MovieDatabase::getMovieById(int id) const
{
    for (const Movie& movie : movies) {
        if (movie.id == id) return movie;
    }
    return Movie();
}

QList<CinemaSession> MovieDatabase::getSessions() const { return sessions; }

QList<CinemaSession> MovieDatabase::getSessionsForMovie(int movieId) const
{
    QList<CinemaSession> result;
    for (const CinemaSession& session : sessions) {
        if (session.movieId == movieId) {
            result.append(session);
        }
    }
    return result;
}

CinemaSession MovieDatabase::getSessionById(int id) const
{
    for (const CinemaSession& session : sessions) {
        if (session.id == id) return session;
    }
    return CinemaSession();
}

bool MovieDatabase::bookTicket(int sessionId, int seats)
{
    // Этот метод теперь не нужен для мест, оставьте для совместимости
    for (CinemaSession& session : sessions) {
        if (session.id == sessionId && session.availableSeats >= seats) {
            session.availableSeats -= seats;
            return true;
        }
    }
    return false;
}

void MovieDatabase::updateSession(const CinemaSession& updatedSession)
{
    for (CinemaSession& session : sessions) {
        if (session.id == updatedSession.id) {
            session = updatedSession;
            break;
        }
    }
}
