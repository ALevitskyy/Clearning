struct node
{int key; struct node *next;};
struct node *z;
struct node *merge(struct node *a, struct node *b)
{
  struct node *c;
  c=z;
  do
  if (a->key <= b->key)
  {c->next = a; c=a; a=a->next;}
  else
  {c->next = b; c = b; b=b->next;}
  while(c!=z);
  c = z->next; z->next = z;
  return c;
}

struct node *mergesort(struct node *c)
{
  struct node *a, *b;
  if(c->next !=z){
    a=c;b=c->next->next->next;
    while(b!=z){
      c=c->next; b = b->next->next;
    }
    b = c->next; c->next = z;
    return merge(mergesort(a), mergesort(b));
  }
  return c;
}

struct node *bottomup_mergesort(struct node *c){
  int i,N;
  struct node *a, *b, *head, *todo, *t;
  head = (struct node *) malloc(sizeof *head);
  head->next = c; a=z;
  for (N=1; a!= head->next; N=N+N)
  {
    todo = head->next; c=head;
    while (todo != z){
      t= todo; a = t;
      for (i=1; i<N; i++) t=t->next;
      b = t->next; t->next = z; t = b;
      for (i=1; i<N; i++) t = t->next;
      todo = t->next; t->next = z;
      c->next = merge(a,b);
      for (i=1; i<=N+N; i++) c= c->next;
    }
  }
  return head->next;
}
