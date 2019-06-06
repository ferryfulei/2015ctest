#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exam.h"

/*
 * Private function prototypes.
 */

static void _print_huffman_tree(const huffman_tree_t *, int);
static void _print_huffman_tree_codes(const huffman_tree_t *, char *, char *);

/*
 * Prints the given Huffman tree.
 */
void print_huffman_tree(const huffman_tree_t *t) {
  printf("Huffman tree:\n");
  _print_huffman_tree(t, 0);
}

/*
 * Private helper function for print_huffman_tree.
 */
static void _print_huffman_tree(const huffman_tree_t *t, int level) {
  int i;
  for (i = 0; i <= level; i++) {
    printf("  ");
  }

  if (t->left == NULL && t->right == NULL) {
    printf("Leaf: '%c' with count %d\n", t->letter, t->count);
  } else {
    printf("Node: accumulated count %d\n", t->count);

    if (t->left != NULL) {
      _print_huffman_tree(t->left, level + 1);
    }

    if (t->right != NULL) {
      _print_huffman_tree(t->right, level + 1);
    }
  }
}

/*
 * Prints the codes contained in the given Huffman tree.
 */
void print_huffman_tree_codes(const huffman_tree_t *t) {
  printf("Huffman tree codes:\n");

  char *code = calloc(MAX_CODE_LENGTH, sizeof(char)), *code_position = code;
  if (code == NULL) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }

  _print_huffman_tree_codes(t, code, code_position);
  free(code);
}

/*
 * Private helper function for print_huffman_tree_codes.
 */
static void _print_huffman_tree_codes(const huffman_tree_t *t,
                                      char *code, char *code_position) {

  if (t->left == NULL && t->right == NULL) {
    printf("'%c' has code \"%s\"\n", t->letter, code);
  } else if (t->left != NULL) {
    *code_position++ = 'L';
    *code_position = '\0';

    _print_huffman_tree_codes(t->left, code, code_position--);
  }

  if (t->right != NULL) {
    *code_position++ = 'R';
    *code_position = '\0';

    _print_huffman_tree_codes(t->right, code, code_position--);
  }
}

/*
 * Prints a list of Huffman trees.
 */
void print_huffman_tree_list(const huffman_tree_list_t *l) {
  printf("Huffman tree list:\n");

  for (; l != NULL; l = l->next) {
    print_huffman_tree(l->tree);
  }
}

/*
 * Frees a Huffman tree.
 */
void huffman_tree_free(huffman_tree_t *t) {
  if (t != NULL) {
    huffman_tree_free(t->left);
    huffman_tree_free(t->right);
    free(t);
  }
}

/*
 * Frees a list of Huffman trees.
 */
void huffman_tree_list_free(huffman_tree_list_t *l) {
  if (l != NULL) {
    huffman_tree_list_free(l->next);
    huffman_tree_free(l->tree);
    free(l);
  }
}

/*
 * Returns 1 if the string s contains the character c and 0 if it does not.
 */
int contains(char *s, char c) {
  for (int i = 0; i < strlen(s); i++) {
    if (c == s[i]) {
      return 1;
    }
  }
  return 0;
}

/*
 * Returns the number of occurrences of c in s.
 */
int frequency(char *s, char c) {
  int count = 0;
  for (int i = 0; i < strlen(s); i++) {
    if (s[i] == c) {
      count++;
    }
  }
  return count;
}

/*
 * Takes a string s and returns a new heap-allocated string containing only the
 * unique characters of s.
 *
 * Pre: all strings will have fewer than or equal to MAX_STRING_LENGTH - 1
 *      characters.
 */
char *nub(char *s) {
  char *acc = (char *)malloc(sizeof(char) * (MAX_STRING_LENGTH));
  acc[0] = '\0';
  int count = 0;
  for (int i = 0; i < strlen(s); i++) {
    if (!contains(acc, s[i])) {
      acc[count] = s[i];
      acc[count + 1] = '\0';
      count++;
    }
  }
  acc = realloc(acc, (strlen(acc) + 1) * sizeof(char));
  return acc;
}


/*
 * Adds the Huffman tree t to the list l, returning a pointer to the new list.
 *
 * Pre:   The list l is sorted according to the frequency counts of the trees
 *        it contains.
 *
 * Post:  The list l is sorted according to the frequency counts of the trees
 *        it contains.
 */
