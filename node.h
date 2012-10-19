typedef struct node {
    int train_id;
    int crossing_time;
    struct node *next;
    struct node *prev;
} node;
