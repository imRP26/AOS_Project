#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

int* inputArray;
int* array1;
int* array2;
const int selectionSortLimit = 5;

void selectionSort(int* inputArray, int low, int high) {
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

void merge(int* shmArray, int low, int mid, int high) {
    int num1 = mid - low + 1, num2 = high - mid, i, j, k;
    for (i = 0; i < num1; i++)
        array1[i] = shmArray[low + i];
    for (j = 0; j < num2; j++)
        array2[j] = shmArray[mid + j + 1];
    i = 0;
    j = 0;
    k = low;
    while (i < num1 && j < num2) {
        if (array1[i] < array2[j])
            shmArray[k++] = array1[i++];
        else
            shmArray[k++] = array2[j++];
    }
    while (i < num1)
        shmArray[k++] = array1[i++];
    while (j < num2)
        shmArray[k++] = array2[j++];
}

void mergeSort(int* shmArray, int low, int high) {
    if (low >= high)
        return;
    if (high - low + 1 < selectionSortLimit) {
        selectionSort(shmArray, low, high);
        return;
    }
    int mid = low + (high - low) / 2, leftChild, rightChild, status;
    leftChild = fork();
    if (leftChild < 0) {
        cout << "Error in Fork for leftChild!!" << endl;
        exit(1);
    }
    else if (!leftChild) {
        mergeSort(shmArray, low, mid);
        exit(0);
    }
    else {
        rightChild = fork();
        if (rightChild < 0) {
            cout << "Error in Fork for rightChild!!" << endl;
            exit(1);
        }
        else if (!rightChild) {
            mergeSort(shmArray, mid + 1, high);
            exit(0);
        }
    }
    waitpid(leftChild, &status, 0);
    waitpid(rightChild, &status, 0);
    merge(shmArray, low, mid, high);
}

int main() {
    time_t startNow, endNow;
    time(&startNow);
    int i, arraySize, minValue = 1, maxValue = 1e6, id, matID;
    int* shmArray;
    key_t k;
    cin >> arraySize;
    inputArray = (int*)malloc(sizeof(int) * (arraySize + 1));
    array1 = (int*)malloc(sizeof(int) * (arraySize + 1));
    array2 = (int*)malloc(sizeof(int) * (arraySize + 1));
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(minValue, maxValue);
    for (i = 0; i < arraySize; i++)
        inputArray[i] = distribution(generator);
    k = IPC_PRIVATE;
    id = shmget(k, arraySize * sizeof(int), IPC_CREAT | 0666);
    if (id < 0) {
        cout << "Error in doing shmget!!" << endl;
        exit(1);
    }
    shmArray = (int*)shmat(id, NULL, 0);
    if (matID < 0) {
        cout << "Error in doing shmat!!" << endl;
        exit(1);
    }
    for (i = 0; i < arraySize; i++)
        shmArray[i] = inputArray[i];
    mergeSort(shmArray, 0, arraySize - 1);
    if (shmdt(shmArray) == -1) {
        cout << "Error in shmdt!!" << endl;
        exit(1);
    }
    if (shmctl(id, IPC_RMID, NULL) == -1) {
        cout << "Error in shmcelt!!" << endl;
        exit(1);
    }
    time(&endNow);
    double timeTaken = double(endNow - startNow);
    cout << "Time taken : " << fixed << timeTaken << setprecision(3) << " seconds." << endl;
}
