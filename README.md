# Multiuser Socket Chat Library (C++)

A reusable multi-client terminal chat library built using **raw TCP sockets in C++**, using `select()` for event-driven I/O multiplexing.

This project started as a low-level socket chat application and was later refactored into a reusable event-driven networking library.

---

# 🚀 Features

- Multi-client TCP chat system
- Event-driven architecture using `select()`
- Reusable `ChatServer` and `ChatClient` classes
- Username system with duplicate-name rejection
- Broadcast and private messaging
- Command support
- ANSI terminal colour support
- Event-based frontend/backend separation
- Static library build (`.a`) support

---

# 📦 Download & Use

A precompiled static library release is available under:

```text
Release Tag: v1
```

Download the zip file which includes
- `/include/ChatServer.h`
- `/include/ChatClient.h`
- `/lib/libchat.a`


Then include the headers and link the static library while compiling.

---

# 🛠️ Linking the Library

Example compile command:

```bash
g++ main.cpp -I./include -L./lib -lchat
```

Where:

- `-I` → include path for headers
- `-L` → library path
- `-lchat` → links `libchat.a`

---

# 🧠 Library Design

The library follows an **event-driven architecture**.

Both `ChatServer` and `ChatClient` expose a:

```cpp
Event run();
```

method which processes **one network event at a time**.

Each event contains:

```cpp
event.type
event.message
```

---

# 📌 Available Event Types

| Event | Description |
|-------|-------------|
| `NONE` | No meaningful event |
| `MESSAGE` | Normal chat message |
| `PERSONAL` | Private message |
| `SERVER` | System/server response |
| `USER_JOIN` | User joined chat |
| `USER_LEFT` | User disconnected |
| `ERROR` | Recoverable error |
| `FATAL` | Fatal connection/server issue |

---

# 🎨 Terminal Colours

ANSI colour macros are already defined inside the headers.

Example:

```cpp
cout << YELLOW << "Hello" << RESET << endl;
```

Available colours include:

```cpp
RED
GREEN
BLUE
YELLOW
CYAN
MAGENTA
RESET
```

---

# 💬 Commands

| Command | Description |
|----------|-------------|
| `/users` | List all connected users |
| `/rename <name>` | Change username |
| `/msg <name> <text>` | Send private message |

---

# ⚡ Basic Server Usage

Initialize the server:

```cpp
ChatServer srv("3456");
```

Run the event loop:

```cpp
while(true){
    Event state = srv.run();

    if(state.type == USER_JOIN){
        cout << GREEN << state.message << RESET << endl;
    }

    if(state.type == MESSAGE){
        cout << BLUE << state.message << RESET << endl;
    }

    if(state.type == USER_LEFT){
        cout << YELLOW << state.message << RESET << endl;
    }

    if(state.type == ERROR){
        cout << RED << state.message << RESET << endl;
    }
}
```

---

# ⚡ Basic Client Usage

Initialize the client:

```cpp
ChatClient clt("3456", "127.0.0.1");
```

Select username until accepted:

```cpp
while(true){
    Event result = clt.select_username();

    if(result.type == ERROR){
        cout << result.message << endl;
        continue;
    }

    break;
}
```

Run the client event loop:

```cpp
while(true){
    Event state = clt.run();

    if(state.type == MESSAGE){
        cout << state.message << endl;
    }

    if(state.type == PERSONAL){
        cout << CYAN << state.message << RESET << endl;
    }

    if(state.type == SERVER){
        cout << MAGENTA << state.message << RESET << endl;
    }

    if(state.type == FATAL){
        cout << RED << state.message << RESET << endl;
        break;
    }
}
```

---

# 📂 Example Implementations

Inside:

```text
lib/USE
```

you can find example implementations demonstrating how to:
- create a server
- create clients
- handle events
- display coloured terminal output
- build custom frontends using the library

---

# 📁 Project Structure

```text
lib/
│
├── include/
│   ├── ChatServer.h
│   └── ChatClient.h
│
├── src/
│   ├── ChatServer.cpp
│   └── ChatClient.cpp
│
├── build/
│   └── libchat.a
│
└── USE/
    ├── server_example.cpp
    └── client_example.cpp
```

---

# 🧪 Original Raw-Socket Implementations

The root project directory also contains the original:
- `server.cpp`
- `client.cpp`

implementations created before converting the project into a reusable library.

These files directly use:
- `socket`
- `bind`
- `listen`
- `accept`
- `recv`
- `send`
- `select`

without the abstraction layer provided by the library.

---

# 🧠 Concepts Used

- Low-level socket programming
- TCP networking
- `select()`-based I/O multiplexing
- Event-driven system design
- State management
- Basic application-layer protocol design
- Static library packaging (`.a`)
- Reusable C++ API architecture

---

# 📌 Notes

- Default localhost IP:
  
```text
127.0.0.1
```

- Default port:

```text
3456
```

- Current implementation targets POSIX systems (Linux/macOS)

---

# 🧑‍💻 Author

Built as part of learning:
- systems programming
- networking
- reusable library/API design
- event-driven architecture in C++