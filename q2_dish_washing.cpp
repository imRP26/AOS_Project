//compile using g++ q1.cpp -lpthread -lrt


#include <bits/stdc++.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

using namespace std::chrono;
using namespace std;

// #ifndef ONLINE_JUDGE
// 	#include "../../../debug.cpp"
// #endif

int n, m;
pthread_t *students;
sem_t *taps;
sem_t *scrotch;

time_point<std::chrono::high_resolution_clock> start;

struct args{
	int tap_number, student_number;
};

void wash_utensil(int scrub_time, int wash_time, int student_number, int tap_number, const char *utensil_name){
	int *buffer = (int *)malloc(sizeof(int));

	*buffer = 0;
	int acquired_scrotch_number = -1;
	// if(tap_number == 4){
	// 	debug("HERE");
	// }
	while(true){
		if(tap_number != n)
			sem_getvalue(&scrotch[tap_number], buffer);
		if(*buffer == 0){
			if(tap_number != 0)
				sem_getvalue(&scrotch[tap_number-1], buffer);
			if(*buffer != 0){
				sem_wait(&scrotch[tap_number-1]);
				acquired_scrotch_number = tap_number-1;
				break;
			}
		}
		else{
			sem_wait(&scrotch[tap_number]);
			acquired_scrotch_number = tap_number;
			break;
		}
	}

	// printf("%d \n", acquired_scrotch_number);
	printf("Student %d picked up scrotch %d\n", student_number, acquired_scrotch_number);

	// if(student_number == 4){
	// 	debug("KKK");
	// }

	sleep(scrub_time);
	sem_post(&scrotch[acquired_scrotch_number]);
	printf("Student %d completed scrubbing his %s\n", student_number, utensil_name);

	sleep(wash_time);
	printf("Student %d washed his %s\n", student_number, utensil_name);

	free(buffer);
}

void *student_thread(void *arg){
	int student_number = ((args *)arg) -> student_number;
	int tap_number = ((args *)arg) -> tap_number;
    free(arg);

	printf("Student %d goes to tap %d\n", student_number, tap_number);
    sem_wait(&taps[tap_number]);

    
    wash_utensil(4, 5, student_number, tap_number, "Plate");
    wash_utensil(3, 3, student_number, tap_number, "Glass");
    wash_utensil(2, 1, student_number, tap_number, "Spoon");

    sem_post(&taps[tap_number]);

	auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    double duration_seconds = (double)duration.count()/1000;
    printf("\n------ STUDENT %d FINISHED AT TIME %ld ------\n\n", student_number, lround(duration_seconds));

    return NULL;
}

int32_t main(int argc, char *argv[]){
	if(argc != 3){
		cout << "INVALID NUMBER OF ARGUMENTS\n";
		return 0;
	}

	mt19937 rand(chrono::steady_clock::now().time_since_epoch().count());
	n = stoi(argv[1]);
	m = stoi(argv[2]);

	taps = (sem_t *)malloc(sizeof(sem_t)*n);
	scrotch = (sem_t *)malloc(sizeof(sem_t)*(n-1));
	students = (pthread_t *)malloc(sizeof(pthread_t)*m);

	for(int i=0; i<n; i++)
		sem_init(&taps[i], 0, 1);
	for(int i=0; i<n-1; i++)
		sem_init(&scrotch[i], 0, 1);

	start = high_resolution_clock::now();
	for(int i=0; i<m; i++){
		args *arguments = (args *)malloc(sizeof(args));
		arguments->tap_number = rand()%n;
		arguments->student_number = i;
		pthread_create(&students[i], NULL, student_thread, arguments);
	}


	for(int i=0; i<m; i++){
		pthread_join(students[i], NULL);
	}

	free(taps);
	free(scrotch);
	free(students);
	
}