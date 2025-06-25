# 🎮 Multi-Client UDP Guessing Game with Scoreboard

A fun, real-time, multi-client number guessing game implemented using **UDP Sockets in C**. Players connect over the LAN, register their name, and try to guess a secret number between 1 and 100. The first correct guess ends the round, logs the winner, and starts a new game automatically.

---

## 🚀 Features

- 🧑‍🤝‍🧑 Multi-client UDP server (no TCP overhead)  
- 📝 Username registration via client  
- 🎯 Secret number randomly generated every round  
- 🧠 Real-time hints ("Too low", "Too high", "Correct") broadcast instantly  
- 🏆 Scoreboard tracking number of attempts per player  
- 🗂️ Persistent logging of game results to `scoreboard.txt`  
- ⏰ Timeout: automatically removes inactive players after 60 seconds  
- 🛠 Admin console for live scoreboard display  
- 🔄 Graceful shutdown for server and clients  
- 🌈 Color-coded terminal UI for better experience  

---

## 📁 Project Structure

```
udp_guess_game/
├── server.c          # UDP Server with scoreboard and timeout management
├── client.c          # UDP Client with registration and interactive guessing
├── scoreboard.txt    # Auto-generated persistent log file
├── Makefile          # Build automation for compiling server and client
└── README.md         # Project overview and instructions
```

---

## 🧰 Requirements

- Operating System: Linux or Unix-based (tested on Ubuntu)  
- Compiler: `gcc` with pthread support  
- Terminal emulator for server and clients  

---

## 🧱 Compilation

Open your terminal inside the `udp_guess_game` directory and run:

```bash
make
```

This will compile and generate two executables:  
- `server` — Run this on the host machine to manage the game  
- `client` — Run one or more clients on player machines  

---

## 🚦 How to Run

1. **Start the server:**

```bash
./server
```

You will see the server generate a secret number and start waiting for clients.

2. **Start one or more clients:**

```bash
./client
```

- Enter your player name when prompted.  
- Start guessing numbers between 1 and 100.  
- Receive immediate hints and broadcast updates.

---

## 🛠 Admin Commands

While the server is running, you can type commands in the server terminal:

- `SHOW_SCOREBOARD`  
  Displays the current scoreboard with all players and their attempt counts.

- Any other input will show:  
  ```
  ⚠️ Only use valid commands like: SHOW_SCOREBOARD
  ```

---

## ⏰ Inactivity Handling

- Players inactive for more than 60 seconds will be automatically removed.  
- Inactive players receive a notification and the client program exits gracefully.

---

## 🎉 Gameplay Flow

- Players guess numbers independently; hints are sent immediately.  
- When a player guesses correctly, **all clients are notified immediately**.  
- A new game round starts automatically with a new secret number.  

---

## 🧹 Graceful Shutdown

- Press `Ctrl+C` on server or client to exit cleanly.  
- All sockets will close properly and resources freed.

---

## 📝 Notes

- This is a UDP-based implementation, so packet loss is possible on unstable networks.  
- Designed for learning and demonstration of UDP sockets, concurrency, and simple network protocols.

---

## 📧 Contact

For any questions or issues, feel free to reach out.

---

Enjoy the game and happy guessing! 🎯