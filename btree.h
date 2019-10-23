/*
 * 
 * You will need to write your B+Tree almost entirely from scratch. 
 * 
 * B+Trees are dynamically balanced tree structures that provides efficient support for insertion, deletion, equality, and range searches. 
 * The internal nodes of the tree direct the search and the leaf nodes hold the base data..
 * 
 * For a basic rundown on B+Trees, we will refer to parts of Chapter 10 of the textbook Ramikrishnan-Gehrke 
 * (all chapters and page numbers in this assignment prompt refer to the 3rd edition of the textbook).
 *
 * Read Chapter 10 which is on Tree Indexing in general. In particular, focus on Chapter 10.3 on B+Tree.
 */

#ifndef BTREE_H
#define BTREE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_types.h"
#include "query.h"
#include "ctest.h"
#define bool char
#define false 0
#define true 1
#endif

//The maximum order can be changed
#define MINIMUM_ORDER 4
#define MAXIMUM_ORDER 40
// The DEFAULT_VALUE represents the maximum and minimum number of entries in a node 
#define DEFAULT_VALUE 4

/* 
Designing your C Structs for B+Tree nodes (Chapter 10.3.1)
How will you represent a B+Tree as a C Struct (or series of C structs that work together)? There are many valid ways to do this part of your design, and we leave it open to you to try and tune this as you progress through the project.
How will you account for a B+Tree node being an internal node or a leaf node? Will you have a single node type that can conditionally be either of the two types, or will you have two distinct struct types?
How many children does each internal node have? This is called the fanout of the B+Tree.
What is the maximum size for a leaf node? How about for an internal node?
What is the minimum threshold of content for a node, before it has to be part of a rebalancing?
*/
// TODO: here you will need to define a B+Tree node(s) struct(s)
/*
*The typedef struct value represents the value that is referenced by a specific key.
*/
typedef struct value{
	int val;
} value;
int order = DEFAULT_VALUE;
/*
* This struct serves both for the leaf and the internal node
* val_keys variable keeps track of the no of valid keys and the number of valid pointers is val_keys
* Number of pointers = val_keys + 1
*/
typedef struct node {
	void ** pointers;
	int * keys;
	struct node * parentNode;
	bool isLeaf;
	int val_keys;
    struct node * next;
    	} node;

/* The following are methods that can be invoked on B+Tree node(s).
 * Hint: You may want to review different design patterns for passing structs into C functions.
 */
/* FIND (Chapter 10.4)
This is an equality search for an entry whose key matches the target key exactly.
How many nodes need to be accessed during an equality search for a key, within the B+Tree? 
*/
// TODO: here you will need to define find/SEARCH related method(s) of finding key-values in your B+Tree.
/* This method finds the value of a given key and checks  for its availability in the b+ tree
 */
void findValKey(node * const root, int key, bool tempRec) {
    node * leaf = NULL;
	value * q = findVal(root, key, tempRec, NULL);
	if (q == NULL)
		printf("Value not found for the given key %d.\n", key);
	else 
		printf("Value at %p is key %d, value %d.\n",
				q, key, q->val);
}

// This method does a top-down binary search using the key provided and returns the leaf and its value that contains th specific key.

node * findValueLeaf(node * const root, int key, bool tempRec) {
	if (root == NULL) {
		if (tempRec) 
			printf("This is an Empty tree");
		return root;
	}
	int k = 0;
	node * a = root;
	while (!a->isLeaf) {
		if (tempRec) {
			printf("[");
			for (k = 0; k < a->val_keys - 1; k++)
				printf("%d ", a->keys[k]);
			printf("%d] ", a->keys[k]);
		}
		k = 0;
		while (k < a->val_keys) {
			if (key >= a->keys[k]) k++;
			else break;
		}
		if (tempRec)
			printf("%d ->", k);
		a = (node *)a->pointers[k];
	}
	if (tempRec) {
		printf("Leaf [");
		for (k = 0; k < a->val_keys - 1; k++)
			printf("%d ", a->keys[k]);
		printf("%d] ->\n", a->keys[k]);
	}
	return a;
}

