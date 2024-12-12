#include "receptionist.h"



void serviceTime(int ordertime){
    int t1 = 0.5*ordertime;
    int t2 = ordertime - t1;
    t2 = rand() % (t2 + 1);
    cout << t2<< " "<< t1 <<endl;
    int TotalServiceTime = t1 + t2;
    cout<< TotalServiceTime << endl;
    //sleep(TotalServiceTime);
}



int main(int argc , char **argv){
    srand(time(NULL));  // Αρχικοποιω την rand με το time
    int ordertime = atoi(argv[2]);
    int shmid = atoi(argv[4]);


    serviceTime(ordertime);
    Project_Memory* pm;
    pm = (Project_Memory *) shmat(shmid , NULL, 0);   // Προσαρτούμε την μνήμη
    if ( pm == NULL){
        cout << "Receptionist: Cant attach share memory. Program is terminating..." << endl;
        exit (1);
    }
    cout<< "receptionist i am here"<< endl;
    while(true){
        sem_wait(&pm->semaphores.mtx_receptionist);
    }

}