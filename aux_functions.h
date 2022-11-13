#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tree.h"
#define NO_ARG ""
#define PARENT_DIR ".."

void init_node(TreeNode *parent, TreeNode *new_node, char *NodeName)
{
    new_node->parent = parent;
    new_node->name = calloc(strlen(NodeName) + 1, 1);
    (char *)memcpy(new_node->name, NodeName, strlen(NodeName));
    new_node->content = NULL;
}

void free_node(TreeNode *node)
{
    FileContent *content_to_del;
    content_to_del = node->content;
    if (node->type == FILE_NODE)
    {
        if (content_to_del != NULL)
        {
            if (content_to_del->text != NULL)
            {
                free(content_to_del->text);
                free(node->content);
            }
        }
    }
    free(node->name);
    free(node);
}

void show_content(FolderContent *content_to_show)
{
    ListNode *temp, *each_one;
    if (content_to_show == NULL || content_to_show->children == NULL)
    {
        return;
    }
    temp = content_to_show->children->head;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    printf("%s\n", temp->info->name);
    while (temp != content_to_show->children->head)
    {
        each_one = content_to_show->children->head;
        while (each_one->next != temp)
        {
            each_one = each_one->next;
        }
        printf("%s\n", each_one->info->name);
        temp = each_one;
    }
}

ListNode *search_element(List *list, char *arg, enum TreeNodeType type)
{
    if (list->head == NULL)
    {
        return NULL;
    }
    ListNode *temp = list->head;
    while (temp != NULL)
    {
        if (memcmp(temp->info->name, arg, strlen(temp->info->name)) == 0)
        {
            break;
        }
        temp = temp->next;
    }
    if (type == 2)
    {
        return temp;
    }
    if (temp == NULL)
    {
        return NULL;
    }
    if (type == temp->info->type)
    {
        return temp;
    }
}

void delete_simple_node(TreeNode *currentNode, FolderContent *all_content, ListNode *temp)
{
    if (temp == all_content->children->head)
    {
        if (temp->next == NULL)
        {
            free_node(temp->info);
            List *to_del;
            to_del = all_content->children;
            free(to_del);
            free(all_content);
            free(temp);
            currentNode->content = NULL;
            return;
        }
        temp = temp->next;
        ListNode *to_del;
        to_del = all_content->children->head;
        all_content->children->head = temp;
        free_node(to_del->info);
        free(to_del);
        return;
    }
    ListNode *traverse;
    traverse = all_content->children->head;
    while (traverse->next != temp)
    {
        traverse = traverse->next;
    }
    if (temp->next == NULL)
    {
        free_node(temp->info);
        free(temp);
        traverse->next = NULL;
        return;
    }
    else
    {
        traverse->next = temp->next;
        free_node(temp->info);
        free(temp);
    }
}

void free_emptyFolder_File(TreeNode *currentNode)
{
    FolderContent *content_to_del;
    content_to_del = currentNode->content;
    ListNode *temp, *prev;
    temp = content_to_del->children->head;
    free(content_to_del->children);
    free(content_to_del);
    while (temp != NULL)
    {
        prev = temp;
        temp = temp->next;
        if (prev->info->content != NULL && prev->info->type == FOLDER_NODE)
        {
            free_emptyFolder_File(prev->info);
        }
        free_node(prev->info);
        free(prev);
    }
}

void tab_fun(int nr_tabs)
{
    int i = 0;
    for (i = 0; i < nr_tabs; i++)
    {
        printf("    ");
    }
}

