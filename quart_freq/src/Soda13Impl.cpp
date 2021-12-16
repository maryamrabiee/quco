#include <algorithm>
#include "int_stuff.h"
#include "soda13_calc.h"
#include "hdt_factory.h"
#include "util.h"
#include "counting_linked_list.h"

#ifdef doCounter
long long Soda13Calc::runtimeThingy = 0;
#endif

#ifndef CONTRACT_MAX_EXTRA_SIZE
#define CONTRACT_MAX_EXTRA_SIZE 100
#endif

Soda13Calc::Soda13Calc()
{
	dummyHDTFactory = new HDTFactory(0);
}

Soda13Calc::~Soda13Calc()
{
	delete dummyHDTFactory;
}

INTTYPE_REST Soda13Calc::calculateTripletDistance(RootedTree *t1, RootedTree *t2)
{
	START_STOPWATCH(stopwatchTotal)
	INC_RUNTIME_COUNTER
	this->t1 = t1;

	t1->pairAltWorld(t2);
	if (t1->isError())
	{
		cerr << "Didn't pair up perfectly, sorry dude, but I'm aborting!" << endl;
		return -1;
	}

	// Section 3 of Soda13: Counting unresolved triplets and quartets in a single tree
	calcUnresolved(t1);

	hdt = HDT::constructHDT(t2, t1->maxDegree, dummyHDTFactory);

	resolvedTriplets = unresolvedTriplets = 0;
	count(t1);
	// HDT is deleted in count if extracting and contracting!
#ifndef doExtractAndContract
	delete hdt->factory;
#endif

	STOP_STOPWATCH(stopwatchTotal)

#ifdef doStopwatch
	#ifdef doCompactStopWatch
		cout << t1->n << " ";
		cout << stopwatchTotal.getTotalTime() << " ";
		cout << stopwatchColor.getTotalTime() << " ";
		cout << stopwatchCounting.getTotalTime() <<  " ";
		cout << stopwatchContract.getTotalTime() << " ";
	#else
		cout << "Time spent on..." << endl;
		cout << "                    Coloring: " << stopwatchColor.getTotalTime() << " ms" << endl;
		cout << "        Counting in the HDTs: " << stopwatchCounting.getTotalTime() << " ms" << endl;
		cout << "                    Contract: " << stopwatchContract.getTotalTime() << " ms" << endl;
		cout << endl;
	#endif
#endif
	
	return (Util::binom3(t1->n)) - resolvedTriplets - unresolvedTriplets;
}

void Soda13Calc::count(RootedTree *v)
{
	INC_RUNTIME_COUNTER
	if (v->isLeaf() || v->n <= 2)
	{
		// This will make sure the entire subtree has color 0!
		START_STOPWATCH(stopwatchColor)
		v->colorSubtree(0);
		STOP_STOPWATCH(stopwatchColor)

		START_STOPWATCH(stopwatchContract)
#ifdef doExtractAndContract
		delete hdt->factory;
#endif
		STOP_STOPWATCH(stopwatchContract)

		return;
	}

	// v is not a leaf!
	// Find largest subtree
	TemplatedLinkedList<RootedTree*> *largest = v->children;
	int largestN = largest->data->n;
	TemplatedLinkedList<RootedTree*> *beforeLargest = NULL;
	TemplatedLinkedList<RootedTree*> *prev = v->children;
	for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next)
	{
		INC_RUNTIME_COUNTER
		if (current->data->n > largestN)
		{
			largest = current;
			beforeLargest = prev;
			largestN = largest->data->n;
		}
		prev = current;
	}
	if (beforeLargest != NULL)
	{
		beforeLargest->next = largest->next;
		largest->next = v->children;
		v->children = largest;
	}

	// Color i'th subtree (i > 1) with color i
	int c = 2;
	START_STOPWATCH(stopwatchColor)
	for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next)
	{
		INC_RUNTIME_COUNTER
		current->data->colorSubtree(c);
		c++;
	}
	STOP_STOPWATCH(stopwatchColor)

	// Update counters in the HDT
	START_STOPWATCH(stopwatchCounting)
	hdt->updateCounters();
	resolvedTriplets += hdt->getResolvedTriplets();
	unresolvedTriplets += hdt->getUnresolvedTriplets();
#ifdef quartetsToo
	resolvedQuartetsAgree += hdt->quartResolvedAgree;
	resolvedQuartetsAgreeDiag += hdt->quartResolvedAgreeDiag;
#ifndef calcE
	resolvedQuartetsDisagree += hdt->quartResolvedDisagree;
	resolvedQuartetsDisagreeDiag += hdt->quartResolvedDisagreeDiag;
#endif
#ifdef quartetsNoSwap
	resolvedQuartetsAgreeUpper += hdt->quartResolvedAgreeUpper;