// This method searches through the b+ tree and prints the keys, values and pointers within a range
 void findValRange(node * const root, int first_key, int last_key,
		bool tempRec) {
	int i;
	int array_size = last_key - first_key + 1;
	int returned_keys[array_size];//array
	void * returned_pointers[array_size];//array
	int numFind = findVal_range(root, first_key, last_key, tempRec,
			returned_keys, returned_pointers);
	if (!numFind)
		printf("No  key value pairs or pointers found \n");
	else {
		for (i = 0; i < numFind; i++)
			printf("Key: %d   Pointer: %p  Value: %d\n",
					returned_keys[i],
					returned_pointers[i],
					((value *)
					 returned_pointers[i])->val);
	}
}
/* This method hlps find the various keys and their pointers withing a given range
 * The entries are then parsed into arrays and at the end the number of key-pinters pairs is returned.
 */
int findVal_range(node * const root, int first_key, int last_key, bool tempRec,
		int returned_keys[], void * returned_pointers[]) {
	int i, numFind;
	numFind = 0;
	node * n = findValueLeaf(root, first_key, tempRec);
	if (n == NULL) 
        return numFind;
	for (i = 0; i < n->val_keys && n->keys[i] < first_key; i++) ;
	if (i == n->val_keys)
        return numFind;
	while (n != NULL) {
		for (; i < n->val_keys && n->keys[i] <= last_key; i++) {
			returned_keys[numFind] = n->keys[i];//array to store the keys
			returned_pointers[numFind] = n->pointers[i];//array to store the pointers
			numFind++;
		}
		n = n->pointers[order - 1];
		i = 0;
	}
	return numFind;
}
/* This method searches through the b+ tree and returns the value to which a key refers.
 */
value * findVal(node * root, int key, bool tempRec, node ** childLeaf) {
    if (root == NULL) {
        if (childLeaf != NULL) {
            *childLeaf = NULL;
        }
        return NULL;
    }
	int j = 0;
    node * leaf = NULL;
	leaf = findValueLeaf(root, key, tempRec);
    /* In the case that the  root is not NULL, then it must cointain some values 
    * hence we have a for loop to help in iterating through the tree through to the internal nodes and the child leafs
    */
	for (j = 0; j < leaf->val_keys; j++)
		if (leaf->keys[j] == key)
         break;
    if (childLeaf != NULL) {
        *childLeaf = leaf;
    }
	if (j == leaf->val_keys)
		return NULL;
	else
		return (value *)leaf->pointers[j];}

//This function splits a node that is too big 

int SplitNode(int length) {
	if (length % 2 == 0)
		return length/2;
	else
		return length/2 + 1;
}

/* INSERT (Chapter 10.5)
How does inserting an entry into the tree differ from findValing an entry in the tree?
When you insert a key-value pair into the tree, what happens if there is no space in the leaf node? What is the overflow handling algorithm?
For Splitting B+Tree Nodes (Chapter 10.8.3)
*/

// TODO: here you will need to define INSERT related method(s) of adding key-values in your B+Tree.

//This creates a new record to hold the value to which a key refers.
 value * makeValue(int value) {
	node * new_record = (value * malloc(sizeof(value)));
	if (new_record == NULL) {
		perror("A new record will be created");
		exit(EXIT_FAILURE);
	}
	else {
		new_record-> val_keys = value;
	}
	return new_record;
}
//This creates a new node, which would be a leaf or internal node to be utilised in the case of a split
node * make_node(void) {
	node * new_node;
	new_node = malloc(sizeof(node));
	if (new_node == NULL) {
		perror("A new node will be created");
		exit(EXIT_FAILURE);
	}
	new_node->keys = malloc((order - 1) * sizeof(int));
	if (new_node->keys == NULL) {
		perror("New node keys array.");
		exit(EXIT_FAILURE);
	}
	new_node->pointers = malloc(order * sizeof(void *));
	if (new_node->pointers == NULL) {
		perror("New node pointers array.");
		exit(EXIT_FAILURE);
	}
	new_node->isLeaf = false;
	new_node->val_keys = 0;
	new_node->parentNode = NULL;
	new_node-> next = NULL;
	return new_node;
}
//This creates a new leaf from a newly created node
node * make_leaf(void) {
	node * leaf = make_node();
	leaf->isLeaf = true;
	return leaf;
}
//Insertion into the rootNode
int leftIndex(node * parentNode, node * left) {

	int left_index = 0;
	while (left_index <= parentNode->val_keys && 
			parentNode->pointers[left_index] != left)
		left_index++;
	return left_index;
}