void iterateFolder(TreeNode *currentNode, int *nr_files, int *nr_folders, int nr_tabs)
{
    FolderContent *content;
    ListNode *temp, *each_one;
    content = currentNode->content;
    if (content == NULL || content->children == NULL)
    {
        return;
    }
    temp = content->children->head;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    if (temp->info->type == FILE_NODE)
    {
        tab_fun(nr_tabs);
        *nr_files = *nr_files + 1;
        printf("%s\n", temp->info->name);
    }
    else
    {
        *nr_folders = *nr_folders + 1;
        tab_fun(nr_tabs);
        printf("%s\n", temp->info->name);
        iterateFolder(temp->info, nr_files, nr_folders, nr_tabs + 1);
    }

    while (temp != content->children->head)
    {
        each_one = content->children->head;
        while (each_one->next != temp)
        {
            each_one = each_one->next;
        }
        if (each_one->info->type == FILE_NODE)
        {

            tab_fun(nr_tabs);
            *nr_files = *nr_files + 1;
            printf("%s\n", each_one->info->name);
        }
        else
        {
            *nr_folders = *nr_folders + 1;
            tab_fun(nr_tabs);
            printf("%s\n", each_one->info->name);

            iterateFolder(each_one->info, nr_files, nr_folders, nr_tabs + 1);
        }
        temp = each_one;
    }
}

TreeNode *check_path(TreeNode *currentNode, char *path)
{
    char separator[2] = "/";
    char *dir;
    dir = calloc(10, 1);
    char *help_del;
    help_del = dir;
    dir = strtok(path, separator);
    FolderContent *contains;
    contains = currentNode->content;
    TreeNode *init_node;
    init_node = currentNode;
    while (dir != NULL)
    {
        if (strcmp(dir, PARENT_DIR) == 0)
        {
            if (currentNode->parent != NULL)
            {
                currentNode = currentNode->parent;
            }
        }
        else
        {
            if (currentNode->content != NULL)
            {
                contains = currentNode->content;
            }

            if (contains->children != NULL)
            {
                ListNode *temp = search_element(contains->children, dir, 2);
                if (temp == NULL)
                {
                    free(help_del);
                    return init_node;
                }
                else
                {
                    currentNode = temp->info;
                }
            }
            else
            {
                free(help_del);
                return init_node;
            }
        }

        dir = strtok(NULL, separator);
    }
    free(help_del);
    return currentNode;
}

char *copy_text(char *source)
{
    char *destination;
    destination = calloc(strlen(source) + 1, 1);
    memcpy(destination, source, strlen(source));
    return destination;
}

FolderContent *initFolderList(TreeNode *folder)
{
    FolderContent *folder_content;
    folder_content = (FolderContent *)malloc(sizeof(FolderContent));
    folder_content->children = (List *)malloc(sizeof(List));
    return folder_content;
}

void freeFolderList(TreeNode *folder)
{
    FolderContent *content;
    content = folder->content;
    if (content->children->head == NULL)
    {
        free(content->children);
        free(content);
    }
}

void addInDestination(ListNode *nodeToAdd, TreeNode *destNode)
{
    nodeToAdd->info->parent = destNode;
    FolderContent *destinationList;
    destinationList = destNode->content;
    if (destinationList == NULL)
    {
        destinationList = initFolderList(destNode);
        destinationList->children->head = nodeToAdd;
        destNode->content = destinationList;
    }
    else
    {
        ListNode *temp;
        temp = destinationList->children->head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = nodeToAdd;
    }
}

void change_parent(TreeNode *oldParent, TreeNode *newParent, TreeNode *badBoy)
{
    FolderContent *destinationList;
    FolderContent *sourceList;
    sourceList = oldParent->content;
    ListNode *whereBoy, *temp;
    whereBoy = search_element(sourceList->children, badBoy->name, 2);
    destinationList = newParent->content;
    if (whereBoy == sourceList->children->head)
    {
        sourceList->children->head = sourceList->children->head->next;
        whereBoy->next = NULL;
        addInDestination(whereBoy, newParent);
        freeFolderList(oldParent);
        return;
    }
    if (whereBoy->next == NULL)
    {

        temp = sourceList->children->head;
        while (temp->next != whereBoy)
        {
            temp = temp->next;
        }
        temp->next == NULL;
        addInDestination(whereBoy, newParent);
        freeFolderList(oldParent);
        return;
    }
    temp = sourceList->children->head;
    while (temp->next != whereBoy)
    {
        temp = temp->next;
    }
    temp->next = whereBoy->next;
    whereBoy->next = NULL;
    addInDestination(whereBoy, newParent);
    freeFolderList(oldParent);
}