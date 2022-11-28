//this is the encoding.c

#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>

typedef struct tree_node_st {
    char *label;    // "cbe", "c", "_d"
    char *path;
    int counter;
    struct tree_node_st *children[2];
} tree_node_t;

// Returns true if leaf-node
int is_leaf(tree_node_t *node) {
    if (node == NULL) {
        printf("WTF!!!!! Node does not exist\n");
        return -1;
    }
    return ((node->children[0] == NULL) && (node->children[1] == NULL));
}

// Ordered linked list
typedef struct ll_node_st {
    tree_node_t *tree_node;
    struct ll_node_st *next;
    struct ll_node_st *prev;
} ll_node_t;

// Counter the letter frequency
void count_letters(int *arr, FILE *stream) {
    int letter;
    while ((letter = fgetc(stream)) != EOF) {
        if (isprint(letter) || (letter == '\n')) {
            arr[letter]++;
        }
    }
}

tree_node_t *create_tree_node(int label_value, int count) {
    tree_node_t *tree_node = (tree_node_t*)calloc(1, sizeof(tree_node_t));
    if (tree_node == NULL) return NULL;
    // okok, check if null in real code
    tree_node->counter = count;
    asprintf(&(tree_node->label), "%c" ,(char)label_value);
    return tree_node;
}
void insert_queue(ll_node_t **head, tree_node_t *tree_node) {
    if (tree_node == NULL) return;
    ll_node_t *new_node = (ll_node_t*)malloc(sizeof(ll_node_t));
    new_node->next = NULL;
    new_node->prev = NULL;
    new_node->tree_node = tree_node;
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    ll_node_t *it_prev = NULL;
    for (ll_node_t *it = *head; it != NULL; it = it->next) {
        if (tree_node->counter <= it->tree_node->counter) {  // Insert before
            new_node->next = it;
            if (it->prev == NULL) { // Beginning of list
                it->prev = new_node;
                *head = new_node;
                return;
            }
            // Somewhere in the middle
            new_node->prev = it->prev;
            new_node->next = it;
            it->prev->next = new_node;
            it->prev = new_node;
            return;
        }
        it_prev = it;
    }
    // Add to the end
    new_node->prev = it_prev;
    it_prev->next = new_node;
}

tree_node_t *pop(ll_node_t **head) {
    if (*head == NULL) return NULL;
    tree_node_t *ret = (*head)->tree_node;
    ll_node_t *tmp = *head;
    *head = (*head)->next;
    if (*head != NULL) {
        (*head)->prev = NULL;
    }
    free(tmp);
    return ret;
}

void print_queue(ll_node_t *head) {
    printf("The queue has the following elements:\n");
    while (head != NULL) {
        printf("[%s] %d\n", head->tree_node->label, head->tree_node->counter);
        head = head->next;
    }
}

ll_node_t *fill_queue(int *arr, int *dict_size) {
    ll_node_t *head = NULL;
    *dict_size = 0;
    for (int i = 0; i < 128; i++) {
        if (arr[i] > 0) {
            // Create tree_node
            tree_node_t *tree_node = create_tree_node(i, arr[i]);
            insert_queue(&head, tree_node);
            (*dict_size)++;
        }
    }
    return head;
}

void pre_order_traversal(tree_node_t *node, int depth, char *path, tree_node_t **dict, int *dict_index) {
    if (node == NULL) return;
    printf("This is node: %s at level %d with path: %s\n", node->label, depth, path);
    node->path = strdup(path);
    // Check if leaf-node
    if ((node->children[0] == NULL) && (node->children[1] == NULL)) {
        printf("This is a leaf node\n");
        dict[*dict_index] = node;
        (*dict_index)++;
        return;
    }

    path[depth] = '0';
    pre_order_traversal(node->children[0], depth + 1, path,dict, dict_index);
    path[depth] = '1';
    pre_order_traversal(node->children[1], depth + 1, path, dict, dict_index);
    path[depth] = '\0';
}
#define OPEN 17 //'(' //17
#define CLOSE 18 // ')' //18

