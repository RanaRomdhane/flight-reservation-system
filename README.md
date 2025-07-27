# Flight Reservation System ✈️

![C](https://img.shields.io/badge/C-Programming-00599C?logo=c)
![Socket](https://img.shields.io/badge/Socket-Programming-important)
![Multi-threading](https://img.shields.io/badge/Thread-Safe-success)

A robust client-server flight reservation system developed as part of our Systems & Network Programming course at ENICarthage.

## 📌 Project Overview
This system implements a complete flight booking solution with:
- Real-time seat availability
- Concurrent client access
- Transaction management
- Persistent data storage

## 🛠️ Technical Stack
| Component          | Technology               |
|--------------------|--------------------------|
| Language           | C                        |
| Architecture       | Client-Server Model      |
| Communication      | Custom TCP Protocol      |
| Concurrency        | POSIX Threads            |
| Data Persistence   | File-based Storage       |
| Synchronization    | Mutex Locks              |

## ✨ Key Features
### For Clients
- 🔍 Flight search by destination/date
- 🎟️ Real-time seat reservation
- 📝 Booking modification/cancellation
- 💰 Invoice generation

### For System
- 🚀 Multi-client concurrent access
- 🔒 Thread-safe operations
- 💾 Persistent data storage
- ⚡ Custom communication protocol

## 📂 Project Structure
flight-reservation-system/
├── serveur/ # Server application
├── client/ # Client application
├── common/ # Common files
├── data_files/ # Data
├──README.md # This file
└── start_server.sh # Start the server


## 🚀 Getting Started
### Prerequisites
- GCC compiler
- POSIX-compliant system
- Basic network configuration

### Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/RanaRomdhane/flight-reservation-system.git

2. Compile the server:

   ```bash
    cd server
    make


3. Compile the client:

   ```bash
    cd ../client
    make


### Usage
1. Start the server:

   ```bash
    ./server

2. Run clients:

   ```bash
    ./client

## 🚀 Happy travels with our flight reservation system!