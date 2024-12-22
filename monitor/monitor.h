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

#include "../creator/creator.h"

using namespace std;

void printStats(Project_Memory* pm);
void printTablesState(Project_Memory* pm);