//compile using g++ q1.cpp -lpthread -lrt

#include <bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

// #ifndef ONLINE_JUDGE
// 	#include "../../debug.cpp"
// #endif


int n, m;
int **entries;
sem_t *semaphores;

vector<int> available_entries;
sem_t sem_available_entries;

void* editor(void *arg){
	mt19937 rand(chrono::steady_clock::now().time_since_epoch().count());
	int index = *((int *) arg);
    free(arg);
	set<int> seen;
	while(seen.size() != m){

		vector<int> available;
		for(int i=0; i<m; i++)
			if(seen.find(i) == seen.end() && entries[index][i] == -1)
				available.push_back(i);

		if(available.size() == 0)
			break;

		int num = available[rand()%available.size()];
		int *buffer = (int *)malloc(sizeof(int));
		sem_getvalue(&semaphores[num], buffer);
		if(*buffer == 0)
			continue;

		//race condition can occur here but it is harmless (on the rare case that it does occur, an editor would wait on the report)
		sem_wait(&semaphores[num]);
		if(entries[index][num] == -1 && seen.find(num) == seen.end()){
			printf("Editor %d picked up article %d\n", index, num);
			sleep(1);
			int accepted = rand()%2;
			if(accepted == 0){
				printf("Editor %d REJECTED article %d\n", index, num);
			}
			else{
				for(int i=0; i<n; i++)
					entries[i][num] = index;
				printf("Editor %d ACCEPTED article %d\n", index, num);
			}
		}
		seen.insert(num);
		sem_post(&semaphores[num]);
		// sleep(rand()%3);
		// sleep((double)(rand()%30)/10 + 1);
	}
	printf("Editor %d has finished its job\n", index);
	return NULL;
}

int32_t main(int argc, char *argv[]){
	if(argc != 3){
		cout << "INVALID NUMBER OF ARGUMENTS\n";
		return 0;
	}


	n = stoi(argv[1]);
	m = stoi(argv[2]);

	entries = (int **)malloc(sizeof(int *)*n);
	for(int i=0; i<n; i++){
		entries[i] = (int *)malloc(sizeof(int)*m);
	}

	for(int i=0; i<n; i++)
		for(int j=0; j<m; j++)
			entries[i][j] = -1;

	semaphores = (sem_t *)malloc(sizeof(sem_t)*m);
	sem_init(&sem_available_entries, 0, 1);

	for(int i=0; i<m; i++){
		sem_init(&semaphores[i], 0, 1);
		available_entries.push_back(i);
	}

	vector<pthread_t> v;
	for(int i=0; i<n; i++){
		pthread_t t;
		int *index = (int *)malloc(sizeof(int));
		*index = i;
		pthread_create(&t, NULL, editor, index);
		v.push_back(t);
		// sleep(1);
	}

	for(int i=0; i<n; i++){
		pthread_join(v[i], NULL);
	}

	// for(int i=0; i<n; i++){
	// 	for(int j=0; j<m; j++)
	// 		cout << entries[i][j] << " ";
	// 	cout << "\n";
	// }
	// cout << "\n";

	// debug(n, m);
	for(int i=0; i<n; i++){
		vector<int> v;
		for(int j=0; j<m; j++){
			if(entries[i][j] == i)
				v.push_back(j);
		}
		cout << v.size() << "\n";
		for(int i=0; i<v.size(); i++)
			cout << v[i]+1 << " ";
		cout << "\n";
	}
}
