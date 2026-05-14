# Multiuser Socket Chat (C++)

A multi-client terminal chat application built using **raw TCP sockets in C++**, supporting multiple users concurrently using the `select()` system call (event-driven I/O).

---

## 🚀 Features

- Multi-user chat over TCP
- Concurrent client handling using `select()` (no threads)
- Username system with duplicate name rejection
- Colored terminal output for better readability
- Real-time message broadcasting
- Private messaging support

---

## 🧠 Concepts Used

- Low-level socket programming (`socket`, `bind`, `listen`, `accept`, `recv`, `send`)
- I/O multiplexing with `select()`
- Basic client-server protocol design
- State management (`fd → username` mapping)

---

## 💬 Commands

| Command | Description |
|----------|-------------|
| `/users` | List all connected users |
| `/rename <name>` | Change your username |
| `/msg <name> <text>` | Send a private message |

---

## 🛠️ How to Run

### 1. Compile

```bash
g++ server.cpp -o server
g++ client.cpp -o client
```

### 2. Start Server

```bash
./server
```

### 3. Start Client(s)

```bash
./client
```

- Enter a username (duplicates are rejected)
- Start chatting!

---

## 📌 Notes

- The server handles multiple clients using `select()` (event-loop based, no multithreading)
- ANSI escape codes are used for colored terminal output
- Designed as a learning project for low-level networking and system design

---

## 📦 Future Improvements

- Refactor into reusable class/library

---

## 🧑‍💻 Author

Built as part of learning systems programming, networking, and C++.