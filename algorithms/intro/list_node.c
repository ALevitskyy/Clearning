struct node {
  int key;
  struct node *next;
};
struct node *head, *z, *t;
listinitialize(){
  head = (struct node *) malloc(sizeof *head);
  z = (struct node *) malloc(sizeof *z);
  head->next = z; head->key = 0;
  z->next = z;
};

void deletenext(struct node *t) {
  struct node *temp = t->next; // Store the node to be deleted
  t->next = t->next->next;    // Update the next pointer
  free(temp);                 // Free the memory of the deleted node
}

void deletelist() {
  struct node *current = head->next; // Start from the first node after the head
  struct node *temp;

  while (current != z) { // Stop when reaching the sentinel node `z`
    temp = current;      // Store the current node
    current = current->next; // Move to the next node
    free(temp);          // Free the current node
  }

  head->next = z; // Reset the list to its initial state
}

struct node *insertafter(int v, struct node *t){
  struct node *x;
  x = (struct node *) malloc(sizeof *x);
  x->key = v; x-> next = t->next;
  t->next = x;
  return x;
}