// Insertion into the leaf
node * insertLeaf(node * leaf, int key, value * pointer) {

	int i, insertion_point;

	insertion_point = 0;
	while (insertion_point < leaf->val_keys && leaf->keys[insertion_point] < key)
		insertion_point++;

	for (i = leaf->val_keys; i > insertion_point; i--) {
		leaf->keys[i] = leaf->keys[i - 1];
		leaf->pointers[i] = leaf->pointers[i - 1];
	}
	leaf->keys[insertion_point] = key;
	leaf->pointers[insertion_point] = pointer;
	leaf->val_keys++;
	return leaf;
}

/* This handles the split function in the case the leaf is full
 */
node * insertionleafAfterSplit(node * root, node * leaf, int key, value * pointer) {

	node * new_leaf;
	int * temp_keys;
	void ** temp_pointers;
	int insertion_index, split, new_key, i, j;
	new_leaf = make_leaf();
	temp_keys = malloc(order * sizeof(int));
	if (temp_keys == NULL) {
		perror("Temporary keys array.");
		exit(EXIT_FAILURE);
	}
	temp_pointers = malloc(order * sizeof(void *));
	if (temp_pointers == NULL) {
		perror("Temporary pointers array.");
		exit(EXIT_FAILURE);
	}
	insertion_index = 0;
	while (insertion_index < order - 1 && leaf->keys[insertion_index] < key)
		insertion_index++;
	for (i = 0, j = 0; i < leaf->val_keys; i++, j++) {
		if (j == insertion_index) j++;
		temp_keys[j] = leaf->keys[i];
		temp_pointers[j] = leaf->pointers[i];
	}
	temp_keys[insertion_index] = key;
	temp_pointers[insertion_index] = pointer;
	leaf->val_keys = 0;

	split = SplitNode(order - 1);

	for (i = 0; i < split; i++) {
		leaf->pointers[i] = temp_pointers[i];
		leaf->keys[i] = temp_keys[i];
		leaf->val_keys++;
	}

	for (i = split, j = 0; i < order; i++, j++) {
		new_leaf->pointers[j] = temp_pointers[i];
		new_leaf->keys[j] = temp_keys[i];
		new_leaf->val_keys++;
	}

	free(temp_pointers);
	free(temp_keys);

	new_leaf->pointers[order - 1] = leaf->pointers[order - 1];
	leaf->pointers[order - 1] = new_leaf;

	for (i = leaf->val_keys; i < order - 1; i++)
		leaf->pointers[i] = NULL;
	for (i = new_leaf->val_keys; i < order - 1; i++)
		new_leaf->pointers[i] = NULL;

	new_leaf->parentNode = leaf->parentNode;
	new_key = new_leaf->keys[0];

	return insertToParentNode(root, leaf, new_key, new_leaf);
}

// Allows insertion into the b+ tree without impeding on any of the b plus tree's properties
node * insertToNode(node * root, node * n, 
		int left_index, int key, node * right) {
	int i;

	for (i = n->val_keys; i > left_index; i--) {
		n->pointers[i + 1] = n->pointers[i];
		n->keys[i] = n->keys[i - 1];
	}
	n->pointers[left_index + 1] = right;
	n->keys[left_index] = key;
	n->val_keys++;
	return root;
}


