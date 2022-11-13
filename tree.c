#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aux_functions.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."
#define FILE_NAME 50
#define NUMBER_FILES 20

FileTree createFileTree(char *rootFolderName)
{
    FileTree fileTree;
    TreeNode *root;
    root = (TreeNode *)malloc(sizeof(TreeNode));
    init_node(NULL, root, rootFolderName);
    root->type = FOLDER_NODE;
    fileTree.root = root;
    root->content = NULL;
    return fileTree;
}

void freeTree(FileTree fileTree)
{
    FolderContent *content_to_del;
    content_to_del = fileTree.root->content;
    if (fileTree.root->content == NULL)
    {
        free(fileTree.root->name);
        free(fileTree.root);
        return;
    }
    free_emptyFolder_File(fileTree.root);
    free(fileTree.root->name);
    free(fileTree.root);
}

void ls(TreeNode *currentNode, char *arg)
{
    // TODO 
    FolderContent *content_to_show;
    content_to_show = currentNode->content;
    if (currentNode->content == NULL)
    {
        return;
    }
    ListNode *temp;
    if (strcmp(arg, NO_ARG) == 0)
    {
        show_content(content_to_show);
    }
    else
    {
        temp = search_element(content_to_show->children, arg, 2);
        if (temp == NULL)
        {
            printf("ls: cannot access '%s': No such file or directory", arg);
            return;
        }
        if (temp->info->type == FILE_NODE)
        {
            FileContent *contains_of_file;
            contains_of_file = temp->info->content;
            if (strcmp(contains_of_file->text, NO_ARG) != 0)
            {
                printf("%s: %s\n", temp->info->name, contains_of_file->text);
            }
            else
            {
                printf("%s\n", temp->info->name);
            }
            return;
        }
        if (temp -> info -> type == FOLDER_NODE)
        {
            content_to_show = temp -> info -> content;
            show_content(content_to_show);
            return;
        }
    }
}

void pwd(TreeNode *treeNode)
{
    // TODO
    FileList *start;
    start = malloc(sizeof(FileList));
    start -> text = calloc(strlen(treeNode -> name) + 1, 1);
    memcpy(start->text, treeNode->name, strlen(treeNode->name));
    start->next = NULL;

    TreeNode *temp = treeNode -> parent;
    while (temp != NULL)
    {
        FileList *new;
        new = malloc(sizeof(FileList));
        new->text = calloc(strlen(temp->name)+1,1);
        memcpy(new->text, temp->name, strlen(temp->name));
        new->next = start;
        start = new;
        temp = temp->parent;
    }
    FileList *each_node;
    each_node = start;
    while (each_node -> next != NULL)
    {
        printf("%s/", each_node->text);
        each_node = each_node->next;
    }
    printf("%s\n", each_node->text);
    while ( start != NULL)
    {
        each_node = start;
        start = start -> next;
        free(each_node->text);
        free(each_node);
    }
}

TreeNode *cd(TreeNode *currentNode, char *path)
{
    // TODO
    TreeNode *init_Node;
    init_Node = currentNode;
    if (strcmp(path, NO_ARG) == 0)
    {
        return currentNode;
    }

    TreeNode *cd_result = check_path(currentNode, path);
    if (init_Node == cd_result)
    {
        printf("cd: no such file or directory: %s\n", path);
        currentNode = init_Node;
        return currentNode;
    }
    else
    {
        currentNode = cd_result;
        return currentNode;
    }
}

void tree(TreeNode *currentNode, char *arg)
{
    // TODO
    int nr_files = 0, nr_folders = 0;
    FolderContent *contentShow;
    contentShow = currentNode->content;
    ListNode *temp;
    TreeNode *nodeOfTree = currentNode;
    int nr_tabs = 0;
    if (strcmp(arg, NO_ARG) == 0)
    {
        if (contentShow != NULL)
        {
            iterateFolder(currentNode, &nr_files, &nr_folders, nr_tabs);
            printf("%d directories, %d files", nr_folders, nr_files);
            return;
        }      
    }
    if (contentShow !=NULL || contentShow->children != NULL)
    {
        if (strstr(arg, "/") == NULL)
        {
            temp = search_element(contentShow->children, arg, FOLDER_NODE);
            if (temp == NULL)
            {
                printf("%s [error opening dir]\n\n0 directories, 0 files\n",arg);
                return;
            }
            iterateFolder(temp->info, &nr_files, &nr_folders, nr_tabs);
        }
        else
        {
            nodeOfTree = check_path(currentNode, arg);
            if (nodeOfTree == currentNode)
            {
                printf("%s [error opening dir]\n\n0 directories, 0 files\n", arg);
                return;
            }
            iterateFolder(nodeOfTree, &nr_files, &nr_folders, nr_tabs);
        }
    }
    currentNode = nodeOfTree;
    printf("%d directories, %d files", nr_folders, nr_files);
}

