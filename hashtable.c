#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

unsigned int hash_func(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = hash * 31 + *key;
        key++;
    }
    return hash % TABLE_SIZE;
}

HashTable *ht_create() {
    HashTable *table = malloc(sizeof(HashTable));
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    return table;
}

void ht_set(HashTable *table, const char *key, const char *value) {
    unsigned int index = hash_func(key);
    Node *curr = table->buckets[index];

    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            free(curr->value);
            curr->value = strdup(value);
            return;
        }
        curr = curr->next;
    }

    Node *new_node = malloc(sizeof(Node));
    new_node->key = strdup(key);
    new_node->value = strdup(value);
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
}

char *ht_get(HashTable *table, const char *key) {
    unsigned int index = hash_func(key);
    Node *curr = table->buckets[index];

    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            return curr->value;
        }
        curr = curr->next;
    }
    return NULL;
}

int ht_del(HashTable *table, const char *key) {
    unsigned int index = hash_func(key);
    Node *curr = table->buckets[index];
    Node *prev = NULL;

    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            if (prev == NULL) {
                table->buckets[index] = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr->key);
            free(curr->value);
            free(curr);
            return 1;
        }
        prev = curr;
        curr = curr->next;
    }
    return 0;
}

int ht_exists(HashTable *table, const char *key) {
    return ht_get(table, key) != NULL;
}
