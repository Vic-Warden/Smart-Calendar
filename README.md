# Field Command: Pattern Cuckoo

An IoT smart calendar system built around an **ESP32** microcontroller with a Warhammer 40K aesthetic. The device retrieves appointments from a web API, displays them on an LCD, plays audio cues via a DFPlayer Mini, and adapts to ambient light. The web interface features a static landing page and a Svelte SPA.

---

## Stack

| Layer | Technology |
|---|---|
| Embedded | ESP32, PIR sensor, LDR, servo, DFPlayer Mini, I2C LCD, TM1637 |
| Backend | PHP, MariaDB (MySQL) |
| Frontend | HTML, Tailwind CSS, Three.js, Svelte + Vite |
| Infrastructure | Docker (Nginx, PHP-FPM, MariaDB, phpMyAdmin, tunnel) |

---

## Requirements

- Docker & Docker Compose
- A `.env` file at the project root (see below)

---

## Setup

1. Create a `.env` file:

```env
WEBSERVER_PORT=80
MYSQL_PORT=3306
MYSQL_ROOT_PASSWORD=your_password
HVA_USERNAME=your_username
```

2. Start the stack:

```bash
docker-compose up -d
```

| Service | URL |
|---|---|
| Web app | http://localhost/ |
| Dashboard | http://localhost/dashboard |
| phpMyAdmin | http://localhost/phpmyadmin |

---

## Project Structure

```
├── docs/          # Portfolio documentation (MkDocs)
├── embedded/      # ESP32 source code (.ino)
├── web/           # PHP, HTML, CSS, JS, assets
├── omnissiah-info/# Svelte SPA (Vite)
├── docker/        # Docker images & config
└── docker-compose.yml
```

---

## Database

| Parameter | Value |
|---|---|
| Host (from PHP) | `mariadb` |
| Host (from host) | `localhost` |
| Port | `3306` |
| Username | `root` |
| Password | See `.env` |

> Create the database manually via phpMyAdmin or MySQL CLI before first use.

---

## Communication Overview

| Source | Destination | Protocol | Port |
|---|---|---|---|
| ESP32 | PHP API | HTTP | 80 |
| ESP32 | NTP Pool | UDP | 123 |
| Mobile/Browser | Nginx | HTTPS | 443 |
| Nginx | PHP-FPM | FastCGI | 9000 |
| PHP | MariaDB | MySQL | 3306 |
| Vite Dev Server | Browser | WebSocket | 5173 |