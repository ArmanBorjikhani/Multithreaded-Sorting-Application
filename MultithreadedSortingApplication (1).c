/*======================================================
  Sort an integer array using two worker threads and one merger thread (pthread).
  Build:   gcc -pthread -o msort multithreaded_sort.c
  Usage:   ./msort "3 5 2 1 9 7 4 6 8"  
=======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 256

int  data[MAX_SIZE];    
int  sorted[MAX_SIZE];   
size_t n;                

[lo, hi] (includes both endpoints) */
static void insertion_sort(int *arr, size_t lo, size_t hi)
{
    for (size_t i = lo + 1; i <= hi; ++i) {
        int key = arr[i];
        size_t j = i;
        while (j > lo && arr[j - 1] > key) {
            arr[j] = arr[j - 1];
            --j;
        }
        arr[j] = key;
    }
}

struct sort_args { size_t lo, hi; };

void *sort_worker(void *arg)
{
    struct sort_args *a = arg;
    insertion_sort(data, a->lo, a->hi);
    pthread_exit(NULL);
}

struct merge_args { size_t mid; };

void *merge_worker(void *arg)
{
    struct merge_args *m = arg;
    size_t i = 0, j = m->mid, k = 0;
    while (i < m->mid && j < n)
        sorted[k++] = (data[i] <= data[j]) ? data[i++] : data[j++];
    while (i < m->mid) sorted[k++] = data[i++];
    while (j < n)      sorted[k++] = data[j++];
    pthread_exit(NULL);
}

static void parse_input(int argc, char *argv[])
{
    n = 0;
    for (int i = 1; i < argc && n < MAX_SIZE; ++i)
        data[n++] = atoi(argv[i]);
    if (n == 0) {
        fprintf(stderr, "Please supply a list of integers.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    parse_input(argc, argv);

    size_t mid = n / 2;

    pthread_t tid_sort1, tid_sort2, tid_merge;
    struct sort_args a1 = {0, mid - 1};
    struct sort_args a2 = {mid, n - 1};
    struct merge_args m = {mid};

    pthread_create(&tid_sort1, NULL, sort_worker, &a1);
    pthread_create(&tid_sort2, NULL, sort_worker, &a2);

    pthread_join(tid_sort1, NULL);
    pthread_join(tid_sort2, NULL);

    pthread_create(&tid_merge, NULL, merge_worker, &m);
    pthread_join(tid_merge, NULL);

    /* parent prints */
    printf("Sorted result: ");
    for (size_t i = 0; i < n; ++i)
        printf("%d%s", sorted[i], (i + 1 == n) ? "\n" : " ");

    return 0;
}
