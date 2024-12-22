#include "monitor.h"



void printStats(Project_Memory* pm){
    cout << "Statistics:" << endl;
    cout << "Number of visitors: " << pm->stats.num_visitors << endl;
    cout << "Number of water: " << pm->stats.num_water << endl;
    cout << "Number of wine: " << pm->stats.num_wine << endl;
    cout << "Number of cheese: " << pm->stats.num_cheese << endl;
    cout << "Number of salads: " << pm->stats.num_salads << endl;
    cout << "Average waiting time: " << pm->stats.waiting_time / pm->stats.num_visitors << endl;
    cout << "Average visit duration: " << pm->stats.visit_dur / pm->stats.num_visitors << endl;
}

void printTablesState(Project_Memory* pm){
    cout << "Tables state:" << endl;
    for (int i = 0; i < 3; i++){
        printf("Table %d: \n", i);
        for(int j = 0; j < 4; j++){
            if(pm->tables[i].chairs[j] == 0){
                printf("Chair %d: is empty.\n", j);
            }
            else{
                printf("Chair %d: is occupied by Visitor %d.\n", j, pm->tables[i].chairs[j]);
            }  
        }
    }

}


int main(int argc , char **argv){
    int shmid = atoi(argv[2]);

    Project_Memory* pm;
    pm = (Project_Memory *) shmat(shmid , NULL, 0);   // Προσαρτούμε την μνήμη
    if ( pm == (void *) -1){
        cout << "Monitor: Cant attach share memory. Program is terminating..." << endl;
        exit (1);
    }
    // Παρουσιάζω τα στατιστικα
    sem_wait(&pm->semaphores.mtx_stats);
    printStats(pm);
    sem_post(&pm->semaphores.mtx_stats);

    // Παρουσιάζω την κατασταση στα τραπέζια
    sem_wait(&pm->semaphores.mtx_tables);
    printTablesState(pm);
    sem_post(&pm->semaphores.mtx_tables);

    printf("End of Statistics. Monitor has just finished.\n");
    
    
    // detaches the segment:
    int err;
    err = shmdt((void *)pm);    // αποσυνδεομαι απο την μνημη
    if ( err == -1 ) perror("Detachment");
}