#ifndef calcE
	resolvedQuartetsDisagreeUpper += hdt->quartResolvedDisagreeUpper;
#endif
#endif
#ifdef calcE
	unresolvedQuartets += hdt->quartSumE;
#endif
#endif
	STOP_STOPWATCH(stopwatchCounting)

#ifdef doExtractAndContract
	// Extract
	RootedTree** extractedVersions = new RootedTree*[v->numChildren - 1];
	c = 0;
	for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next)
	{
		INC_RUNTIME_COUNTER
		if (current->data->isLeaf() || current->data->n <= 2)
		{
			extractedVersions[c] = NULL;
		}
		else
		{
			START_STOPWATCH(stopwatchContract)
			current->data->markHDTAlternative();
			RootedTree *extractedT2 = hdt->extractAndGoBack(t1->factory);
			extractedVersions[c] = extractedT2->contract();
			delete extractedT2->factory;
			STOP_STOPWATCH(stopwatchContract)
		}
		c++; // Weee :)
	}
#endif

	// Color to 0
	START_STOPWATCH(stopwatchColor)
	for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next)
	{
		INC_RUNTIME_COUNTER
		current->data->colorSubtree(0);
	}
	STOP_STOPWATCH(stopwatchColor)

	// Contract and recurse on 1st child
	RootedTree *firstChild = v->children->data;
	if (firstChild->isLeaf() || firstChild->n <= 2)
	{
		// Do "nothing" (except clean up and possibly color!)
#ifdef doExtractAndContract
		// Notice no recoloring here... It's not neccesary as it is extracted and contracted away,
		// and will actually cause an error if called with firstChild->colorSubtree(0) as t2 is linked
		// to a non-existing hdt (as we just deleted it) (we could wait with deleting it, but as we don't need the coloring why bother)
		START_STOPWATCH(stopwatchContract)
		delete hdt->factory;
		STOP_STOPWATCH(stopwatchContract)
#else
		START_STOPWATCH(stopwatchColor)
		firstChild->colorSubtree(0);
		STOP_STOPWATCH(stopwatchColor)
#endif
	}
	else
	{
#ifdef doExtractAndContract
		bool hdtTooBig = firstChild->n * CONTRACT_MAX_EXTRA_SIZE < hdt->leafCount();
		if (hdtTooBig)
		{
			HDT *newHDT;

			START_STOPWATCH(stopwatchContract)
			firstChild->markHDTAlternative();
			RootedTree *extractedT2 = hdt->extractAndGoBack(t1->factory);
			RootedTree *contractedT2 = extractedT2->contract();
			delete extractedT2->factory;
			newHDT = HDT::constructHDT(contractedT2, t1->maxDegree, dummyHDTFactory, true);
			delete contractedT2->factory;
			delete hdt->factory;
			STOP_STOPWATCH(stopwatchContract)
			hdt = newHDT;
		}
#endif
		count(firstChild);
		// HDT is deleted in recursive call!
	}

	// Color 1 and recurse
	c = 0;
	for(TemplatedLinkedList<RootedTree*> *current = v->children->next; current != NULL; current = current->next)
	{
		INC_RUNTIME_COUNTER
		if (!current->data->isLeaf() && current->data->n > 2)
		{
#ifdef doExtractAndContract
			START_STOPWATCH(stopwatchContract)
			hdt = HDT::constructHDT(extractedVersions[c], t1->maxDegree, dummyHDTFactory, true);
			delete extractedVersions[c]->factory;
			STOP_STOPWATCH(stopwatchContract)
#endif

			START_STOPWATCH(stopwatchColor)
			current->data->colorSubtree(1);
			STOP_STOPWATCH(stopwatchColor)

			count(current->data);
		}
		c++; // Weee :)
		// HDT is deleted on recursive calls!
	}

#ifdef doExtractAndContract
	delete[] extractedVersions;
#endif
}

