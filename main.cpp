#include "mainwindow.h"
#include "logindialog.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Устанавливаем темную тему
    a.setStyle(QStyleFactory::create("Plastique"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    a.setPalette(darkPalette);

    // Создаем диалог входа
    LoginDialog loginDialog;

    // Показываем диалог входа
    if (loginDialog.exec() == QDialog::Accepted) {
        // Проверяем, воше  л ли пользователь
        if (loginDialog.isLoggedIn()) {
            // Создаем главное окно
            MainWindow w;
            w.setCurrentUser(loginDialog.getLoggedUsername()); // Используем новый метод
            w.show();

            // Запускаем главный цикл приложения
            return a.exec();
        }
    }

    // Если пользователь не вошел или закрыл окно, завершаем программу
    return 0;
}
