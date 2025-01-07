/*
* @author   Jatin Jain
* @file     quicksort.c
* @desc     this is the implementation of quick sort for an array of numbers read from a .txt file, the implementation compares the results and time complexity of threaded quick sort and a non=-threaded quicksort. 
* @usage    ./quicksort [-p] <filename.txt>
* @date     6 december 2024
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

//global values
int thread_count = 0;
pthread_mutex_t thread_count_mutex = PTHREAD_MUTEX_INITIALIZER;


typedef struct {
    int *data;
    size_t size;
} ThreadArgs;

/**
 * @brief Partitions an array into three subarrays based on a pivot value.
 * 
 * This function divides the input array into three separate subarrays:
 * 1. Elements less than the pivot.
 * 2. Elements equal to the pivot.
 * 3. Elements greater than the pivot.
 * 
 * Memory is dynamically allocated for the resulting subarrays, and pointers
 * to these arrays are returned to the caller. It is the caller's responsibility
 * to free the allocated memory.
 * 
 * @param[in] arr        Pointer to the input array of integers.
 * @param[in] size       The number of elements in the input array.
 * @param[in] pivot      The pivot value used for partitioning.
 * @param[out] less      Pointer to an integer pointer that will point to the array of elements less than the pivot.
 * @param[out] less_size Pointer to a size_t variable where the number of elements in the "less" array will be stored.
 * @param[out] equal     Pointer to an integer pointer that will point to the array of elements equal to the pivot.
 * @param[out] equal_size Pointer to a size_t variable where the number of elements in the "equal" array will be stored.
 * @param[out] more      Pointer to an integer pointer that will point to the array of elements greater than the pivot.
 * @param[out] more_size Pointer to a size_t variable where the number of elements in the "more" array will be stored.
 * 
 * @return int Returns 0 on success, or -1 if memory allocation fails.
 */
int partition(int *arr,size_t size, int pivot, int **less, size_t *less_size,int **equal, size_t *equal_size, int **more, size_t *more_size) {
    
    int* less_arr = (int*)malloc(size * sizeof(int));
    int* more_arr = (int*)malloc(size * sizeof(int));
    int* equal_arr = (int*)malloc(size * sizeof(int));

    if(!less_arr || !more_arr || !equal_arr) {
        fprintf(stderr,"Exit Code: Failed to allocate memory");
        return -1;
    }

    size_t less_count = 0, more_count = 0, equal_count = 0;
    for(size_t i = 0; i < size; i++){
        if(arr[i] < pivot) less_arr[less_count++] = arr[i];
        else if(arr[i] > pivot) more_arr[more_count++] = arr[i];
        else equal_arr[equal_count++] = arr[i];
    }

    *less = less_arr;
    *equal = equal_arr;
    *more = more_arr;
    *less_size = less_count;
    *more_size = more_count;
    *equal_size = equal_count;
    return 0;
}

/**
 * @brief Merges three arrays into a single result array.
 * 
 * This function combines three partitions (`less`, `equal`, and `more`) into a 
 * single contiguous array (`result`) in the following order:
 * 1. Elements from the `less` array.
 * 2. Elements from the `equal` array.
 * 3. Elements from the `more` array.
 * 
 * The function assumes that the `result` array has enough allocated space to
 * store all the elements from the three input arrays.
 * 
 * @param[out] result      Pointer to the array where the merged result will be stored.
 * @param[in]  less        Pointer to the array containing elements in the "less" partition.
 * @param[in]  less_size   Number of elements in the "less" partition.
 * @param[in]  equal       Pointer to the array containing elements in the "equal" partition.
 * @param[in]  equal_size  Number of elements in the "equal" partition.
 * @param[in]  more        Pointer to the array containing elements in the "more" partition.
 * @param[in]  more_size   Number of elements in the "more" partition.
 */
void merge(int *result, const int *less, size_t less_size,
                      const int *equal, size_t equal_size,
                      const int *more, size_t more_size) {
    size_t index = 0;
    // Copy 'less' partition
    memcpy(result + index, less, less_size * sizeof(int));
    index += less_size;

    // Copy 'equal' partition
    memcpy(result + index, equal, equal_size * sizeof(int));
    index += equal_size;

    // Copy 'more' partition
    memcpy(result + index, more, more_size * sizeof(int));
}


/**
 * @brief Performs the quicksort algorithm on an array of integers.
 *
 * This function implements a recursive quicksort algorithm. It partitions
 * the input data into three subarrays based on a pivot value (less than,
 * equal to, and greater than the pivot), sorts the "less" and "more" arrays
 * recursively, and then merges the sorted results into a single sorted array.
 *
 * @param size The size of the input array.
 * @param data A pointer to the array of integers to be sorted.
 * @return A pointer to a newly allocated array containing the sorted elements,
 *         or NULL if the input size is zero or memory allocation fails.
 */
int *quicksort(size_t size, const int *data) {
    if (size == 0) return NULL;

    int pivot = data[0];
    int *less, *more, *equal;
    size_t less_size, more_size, equal_size;

    if (partition((int *)data, size, pivot, &less, &less_size,&equal,&equal_size, &more, &more_size) < 0)
        return NULL;

    int *sorted_less = quicksort(less_size, less);
    int *sorted_more = quicksort(more_size, more);

    int *result = malloc(size * sizeof(int));
    merge(result, sorted_less, less_size, equal, equal_size, sorted_more, more_size);
    

    free(less);
    free(more);
    free(equal);
    free(sorted_less);
    free(sorted_more);

    return result;
}


