# Localized Version Control System (LVCS)

A lightweight, Git-inspired version control engine written in C++17 implementing content-addressable storage, atomic rollback functionality, custom tree structures, and Max-Heap chronological history scheduling.

## Core Features
- **Content-Addressable Storage**: Hashes file contents to decouple storage from filenames, preventing duplication in `.lvcs/objects`.
- **Atomic Historical Rollbacks**: Reconstructs working directory snapshots to any target commit state without data corruption.
- **Commit Graph & O(1) Lookup**: Implements a custom in-memory Commit Tree (`CommitNode`) integrated with Hash Maps for constant-time state retrieval.
- **Custom Max-Heap Engine**: Built-in binary Max-Heap sorting commits by timestamp in $O(\log N)$ time to systematically render chronological version telemetry.

## Build and Run
```bash
# Build using C++17 standard
g++ -std=c++17 LocalVCS.cpp -o lvcs

# Commands
./lvcs init
./lvcs add <filepath>
./lvcs commit "<message>"
./lvcs rollback <commit_id>
./lvcs log