INTTYPE_N4 Soda13Calc::calculateQuartetDistance(UnrootedTree *t1, UnrootedTree *t2)
{
	START_STOPWATCH(stopwatchTotal)
	INC_RUNTIME_COUNTER
#ifndef quartetsToo
	cout << "This is not compiled with the quartet distance calculation. Exiting!" << endl;
	return -1;
#else
	this->t1 = t1->convertToRootedTree(NULL);
	this->t2 = t2->convertToRootedTree(this->t1->factory);

	this->t1->pairAltWorld(this->t2);
	if (this->t1->isError())
	{
		cerr << "Didn't pair up perfectly, sorry dude, but I'm aborting!" << endl;
		delete this->t1->factory;
		delete this->t2->factory;
		return -1;
	}

	// Section 3 of Soda13: Counting unresolved triplets and quartets in a single tree
	calcUnresolved(this->t1);
#ifndef calcE
	INTTYPE_N4 abc = Util::binom4(this->t1->n) - this->t1->getUnresolvedQuartets();
#endif
	hdt = HDT::constructHDT(this->t2, this->t1->maxDegree, dummyHDTFactory);

	resolvedQuartetsAgree = resolvedQuartetsAgreeDiag = 0;
	resolvedQuartetsDisagree = resolvedQuartetsDisagreeDiag = 0;
#ifdef quartetsNoSwap
	resolvedQuartetsAgreeUpper = resolvedQuartetsDisagreeUpper = 0;
#endif
#ifdef calcE
	unresolvedQuartets = 0;
#endif

	count(this->t1);
#ifndef doExtractAndContract
	delete hdt->factory;
#endif

	INTTYPE_N4 result = -1;
#if defined(quartetsNoSwap) && !defined(quartetsSwapAnyways)
	INTTYPE_N4 a = resolvedQuartetsAgree + resolvedQuartetsAgreeDiag + resolvedQuartetsAgreeUpper;
	#ifdef calcE
		result = Util::binom4(this->t1->n) - a - unresolvedQuartets;
	#else
		calcUnresolved(this->t2);
		INTTYPE_N4 abd = Util::binom4(this->t2->n) - this->t2->getUnresolvedQuartets();
		INTTYPE_N4 b = resolvedQuartetsDisagree + resolvedQuartetsDisagreeDiag + resolvedQuartetsDisagreeUpper;
		result = abd - a + abc - a - b;
	#endif
#else
	RootedTreeFactory *oldFactory1 = this->t1->factory;
	RootedTreeFactory *oldFactory2 = this->t2->factory;

	this->t2 = t1->convertToRootedTree(oldFactory1);
	this->t1 = t2->convertToRootedTree(oldFactory2);

	delete oldFactory1;
	delete oldFactory2;

	this->t1->pairAltWorld(this->t2);
	if (this->t1->isError())
	{
		cerr << "Didn't pair up perfectly, sorry dude, but I'm aborting!" << endl;
		return -1;
	}

	// Section 3 of Soda13: Counting unresolved triplets and quartets in a single tree
	calcUnresolved(this->t1);
#ifndef calcE
	INTTYPE_N4 abd = Util::binom4(this->t1->n) - this->t1->getUnresolvedQuartets();
#endif
	hdt = HDT::constructHDT(this->t2, this->t1->maxDegree, dummyHDTFactory);

	INTTYPE_N4 firstResolvedQuartetsAgree = resolvedQuartetsAgree;
	INTTYPE_N4 firstResolvedQuartetsAgreeDiag = resolvedQuartetsAgreeDiag;
#ifndef calcE
	INTTYPE_N4 firstResolvedQuartetsDisagree = resolvedQuartetsDisagree;
	INTTYPE_N4 firstResolvedQuartetsDisagreeDiag = resolvedQuartetsDisagreeDiag;
#endif
#ifdef quartetsNoSwap
	INTTYPE_N4 firstResolvedQuartetsAgreeUpper = resolvedQuartetsAgreeUpper;
#ifndef calcE
	INTTYPE_N4 firstResolvedQuartetsDisagreeUpper = resolvedQuartetsDisagreeUpper;
#endif
#endif
	resolvedQuartetsAgree = resolvedQuartetsAgreeDiag = 0;
	resolvedQuartetsDisagree = resolvedQuartetsDisagreeDiag = 0;
#ifdef quartetsNoSwap
	resolvedQuartetsAgreeUpper = resolvedQuartetsDisagreeUpper = 0;
#endif
#ifdef calcE
	INTTYPE_N4 firstETry = unresolvedQuartets;
	unresolvedQuartets = 0;
#endif
	count(this->t1);
#ifndef doExtractAndContract
	delete hdt->factory;
#endif

	INTTYPE_N4 a = firstResolvedQuartetsAgree + resolvedQuartetsAgree + resolvedQuartetsAgreeDiag;

	#ifdef calcE
		#ifdef quartetsNoSwap //i.e. swapcheck!!
		if (resolvedQuartetsAgreeUpper != firstResolvedQuartetsAgree || resolvedQuartetsAgree != firstResolvedQuartetsAgreeUpper)
		{
			cout << resolvedQuartetsAgreeUpper << " vs " << firstResolvedQuartetsAgree << endl;
			cout << resolvedQuartetsAgree << " vs " << firstResolvedQuartetsAgreeUpper << endl;
			cout << "FAIL!" << endl;
		}
		else
		#endif
		{
			if (firstETry != unresolvedQuartets)
			{
				cout << "FAIL on E!" << endl;
			}
			else if (resolvedQuartetsAgreeDiag != firstResolvedQuartetsAgreeDiag)
			{
				cout << "FAIL on A diag!" << endl;
			}
			else result = Util::binom4(this->t1->n) - a - unresolvedQuartets;
		}
	#else
	INTTYPE_N4 b = firstResolvedQuartetsDisagree + resolvedQuartetsDisagree + resolvedQuartetsDisagreeDiag;

#ifdef quartetsNoSwap
	if (resolvedQuartetsAgreeUpper != firstResolvedQuartetsAgree || resolvedQuartetsAgree != firstResolvedQuartetsAgreeUpper)
	{
		cout << resolvedQuartetsAgreeUpper << " vs " << firstResolvedQuartetsAgree << endl;
		cout << resolvedQuartetsAgree << " vs " << firstResolvedQuartetsAgreeUpper << endl;
		cout << "FAIL!" << endl;
	}
	else if (resolvedQuartetsDisagreeUpper != firstResolvedQuartetsDisagree || resolvedQuartetsDisagree != firstResolvedQuartetsDisagreeUpper)
	{
		cout << resolvedQuartetsDisagreeUpper << " vs " << firstResolvedQuartetsDisagree << endl;
		cout << resolvedQuartetsDisagree << " vs " << firstResolvedQuartetsDisagreeUpper << endl;
		cout << "FAIL!" << endl;
	}
	else
#endif
	{
		if (resolvedQuartetsAgreeDiag == firstResolvedQuartetsAgreeDiag && resolvedQuartetsDisagreeDiag == firstResolvedQuartetsDisagreeDiag)
			result = abd - a + abc - a - b;
		else
			cout << "FAIL!" << endl;
	}
	#endif

	STOP_STOPWATCH(stopwatchTotal)
#endif

#ifdef doStopwatch
	INTTYPE_REST numLeafs = this->t1->n;
#endif
	delete this->t1->factory;
	delete this->t2->factory;

#ifdef doStopwatch
	#ifdef doCompactStopWatch
		cout << numLeafs << " ";
		cout << stopwatchTotal.getTotalTime() << " ";
		cout << stopwatchColor.getTotalTime() << " ";
		cout << stopwatchCounting.getTotalTime() <<  " ";
		cout << stopwatchContract.getTotalTime() << " ";
	#else
		cout << "Time spent on..." << endl;
		cout << "                    Coloring: " << stopwatchColor.getTotalTime() << " ms" << endl;
		cout << "        Counting in the HDTs: " << stopwatchCounting.getTotalTime() << " ms" << endl;
		cout << "                    Contract: " << stopwatchContract.getTotalTime() << " ms" << endl;
		cout << endl;
	#endif
#endif

	// HDT is deleted in count!
	return result;
#endif
}