/**
 * @brief Threaded implementation of the quicksort algorithm.
 * 
 * This function performs a parallelized quicksort using pthreads to sort subarrays concurrently. 
 * It partitions the input data into three sections: less than the pivot, equal to the pivot, and greater than the pivot.
 * The less-than and greater-than partitions are sorted in separate threads. Afterward, these partitions are merged 
 * to form the final sorted array.
 *
 * @param args A pointer to a ThreadArgs structure that contains the array to sort and its size.
 * 
 * @return A pointer to the sorted array. NULL is returned if the partition fails or the size is zero.
 */
void* quicksort_threaded(void *args) {
    pthread_mutex_lock(&thread_count_mutex);
    thread_count++;
    pthread_mutex_unlock(&thread_count_mutex);

    ThreadArgs *input = (ThreadArgs *)args;
    size_t size = input->size;
    int *data = input->data;

    if (size == 0) return NULL;

    int pivot = data[0];
    int *less, *more, *equal;
    size_t less_size, more_size, equal_size;
    //checks if partition fails ad exits all threads.
    if (partition(data, size, pivot, &less, &less_size,&equal, &equal_size, &more, &more_size) < 0){
        pthread_exit(NULL);
    }

    ThreadArgs less_args = {less, less_size};
    ThreadArgs more_args = {more, more_size};

    pthread_t less_thread, more_thread;
    pthread_create(&less_thread, NULL, quicksort_threaded, &less_args);
    pthread_create(&more_thread, NULL, quicksort_threaded, &more_args);

    void *sorted_less;
    void *sorted_more;
    pthread_join(less_thread, &sorted_less);
    pthread_join(more_thread, &sorted_more);

    int *result = malloc(size * sizeof(int));
    merge(result, (int *)sorted_less, less_size, equal, equal_size, (int *)sorted_more, more_size);
    
    free(less);
    free(more);
    free(equal);
    free(sorted_less);
    free(sorted_more);
    return result; //returning the pointer to the result array
}


/** 
 * @brief Main function for sorting integers using both non-threaded and threaded quicksort.
 * 
 * This program reads integers from a file, performs non-threaded quicksort and threaded quicksort on the data, 
 * compares their execution times, and optionally prints the unsorted and sorted results if the "-p" flag is used.
 * 
 * Usage: 
 *   ./program [-p] <file_of_integers>
 * 
 * - If `-p` is provided, the program will print the unsorted and sorted lists.
 * - Reads integers from the provided input file and sorts them using two sorting methods.
 * 
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Returns 0 on successful execution, or 1 if an error occurs.
 */
int main(int argc, char *argv[]) {
    
    // Validate command-line arguments
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s [-p] file_of_integers\n", argv[0]);
        return 1;
    }

    int print_flag = 0; // Flag to determine if the program should print results
    char *filename;

    // Check if the '-p' option is used
    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        print_flag = 1;
        filename = argv[2];
    } else {
        filename = argv[1];
    }

    // Open file containing integers to sort
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Dynamically allocate memory to read integers from file
    int *data = NULL;
    size_t size = 0, capacity = 10;
    data = malloc(capacity * sizeof(int));
    if (!data) {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }

    // Read integers from file into dynamically allocated array
    while (fscanf(file, "%d", &data[size]) == 1) {
        size++;
        if (size == capacity) { // Resize memory when array is full
            capacity *= 2;
            data = realloc(data, capacity * sizeof(int));
            if (!data) {
                perror("Memory allocation failed");
                fclose(file);
                return 1;
            }
        }
    }
    fclose(file);

    // Print the unsorted list if print_flag is set
    if (print_flag) {
        printf("Unsorted list before non-threaded quicksort: ");
        for (size_t i = 0; i < size; i++) {
            printf("%d", data[i]);
            if (i < size - 1) printf(", ");
        }
        printf("\n");
    }

    clock_t start, end;

    // Perform non-threaded quicksort and measure its execution time
    start = clock();
    int *sorted_non_threaded = quicksort(size, data);
    end = clock();
    double non_threaded_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Non-threaded time:  %f\n", non_threaded_time);

    // Print the sorted list if print_flag is set
    if (print_flag) {
        printf("Resulting list: ");
        for (size_t i = 0; i < size; i++) {
            printf("%d", sorted_non_threaded[i]);
            if (i < size - 1) printf(", ");
        }
        printf("\n");
    }

    // Print the unsorted data before threaded quicksort if print_flag is set
    if (print_flag) {
        printf("Unsorted list before threaded quicksort: ");
        for (size_t i = 0; i < size; i++) {
            printf("%d", data[i]);
            if (i < size - 1) printf(", ");
        }
        printf("\n");
    }

    // Perform threaded quicksort and measure its execution time
    start = clock();
    ThreadArgs args = {data, size};
    int *sorted_threaded;
    pthread_t main_thread;

    // Start a new thread for threaded quicksort
    pthread_create(&main_thread, NULL, quicksort_threaded, &args);
    pthread_join(main_thread, (void **)&sorted_threaded);
    end = clock();
    double threaded_time = (double)(end - start) / CLOCKS_PER_SEC;

    // Count number of threads spawned during the execution
    extern int thread_count; // The global variable counting threads
    printf("Threaded time:      %f\n", threaded_time);
    printf("Threads spawned:    %d\n", thread_count);

    // Print the sorted threaded result if the print_flag is set
    if (print_flag) {
        printf("Resulting list: ");
        for (size_t i = 0; i < size; i++) {
            printf("%d", sorted_threaded[i]);
            if (i < size - 1) printf(", ");
        }
        printf("\n");
    }

    // Free dynamically allocated memory
    free(data);
    free(sorted_non_threaded);
    free(sorted_threaded);

    return 0;
}

