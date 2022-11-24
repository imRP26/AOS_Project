//#include <boost/random.hpp>
//#include <boost/nondet_random.hpp>
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
    int i = 0, j = 0, k = 0, num1 = mid - low + 1, num2 = high - mid;
    int tempArray[high - low + 1];
    while (i < num1 && j < num2) {
        if (array[i] < array[j])
            tempArray[k++] = array[i++];
        else
            tempArray[k++] = array[j++];
    }
    while (i < num1)
        tempArray[k++] = array[i++];
    while (j < num2)
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

    // Generic variable declaration and starting the clock
    time_t startNow, endNow;
    time(&startNow);
    int i, arraySize, minValue = 1, maxValue = 1e6, shmID;
    key_t k = IPC_PRIVATE;
    int* shmArray;
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
    
    //unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    //default_random_engine generator(seed);
    //uniform_int_distribution<int> uniformDistribution(minValue, maxValue);
    
    //boost::random::random_device randomDevice;
    //boost::random::mt19937 generator(randomDevice());
    //boost::random::uniform_int_distribution<> uniformDistribution(minValue, maxValue);

    random_device randomDevice;
    mt19937 generator(randomDevice());
    uniform_int_distribution<int> uniformDistribution(minValue, maxValue);
    for (i = 0; i < arraySize; i++)
        shmArray[i] = uniformDistribution(generator);
    
    // Crux of the question
    mergeSort(shmArray, 0, arraySize - 1);
    
    // Verification that the sorting actually happened
    for (i = 1; i < arraySize; i++) {
        cout << shmArray[i] << ' ';
        if (shmArray[i] < shmArray[i - 1]) {
            cout << "Still not sorted!!" << endl;
            exit(1);
        }
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
    time(&endNow);
    double timeTaken = double(endNow - startNow);
    cout << "Time taken : " << fixed << timeTaken << setprecision(3) << " seconds." << endl;
}
