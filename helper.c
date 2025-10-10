/*
Description	: Inverted Indexing using Hash Table
Date		: 10th October 2025
Author		: Sakthivel A
*/

#include "header.h"

int create_flag = 0;         // set when create_database() runs
int update_flag = 0;         // set when update_database() runs
int db_loaded_flag = 0;      // set when load_database() runs

Slist *head = NULL;          // files already loaded into hashtable (from db or created)
Slist *arg_head = NULL;      // validated command-line files (to be processed by create)

// open a file for reading
FILE *file_check(char *filename) {
    return fopen(filename, "r");
}

// return 1 if file empty
int isfile_empty(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    return (size == 0);
}

// Validate command-line files and add them to arg_head (not head)
void validate(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        char *dot = strrchr(argv[i], '.');
        if (!dot || strcmp(dot, ".txt") != 0) {
            printf("Error: %s must have .txt extension\n", argv[i]);
            continue;
        }

        FILE *fp = file_check(argv[i]);
        if (fp == NULL) {
            printf("Error: %s file not found\n", argv[i]);
            continue;
        }

        if (isfile_empty(fp)) {
            printf("Error: %s file is empty\n", argv[i]);
            fclose(fp);
            continue;
        }

        // check duplicates within arg_head (same CLI passed twice)
        int duplicate = 0;
        Slist *temp = arg_head;
        while (temp) {
            if (strcmp(temp->f_name, argv[i]) == 0) {
                printf("Error: %s is a duplicate file (skipped)\n", argv[i]);
                duplicate = 1;
                break;
            }
            temp = temp->link;
        }

        if (duplicate) {
            fclose(fp);
            continue; // skip duplicate CLI entries
        }

        // Add validated CLI filename to arg_head (not to head)
        Slist *node = malloc(sizeof(Slist));
        if (!node) {
            printf("Memory allocation failed\n");
            fclose(fp);
            return;
        }
        node->f_name = malloc(strlen(argv[i]) + 1);
        strcpy(node->f_name, argv[i]);
        node->link = NULL;

        if (arg_head == NULL)
            arg_head = node;
        else {
            Slist *last = arg_head;
            while (last->link)
                last = last->link;
            last->link = node;
        }
        printf("File %s validated successfully\n", argv[i]);
        fclose(fp);
    }
}

// initialize hash table
void create_hash_table() {
    for (int i = 0; i < 28; i++) {
        hash_table[i].head = NULL;
        hash_table[i].ind = i;
    }
    printf("Hash Table Created Successfully\n");
}

// add contents of a single file into the hashtable
void add_database(char *filename) {
    FILE *fp = file_check(filename);
    if (!fp) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    char word[50];
    while (fscanf(fp, "%49s", word) == 1) {
        if (word[0] == '\0') continue;

        int in;
        if (word[0] >= 'A' && word[0] <= 'Z') in = word[0] - 'A';
        else if (word[0] >= 'a' && word[0] <= 'z') in = word[0] - 'a';
        else if (isdigit(word[0])) in = 26;
        else in = 27;

        if (hash_table[in].head == NULL) {
            main_node *main_new = malloc(sizeof(main_node));
            sub_node *sub_new = malloc(sizeof(sub_node));
            if (!main_new || !sub_new) { printf("Memory allocation failed\n"); fclose(fp); return; }

            strcpy(main_new->word, word);
            main_new->file_count = 1;
            main_new->Slink = sub_new;
            main_new->Mlink = NULL;

            strcpy(sub_new->file_name, filename);
            sub_new->word_count = 1;
            sub_new->link = NULL;

            hash_table[in].head = main_new;
        } else {
            main_node *temp = hash_table[in].head;
            main_node *prev = NULL;
            while (temp && strcmp(temp->word, word) != 0) {
                prev = temp;
                temp = temp->Mlink;
            }

            if (!temp) {
                main_node *main_new = malloc(sizeof(main_node));
                sub_node *sub_new = malloc(sizeof(sub_node));
                if (!main_new || !sub_new) { printf("Memory allocation failed\n"); fclose(fp); return; }

                strcpy(main_new->word, word);
                main_new->file_count = 1;
                main_new->Slink = sub_new;
                main_new->Mlink = NULL;

                strcpy(sub_new->file_name, filename);
                sub_new->word_count = 1;
                sub_new->link = NULL;

                prev->Mlink = main_new;
            } else {
                sub_node *sub_temp = temp->Slink;
                sub_node *sub_prev = NULL;
                while (sub_temp && strcmp(sub_temp->file_name, filename) != 0) {
                    sub_prev = sub_temp;
                    sub_temp = sub_temp->link;
                }
                if (sub_temp) sub_temp->word_count++;
                else {
                    sub_node *sub_new = malloc(sizeof(sub_node));
                    if (!sub_new) { printf("Memory allocation failed\n"); fclose(fp); return; }
                    strcpy(sub_new->file_name, filename);
                    sub_new->word_count = 1;
                    sub_new->link = NULL;
                    sub_prev->link = sub_new;
                    temp->file_count++;
                }
            }
        }
    }

    fclose(fp);
}

