/*
* train_mngmnt.c
* This file contains the controller and trains thread implementation
*/

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "train.h"
#include "node.h"
#include "defines.h"
#include "prototypes.h"
#include "main.h"

/*
* create_train(void *train_info)
*/
void 
*create_train(void *train_info)
{
    train *atrain = (train*)train_info;

    #ifdef VERBOSE_DEBUG
    printf("train[%d] started\n",atrain->train_id);
    #endif
    
    /* Wait until we have finished loading all trains from input */
    pthread_mutex_lock(&loadingcond_mutex);
    while(!input_loaded) {
        #ifdef VERBOSE_DEBUG
        printf("Train[%d] blocked\n",atrain->train_id);
        #endif
        pthread_cond_wait(&loading_cond, &loadingcond_mutex); 
    }       
    pthread_mutex_unlock(&loadingcond_mutex);

    /*'load' cargo*/
    usleep(atrain->loading_time*TIME_DELAY);    
    #ifdef VERBOSE_DEBUG
    printf("Train[%d] has finished loading its cargo\n",atrain->train_id);
    #endif
    
    /*Detect direction and add to appropiate queue */
    switch(atrain->direction) {
        case 'e':    /* East low priority */
            pthread_mutex_lock(&queue_le);
            queue_insert(&queue_le_f,&queue_le_r,atrain);            
            pthread_mutex_unlock(&queue_le);
            
            printf("Train %2d is ready to go East\n",atrain->train_id);
            
            pthread_mutex_lock(&emptyqueue_mutex);       
            empty_queue=1;
            pthread_cond_signal(&queue_cond);
            pthread_mutex_unlock(&emptyqueue_mutex);
        break;
        case 'E':    /* East high priority */
            pthread_mutex_lock(&queue_he);
            queue_insert(&queue_he_f,&queue_he_r,atrain);
            pthread_mutex_unlock(&queue_he);
            
            printf("Train %2d is ready to go East\n",atrain->train_id);
            
            pthread_mutex_lock(&emptyqueue_mutex);
            empty_queue=1;
            pthread_cond_signal(&queue_cond);
            pthread_mutex_unlock(&emptyqueue_mutex);
        break;
        case 'w':    /* West low priority */
            pthread_mutex_lock(&queue_lw);
            queue_insert(&queue_lw_f,&queue_lw_r,atrain);
            pthread_mutex_unlock(&queue_lw);
            
            printf("Train %2d is ready to go West\n",atrain->train_id);
            
            pthread_mutex_lock(&emptyqueue_mutex);            
            empty_queue=1;
            pthread_cond_signal(&queue_cond);
            pthread_mutex_unlock(&emptyqueue_mutex);
        break;
        case 'W':    /* West high priority */
            pthread_mutex_lock(&queue_hw);
            queue_insert(&queue_hw_f,&queue_hw_r,atrain);
            pthread_mutex_unlock(&queue_hw);
            
            printf("Train %2d is ready to go West\n",atrain->train_id);
            
            pthread_mutex_lock(&emptyqueue_mutex);
            empty_queue=1;
            pthread_cond_signal(&queue_cond);
            pthread_mutex_unlock(&emptyqueue_mutex);
        break;
    }    
    pthread_exit(NULL); 
}
/*
*
*/
void 
*init_controller() 
{
    char *last_direction_human;
    
    #ifdef VERBOSE_DEBUG
    printf("Controller thread has started\n");
    #endif
    
    /* We wait until there is something in queue */
    pthread_mutex_lock(&emptyqueue_mutex);
    while(!empty_queue) {
        #ifdef VERBOSE_DEBUG
        printf("Controller waiting for something in queue.\n");
        #endif
        pthread_cond_wait(&queue_cond, &emptyqueue_mutex); 
    }       
    pthread_mutex_unlock(&emptyqueue_mutex);
    
    #ifdef VERBOSE_DEBUG
        printf("Controller thread has found something in the queue\n");
    #endif   
     
    while(trains_in_queue>0) {
    
        /* Lock queues - No adding while we're selecting a train */
        pthread_mutex_lock(&queue_le);
        pthread_mutex_lock(&queue_he);
        pthread_mutex_lock(&queue_hw);
        pthread_mutex_lock(&queue_lw);
    
        node *train_he,*train_le,*train_hw,*train_lw,*train_on_track;
    
        /*
        Last direction codes:
        0 - Not set
        1 - Last Train came from left
        2 - Last Train came from right    
        */    
    
        switch(last_direction) {    
            case 0: /* First train to cross */
            /* Since this is the first train crossing the track we*/
            /* check the queues in the following order: HE,LE,HW,LW*/
                train_he=queue_pop(&queue_he_f,&queue_he_r);
                if(train_he->train_id==-1) {//high priority East station is empty
                    free(train_he);
                    train_le=queue_pop(&queue_le_f,&queue_le_r);
                    if(train_le->train_id==-1) {//low priority East station is empty
                        free(train_le);
                        train_hw=queue_pop(&queue_hw_f,&queue_hw_r);
                        if(train_hw->train_id==-1) {//high priority West station is empty
                            free(train_hw);
                            train_on_track=queue_pop(&queue_lw_f,&queue_lw_r);
                            last_direction=2;
                        } else { train_on_track=train_hw; last_direction=2; }
                    } else { train_on_track=train_le; last_direction=1;}
                } else { train_on_track=train_he; last_direction=1;}    
            break;
                
            case 1:/*Last train came from East*/    
            /*Since last train came from left, we first check the West side */
    
                train_hw=queue_pop(&queue_hw_f,&queue_hw_r);
                if(train_hw->train_id==-1) {//high priority west is empty
                    free(train_hw);
                    train_lw=queue_pop(&queue_lw_f,&queue_lw_r);
                    if(train_lw->train_id==-1) {//low priority west is empty
                        free(train_lw);
                        train_he=queue_pop(&queue_he_f,&queue_he_r);
                        if(train_he->train_id==-1) {//high priority east is empty
                            free(train_he);
                            train_on_track=queue_pop(&queue_le_f,&queue_le_r);
                            last_direction=1;
                        } else { train_on_track=train_he; last_direction=1; }
                    } else { train_on_track=train_lw; last_direction=2;}
                } else { train_on_track=train_hw; last_direction=2;}        
            break;
            
            case 2:/*Last train came from the right*/
            /*Since last train came from the West, we first check the East side */
                train_he=queue_pop(&queue_he_f,&queue_he_r);
                if(train_he->train_id==-1) {//high priority east station is empty
                    free(train_he);
                    train_le=queue_pop(&queue_le_f,&queue_le_r);
                    if(train_le->train_id==-1) {//low priority east station is empty
                        free(train_le);
                        train_hw=queue_pop(&queue_hw_f,&queue_hw_r);
                        if(train_hw->train_id==-1) {//high priority west station is empty
                            free(train_hw);
                            train_on_track=queue_pop(&queue_lw_f,&queue_lw_r);
                            last_direction=2;
                        } else { train_on_track=train_hw; last_direction=2; }
                    } else { train_on_track=train_le; last_direction=1;}
                } else { train_on_track=train_he; last_direction=1;}    
    
            break;
        }   
       /*  Unlock queues */    
        pthread_mutex_unlock(&queue_le);
        pthread_mutex_unlock(&queue_he);
        pthread_mutex_unlock(&queue_hw);
        pthread_mutex_unlock(&queue_lw);
    
        /* Nothing in queue, wait for something */
        if(train_on_track->train_id==-1) {
            continue;
        }    
    
        /* Direction code to human readable string */
        switch(last_direction) {
            case 1:
                last_direction_human="East";
            break;
            case 2:
                last_direction_human="West";
        }
        
        /* Train in crossing the main track */        
        printf("Train %2d is ON the main track going %4s\n", train_on_track->train_id,last_direction_human);   
        #ifdef VERBOSE_DEBUG
        printf("Train info: crossing time: %d\n",train_on_track->crossing_time);
        #endif
        usleep(train_on_track->crossing_time*TIME_DELAY);
        printf("Train %2d is OFF the main track after going %4s\n", train_on_track->train_id, last_direction_human);
        
        trains_in_queue--;
        /* Destroy train */
        free(train_on_track);    
    }
    
    pthread_exit(NULL);
}
