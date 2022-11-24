#include <chrono>
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
#include <vector>
using namespace std;


const int selectionSortLimit = 5;


// Selection Sorting when the parition size in mergeSort operation < 5
void selectionSort(int array[], int low, int high) {
    int minimumIndex, i, j;
    for (i = low; i <= high; i++) {
        minimumIndex = i;
        for (j = i + 1; j <= high; j++) {
            if (array[j] < array[minimumIndex])
                minimumIndex = j;
        }
        swap(array[minimumIndex], array[i]);
    }
}


// Merge operation in Merge-Sorting
void merge(int array[], int low, int mid, int high) {
    int i = low, j = mid + 1, k = 0, num1 = mid - low + 1, num2 = high - mid;
    int tempArray[high - low + 1];
    while (i <= mid && j <= high) {
        if (array[i] < array[j])
            tempArray[k++] = array[i++];
        else
            tempArray[k++] = array[j++];
    }
    while (i <= mid)
        tempArray[k++] = array[i++];
    while (j <= high)
        tempArray[k++] = array[j++];
    for (i = low; i <= high; i++)
        array[i] = tempArray[i - low];
}


// Merge-Sorting
void mergeSort(int array[], int low, int high) {
    // Default case
    if (low >= high)
        return;
    if (high - low + 1 < selectionSortLimit) {
        selectionSort(array, low, high);
        return;
    }

    // calling fork upon the left and right parts of the array
    int mid = low + (high - low) / 2, leftChild, rightChild, status = 0;
    leftChild = fork();
    if (leftChild < 0) {
        cout << "Error in Fork for leftChild!!" << endl;
        exit(-1);
    }
    else if (!leftChild) {
        mergeSort(array, low, mid);
        exit(0);
    }
    else {
        rightChild = fork();
        if (rightChild < 0) {
            cout << "Error in Fork for rightChild!!" << endl;
            exit(-1);
        }
        else if (!rightChild) {
            mergeSort(array, mid + 1, high);
            exit(0);
        }
    }

    // waiting for the children processes to finish
    waitpid(leftChild, &status, 0);
    waitpid(rightChild, &status, 0);

    // merging the sorted left and right partitions
    merge(array, low, mid, high);
}


int main() {
    const clock_t begin_time = clock();
    int i, arraySize, maxValue = 1e6, shmID, num;
    key_t k = IPC_PRIVATE;
    int* shmArray;
    vector<int> v;
    cin >> arraySize;

    // Calculating segment length and creating the segment
    size_t shmSize = sizeof(int) * arraySize;
    if ((shmID = shmget(k, shmSize, IPC_CREAT | 0666)) < 0) {
        cout << "Error while doing shmget!!" << endl;
        exit(1);
    }

    // Attaching the segment to the data space
    if ((shmArray = (int*)shmat(shmID, NULL, 0)) == (int*)-1) {
        cout << "Error while doing shmat!!" << endl;
        exit(1);
    }

    // Assigning random numbers to the input array
    mt19937 rand(chrono::steady_clock::now().time_since_epoch().count());
    for (i = 0; i < arraySize; i++) {
        num = rand() % maxValue;
        shmArray[i] = num;
        v.push_back(num);
    }
    
    // Crux of the question
    mergeSort(shmArray, 0, arraySize - 1);
    
    // Verification that the sorting actually happened
    sort(v.begin(), v.end());
    for (i = 0; i < arraySize; i++) {
        if (v[i] != shmArray[i]) {
            cout << v[i] << " != " << shmArray[i] << ", Sorting didn't happen correctly!!" << endl;
            exit(1);
        }
        //cout << shmArray[i] << ' ';
    }
    cout << endl;

    // Detatching from the shared memory
    if (shmdt(shmArray) == -1) {
        cout << "Error in shmdt!!" << endl;
        exit(1);
    }

    // Deleting the shared memory
    if (shmctl(shmID, IPC_RMID, NULL) == -1) {
        cout << "Error in shmctl!!" << endl;
        exit(1);
    }
    
    // Calculating the time taken till mergesort completion
    cout << "Time elapsed : " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " seconds." << endl;
}
