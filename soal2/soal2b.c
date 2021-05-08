#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 

pthread_t tid[24];
int a_glob[4][6], b_glob[4][6];
int output[4][6];
int (*result_matrix)[6];

int factorial2(int m, int n){
    int res=1;
    for(int j=m; j>n; j--){
        res = res*j;
    }
    return res;
}

int factorial1(int n){
    if(n>=1){
        return n*factorial1(n-1);
    }
    else{
        return 1;
    }
}

void *mult_matrix(void *arg){
    for(int i=0; i<4; i++){
        for(int j=0; j<6; j++) {
            pthread_t id = pthread_self();
            int index = 0;
            if(pthread_equal(id, tid[index])) {

                //If a >= b -> a!/(a-b)!
                if(a_glob[i][j] >= b_glob[i][j]){
                    output[i][j] = factorial2(a_glob[i][j], (a_glob[i][j] - b_glob[i][j]));
                }

                //If b > a -> a!
                if(b_glob[i][j] > a_glob[i][j]){
                    output[i][j] = factorial1(a_glob[i][j]);
                }

                //If 0 -> 0
                if(a_glob[i][j] == 0 || b_glob[i][j] == 0){
                    output[i][j] = 0;
                }

                index++;
                }
            }
        
    }
}

int main(){

    key_t key = 1112;
    int shmid = shmget(key,sizeof(int[4][6]), 0666 | IPC_CREAT);  
    result_matrix =  shmat(shmid,NULL,0);  

	int matrix_b[4][6] ={
		{1, 2, 0, 3, 2, 2},
		{1, 2, 2, 3, 2, 3},
        {0, 3, 2, 1, 1, 0},
        {1, 2, 2, 3, 2, 3}
	};

    printf("Matrix a:\n");
    for(int k=0; k<4; k++) {
        for(int l=0; l<6; l++){
            //printf("%4d ", result_matrix[k][l]);
            a_glob[k][l] = result_matrix[k][l];
            printf("%4d ", a_glob[k][l]);
        }
        printf("\n");
    }

    printf("\nMatrix b:\n");
    for(int k=0; k<4; k++) {
        for(int l=0; l<6; l++){
            b_glob[k][l] = matrix_b[k][l];
            printf("%4d ", b_glob[k][l]);
        }
        printf("\n");
    }

    //buat thread
    for(int i=0; i<24; i++){
        pthread_create(&(tid[i]), NULL, &mult_matrix, NULL); 
        
    }

    //joinkan thread
    for(int i=0; i<24; i++){
        pthread_join(tid[i], NULL);
    }

    //print hasil matriks
    printf("\nResult Matrix:\n");
    for(int i=0; i<4; i++){
        for(int j=0; j<6; j++)
        {
            printf("%4d ", output[i][j]);
        }
        printf("\n");
    }

    shmdt(result_matrix); 
    shmctl(shmid,IPC_RMID,NULL); 
    exit(0);
    return 0;
}