void tree_string_generator(FILE *fpDict, tree_node_t *node) {
    if (node == NULL) return;
    fprintf(fpDict,"%s", node->label);
    if (node->children[0] != NULL) {
        fprintf(fpDict,"%c", OPEN);
        tree_string_generator(fpDict, node->children[0] );
        fprintf(fpDict,"%c", CLOSE);
    }
    if (node->children[1] != NULL) {
        fprintf(fpDict,"%c", OPEN);
        tree_string_generator(fpDict, node->children[1] );
        fprintf(fpDict,"%c", CLOSE);
    }
}

/*
 *  Command line arguments:
 *  huffman_encode <source> <dict> <result>
 *  source is file to encode, e.g., mobydick.txt
 *  dict is output file with the dictionary (single line)
 *  result is the encoded file
 */
int main(int argc, char *argv[]) {
    // Check arguments
    if (argc != 4) {
        printf("Use like: huffman <input-file> <output dict> <encoded file>\n");
        return 0;
    }
    // Try to open the source file
    FILE *source = fopen(argv[1], "r");
    if (source == NULL) {
        printf("Cannot open \"%s\", exiting now!\n", argv[1]);
        return 0;
    }
    // Count the occurrences of the letters
    int letters[128] = {0};


    count_letters(letters, source);
    fclose(source);

    int dict_size = 0;
    ll_node_t *queue = fill_queue(letters, &dict_size); // Fill queue and count length
    print_queue(queue);

    if (queue == NULL) {
        printf("Nothing to do\n");
        return 0;
    }

    while ((queue != NULL) && (queue->next != NULL)) {   // Check we have at least 2 nodes in the queue
        tree_node_t *new_elem = (tree_node_t *) malloc(sizeof(tree_node_t));
        new_elem->children[0] = pop(&queue);
        new_elem->children[1] = pop(&queue);
        new_elem->counter = new_elem->children[0]->counter + new_elem->children[1]->counter;
        new_elem->label = NULL;
        new_elem->path = NULL;
        asprintf(&new_elem->label, "%s%s", new_elem->children[0]->label, new_elem->children[1]->label);
        insert_queue(&queue, new_elem);
    }

    print_queue(queue);
    // Find the dictionary
    // Pre-order-traversal
    char path[128] = {0};
    int depth = 0;
    tree_node_t **dict = (tree_node_t**)malloc(sizeof(tree_node_t*)*dict_size);
    int dict_index = 0;
    pre_order_traversal(queue->tree_node, depth, path, dict, &dict_index);
    tree_node_t *dict2[128] = {0};
    int max_depth = 0;
    for (int i = 0; i < dict_size; i++) {
        printf("[%d] %s - %s\n", i, dict[i]->label, dict[i]->path);
        dict2[dict[i]->label[0]] = dict[i];
        if (strlen(dict[i]->path) > max_depth) {
            max_depth = strlen(dict[i]->path);
        }
    }

    printf("Dictionary has a maximum depth of %d\n", max_depth);

    FILE *fpDict = fopen(argv[2], "w");
    if (fpDict == NULL) {
        printf("Cannot create file %s for dictionary\n", argv[2]);
        return 0;
    }

    tree_string_generator(fpDict, queue->tree_node);
    fclose(fpDict);

    FILE *fpEncoded = fopen(argv[3], "w");
    if (fpEncoded == NULL) {
        printf("Cannot create file %s for encoded output\n", argv[3]);
        return 0;
    }

    char letter;
    source = fopen(argv[1], "r");
    unsigned int length = 0;
    unsigned int src_length = 0;

    while ((letter = fgetc(source)) != EOF) {
        if (isprint(letter) || (letter == '\n')) {
            // Encoding of each character
            fprintf(fpEncoded, "%s", dict2[letter]->path);
            length += strlen(dict2[letter]->path);
            src_length++;
        }
    }
    fclose(fpEncoded);
    printf("Encodes file has %u bytes, but should be %u kbytes.\n", length, length/8192);
    printf("Compression of: %.2f\%\n", (double)(100*length/8)/(double)src_length);
    return 0;
}
