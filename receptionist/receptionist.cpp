#include "receptionist.h"



// Ο τυχαίος χρονος εξυπηρέτησης της παραγγελιας
void serviceTime(int ordertime){
    int t1 = 0.5*ordertime;
    int t2 = ordertime - t1;
    t2 = rand() % (t2 + 1);
    //cout << t2<< " "<< t1 <<endl;
    int TotalServiceTime = t1 + t2;
    //cout<< TotalServiceTime << endl;
    sleep(TotalServiceTime);
}

void addToLogFile(Project_Memory* pm, char* fileName, char* msg){
    sem_wait(&pm->semaphores.mtx_log);
    FILE* fp = fopen(fileName, "a");
    fprintf(fp, "%s\n", msg);
    fclose(fp);
    sem_post(&pm->semaphores.mtx_log);
}


// Εξυπηρετεί την παραγγελία του visitor και ενημερώνει τα Stats
void serviceOrder(Project_Memory* pm, int ordertime, char* fileName){
    sem_wait(&pm->semaphores.mtx_stats);    // Προστατευουμε το stats διοτι εχει προσβαση και το monitor
    if(pm->orders.water){
        pm->stats.num_water++;
        pm->orders.water = 0;
    }
    if (pm->orders.wine){
        pm->stats.num_wine++;
        pm->orders.wine = 0;
    }
    if (pm->orders.cheese){
        pm->stats.num_cheese++;
        pm->orders.cheese = 0;
    }
    if(pm->orders.salad){
        pm->stats.num_salads++;
        pm->orders.salad = 0;
    }  
    sem_post(&pm->semaphores.mtx_stats);
    serviceTime(ordertime);
    addToLogFile(pm, fileName, (char*)"Receptionist: Just complete the order.\n");
}



int main(int argc , char **argv){
    srand(time(NULL));  // Αρχικοποιω την rand με το time
    int ordertime = atoi(argv[2]);
    int shmid = atoi(argv[4]);
    int visitors_num = atoi(argv[6]);
    char fileName[100];
    strcpy(fileName, "../LogFile.txt");



    Project_Memory* pm;
    pm = (Project_Memory *) shmat(shmid , NULL, 0);   // Προσαρτούμε την μνήμη
    if ( pm == (void *) -1){
        cout << "Receptionist: Cant attach share memory. Program is terminating..." << endl;
        exit (1);
    }
    cout<< "Receptionist i am here."<< endl;
    for(int i = 0; i < visitors_num; i++){
        sem_wait(&pm->semaphores.mtx_receptionist); // ο receptionist περιμενει καποιον visitor να του κανει Post
        serviceOrder(pm, ordertime, fileName);  // εξυπηρετει την παραγγελια και ενυημερωνει τα stats
        sem_post(&pm->semaphores.mtx_visitor);  // ειδοποιουμε τον visitor να παρει την παραγγελια
    }
    // detaches the segment:
    int err;
    err = shmdt((void *)pm);    // αποσυνδεομαι απο την μνημη
    if ( err == -1 ) perror("Detachment");

}