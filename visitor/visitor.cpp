#include "visitor.h"



// Ο τυχαίος χρονος αναμονης στο τραπεζι
void restTime(int resttime){
    int t1 = 0.7*resttime;
    int t2 = resttime - t1;
    t2 = rand() % (t2 + 1);
    //cout << t2<< " "<< t1 <<endl;
    int TotalRestTime = t1 + t2;
    //cout<< TotalRestTime << endl;
    sleep(TotalRestTime);
}

void addToLogFile(Project_Memory* pm, char* fileName, char* msg){
    sem_wait(&pm->semaphores.mtx_log);
    FILE* fp = fopen(fileName, "a");
    fprintf(fp, "%s\n", msg);
    fclose(fp);
    sem_post(&pm->semaphores.mtx_log);
}

// Κανει τυχαια παραγγελια
void placeAndGetOrder(Project_Memory* pm){
    srand(getpid());  // Αρχικοποιω την rand με το time
    int randNum = rand()%3;
    if(randNum == 0){
        pm->orders.water = 1;
    }
    else if(randNum == 1){
        pm->orders.wine = 1;
    }
    else{
        pm->orders.water = 1;
        pm->orders.wine = 1;
    }

    if((rand()%2) == 0){
        pm->orders.cheese = 1;
    }
    if((rand()%2) == 0){
        pm->orders.salad = 1;
    }
}

// Ελεγχει αν υπαρχει εστω και μια κενή καρεκλα
bool isEmptyChair(Project_Memory* pm){
    for(int i = 0; i < 3; i++){
        if(pm->tables[i].can_i_sit){
            return true; 
        }     
    }  
    return false; 
}