void mkdir(TreeNode *currentNode, char *folderName)
{
    TreeNode *new_folder;
    new_folder = (TreeNode *)malloc(sizeof(TreeNode));
    init_node(currentNode, new_folder, folderName);
    new_folder->type = FOLDER_NODE;

    FolderContent *folder_contains;
    if (currentNode->content != NULL)
    {
        folder_contains = currentNode->content;
        ListNode *temp;
        temp = folder_contains->children->head;

        if (search_element(folder_contains->children, folderName, 2) != NULL)
        {
            free_node(new_folder);
            printf("mkdir: cannot create directory '%s': File exists\n", folderName);
            return;
        }

        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        ListNode *new;
        new = (ListNode *)malloc(sizeof(ListNode));
        new->info = new_folder;
        new->next = NULL;
        temp->next = new;
        return;
    }
    else
    {
        folder_contains = (FolderContent *)malloc(sizeof(FolderContent));
        folder_contains->children = (List *)malloc(sizeof(List));
        folder_contains->children->head = NULL;

        ListNode *new;
        new = (ListNode *)malloc(sizeof(ListNode));
        new->info = new_folder;
        new->next = NULL;

        folder_contains->children->head = new;
        currentNode->content = folder_contains;
    }
}

void rmrec(TreeNode *currentNode, char *resourceName)
{
    // TODO
    if (strcmp(resourceName, NO_ARG) == 0)
    {
        return;
    }
    FolderContent *content;
    content = currentNode->content;
    if (content == NULL || content->children == NULL)
    {
        printf("rmrec: failed to remove '%s': No such file or directory",resourceName);
        return;
    }

    ListNode *temp = search_element(content->children, resourceName, 2);
    if (temp == NULL)
    {
        printf("rmrec: failed to remove '%s': No such file or directory",resourceName);
        return;
    }
    else
    {
        content = currentNode->content;
        if (temp->info->type == FOLDER_NODE)
        {
            if (content != NULL || content->children != NULL )
            {
                free_emptyFolder_File(temp->info);
            }
        }
            delete_simple_node(currentNode, content, temp);
    }
}

void rm(TreeNode *currentNode, char *fileName)
{
    // TODO
    if (strcmp(fileName, NO_ARG) == 0)
    {
        return;
    }
    FolderContent *all_content;
    all_content = currentNode->content;
    ListNode *temp;
    temp = search_element(all_content->children, fileName, 2);
    if (temp == NULL)
    {
        printf("rm: failed to remove '%s': No such file or directory\n", fileName);
        return;
    }
    if (temp->info->type == FOLDER_NODE)
    {
        printf("rm: cannot remove '%s': Is a directory\n", fileName);
        return;
    }

    delete_simple_node(currentNode, all_content, temp);
}

void rmdir(TreeNode *currentNode, char *folderName)
{
    // TODO
    FolderContent *contains;
    contains = currentNode->content;
    ListNode *temp;
    temp = search_element(contains->children, folderName, 2);
    if (temp == NULL)
    {
        printf("rmdir: failed to remove '%s': No such file or directory\n", folderName);
        return;
    }
    if (temp->info->type == FILE_NODE)
    {
        printf("rmdir: failed to remove '%s': Not a directory\n", folderName);
        return;
    }
    if (temp->info->content != NULL)
    {
        printf("rmdir: failed to remove '%s': Directory not empty", folderName);
        return;
    }
    delete_simple_node(currentNode, contains, temp);
}

