/*
Description	: Inverted Indexing using Hash Table
Date		: 10th October 2025
Author		: Sakthivel A
*/
#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define FAILURE 0
#define SUCCESS 1

typedef struct sll{
    char * f_name;
    struct sll * link; 
}Slist;

extern Slist *head;

typedef struct Sub{
    int word_count;
    char file_name[20];
    struct Sub * link;
}sub_node;

typedef struct Main{
    int file_count;
    char word[50];
    struct Sub * Slink;
    struct Main * Mlink;
}main_node;

typedef struct array{
    main_node * head;
    int ind;
}arr;

static arr hash_table[28];

void load_database();
void validate(int, char**);
FILE * file_check(char *);
int isfile_empty(FILE *);
void create_database(int, char**);
void create_hash_table();
void add_database(char *);
void display_database();
void search_database();
void update_database();
void save_exit_database();

#endif
