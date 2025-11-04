#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Function to merge two sorted arrays
void merge(int *array, int start, int mid, int end) {
    int n1 = mid - start + 1;
    int n2 = end - mid;

    int *left = (int *)malloc(n1 * sizeof(int));
    int *right = (int *)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++)
        left[i] = array[start + i];
    for (int i = 0; i < n2; i++)
        right[i] = array[mid + 1 + i];

    int i = 0, j = 0, k = start;
    while (i < n1 && j < n2) {
        if (left[i] <= right[j])
            array[k++] = left[i++];
        else
            array[k++] = right[j++];
    }

    while (i < n1)
        array[k++] = left[i++];

    while (j < n2)
        array[k++] = right[j++];

    free(left);
    free(right);
}

// Recursive Merge Sort function
void merge_sort(int *array, int start, int end) {
    if (start < end) {
        int mid = start + (end - start) / 2;
        merge_sort(array, start, mid);
        merge_sort(array, mid + 1, end);
        merge(array, start, mid, end);
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    double mpi_start_time, mpi_end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 100000000; // Array size (adjustable for testing)
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
    int chunk_size = n / size;
    int *sub_array = (int *)malloc(chunk_size * sizeof(int));

    MPI_Scatter(array, chunk_size, MPI_INT, sub_array, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Each process sorts its chunk sequentially
    merge_sort(sub_array, 0, chunk_size - 1);

    // Gather sorted chunks back to the root process
    MPI_Gather(sub_array, chunk_size, MPI_INT, array, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Master process merges the sorted chunks
    if (rank == 0) {
        int step = chunk_size;
        while (step < n) {
            for (int i = 0; i < n; i += 2 * step) {
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

        free(array);
    }

    free(sub_array);
    MPI_Finalize();
    return 0;
}
