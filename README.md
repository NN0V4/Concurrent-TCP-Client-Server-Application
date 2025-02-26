# Concurrent-TCP-Client-Server-Application
This project implements a concurrent TCP server that processes string operations requested by multiple clients. The server can handle up to 4 clients concurrently, logging every request and response. The client reads input from a file and communicates with the server over a single TCP connection.

⚙️ How It Works


1️⃣ Server Functionality

Accepts a port number as a command-line argument.

Listens for client connections and processes up to 4 clients concurrently.

Performs five string operations:

Count words in a string.

Count vowels in a string.

Count special characters (non-alphanumeric).

Find the longest word.

Identify repeated words.

Maintains a log file recording requests and results.

Handles client disconnection via SIGQUIT signal.

2️⃣ Client Functionality

Accepts server IP, port number, and input file name as command-line arguments.

Reads each line from the file, prompts the user to select an operation, and sends the request to the server.

Receives and displays results from the server.
Uses a single persistent TCP connection for multiple requests.
Handles invalid operations gracefully.
