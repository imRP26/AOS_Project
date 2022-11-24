# Concurrent-Merge-Sort

*********
**QUESTION DESCRIPTION**
*********
- Given a number n and n numbers, sort the numbers using Merge Sort.
- Recursively make two child processes, one for the left half, one for the right half. If the number of elements in the array for a 
  process is less than 5, perform a selection sort.
- The parent of the two children then merges the result and returns back to the parent and so on.
- Compare the performance of the merge sort with a normal merge sort implementation and make a report.
- You must use the shmget, shmat functions as taught in the tutorial.
- Use threads in place of processes for Concurrent Merge Sort. Add the performance comparison to the above report.
**********
**Solution**
<br>
For Normal Merge Sort, the code works as per expectation (Trivial), but for the version of Merge Sort (named as **concurrentMergeSort** here) that works on spawning 2 processes in every mergeSort step recursively, the maximum input size for which mergeSort works is observed to be **2366** so far.
<br>
From the [OSX Man Page for fork()](https://www.unix.com/man-page/osx/2/fork/), it has been mentioned :- 
<br>
<br>
**Fork() will fail and no child process will be created if:**
- **The system-imposed limit on the total number of processes under execution would be exceeded. This limit is configuration-dependent.**
- **The system-imposed limit MAXUPRC (<sys/param.h>) on the total number of processes under execution by a single user would be exceeded.**
- **There is insufficient swap space for the new process.**

For multi-threaded Merge Sort, 