huffman_tree_list_t *huffman_tree_list_add(huffman_tree_list_t *l,
                                            huffman_tree_t *t) {
  huffman_tree_list_t *previous = NULL;
  huffman_tree_list_t *current = l;
  huffman_tree_list_t *new_tree = (huffman_tree_list_t *)malloc(sizeof(huffman_tree_list_t));
  new_tree->tree = t;
  if (l == NULL) {
    return new_tree;
  }
  while (current != NULL) {
    if (t->count < current->tree->count) {
      if (previous == NULL) {
        new_tree->next = current;
        return new_tree;
      }
      previous->next = new_tree;
      new_tree->next = current;
      return l;
    }
    previous = current;
    current = current->next;
  }
  previous->next = new_tree;
  return l;
}

/*
 * Takes a string s and a lookup table and builds a list of Huffman trees
 * containing leaf nodes for the characters contained in the lookup table. The
 * leaf nodes' frequency counts are derived from the string s.
 *
 * Pre:   t is a duplicate-free version of s.
 *
 * Post:  The resulting list is sorted according to the frequency counts of the
 *        trees it contains.
 */
huffman_tree_list_t *huffman_tree_list_build(char *s, char *t) {
  huffman_tree_list_t *list = NULL;
  for (int i = 0; i < strlen(t); i++) {
    char current = t[i];
    int count = frequency(s, current);
    huffman_tree_t *tree = malloc(sizeof(huffman_tree_t));
    tree->count = count;
    tree->letter = current;
    list = huffman_tree_list_add(list, tree);
  }
  return list;
}

/*
 * Reduces a sorted list of Huffman trees to a single element.
 *
 * Pre:   The list l is non-empty and sorted according to the frequency counts
 *        of the trees it contains.
 *
 * Post:  The resuling list contains a single, correctly-formed Huffman tree.
 */
huffman_tree_list_t *huffman_tree_list_reduce(huffman_tree_list_t *l) {
  huffman_tree_list_t *last_one = l;
  huffman_tree_list_t *next_one = l->next;
  while (last_one != NULL && next_one != NULL) {
    int count_last = last_one->tree->count;
    int count_next = next_one->tree->count;
    huffman_tree_t *new_node = malloc(sizeof(huffman_tree_t));
    new_node->letter = ' ';
    new_node->left = last_one->tree;
    new_node->right = next_one->tree;
    new_node->count = count_last + count_next;
    l = huffman_tree_list_add(next_one->next, new_node);
    last_one = l;
    next_one = last_one->next;
  }
  return l;
}

/*
 * Accepts a Huffman tree t and a string s and returns a new heap-allocated
 * string containing the encoding of s as per the tree t.
 *
 * Pre: s only contains characters present in the tree t.
 */

char *encode_traverse(char *acc, char target, huffman_tree_t *t) {
    if (t->letter == target) {
        return acc;
    }
    char *res = NULL;
    int length = strlen(acc);
    if (t->left != NULL) {
        acc[length] = 'L';
        acc[length + 1] = '\0';
        res = encode_traverse(acc, target, t->left);
    }
    if (!res && t->right != NULL) {
        acc[length] = 'R';
        acc[length + 1] = '\0';
        res = encode_traverse(acc, target, t->right);
    }
    return res;
}

char *huffman_tree_encode(huffman_tree_t *t, char *s) {
    char *result = (char *)malloc(sizeof(char) * MAX_STRING_LENGTH);
    for (int i = 0; i < strlen(s); i++) {
        char *acc = (char *)malloc(sizeof(char) * MAX_STRING_LENGTH);
        encode_traverse(acc, s[i], t);
        strcat(result, acc);
        free(acc);
    }
    return result;
}

/*
 * Accepts a Huffman tree t and an encoded string and returns a new
 * heap-allocated string contained the decoding of the code as per the tree t.
 *
 * Pre: the code given is decodable using the supplied tree t.
 */
typedef struct return_value {
    int count;
    char letter;
} return_value_t;


return_value_t *decode_traverse(char *code, huffman_tree_t *t, int count) {
    if (t->letter != ' ') {
        return_value_t *return_value = (return_value_t *)malloc(sizeof(return_value_t));
        return_value->count = count;
        return_value->letter = t->letter;
        return return_value;
    }
    if (code[count] == 'L') {
        return decode_traverse(code, t->left, count + 1);
    } else {
        return decode_traverse(code, t->right, count + 1);
    }
}

char *huffman_tree_decode(huffman_tree_t *t, char *code) {
    char *result = (char *) malloc(sizeof(char) * MAX_STRING_LENGTH);
    int count = 0;
    while (strlen(code) > 0) {
        return_value_t *return_value = decode_traverse(code, t, 0);
        result[count++] = return_value->letter;
        code += return_value->count;
        free(return_value);
    }
    result[count] = '\0';
    return result;
}