int main (int argc , char **argv){
    srand(getpid());  // Αρχικοποιω την rand με το time
    int resttime = atoi(argv[2]);
    int shmid = atoi(argv[4]);
    int table, chair, wait_before_entrance;
    char fileName[100], logmsg[100];
    double t1, t2, t3;  //  t1 = time before entrance, t2 = time after entrance, t3 = leaving time
    struct tms tb1 , tb2, tb3;
    double ticspersec;
    strcpy(fileName, "../LogFile.txt");

    // ΘΑ ΠΡΕΠΕΙ ΝΑ ΜΗΝ ΕΡΧΟΝΤΑΙ ΟΛΟΙ ΜΑΖΙ ΟΙ VISITORS
    wait_before_entrance = rand() % 25;
    sleep(wait_before_entrance);

    ticspersec = (double) sysconf(_SC_CLK_TCK);
    t1 = (double) times(&tb1);  // ποτε ηρθε ο visitor

    Project_Memory* pm;
    pm = (Project_Memory *) shmat(shmid , NULL, 0);   // Προσαρτούμε την μνήμη
    if ( pm == (void *) -1){
        cout << "Visitor: Cant attach share memory. Program is terminating..." << endl;
        exit (1);
    }

    // Θα πρεπει τωρα να περιμενει στην ουρα για να παρει καρεκλα και μετα να παραγγειλει 
    sem_wait(&pm->semaphores.mtx_entrance);
    
    // ενημερωνω το πληθος των visitors για τα στατιστικα 
    sem_wait(&pm->semaphores.mtx_stats);
    pm->stats.num_visitors++;
    sem_post(&pm->semaphores.mtx_stats);

    t2 = (double) times(&tb2);  // χρονος για το ποτε εφυγε απο την ουρα και παει να βρει τραπεζι 
    sem_wait(&pm->semaphores.mtx_tables);   // Στην περιοχη των τραπεζιων-καρεκλών πρεπει να ειναι ΜΟΝΟ ενας visitor
    for(int i = 0; i < 3; i++){     // ψαχνω σε ποιο τραπεζι μπορω να κατσω(παντα υπαρχει ενα που μπορω)
        if(pm->tables[i].can_i_sit){
            table = i;
            for(int j = 0; j < 4 ; j++){    // ψαχνω σε ποια καρεκλα του τραπεζιου i μπορω να κατσω
                if(pm->tables[i].chairs[j] == 0){   // βρηκα ελευθερη καρεκλα
                    chair = j;
                    pm->tables[i].chairs[chair] = getpid();    // καταλαμβανω την καρεκλα
                    pm->tables[i].reserved_chairs++;    // ενημερωνω ποσες καρεκλες ειναι κατειλημμενες
                    if(pm->tables[i].reserved_chairs == 4){ // αν επιασα την τελευταια ελευθερη καρεκλα
                        pm->tables[i].can_i_sit = false;    
                    }
                    break;
                }
            }
            break;
        }
    }
    sprintf(logmsg, "Visitor %d: Found table %d and chair %d.\n", getpid(), table, chair);
    addToLogFile(pm, fileName, logmsg);

    /*  σε αυτο το σημειο ο visitor εχει κατοχυρωσει καρεκλα. αν υπαρχει και καποια επιπλεον καρεκλα ελευθερη τοτε κανει signal
        στο entrance ωστε να περασει ο επομενος visitor. θελουμε να ερχεται στην περιοχη των τραπεζιων ενας νεος visitor
        μονο οταν μπορει να βρει ελευθερη καρεκλα. αν δεν υπαρχει ελευθερη καρεκλα τοτε signal στο entrance θα κανει ενας visitor
        ο οποιος αποχωρει και ειναι ο τελευταιος του τραπεζιου.
    */ 
    if(isEmptyChair(pm)){    // αν υπαρχει και αλλη ελευθερη καρεκλα
        sem_post(&pm->semaphores.mtx_entrance);
    }
    sem_post(&pm->semaphores.mtx_tables);   // ο visitor φευγει απο την περιοχη των τραπεζιων για να παει να παραγγειλει

    // Βαζουμε παραγγελια κανω σημα στον receptionist, περιμενουμε και μας ειδοποιησει να την παρουμε 
    sprintf(logmsg, "Visitor %d: Ready to place an order.\n", getpid());
    addToLogFile(pm, fileName, logmsg);
    placeAndGetOrder(pm);
    sem_post(&pm->semaphores.mtx_receptionist); // κανω signal να με εξυπηρετησει 
    sem_wait(&pm->semaphores.mtx_visitor);  //  περιμενω μεχρι να μου δωσει ο receptionist το order
    addToLogFile(pm, fileName, (char*)"Visitor: Took the order.\n");
    restTime(resttime); // Εδω καταναλλωνει οτι πηρε απο το μπαρ και κανει sleep()

    sem_wait(&pm->semaphores.mtx_tables);
    pm->tables[table].chairs[chair] = 0;        // απελευθερωνω την καρεκλα
    pm->tables[table].reserved_chairs--;    
    if(pm->tables[table].reserved_chairs == 0){
        if(!isEmptyChair(pm)){
            sem_post(&pm->semaphores.mtx_entrance);
        }
        pm->tables[table].can_i_sit = true;
    }
    sem_post(&pm->semaphores.mtx_tables);



    
    t3 = (double) times(&tb3);  // Ο χρονος που φευγει ο visitor
    // Ενημερωνουμε τα στατιστικα για το time
    sem_wait(&pm->semaphores.mtx_stats);
    pm->stats.waiting_time += ((t2-t1) / ticspersec);   // χρονος παραμονής στο entrance

    pm->stats.visit_dur += ((t3-t2) / ticspersec);     // χρονος μετα το entrance μεχρι που εφυγε 
    sem_post(&pm->semaphores.mtx_stats);
    sprintf(logmsg, "Visitor %d: Leaving Nemea after %.2lf.\n", getpid(), ((t3-t1) / ticspersec)); 
    addToLogFile(pm, fileName, logmsg);
    
    // detaches the segment:
    int err;
    err = shmdt((void *)pm);    // αποσυνδεομαι απο την μνημη
    if ( err == -1 ) perror("Detachment");

    cout<< " Visitor just finished."<< endl;
}