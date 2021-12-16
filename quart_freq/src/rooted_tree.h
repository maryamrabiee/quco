#ifndef ROOTED_TREE_H
#define ROOTED_TREE_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "int_stuff.h"
#include "templated_linked_list.h"
class HDT;// Forward declaration...
class RootedTreeFactory; // Forward declaration...

#ifdef _DEBUG
	#include "nvwa/debug_new.h"
#endif

using namespace std;

class RootedTree
{
	public:
		unsigned int level;
		RootedTree *parent;
		TemplatedLinkedList<RootedTree*> *children;
		string name;
		unsigned int numChildren;
		int maxDegree;
		RootedTree *altWorldSelf;
		HDT *hdtLink;
		int color;
		INTTYPE_REST numZeroes;

		RootedTreeFactory *factory;

		// Soda13 counting stuff
		INTTYPE_REST p_d;
		INTTYPE_REST t_d;
		INTTYPE_REST unresolvedTriplets;
		INTTYPE_N4 q_d;
		INTTYPE_N4 unresolvedQuartetsHelper;
		int n;

		RootedTree();
		void initialize(string name);
		bool isLeaf();
		static unsigned int max(RootedTree *t1, RootedTree *t2, RootedTree *t3);
		static unsigned int max(RootedTree *t1, RootedTree *t2, RootedTree *t3, RootedTree *t4);
		void addChild(RootedTree *t);
		RootedTree* getParent();
		INTTYPE_REST getUnresolvedTriplets();
		INTTYPE_N4 getUnresolvedQuartets();
		void toDot();
		vector<RootedTree*>* getList();
		void pairAltWorld(RootedTree *t);
		void colorSubtree(int c);
		void markHDTAlternative();
		bool isError();
		RootedTree *contract(RootedTreeFactory *factory = NULL);

	private:
		bool error;

		void toDotImpl();
		void getListImpl(vector<RootedTree*>* list);
		void computeNullChildrenData();
		RootedTree* contractImpl(RootedTreeFactory *factory);
};

#endif