#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define OPEN '('  //17 //'(' //17
#define CLOSE ')' //18 // ')' //18

typedef struct tree_node_st {
    char *label;
    struct tree_node_st *children[2];
    struct tree_node_st *parent;
    int counter;
} tree_node_t;

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
    tree_node_t *current_node = newnode(NULL);

    while ((letter = fgetc(stream)) != EOF) {
        if (letter == OPEN) {
            //create left child
            if (current_node->children[0] == NULL) {
                current_node->children[0] = newnode(current_node);
                current_node = current_node->children[0];
            } else {
                //create rigth child
                current_node->children[1] = newnode(current_node);
                current_node = current_node->children[1];
            }
        } else if (letter == CLOSE) {
            current_node = current_node->parent;
        } else {
            current_node->label = letter;
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

    //write to file
    FILE *stream2 = fopen(filename3, "w");
    if (stream2 == NULL) {
        printf("Error opening file %s", filename3);
        return NULL;
    }

    while ((letter = (fgetc(stream)) - 48) != EOF) {

        if (is_leaf(current_node)) {
            fprintf(stream2, "%c", current_node->label);
            current_node = root;
        }
        if (letter == 0) {
            current_node = current_node->children[0];
        } else if (letter == 1) {
            current_node = current_node->children[1];
        } else {
            printf("\nDONE\n");
            return 0;
        }
    }
    fclose(stream2);
}

int main() {
    char *filename = "test.dict";
    char *filename2 = "test.huff";
    char *filename3 = "final.huff";

    tree_node_t *root = C_tree(filename);
    tree_string_generator(root);
    printf("\n");
    decode(filename2, root, filename3);

    return 0;

}

