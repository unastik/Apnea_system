#ifndef NODE_APN_H
#define NODE_APN_H

#include <Arduino.h>

typedef struct _node
{
    uint8_t id = 0;
    char* name;
    int32_t value = 0;
    uint64_t n = 0;
    uint32_t time = 0;
    uint8_t frequency = 0;
    char* unit;
    bool flagAcquired = false;
    uint8_t flagCalculate = 0;
    uint8_t flagStream = 0;
    struct _node *next,*prev;
} node;

node* addNode(node* home);
node* gotoLastNode(node* home);

void configNode(node* N, char* nodeName, char* nodeUnit, uint8_t freq);

/*
void getThermis();
void getAbdo();
void getThorax();
*/

#endif