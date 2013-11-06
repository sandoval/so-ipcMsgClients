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
#include <limits.h>

void treatSigint() {
    printf("Received SIGINT!\n");
    tearDown();
    exit(1);
}

void receiveMessage(message* msg) {
    if (msg->mtype == 1) {
        if (msg->mdata.text[0] == 'y') {
            //Client managed to connect to the pool.
            message printMsg;
            //Set message we want to print.
            strcpy(printMsg.mdata.text, "Please print this message!\n");
            //Set printer as destination.
            printMsg.mdata.destination = INT_MAX;
            //Set print message type (3)
            printMsg.mtype = 3;
            sendMessage(&printMsg);
        } else {
            //Client could not connect. Pool is probably full. Try again after a second.
            sleep(1);
            message connectMsg;
            connectMsg.mtype = 1;
            connectMsg.mdata.destination = INT_MAX;
            sendMessage(&connectMsg);
        }
    } else if (msg->mtype == 2) {
        if (msg->mdata.text[0] == 'y') {
            printf("[NODE %d] Successfully disconnected!\n", msg->mdata.destination);
        }
    } else if (msg->mtype == 3) {
        if (msg->mdata.text[0] == 'y') {
            //Print was successful.
            printf("[NODE %d] Successfully printed!\n", msg->mdata.destination);
        }
        //Let's get out of the pool.
        message disconnectMsg;
        disconnectMsg.mtype = 2;
        disconnectMsg.mdata.destination = INT_MAX;
        sendMessage(&disconnectMsg);
    } else {
        printf("[NODE %d] Message %ld of type %ld received from %d\n", msg->mdata.destination, msg->mdata.messageId, msg->mtype, msg->mdata.source);
    }
}

int main(int argc, const char * argv[])
{
    int pid, i;
    struct sigaction action;
    message* msg;
    
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
    
    msg = malloc(sizeof(message));
    
    //Set printer manager as destination.
    msg->mdata.destination = INT_MAX;
    //Set message type as pool entry request.
    msg->mtype = 1;
    
    sendMessage(msg);
    free(msg);
    
    printf("Node %d reporting!\n", i);
    
    listenForMessages(0, receiveMessage);
    
    tearDown();
    
    return 0;
}