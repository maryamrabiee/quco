#ifndef ROOTED_TREE_FACTORY_H
#define ROOTED_TREE_FACTORY_H
#include "rooted_tree.h"
#include "memory_allocator.h"

#ifdef _DEBUG
	#include "nvwa/debug_new.h"
#endif

class RootedTreeFactory
{
	private:
		RootedTree *createdRT;
		RootedTree *currentRT;
		TemplatedLinkedList<RootedTree*> *createdTLL;
		TemplatedLinkedList<RootedTree*> *currentTLL;
		int currentLocationRT, currentLocationTLL;
		int size;
		MemoryAllocator<RootedTree> *memRT;
		MemoryAllocator<TemplatedLinkedList<RootedTree*> > *memTLL;

	public:
		RootedTreeFactory(RootedTreeFactory *copyMemAllocFrom = NULL);
		~RootedTreeFactory();
		RootedTree* getRootedTree(string name = "");
		TemplatedLinkedList<RootedTree*>* getTemplatedLinkedList();
		long long getSizeInRam();
};

#endif