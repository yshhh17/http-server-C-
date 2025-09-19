# TCP Chat Application

A simple terminal-based chat application built in **C** using sockets and multithreading. This project allows multiple users on the same LAN to communicate in real-time.

## Features
- Real-time chat between two or more PCs on the same network
- Multi-threaded server to handle concurrent clients
- Lightweight and terminal-based (no GUI required)
- Easy to set up and use

## How It Works
1. The project contains **two files**:  
   - `server2.c` – Run this on the PC that will act as the server.  
   - `client1.c` – Run this on the other PC(s) that want to connect to the server.
2. Connect both PCs to the **same network**.
3. Open a terminal on the server PC and run:
   ```bash
   gcc server2.c -o server
   ./server

4. On the client PC, open a terminal and run:
  ```bash
   gcc client1.c -o server
   ./client
