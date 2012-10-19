void *create_train(void *train_info);
void *init_controller();
void queue_insert(node **front, node **rear, train *new_train);
void queue_list(node *front);
node* queue_pop(node **front,node **rear);
train* load_train_info(int train_id, char *input);
