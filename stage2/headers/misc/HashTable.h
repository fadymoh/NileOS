#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "includes.h"

// typedef struct RegistryServiceValue_t
// {
//     int coreCount;
//     int coreID[(1 << 8)];
// } RegistryServiceValue;

// typedef struct MACIdentifier_t
// {
//     char *key; // MAC String in our current case
//     RegistryServiceValue *value;
// } MACIdentifier;

// typedef struct MACHashTable_t
// {
//     MACIdentifier *array;
//     int capacity;
//     int size;
// } MACHashTable;

typedef struct IPIdentifier_t
{
    char MACAddress[10][6];  // 10 MAC Addresses each of size 6
    int coreCount[10];       // 10 integers for each MAC Address's core count
    int coreID[10][(1 << 8)] // 10 arrays for each MAC Address's core IDs containing that service
} IPIdentifier;

typedef struct IPHashTable_t
{
    IPIdentifier *array;
    int capacity;
    int size;
} IPHashTable;

typedef struct ServiceDataIdentifier_t
{
    char *key; // Service Name in our current case
    IPHashTable *value;
} ServiceDataIdentifier;

// array of service identifiers
typedef struct GlobalHashTable_t
{
    ServiceDataIdentifier *array;
    int capacity;
    int size;
} GlobalHashTable;

uint64_t hashcode(uint64_t capacity, char *key);

//void insert(GlobalHashTable *table, char *serviceName, char *IP, char *MAC, RegistryServiceValue *value);
//RegistryServiceValue *GetElementFromHashTable(GlobalHashTable *table, char *key);
void init_array(GlobalHashTable *table);
void display(GlobalHashTable *table);

#endif