# Mini Redis (C)

A simple in-memory key-value store with a TCP server, similar to Redis.

## Files
- hashtable.h / hashtable.c - hash table with chaining for storing keys and values
- server.c - TCP server, handles client connections and commands
- Makefile - build the project

## Build

```
make
```

## Run

```
./mini_redis
```

Server starts on port 6380.

## Commands

Connect using netcat or telnet:

```
nc localhost 6380
```

Supported commands:

```
SET key value
GET key
DEL key
EXISTS key
QUIT
```

Example:

```
SET name john
OK
GET name
john
EXISTS name
1
DEL name
OK
GET name
NULL
```

## Notes

- Hash table uses chaining to handle collisions.
- Server uses one thread per client connection with a mutex so multiple clients can read/write the same data safely.
- Values are stored as strings.