// Create database using validated CLI files in arg_head
void create_database(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Error: No file passed to make database\n");
        return;
    }

    validate(argc, argv);   // will create linked list
    
    if (head == NULL) {
        printf("Warning: No valid files added. Please proceed with caution.\n");
    }

    if (create_flag) {
        printf("Error: Database already created. Please save before creating again.\n");
        return;
    }

    if (arg_head == NULL) {
        printf("No valid input files provided to create database.\n");
        return;
    }

    create_flag = 1;

    Slist *cur = arg_head;
    while (cur) {
        // skip file if already present in head (was loaded via update)
        int found = 0;
        Slist *check = head;
        while (check) {
            if (strcmp(check->f_name, cur->f_name) == 0) { found = 1; break; }
            check = check->link;
        }

        if (found) {
            printf("File %s already present in database, skipping...\n", cur->f_name);
            cur = cur->link;
            continue;
        }

        // process new file and add its content to hashtable
        printf("Processing new file: %s\n", cur->f_name);
        add_database(cur->f_name);

        // add this file to head (loaded files list)
        Slist *node = malloc(sizeof(Slist));
        if (!node) { printf("Memory allocation failed\n"); return; }
        node->f_name = malloc(strlen(cur->f_name) + 1);
        strcpy(node->f_name, cur->f_name);
        node->link = NULL;

        if (!head) head = node;
        else {
            Slist *last = head;
            while (last->link) last = last->link;
            last->link = node;
        }

        cur = cur->link;
    }

    printf("Hash Table Created/Updated Successfully\n");
}

// Display contents of hashtable
void display_database() {
    printf("------------------------------------------------------------\n");
    printf("%-6s %-20s %-20s %-10s\n", "Index", "Word", "File Name", "Word Count");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < 28; i++) {
        if (!hash_table[i].head) continue;
        main_node *main_temp = hash_table[i].head;
        while (main_temp) {
            sub_node *sub_temp = main_temp->Slink;
            int first = 1;
            while (sub_temp) {
                if (first)
                    printf("%-6d %-20s %-20s %-10d\n", i, main_temp->word, sub_temp->file_name, sub_temp->word_count);
                else
                    printf("%-6s %-20s %-20s %-10d\n", "", main_temp->word, sub_temp->file_name, sub_temp->word_count);
                sub_temp = sub_temp->link;
                first = 0;
            }
            main_temp = main_temp->Mlink;
        }
    }
    printf("------------------------------------------------------------\n");
}

// Search a word in hashtable
void search_database() {
    char search_word[50];
    printf("Enter the word to search: ");
    scanf("%49s", search_word);

    int in;
    if (search_word[0] >= 'A' && search_word[0] <= 'Z') in = search_word[0] - 'A';
    else if (search_word[0] >= 'a' && search_word[0] <= 'z') in = search_word[0] - 'a';
    else if (isdigit(search_word[0])) in = 26;
    else in = 27;

    main_node *main_temp = hash_table[in].head;
    while (main_temp) {
        if (strcmp(main_temp->word, search_word) == 0) {
            printf("------------------------------------------------------------\n");
            printf("%-6s %-20s %-20s %-10s\n", "Index", "Word", "File Name", "Word Count");
            printf("------------------------------------------------------------\n");
            sub_node *sub_temp = main_temp->Slink;
            while (sub_temp) {
                printf("%-6d %-20s %-20s %-10d\n", in, main_temp->word, sub_temp->file_name, sub_temp->word_count);
                sub_temp = sub_temp->link;
            }
            printf("------------------------------------------------------------\n");
            return;
        }
        main_temp = main_temp->Mlink;
    }

    printf("Word '%s' not found in the database.\n", search_word);
}