// Handles the node overload when the fanout is exceeded.
node * insertionNodeAfterSplit(node * root, node * old_node, int left_index, 
		int key, node * right) {

	int a, b, split, primeN;
	node * new_node, * child;
	int * temp_keys;
	node ** temp_pointers;

	temp_pointers = malloc((order + 1) * sizeof(node *));//memory allocation
	if (temp_pointers == NULL) {
		perror("Temporary pointers array for splitting nodes.");
		exit(EXIT_FAILURE);
	}
	temp_keys = malloc(order * sizeof(int));
	if (temp_keys == NULL) {
		perror("Temporary keys array for splitting nodes.");
		exit(EXIT_FAILURE);
	}

	for (a = 0, b = 0; a < old_node->val_keys + 1; a++, b++) {
		if (b == left_index + 1) b++;
		temp_pointers[b] = old_node->pointers[a];
	}

	for (a = 0, b = 0; a < old_node->val_keys; a++, b++) {
		if (b == left_index) b++;
		temp_keys[b] = old_node->keys[a];
	}

	temp_pointers[left_index + 1] = right;
	temp_keys[left_index] = key;

	// A new node is created and half the keys and pointers are copied there  
	split = SplitNode(order);
	new_node = make_node();
	old_node->val_keys = 0;
	for (a = 0; a < split - 1; a++) {
		old_node->pointers[a] = temp_pointers[a];
		old_node->keys[a] = temp_keys[a];
		old_node->val_keys++;
	}
	old_node->pointers[a] = temp_pointers[a];
	primeN = temp_keys[split - 1];
	for (++a, b = 0; a < order; a++, b++) {
		new_node->pointers[b] = temp_pointers[a];
		new_node->keys[b] = temp_keys[a];
		new_node->val_keys++;
	}
	new_node->pointers[b] = temp_pointers[a];
	free(temp_pointers);
	free(temp_keys);
	new_node->parentNode = old_node->parentNode;
	for (a = 0; a <= new_node->val_keys; a++) {
		child = new_node->pointers[a];
		child->parentNode = new_node;
	}
	return insertToParentNode(root, old_node, primeN, new_node);
}

/* Inserts a new node (leaf or internal node) into the B+ tree.
 * Returns the root of the tree after insertion.
 */
node * insertToParentNode(node * root, node * left, int key, node * right) {

	int left_index;
	node * parentNode;

	parentNode = left->parentNode;

	if (parentNode == NULL)
		return insertToRoot(left, key, right);
	left_index = leftIndex(parentNode, left);

	if (parentNode->val_keys < order - 1)
		return insertToNode(root, parentNode, left_index, key, right);
	//spliting the node in order to preserve the tree's properties. 
	 	return insertionNodeAfterSplit(root, parentNode, left_index, key, right);
node * insert(node * root, int key, int value) {

	value * record_pointer = NULL;
	node * leaf = NULL;

	record_pointer = findVal(root, key, false, NULL);
    if (record_pointer != NULL) {
    //Instances where the key already exists/ handling duplicates.
        record_pointer->val = value;
        return root;
    }
	record_pointer = makeValue(value);
	if (leaf->val_keys < order - 1) {
		leaf = insertLeaf(leaf, key, record_pointer);
		return root;
	}
	//splitting the leaf
	return insertionleafAfterSplit(root, leaf, key, record_pointer);
}


/* BULK LOAD (Chapter 10.8.2)
Bulk Load is a special operation to build a B+Tree from scratch, from the bottom up, when beginning with an already known dataset.
Why might you use Bulk Load instead of a series of inserts for populating a B+Tree? Compare the cost of a Bulk Load of N data entries versus that of an insertion of N data entries? What are the tradeoffs?
*/

// TODO: here you will need to define BULK LOAD related method(s) of initially adding all at once some key-values to your B+Tree.
// BULK LOAD only can happen at the start of a workload


/*RANGE (GRADUATE CREDIT)
Scans are range searches for entries whose keys fall between a low key and high key.
Consider how many nodes need to be accessed during a range search for keys, within the B+Tree?
Can you describe two different methods to return the qualifying keys for a range search? 
(Hint: how does the algorithm of a range search compare to an equality search? What are their similarities, what is different?)
Can you describe a generic cost expression for Scan, measured in number of random accesses, with respect to the depth of the tree?
*/

// TODO GRADUATE: here you will need to define RANGE for findValing qualifying keys and values that fall in a key range.



