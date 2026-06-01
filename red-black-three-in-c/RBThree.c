#include "RBThree.h"

int main(int argc, char const *argv[])
{
    struct RBThree *three = malloc(sizeof(struct RBThree));
    struct Node n10 = {
        .key = 10,
        .parent = NULL,
        .left = NULL,
        .right = NULL,
        .color = BLACK};

    struct Node n5 = {
        .key = 5,
        .parent = &n10,
        .left = NULL,
        .right = NULL,
        .color = BLACK};

    struct Node n15 = {
        .key = 15,
        .parent = &n10,
        .left = NULL,
        .right = NULL,
        .color = BLACK};

    struct Node n3 = {
        .key = 3,
        .parent = &n5,
        .left = NULL,
        .right = NULL,
        .color = RED};

    struct Node n7 = {
        .key = 7,
        .parent = &n5,
        .left = NULL,
        .right = NULL,
        .color = RED};

    struct Node n13 = {
        .key = 13,
        .parent = &n15,
        .left = NULL,
        .right = NULL,
        .color = RED};

    struct Node n17 = {
        .key = 17,
        .parent = &n15,
        .left = NULL,
        .right = NULL,
        .color = RED};


    /* Links */
    n10.left = &n5;
    n10.right = &n15;

    n5.left = &n3;
    n5.right = &n7;

    n15.left = &n13;
    n15.right = &n17;

    three->rooth = &n10;

    struct Node* lookedKey = malloc(sizeof(struct Node));
    lookedKey = search(three, 13);
    printf("Trovato");
    printNode(lookedKey);
    return 0;
}

/*=======================================*/
// CORE METHODS
/*=======================================*/

struct Node* search (struct RBThree* three, int key)
{
    struct Node* currentNode = three->rooth;
    printNode(currentNode);

    while (currentNode != NULL && currentNode->key != key)
    {

        if(key < currentNode->key){currentNode = currentNode->left;}
        else{currentNode = currentNode->right;}

    } 

    return currentNode;
}


/*=======================================*/
// AUSILIARY METHODS
/*=======================================*/

void printNode(struct Node* node)
{
    if (node == NULL)
        return;

    printf("\n=========================================\n");
    printf(" -key:%d\n -Color: %c\n",
            node->key,
            node->color == RED ? 'R' : 'B');

    printf(" -left:");
    printKeyAtCertainDirection(node->left);
    printf(" -right:");
    printKeyAtCertainDirection(node->right);
    printf("=========================================\n");


}

void printKeyAtCertainDirection(struct Node* dir) 
{
    if(dir == NULL) printf("NULL\n");
    else{printf("%d\n",dir->key );}
}



