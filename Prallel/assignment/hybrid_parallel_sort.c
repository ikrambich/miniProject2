#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

// Function to merge two sorted arrays
void merge(int *array, int start, int mid, int end) {
    int n1 = mid - start + 1; // Size of the left subarray
    int n2 = end - mid;       // Size of the right subarray

    // Allocate memory for left and right subarrays
    int *left = (int *)malloc(n1 * sizeof(int));
    int *right = (int *)malloc(n2 * sizeof(int));

    // Copy elements to the subarrays
    for (int i = 0; i < n1; i++)
        left[i] = array[start + i];
    for (int i = 0; i < n2; i++)
        right[i] = array[mid + 1 + i];

    int i = 0, j = 0, k = start;
    // Merge the two subarrays back into the original array
    while (i < n1 && j < n2) {
        if (left[i] <= right[j])
            array[k++] = left[i++];
        else
            array[k++] = right[j++];
    }

    // Copy any remaining elements from the left subarray
    while (i < n1)
        array[k++] = left[i++];

    // Copy any remaining elements from the right subarray
    while (j < n2)
        array[k++] = right[j++];

    // Free dynamically allocated memory
    free(left);
    free(right);
}

// Recursive Merge Sort function
void merge_sort(int *array, int start, int end) {
    if (start < end) {
        int mid = start + (end - start) / 2;

        // Parallelize sorting using OpenMP
        #pragma omp parallel sections
        {
            // Sort the left half
            #pragma omp section
            merge_sort(array, start, mid);

            // Sort the right half
            #pragma omp section
            merge_sort(array, mid + 1, end);
        }

        // Merge the two sorted halves
        merge(array, start, mid, end);
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    double mpi_start_time, mpi_end_time; // Timing for MPI
    double omp_start_time, omp_end_time; // Timing for OpenMP

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get the rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get the total number of processes

    int n = 100000000; // Array size (adjustable for larger tests)
    int *array = NULL;

    // Master process initializes the array
    if (rank == 0) {
        array = (int *)malloc(n * sizeof(int));
        printf("Original Array: ");
        for (int i = 0; i < n; i++) {
            array[i] = rand() % 100; // Generate random numbers
            printf("%d ", array[i]);
        }
        printf("\n");
    }

    // Start MPI timing
    mpi_start_time = MPI_Wtime();

    // Scatter data to all processes
    int chunk_size = n / size; // Determine the size of each chunk
    int *sub_array = (int *)malloc(chunk_size * sizeof(int));

    // Divide the array among all processes
    MPI_Scatter(array, chunk_size, MPI_INT, sub_array, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Start OpenMP timing
    omp_start_time = omp_get_wtime();

    // Each process sorts its chunk using OpenMP
    #pragma omp parallel
    {
        #pragma omp single
        merge_sort(sub_array, 0, chunk_size - 1);
    }

    // End OpenMP timing
    omp_end_time = omp_get_wtime();

    // Gather sorted chunks back to the root process
    MPI_Gather(sub_array, chunk_size, MPI_INT, array, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Master process merges the sorted chunks
    if (rank == 0) {
        int step = chunk_size;
        while (step < n) {
            int i;
            // Parallel merge of chunks using OpenMP
            #pragma omp parallel for private(i)
            for (i = 0; i < n; i += 2 * step) {
                int mid = i + step - 1;
                int end = (i + 2 * step - 1 < n) ? (i + 2 * step - 1) : (n - 1);
                merge(array, i, mid, end);
            }
            step *= 2;
        }

        // End MPI timing
        mpi_end_time = MPI_Wtime();

        // Print the sorted array
        printf("Sorted Array: ");
        for (int i = 0; i < n; i++)
            printf("%d ", array[i]);
        printf("\n");

        // Print timing results
        printf("MPI Total Time: %f seconds\n", mpi_end_time - mpi_start_time);
        printf("OpenMP Sorting Time: %f seconds\n", omp_end_time - omp_start_time);
        printf("Total Execution Time: %f seconds\n", (mpi_end_time - mpi_start_time));
    }

    // Free allocated memory
    free(sub_array);
    if (rank == 0)
        free(array);

    // Finalize MPI
    MPI_Finalize();
    return 0;
}
