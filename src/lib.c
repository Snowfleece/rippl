#include <stdio.h>
#include "lib.h"
#include "thunk.h"
#include "mymap.h"
#include <string.h>
#include "natives.h"

struct Thunk add_init_thunk[1];
struct Thunk sub_init_thunk[1];
struct Thunk mult_init_thunk[1];
struct Thunk divi_init_thunk[1];
struct Thunk mod_init_thunk[1];
struct Thunk powe_init_thunk[1];
struct Thunk eq_init_thunk[1];
struct Thunk neq_init_thunk[1];
struct Thunk geq_init_thunk[1];
struct Thunk leq_init_thunk[1];
struct Thunk less_init_thunk[1];
struct Thunk neg_init_thunk[1];

struct Thunk addf_init_thunk[1];
struct Thunk subf_init_thunk[1];
struct Thunk multf_init_thunk[1];
struct Thunk divf_init_thunk[1];
struct Thunk powef_init_thunk[1];
struct Thunk eqf_init_thunk[1];
struct Thunk neqf_init_thunk[1];
struct Thunk geqf_init_thunk[1];
struct Thunk leqf_init_thunk[1];
struct Thunk lessf_init_thunk[1];
struct Thunk greaterf_init_thunk[1];
struct Thunk negf_init_thunk[1];

struct Thunk andb_init_thunk[1];
struct Thunk orb_init_thunk[1];
struct Thunk notb_init_thunk[1];

struct Thunk cons_init_thunk[1];
struct Thunk cat_init_thunk[1];
struct Thunk length_init_thunk[1];
struct Thunk head_init_thunk[1];
struct Thunk tail_init_thunk[1];

struct Thunk first_init_thunk[1];
struct Thunk second_init_thunk[1];

struct Thunk *makeInt(int x) {
	int *i = malloc(sizeof(int));
	*i = x;
	return init_thunk_literal(i);
}

struct Thunk *makeBool(char x) {
	return makeChar(x);
}	

struct Thunk *makeChar(char x) {
	char *b = malloc(sizeof(char));
	*b = x;
	return init_thunk_literal(b);
}

struct Thunk *makeFloat(float x) {
	float *f = malloc(sizeof(float));
	*f = x;
	return init_thunk_literal(f);
}

struct Tuple *makeTuple(struct Thunk *data1, struct Thunk *data2, int t1, int t2) {
	struct Tuple *newtup = malloc(sizeof(struct Tuple));

	newtup->t1 = t1;
	newtup->t2 = t2;

	newtup->first = data1;
	newtup->second = data2;

	return newtup;
}

struct Maybe *makeMaybe(struct Thunk *data, int ty) {
	struct Maybe *may = malloc(sizeof(struct Maybe));
	if (data) {
		may->is_none = 0;
	} else {
		may->is_none = 1;
	}

	may->data = data;
	return may;
}

struct Thunk *makeEmptyList(int ty) {
	struct List *new = malloc(sizeof(struct List));	
	memset(new,0,sizeof(struct List));

	new->start = 0;
	new->end = 0;
	new->curr_index = -1;
	new->last_eval = NULL;
	new->type = LITLIST;
	new->content_type = ty;

	
	return init_thunk_literal(new);
}

struct Thunk *makeRangeList(struct Thunk *start, struct Thunk *end) {
	struct List *list = malloc(sizeof(struct List));
	list->start = *(int *)(invoke(start));
	list->end = *(int *)(invoke(end));
	list->content_type = INT;
	list->type = RANGE;
	list->curr_index = list->start;

	struct Thunk *data = makeInt(list->start);
	list->head = makeNode(data);
	list->last_eval = list->head;

	explodeRangeList(list);

	return init_thunk_literal(list);
}

struct Thunk *makeInfinite(int start) {
	struct List *list = malloc(sizeof(struct List));

	list->content_type = INT;
	list->type = INFINITE;
	list->start = start;
	list->end = -1;
	list->last_eval = 0;
	list->curr_index = start;

	struct Thunk *data = makeInt(start);
	list->head = makeNode(data);	
	
	list->last_eval = list->head;
	return init_thunk_literal(list);
}

struct Node *makeNode(struct Thunk *data_thunk) {
	struct Node *new = malloc(sizeof(struct Node));

	new->data = data_thunk;
	new->next = NULL;
	return new;
}


void explodeRangeList(void *list) {
	struct List *llist = (struct List *)list;
	
	while (llist->curr_index < llist->end) {
		evalNextNode(list);
	}
}

void evalNextNode(void *list) {
	struct List *llist = (struct List *)list;
	
	if (llist->type == RANGE || llist->type == INFINITE) {
		llist->curr_index++;
		struct Thunk *data = makeInt(llist->curr_index);
		struct Node *newNode = makeNode(data);
		llist = appendNode(llist, newNode);
	}
}

struct List *appendNode(struct List *list, struct Node *node) {
	if (!(list->head)) {
		list->head = node;
		list->last_eval = node;
	} else {
		(list->last_eval)->next = node;
		list->last_eval = node;
	}
	return list;
}

