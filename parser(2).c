#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "train.h"
#include "defines.h"

/*
* load_train_info(char *input_line)
* Returns a pointer to a the new train struct that contains
* the information taken from the input line
*/
train* 
load_train_info(int train_id,char *input_line)
{
    struct train *atrain;
    char *strtok_result=NULL;
    
    /* Our delimeters - We need them to parse the input string */    
    char *comma_delimeter=",";
    char *colon_delimeter=":";
    char *newline_del="\n";
    
    /* We can assume we will always get some data but just in case, a very basic error check */
    if(input_line==NULL) {
        printf("Error while reading input\n");
        exit(ERR);
    }
    
    atrain=malloc(sizeof(struct train));
    if(atrain==NULL) {
        printf("Error while allocating memory. Exiting.\n");
        exit(ERR);
    }
    
    /*Assign direction*/
    strtok_result=strtok(input_line,colon_delimeter);
    atrain->direction=*strtok_result;    
    /*Assign loading time*/
    strtok_result=strtok(NULL,comma_delimeter);
    atrain->loading_time=atoi(strtok_result); 
    /*Assign crossing time*/
    strtok_result=strtok(NULL,newline_del);
    atrain->crossing_time=atoi(strtok_result);     
    /*Assign train id */   
    atrain->train_id=train_id;
    
    #ifdef VERBOSE_DEBUG
        printf("Train [%d]:\n",atrain->train_id);
        printf("loading time: %d\n",atrain->loading_time);
        printf("crossing time: %d\n",atrain->crossing_time);
        printf("direction: %c\n\n",atrain->direction);
    #endif
    
    return atrain;
}
