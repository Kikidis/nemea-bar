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
        pm->tables[i].current_free_chair = 0;
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
    retval = sem_init (&pm->semaphores.mtx_log ,1 , 0);   
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
    err = shmdt((void *)pm);
    if ( err == -1 ) perror("Detachment");
    
    /* Remove segment. */
    err = shmctl(shmid, IPC_RMID , 0);
    if (err == -1){
        perror("Removal.");
    }
    else
        printf("Removed. %d\n", err);


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
            int retval = execl ("../visitor/visitor", "./visitor", rest_time, shm_id, NULL);
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
    char shm_id[100];
    sprintf(order_time, "%d", ordertime);
    sprintf(shm_id, "%d", id);
    pid = fork();
    if(pid == -1){
        exit(1);
    }
    if(pid == 0){
        execl ("../receptionist/receptionist", "./receptionist", "-d", order_time, "-s", shm_id, NULL);
    }
}


void createLogFile(char* fileName){
    FILE* fp = fopen(fileName, "w");
    fprintf(fp, "%s\n", "logfile just created.");
    fclose(fp);

}

int main(){
    int id;
    char filename[100];
    strcpy(filename, "../LogFile.txt");
    
    Project_Memory *p = createInIt_memory(&id);cout<< "receptionist i am here"<< endl;
    createLogFile(filename);
    createReceptionist(3, id);
    free_project_memory(id, p);


}