void Soda13Calc::calcUnresolved(RootedTree *t)
{
	INC_RUNTIME_COUNTER
	INTTYPE_REST prevS = 0; // not true!! Set later...
	INTTYPE_REST prevP = 0;
	INTTYPE_REST prevT = 0;
	INTTYPE_N4 prevQ = 0;
	int nSum = 0;
	t->n = 0;

	if (t->isLeaf())
	{
		t->p_d = 0;
		t->t_d = 0;
		t->q_d = 0;
		t->n = 1;
		return;
	}

	INTTYPE_REST unresolvedTripletesSum = 0;
	INTTYPE_N4 unresolvedQuartetsHelperSum = 0;
	for(TemplatedLinkedList<RootedTree*> *i = t->children; i != NULL; i = i->next)
	{
		INC_RUNTIME_COUNTER
		RootedTree *childI = i->data;
		calcUnresolved(childI);

		// Is it the first subtree? Then do something special!
		if (i == t->children)
		{
			prevS = childI->n;
			nSum += childI->n;
		}
		else
		{
			nSum += childI->n;

			/*
				This is not very clear in the article... The article says:
				"Letting n_i^v denote the size the subtree rooted at the ith child of v"
				but actually means the number of leafs in the subtree rooted at the ith child of v.
			*/
			INTTYPE_REST s = prevS + childI->n;
			INTTYPE_REST p = prevP + childI->n * prevS;
			INTTYPE_REST t = prevT + childI->n * prevP;
			INTTYPE_N4 q = prevQ + (INTTYPE_N4) childI->n * prevT;
			prevS = s;
			prevP = p;
			prevT = t;
			prevQ = q;
		}

		unresolvedTripletesSum += childI->unresolvedTriplets;
		unresolvedQuartetsHelperSum += childI->unresolvedQuartetsHelper;
	}
	t->unresolvedTriplets = unresolvedTripletesSum + prevT;
	t->n = nSum;
	t->p_d = prevP;
	t->t_d = prevT;
	t->q_d = prevQ;
	t->unresolvedQuartetsHelper = unresolvedQuartetsHelperSum + t->q_d - (INTTYPE_N4) t->t_d * t->n;
}
