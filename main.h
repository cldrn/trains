/* Mutex and cond vars necessary to ensure trains start loading at the same time */
extern pthread_mutex_t loadingcond_mutex;
extern pthread_cond_t  loading_cond;
/* Mutexes to guard the queues */
extern pthread_mutex_t queue_le;
extern pthread_mutex_t queue_he;
extern pthread_mutex_t queue_lw;
extern pthread_mutex_t queue_hw;
/* Mutexes and boolean flag to indicate theres something in queue*/
extern pthread_mutex_t emptyqueue_mutex;
extern pthread_cond_t  queue_cond;

int input_loaded;
int empty_queue;
int last_direction;
int trains_in_queue;

/*Low priority East station*/
node *queue_le_f;
node *queue_le_r;
/*High priority East station*/
node *queue_he_f;
node *queue_he_r;
/*Low priority West station*/
node *queue_lw_f;
node *queue_lw_r;
/*High priority West station*/
node *queue_hw_f;
node *queue_hw_r;
