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
int* array1;
int* array2;
const int selectionSortLimit = 5;

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

void merge(int low, int mid, int high) {
    int num1 = mid - low + 1, num2 = high - mid, i, j, k;
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
    time_t startNow, endNow;
    time(&startNow);
    int i, arraySize, minValue = 1, maxValue = 1e6;
    bool flag = 1;
    cin >> arraySize;
    inputArray = (int*)malloc(sizeof(int) * (arraySize + 1));
    array1 = (int*)malloc(sizeof(int) * (arraySize + 1));
    array2 = (int*)malloc(sizeof(int) * (arraySize + 1));
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(minValue, maxValue);
    for (i = 0; i < arraySize; i++)
        inputArray[i] = distribution(generator);
    mergeSort(0, arraySize - 1);
    time(&endNow);
    double timeTaken = double(endNow - startNow);
    cout << "Time taken : " << fixed << timeTaken << setprecision(3) << " seconds." << endl;
}
