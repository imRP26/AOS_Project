#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <pthread.h>
#include <random>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
using namespace std;


struct threadNode {
    int leftIndex, rightIndex;
};
int* inputArray;


// When the partition size is < 5
void selectionSort(int low, int high) {
    int minimumIndex, i, j;
    for (i = low; i <= high; i++) {
        minimumIndex = i;
        for (j = i + 1; j <= high; j++) {
            if (inputArray[j] < inputArray[minimumIndex])
                minimumIndex = j;
        }
        swap(inputArray[i], inputArray[minimumIndex]);
    }
}


// Merge procedure of Merge Sort
void merge(int low, int mid, int high) {
    int i = low, j = mid + 1, k = 0;
    int tempArray[high - low + 1];
    while (i <= mid && j <= high) {
        if (inputArray[i] < inputArray[j])
            tempArray[k++] = inputArray[i++];
        else
            tempArray[k++] = inputArray[j++];
    }
    while (i <= mid)
        tempArray[k++] = inputArray[i++];
    while (j <= high)
        tempArray[k++] = inputArray[j++];
    for (i = low; i <= high; i++)
        inputArray[i] = tempArray[i - low];
}


// Merge Sort using multithreading 
void* mergeSort(void* temp) {
    threadNode* thrd = (threadNode*)temp;
    int returnValue, low = thrd->leftIndex, high = thrd->rightIndex, mid = low + (high - low) / 2;
    if (low >= high)
        return 0;
    if (high - low < 5) {
        selectionSort(low, high);
        return 0;
    }
    threadNode leftChild, rightChild;
    pthread_t tid1, tid2;
    leftChild.leftIndex = low;
    leftChild.rightIndex = mid;
    rightChild.leftIndex = mid + 1;
    rightChild.rightIndex = high;
    returnValue = pthread_create(&tid1, NULL, mergeSort, &leftChild);
    if (returnValue) {
        cout << "Unable to create thread for left child!!" << endl;
        exit(1);
    }
    returnValue = pthread_create(&tid2, NULL, mergeSort, &rightChild);
    if (returnValue) {
        cout << "Unable to create thread for right child!!" << endl;
        exit(1);
    }
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    merge(low, mid, high);
    pthread_exit(NULL);
    return 0;
}


int main() {
    // Generic variable declaration
    const clock_t begin_time = clock();
    int i, arraySize, maxValue = 1e6, shmID, num, threadReturnValue;
    vector<int> v;
    pthread_t tid;
    threadNode tnode;
    tnode.leftIndex = 0; 
    cin >> arraySize;
    tnode.rightIndex = arraySize - 1;
    inputArray = (int*)malloc(sizeof(int) * arraySize);

    // Assigning random numbers to the input array
    mt19937 rand(chrono::steady_clock::now().time_since_epoch().count());
    for (i = 0; i < arraySize; i++) {
        num = rand() % maxValue;
        inputArray[i] = num;
        v.push_back(num);
    }

    // creating thread
    threadReturnValue = pthread_create(&tid, NULL, mergeSort, &tnode);
    if (threadReturnValue) {
        cout << "Error in creating thread!!" << endl;
        exit(1);
    }
    pthread_join(tid, NULL);

    // Sorting verification
    sort(v.begin(), v.end());
    for (i = 0; i < arraySize; i++) {
        if (v[i] != inputArray[i]) {
            cout << v[i] << " != " << inputArray[i] << ", still not sorted!!";
            exit(1);
        }
        //cout << inputArray[i] << ' ';
    }
    cout << endl;

    // Calculating the time taken till mergesort completion
    cout << "Time elapsed : " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " seconds." << endl;
}
