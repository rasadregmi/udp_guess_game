# 🎮 Multi-Client UDP Guessing Game with Scoreboard

A fun, real-time, multi-client number guessing game implemented using **UDP Sockets in C**. Players connect over the LAN, register their name, and try to guess a secret number between 1 and 100. The first correct guess ends the round, logs the winner, and starts a new game automatically.

---

## 🚀 Features

- 🧑‍🤝‍🧑 Multi-client UDP server (no TCP overhead)
- 📝 Username registration via client
- 🎯 Secret number randomly generated every round
- 🧠 Real-time hints ("Too low", "Too high", "Correct")
- 🏆 Scoreboard with number of attempts
- 🗂️ Persistent logging to `scoreboard.txt`
- ⏰ Timeout: removes inactive players (after 60s)
- 🛠 Admin console: view live scoreboard

---

## 📁 Project Structure

udp_guess_game/
├── server.c # UDP Server with scoreboard and timeout
├── client.c # UDP Client with registration and guessing
├── scoreboard.txt # Auto-generated log file
├── Makefile # Build automation
└── README.md # This file

---

## 🧰 Requirements

- OS: Linux / Unix-based system
- Compiler: `gcc`
- Terminal

---

## 🧱 Compilation

Open terminal in project directory and run:

```bash
make

This will generate:
- server (for admin/host)
- client (for each player)

