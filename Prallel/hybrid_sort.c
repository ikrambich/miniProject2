#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Merge function to merge two sorted arrays
void merge(int *array, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++)
        L[i] = array[left + i];
    for (int i = 0; i < n2; i++)
        R[i] = array[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            array[k] = L[i];
            i++;
        } else {
            array[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        array[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        array[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

// Merge sort function
void merge_sort(int *array, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        merge_sort(array, left, mid);
        merge_sort(array, mid + 1, right);

        merge(array, left, mid, right);
    }
}

// Parallel merge sort using OpenMP
void parallel_merge_sort(int *array, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

#pragma omp parallel sections
        {
#pragma omp section
            merge_sort(array, left, mid);
#pragma omp section
            merge_sort(array, mid + 1, right);
        }

        merge(array, left, mid, right);
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    int *array = NULL;
    int *chunk = NULL;
    int array_size = 1000000;  // Adjust as needed
    int chunk_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    chunk_size = array_size / size;

    if (rank == 0) {
        // Master process generates the array
        array = (int *)malloc(array_size * sizeof(int));
        srand(time(NULL));
        for (int i = 0; i < array_size; i++) {
            array[i] = rand() % 1000000;
        }
    }

    // Scatter chunks of the array to all processes
    chunk = (int *)malloc(chunk_size * sizeof(int));
    MPI_Scatter(array, chunk_size, MPI_INT, chunk, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Sort the chunk using OpenMP
    double start_time = MPI_Wtime();
    parallel_merge_sort(chunk, 0, chunk_size - 1);
    double sort_time = MPI_Wtime() - start_time;

    // Gather sorted chunks at the master process
    MPI_Gather(chunk, chunk_size, MPI_INT, array, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Master process performs the final merge
        start_time = MPI_Wtime();
        for (int i = 1; i < size; i++) {
            merge(array, 0, i * chunk_size - 1, (i + 1) * chunk_size - 1);
        }
        double merge_time = MPI_Wtime() - start_time;

        printf("Hybrid Parallel Sorting Completed!\n");
        printf("Time for sorting chunks (OpenMP): %.2f seconds\n", sort_time);
        printf("Time for final merge (MPI): %.2f seconds\n", merge_time);

        free(array);
    }

    free(chunk);
    MPI_Finalize();
    return 0;
}
