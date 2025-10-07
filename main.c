#include "header.h"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Error: No file passed to make database\n");
        return FAILURE;
    }

    validate(argc, argv);   // will create linked list

    if (head == NULL) {
        printf("Warning: No valid files added. Please proceed with caution.\n");
    }

    create_hash_table();

    int opt;
    while (1) {
        printf("\nMenu:\n");
        printf("1. Create Database\n");
        printf("2. Display Database\n");
        printf("3. Search Database\n");
        printf("4. Update Database\n");
        printf("5. Save & Exit\n");
        printf("Enter your option: ");

        if (scanf("%d", &opt) != 1) {
            printf("Error: Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // clear buffer
            continue;
        }

        switch (opt) {
            case 1:
                create_database(argc, argv);
                break;
            case 2:
                display_database();
                break;
            case 3:
                search_database();
                break;
            case 4:
                update_database();
                break;
            case 5:
                save_exit_database();
                return 0;
            default:
                printf("Error: Invalid option\n");
                break;
        }
    }
}
