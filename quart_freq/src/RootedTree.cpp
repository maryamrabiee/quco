#include <cstdlib> // for exit
#include "rooted_tree.h"
#include "hdt.h"
#include "soda13_calc.h"

RootedTree::RootedTree()
{
	INC_RUNTIME_COUNTER
}

void RootedTree::initialize(string name)
{
	INC_RUNTIME_COUNTER
	parent = altWorldSelf = NULL;
	children = NULL;
	level = maxDegree = 0;
	numZeroes = numChildren = 0;

	// Soda13 counting stuff set to default to -1 so we can print if they're not -1...
	p_d = -1;
	t_d = -1;
	q_d = -1;
	n = -1;
	unresolvedTriplets = unresolvedQuartetsHelper = 0;

	// Color 1 is standard...
	color = 1;
	this->name = name;
}

bool RootedTree::isLeaf()
{
	INC_RUNTIME_COUNTER
	return numChildren == 0;
}

unsigned int RootedTree::max(RootedTree *t1, RootedTree *t2, RootedTree *t3)
{
	INC_RUNTIME_COUNTER
	RootedTree *max = t1;
	if (t2->level > max->level) max = t2;
	if (t3->level > max->level) max = t3;
	return max->level;
}

unsigned int RootedTree::max(RootedTree *t1, RootedTree *t2, RootedTree *t3, RootedTree *t4)
{
	INC_RUNTIME_COUNTER
	RootedTree *max = t1;
	if (t2->level > max->level) max = t2;
	if (t3->level > max->level) max = t3;
	if (t4->level > max->level) max = t4;
	return max->level;
}

void RootedTree::addChild(RootedTree *t)
{
	INC_RUNTIME_COUNTER
	numChildren++;
	t->parent = this;
	TemplatedLinkedList<RootedTree*> *newItem = factory->getTemplatedLinkedList();
	newItem->data = t;
	newItem->next = children;
	children = newItem;
}

RootedTree* RootedTree::getParent()
{
	INC_RUNTIME_COUNTER
	return parent;
}

INTTYPE_REST RootedTree::getUnresolvedTriplets()
{
	INC_RUNTIME_COUNTER
	return unresolvedTriplets;
}

INTTYPE_N4 RootedTree::getUnresolvedQuartets()
{
	INC_RUNTIME_COUNTER
	return unresolvedQuartetsHelper + (INTTYPE_N4) n * unresolvedTriplets;
}

void RootedTree::toDot()
{
	cout << "digraph g {" << endl;
	cout << "node[shape=circle];" << endl;
	toDotImpl();
	cout << "}" << endl;
}

vector<RootedTree*>* RootedTree::getList()
{
	INC_RUNTIME_COUNTER
	vector<RootedTree*>* list = new vector<RootedTree*>();
	getListImpl(list);
	return list;
}

void RootedTree::pairAltWorld(RootedTree *t)
{
	INC_RUNTIME_COUNTER
	error = false;
	vector<RootedTree*>* l = t->getList();
	map<string, RootedTree*> altWorldLeaves;

	for(vector<RootedTree*>::iterator i = l->begin(); i != l->end(); i++)
	{
		INC_RUNTIME_COUNTER
		RootedTree *leaf = *i;
		altWorldLeaves[leaf->name] = leaf;
	}

	delete l;
	l = getList();
	map<string, RootedTree*>::iterator altWorldEnd = altWorldLeaves.end();
	for(vector<RootedTree*>::iterator i = l->begin(); i != l->end(); i++)
	{
		INC_RUNTIME_COUNTER
		RootedTree *leaf = *i;
		map<string, RootedTree*>::iterator j = altWorldLeaves.find(leaf->name);
		if (j == altWorldEnd)
		{
			// This leaf wasn't found in the input tree!
			cerr << "Leaves doesn't agree! Aborting! (" << leaf->name << " didn't exist in second tree)" << endl;
			error = true;
			delete l;
			return;
		}
				
		// If we got this far, we found the match! Setup bidirectional pointers!
		leaf->altWorldSelf = j->second;
		j->second->altWorldSelf = leaf;

		// Delete result
		altWorldLeaves.erase(j);
	}

	// Is there results left in altWorldLeaves? If so it had more leaves than we do...
	if (altWorldLeaves.size() > 0)
	{
		cerr << "Leaves doesn't agree! Aborting! (" << altWorldLeaves.begin()->first << " didn't exist in first tree)";
		if (altWorldLeaves.size() > 1)
			cerr << " (and " << (altWorldLeaves.size() - 1) << " other leaves missing from first tree!)";
		cerr << endl;
		error = true;
		delete l;
		return;
	}

	delete l;
}

