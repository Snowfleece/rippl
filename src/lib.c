#include <stdio.h>
#include <stdlib.h>

#define	INT			0
#define	BOOL		1
#define	CHAR		2
#define	FLOAT		3
#define	LIST		4
#define	TUPLE		5
#define MAYBE		6

#define RANGE		0
#define INFINITE	1
#define COMP		2
#define LITLIST		3

struct Tuple {
	int t1;
	int t2;
	void *first;
	void *second;
};

struct Maybe {
	int ty;
	int is_none;
	void *data;
};

struct Node {
	void *data;
	struct Node *next;
};

struct ListComp {
	struct Node *head;
	struct Node *last_eval;
	int curr_index; 
	int start;
	int end;
	int content_type;
	int type;
};

void printPrim(void *data, int ty);
void printAny(void *thing, int ty);
void printList(void *list);
void printTuple(void *tup);
void printMaybe(void *may);
void printPrimList(void *list);
void printRangeList(void *list);
void printInfinteList(void *list);
void printCompList(void *list);

int *makeInt(int x);
char *makeBool(char x);
char *makeChar(char x);
float *makeFloat(float x);
struct Node *makeNode(void *data);
struct ListComp *makeEmptyList(int ty);
struct ListComp *makeInfinite(int start);
struct ListComp *makeListRange(int start, int end);
struct Tuple *makeTuple(void *data1, void *data2, int t1, int t2);
struct Maybe *makeMaybe(void *data, int ty);

void explodeRangeList(void *list);
void evalNextNode(void *list);

int *makeInt(int x) {
	int *i = malloc(4);
	*i = x;
	return i;
}

char *makeBool(char x) {
	return makeChar(x);
}	

char *makeChar(char x) {
	char *b = malloc(1);
	*b = x;
	return b;
}

float *makeFloat(float x) {
	float *f = malloc(8);
	*f = x;
	return f;
}

struct Node *makeNode(void *data) {
	struct Node *new = malloc(sizeof(struct Node));
	new->data = data;
	new->next = NULL;
	return new;
}

struct Tuple *makeTuple(void *data1, void *data2, int t1, int t2) {
	struct Tuple *newtup = malloc(sizeof(struct Tuple));

	newtup->t1 = t1;
	newtup->t2 = t2;
	newtup->first = data1; //TODO: does this work lol
	newtup->second = data2;

	return newtup;
}

struct Maybe *makeMaybe(void *data, int ty) {
	struct Maybe *may = malloc(sizeof(struct Maybe));
	if (data) {
		may->is_none = 0;
	} else {
		may->is_none = 1;
	}
	may->data = data;
	return may;
}

struct ListComp *makeEmptyList(int ty) {
	struct ListComp *new = malloc(sizeof(struct ListComp));	

	new->start = 0;
	new->end = 0;
	new->curr_index = -1;
	new->last_eval = NULL;
	new->type = LITLIST;
	new->content_type = ty;

	return new;
}

struct ListComp *makeListRange(int start, int end) {
	struct ListComp *list = malloc(sizeof(struct ListComp));
	list->start = start;
	list->end = end;
	list->content_type = INT;
	list->type = RANGE;
	list->curr_index = start;

	int *data = makeInt(start);
	list->head = makeNode(data);

	list->last_eval = list->head;

	return list;
}

struct ListComp *makeInfinite(int start) {
	struct ListComp *list = malloc(sizeof(struct ListComp));

	list->content_type = INT;
	list->type = INFINITE;
	list->start = start;
	list->end = -1;
	list->last_eval = 0;
	list->curr_index = start;

	int *data = makeInt(start);
	list->head = makeNode(data);	
	
	list->last_eval = list->head;
	return list;
}

void explodeListRange(void *list) {
	struct ListComp *llist = (struct ListComp *)list;
	
	while (llist->curr_index < llist->end) {
		evalNextNode(list);
	}
}

void evalNextNode(void *list) {
	struct ListComp *llist = (struct ListComp *)list;
	
	llist->curr_index++;
	int *data = makeInt(llist->curr_index);
	struct Node *newNode = makeNode(data);
	(llist->last_eval)->next = newNode;
	llist->last_eval = newNode;
}

void printAny(void *thing, int ty) {
	if (ty <= FLOAT) {
		printPrim(thing, ty);
	} else if (ty == LIST) {
		printList(thing);
	} else if (ty == TUPLE) {
		printTuple(thing);
	} else if (ty == MAYBE) {
		printMaybe(thing);
	}
}

void printList(void *list) {
	struct ListComp *llist = (struct ListComp*) list;

	int type = llist->type;	
	struct Node *curr = llist->head;		
	int content_type = llist->content_type;

	if (type == RANGE) {
		printRangeList(list);
	} else if (type == INFINITE) {
		printInfinteList(list);
	} else if (type == COMP) {
		//TODO
	} else /* LISTLIT */ {
		printPrimList(list);
	}
}

void printPrimList(void *list) {
	struct ListComp *llist = (struct ListComp*) list;
	
	int ty = llist->content_type;
	struct Node *curr = llist->head;

	printf("[");
	while (curr) {
		printAny(curr->data, ty);
		curr = curr->next;
		if (curr->next) {
			printf(", ");
		}
	}	
	printf("]");
}

void printInfinteList(void *list) {
	struct ListComp *llist = (struct ListComp*) list;
	
	int ty = llist->content_type;
	struct Node *head = llist->head;

	printf("[");
	printAny(head->data, ty);
	printf("...]");
}

void printRangeList(void *list) {
	struct ListComp *llist = (struct ListComp*) list;
	
	int ty = llist->content_type;
	struct Node *head = llist->head;

	if (llist->curr_index == llist->end) {
		printPrimList(list);
	} else {
		explodeListRange(list);
		printPrimList(list);
	}
}

void printCompList(void *list);

void printTuple(void *tup) {
	struct Tuple *tupl = (struct Tuple*)tup;
	int t1 = tupl->t1;
	int t2 = tupl->t2;

	printf("(");
	printAny(tupl->first, t1);		
	printf(", ");
	printAny(tupl->second, t2);
	printf(")");	
}

void printMaybe(void *may) {
	struct Maybe* mayb = (struct Maybe*)may;
	int ty = mayb->ty;
	if (mayb->is_none) {
		printf("None");
	} else {
		printf("Some ");
		printAny(mayb->data, ty);
	}
}

void printPrim(void *data, int ty) {
	if (ty == INT) {
		int int_data = *(int *)data;
		printf("%d",int_data);
	} else if (ty == BOOL) {
		int bool_data = *(int *)data;
		if (bool_data) {
			printf("true");
		} else {
			printf("false");
		}
	} else if (ty == FLOAT) {
		float float_data = *(float *)data;
		printf("%f",float_data);
	} else if (ty == CHAR) {
		char char_data = *(char *)data;
		printf("%c",char_data);
	}
}