struct Thunk *appendNodeThunk(struct Thunk *list, struct Node *node) {
	return init_thunk_literal(appendNode(invoke(list),node));
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

void printAnyThunk(struct Thunk *primThunk, int ty) {
	void *thing = primThunk->value;
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

void printList(void *list_thunk) {
	// TODO lol does this work 
/*	struct List *list = invoke(list_thunk);

	int type = llist->type;	
	struct Node *curr = llist->head;		
	int content_type = llist->content_type;

	if (type == RANGE) {
		printRangeList(list);
	} else if (type == INFINITE) {
		printInfinteList(list);
	} else if (type == COMP) {
		//TODO
	} else  {
		printPrimList(list);
	}*/
}

void printPrimList(struct Thunk *list_thunk) {
	struct List *list = invoke(list_thunk);
	
	int ty = list->content_type;
	struct Node *curr = list->head;

	if (ty!= CHAR)
		printf("[");
	while (curr) {
		invoke(curr->data);
		printAny((curr->data)->value, ty);	
		curr = curr->next;
		if (curr && ty != CHAR) {
			printf(", ");
		}
	}	
	if (ty != 2)
		printf("]");
}

void printInfinteList(void *list) {
	struct List *llist = (struct List*) list;
	
	int ty = llist->content_type;
	struct Node *head = llist->head;

	printf("[");
	printAny((head->data)->value, ty);
	printf("...]");
}

void printRangeList(struct Thunk *list_thunk) {
	struct List *llist = invoke(list_thunk);
	
	int ty = llist->content_type;
	struct Node *head = llist->head;	

	if (llist->curr_index == llist->end) {
		printPrimList(list_thunk);
	} else {
		explodeRangeList(llist);
		printPrimList(list_thunk);
	}
}

void printCompList(void *list);

void printTuple(void *tup) {
	struct Tuple *tupl = (struct Tuple*)tup;
	int t1 = tupl->t1;
	int t2 = tupl->t2;

	printf("(");
	printAny(((tupl->first))->value, t1);		
	printf(", ");
	printAny((tupl->second)->value, t2);
	printf(")");	
}

void printMaybe(void *may) {
	struct Maybe* mayb = (struct Maybe*)may;
	int ty = mayb->ty;
	if (mayb->is_none) {
		printf("None");
	} else {
		printf("Some ");
		printAny((mayb->data)->value, ty);
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

void printBool(char b) {
    printf("%s", b != 0 ? "true" : "false");
}


struct Thunk *makeIte(struct Thunk *cond_thunk, struct Thunk *then_thunk, 
	struct Thunk *else_thunk){
	
    void *val = invoke(cond_thunk);
    char boolean_val = *(char *)(val);
    if(boolean_val){
        return then_thunk;	
    } else {
	return else_thunk;
    }
}

void initNativeThunks() {
	// Integer operations
	init_thunk(add_init_thunk, &add_eval, 2);
	init_thunk(sub_init_thunk, &sub_eval, 2);
	init_thunk(mult_init_thunk, &mult_eval, 2);
	init_thunk(divi_init_thunk, &divi_eval, 2);
	init_thunk(mod_init_thunk, &mod_eval, 2);
	init_thunk(powe_init_thunk, &powe_eval, 2);
	init_thunk(eq_init_thunk, &eq_eval, 2);
	init_thunk(neq_init_thunk, &neq_eval, 2);
	init_thunk(geq_init_thunk, &geq_eval, 2);
	init_thunk(leq_init_thunk, &leq_eval, 2);
	init_thunk(powe_init_thunk, &powe_eval, 2);
	init_thunk(eq_init_thunk, &eq_eval, 2);
	init_thunk(neq_init_thunk, &neq_eval, 2);
	init_thunk(geq_init_thunk, &geq_eval, 2);
	init_thunk(less_init_thunk, &less_eval, 2);
	init_thunk(greater_init_thunk, &greater_eval, 2);
	init_thunk(neg_init_thunk, &neg_eval, 1);

	// Float operations
	init_thunk(addf_init_thunk, &addf_eval, 2);
	init_thunk(subf_init_thunk, &subf_eval, 2);
	init_thunk(multf_init_thunk, &multf_eval, 2);
	init_thunk(divf_init_thunk, &divf_eval, 2);
	init_thunk(powef_init_thunk, &powef_eval, 2);
	init_thunk(eqf_init_thunk, &eqf_eval, 2);
	init_thunk(neqf_init_thunk, &neqf_eval, 2);
	init_thunk(geqf_init_thunk, &geqf_eval, 2);
	init_thunk(leqf_init_thunk, &leqf_eval, 2);
	init_thunk(powef_init_thunk, &powef_eval, 2);
	init_thunk(eqf_init_thunk, &eqf_eval, 2);
	init_thunk(neqf_init_thunk, &neqf_eval, 2);
	init_thunk(geqf_init_thunk, &geqf_eval, 2);
	init_thunk(lessf_init_thunk, &lessf_eval, 2);
	init_thunk(greaterf_init_thunk, &greaterf_eval, 2);
	init_thunk(negf_init_thunk, &negf_eval, 1);

	// Boolean operations
	init_thunk(andb_init_thunk, &andb_eval, 2);
	init_thunk(orb_init_thunk, &orb_eval, 2);
	init_thunk(notb_init_thunk, &notb_eval, 1);

	// List operations
	init_thunk(cons_init_thunk, &cons_eval, 2);
	init_thunk(cat_init_thunk, &cat_eval, 2);
	init_thunk(length_init_thunk, &length_eval, 1);
	init_thunk(head_init_thunk, &head_eval, 1);
	init_thunk(tail_init_thunk, &tail_eval, 1);

	// Tuple operations
	init_thunk(first_init_thunk, &first_eval, 1);
	init_thunk(second_init_thunk, &second_eval, 1);

}
