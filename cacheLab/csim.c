#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

typedef struct{
    int s;//2^s sets
    int S;//S=2^s
    int b;//2^b per block	
    int B;//B=2^b
    int E;//number of lines per set
    int hitNum;
    int missNum;
    int evictionNum;
    int verbosity;
} cacheProperty;

typedef struct {
    int valid;
    unsigned long long tag;
    char *block;
    int usedCounter;
} setLine;

typedef struct {
    setLine *lines;
} cacheSet;

typedef struct {
    cacheSet *sets;
} cache;

//initiate the cache
cache initiate(long long S, int E) 
{
    cache currentCache;	
    cacheSet set;
    setLine line;
    currentCache.sets = (cacheSet *) malloc(sizeof(cacheSet) * S);
    for (int i = 0; i < S; i++) 
    {
        set.lines =  (setLine *) malloc(sizeof(setLine) * E);
        currentCache.sets[i] = set;
        for (int j = 0; j < E; j++) 
        {
            line.valid = 0; 
            line.tag = 0; 
            set.lines[j] = line;
            line.usedCounter = 0;
        }
    } 
    return currentCache;
}
//verbosity
int verbosity = 0;

//print usage message
void printUsage(){
    printf("Usage: ./csim [-h] [-v] -s <s> -E <E> -b <b> -t <tracefile>\n");
    printf("-s: number of set index(2^s sets)\n");
    printf("-E: number of lines per set\n");
    printf("-b: number of block offset bits\n");
    printf("-t: trace file name\n");
}
//method to check if hit
int checkHit(setLine line, unsigned long long tag){
    if(line.valid){
        if(line.tag == tag){
            return 1;
        }
    }
    return 0;
}
//method to check if the set is full
int checkFull(cacheSet set, cacheProperty property){
    for(int i = 0; i<property.E; i++){
        if(set.lines[i].valid == 0){
            return 1;
        }
    }
    return 0;
}
//method to find the next open line
int findIndex(cacheSet set, cacheProperty property){
    for(int i = 0; i<property.E; i++){
        if(set.lines[i].valid == 0){
            return i;
        }
    }
    //shouldn't get a -1 anyway, method only called when there is granted space
    return -1;
}
//method to find the line to evict
int findEvict(cacheSet set, cacheProperty property){
    int min = set.lines[0].usedCounter;
    int index = 0;
    for(int i = 0; i < property.E ; i++){
        if(min>set.lines[i].usedCounter){
            index = i;
            min = set.lines[i].usedCounter;
        }
    }
    return index;
}

int findMax(cacheSet set, cacheProperty property){
    int max = set.lines[0].usedCounter;
    int index = 0;
    for(int i = 0; i < property.E ; i++){
        if(set.lines[i].usedCounter>max){
            index = i;
            max = set.lines[i].usedCounter;
        }
    }
    return index;
}

cacheProperty simulate(cache currentCache,cacheProperty property, unsigned long long address){
    //compute the size of the tag, 64 bit system
    int tagSize = 64-(property.b + property.s);
    unsigned long long tag = address >> (property.s + property.b);
    //use the tagSize to compute for the set index
    unsigned long long setIndex = (address << (tagSize))>> (tagSize + property.b);
    cacheSet set = currentCache.sets[setIndex];
    //loop through lines in the set
    int hit = 0;
    for (int i = 0; i<property.E; i++){
        setLine currentLine = set.lines[i];
        //check if there is a hit
        if(checkHit(currentLine, tag) == 1){
            //if hit, update the staffs in the property
            property.hitNum+=1;
            hit = 1;
            int max = findMax(set, property);
            currentCache.sets[setIndex].lines[i].usedCounter = currentCache.sets[setIndex].lines[max].usedCounter+1;
        }
    }
    if(hit == 0 && checkFull(set, property) == 1){
    //if not full then it is a miss, update the staffs in the property&set
        property.missNum+=1;
        int index = findIndex(set, property);
        set.lines[index].tag = tag;
        set.lines[index].valid = 1;
        int max = findMax(set, property);
        currentCache.sets[setIndex].lines[index].usedCounter = currentCache.sets[setIndex].lines[max].usedCounter+1;
    }else if(hit == 0){
        //evict, update the staffs in the property&set
        property.missNum+=1;
        property.evictionNum+=1;
        //find the evict line
        int evictIndex = findEvict(set, property);
        set.lines[evictIndex].tag = tag;
        int max = findMax(set, property);
        currentCache.sets[setIndex].lines[evictIndex].usedCounter = currentCache.sets[setIndex].lines[max].usedCounter+1;
    }
    return property;
}
int main(int argc, char** argv)
{
    cache currentCache;
    cacheProperty property;
    //initiate the char to store the trace
    char* trace;
    char input;
    property.verbosity = 0;
    //parse input
    while( (input=getopt(argc,argv,"s:E:b:t:vh")) != -1)
    {
        switch(input){
        case 's':
            property.s = atoi(optarg);
            break;
        case 'E':
            property.E = atoi(optarg);
            break;
        case 'b':
            property.b = atoi(optarg);
            break;
        case 't':
            trace = optarg;
            break;
        case 'v':
            property.verbosity = 1;
            break;
        //help
        case 'h':
            printUsage();
            exit(0);
        default:
            printUsage();
            exit(-1);
        }
    }
    //compute S and B
    property.S = 1 << property.s;
    property.B = 1 << property.b;
    //initialize the cache
    currentCache = initiate(property.S, property.E);
    //initialize the counters
    property.missNum = 0;
    property.hitNum = 0;
    property.evictionNum = 0;
    //input file
    FILE *tmp;
    //wrap the trace into tmp
    char command;
    unsigned long long address;
    int size;
    tmp = fopen(trace, "r");
    while(fscanf(tmp, " %c %llx,%d", &command, &address, &size) == 3){
        switch(command){
            //just ignore I
            case 'I':
                break;
            case 'L':
                property = simulate(currentCache, property, address);
                break;
            case 'S':
                property = simulate(currentCache, property, address);
                break;
            //twice for M
            case 'M':
                property = simulate(currentCache, property, address);
                property = simulate(currentCache, property, address);	
                break;
            default:
                break;
        }
    }
    //print result
    printSummary(property.hitNum, property.missNum, property.evictionNum);
    fclose(tmp);
    return 0;
}
