#ifndef HASHTABLE_H
#define HASHTABLE_H

#define TABLE_SIZE 1024

typedef struct Node {
    char *key;
    char *value;
    struct Node *next;
} Node;

typedef struct {
    Node *buckets[TABLE_SIZE];
} HashTable;

HashTable *ht_create();
void ht_set(HashTable *table, const char *key, const char *value);
char *ht_get(HashTable *table, const char *key);
int ht_del(HashTable *table, const char *key);
int ht_exists(HashTable *table, const char *key);

#endif
