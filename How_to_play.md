# 🎮 Multi-Client UDP Guessing Game with Scoreboard

## 🧑‍💻 How to Play

### 1. Compile the Project

Open a terminal in the project directory and run:
```bash
make
```
This builds:
- `server` (the game host/admin)
- `client` (for each player)

### 2. Start the Server

In a terminal, run:
```bash
./server
```
You will see output similar to:
```
🎯 New secret number: 53
Enter admin command (SHOW_SCOREBOARD):
```
The server now listens for clients.

### 3. Start a Client

In another terminal (or multiple terminals for multiple players), run:
```bash
./client
```
You will be prompted:
```
Enter your name: <YourName>
```
After entering your name, you’ll see:
```
✅ Welcome <YourName>! Start guessing...
```

### 4. Guessing Loop

After registration, enter guesses:
```text
Enter your guess (1-100): 42
```
The server responds:
```
🧠 Server: 🔻 Too low (1 attempts)
```
or
```
🧠 Server: 🔺 Too high (2 attempts)
```
Keep guessing until:
```
🧠 Server: 🎉 <WinnerName> guessed it in <N> attempts!
🧠 Server: 🔁 New game starting!
```

### 5. Multiplayer

- Run `./client` in separate terminals for each player.
- Each player registers with a unique name.
- All players send guesses concurrently.
- When one wins, all clients receive the broadcast message.
- A new round starts automatically.

### 6. Admin Commands (on Server)

In the server terminal, type:
```text
SHOW_SCOREBOARD
```
Output:
```
📊 Scoreboard:
 - Alice: 3 attempts
 - Bob: 5 attempts
```
Shows active players and their current attempt counts.

### 7. Timeout / Inactivity

- Players inactive for 60 seconds are automatically removed.
- The server prints:
```
⏰ Removing inactive player: <PlayerName>
```
- Helps manage participants in long-running games.

### 8. Logging

- Winning guesses are appended to `scoreboard.txt`, e.g.:
```
Alice guessed the number in 3 attempts on Tue Jun 24 18:32:18 2025
```
- Check this file for game history.

### 9. Exiting

- To stop the server: press `Ctrl+C` in the server terminal.
- Clients will no longer receive responses once the server is stopped.

### 10. Summary of Commands

- **Compile**: `make`
- **Start Server**: `./server`
- **Start Client**: `./client`
- **Admin**: Type `SHOW_SCOREBOARD` in server terminal
- **Guess**: Enter a number between 1 and 100 at client prompt

---

## 📂 Project Structure
```
udp_guess_game/
├── server.c
├── client.c
├── scoreboard.txt    
├── Makefile
└── README.md         
```

---

Enjoy the game and happy guessing! 🚀