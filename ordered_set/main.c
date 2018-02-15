#include <stdio.h>
#include <assert.h>

#include "ordered_set.h"

int main(int argc, char *argv[])
{
	
	struct ordered_set *set = create_set();


	insert(set, 12);
	insert(set, 13);
	insert(set, 1);
	
	assert(contains(set,12));
	assert(contains(set,13));
	assert(contains(set,1));

	print_inorder(set->root);
	
	


	return 0;
}
