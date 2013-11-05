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

void receiveMessage(message* msg) {
    printf("Message %ld received from %d\n", msg->mdata.messageId, msg->mdata.source);
}

int main(int argc, const char * argv[])
{
    int pid, i;
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
    
    message* message = malloc(sizeof(message));
    
    message->mdata.destination = 0;
    message->mdata.source = i;
    message->mtype = 1;
    strcpy(message->mdata.text, "Brubles!");
    sendMessage(message);
    free(message);
    
    printf("Node %d reporting!\n", i);
    
    listenForMessages(0, receiveMessage);
    
    tearDown();
    
    return 0;
}