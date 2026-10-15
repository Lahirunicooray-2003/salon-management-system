# Salon Aura - Salon Management System

ICT1306 Object Oriented Programming group assignment.
A desktop application written in C++ (MFC, Visual Studio) that stores its data in a MySQL database.

## Features
- Admin login (checked against the database, SQL-injection safe)
- Customer, Service and Stylist management (add, update, delete)
- Appointment booking with a double-booking check
- Appointment status: Booked, Completed or Cancelled (right-click a row); cancelled slots can be booked again
- Live search box on every list (type to filter)
- Object oriented design: abstract `Person` class, `Customer` and `Stylist` inherit from it,
  encapsulation with validating setters, polymorphism through a `Person` pointer

## Screenshots
| Login | Dashboard |
|---|---|
| ![Login](screenshots/login.png) | ![Dashboard](screenshots/dashboard.png) |

| Customers | Appointments |
|---|---|
| ![Customers](screenshots/customers.png) | ![Appointments](screenshots/appointments.png) |

## Requirements
- Windows 10/11, Visual Studio 2022 or later with "Desktop development with C++" (MFC)
- MySQL Server 8.x and MySQL Workbench
- MySQL C connector (`mysql.h`, `libmysql.lib`, `libmysql.dll`)

## How to run
1. **Create the database**
   Open `database/schema.sql` in MySQL Workbench and run it.
   It creates the database `salon_db`, the five tables and some sample data
   (demo login: `admin` / `admin123`).
2. **Set the connection**
   Copy `DBConfig.example.h` to `DBConfig.h` and enter your own MySQL host, user and password.
   `DBConfig.h` is in `.gitignore`, so a real password is never uploaded.
3. **Link the MySQL connector in Visual Studio**
   Project > Properties (x64):
   - C/C++ > General > Additional Include Directories: the connector's `include` folder
   - Linker > General > Additional Library Directories: the connector's `lib` folder
   - Linker > Input > Additional Dependencies: `libmysql.lib`
4. **Build and run** (Ctrl+Shift+B, Ctrl+F5).
   Copy `libmysql.dll` and the pictures from the `assets` folder next to
   `SalonManagementSystem.exe` (`x64\Debug`). Without the pictures the program still works,
   it just shows plain backgrounds.

## Project structure
| Part | Purpose |
|---|---|
| `Person`, `Customer`, `Stylist` | model classes (abstraction, inheritance, polymorphism) |
| `DBConnection` | all MySQL work, including `Escape()` |
| `CCustomerDlg`, `CServiceDlg`, `CStylistDlg`, `CAppointmentDlg` | management screens |
| `CDashboardDlg`, `CSalonManagementSystemDlg` | dashboard and login |
| `Theme.h` | colours, background images, custom buttons |
| `SearchBox.h` | small reusable search box that filters a list |
| `database/schema.sql` | database structure and sample data |

## Team
ICT1306 Object Oriented Programming group assignment, 2026.

| Reg. No | Name |
|---|---|
| 6698 | G.K.P. Nimasha |
| 6690 | P.M.C.D. Mohottala |
| 6637 | T.M.C.L.B. Cooray |

## Demo video
(add the unlisted YouTube link here)