void touch(TreeNode *currentNode, char *fileName, char *fileContent)
{
    TreeNode *new_file;
    new_file = (TreeNode *)malloc(sizeof(TreeNode));
    init_node(currentNode, new_file, fileName);
    new_file->type = FILE_NODE;

    if (fileContent != NULL)
    {
        FileContent *new_content;
        new_content = (FileContent *)malloc(sizeof(FileContent));
        new_content->text = calloc(strlen(fileContent) + 1, 1);
        memcpy(new_content->text, fileContent, strlen(fileContent));
        new_file->content = new_content;
    }
    // completely created the file_node;
    // connect it to current_folder
    FolderContent *folder_new_content;
    if (currentNode->content == NULL)
    {
        folder_new_content = (FolderContent *)malloc(sizeof(FolderContent));
        folder_new_content->children = (List *)malloc(sizeof(List));

        ListNode *new_node;
        new_node = (ListNode *)malloc(sizeof(ListNode));
        new_node->info = new_file;
        new_node->next = NULL;
        folder_new_content->children->head = new_node;

        currentNode->content = folder_new_content;
    }
    else
    {
        folder_new_content = currentNode->content;
        ListNode *temp;
        temp = folder_new_content->children->head;
        while (temp->next != NULL)
        {
            if (memcmp(fileName, temp->info->name, strlen(fileName)) == 0)
            {
                free_node(new_file);
                return;
            }
            temp = temp->next;
        }

        if (memcmp(fileName, temp->info->name, strlen(fileName)) == 0)
        {
            free_node(new_file);
            return;
        }
        ListNode *new_node;
        new_node = (ListNode *)malloc(sizeof(ListNode));
        new_node->info = new_file;
        new_node->next = NULL;
        temp->next = new_node;
    }
}

void cp(TreeNode *currentNode, char *source, char *destination)
{
    // TODO
    TreeNode *init_node, *checkSource,*checkDes;
    init_node = currentNode;
    checkSource = check_path(currentNode, source);
    if (checkSource->type == FOLDER_NODE)
    {
        printf("cp: -r not specified; omitting directory '%s'", source);
        return;
    }
    else
    {
        char *destPath = copy_text(destination);
        currentNode = init_node;
        checkDes = check_path(currentNode, destination);
        if (checkDes == init_node)
        {
            printf("cp: failed to access '%s': Not a directory", destPath);
            free(destPath);
            return;
        }
        FileContent *contentDes, *contentSource;
        if (checkDes->type == FILE_NODE)
        {
            contentDes = checkDes->content;
            if (contentDes->text != NULL)
            {
                free(contentDes->text);
            }
            contentSource = checkSource->content;
            contentDes->text = copy_text(contentSource->text);
            checkDes->content = contentDes;           
        }
        else
        {
                contentSource = checkSource->content;
                touch(checkDes, checkSource->name, contentSource->text);
        }
        free(destPath);
    }
    
}

void mv(TreeNode *currentNode, char *source, char *destination)
{
    // TODO
    char *dest_rep = copy_text(destination);
    FileContent *contentFile;
    FolderContent *contentFolder;
    TreeNode *init_node, *checkSource,*checkDes;
    ListNode *temp;
    init_node = currentNode;
    checkSource = check_path(currentNode, source);
    currentNode = init_node;
    checkDes = check_path(currentNode, destination);
    if (checkDes == currentNode)
    {
        printf("mv: failed to access '%s': Not a directory", dest_rep);
        free(dest_rep);
        return;
    }
    currentNode = init_node;
    if (checkSource->type == FILE_NODE && checkDes->type == FOLDER_NODE)
    {
        contentFile = checkSource->content;
        touch(checkDes, checkSource->name, contentFile->text);
        contentFolder = checkSource->parent->content;
        temp = search_element(contentFolder->children, checkSource->name, FILE_NODE);
        delete_simple_node(checkSource->parent, contentFolder, temp);
    }
    if (checkSource->type == FOLDER_NODE && checkDes->type == FOLDER_NODE)
    {
        if (checkSource->content == NULL)
        {
            contentFolder = checkDes->content;
            mkdir(checkDes, checkSource->name);
            rmdir(checkSource->parent, checkSource->name);
        }
        else
        {
            change_parent(checkSource->parent, checkDes, checkSource);
        }
    }
    if (checkSource->type == FILE_NODE && checkDes -> type == FILE_NODE)
    {
        FileContent *destinationContent;
        contentFile = checkSource->content;
        destinationContent = checkDes ->content;
        if (destinationContent != NULL)
        {
            free(destinationContent->text);
        }
            destinationContent->text = copy_text(contentFile->text);
            rm(checkSource->parent, checkSource->name);
    }
    free(dest_rep);
}
