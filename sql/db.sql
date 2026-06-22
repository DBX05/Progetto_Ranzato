CREATE DATABASE IF NOT EXISTS agendadb;
USE agendadb;

DROP TABLE IF EXISTS impegni;
DROP TABLE IF EXISTS persona;

CREATE TABLE persona (
    Id INT AUTO_INCREMENT NOT NULL,
    Nome VARCHAR(50),
    Email VARCHAR(100),
    Password VARCHAR(100),
    DataNascita DATE,
    PRIMARY KEY (Id)
);

CREATE TABLE impegni (
    Id INT AUTO_INCREMENT NOT NULL,
    DataInizio DATE,
    DataFine DATE,
    Priorita INT,
    Nome VARCHAR(100),
    Descrizione TEXT,
    OrarioInizio TIME,
    OrarioFine TIME,
    Proprietario INT NOT NULL,
    PRIMARY KEY (Id),
    FOREIGN KEY (Proprietario) REFERENCES persona(Id)
        ON DELETE CASCADE
        ON UPDATE CASCADE
);

INSERT INTO persona (Nome, Email, Password, DataNascita) VALUES
('Riccardo Bianchi', 'riccardo@example.com', 'pass123', '1999-04-12'),
('Laura Verdi', 'laura.verdi@example.com', 'laura2024', '1987-11-03'),
('Marco Rossi', 'marco.rossi@example.com', 'marco_pw', '1995-07-21');

INSERT INTO impegni (DataInizio, DataFine, Priorita, Nome, Descrizione, OrarioInizio, OrarioFine, Proprietario) VALUES
('2026-06-10', '2026-06-10', 1, 'Riunione di lavoro', 'Meeting con il team', '09:00:00', '10:30:00', 1),
('2026-06-11', '2026-06-11', 2, 'Allenamento', 'Palestra', '18:00:00', '19:30:00', 1),
('2026-06-12', '2026-06-12', 3, 'Visita medica', NULL, '15:00:00', '16:00:00', 2),
('2026-06-13', '2026-06-13', 1, 'Compleanno', 'Cena con amici', '20:00:00', '23:00:00', 3),
('2026-06-14', '2026-06-14', 2, 'Studio', NULL, '14:00:00', '17:00:00', 1);