void RootedTree::colorSubtree(int c)
{
	INC_RUNTIME_COUNTER
	color = c;
	if (altWorldSelf != NULL)
	{
		altWorldSelf->color = c;
		if (altWorldSelf->hdtLink != NULL)
		{
			altWorldSelf->hdtLink->mark();
		}
	}

	for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
	{
		i->data->colorSubtree(c);
	}
}

void RootedTree::markHDTAlternative()
{
	INC_RUNTIME_COUNTER
	if (altWorldSelf != NULL)
	{
		if (altWorldSelf->hdtLink != NULL)
		{
			altWorldSelf->hdtLink->markAlternative();
		}
	}

	for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
	{
		i->data->markHDTAlternative();
	}
}

bool RootedTree::isError()
{
	INC_RUNTIME_COUNTER
	return error;
}

void RootedTree::toDotImpl()
{
	cout << "n" << this << "[label=\"";
	if (isLeaf() && numZeroes > 0) cout << "0's: " << numZeroes;
	else cout << name;
	
	//if (n != -1) cout << "\\nn: " << n << "; p_d: " << p_d << "\\nt_d: " << t_d << "; q_d: " << q_d;
	cout << "\"];" << endl;

	for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
	{
		RootedTree *t = i->data;
		t->toDotImpl();
		cout << "n" << this << " -> n" << t << ";" << endl;
	}
}

void RootedTree::getListImpl(vector<RootedTree*>* list)
{
	INC_RUNTIME_COUNTER
	if (isLeaf())
	{
		list->push_back(this);
	}

	for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
	{
		INC_RUNTIME_COUNTER
		RootedTree *t = i->data;
		t->level = level + 1;
		t->getListImpl(list);
	}
}

void RootedTree::computeNullChildrenData()
{
	INC_RUNTIME_COUNTER
	if (isLeaf()) return;

	bool allZeroes = true;
	numZeroes = 0;
	for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
	{
		INC_RUNTIME_COUNTER
		i->data->computeNullChildrenData();
		if (i->data->numZeroes == 0) allZeroes = false;
		else numZeroes += i->data->numZeroes;
	}
	if (!allZeroes) numZeroes = 0;
}

RootedTree* RootedTree::contract(RootedTreeFactory *factory)
{
	INC_RUNTIME_COUNTER
	computeNullChildrenData();
	return contractImpl(factory);
}

RootedTree* RootedTree::contractImpl(RootedTreeFactory *factory)
{
	INC_RUNTIME_COUNTER
	if (isLeaf()) return this; // reuse leaves!!

	if (factory == NULL) factory = new RootedTreeFactory(this->factory);

	INTTYPE_REST totalNumZeroes = 0;
	RootedTree *firstNonZeroChild = NULL;
	RootedTree *ourNewNode = NULL;
	for(TemplatedLinkedList<RootedTree*> *i = children; i != NULL; i = i->next)
	{
		INC_RUNTIME_COUNTER
		RootedTree *t = i->data;
		if (t->numZeroes > 0) totalNumZeroes += t->numZeroes;
		else
		{
			if (firstNonZeroChild == NULL) firstNonZeroChild = t->contractImpl(factory);
			else
			{
				if (ourNewNode == NULL)
				{
					ourNewNode = factory->getRootedTree();
					ourNewNode->addChild(firstNonZeroChild);
				}
				ourNewNode->addChild(t->contractImpl(factory));
			}
		}
	}

	// Have we found >= 2 non-zero children?
	if (ourNewNode == NULL)
	{
		#ifdef beParanoid
			if (firstNonZeroChild == NULL)
			{
				exit(-2);
			}
		#endif

		// No... We only have 1 non-zero children!
		if (firstNonZeroChild->numChildren == 2)
		{
			RootedTree *zeroChild = firstNonZeroChild->children->data;
			RootedTree *otherOne = firstNonZeroChild->children->next->data;
			if (zeroChild->numZeroes == 0)
			{
				RootedTree *tmp = otherOne;
				otherOne = zeroChild;
				zeroChild = tmp;
			}
			if (zeroChild->numZeroes != 0 && !otherOne->isLeaf())
			{
				// The 1 child has a zero child and only 2 children, the other not being a leaf, i.e. we can merge!
				zeroChild->numZeroes += totalNumZeroes;
				return firstNonZeroChild;
			}
			// if (zeroChild->numZeroes == 0) it's not a zerochild!!
		}

		// The child doesn't have a zero child, i.e. no merge...
		ourNewNode = factory->getRootedTree();
		ourNewNode->addChild(firstNonZeroChild);
	}

	// We didn't merge below --- add zero-leaf if we have any zeros...
	if (totalNumZeroes > 0)
	{
		RootedTree *zeroChild = factory->getRootedTree();
		zeroChild->numZeroes = totalNumZeroes;
		ourNewNode->addChild(zeroChild);
	}

	return ourNewNode;
}