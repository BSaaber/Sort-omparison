#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// type of generating array
typedef enum ArrayType {
        RANDOM = 0, // random
        SORTED = 1, // from min to max
        ANTISORTED = 2 // from max to min
} arrayType;

// result of testing function
typedef enum TestRes {
        GOOD = 0,
        BAD = 1
} testRes;

//compare two elements, update counter
static int compare_a_less_b(long long int a, long long int b, int *counter){
    *counter +=1;
    if (a < b){
        return 1;
    }
    return 0;
}

//swap two elements, update counter
static void swap(long long int *a, long long int *b, int *move_counter){
    long long int t = *a;
    *a = *b;
    *b = t;
    *move_counter += 3;
}

static void selection_sort(int n, long long int *array){
    int counter = 0;
    int swap_counter = 0;
    long long int tmin;
    int tmin_index;
    for (int i = 0; i < n - 1; i++){
        tmin = array[i];
        swap_counter += 1;
        tmin_index = i;
        // find min element in not sorted part of array
        for (int j = i + 1; j < n; j++){
            if (compare_a_less_b(array[j], tmin, &counter)){
                tmin = array[j];
                swap_counter += 1;
                tmin_index = j;
            }
        }
        // swap elements
        array[tmin_index] = array[i];
        array[i] = tmin;
        swap_counter += 2;

    }
    printf("Selection sort. Array size: %d. Number of comparisons: %d. Number of swaps: %d.\n", n, counter, swap_counter);
}

// based on lecture materials [ https://contest.solutions/public-archives/algcourse-2021/hse/video/lection11.mp4 ]
static void quicksort(long long int *a, int left, int right, int *swap_counter, int *cmp_counter){
    int i, j;
    i = left;
    j = right;
    long long int comp = a[(left + right)/2];
    *swap_counter += 1;
    do {
        //find first element  bigger than comparator
        while (compare_a_less_b(a[i], comp, cmp_counter) && i < right){
            i++;
        }
        // find last element smaller than comparator
        while(compare_a_less_b(comp, a[j], cmp_counter) && j > left){
            j--;
        }
        // swap them if not the end of  iteration
        if (i <= j) {
            swap(&a[i], &a[j], swap_counter);
            i++;
            j--;
        }
    }
    // go until arr is devided on two parts - less and bigger than comparator
    while (i <= j);
    if (left < j){
        // make same with left part
        quicksort(a, left, j, swap_counter, cmp_counter);
    }
    if(i < right){
        // make same with right part
        quicksort(a, i, right, swap_counter, cmp_counter);
    }
}

// just a form to start recursion quick sort with start parameters
static void quick_sort(int n, long long int *a){
    int swap_counter = 0;
    int cmp_counter = 0;
    quicksort(a, 0, n - 1, &swap_counter, &cmp_counter);
    printf("Quick sort. Array size: %d. Number of comparisons: %d. Number of swaps: %d.\n", n, cmp_counter, swap_counter);
}

static int test(int n, long long int *start_array, long long int *sorted_array){
    for (int i = 0; i < n - 1; i++){
        // error if sorted array is not sorted
        if (sorted_array[i] > sorted_array[i + 1]){
            return BAD;
        }
    }
    int *check_array = (int*) malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++){
        // array of zeros with n elements
        check_array[i] = 0;
    }

    int flag;
    // for elements in sorted
    for (int i = 0; i < n; i++){
        flag = 1;
        for (int j = 0; j < n; j++){
            // if there is such an element in original and it is not marked
            if (check_array[j] == 0 && sorted_array[i] == start_array[j]){
                // lower the flag and mark element in original
                flag = 0;
                check_array[j] = 1;
                break;
            }
        }
        // error if there is no unmarked elements in original equals to current element in sorted
        if (flag == 1){
            free(check_array);
            return BAD;
        }
    }
    free(check_array);
    return GOOD;
}

static void generate_array(int size, long long int *array, int seed, arrayType type){

    unsigned long long int t;
    unsigned long long int tt = 0;
    if (type == RANDOM){
        srand (seed+time(NULL));
        for (int i = 0; i < size; i++){
            for (int j = 0; j < 64; j++){
                t = rand() % 2;
                t = t << j;
                tt = tt | t;
            }
            array[i] = (long long int) tt;
        }
    }
    // because difference between elements doesn t make sense in comparisons, let s make row of numbers
    else if (type == SORTED){
        for (int i = 0; i < size; i++){
            array[i] = i;
        }
    }
    else{ // or in reversed way
        for (int i = 0; i < size; i++){
            array[size - i - 1] = i;
        }
    }
}
//debugging function
/*
static void print_array(int size, long long int *arr){
    for (int i = 0; i < size; i++){
        printf("%ld ", (long int) arr[i]);
    }
    printf("\n");
}
*/



int main(void) {
   int n;
   // input array size
   if (scanf("%d", &n) == 0 || n < 1){
        printf("ERROR: BAD ARRAY SIZE");
        return -1;
   }
   int arr_amount = 4;
   // arr of 4 generated arrays
   long long int (*originals)[n];
   originals = (long long int (*)[n]) malloc(sizeof(long long int*) * arr_amount * n);
   // arr of copies of arrays
   long long int (*copies)[n];
   copies = (long long int (*)[n]) malloc(sizeof(long long int*) * arr_amount * n);

   int seed = 1;

   //generating arrays
   generate_array(n, originals[0], -1, SORTED);
   generate_array(n, originals[1], -1, ANTISORTED);
   generate_array(n, originals[2], seed++, RANDOM);
   generate_array(n, originals[3], seed++, RANDOM);
   // making copies of arrays
   for (int i = 0; i < arr_amount; i++){
        memcpy(copies[i], originals[i], sizeof(long long int) * n);
   }

   //sorting copies by selection sort
   for (int i = 0; i < arr_amount; i++){
        selection_sort(n,copies[i]);
        // print error and  stop the program if something goes wrong
        if (test(n, originals[i], copies[i]) == BAD){
            printf("SELECTION SORT ERROR: sorted array doesn't match the original.\n");
            printf("original array:\n");
            //print_array(n, originals[i]);
            printf("sorted array:\n");
            //print_array(n, copies[i]);
            goto stop;
        }
        else {
            printf("Sort is correct.\n");
        }

   }
   printf("\n");

   //recopy arrs from originals
   for (int i = 0; i < arr_amount; i++){
        memcpy(copies[i], originals[i], sizeof(long long int) * n);
   }

   //sorting copies by quick sort
   for (int i = 0; i < arr_amount; i++){
        quick_sort(n, copies[i]);
        // print error and  stop the program if something goes wrong
        if (test(n, originals[i], copies[i]) == BAD){
            printf("SELECTION ERROR: sorted array doesn't match the original.\n");
            printf("original array:\n");
            //print_array(n, originals[i]);
            printf("sorted array:\n");
            //print_array(n, copies[i]);
            goto stop;
        }
        else {
            printf("Sort is correct.\n");
        }
   }

   stop:
   // free memory
   free(copies);
   free(originals);
   return 0;
}
