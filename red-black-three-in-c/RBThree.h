#include <stdio.h>
#include <string.h>
#include <errno.h> 
#include <stdlib.h>


#define EXIT -1

enum COLORS {
    RED,
    BLACK
};

enum BRANCH_DIRECTION{
    LEFT,
    RIGHT
};

struct Node
{
    int key;

    struct Node* parent;
    
    struct {
        struct Node* left;
        struct Node* right;
    };

    enum COLORS color;
    
};

struct RBThree
{
    struct Node* rooth;
};

/*=======================================*/
// CORE METHODS
/*=======================================*/
struct Node *search(struct RBThree *, int );

/*=======================================*/
// AUSILIARY METHODS
/*=======================================*/
void printNode(struct Node* );
void printKeyAtCertainDirection(struct Node* );