// Update database from saved file: load file content into hashtable and add file names found into head
void update_database()
{
    if (update_flag || create_flag) {
        printf("Error: Database already updated/created in this session. Please save before updating again.\n");
        return;
    }

    char db_file[50];
    printf("Enter the database file name to update: ");
    scanf("%49s", db_file);

    // check extension and file existence
    char *dot = strrchr(db_file, '.');
    if (!dot || strcmp(dot, ".txt") != 0) { printf("Error: Database file must have .txt extension\n"); return; }
    FILE *fp = fopen(db_file, "r");
    if (!fp) { printf("Error: File not found\n"); return; }

    // simple start/end '#' check
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size < 2) { printf("Error: Database file format invalid (too small)\n"); fclose(fp); return; }
    rewind(fp);
    char first = fgetc(fp);
    if (first == EOF) { printf("Error: Empty file\n"); fclose(fp); return; }
    char last = '\0';
    fseek(fp, -1, SEEK_END);
    while (ftell(fp) > 0) {
        last = fgetc(fp);
        if (last == '#' || (last != '\n' && last != '\r' && last != ' ' && last != '\t')) break;
        fseek(fp, -2, SEEK_CUR);
    }
    if (first != '#' || last != '#') { printf("Error: Database file format invalid (missing #)\n"); fclose(fp); return; }
    fclose(fp);

    // load database into hashtable
    load_database();

    // scan hashtable and ensure each file in subnodes exists in head (add if missing)
    for (int i = 0; i < 28; i++) {
        main_node *main_temp = hash_table[i].head;
        while (main_temp) {
            sub_node *sub_temp = main_temp->Slink;
            while (sub_temp) {
                int found = 0;
                Slist *check = head;
                while (check) {
                    if (strcmp(check->f_name, sub_temp->file_name) == 0) { found = 1; break; }
                    check = check->link;
                }
                if (!found) {
                    Slist *node = malloc(sizeof(Slist));
                    node->f_name = malloc(strlen(sub_temp->file_name) + 1);
                    strcpy(node->f_name, sub_temp->file_name);
                    node->link = NULL;
                    if (!head) head = node;
                    else {
                        Slist *last = head;
                        while (last->link) last = last->link;
                        last->link = node;
                    }
                }
                sub_temp = sub_temp->link;
            }
            main_temp = main_temp->Mlink;
        }
    }

    update_flag = 1;
    db_loaded_flag = 1;
    create_flag = 0; // allow create after update
    printf("Database updated successfully from %s\n", db_file);
}

// Save hashtable into database.txt in the required format
void save_exit_database() {
    FILE *fp = fopen("database.txt", "w");
    if (!fp) { printf("Error: Could not open database.txt for writing\n"); return; }
    fprintf(fp, "#%d\n", 28);
    for (int i = 0; i < 28; i++) {
        main_node *main_temp = hash_table[i].head;
        while (main_temp) {
            fprintf(fp, "%d;%s;%d;", i, main_temp->word, main_temp->file_count);
            sub_node *sub_temp = main_temp->Slink;
            while (sub_temp) {
                fprintf(fp, "%s;%d;", sub_temp->file_name, sub_temp->word_count);
                sub_temp = sub_temp->link;
            }
            fprintf(fp, "#\n");
            main_temp = main_temp->Mlink;
        }
    }
    fclose(fp);
    printf("Database saved successfully to database.txt. Exiting...\n");
    create_flag = update_flag = db_loaded_flag = 0;
}

// Load database.txt into hashtable (semicolon format)
void load_database() {
    FILE *fp = fopen("database.txt", "r");
    if (!fp) { printf("Error: No saved database found.\n"); return; }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        int index, file_count;
        char word[50];
        char *token = strtok(line, ";");
        if (!token) continue;
        index = atoi(token);

        token = strtok(NULL, ";");
        if (!token) continue;
        strcpy(word, token);

        token = strtok(NULL, ";");
        if (!token) continue;
        file_count = atoi(token);

        main_node *main_new = malloc(sizeof(main_node));
        strcpy(main_new->word, word);
        main_new->file_count = file_count;
        main_new->Mlink = NULL;
        main_new->Slink = NULL;

        sub_node *sub_prev = NULL;
        for (int i = 0; i < file_count; i++) {
            char *fname = strtok(NULL, ";");
            char *count_str = strtok(NULL, ";");
            if (!fname || !count_str) break;

            sub_node *sub_new = malloc(sizeof(sub_node));
            strcpy(sub_new->file_name, fname);
            sub_new->word_count = atoi(count_str);
            sub_new->link = NULL;

            if (sub_prev == NULL) main_new->Slink = sub_new;
            else sub_prev->link = sub_new;
            sub_prev = sub_new;
        }

        if (hash_table[index].head == NULL) hash_table[index].head = main_new;
        else {
            main_node *temp = hash_table[index].head;
            while (temp->Mlink) temp = temp->Mlink;
            temp->Mlink = main_new;
        }
    }

    fclose(fp);
    db_loaded_flag = 1;
    printf("Database loaded successfully.\n");
}
