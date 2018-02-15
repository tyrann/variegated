#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ordered_set.h"



struct ordered_set* create_set(){
	struct ordered_set* set;
	assert(set = calloc(sizeof(struct ordered_set),1));
	assert(set->root = calloc(sizeof(struct node),1));
	return set;	
}

void insert(struct ordered_set *set, int i){
	assert(set);
	assert(set->root);
	struct node *root = set->root;	

	while(root){
		if(i < root->index){
			if(root->left){
				root = root->left;
			}
			else{
				assert(root->left = calloc(sizeof(struct node),1));
				root->left->index = i;
				return;
			}
		}
		else if(i > root->index){
			if(root->right){
				root = root->right;
			}
			else{
				assert(root->right = calloc(sizeof(struct node),1));
				root->right->index = i;
				return;
			}
		}
	}

}

struct node *contains(struct ordered_set *set, int i){
	assert(set);
	assert(set->root);
	struct node *root = set->root;
	while(root){
		if(root->index == i){
			return root;
		}
		else if(i < root->index){
			root = root->left;
		}		
		else if (i > root->index){
			root = root->right;
		}
	}
	return NULL;

}

void print_preorder(struct node *n){
	if(!n){
		return;
	}
	fprintf(stdout,"%d ",n->index);
	print_inorder(n->left);
	print_inorder(n->right);
}

void print_inorder(struct node *n){
	if(!n){
		return;
	}
	print_inorder(n->left);
	fprintf(stdout,"%d ",n->index);
	print_inorder(n->right);
}


void print_postorder(struct node *n){
	if(!n){
		return;
	}
	print_inorder(n->left);
	print_inorder(n->right);
	fprintf(stdout,"%d ",n->index);
}

