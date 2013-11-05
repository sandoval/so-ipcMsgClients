//
//  main.c
//  ipcMsgClients
//
//  Created by Daniel Sandoval on 04/11/2013.
//  Copyright (c) 2013 Departamento de Ciência da Computação - Universidade de Brasília. All rights reserved.
//

#include <stdio.h>
#include "../../ipcMessaging/ipcMessaging/ipcMessaging.h"
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void treatSigint() {
    printf("Received SIGINT!\n");
    tearDown();
    exit(1);
}

int main(int argc, const char * argv[])
{
    int pid, i;
    message message;
    struct sigaction action;
    signal(SIGCHLD, SIG_IGN);
    
    for (i = 0; i < 8; i++) {
        pid = fork();
        if (pid == 0) {
            setup(i);
            break;
        }
    }
    if (i == 8) {
        setup(i);
    }
    
    action.sa_handler = treatSigint;
    sigaction(SIGINT, &action, NULL);
    
    message.destination = 0;
    message.source = i;
    message.mtype = 1;
    strcpy(message.text, "Brubles!");
    sendMessage(&message);
    
    printf("Node %d reporting!\n", i);
    for (i = 0; i<10; i++) {
        sleep(10);
    }
    
    tearDown();
    
    return 0;
}