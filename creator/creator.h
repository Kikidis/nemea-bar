#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/times.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <wait.h>

#define TABLES 3
#define CHAIRS 4
#define SEGMENTPERM 0666
#define VISITORSNUM 10

using namespace std;


struct Table{
    int chairs[4];  // 0 αδεια θεση, 1 κατειλημμένη
    bool can_i_sit;     // Αν ειναι true ενας επισκεπτης μπορει να κατσει, αλλιως οχι 
    int reserved_chairs;    // ποσες καρεκλες ειναι κατειλημμενες
};

struct Order{
    int water;
    int wine;
    int cheese;
    int salad;
};

struct Stats{
    int num_water;
    int num_wine;
    int num_cheese;
    int num_salads;
    double visit_dur;
    int num_visitors;
    double waiting_time;
};

/*
    Ο receptionist κολλαει (μπλοκαρει) στον semaphore του και περιμενει να του κανει Post Καποιος visitor που εχει βαλει παραγγελια.
    Εξυπηρετει την παραγγελια και κανει Post στον visitor να παρει τα πραγματα και ξανα μπλοκαρει. Περιμενοντας τον επομενο visitor.
    Οι visitors οταν ερχονται μπλοκαρονται στο entrance, εκτος απο εναν. Στην περιοχη των τραπεζιων μπαινει μονο ενας visitor 
    και συμπεριφερεται ως εξης: - βρισκει καρεκλα και αν υπαρχουν κενες θεσεις οπουδηποτε κανει Post στο entrance, αλλιως οχι
    και προχωραει για την παραγγελια του. εvας visitor Που φευγει αν ειναι ο τελευταιος τοτε κοιτα αν υπαρχουν σε αλλα τραπεζια ελευθερες θεσεις,
    αν υπαρχουν απλως φευγει, αν δεν υπαρχουν τοτε κανει Post στο entrance. Προσπαθουμε παντα το entrance να ειναι 0 η 1 ωστε οποιος μπαινει 
    να μπορει παντα να βρει μια θεση ελευθερη.
*/

struct Semaphores{
    sem_t mtx_tables;   // Μονο ενας visitor μπαινει ή βγαινει απο την περιοχη των τραπεζιων
    sem_t mtx_visitor;
    sem_t mtx_receptionist;
    sem_t mtx_entrance;
    sem_t mtx_stats;    // Mutex για τα στατιστικα -μονο ενας ενημερωνει καθε φορα
    sem_t mtx_log;      // Mutex για το LogFile -μονο ενας ενημερωνει καθε φορα
};

struct Project_Memory{  // η Shared Memory του προγραμματος
    Table tables[3];
    Order orders;

    Stats stats;
    Semaphores semaphores;
};

Project_Memory* createInIt_memory(int* id);

void createVisitors(int resttime, int id);
void createReceptionist(int ordertime, int id);

void createLogFile(char* fileName);
void addToLogFile(char* fileName, char* msg);

void free_project_memory(int shmid);

