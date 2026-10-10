CREATE DATABASE IF NOT EXISTS salon_db;
USE salon_db;

CREATE TABLE Customer (
    CustomerID INT AUTO_INCREMENT PRIMARY KEY,
    Name       VARCHAR(100) NOT NULL,
    Phone      VARCHAR(15)  NOT NULL,
    Email      VARCHAR(100)
);

CREATE TABLE Stylist (
    StylistID  INT AUTO_INCREMENT PRIMARY KEY,
    Name       VARCHAR(100) NOT NULL,
    Phone      VARCHAR(15)  NOT NULL,
    Email      VARCHAR(100),
    Speciality VARCHAR(100)
);

CREATE TABLE Service (
    ServiceID   INT AUTO_INCREMENT PRIMARY KEY,
    ServiceName VARCHAR(100) NOT NULL,
    Price       DECIMAL(10,2) NOT NULL,
    Duration    INT NOT NULL
);

CREATE TABLE Appointment (
    AppointmentID   INT AUTO_INCREMENT PRIMARY KEY,
    CustomerID      INT NOT NULL,
    StylistID       INT NOT NULL,
    ServiceID       INT NOT NULL,
    AppointmentDate DATE NOT NULL,
    AppointmentTime TIME NOT NULL,
    Status          VARCHAR(20) DEFAULT 'Booked',
    FOREIGN KEY (CustomerID) REFERENCES Customer(CustomerID) ON DELETE CASCADE,
    FOREIGN KEY (StylistID)  REFERENCES Stylist(StylistID),
    FOREIGN KEY (ServiceID)  REFERENCES Service(ServiceID)
);

CREATE TABLE Users (
    UserID   INT AUTO_INCREMENT PRIMARY KEY,
    Username VARCHAR(50) NOT NULL UNIQUE,
    Password VARCHAR(100) NOT NULL,
    Role     VARCHAR(20) NOT NULL
);

INSERT INTO Users (Username, Password, Role) VALUES
    ('admin', 'admin123', 'Admin'),
    ('reception', 'rec123', 'Receptionist');

INSERT INTO Customer (Name, Phone, Email) VALUES
    ('Nimal Perera', '0771234567', 'nimal@gmail.com'),
    ('Kamala Silva', '0712345678', 'kamala@gmail.com');

INSERT INTO Stylist (Name, Phone, Email, Speciality) VALUES
    ('Sanduni Fernando', '0761112223', 'sanduni@salon.com', 'Hair Colour'),
    ('Ruwan Jayasinghe', '0753334445', 'ruwan@salon.com', 'Haircut');

INSERT INTO Service (ServiceName, Price, Duration) VALUES
    ('Haircut', 1500.00, 30),
    ('Hair Colour', 6000.00, 90),
    ('Facial', 3500.00, 60),
    ('Manicure', 2000.00, 45);