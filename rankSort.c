#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <limits.h>

#define MAX 10

void mergearray(int k,int *array, int *array2) {
  int start[k];
  int stop[k];
  int i;

  for (i = 0;i<k;i++) {
    start[i] = i*MAX/k;
    stop[i] = ((i+1)*MAX/k);
    printf("%d %d",start[i],stop[i]);
  }

  int count = 0;
  while (count < MAX) {
    int temp = INT_MAX;
    int position = -1;
    for (i = 0; i<k; i++) {
      if (array[start[i]] <= temp && start[i] < stop[i]) {
        temp = array[start[i]];
        position = i;
      }
    }
    array2[count] = array[start[position]];
    count++;
    start[position]++;
  }
}

void shuffle(int *array, size_t n) {
  if (n > 1) {
    size_t i;
    for (i = 0; i < n - 1; i++) {
      size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
      int t = array[j];
      array[j] = array[i];
      array[i] = t;
    }
  }
}

int main(int argc, char *argv[]) {
  int i,j,myid,numprocs;
  int list[MAX];

  MPI_Init( &argc, &argv );
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&myid);

  int buffer[MAX/numprocs];
  int rank_list[MAX/numprocs];
  int sort_list[MAX/numprocs];

  for (i = 0;i<MAX/numprocs;i++) {
    rank_list[i] = 0;
    sort_list[i] = 0;
  }

  if (myid == 0) {
    for(i=0; i<MAX; i++)
	    list[i] = i;
	  shuffle(list,MAX);
  }


  MPI_Scatter((void*)list,MAX/numprocs,MPI_INT,
              (void*)buffer,MAX/numprocs,MPI_INT,
              0,MPI_COMM_WORLD);

  for (i=0; i<MAX/numprocs; i++) {
		for (j=0; j<i; j++) {
	    if (buffer[i] >= buffer[j])
	      rank_list[i]++;
      else
	      rank_list[j]++;
	  }
	}

	for (i=0; i<MAX/numprocs; i++)
		sort_list[rank_list[i]] = buffer[i];

  MPI_Gather(sort_list,MAX/numprocs,MPI_INT,list,MAX/numprocs,MPI_INT,0,MPI_COMM_WORLD);

  if (myid == 0) {
    int final[MAX];
    mergearray(numprocs,list,final);
    for(i=0; i<MAX; i++)
	    printf("%d\n",final[i]);
  }

  MPI_Finalize();
}
