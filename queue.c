/*
* queue.c
* This file contains an implementation of a queue using a doubly linked list.
* Queues represent stations.
*/

#include <stdio.h>
#include <stdlib.h>
#include "train.h"
#include "node.h"
#include "defines.h"

/*
* queue_insert(node **front,node **rear, train *new_train)
* Inserts a new element in our queue.
*/
void
queue_insert(node **front,node **rear, train *new_train)
{
    node *newnode;      /* New node to insert */    
    newnode=malloc(sizeof(node));
    if(newnode==NULL) {
        printf("Error while allocating memory\n");
        exit(ERR);
    }
    
    /* Set new node data */
    newnode->next=NULL;
    newnode->prev=NULL;
    newnode->train_id=new_train->train_id;
    newnode->crossing_time=new_train->crossing_time;
    
    if(*front==NULL && *rear==NULL) {    /* Queue is empty */
        *front=newnode;
        *rear=newnode;
    } else if (*front==NULL || *rear==NULL) {    /* There is something wrong with our data */
        printf("Error: Corrupt data\n");
        exit(ERR);
    } else {    /* Insert at rear   */
        (*rear)->next=newnode;  
        newnode->prev=*rear;
        *rear=newnode;
    }
}

/*
* queue_pop(node **front,node **rear)
* Returns first element in queue
*/ 
node* 
queue_pop(node **front,node **rear)
{
    node *delnode;      /* Node to pop */

    if((*front)==NULL && (*rear)==NULL) {    /* Queue is empty */
        #ifdef VERBOSE_DEBUG
        printf("Queue is empty. Can't pop\n");
        #endif
        delnode=malloc(sizeof(struct node));
        delnode->train_id=-1; 
    } else if ((*front)==NULL || (*rear)==NULL) { /* We have corrupt data */
        printf("Error: Corrupt Data\n");
        exit(ERR);
    } else { /* Return element at front of queue */
        delnode=*front;
        (*front)=(*front)->next;
        if( NULL == *front ) { /* if we pop the last item */
            *rear = *front;
        } else {
            (*front)->prev = NULL;
        }   
    }
    return delnode;
}

/*
* queue_list(node *f)
* Displays elements contained in the queue.
*/
void
queue_list(node *f)
{
    while(f!=NULL)
    {
        printf("[%d]",f->train_id);
        f=f->next;
    }
    printf("\n");
}
