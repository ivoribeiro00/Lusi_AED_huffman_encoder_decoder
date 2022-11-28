#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


#define OPEN '('  //17 //'(' //17
#define CLOSE ')' //18 // ')' //18

typedef struct tree_node_st {
    char *label;    // "cbe", "c", "_d"
    struct tree_node_st *children[2];
    struct tree_node_st *parent;
    int counter;
} tree_node_t;

//is_leaf
int is_leaf(tree_node_t *node) {
    if (node == NULL) {
        printf("WTF!!!!! Node does not exist\n");
        return -1;
    }
    return ((node->children[0] == NULL) && (node->children[1] == NULL));
}

void tree_string_generator(tree_node_t *node) {
    if (node == NULL) return;

    if (is_leaf(node)) {
        //printf(node->label);
        //printf("%s", node->label);
        printf("%c", node->label);
    }

    if (node->children[0] != NULL) {
        printf("%c", OPEN);
        tree_string_generator(node->children[0]);
        printf("%c", CLOSE);
    }
    if (node->children[1] != NULL) {
        printf("%c", OPEN);
        tree_string_generator(node->children[1]);
        printf("%c", CLOSE);
    }
}


tree_node_t *newnode(tree_node_t *parent) {
    tree_node_t *new_node = (tree_node_t *) malloc(sizeof(tree_node_t));
    if (new_node == NULL) {
        printf("Error: calloc failed\n");
        return NULL;
    }
    new_node->counter = 0;
    //new_node->label = NULL;
    new_node->parent = parent;
    new_node->children[0] = NULL;
    new_node->children[1] = NULL;
    return new_node;
}

tree_node_t *C_tree(const char *filename) {

    FILE *stream = fopen(filename, "r");
    if (stream == NULL) {
        printf("Error opening file %s", filename);
        return NULL;
    }

    int letter;
    //int arr[256] = {0};

    //tree_node_t *root = newnode(NULL);
    tree_node_t *current_node = newnode(NULL);

    while ((letter = fgetc(stream)) != EOF) {
        if (letter == OPEN) {
            //create left child
            if (current_node->children[0] == NULL) {
                current_node->children[0] = newnode(current_node);
                current_node = current_node->children[0];
            } else {
                current_node->children[1] = newnode(current_node);
                current_node = current_node->children[1];
            }
        } else if (letter == CLOSE) {
            current_node = current_node->parent;
        } else {
            //label
            current_node->label = letter;
            //current_node->label = (char) letter;
            current_node->counter++;
        }
    }
    fclose(stream);
    return current_node;
}

tree_node_t *decode(const char *filename2, tree_node_t *root, const char *filename3) {
    FILE *stream = fopen(filename2, "r");
    if (stream == NULL) {
        printf("Error opening file %s", filename2);
        return NULL;
    }

    tree_node_t *current_node = newnode(NULL);
    current_node = root;

    int letter;
    char aux[256];
    //write to file
    FILE *stream2 = fopen(filename3, "w");
    if (stream2 == NULL) {
        printf("Error opening file %s", filename3);
        return NULL;
    }

    while ((letter = (fgetc(stream)) - 48) != EOF) {

        if (is_leaf(current_node)) {
            fprintf(stream2,"%c", current_node->label);
            current_node = root;
            //conctaenate the string
           // aux[strlen(aux)] = current_node->label;
        }
        if (letter == 0) {
            current_node = current_node->children[0];
            //conctaenate the string
            //aux[strlen(aux)] = current_node->label;
        } else if (letter == 1) {
            current_node = current_node->children[1];
            //conctaenate the string
            //aux[strlen(aux)] = current_node->label;
        } else {
            printf("\nDONE\n");
            return 0;
        }
    }
    fclose(stream2);
    //return aux;
}


/*
//function to create the huffman tree
tree_node_t *create_tree_node(int letter, int count) {

    tree_node_t *tree_node = (tree_node_t *) calloc(1, sizeof(tree_node_t));
    tree_node_t *current_node = malloc(sizeof(tree_node));
    if (current_node == NULL) return NULL;

    current_node->counter = count;
    current_node->label = letter;
    current_node->parent = tree_node;
    current_node->children[0] = NULL;
    current_node->children[1] = NULL;

    // parent(child[1],child[2])

    while (current_node->parent != tree_node) {
        //if letter is OPEN then create a new node and make it the left child of the current node
        if (letter == OPEN) {
            current_node->children[0] = create_tree_node(letter, count);
            current_node->children[0] = current_node;
            current_node = current_node->children[0];
        }
            //if ch is CLOSE then move up to the parent of the current node
        else if (letter == CLOSE) {
            current_node = current_node->parent;
        }
            //if letter is a letter then create a new node and make it the right child of the current node
        else {
            current_node->children[1] = create_tree_node(letter, count);
            current_node->children[1] = current_node;
            current_node = current_node->children[1];
        }
        return current_node;
    }
}
*/

//read the test.dict file and store the data in an array
int main() {
    char *filename = "test.dict";
    char *filename2 = "test.huff";
    char *filename3 = "final.huff";
    char aux[256];

    tree_node_t *root = C_tree(filename);
    tree_string_generator(root);
    printf("\n");
    decode(filename2, root, filename3);
    //printf("\n");

    /*
    FILE *fp;
    char ch;
    int i = 0;
    int arr[256] = {0};
    fp = fopen("test.dict", "r");
    if (fp == NULL) {
        printf("Error opening file");
        return 0;
    }
    while ((ch = fgetc(fp)) != EOF) {
        if (isprint(ch) || (ch == '\n')) {
            arr[ch]++;
            //printf("%c", ch); // _DAECB(_D(_)(D))(AECB(A)(ECB(E)(CB(C)(B))))
        }
    }
    fclose(fp);

    for (i = 0; i < 256; i++) {
        if (arr[i] != 0) {

            //call the create_tree_node function
            //create_tree_node(i, arr[i]);
            printf("%c - %d\n", i, arr[i]);
        }
    }
    return 0;
    */

}
/*
{ //FILE *p3file = fopen(filename3, "w");
    //-48 pq é ascii
    while ((letter = (fgetc(p2file)) - 48) != EOF) {

        if (is_leaf(current_node)) {
            fputc(current_node->label, p2file);
        }
        if (letter == 0) {
            current_node = current_node->children[0];
        }
        if (letter == 1) {
            current_node = current_node->children[1];
        }
    }
    printf("all done \n ");

}
}*/

