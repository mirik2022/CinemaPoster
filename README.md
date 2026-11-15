# CinemaPoster

CinemaPoster is a desktop cinema-poster and ticket-booking application built with **C++17** and **Qt 6**. It provides a graphical interface for browsing movies, viewing showtimes, selecting seats, and managing ticket reservations.

## Features

- User registration and sign-in
- Movie catalogue with poster images and detailed information
- Search, filtering, and sorting of movies
- Showtimes and session information
- Interactive seat selection
- Ticket booking and cancellation
- User profile and booking history
- User balance management
- Dark application theme

## Technology stack

- C++17
- Qt 6 Widgets
- Qt Core
- CMake
- Qt Creator (recommended IDE)

## Project structure

```text
CinemaPoster/
├── images/                 # Movie posters and image assets
├── bookingdialog.cpp/.h    # Booking and seat-selection dialog
├── logindialog.cpp/.h      # Sign-in and registration
├── main.cpp                # Application entry point
├── mainwindow.cpp/.h       # Main application window
├── moviedatabase.cpp/.h    # Movie, session, and booking data logic
├── movieinfodialog.cpp/.h  # Movie details and session selection
├── seatbutton.cpp/.h       # Custom seat button
├── user.cpp/.h             # User model and related logic
├── style.qss               # Application stylesheet
├── resources.qrc           # Qt resource collection
├── CMakeLists.txt          # CMake build configuration
└── CinemaPoster.pro        # qmake project file
```

## Requirements

- Qt 6 with the **Core** and **Widgets** modules
- CMake 3.16 or newer
- A C++17-compatible compiler
- Qt Creator is recommended for configuring and building the project

The supplied `CMakeLists.txt` is the recommended build entry point. The qmake project file is also included, but its source and dependency lists may need to be updated to match the current application files.

## Build with Qt Creator

1. Install Qt 6 and a compatible C++ compiler.
2. Open `CMakeLists.txt` in Qt Creator.
3. Select a Qt 6 kit with a supported compiler.
4. Allow Qt Creator to configure the CMake project.
5. Build and run the `CinemaPoster` target.

## Build from the command line

From the project root, configure and build with CMake:

```bash
cmake -S . -B build
cmake --build build --config Release
```

CMake must be able to locate your Qt 6 installation. If Qt is not detected automatically, configure `CMAKE_PREFIX_PATH` to point to the Qt 6 installation directory.

## Local application data

The application uses local text files for user and booking information. Depending on how the application is launched, these files are created in its working directory:

- `users.txt` — user records
- `booked_seats.txt` — booked seat information
- `<username>_tickets.txt` — tickets associated with a user

These files are runtime data, not source files. They are excluded from Git by the supplied `.gitignore`. Back them up separately if you need to preserve local user or booking data.

## Notes

- Movie poster assets are stored in `images/` and are part of the project.
- Build output and Qt Creator user-specific files are not required in the repository.
- The project has not been packaged here as a ready-to-install installer; build it in a configured Qt environment.

## License

No license has been specified for this repository yet. All rights remain with their respective owners unless a license is added.
