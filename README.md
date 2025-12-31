# Hospital Patient Queue System (C)

A console-based hospital patient management system written in C.  
The system manages patients using a **priority-based queue** and supports searching, sorting, file persistence, and statistical reporting.

## Features

- Priority-based patient queue (1 = critical, 5 = low)
- Dynamic memory management using `malloc` and `realloc`
- Linear search (by name)
- Binary search (by ID)
- Merge sort (recursive)
- File save/load functionality
- Time-stamped patient records
- 2D array statistics (priority vs age groups)

## Concepts Used

- Structs and pointers
- Dynamic arrays
- Sorting and searching algorithms
- Recursion
- File handling
- Input validation
- Time handling in C

## How to Compile
```bash
gcc main.c -o hospital
```

## How to Run
```bash
./hospital
```
## Author
**Moaaz Mohamed**
Computer Engineering - AAST

## Example Output
```
=========== Hospital Patient Queue System ===========
1) Add Patient
2) Serve Next Patient
3) Display Queue
4) Search (Binary by ID / Linear by Name)
5) Stats (2D Array report)
6) Save to File
7) Load from File
0) Exit
=====================================================
Choose: 1
Enter ID (1..999999): 101
Enter Name: Moaaz
Enter Age (0..120): 20
Enter Priority (1=critical .. 5=low): 2
Enter Diagnosis: Checkup
Patient added to queue.

=========== Hospital Patient Queue System ===========
1) Add Patient
2) Serve Next Patient
3) Display Queue
4) Search (Binary by ID / Linear by Name)
5) Stats (2D Array report)
6) Save to File
7) Load from File
0) Exit
=====================================================
Choose: 3
--- Current Queue (Priority Order) ---
ID   | Name  | Age | Priority | Diagnosis | Added
101  | Moaaz | 20  | 2        | Checkup   | Fri Dec 27 10:15:30 2025
```
