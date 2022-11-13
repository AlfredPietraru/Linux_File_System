#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include <time.h>
#define LINE_MAX_LEN 1000
#define TOKEN_MAX_LEN 300

#define LS "ls"
#define PWD "pwd"
#define TREE "tree"
#define CD "cd"
#define MKDIR "mkdir"
#define RMDIR "rmdir"
#define TOUCH "touch"
#define RM "rm"
#define RMREC "rmrec"
#define MV "mv"
#define CP "cp"

char *my_strdup(char *input)
{
    int len = strlen( input ) + 1;
    char *output;
    output = input;
    return output;
}

void execute_command(char *cmd, char *arg1, char *arg2) {
    printf("$ %s %s %s\n", cmd, arg1, arg2);
}

TreeNode* process_command(TreeNode* currentFolder,
        char cmd[3][TOKEN_MAX_LEN], int token_count) {
    execute_command(cmd[0], cmd[1], cmd[2]);
    if (!strcmp(cmd[0], LS)) {
        ls(currentFolder, cmd[1]);
    } else if (!strcmp(cmd[0], PWD)) {
        pwd(currentFolder);
    } else if (!strcmp(cmd[0], TREE)) {
        tree(currentFolder, cmd[1]);
    } else if (!strcmp(cmd[0], CD)) {
        currentFolder = cd(currentFolder, cmd[1]);
    } else if (!strcmp(cmd[0], MKDIR)) {
        mkdir(currentFolder, my_strdup(cmd[1]));
    } else if (!strcmp(cmd[0], RMDIR)) {
        rmdir(currentFolder, cmd[1]);
    } else if (!strcmp(cmd[0], RM)) {
        rm(currentFolder, cmd[1]);
    } else if (!strcmp(cmd[0], RMREC)) {
        rmrec(currentFolder, cmd[1]);
    } else if (!strcmp(cmd[0], TOUCH)) {
        touch(currentFolder, my_strdup(cmd[1]), my_strdup(cmd[2]));
    } else if (!strcmp(cmd[0], MV)) {
        mv(currentFolder, cmd[1], cmd[2]);
    } else if (!strcmp(cmd[0], CP)) {
        cp(currentFolder, cmd[1], cmd[2]);
    } else {
        printf("UNRECOGNIZED COMMAND!\n");
    }
    printf("\n");
    return currentFolder;
}

int main() {
    double time_spent = 0.0;
    clock_t begin = clock();
    char line[LINE_MAX_LEN];
    char cmd[3][TOKEN_MAX_LEN];
    char *token;

    FileTree fileTree = createFileTree(my_strdup("root"));
    TreeNode* currentFolder = fileTree.root;

   while (fgets(line, sizeof(line), stdin)!=NULL) {
        line[strlen(line)-1] = 0;

        cmd[0][0] = cmd[1][0] = cmd[2][0] = 0;

        int token_idx = 0;
        token = strtok(line, " ");
        while (token) {
            strcpy(cmd[token_idx], token);
            ++token_idx;

            token = strtok(NULL, " ");
        }
        currentFolder = process_command(currentFolder, cmd, token_idx);
    }
    freeTree(fileTree);
    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    return 0;
}
