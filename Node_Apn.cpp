#include "Node_Apn.h"


node* addNode(node* home)
{
    node *a;
    a                   = gotoLastNode(home);
    a->next             = (node*) malloc(sizeof(node));
    
    a->next->prev       = a;
    a->next->next       = NULL;
    a->next->id         = a->id + 1;
    a->next->n = 0;
    return a->next;
}


node* gotoLastNode(node* home)
{
    node *a;
    a = home;
    while(a->next != NULL){
        a = a->next;
    }
    return a;
}

void configNode(node* N, char* nodeName, char* nodeUnit, uint8_t freq)
{
	N->name = nodeName;
	N->unit = nodeUnit;
	N->frequency = freq;
	N->flagCalculate = 0;
	N->flagStream = 0;
}
