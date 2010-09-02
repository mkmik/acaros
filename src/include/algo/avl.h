#ifndef _AVL_H_
#define _AVL_H_

/* This is an implementation of AVL trees.
 * implementation based on http://www.purists.org/georg/avltree 
 * Data genericity modeled after linux linked list implementation (list.h)
 *
 * usage:
 * if you want to put a xyz structure in a AVL tree 
 * declare a member avl_node_t inside the struct (like list_head_t)
 * then (possibly in the header file next to the struct decl)
 * use the macro "avl_make_compare(type, member, key)"
 * where "type" is the type of the struct, "member" is
 * the name of the avl_node_t member, and "key" is the
 * name of a numeric member wich will be the ordering key.
 *  You can provide custom ordering functions. Please see
 * read and understand this header file for details.
 */

typedef struct avl_node avl_node_t;
typedef enum AVLRES avl_res_t;
enum AVLRES {
  AVL_ERROR = 0,
  AVL_OK,
  AVL_BALANCE,
};


struct avl_node {
  avl_node_t *left;
  avl_node_t *right;
  int skew;
};

#define null_avl_node ((avl_node_t){0,0,0})

typedef int (*avl_compare_t)(avl_node_t* a, avl_node_t* b);

/** adds an element to the tree.
 * you must specify the pointer to the node, usually
 * with &element->member. You must pass the pointer to the root
 * variable, because the root can be updated (it is of the avl_node_t**)
 **/
#define avl_add(node, root, type) _avl_add(node, root, type ## _avlcompare)
avl_res_t _avl_add(avl_node_t* node, avl_node_t** root, 
		  avl_compare_t compare);

/** using this macro you can get back the object 
 * containing this node if you have a pointer to an  avl_node_t
 **/
#define avl_entry(ptr, type, member) \
        ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
#define _avl_find(key, root, type, member, parent_node) \
({ \
  avl_node_t *node = root; \
  avl_node_t **parent = parent_node; \
  *parent = 0; \
  type* entry; \
  int match; \
  while(node) { \
    entry = avl_entry(node, type, member); \
    match = type ## _avlmatch(key, entry); \
    if(match == 0) \
      break; \
    *parent = node; \
    if(match < 0) \
      node = node->left; \
    if(match > 0) \
      node = node->right; \
  } \
  entry = node ? avl_entry(node, type, member) : 0; \
}) 

/** this function quickly searchs for an element matching the 
 * key "key" in the tree defined by root, type and member. 
 *  It returns a pointer to the object casted to the appropriate type.
 *  Returns 0 if no element is found */
#define avl_find(key, root, type, member) \
({ \
 avl_node_t *dummy_parent; \
 type* entry; \
 entry = _avl_find(key, root, type, member, &dummy_parent); \
})

/** this function deletes an element given a key and the root of the tree.
 * Make sure the item really exist.
 * */
#define avl_delete(key, root, type, member) \
 _avl_delete(&avl_find(key, *root, type, member)->member, root, type ## _avlcompare);

avl_res_t _avl_delete(avl_node_t* node, avl_node_t** root, avl_compare_t compare);

#define avl_make_compare(type, member, key) \
static inline int type ## _avlcompare(avl_node_t *_a, avl_node_t *_b) {   \
  type *a = avl_entry(_a, type, member); \
  type *b = avl_entry(_b, type, member); \
  return a->key < b->key ? -1 : (a->key == b->key ? 0 : 1); \
}
#define avl_make_match(type, key) \
static inline int type ## _avlmatch(typeof(((type*)0)->key) k, type *b) {   \
  return k < b->key ? -1 : (k == b->key ? 0 : 1); \
}

#define avl_make_string_compare(type, member, key) \
static inline int type ## _avlcompare(avl_node_t *_a, avl_node_t *_b) {   \
  type *a = avl_entry(_a, type, member); \
  type *b = avl_entry(_b, type, member); \
  return strcmp(a->key, b->key); \
}
#define avl_make_string_match(type, key) \
static inline int type ## _avlmatch(typeof(((type*)0)->key) k, type *b) {   \
  return strcmp(k, b->key); \
}

typedef void (*avl_traverse_t)(avl_node_t *node);
void _avl_traverse(avl_node_t** root, avl_traverse_t traverse);

/** traverses the tree depth first in increasing order.
 * it executes the next block of code for each element in the tree
 * storing a pointer to the current element in the "var" variable.
 *  The "var" variable is automatically declared for you and its scope
 * is only inside the block.
 *  This macro is peculiar because looks like a loop but
 * is implemented as nested funcion which is  called at each 
 * iteration (recursion of _avl_traverse)
 *  The problem is only in the scope of the name of this nested function.
 * in this case be sure to use a different "var" name for each avl_traverse
 * in the same scope if you use the same "type"/"member" combination.
 **/
#define avl_traverse(var, root, type, member) \
static void _traverse ## type ## var ## member (type* var); \
void _traverseNode ## type ## var ## member (avl_node_t* _node) { \
  _traverse ## type ## var ## member (avl_entry(_node, type, member)); \
} \
_avl_traverse(root, _traverseNode ## type ## var ## member); \
static void _traverse ## type ## var ## member (type* var)


 

#endif
