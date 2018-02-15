#include <stdint.h>

struct node {
	void *data;
	uint64_t index;

	struct node *left;
	struct node *right;
};

struct ordered_set {
	struct node *root;
};

struct ordered_set* create_set();
void insert(struct ordered_set *set, int i);
struct node* contains(struct ordered_set *set, int i);
void print_inorder(struct node *n);
