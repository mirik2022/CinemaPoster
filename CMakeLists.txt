cmake_minimum_required(VERSION 3.16)

project(CinemaPoster VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка для автоматического запуска MOC, UIC и RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOMOC_RELAXED_MODE ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Поиск пакетов Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Создание исполняемого файла
qt_add_executable(CinemaPoster
    main.cpp
    mainwindow.cpp
    mainwindow.h
    moviedatabase.cpp
    moviedatabase.h
    user.cpp
    user.h
    logindialog.cpp
    logindialog.h
    bookingdialog.cpp
    bookingdialog.h
    movieinfodialog.cpp
    movieinfodialog.h
    seatbutton.h
    seatbutton.cpp
)

# После qt_add_executable добавьте:
# После qt_add_executable добавьте:

# Для Windows
if(WIN32)
    # Укажите путь к иконке
    set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icon.ico")

    # Проверяем, существует ли файл иконки
    if(EXISTS ${APP_ICON})
        # Для Windows
        set(RESOURCE_FILE "${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc")

        # Создаем .rc файл
        file(WRITE ${RESOURCE_FILE}
            "IDI_ICON1 ICON DISCARDABLE \"${APP_ICON}\"\n"
            "1 VERSIONINFO\n"
            "FILEVERSION 1,0,0,0\n"
            "PRODUCTVERSION 1,0,0,0\n"
            "FILEOS 0x4\n"
            "FILETYPE 0x1\n"
            "{\n"
            "  BLOCK \"StringFileInfo\"\n"
            "  {\n"
            "    BLOCK \"040904b0\"\n"
            "    {\n"
            "      VALUE \"CompanyName\", \"Cinema Vision\\0\"\n"
            "      VALUE \"FileDescription\", \"Cinema Poster Application\\0\"\n"
            "      VALUE \"FileVersion\", \"1.0.0.0\\0\"\n"
            "      VALUE \"InternalName\", \"CinemaPoster\\0\"\n"
            "      VALUE \"LegalCopyright\", \"Copyright © 2024 Cinema Vision\\0\"\n"
            "      VALUE \"OriginalFilename\", \"CinemaPoster.exe\\0\"\n"
            "      VALUE \"ProductName\", \"Cinema Poster\\0\"\n"
            "      VALUE \"ProductVersion\", \"1.0.0.0\\0\"\n"
            "    }\n"
            "  }\n"
            "  BLOCK \"VarFileInfo\"\n"
            "  {\n"
            "    VALUE \"Translation\", 0x409, 1200\n"
            "  }\n"
            "}\n"
        )

        # Добавляем ресурсный файл к цели
        target_sources(CinemaPoster PRIVATE ${RESOURCE_FILE})

        # Для MinGW
        if(MINGW)
            set_target_properties(CinemaPoster PROPERTIES
                LINK_FLAGS "-Wl,--subsystem,windows -mwindows"
            )
        endif()
    else()
        message(WARNING "Файл иконки не найден: ${APP_ICON}")
    endif()
endif()
# Для релиза
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Подключение Qt модулей
target_link_libraries(CinemaPoster
    PRIVATE
        Qt6::Core
        Qt6::Widgets
)

# Настройка свойств целевого приложения
target_compile_definitions(CinemaPoster PRIVATE "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>")
target_compile_features(CinemaPoster PRIVATE cxx_std_17)

# Добавление ресурсов
qt_add_resources(CinemaPoster "resources"
    PREFIX
    "/"
    FILES
    style.qss
    images/interstellar.jpg
    images/inception.jpg
    images/matrix.jpg
    images/joker.jpg
    images/avatar.jpg
    images/oppenheimer.jpg
    images/barbie.jpg
    images/tenet.jpg
    images/claustr.jpg
    images/divergent.jpg
    images/duna.jpg
    images/euphoria.jpg
    images/gem.jpg
    images/hrabr.jpg
    images/illusion.jpg
    images/venom.jpg
    images/uncharted.jpg
    images/mstit.jpg
    images/odna.jpg
)

# Настройка для Windows
if(WIN32)
    set_target_properties(CinemaPoster PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()
