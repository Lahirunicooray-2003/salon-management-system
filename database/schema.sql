-- =====================================================================
--  Salon Aura - Salon Management System (ICT1306 OOP group assignment)
--  Database setup script for MySQL 8.x
--
--  How to run:
--    MySQL Workbench : File > Open SQL Script > this file > run (lightning icon)
--    Command line    : mysql -u root -p < database/schema.sql
--
--  After running it, put the same host / user / password in DBConfig.h
--  (see DBConfig.example.h) and the application can connect.
-- =====================================================================

CREATE DATABASE IF NOT EXISTS salon_db
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE salon_db;

-- Remove old tables first so the script can be run again (children first)
DROP TABLE IF EXISTS Appointment;
DROP TABLE IF EXISTS Service;
DROP TABLE IF EXISTS Stylist;
DROP TABLE IF EXISTS Customer;
DROP TABLE IF EXISTS Users;

-- ---------------------------------------------------------------------
--  Users : accounts that can log in to the application
-- ---------------------------------------------------------------------
CREATE TABLE Users (
    UserID    INT AUTO_INCREMENT PRIMARY KEY,
    Username  VARCHAR(50)  NOT NULL UNIQUE,
    Password  VARCHAR(100) NOT NULL,
    Role      VARCHAR(30)  NOT NULL DEFAULT 'Admin'
);

-- ---------------------------------------------------------------------
--  Customer
-- ---------------------------------------------------------------------
CREATE TABLE Customer (
    CustomerID INT AUTO_INCREMENT PRIMARY KEY,
    Name       VARCHAR(100) NOT NULL,
    Phone      VARCHAR(20)  NOT NULL,
    Email      VARCHAR(100) NOT NULL
);

-- ---------------------------------------------------------------------
--  Stylist
-- ---------------------------------------------------------------------
CREATE TABLE Stylist (
    StylistID  INT AUTO_INCREMENT PRIMARY KEY,
    Name       VARCHAR(100) NOT NULL,
    Phone      VARCHAR(20)  NOT NULL,
    Email      VARCHAR(100) NOT NULL,
    Speciality VARCHAR(100) NOT NULL
);

-- ---------------------------------------------------------------------
--  Service
-- ---------------------------------------------------------------------
CREATE TABLE Service (
    ServiceID   INT AUTO_INCREMENT PRIMARY KEY,
    ServiceName VARCHAR(100)  NOT NULL,
    Price       DECIMAL(10,2) NOT NULL,
    Duration    INT           NOT NULL        -- minutes
);

-- ---------------------------------------------------------------------
--  Appointment : links a customer, a stylist and a service
-- ---------------------------------------------------------------------
CREATE TABLE Appointment (
    AppointmentID   INT AUTO_INCREMENT PRIMARY KEY,
    CustomerID      INT         NOT NULL,
    StylistID       INT         NOT NULL,
    ServiceID       INT         NOT NULL,
    AppointmentDate DATE        NOT NULL,
    AppointmentTime TIME        NOT NULL,
    Status          VARCHAR(20) NOT NULL DEFAULT 'Booked',
    FOREIGN KEY (CustomerID) REFERENCES Customer(CustomerID),
    FOREIGN KEY (StylistID)  REFERENCES Stylist(StylistID),
    FOREIGN KEY (ServiceID)  REFERENCES Service(ServiceID)
);

-- =====================================================================
--  Sample data for testing / demo
-- =====================================================================

-- Demo login.  Change the password before real use.
INSERT INTO Users (Username, Password, Role) VALUES ('admin', 'admin123', 'Admin');

INSERT INTO Customer (Name, Phone, Email) VALUES
    ('Nimal Perera', '0771234567', 'nimal@gmail.com'),
    ('Kamala Silva', '0712345668', 'kamala@gmail.com');

INSERT INTO Stylist (Name, Phone, Email, Speciality) VALUES
    ('Sanduni Fernando',  '0761112223', 'sanduni@salon.com', 'Hair Colouring'),
    ('Ruwan Jayasinghe',  '0753334445', 'ruwan@salon.com',   'Haircut'),
    ('Liyo',              '0716989449', 'liyo@salon.com',    'Hair Care');

INSERT INTO Service (ServiceName, Price, Duration) VALUES
    ('Haircut',     1500.00, 30),
    ('Hair Colour', 6000.00, 90),
    ('Facial',      3500.00, 60),
    ('Manicure',    2000.00, 45);

INSERT INTO Appointment (CustomerID, StylistID, ServiceID, AppointmentDate, AppointmentTime, Status)
VALUES (1, 3, 2, '2026-10-20', '10:45:00', 'Booked');
