#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;


int* inputArray;
const int selectionSortLimit = 5;


// When partition size is < 5
void selectionSort(int low, int high) {
    int minimumIndex, i, j;
    for (i = low; i <= high; i++) {
        minimumIndex = i;
        for (j = i + 1; j <= high; j++) {
            if (inputArray[minimumIndex] > inputArray[j])
                minimumIndex = j;
        }
        swap(inputArray[minimumIndex], inputArray[i]);
    }
}


// Merging both the partitions
void merge(int low, int mid, int high) {
    int num1 = mid - low + 1, num2 = high - mid, i, j, k;
    int array1[num1], array2[num2];
    for (i = 0; i < num1; i++)
        array1[i] = inputArray[low + i];
    for (j = 0; j < num2; j++)
        array2[j] = inputArray[mid + j + 1];
    i = 0;
    j = 0;
    k = low;
    while (i < num1 && j < num2) {
        if (array1[i] < array2[j])
            inputArray[k++] = array1[i++];
        else
            inputArray[k++] = array2[j++];
    }
    while (i < num1)
        inputArray[k++] = array1[i++];
    while (j < num2)
        inputArray[k++] = array2[j++];
}


// Merge Sort procedure
void mergeSort(int low, int high) {
    if (low >= high)
        return;
    if (high - low + 1 < selectionSortLimit) {
        selectionSort(low, high);
        return;
    }
    int mid = low + (high - low) / 2;
    mergeSort(low, mid);
    mergeSort(mid + 1, high);
    merge(low, mid, high);
}


int main() {
    // Generic variable declaration and starting the clock
    const clock_t begin_time = clock();
    int i, arraySize, minValue = 1, maxValue = 1e6, num;
    bool flag = 1;
    vector<int> v;
    cin >> arraySize;
    inputArray = (int*)malloc(sizeof(int) * (arraySize + 1));
    
    // Assigning random numbers to the input array
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(minValue, maxValue);
    for (i = 0; i < arraySize; i++) {
        num = distribution(generator);
        v.push_back(num);
        inputArray[i] = num;
    }

    // CRUX of the question
    mergeSort(0, arraySize - 1);

    // Sorting verification
    sort(v.begin(), v.end());
    for (i = 0; i < arraySize; i++) {
        if (v[i] != inputArray[i]) {
            cout << v[i] << " != " << inputArray[i] << ", not sorted!!" << endl;
            exit(1);
        }
    }
    
    // Measuring the time elapsed
    cout << "Time elapsed : " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " seconds." << endl;
}
