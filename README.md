# Client Server Socket Programming in C

This project consists of implementing a client-server socket program in C with various designs and features. The aim is to explore different server designs and observe their performance metrics. Below is an overview of the project's features:

## Instructions to run

 1. Each part of the project has corresponding folder (eg. fork for fork-
    based server, non_block folder has 3 nested folders)
2. Navigate to each folder to run the code, using following steps 
	- `make` to compile server and client files
	- `make run_server` to run the server code
	- `make run_client` to run the client code (do it in separate terminal)
	- `make clean` to remove binaries and text output





## Server Program

1.  **Sequential Server**: Opens a new file upon the first client connection. For each client request, it computes the factorial of the received number, stores the result along with the client's ID in the file, and sends the result back to the client. The file is closed when all client connections are closed.
    
2.  **Concurrent Server with Multiple Processes**: Utilizes the `fork` system call to create multiple processes for concurrent handling of client requests.
    
3.  **Concurrent Server with Multiple Threads**: Uses the `pthreads` library to create multiple threads for concurrent handling of client requests.
    
4.  **Non-blocking Server**: Capable of managing a maximum of 10 clients using different system calls:
    
    -   **Using select()**: Implements non-blocking I/O operations using the `select()` system call.
    -   **Using poll()**: Implements non-blocking I/O operations using the `poll()` system call.
    -   **Using epoll API**: Utilizes the epoll API for efficient event-driven I/O operations.

## Metrics and Observations

1.  For each server design (except 2a), the server concurrently processes 10 client requests. This allows running 10 concurrent client programs.
    
2.  The following metrics are observed:
    
    -   Sequential server programs
    ![Alt text](https://i.imgur.com/3BqMhMt.png "Sequential server program")
    
    -   Concurrent server programs with multiple processes
![Alt text](https://i.imgur.com/qM3JPQa.png "Concurrent program with multiple process")

    -   Concurrent server program with multiple threads
    ![Alt text](https://i.imgur.com/m4mEudc.png "Sequential server program")
    
    -   Non-blocking server that can manage total 10 clients using select() system call
![Alt text](https://i.imgur.com/OwBKvsL.png "Concurrent program with multiple process")

    -   Non-blocking server that can manage total 10 clients using poll() system call
    ![Alt text](https://i.imgur.com/7avWEfw.png "Sequential server program")
    
    -   Non-blocking server that can manage total 10 clients using epoll() system call
![Alt text](https://i.imgur.com/j6t4MBI.png "Concurrent program with multiple process")

