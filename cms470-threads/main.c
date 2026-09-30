#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// struct so i can pass the array and its size as one argument
struct sortingParams {
  double *arr;
  int size;
};

// struct for the merge thread, needs both halves + where to put the result
struct mergingParams {
  double *left;
  int leftSize;
  double *right;
  int rightSize;
  double *dest;
};

// swaps two values in the array
void swap(double *a, double *b) {
  double temp = *a;
  *a = *b;
  *b = temp;
}

// selection sort, find the smallest in the unsorted part and swap it to the front
void selectionSort(double *arr, int size) {
  for (int i = 0; i < size - 1; i++) {
    int minIndex = i;
    for (int j = i + 1; j < size; j++) {
      if (arr[j] < arr[minIndex]) {
        minIndex = j;
      }
    }
    swap(&arr[i], &arr[minIndex]);
  }
}

// the function the sorting threads run
// it unpacks the struct then just calls selectionSort
void *sortThread(void *arg) {
  struct sortingParams *params = (struct sortingParams *)arg;
  selectionSort(params->arr, params->size);
  return NULL;
}

// the function the merge thread runs
// goes through both sorted halves at the same time and always takes
// whichever front value is smaller, so its O(n) and not another sort
void *mergeThread(void *arg) {
  struct mergingParams *params = (struct mergingParams *)arg;
  int i = 0; // spot in left half
  int j = 0; // spot in right half
  int k = 0; // spot in dest

  while (i < params->leftSize && j < params->rightSize) {
    if (params->left[i] <= params->right[j]) {
      params->dest[k] = params->left[i];
      i++;
    } else {
      params->dest[k] = params->right[j];
      j++;
    }
    k++;
  }
  // one half ran out, copy the rest of the other one over
  while (i < params->leftSize) {
    params->dest[k] = params->left[i];
    i++;
    k++;
  }
  while (j < params->rightSize) {
    params->dest[k] = params->right[j];
    j++;
    k++;
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  // make sure the user actually gave us a size
  if (argc != 2) {
    printf("Usage: %s <array size>\n", argv[0]);
    return 0;
  }
  int n = atoi(argv[1]);
  if (n <= 0) {
    printf("Error: array size needs to be a positive number\n");
    return 0;
  }

  struct timespec ts_begin, ts_end;
  double elapsed;

  // seed so the random numbers are the same every run (for testing only)

  // set up all the arrays
  double *A = malloc(n * sizeof(double));          
  double *B = malloc(n * sizeof(double));          
  double *C = malloc(n * sizeof(double));          
  int half = n / 2;
  double *firstHalf = malloc(half * sizeof(double));
  double *secondHalf = malloc((n - half) * sizeof(double)); // n - half so odd sizes work

  // fill A with random doubles between 1.0 and 1000.0
  for (int i = 0; i < n; i++) {
    A[i] = ((double)rand() / RAND_MAX) * 999.0 + 1.0;
  }

  //1 thread case
  // copy A into B - we sort a the same
  for (int i = 0; i < n; i++) {
    B[i] = A[i];
  }

  clock_gettime(CLOCK_MONOTONIC, &ts_begin);

  struct sortingParams paramsB = {B, n};
  pthread_t threadB;
  pthread_create(&threadB, NULL, sortThread, &paramsB);
  pthread_join(threadB, NULL); // wait for it to finish

  clock_gettime(CLOCK_MONOTONIC, &ts_end);
  elapsed = ts_end.tv_sec - ts_begin.tv_sec;
  elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) / 1000000000.0;

  printf("Sorting is done in %.1fms when one thread is used\n", elapsed * 1000);

  //TWO THREADS CASE 
  // copy the first half of A into firstHalf and the rest into secondHalf
  for (int i = 0; i < half; i++) {
    firstHalf[i] = A[i];
  }
  for (int i = half; i < n; i++) {
    secondHalf[i - half] = A[i];
  }

  clock_gettime(CLOCK_MONOTONIC, &ts_begin);

  // start both sorting threads first THEN join both,
  struct sortingParams params1 = {firstHalf, half};
  struct sortingParams params2 = {secondHalf, n - half};
  pthread_t threadA1, threadA2;
  pthread_create(&threadA1, NULL, sortThread, &params1);
  pthread_create(&threadA2, NULL, sortThread, &params2);
  pthread_join(threadA1, NULL);
  pthread_join(threadA2, NULL);

  // both halves are sorted now, so merge them into C
  struct mergingParams paramsM = {firstHalf, half, secondHalf, n - half, C};
  pthread_t threadM;
  pthread_create(&threadM, NULL, mergeThread, &paramsM);
  pthread_join(threadM, NULL);

  clock_gettime(CLOCK_MONOTONIC, &ts_end);
  elapsed = ts_end.tv_sec - ts_begin.tv_sec;
  elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) / 1000000000.0;

  printf("Sorting is done in %.1fms when two threads are used\n", elapsed * 1000);

  // clean up
  free(A);
  free(B);
  free(C);
  free(firstHalf);
  free(secondHalf);
  return 0;
}