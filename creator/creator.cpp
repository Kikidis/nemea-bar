#include "creator.h"



Project_Memory* createInIt_memory(int* id){
    /* Make shared memory segment. */
    *id = shmget(IPC_PRIVATE , sizeof(Project_Memory) , SEGMENTPERM ); // δημιουργουμε την μνημη 
    if (*id == -1){
        cout << "Cant create share memory. Program is terminating..." << endl;
        exit(1);
    }
    /* Attach the segment. */
    Project_Memory* pm;
    pm = (Project_Memory *) shmat(*id , NULL, 0);   // Ανακτούμε την μνήμη
    if ( pm == NULL){
        cout << "Cant attach share memory. Program is terminating..." << endl;
        exit (2);
    }
    /* Initialize the segment. */
    // tables
    for(int i =0; i < TABLES; i++){
        pm->tables[i].can_i_sit = true;
        pm->tables[i].reserved_chairs = 0;
        for(int j = 0; j < CHAIRS; j++){
            pm->tables[i].chairs[j] = 0;
        }
    }

    // order
    pm->orders.water = 0;
    pm->orders.wine = 0;
    pm->orders.cheese = 0;
    pm->orders.salad = 0;

    // stats
    pm->stats.num_cheese = 0;
    pm->stats.num_salads = 0;
    pm->stats.num_visitors = 0;
    pm->stats.num_water = 0;
    pm->stats.num_wine = 0;
    pm->stats.visit_dur = 0;
    pm->stats.waiting_time = 0;

    /* Initialize the semaphore. */
    // το δευτερο ορισμα ειναι 1 για processes και το τριτο ειναι η αρχικοποιηση του
    int retval = sem_init (&pm->semaphores.mtx_tables ,1 , 1);   
    if (retval != 0) {
        cout<< "Couldn't initialize semaphore." << endl;
        exit (3); 
    }
    retval = sem_init (&pm->semaphores.mtx_visitor ,1 , 0);   
    if (retval != 0) {
        cout<< "Couldn't initialize semaphore." << endl;
        exit (3); 
    }
    retval = sem_init (&pm->semaphores.mtx_receptionist ,1 , 0);   
    if (retval != 0) {
        cout<< "Couldn't initialize semaphore." << endl;
        exit (3); 
    }
    retval = sem_init (&pm->semaphores.mtx_entrance ,1 , 1);   
    if (retval != 0) {
        cout<< "Couldn't initialize semaphore." << endl;
        exit (3); 
    }
    retval = sem_init (&pm->semaphores.mtx_stats ,1 , 1);   
    if (retval != 0) {
        cout<< "Couldn't initialize semaphore." << endl;
        exit (3); 
    }
    retval = sem_init (&pm->semaphores.mtx_log ,1 , 1);   
    if (retval != 0) {
        cout<< "Couldn't initialize semaphore." << endl;
        exit (3); 
    }
    return pm;
}

void free_project_memory(int shmid, Project_Memory* pm){
    // free shared memory
    sem_destroy(&pm->semaphores.mtx_entrance);
    sem_destroy(&pm->semaphores.mtx_log);
    sem_destroy(&pm->semaphores.mtx_receptionist);
    sem_destroy(&pm->semaphores.mtx_stats);
    sem_destroy(&pm->semaphores.mtx_tables);
    sem_destroy(&pm->semaphores.mtx_visitor);

    // detaches the segment:
    int err;
    err = shmdt((void *)pm);    // αποσυνδεομαι απο την μνημη
    if ( err == -1 ) perror("Detachment");
    
    /* Remove segment. */
    err = shmctl(shmid, IPC_RMID , 0);      // διαγραφω την μνημη
    if (err == -1){
        perror("Removal.");
    }
    else
        printf("Removed. Errors: %d\n", err);


}

// κατασκευαζω τους visitors
void createVisitors(int resttime, int id){
    pid_t pid;
    char rest_time[100];
    char shm_id[100];
    sprintf(rest_time, "%d", resttime);
    sprintf(shm_id, "%d", id);
    for(int i = 0; i < VISITORSNUM ; i++){
        pid = fork();
        if(pid == -1){
            exit(1);
        }
        if(pid == 0){
            int retval = execl ("../visitor/visitor", "./visitor", "-d", rest_time, "-s", shm_id, NULL);
            if(retval == -1){
                cout << "exel failed." << endl;
                exit(1);
            }
        }
    }
}

// κατασκευαζω τον receptionist
void createReceptionist(int ordertime, int id){
    pid_t pid;
    char order_time[100];
    char shm_id[100], visitors_num[100];
    sprintf(order_time, "%d", ordertime);
    sprintf(shm_id, "%d", id);
    sprintf(visitors_num, "%d", VISITORSNUM);
    pid = fork();
    if(pid == -1){
        exit(1);
    }
    if(pid == 0){
        execl ("../receptionist/receptionist", "./receptionist", "-d", order_time, "-s", shm_id, "-v", visitors_num, NULL);
    }
}

 // Δημιουργουμε το Logfile
void createLogFile(char* fileName){
    FILE* fp = fopen(fileName, "w");
    fprintf(fp, "%s\n", "LogFile just created.");
    fclose(fp);
}

void addToLogFile(char* fileName, char* msg){
    FILE* fp = fopen(fileName, "a");
    fprintf(fp, "%s\n", msg);
    fclose(fp);
}

void printStatistics(Project_Memory* pm){
    cout << "Statistics:" << endl;
    cout << "Number of visitors: " << pm->stats.num_visitors << endl;
    cout << "Number of water: " << pm->stats.num_water << endl;
    cout << "Number of wine: " << pm->stats.num_wine << endl;
    cout << "Number of cheese: " << pm->stats.num_cheese << endl;
    cout << "Number of salads: " << pm->stats.num_salads << endl;
    cout << "Average waiting time: " << pm->stats.waiting_time / pm->stats.num_visitors << endl;
    cout << "Average visit duration: " << pm->stats.visit_dur / pm->stats.num_visitors << endl;
}

int main(){
    int id;
    int ret, status;
    char fileName[100];
    strcpy(fileName, "../LogFile.txt");
    
    Project_Memory *pm = createInIt_memory(&id);
    printf("I am creator. Share memory ID: %d\n", id);
    createLogFile(fileName);
    createReceptionist(3, id);
    createVisitors(2, id);

    while((ret = wait(&status)) != -1); // wait for all children to finish
    printStatistics(pm);
    free_project_memory(id, pm);

}