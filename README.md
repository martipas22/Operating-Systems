# Operating Systems — C Programming Projects

A curated selection of practical assignments and seminar exercises from an Operating Systems course, covering POSIX file I/O, process creation, inter-process communication, Unix signals, pthreads, mutexes, and semaphores.

## Practical assignments

| File | Topic |
|---|---|
| `projects/file-operations/text-binary-integer-converter.c` | Convert integer data between text and binary file representations using POSIX file I/O. |
| `projects/file-operations/binary-file-integer-editor.c` | Insert, remove, and modify data in binary files using `lseek`, `read`, `write`, and `ftruncate`. |
| `projects/concurrency/multistage-producer-consumer-pipeline.c` | Concurrent producer-consumer pipeline using POSIX threads, mutexes, and semaphores. |

## Selected seminar exercises

| File(s) | Topic |
|---|---|
| `examples/processes/bidirectional-pipe-square-calculator.c` | Parent-child IPC with two pipes. |
| `examples/processes/parent-child-even-number-filter.c` | Child process filters integers and returns results over a pipe. |
| `examples/signals/send-sigusr1.c`, `wait-for-sigusr1.c` | Sending and receiving a Unix signal. |
| `examples/threads/mutex-protected-particle-transfers.c` | Thread synchronization with a mutex. |
| `examples/threads/semaphore-limited-resource-access.c`, `semaphore.[ch]` | Limit concurrent access with a counting semaphore implementation. |

## Build

These are independent C programs intended for a POSIX environment (Linux recommended). For example:

```bash
gcc -Wall -Wextra -pthread projects/concurrency/multistage-producer-consumer-pipeline.c -o producer_consumer
gcc -Wall -Wextra projects/file-operations/text-binary-integer-converter.c -o converter
gcc -Wall -Wextra -pthread examples/threads/semaphore-limited-resource-access.c examples/threads/semaphore.c -o semaphore_demo
```

Some files are preserved as coursework exercises rather than production-ready utilities; review input handling and synchronization before adapting them for production.

## Curation

Only a focused selection of source code is included. Duplicated solutions, instructor slide decks, temporary outputs, and compiled binaries from the original archive were excluded. Code is retained substantially as originally written; descriptive filenames and this README were added for navigation.
