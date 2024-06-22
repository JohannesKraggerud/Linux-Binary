#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_PATH_LENGTH 1024
#define MAX_FILES 1000

void search_files(const char *base_path, const char *filename, char files[][MAX_PATH_LENGTH], int *file_count);
void edit_file(const char *filepath);
void prompt_user_to_choose_file(char files[][MAX_PATH_LENGTH], int file_count);
char **filename_completion(const char *text, int start, int end);
char *filename_generator(const char *text, int state);

char files[MAX_FILES][MAX_PATH_LENGTH];
int file_count = 0;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    rl_attempted_completion_function = filename_completion;

    search_files(".", filename, files, &file_count);

    if (file_count == 0) {
        printf("File '%s' not found.\n", filename);
    } else if (file_count == 1) {
        edit_file(files[0]);
    } else {
        prompt_user_to_choose_file(files, file_count);
    }

    return 0;
}

void search_files(const char *base_path, const char *filename, char files[][MAX_PATH_LENGTH], int *file_count) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(base_path))) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[MAX_PATH_LENGTH];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);
            search_files(path, filename, files, file_count);
        } else {
            if (strcmp(entry->d_name, filename) == 0) {
                snprintf(files[*file_count], MAX_PATH_LENGTH, "%s/%s", base_path, entry->d_name);
                (*file_count)++;
                if (*file_count >= MAX_FILES) {
                    closedir(dir);
                    return;
                }
            }
        }
    }
    closedir(dir);
}

void edit_file(const char *filepath) {
    printf("Editing file: %s\n", filepath);
    char command[MAX_PATH_LENGTH + 6];
    snprintf(command, sizeof(command), "cat > %s", filepath);
    system(command);
}

void prompt_user_to_choose_file(char files[][MAX_PATH_LENGTH], int file_count) {
    int choice;
    printf("Which of the files would you like to edit:\n");
    for (int i = 0; i < file_count; ++i) {
        printf("%d: %s\n", i + 1, files[i]);
    }
    printf("Type the number to edit that file: ");
    scanf("%d", &choice);

    if (choice > 0 && choice <= file_count) {
        edit_file(files[choice - 1]);
    } else {
        printf("Invalid choice.\n");
    }
}

char **filename_completion(const char *text, int start, int end) {
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, filename_generator);
}

char *filename_generator(const char *text, int state) {
    static int list_index, len;
    static char *filenames[MAX_FILES];
    static int file_count;

    if (!state) {
        DIR *dir;
        struct dirent *entry;
        list_index = 0;
        len = strlen(text);
        file_count = 0;

        if ((dir = opendir(".")) != NULL) {
            while ((entry = readdir(dir)) != NULL && file_count < MAX_FILES) {
                if (entry->d_type != DT_DIR) {
                    filenames[file_count] = strdup(entry->d_name);
                    file_count++;
                }
            }
            closedir(dir);
        }
    }

    while (list_index < file_count) {
        char *name = filenames[list_index++];
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

// These are the build instructions:
// Dependencies: readline
// To install dependencies on Ubuntu: sudo apt-get install libreadline-dev
// To compile: gcc -o qe quic-edit.c -lreadline
