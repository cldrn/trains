#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "train.h"
#include "node.h"
#include "prototypes.h"
#include "defines.h"

/* Mutex and cond vars necessary to ensure trains start loading at the same time */
pthread_mutex_t loadingcond_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  loading_cond=PTHREAD_COND_INITIALIZER;
/* Mutexes and boolean flag to indicate theres something in queue*/
pthread_mutex_t emptyqueue_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  queue_cond=PTHREAD_COND_INITIALIZER;
/*Mutexes protecting the 'stations'/queues*/
pthread_mutex_t queue_le=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queue_he=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queue_lw=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queue_hw=PTHREAD_MUTEX_INITIALIZER;

int input_loaded;    /* Flag: Ensures trains start loading at the same time */ 
int empty_queue;    /* Flag: Indicates the state of the queues */
int trains_in_queue;    /* Number of trains left in queues */
int last_direction;    /* Flag: Last train direction  */

/*Low priority East station - Front and Rear ptrs: Doubly Linked List Queue */
node *queue_le_f;
node *queue_le_r;
/*High priority East station - Front and Rear ptrs: Doubly Linked List Queue */
node *queue_he_f;
node *queue_he_r;
/*Low priority West station - Front and Rear ptrs: Doubly Linked List Queue */
node *queue_lw_f;
node *queue_lw_r;
/*High priority West station - Front and Rear ptrs: Doubly Linked List Queue */
node *queue_hw_f;
node *queue_hw_r;

/*
* Main method - 2 args required 
* Main function loads input file and creates a train per input entry. 
* It emulates a railway system with 4 stations and 1 track as specified in 
*/
int 
main(int argc, char *argv[]) 
{
    int i,train_num;
    FILE *fp;
    char input_string[MAX_LINE_LENGTH];
    
    input_loaded=0;    /* Set Flag: Input hasn't been loaded */
    empty_queue=0;    /* Set Flag: Queues are empty */   
    last_direction=0;    /* Set Flag: No train has crossed the track */
    
    /*Detect incorrect arg number */
    if(argc!=3) {
        printf("Usage: %s <Input file> <Number of trains to load>\n", argv[0]);
        exit(ERR);
    }
    trains_in_queue=train_num=atoi(argv[2]);
    
    /*Open input file*/
    fp = fopen(argv[1], "r");
    if(fp==NULL) {
        printf("Error: can't open input file:%s\n",argv[1]);
        exit(ERR);
    }

    train *trains[train_num];

    pthread_t train_threads[train_num];
    pthread_t train_controller;
    
    /*Get input from file*/
    /*we can assume that file contains at least argv[2] trains so lets use a loop*/
    for(i=0;i<train_num;i++) {
        fgets(input_string, MAX_LINE_LENGTH, fp);
        
        int input_l=strlen(input_string);
        char *train_info;
                
        #ifdef VERBOSE_DEBUG
            printf("String loaded from file:%s", input_string);
        #endif
        
        train_info=malloc(sizeof(char)*input_l);
        if(train_info==NULL) {
            printf("Error while allocating memory.\n");
            exit(ERR);
        }
        
        strncpy(train_info,input_string,input_l);
        trains[i]=load_train_info(i,train_info);/* Loads train info into struct */
        pthread_create(&train_threads[i],NULL,create_train,(void *)trains[i]);
        free(train_info);
    }
    
    /*We have loaded all the input, signal trains to start loading */
    pthread_mutex_lock(&loadingcond_mutex);
    input_loaded=1;
    pthread_cond_broadcast(&loading_cond);
    pthread_mutex_unlock(&loadingcond_mutex);
    
    #ifdef VERBOSE_DEBUG
        printf("[%d] trains have been loaded in memory\n",train_num);
    #endif
    
    /*Initiliaze controller thread*/
    pthread_create(&train_controller,NULL,init_controller,NULL);
    
    /* Wait until all the trains have crossed the path */
    for(i=0;i<train_num;i++) 
        pthread_join( train_threads[i], NULL);    
    pthread_join(train_controller,NULL);  
    
    #ifdef VERBOSE_DEBUG
    /* Just to make sure all the queues are empty at the end */
    printf("HP West Queue:\n");
    queue_list(queue_hw_f);
    printf("LP West Queue:\n");    
    queue_list(queue_lw_f);
    printf("HP East Queue:\n");
    queue_list(queue_he_f);
    printf("LP East Queue:\n");
    queue_list(queue_le_f);    
    #endif  
      
    /*Destroy mutexes and cond vars */
    pthread_cond_destroy(&loading_cond);
    pthread_mutex_destroy(&loadingcond_mutex);

    pthread_mutex_destroy(&queue_le);
    pthread_mutex_destroy(&queue_he);
    pthread_mutex_destroy(&queue_lw);
    pthread_mutex_destroy(&queue_hw);

    /*Free structs*/
    for(i=0;i<train_num;i++)
        free(trains[i]);
    
    fclose(fp);
    
    return OK;
}
