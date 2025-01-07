**Quicksort with Multithreading**

This project explores the implementation and performance comparison of a standard quicksort algorithm and a multithreaded version using pthreads.

**Features:**

- **Quicksort Implementation:**
    - Implements a standard, non-threaded quicksort algorithm.
    - Implements a multithreaded quicksort algorithm using pthreads for parallel execution.
- **Performance Comparison:**
    - Measures and compares the execution time of both non-threaded and threaded quicksort.
    - Provides insights into the performance benefits of multithreading for this sorting algorithm.
- **File Input:**
    - Reads input data from a specified file containing a list of integers.
- **Optional Printing:**
    - Allows the user to print the unsorted and sorted lists using the `-p` command-line option.

**Usage:**

```bash
./quicksort [-p] <filename.txt>
```

- `<filename.txt>`: The path to the file containing the integers to be sorted.
- `-p`: Optional flag to print the unsorted and sorted lists.

**Compilation:**

1. Save the code in files named `quicksort.c` and `quicksort.h` (if applicable).
2. Compile the code using a C compiler with appropriate flags:

   ```bash
   gcc -o quicksort quicksort.c -lpthread 
   ```

**Project Structure:**

- `quicksort.c`: Contains the main function and the implementation of the quicksort algorithms.
- `quicksort.h` (optional): Contains any necessary header files or function prototypes.
- `README.md`: This file.

**How it Works:**

1. **Input:** Reads integers from the specified file into an array.
2. **Non-threaded Quicksort:** 
   - Performs the standard quicksort algorithm on a copy of the input data.
   - Measures the execution time.
3. **Threaded Quicksort:**
   - Creates a separate thread for each recursive call to quicksort for the "less than" and "greater than" partitions.
   - Joins the threads to wait for their completion.
   - Merges the sorted subarrays to obtain the final sorted array.
   - Measures the execution time.
4. **Output:** 
   - Prints the execution times for both non-threaded and threaded quicksort.
   - Optionally prints the unsorted and sorted lists if the `-p` flag is used.

**Key Considerations:**

- **Thread Synchronization:** Uses a mutex to protect the `thread_count` variable, ensuring accurate thread counting.
- **Memory Management:** Dynamically allocates memory for arrays and frees it appropriately to avoid memory leaks.
- **Error Handling:** Includes basic error handling for file opening, memory allocation, and invalid command-line arguments.

**Note:**

- The performance of the threaded quicksort will depend on factors such as the number of cores available, the size of the input data, and the overhead of thread creation and synchronization.
- For very small datasets, the overhead of creating and managing threads may outweigh the benefits of parallelism.

This README provides a basic overview of the project. You can further enhance it by adding more details about the specific implementation choices, performance analysis, and potential improvements.

I hope this README is helpful!
