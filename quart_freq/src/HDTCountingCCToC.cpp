#include <cstdlib> // for exit
#include "hdt.h"
#include "rooted_tree.h"
#include "hdt_factory.h"
#include "counting_linked_list.h"
#include "soda13_calc.h"

void HDT::handleCCToC()
{
	INC_RUNTIME_COUNTER
	// Recurse (if not I child) (we really _should_ have both children!)
	if (!left->up2date) left->updateCounters();
	if (!right->up2date) right->updateCounters();

	// NOTE: We generate this with c2 (the higher one) being the left child and c1 (the lower one) being the right child
	HDT *c1 = this->right;
	HDT *c2 = this->left;

	// n_i is just the sum of the 2 children, thus n_circ is also just the sum of the 2 children
	n_circ = c1->n_circ + c2->n_circ;

	// Initialize sum to 0 so we can increment it in the loop
	n_circ_square = 0;

	// Quartets
#ifdef quartetsToo
	// Initialize sums to 0 so we can increment it in the loop
	n_circ_circ = 0;
	n_square_paren_circ_circ = 0;
	n_paren_circ_circ = 0;
	n_paren_circ_square = 0;
	n_circ_arrow_paren_square_square = 0;
	n_bracket_circ_square = 0;
	n_circ_arrow_square_square = 0; // Added by us

	// Not dependent on i
	n_0_circ = c1->n_0_circ + c2->n_0_circ;
	n_paren_0_circ = c1->n_paren_0_circ + c2->n_paren_0_circ;

#ifndef calcE
	// Figure 15 (not dependent on i)
	n_paren_circ_paren_circ_square = c1->n_paren_circ_paren_circ_square + c2->n_paren_circ_paren_circ_square;
	n_circ_paren_circ_square = c1->n_circ_paren_circ_square + c2->n_circ_paren_circ_square; // sums over something that just sums

	// Figure 15 sums (initialize)
	n_bracket_circ_paren_circ_square = 0;
	n_circ_arrow_circ_square = 0;
	n_paren_circ_square_arrow_circ = 0;
	/*
	n_paren_circ_arrow_circ_arrow_square = 0;
	*/

	// Added by us (Initialize)
	n_circ_arrow_paren_circ_square = 0;
	n_circ_arrow_circ_arrow_square = 0;
	n_circ_arrow_square_arrow_circ = 0;
#endif

#ifdef quartetsNoSwap
	// Added by us for filling out tables (initialize)
	n_bracket_circ_circ = 0;
	n_paren_circ_paren_square_square = 0;
	n_circ_arrow_circ = 0;
	n_circ_arrow_square = 0;
	n_paren_circ_circ_arrow_square = 0;
	n_bracket_circ_paren_square_square = 0;
	n_circ_arrow_square_arrow_square = 0;
#endif

	// Fetch the childrens "result counting data"
	quartResolvedAgree = c1->quartResolvedAgree + c2->quartResolvedAgree;
	quartResolvedAgreeDiag = c1->quartResolvedAgreeDiag + c2->quartResolvedAgreeDiag;
#ifndef calcE
	quartResolvedDisagree = c1->quartResolvedDisagree + c2->quartResolvedDisagree;
	quartResolvedDisagreeDiag = c1->quartResolvedDisagreeDiag + c2->quartResolvedDisagreeDiag;
#endif
#ifdef quartetsNoSwap
	quartResolvedAgreeUpper = c1->quartResolvedAgreeUpper + c2->quartResolvedAgreeUpper;
#ifndef calcE
	quartResolvedDisagreeUpper = c1->quartResolvedDisagreeUpper + c2->quartResolvedDisagreeUpper;
#endif
#endif

#ifdef calcE
	// Initialize sums and stuff for calculating E
	n_circ_square_triangle = c1->n_circ_square_triangle + c2->n_circ_square_triangle;
	n_circ_square_arrow_triangle = 0;
	n_circ_arrow_square_triangle = 0;
	n_paren_circ_square_triangle = c1->n_paren_circ_square_triangle + c2->n_paren_circ_square_triangle;
	n_0_circ_square = c1->n_0_circ_square + c2->n_0_circ_square;
	n_0_circ_arrow_square = 0;
	n_0_arrow_circ_square = 0;
	n_circ_square_arrow_0 = 0;
	n_circ_arrow_0_square = 0;
	n_paren_0_circ_square = c1->n_paren_0_circ_square + c2->n_paren_0_circ_square;
	n_bracket_circ_square_triangle = 0;
	n_bracket_0_circ_square = 0;

	// Fetch sum for calculating E from children
	quartSumE = c1->quartSumE + c2->quartSumE;
#endif
#endif

	// Fetch the childrens "result counting data"
	tripResolved = c1->tripResolved + c2->tripResolved;
	tripUnresolved = c1->tripUnresolved + c2->tripUnresolved;

	// Pointer stuff for walking over all children's data
	CountingLinkedList *c1Next = c1->countingVars;
	CountingLinkedList *c1Count = NULL;
	CountingLinkedList *c2Next = c2->countingVars;
	CountingLinkedList *c2Count = NULL;
	CountingLinkedList *ourCount = countingVars;

	// Quartets
#ifdef quartetsToo
	INTTYPE_REST c1Zero = 0;
	INTTYPE_REST c2Zero = 0;

	// The list is sorted, i.e. if zero's there it's the first one!
	if (c1Next->num == 0) c1Zero = c1Next->n_i;
	if (c2Next->num == 0) c2Zero = c2Next->n_i;

	// Not dependent on i (begin)
	n_0_arrow_circ = c1->n_0_arrow_circ + c2->n_0_arrow_circ + 
		c1Zero * c2->n_circ;

	n_circ_arrow_0 = c1->n_circ_arrow_0 + c2->n_circ_arrow_0 +
		c1->n_circ * c2Zero;

	n_0_arrow_circ_circ = c1->n_0_arrow_circ_circ + c2->n_0_arrow_circ_circ +
		c1Zero * c2->n_circ_circ;
	// Not dependent on i (end)
#endif

	while (c1Next != NULL || c2Next != NULL)
	{
		INC_RUNTIME_COUNTER
		if (c2Next == NULL || (c1Next != NULL && c1Next->num < c2Next->num))
		{
			// Operate only on c1
			c1Count = c1Next;
			c2Count = &CountingLinkedList::dummyLL;
			ourCount->num = c1Count->num;

			if (c1Next->type == CountingLinkedList::End) c1Next = NULL;
			else c1Next = c1Next->next;
		}
		else if (c1Next == NULL || (c2Next != NULL && c2Next->num < c1Next->num))
		{
			// Operate only on c2
			c2Count = c2Next;
			c1Count = &CountingLinkedList::dummyLL;
			ourCount->num = c2Count->num;

			if (c2Next->type == CountingLinkedList::End) c2Next = NULL;
			else c2Next = c2Next->next;
		}
		else //c1Count->num == c2Count->num
		{
			c1Count = c1Next;
			c2Count = c2Next;
			ourCount->num = c1Count->num;

			if (c1Next->type == CountingLinkedList::End) c1Next = NULL;
			else c1Next = c1Next->next;
			if (c2Next->type == CountingLinkedList::End) c2Next = NULL;
			else c2Next = c2Next->next;
		}

		// Update counters (triplets)
		ourCount->n_i = c1Count->n_i + c2Count->n_i;
		if (ourCount->num == 0)
		{
			// Go to next one => We're done!
			if (c1Next == NULL && c2Next == NULL)
				ourCount->type = CountingLinkedList::End;
			else
			{
				// Go to next one (there's more!)
				ourCount->type = CountingLinkedList::Regular;
				if (ourCount->next == NULL) ourCount->next = factory->getLL();
				ourCount = ourCount->next;				
				c1Count = c1Next;
				c2Count = c2Next;
			}

			continue;
		}
		ourCount->n_i_circ = c1Count->n_i_circ + c2Count->n_i_circ;
		ourCount->n_paren_ii = c1Count->n_paren_ii + c2Count->n_paren_ii;
		ourCount->n_i_arrow_circ = c1Count->n_i_arrow_circ + c2Count->n_i_arrow_circ +
			c1Count->n_i * (c2->n_circ - c2Count->n_i);

		// Quartets
#ifdef quartetsToo
		// 2nd group in figure 12 (quartets only)
		ourCount->n_0_i = c1Count->n_0_i + c2Count->n_0_i;
		ourCount->n_ii = c1Count->n_ii + c2Count->n_ii;
		ourCount->n_0_paren_ii = c1Count->n_0_paren_ii + c2Count->n_0_paren_ii;
		ourCount->n_circ_paren_ii = c1Count->n_circ_paren_ii + c2Count->n_circ_paren_ii;
		ourCount->n_i_paren_0_circ = c1Count->n_i_paren_0_circ + c2Count->n_i_paren_0_circ;
		ourCount->n_i_paren_circ_circ = c1Count->n_i_paren_circ_circ + c2Count->n_i_paren_circ_circ;
		ourCount->n_i_paren_circ_square = c1Count->n_i_paren_circ_square + c2Count->n_i_paren_circ_square;

		// In the box
		ourCount->n_bracket_i_circ = ourCount->n_i * (n_circ - ourCount->n_i);

		// 3rd group in figure 12 (quartets only)
		ourCount->n_paren_0_i = c1Count->n_paren_0_i + c2Count->n_paren_0_i;
		ourCount->n_paren_i_circ = c1Count->n_paren_i_circ + c2Count->n_paren_i_circ;
		ourCount->n_paren_0_paren_ii = c1Count->n_paren_0_paren_ii + c2Count->n_paren_0_paren_ii;
		ourCount->n_paren_circ_paren_ii = c1Count->n_paren_circ_paren_ii + c2Count->n_paren_circ_paren_ii;
		ourCount->n_paren_i_paren_0_circ = c1Count->n_paren_i_paren_0_circ + c2Count->n_paren_i_paren_0_circ;

		// 4th group in figure 12 (quartets only)
		ourCount->n_bracket_0_paren_ii = c1Count->n_bracket_0_paren_ii + c2Count->n_bracket_0_paren_ii +
			Util::binom2(c1Count->n_i) * c2Zero +
			c1Zero * c2Count->n_paren_ii +
			c1Count->n_i * c2Count->n_i_arrow_0;

		ourCount->n_bracket_circ_paren_ii = c1Count->n_bracket_circ_paren_ii + c2Count->n_bracket_circ_paren_ii +
			Util::binom2(c1Count->n_i) * (c2->n_circ - c2Count->n_i) +
			(c1->n_circ - c1Count->n_i) * c2Count->n_paren_ii +
			c1Count->n_i * c2Count->n_i_arrow_circ;

		ourCount->n_bracket_i_paren_0_circ = c1Count->n_bracket_i_paren_0_circ + c2Count->n_bracket_i_paren_0_circ +
			c1Zero * (c1->n_circ - c1Count->n_i) * c2Count->n_i +
			c1Count->n_i * (c2->n_paren_0_circ - c2Count->n_paren_0_i) +
			c1Zero * c2Count->n_circ_arrow_i +
			(c1->n_circ - c1Count->n_i) * c2Count->n_0_arrow_i;

		// 5th group in figure 12 (quartets only)
		ourCount->n_0_arrow_i = c1Count->n_0_arrow_i + c2Count->n_0_arrow_i +
			c1Zero * c2Count->n_i;

		ourCount->n_i_arrow_0 = c1Count->n_i_arrow_0 + c2Count->n_i_arrow_0 +
			c1Count->n_i * c2Zero;

		ourCount->n_i_arrow_i = c1Count->n_i_arrow_i + c2Count->n_i_arrow_i +
			c1Count->n_i * c2Count->n_i;

		ourCount->n_circ_arrow_i = c1Count->n_circ_arrow_i + c2Count->n_circ_arrow_i +
			(c1->n_circ - c1Count->n_i) * c2Count->n_i;

		ourCount->n_0_arrow_paren_ii = c1Count->n_0_arrow_paren_ii + c2Count->n_0_arrow_paren_ii +
			c1Zero * c2Count->n_paren_ii;

		// NOTICE: THIS HAS CHANGED FROM THE ARTICLE!
		ourCount->n_i_arrow_paren_0_circ = c1Count->n_i_arrow_paren_0_circ + c2Count->n_i_arrow_paren_0_circ +
			c1Count->n_i * (c2->n_paren_0_circ - c2Count->n_paren_0_i);

		ourCount->n_i_arrow_paren_circ_square = c1Count->n_i_arrow_paren_circ_square + c2Count->n_i_arrow_paren_circ_square +
			c1Count->n_i * (c2->n_paren_circ_square - c2Count->n_paren_i_circ);

		ourCount->n_circ_arrow_paren_ii = c1Count->n_circ_arrow_paren_ii + c2Count->n_circ_arrow_paren_ii +
			(c1->n_circ - c1Count->n_i) * c2Count->n_paren_ii;

		ourCount->n_i_arrow_0_circ = c1Count->n_i_arrow_0_circ + c2Count->n_i_arrow_0_circ +
			c1Count->n_i * (c2->n_0_circ - c2Count->n_0_i);

		ourCount->n_i_arrow_circ_circ = c1Count->n_i_arrow_circ_circ + c2Count->n_i_arrow_circ_circ +
			c1Count->n_i * (c2->n_circ_circ - c2Count->n_ii);

		ourCount->n_i_arrow_circ_square = c1Count->n_i_arrow_circ_square + c2Count->n_i_arrow_circ_square +
			c1Count->n_i * (c2->n_circ_square - c2Count->n_i_circ);

		ourCount->n_circ_arrow_ii = c1Count->n_circ_arrow_ii + c2Count->n_circ_arrow_ii +
			(c1->n_circ - c1Count->n_i) * c2Count->n_ii;

		ourCount->n_paren_ii_arrow_0 = c1Count->n_paren_ii_arrow_0 + c2Count->n_paren_ii_arrow_0 +
			c1Count->n_paren_ii * c2Zero;

		ourCount->n_paren_ii_arrow_circ = c1Count->n_paren_ii_arrow_circ + c2Count->n_paren_ii_arrow_circ +
			c1Count->n_paren_ii * (c2->n_circ - c2Count->n_i);

		ourCount->n_paren_circ_circ_arrow_i = c1Count->n_paren_circ_circ_arrow_i + c2Count->n_paren_circ_circ_arrow_i +
			(c1->n_paren_circ_circ - c1Count->n_paren_ii) * c2Count->n_i;

		ourCount->n_0_arrow_i_arrow_i = c1Count->n_0_arrow_i_arrow_i + c2Count->n_0_arrow_i_arrow_i +
			c1Zero * c2Count->n_i_arrow_i +
			c1Count->n_0_arrow_i * c2Count->n_i;

		ourCount->n_i_arrow_circ_arrow_0 = c1Count->n_i_arrow_circ_arrow_0 + c2Count->n_i_arrow_circ_arrow_0 +
			c1Count->n_i * (c2->n_circ_arrow_0 - c2Count->n_i_arrow_0) +
			c1Count->n_i_arrow_circ * c2Zero;

		ourCount->n_i_arrow_0_arrow_circ = c1Count->n_i_arrow_0_arrow_circ + c2Count->n_i_arrow_0_arrow_circ +
			c1Count->n_i * (c2->n_0_arrow_circ - c2Count->n_0_arrow_i) +
			c1Count->n_i_arrow_0 * (c2->n_circ - c2Count->n_i);

		ourCount->n_circ_arrow_i_arrow_i = c1Count->n_circ_arrow_i_arrow_i + c2Count->n_circ_arrow_i_arrow_i +
			(c1->n_circ - c1Count->n_i) * c2Count->n_i_arrow_i +
			c1Count->n_circ_arrow_i * c2Count->n_i;

		// Added by us!
		ourCount->n_i_arrow_paren_circ_circ = c1Count->n_i_arrow_paren_circ_circ + c2Count->n_i_arrow_paren_circ_circ +
			c1Count->n_i * (c2->n_paren_circ_circ - c2Count->n_paren_ii);

		ourCount->n_0_arrow_ii = c1Count->n_0_arrow_ii + c2Count->n_0_arrow_ii +
			c1Zero * c2Count->n_ii;

		ourCount->n_paren_0_circ_arrow_i = c1Count->n_paren_0_circ_arrow_i + c2Count->n_paren_0_circ_arrow_i +
			(c1->n_paren_0_circ - c1Count->n_paren_0_i) * c2Count->n_i;

		// Figure 15 counters (part 1-4, all with j) & Figure 16 sums (with j)
		ourCount->resetIterator();
		c1Count->resetIterator();
		c2Count->resetIterator();
		c1->countingVars->resetIterator();
		c2->countingVars->resetIterator();

		unsigned int wantedMax = degree+1;
		unsigned int lastJPlus1 = 1;
		if (ourCount->n_i_j != NULL) ourCount->n_i_j->type = CountingLinkedListNumOnly::SkipAndEnd;

#ifndef calcE
		if (ourCount->n_paren_i_j != NULL) ourCount->n_paren_i_j->type = CountingLinkedListNumOnly::SkipAndEnd;
#endif
#ifdef quartetsNoSwap
		if (ourCount->n_j_arrow_i != NULL) ourCount->n_j_arrow_i->type = CountingLinkedListNumOnly::SkipAndEnd;
		if (ourCount->n_i_arrow_j != NULL) ourCount->n_i_arrow_j->type = CountingLinkedListNumOnly::SkipAndEnd;
#ifndef calcE
		if (ourCount->n_i_paren_i_j != NULL) ourCount->n_i_paren_i_j->type = CountingLinkedListNumOnly::SkipAndEnd;
		if (ourCount->n_paren_i_paren_i_j != NULL) ourCount->n_paren_i_paren_i_j->type = CountingLinkedListNumOnly::SkipAndEnd;
		if (ourCount->n_bracket_i_paren_i_j != NULL) ourCount->n_bracket_i_paren_i_j->type = CountingLinkedListNumOnly::SkipAndEnd;
#endif
#endif

#ifndef calcE
		ourCount->n_circ_arrow_paren_i_square = c1Count->n_circ_arrow_paren_i_square + c2Count->n_circ_arrow_paren_i_square;
		ourCount->n_circ_arrow_i_square = c1Count->n_circ_arrow_i_square + c2Count->n_circ_arrow_i_square;
		ourCount->n_circ_arrow_paren_i_circ = c1Count->n_circ_arrow_paren_i_circ + c2Count->n_circ_arrow_paren_i_circ;
		ourCount->n_circ_arrow_i_circ = c1Count->n_circ_arrow_i_circ + c2Count->n_circ_arrow_i_circ;
#endif

#ifdef quartetsNoSwap
		// Added by us for filling out tables
		// A
		ourCount->n_bracket_i_paren_circ_circ = c1Count->n_bracket_i_paren_circ_circ + c2Count->n_bracket_i_paren_circ_circ + 
			(c1->n_bracket_circ_circ - Util::binom2(c1Count->n_i)) * c2Count->n_i + c1Count->n_i * (c2->n_paren_circ_circ - c2Count->n_paren_ii);

		ourCount->n_bracket_i_paren_circ_square = c1Count->n_bracket_i_paren_circ_square + c2Count->n_bracket_i_paren_circ_square +
			(c1->n_bracket_circ_square - c1Count->n_bracket_i_circ) * c2Count->n_i + c1Count->n_i * (c2->n_paren_circ_square - c2Count->n_paren_i_circ);

		ourCount->n_i_arrow_circ_arrow_circ = c1Count->n_i_arrow_circ_arrow_circ + c2Count->n_i_arrow_circ_arrow_circ +
			c1Count->n_i * (c2->n_circ_arrow_circ - c2Count->n_i_arrow_i);

		ourCount->n_i_arrow_circ_arrow_square = c1Count->n_i_arrow_circ_arrow_square + c2Count->n_i_arrow_circ_arrow_square +
			c1Count->n_i * (c2->n_circ_arrow_square - c2Count->n_i_arrow_circ - c2Count->n_circ_arrow_i);

#ifndef calcE
		// B
		ourCount->n_bracket_circ_paren_i_circ = c1Count->n_bracket_circ_paren_i_circ + c2Count->n_bracket_circ_paren_i_circ +
			c1Count->n_i * (c2->n_circ_arrow_circ - c2Count->n_i_arrow_i); // More below

		ourCount->n_bracket_circ_paren_i_square = c1Count->n_bracket_circ_paren_i_square + c2Count->n_bracket_circ_paren_i_square +
			c1Count->n_i * (c2->n_circ_arrow_square - c2Count->n_i_arrow_circ - c2Count->n_circ_arrow_i); // More below

		ourCount->n_circ_arrow_i_arrow_circ = c1Count->n_circ_arrow_i_arrow_circ + c2Count->n_circ_arrow_i_arrow_circ; // More below

		ourCount->n_circ_arrow_circ_arrow_i = c1Count->n_circ_arrow_circ_arrow_i + c2Count->n_circ_arrow_circ_arrow_i +
			(c1->n_circ_arrow_circ - c1Count->n_i_arrow_i) * c2Count->n_i; // More below

		ourCount->n_paren_i_circ_arrow_circ = c1Count->n_paren_i_circ_arrow_circ + c2Count->n_paren_i_circ_arrow_circ; // More below
		ourCount->n_circ_arrow_i_arrow_square = c1Count->n_circ_arrow_i_arrow_square + c2Count->n_circ_arrow_i_arrow_square; // More below
		ourCount->n_paren_i_circ_arrow_square = c1Count->n_paren_i_circ_arrow_square + c2Count->n_paren_i_circ_arrow_square; // More below

		ourCount->n_circ_arrow_square_arrow_i = c1Count->n_circ_arrow_square_arrow_i + c2Count->n_circ_arrow_square_arrow_i +
			(c1->n_circ_arrow_square - c1Count->n_circ_arrow_i - c1Count->n_i_arrow_circ) * c2Count->n_i; // More below
#endif
#endif
#ifdef calcE
		// E
		ourCount->n_i_circ_arrow_square = c1Count->n_i_circ_arrow_square + c2Count->n_i_circ_arrow_square; // More below

		ourCount->n_bracket_i_circ_square = c1Count->n_bracket_i_circ_square + c2Count->n_bracket_i_circ_square +
			c1Count->n_i * (c2->n_circ_square - c2Count->n_i_circ);
#endif

		while (true)
		{
			INC_RUNTIME_COUNTER
			unsigned int j = wantedMax;
			NEXT_LEAST_J(c1Count->n_i_j)					NEXT_LEAST_J(c2Count->n_i_j)
#ifndef calcE
			NEXT_LEAST_J(c1Count->n_paren_i_j)				NEXT_LEAST_J(c2Count->n_paren_i_j)
#endif
#ifdef quartetsNoSwap
			NEXT_LEAST_J(c1Count->n_j_arrow_i)				NEXT_LEAST_J(c2Count->n_j_arrow_i)
			NEXT_LEAST_J(c1Count->n_i_arrow_j)				NEXT_LEAST_J(c2Count->n_i_arrow_j)
#ifndef calcE
			NEXT_LEAST_J(c1Count->n_i_paren_i_j)			NEXT_LEAST_J(c2Count->n_i_paren_i_j)
			NEXT_LEAST_J(c1Count->n_paren_i_paren_i_j)		NEXT_LEAST_J(c2Count->n_paren_i_paren_i_j)
			NEXT_LEAST_J(c1Count->n_bracket_i_paren_i_j)	NEXT_LEAST_J(c2Count->n_bracket_i_paren_i_j)
#endif
#endif

			if (gotoIteratorValueForList(c1->countingVars, lastJPlus1) && c1->countingVars->getIteratorNum() < j) j = c1->countingVars->getIteratorNum();
			if (gotoIteratorValueForList(c2->countingVars, lastJPlus1) && c2->countingVars->getIteratorNum() < j) j = c2->countingVars->getIteratorNum();

			if (j >= wantedMax) break;

			// n_i_j
			INTTYPE_REST new_n_i_j = getIteratorValueForNumList(c1Count->n_i_j, j) + getIteratorValueForNumList(c2Count->n_i_j, j);
			addToNumList(ourCount, i_j, j, new_n_i_j);

#ifndef calcE
			// n_paren_i_j
			INTTYPE_REST new_n_paren_i_j = getIteratorValueForNumList(c1Count->n_paren_i_j, j) + getIteratorValueForNumList(c2Count->n_paren_i_j, j);
			addToNumList(ourCount, paren_i_j, j, new_n_paren_i_j);
#endif

#ifdef quartetsNoSwap
			// n_j_arrow_i
			INTTYPE_REST new_n_j_arrow_i = getIteratorValueForNumList(c1Count->n_j_arrow_i, j) + getIteratorValueForNumList(c2Count->n_j_arrow_i, j) +
				c1->countingVars->getIteratorValue(j)->n_i * c2Count->n_i;
			addToNumList(ourCount, j_arrow_i, j, new_n_j_arrow_i);

			// n_i_arrow_j
			INTTYPE_REST new_n_i_arrow_j = getIteratorValueForNumList(c1Count->n_i_arrow_j, j) + getIteratorValueForNumList(c2Count->n_i_arrow_j, j) +
				c1Count->n_i * c2->countingVars->getIteratorValue(j)->n_i;
			addToNumList(ourCount, i_arrow_j, j, new_n_i_arrow_j);

#ifndef calcE
			// Added by us for filling out tables (B)
			// n_i_paren_i_j
			INTTYPE_REST new_n_i_paren_i_j = getIteratorValueForNumList(c1Count->n_i_paren_i_j, j) + getIteratorValueForNumList(c2Count->n_i_paren_i_j, j);
			addToNumList(ourCount, i_paren_i_j, j, new_n_i_paren_i_j);

			// n_paren_i_paren_i_j
			INTTYPE_REST new_n_paren_i_paren_i_j = getIteratorValueForNumList(c1Count->n_paren_i_paren_i_j, j) + getIteratorValueForNumList(c2Count->n_paren_i_paren_i_j, j);
			addToNumList(ourCount, paren_i_paren_i_j, j, new_n_paren_i_paren_i_j);

			// n_bracket_i_paren_i_j
			INTTYPE_REST new_n_bracket_i_paren_i_j = getIteratorValueForNumList(c1Count->n_bracket_i_paren_i_j, j) + getIteratorValueForNumList(c2Count->n_bracket_i_paren_i_j, j) +
				c1Count->n_i * c1->countingVars->getIteratorValue(j)->n_i /* j */ * c2Count->n_i +
				c1Count->n_i * getIteratorValueForNumList(c2Count->n_paren_i_j, j) /*(ij)*/ +
				c1Count->n_i * getIteratorValueForNumList(c2Count->n_j_arrow_i, j) /*(j \UP i)*/ +
				c1->countingVars->getIteratorValue(j)->n_i /* j */ * c2Count->n_i_arrow_i;
			addToNumList(ourCount, bracket_i_paren_i_j, j, new_n_bracket_i_paren_i_j);
#endif
#endif

			if (ourCount->num != j)
			{
				// i != j
				INTTYPE_REST addThis;
#ifndef calcE
				// n_circ_arrow_paren_i_square
				addThis = (c1->n_circ - c1->countingVars->getIteratorValue(j)->n_i /* k */ - c1Count->n_i /* i */) *
					getIteratorValueForNumList(c2Count->n_paren_i_j, j) /* (ik) */;
				if (addThis < 0)
				{
					cout << "WTF #3?!?" << endl;
				}
				else ourCount->n_circ_arrow_paren_i_square += addThis;

				// Added by us!
				// n_circ_arrow_i_square
				addThis = (c1->n_circ - c1->countingVars->getIteratorValue(j)->n_i /* k */ - c1Count->n_i /* i */) *
					getIteratorValueForNumList(c2Count->n_i_j, j) /* ik */;
				if (addThis < 0)
				{
					cout << "WTF #4?!?" << endl;
				}
				else ourCount->n_circ_arrow_i_square += addThis;

				addThis = c1->countingVars->getIteratorValue(j)->n_i /* k */ * getIteratorValueForNumList(c2Count->n_paren_i_j, j) /* (ik) */;
				if (addThis < 0)
				{
					cout << "WTF #5?!?" << endl;
				}
				ourCount->n_circ_arrow_paren_i_circ += addThis;

				addThis = c1->countingVars->getIteratorValue(j)->n_i /* k */ * getIteratorValueForNumList(c2Count->n_i_j, j) /* ik */;
				if (addThis < 0)
				{
					cout << "WTF #6?!?" << endl;
				}
				ourCount->n_circ_arrow_i_circ += addThis;

#ifdef quartetsNoSwap
				// Added by us for filling out tables (B)
				// n_bracket_circ_paren_i_circ
				addThis = c1Count->n_i * c1->countingVars->getIteratorValue(j)->n_i /* j */ * c2->countingVars->getIteratorValue(j)->n_i /* j */ +
					c1->countingVars->getIteratorValue(j)->n_i /* j */ * getIteratorValueForNumList(c2Count->n_paren_i_j, j) /*(ij)*/ +
					c1->countingVars->getIteratorValue(j)->n_i /* j */ * getIteratorValueForNumList(c2Count->n_i_arrow_j, j) /*i \UP j*/;
				if (addThis < 0)
				{
					cout << "WTF #7?!?" << endl;
				}
				ourCount->n_bracket_circ_paren_i_circ += addThis;

				// n_bracket_circ_paren_i_square
				addThis = c1Count->n_i * c1->countingVars->getIteratorValue(j)->n_i /* j */  * (c2->n_circ - c2Count->n_i - c2->countingVars->getIteratorValue(j)->n_i /* j */) +
					(c1->n_circ - c1Count->n_i - c1->countingVars->getIteratorValue(j)->n_i /* j */) *
						(getIteratorValueForNumList(c2Count->n_paren_i_j, j) /*(ij)*/ + getIteratorValueForNumList(c2Count->n_i_arrow_j, j) /*i \UP j*/);
				if (addThis < 0)
				{
					cout << "WTF #8?!?" << endl;
				}
				ourCount->n_bracket_circ_paren_i_square += addThis;

				// n_circ_arrow_i_arrow_circ
				addThis = c1->countingVars->getIteratorValue(j)->n_i /* j */ * getIteratorValueForNumList(c2Count->n_i_arrow_j, j) /*i \UP j*/ +
					getIteratorValueForNumList(c1Count->n_j_arrow_i, j) /*j \UP i*/ * c2->countingVars->getIteratorValue(j)->n_i /* j */;
				if (addThis < 0)
				{
					cout << "WTF #9?!?" << endl;
				}
				ourCount->n_circ_arrow_i_arrow_circ += addThis;

				// n_circ_arrow_circ_arrow_i
				addThis = c1->countingVars->getIteratorValue(j)->n_i /* j */ * getIteratorValueForNumList(c2Count->n_j_arrow_i, j) /*j \UP i*/;
				if (addThis < 0)
				{
					cout << "WTF #10?!?" << endl;
				}
				ourCount->n_circ_arrow_circ_arrow_i += addThis;

				// n_paren_i_circ_arrow_circ
				addThis = getIteratorValueForNumList(c1Count->n_paren_i_j, j) /*(ij)*/ * c2->countingVars->getIteratorValue(j)->n_i /* j */;
				if (addThis < 0)
				{
					cout << "WTF #11?!?" << endl;
				}
				ourCount->n_paren_i_circ_arrow_circ += addThis;

				// n_circ_arrow_i_arrow_square
				addThis = c1->countingVars->getIteratorValue(j)->n_i /* j */ * (c2Count->n_i_arrow_circ - getIteratorValueForNumList(c2Count->n_i_arrow_j, j) /*i \UP j*/) +
					getIteratorValueForNumList(c1Count->n_j_arrow_i, j) * (c2->n_circ - c2Count->n_i - c2->countingVars->getIteratorValue(j)->n_i /* j */);
				if (addThis < 0)
				{
					cout << "WTF #12?!?" << endl;
				}
				ourCount->n_circ_arrow_i_arrow_square += addThis;

				// n_paren_i_circ_arrow_square
				addThis = getIteratorValueForNumList(c1Count->n_paren_i_j, j) /*(ij)*/ * (c2->n_circ - c2Count->n_i - c2->countingVars->getIteratorValue(j)->n_i /* j */);
				if (addThis < 0)
				{
					cout << "WTF #13?!?" << endl;
				}
				ourCount->n_paren_i_circ_arrow_square += addThis;

				// n_circ_arrow_square_arrow_i
				addThis = c1->countingVars->getIteratorValue(j)->n_i /* j */ * (c2Count->n_circ_arrow_i - getIteratorValueForNumList(c2Count->n_j_arrow_i, j));
				if (addThis < 0)
				{
					cout << "WTF #14?!?" << endl;
				}
				ourCount->n_circ_arrow_square_arrow_i += addThis;
#endif
#endif

#ifdef calcE
				// E start
				// n_i_circ_arrow_square
				addThis = getIteratorValueForNumList(c1Count->n_i_j, j) /* ij */ * (c2->n_circ - c2Count->n_i - c2->countingVars->getIteratorValue(j)->n_i /* j */);
				if (addThis < 0)
				{
					cout << "WTF #15?!?" << endl;
				}
				ourCount->n_i_circ_arrow_square += addThis;

				addThis = (c1->n_circ - c1Count->n_i - c1->countingVars->getIteratorValue(j)->n_i /* j */) * getIteratorValueForNumList(c2Count->n_i_j, j) /* ij */;
				if (addThis < 0)
				{
					cout << "WTF #17?!?" << endl;
				}
				ourCount->n_bracket_i_circ_square += addThis;
				// E end
#endif

#ifndef calcE
				// Figure 16: beta & alpha (3rd one that is)
				quartResolvedDisagree += (INTTYPE_N4) c1Count->n_i /* i */ * c1->countingVars->getIteratorValue(j)->n_i /* j */ *
					(c2Count->n_paren_i_circ - getIteratorValueForNumList(c2Count->n_paren_i_j, j) /* (ij) */);

				// Figure 16: beta & beta (3rd one that is)
				quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i /* i */ * c1->countingVars->getIteratorValue(j)->n_i /* j */ *
					(c2Count->n_i_circ - getIteratorValueForNumList(c2Count->n_i_j, j) /* ij */);
#endif

#ifdef quartetsNoSwap
				// Added by us for filling out tables
				ourCount->n_bracket_i_paren_circ_circ += c1->countingVars->getIteratorValue(j)->n_i * getIteratorValueForNumList(c2Count->n_j_arrow_i, j);

				ourCount->n_bracket_i_paren_circ_square += c1->countingVars->getIteratorValue(j)->n_i * (c2Count->n_circ_arrow_i - getIteratorValueForNumList(c2Count->n_j_arrow_i, j));

				ourCount->n_i_arrow_circ_arrow_circ += getIteratorValueForNumList(c1Count->n_i_arrow_j, j) * c2->countingVars->getIteratorValue(j)->n_i;

				ourCount->n_i_arrow_circ_arrow_square += getIteratorValueForNumList(c1Count->n_i_arrow_j, j) * (c2->n_circ - c2Count->n_i - c2->countingVars->getIteratorValue(j)->n_i);

#ifndef calcE
				//
				// +------------------------------------------+
				// | New sums for calculating B (Added by us) |
				// +------------------------------------------+
				//

				// alpha & gamma (continues below)
				quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * c1->countingVars->getIteratorValue(j)->n_i * getIteratorValueForNumList(c2Count->n_i_arrow_j, j);

				// beta & gamma (part 1) (continues below)
				quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * c1->countingVars->getIteratorValue(j)->n_i * (c2Count->n_i_arrow_circ - getIteratorValueForNumList(c2Count->n_i_arrow_j, j));
				quartResolvedDisagreeUpper += (INTTYPE_N4) getIteratorValueForNumList(c1Count->n_bracket_i_paren_i_j, j) * (c2->n_circ - c2Count->n_i - c2->countingVars->getIteratorValue(j)->n_i);
				quartResolvedDisagreeUpper += (INTTYPE_N4) c1->countingVars->getIteratorValue(j)->n_i /* k */ * (c2Count->n_paren_i_paren_i_circ - getIteratorValueForNumList(c2Count->n_paren_i_paren_i_j, j));

				// beta & gamma (part 2) (continues below)
				quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * c1->countingVars->getIteratorValue(j)->n_i /*j*/ * (c2Count->n_circ_arrow_i - getIteratorValueForNumList(c2Count->n_j_arrow_i, j));
#endif
#endif
			}

#ifndef calcE
			if (ourCount->num < j)
			{
				// j > i

				// Figure 16: alpha & alpha sum (2nd one that is)
				quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i /* i */ * c1->countingVars->getIteratorValue(j)->n_i /* j */ *
					getIteratorValueForNumList(c2Count->n_paren_i_j, j) /* (ij) */;

#ifdef quartetsNoSwap
				//
				// +------------------------------------------+
				// | New sums for calculating B (Added by us) |
				// +------------------------------------------+
				//

				// alpha & beta (continues below)
				quartResolvedDisagreeUpper += c1Count->n_i * c1->countingVars->getIteratorValue(j)->n_i * getIteratorValueForNumList(c2Count->n_i_j, j);
#endif
			}
#endif

			lastJPlus1 = j+1;
		}

#ifndef calcE
		// Figure 15 counters (part 1)
		ourCount->n_i_paren_0_i = c1Count->n_i_paren_0_i + c2Count->n_i_paren_0_i;
		ourCount->n_circ_paren_0_i = c1Count->n_circ_paren_0_i + c2Count->n_circ_paren_0_i;
		ourCount->n_0_paren_i_circ = c1Count->n_0_paren_i_circ + c2Count->n_0_paren_i_circ;
		ourCount->n_circ_paren_i_square = c1Count->n_circ_paren_i_square + c2Count->n_circ_paren_i_square;
		ourCount->n_i_paren_i_circ = c1Count->n_i_paren_i_circ + c2Count->n_i_paren_i_circ;
		ourCount->n_circ_paren_i_circ = c1Count->n_circ_paren_i_circ + c2Count->n_circ_paren_i_circ;

		// Figure 15 counters (part 2)
		/*
		ourCount->n_paren_i_paren_i_circ = 0;
		*/
		ourCount->n_paren_i_paren_0_i = c1Count->n_paren_i_paren_0_i + c2Count->n_paren_i_paren_0_i;
		ourCount->n_paren_0_paren_i_circ = c1Count->n_paren_0_paren_i_circ + c2Count->n_paren_0_paren_i_circ;
		ourCount->n_paren_circ_paren_0_i = c1Count->n_paren_circ_paren_0_i + c2Count->n_paren_circ_paren_0_i;

		// Figure 15 counters (part 3)
		ourCount->n_bracket_i_paren_i_circ = c1Count->n_bracket_i_paren_i_circ + c2Count->n_bracket_i_paren_i_circ +
			c1Count->n_i * (c1->n_circ - c1Count->n_i) * c2Count->n_i +
			c1Count->n_i * c2Count->n_circ_arrow_i +
			(c1->n_circ - c1Count->n_i) * c2Count->n_i_arrow_i +
			c1Count->n_i * c2Count->n_paren_i_circ;

		ourCount->n_bracket_0_paren_i_circ = c1Count->n_bracket_0_paren_i_circ + c2Count->n_bracket_0_paren_i_circ +
			c1Count->n_i * (c1->n_circ - c1Count->n_i) * c2Zero +
			(c1->n_circ - c1Count->n_i) * c2Count->n_i_arrow_0 +
			c1Zero * c2Count->n_paren_i_circ +
			// added by us
			c1Count->n_i * (c2->n_circ_arrow_0 - c2Count->n_i_arrow_0);

		ourCount->n_bracket_i_paren_0_i = c1Count->n_bracket_i_paren_0_i + c2Count->n_bracket_i_paren_0_i +
			c1Count->n_i * c2Count->n_paren_0_i +
			c1Count->n_i * c2Count->n_0_arrow_i +
			(c1Zero * c1Count->n_i) * c2Count->n_i +
			// added by us
			c1Zero * c2Count->n_i_arrow_i;

		ourCount->n_bracket_circ_paren_0_i = c1Count->n_bracket_circ_paren_0_i + c2Count->n_bracket_circ_paren_0_i +
			(c1Count->n_i * c1Zero) * (c2->n_circ - c2Count->n_i) +
			(c1->n_circ - c1Count->n_i) * c2Count->n_paren_0_i +
			// added by us
			c1Zero * c2Count->n_i_arrow_circ +
			c1Count->n_i * (c2->n_0_arrow_circ - c2Count->n_0_arrow_i);

		// Figure 15 counters (part 4)
#endif
		ourCount->n_0_arrow_i_circ = c1Count->n_0_arrow_i_circ + c2Count->n_0_arrow_i_circ +
			c1Zero * c2Count->n_i_circ;

#ifndef calcE
		ourCount->n_circ_arrow_0_i = c1Count->n_circ_arrow_0_i + c2Count->n_circ_arrow_0_i +
			(c1->n_circ - c1Count->n_i) * c2Count->n_0_i;

		ourCount->n_i_arrow_i_circ = c1Count->n_i_arrow_i_circ + c2Count->n_i_arrow_i_circ +
			c1Count->n_i * c2Count->n_i_circ;

		ourCount->n_paren_i_circ_arrow_0 = c1Count->n_paren_i_circ_arrow_0 + c2Count->n_paren_i_circ_arrow_0 +
			c1Count->n_paren_i_circ * c2Zero;

		ourCount->n_paren_i_circ_arrow_i = c1Count->n_paren_i_circ_arrow_i + c2Count->n_paren_i_circ_arrow_i +
			c1Count->n_paren_i_circ * c2Count->n_i;
		
		ourCount->n_paren_0_i_arrow_i = c1Count->n_paren_0_i_arrow_i + c2Count->n_paren_0_i_arrow_i +
			c1Count->n_paren_0_i * c2Count->n_i;

		ourCount->n_paren_0_i_arrow_circ = c1Count->n_paren_0_i_arrow_circ + c2Count->n_paren_0_i_arrow_circ
			+ c1Count->n_paren_0_i * (c2->n_circ - c2Count->n_i);

		ourCount->n_0_arrow_paren_i_circ = c1Count->n_0_arrow_paren_i_circ + c2Count->n_0_arrow_paren_i_circ +
			c1Zero * c2Count->n_paren_i_circ;

		ourCount->n_i_arrow_paren_i_circ = c1Count->n_i_arrow_paren_i_circ + c2Count->n_i_arrow_paren_i_circ +
			c1Count->n_i * c2Count->n_paren_i_circ;

		ourCount->n_circ_arrow_paren_0_i = c1Count->n_circ_arrow_paren_0_i + c2Count->n_circ_arrow_paren_0_i +
			(c1->n_circ - c1Count->n_i) * c2Count->n_paren_0_i;

		ourCount->n_i_arrow_paren_0_i = c1Count->n_i_arrow_paren_0_i + c2Count->n_i_arrow_paren_0_i +
			c1Count->n_i * c2Count->n_paren_0_i;
		
		ourCount->n_circ_arrow_i_arrow_0 = c1Count->n_circ_arrow_i_arrow_0 + c2Count->n_circ_arrow_i_arrow_0 +
			(c1->n_circ - c1Count->n_i) * c2Count->n_i_arrow_0 +
			c1Count->n_circ_arrow_i * c2Zero;

		ourCount->n_i_arrow_i_arrow_0 = c1Count->n_i_arrow_i_arrow_0 + c2Count->n_i_arrow_i_arrow_0 +
			c1Count->n_i * c2Count->n_i_arrow_0 +
			c1Count->n_i_arrow_i * c2Zero;

		ourCount->n_0_arrow_circ_arrow_i = c1Count->n_0_arrow_circ_arrow_i + c2Count->n_0_arrow_circ_arrow_i +
			c1Zero * c2Count->n_circ_arrow_i +
			(c1->n_0_arrow_circ - c1Count->n_0_arrow_i) * c2Count->n_i;
		
		ourCount->n_circ_arrow_0_arrow_i = c1Count->n_circ_arrow_0_arrow_i + c2Count->n_circ_arrow_0_arrow_i +
			(c1->n_circ - c1Count->n_i) * c2Count->n_0_arrow_i +
			(c1->n_circ_arrow_0 - c1Count->n_i_arrow_0) * c2Count->n_i;
		
		ourCount->n_i_arrow_0_arrow_i = c1Count->n_i_arrow_0_arrow_i + c2Count->n_i_arrow_0_arrow_i +
			c1Count->n_i * c2Count->n_0_arrow_i +
			c1Count->n_i_arrow_0 * c2Count->n_i;

		ourCount->n_0_arrow_i_arrow_circ = c1Count->n_0_arrow_i_arrow_circ + c2Count->n_0_arrow_i_arrow_circ +
			c1Zero * c2Count->n_i_arrow_circ +
			c1Count->n_0_arrow_i * (c2->n_circ - c2Count->n_i);

		ourCount->n_i_arrow_i_arrow_circ = c1Count->n_i_arrow_i_arrow_circ + c2Count->n_i_arrow_i_arrow_circ +
			c1Count->n_i * c2Count->n_i_arrow_circ +
			c1Count->n_i_arrow_i * (c2->n_circ - c2Count->n_i);
		
		// Added by us
		ourCount->n_i_arrow_0_i = c1Count->n_i_arrow_0_i + c2Count->n_i_arrow_0_i +
			c1Count->n_i * c2Count->n_0_i;

		ourCount->n_i_arrow_circ_arrow_i = c1Count->n_i_arrow_circ_arrow_i + c2Count->n_i_arrow_circ_arrow_i +
			c1Count->n_i * c2Count->n_circ_arrow_i +
			c1Count->n_i_arrow_circ * c2Count->n_i;

		// Figure 15 sums (count up)
		n_bracket_circ_paren_circ_square += ourCount->n_bracket_i_paren_i_circ;
		n_circ_arrow_circ_square += ourCount->n_i_arrow_i_circ;
		n_paren_circ_square_arrow_circ += ourCount->n_paren_i_circ_arrow_i;

		/*
		n_paren_circ_arrow_circ_arrow_square = 0;
		*/
#endif

#ifdef quartetsNoSwap
		// Added by us for filling out tables
		ourCount->n_paren_i_paren_circ_circ = c1Count->n_paren_i_paren_circ_circ + c2Count->n_paren_i_paren_circ_circ;

		ourCount->n_paren_i_paren_circ_square = c1Count->n_paren_i_paren_circ_square + c2Count->n_paren_i_paren_circ_square;

		ourCount->n_paren_circ_square_arrow_i = c1Count->n_paren_circ_square_arrow_i + c2Count->n_paren_circ_square_arrow_i +
			(c1->n_paren_circ_square - c1Count->n_paren_i_circ) * c2Count->n_i;
#endif

#ifdef calcE
		// New counters for calculating E
		ourCount->n_i_circ_square = c1Count->n_i_circ_square + c2Count->n_i_circ_square;

		ourCount->n_circ_square_arrow_i = c1Count->n_circ_square_arrow_i + c2Count->n_circ_square_arrow_i +
			(c1->n_circ_square - c1Count->n_i_circ) * c2Count->n_i;

		ourCount->n_paren_i_circ_square = c1Count->n_paren_i_circ_square + c2Count->n_paren_i_circ_square;

		ourCount->n_0_i_circ = c1Count->n_0_i_circ + c2Count->n_0_i_circ;

		ourCount->n_i_circ_arrow_0 = c1Count->n_i_circ_arrow_0 + c2Count->n_i_circ_arrow_0 +
			c1Count->n_i_circ * c2Zero;

		ourCount->n_0_i_arrow_circ = c1Count->n_0_i_arrow_circ + c2Count->n_0_i_arrow_circ +
			c1Count->n_0_i * (c2->n_circ - c2Count->n_i);

		ourCount->n_0_circ_arrow_i = c1Count->n_0_circ_arrow_i + c2Count->n_0_circ_arrow_i +
			(c1->n_0_circ - c1Count->n_0_i) * c2Count->n_i;

		ourCount->n_paren_0_i_circ = c1Count->n_paren_0_i_circ + c2Count->n_paren_0_i_circ;

		ourCount->n_bracket_0_i_circ = c1Count->n_bracket_0_i_circ + c2Count->n_bracket_0_i_circ +
			c1Zero * c2Count->n_i_circ + 
			c1Count->n_i * (c2->n_0_circ - c2Count->n_0_i) +
			(c1->n_circ - c1Count->n_i) * c2Count->n_0_i;
#endif
#endif

		// Sums and stuff
		n_circ_square += ourCount->n_i_circ;
				
		// Quartets
#ifdef quartetsToo
		n_circ_circ += ourCount->n_ii;
		n_square_paren_circ_circ += ourCount->n_i_paren_circ_circ;
		n_paren_circ_circ += ourCount->n_paren_ii;
		n_paren_circ_square += ourCount->n_paren_i_circ;
		n_circ_arrow_paren_square_square += ourCount->n_circ_arrow_paren_ii; // FIXED!!
		n_bracket_circ_square += ourCount->n_bracket_i_circ;

		// Added by us
		n_circ_arrow_square_square += ourCount->n_i_arrow_circ_circ;

#ifndef calcE
		// B
		n_circ_arrow_paren_circ_square += ourCount->n_i_arrow_paren_i_circ;
		n_circ_arrow_circ_arrow_square += ourCount->n_i_arrow_i_arrow_circ;
		n_circ_arrow_square_arrow_circ += ourCount->n_i_arrow_circ_arrow_i;
#endif

#ifdef quartetsNoSwap
		// Added by us for filling out tables
		// A
		n_bracket_circ_circ += Util::binom2(ourCount->n_i);
		n_paren_circ_paren_square_square += ourCount->n_paren_i_paren_circ_circ;
		n_circ_arrow_circ += ourCount->n_i_arrow_i;
		n_circ_arrow_square += ourCount->n_i_arrow_circ;
		n_paren_circ_circ_arrow_square += ourCount->n_paren_ii_arrow_circ;
		n_bracket_circ_paren_square_square += ourCount->n_bracket_i_paren_circ_circ;
		n_circ_arrow_square_arrow_square += ourCount->n_circ_arrow_i_arrow_i;

#ifndef calcE
		// B
		ourCount->n_paren_circ_paren_i_circ = c1Count->n_paren_circ_paren_i_circ + c2Count->n_paren_circ_paren_i_circ;
		ourCount->n_paren_i_paren_i_circ = c1Count->n_paren_i_paren_i_circ + c2Count->n_paren_i_paren_i_circ;
		ourCount->n_paren_circ_paren_i_square = c1Count->n_paren_circ_paren_i_square + c2Count->n_paren_circ_paren_i_square;
#endif
#endif
#ifdef calcE
		// New sums and stuff for calculating E
		n_circ_square_arrow_triangle += ourCount->n_circ_square_arrow_i;
		n_circ_arrow_square_triangle += ourCount->n_i_arrow_circ_square;
		
		n_0_circ_arrow_square += ourCount->n_0_i_arrow_circ;
		n_0_arrow_circ_square += ourCount->n_0_arrow_i_circ;
		n_circ_square_arrow_0 += ourCount->n_i_circ_arrow_0;
		n_circ_arrow_0_square += ourCount->n_i_arrow_0_circ;

		n_bracket_circ_square_triangle += ourCount->n_bracket_i_circ_square;
		n_bracket_0_circ_square += ourCount->n_bracket_0_i_circ;
#endif
#endif

		// Update resolved/resolved and unresolved/unresolved
		// (Figure 10)
		tripResolved += c1Count->n_i * c2Count->n_i_arrow_circ;
		tripResolved += Util::binom2(c1Count->n_i) * (c2->n_circ - c2Count->n_i);
		tripResolved += (c1->n_circ - c1Count->n_i) * c2Count->n_paren_ii;
		tripUnresolved += c1Count->n_i * (c2->n_circ_square - c2Count->n_i_circ);

		// Quartets
#ifdef quartetsToo
		//quartResolvedAgree & quartResolvedDisagree

		// alpha & alpha
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_i_arrow_paren_circ_circ;
		quartResolvedAgreeDiag += (INTTYPE_N4) Util::binom2(c1Count->n_i) * (c2->n_paren_circ_circ - c2Count->n_paren_ii);

		// beta & alpha
		quartResolvedAgree += (INTTYPE_N4) c1Count->n_i * c2Count->n_i_arrow_paren_circ_square;
		quartResolvedAgree += (INTTYPE_N4) Util::binom2(c1Count->n_i) * (c2->n_paren_circ_square - c2Count->n_paren_i_circ);
		quartResolvedAgree += (INTTYPE_N4) (c1->n_bracket_circ_square - c1Count->n_bracket_i_circ) * c2Count->n_paren_ii;
		quartResolvedAgree += (INTTYPE_N4) c1Count->n_i * (c2->n_circ_arrow_paren_square_square - c2Count->n_i_arrow_paren_circ_circ - c2Count->n_circ_arrow_paren_ii);
		
		// beta & beta (part 1)
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_i_arrow_circ_square;
		quartResolvedAgreeDiag += (INTTYPE_N4) Util::binom2(c1Count->n_i) * (c2->n_circ_square - c2Count->n_i_circ);
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Count->n_i * (c2->n_square_paren_circ_circ - c2Count->n_i_paren_circ_circ - c2Count->n_circ_paren_ii);
		
		// beta & beta (part 2)
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Count->n_i * (c2->n_circ_arrow_square_square - c2Count->n_i_arrow_circ_circ - c2Count->n_circ_arrow_ii);
		quartResolvedAgreeDiag += (INTTYPE_N4) (c1->n_bracket_circ_square - c1Count->n_bracket_i_circ) * c2Count->n_ii;
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_i_paren_circ_square;
		
		// gamma & alpha
		quartResolvedAgree += (INTTYPE_N4) c1Count->n_i * c2Count->n_i_arrow_paren_0_circ;
		quartResolvedAgree += (INTTYPE_N4) Util::binom2(c1Count->n_i) * (c2->n_paren_0_circ - c2Count->n_paren_0_i);
		quartResolvedAgree += (INTTYPE_N4) c1Zero * (c1->n_circ - c1Count->n_i) * c2Count->n_paren_ii;
		quartResolvedAgree += (INTTYPE_N4) (c1->n_circ - c1Count->n_i) * c2Count->n_0_arrow_paren_ii;
		quartResolvedAgree += (INTTYPE_N4) c1Zero * c2Count->n_circ_arrow_paren_ii;
		
		// gamma & beta (part 1)
		quartResolvedAgree += (INTTYPE_N4) Util::binom2(c1Count->n_i) * (c2->n_0_circ - c2Count->n_0_i);
		quartResolvedAgree += (INTTYPE_N4) c1Count->n_i * c2Count->n_i_arrow_0_circ;
		quartResolvedAgree += (INTTYPE_N4) c1Zero * c2Count->n_circ_paren_ii;
		quartResolvedAgree += (INTTYPE_N4) (c1->n_circ - c1Count->n_i) * c2Count->n_0_paren_ii;
		
		// gamma & beta (part 2)
		quartResolvedAgree += (INTTYPE_N4) (c1->n_circ - c1Count->n_i) * c2Count->n_0_arrow_ii;
		quartResolvedAgree += (INTTYPE_N4) c1Zero * c2Count->n_circ_arrow_ii;
		quartResolvedAgree += (INTTYPE_N4) c1Zero * (c1->n_circ - c1Count->n_i) * c2Count->n_ii;
		quartResolvedAgree += (INTTYPE_N4) c1Count->n_i * c2Count->n_i_paren_0_circ;
		
		// gamma & gamma (part 1)
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_i_arrow_circ_arrow_0;
		quartResolvedAgreeDiag += (INTTYPE_N4) Util::binom2(c1Count->n_i) * (c2->n_circ_arrow_0 - c2Count->n_i_arrow_0);
		quartResolvedAgreeDiag += (INTTYPE_N4) (c1->n_circ - c1Count->n_i) * c2Count->n_paren_ii_arrow_0;
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Count->n_bracket_circ_paren_ii * c2Zero;
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Zero * c2Count->n_paren_circ_paren_ii;
		
		// gamma & gamma (part 2)
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_i_arrow_0_arrow_circ;
		quartResolvedAgreeDiag += (INTTYPE_N4) Util::binom2(c1Count->n_i) * (c2->n_0_arrow_circ - c2Count->n_0_arrow_i);
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Zero * c2Count->n_paren_ii_arrow_circ;
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Count->n_bracket_0_paren_ii * (c2->n_circ - c2Count->n_i);
		quartResolvedAgreeDiag += (INTTYPE_N4) (c1->n_circ - c1Count->n_i) * c2Count->n_paren_0_paren_ii;
		
		// gamma & gamma (part 3) (aka the exiting conclusion)
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Zero * c2Count->n_circ_arrow_i_arrow_i;
		quartResolvedAgreeDiag += (INTTYPE_N4) (c1->n_circ - c1Count->n_i) * c2Count->n_0_arrow_i_arrow_i;
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Zero * (c1->n_circ - c1Count->n_i) * c2Count->n_i_arrow_i;
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_paren_0_circ_arrow_i;
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Count->n_bracket_i_paren_0_circ * c2Count->n_i;
		quartResolvedAgreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_paren_i_paren_0_circ;
		
#ifndef calcE
		// Figure 16
		// alpha & alpha
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_arrow_paren_i_circ;
		// next alpha & alpha moved up

		// beta & alpha (the easy ones)
		quartResolvedDisagree += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_arrow_paren_i_square;
		quartResolvedDisagree += (INTTYPE_N4) c1Count->n_i * (c2->n_circ_arrow_paren_circ_square - c2Count->n_i_arrow_paren_i_circ - c2Count->n_circ_arrow_paren_i_circ);
		// beta & alpha (the not-so-nice-one) has been moved up

		// beta & beta (the nice ones)
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_arrow_i_square;
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * (c2->n_circ_arrow_circ_square - c2Count->n_circ_arrow_i_circ - c2Count->n_i_arrow_i_circ);
		// beta & beta (the not-so-nice-one) has been moved up
		// beta & beta (more nice ones)
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_paren_i_square;
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * (c2->n_circ_paren_circ_square - c2Count->n_circ_paren_i_circ - c2Count->n_i_paren_i_circ);

		// gamma & alpha
		quartResolvedDisagree += (INTTYPE_N4) c1Count->n_i * c2Count->n_0_arrow_paren_i_circ;
		quartResolvedDisagree += (INTTYPE_N4) c1Zero * c2Count->n_i_arrow_paren_i_circ; // TODO: Maybe move the c1Zero outside the loop
		quartResolvedDisagree += (INTTYPE_N4) c1Zero * c1Count->n_i * c2Count->n_paren_i_circ; // TODO: Maybe move the c1Zero outside the loop
		quartResolvedDisagree += (INTTYPE_N4) c1Count->n_i * (c1->n_circ - c1Count->n_i) * c2Count->n_paren_0_i;
		quartResolvedDisagree += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_arrow_paren_0_i;
		quartResolvedDisagree += (INTTYPE_N4) (c1->n_circ - c1Count->n_i) * c2Count->n_i_arrow_paren_0_i;

		// gamma & beta (part 1) (continues below the loop)
		quartResolvedDisagree += (INTTYPE_N4) c1Count->n_i * c2Count->n_0_arrow_i_circ;
		quartResolvedDisagree += (INTTYPE_N4) c1Zero * c1Count->n_i * c2Count->n_i_circ; // TODO: Maybe move the c1Zero outside the loop
		quartResolvedDisagree += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_paren_0_i;
		quartResolvedDisagree += (INTTYPE_N4) (c1->n_circ - c1Count->n_i) * c2Count->n_i_paren_0_i;

		// gamma & beta (part 2) (continues below the loop)
		quartResolvedDisagree += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_arrow_0_i;
		quartResolvedDisagree += (INTTYPE_N4) (c1->n_circ - c1Count->n_i) * c2Count->n_i_arrow_0_i;
		quartResolvedDisagree += (INTTYPE_N4) c1Count->n_i * (c1->n_circ - c1Count->n_i) * c2Count->n_0_i;
		quartResolvedDisagree += (INTTYPE_N4) c1Count->n_i * c2Count->n_0_paren_i_circ;

		// gamma & gamma (part 1) (continues below the loop)
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_0_arrow_i_arrow_circ;
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Zero * c1Count->n_i * c2Count->n_i_arrow_circ; // TODO: Maybe move the c1Zero outside the loop
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_paren_0_i_arrow_circ;
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_bracket_i_paren_0_i * (c2->n_circ - c2Count->n_i);
		quartResolvedDisagreeDiag += (INTTYPE_N4) (c1->n_circ - c1Count->n_i) * c2Count->n_paren_i_paren_0_i;

		// gamma & gamma (part 2) (continues below the loop)
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_0_arrow_circ_arrow_i;
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Zero * c1Count->n_i * c2Count->n_circ_arrow_i; // TODO: Maybe move the c1Zero outside the loop
		quartResolvedDisagreeDiag += (INTTYPE_N4) (c1->n_circ - c1Count->n_i) * c2Count->n_paren_0_i_arrow_i;
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_bracket_circ_paren_0_i * c2Count->n_i; // Stupid article writing n_bracket_paren_0_i_circ instead :x
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_paren_circ_paren_0_i;
		
		// gamma & gamma (part 3) (continues below the loop)
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_arrow_i_arrow_0;
		quartResolvedDisagreeDiag += (INTTYPE_N4) (c1->n_circ - c1Count->n_i) * c2Count->n_i_arrow_i_arrow_0;
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * (c1->n_circ - c1Count->n_i) * c2Count->n_i_arrow_0;
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_paren_i_circ_arrow_0;
		quartResolvedDisagreeDiag += (INTTYPE_N4) c2Zero * c1Count->n_bracket_i_paren_i_circ; // TODO: Maybe move the c2Zero outside the loop  // Stupid article writing n_bracket_paren_0_i_circ instead :x

		// gamma & gamma (part 4) (continues below the loop)
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_arrow_0_arrow_i;
		quartResolvedDisagreeDiag += (INTTYPE_N4) (c1->n_circ - c1Count->n_i) * c2Count->n_i_arrow_0_arrow_i;
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * (c1->n_circ - c1Count->n_i) * c2Count->n_0_arrow_i;
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_bracket_0_paren_i_circ * c2Count->n_i;
		quartResolvedDisagreeDiag += (INTTYPE_N4) c1Count->n_i * c2Count->n_paren_0_paren_i_circ;
#endif

#ifdef quartetsNoSwap
		//
		// +------------------------------------------+
		// | New sums for calculating A (Added by us) |
		// +------------------------------------------+
		//

		// alpha & beta
		quartResolvedAgreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_i_arrow_circ_circ;
		quartResolvedAgreeUpper += (INTTYPE_N4) Util::binom2(c1Count->n_i) * (c2->n_circ_circ - c2Count->n_ii);
		quartResolvedAgreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_i_paren_circ_circ;

		// alpha & gamma
		quartResolvedAgreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_i_arrow_circ_arrow_circ;
		quartResolvedAgreeUpper += (INTTYPE_N4) Util::binom2(c1Count->n_i) * (c2->n_circ_arrow_circ - c2Count->n_i_arrow_i);
		quartResolvedAgreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_paren_circ_circ_arrow_i;
		quartResolvedAgreeUpper += (INTTYPE_N4) c1Count->n_bracket_i_paren_circ_circ * c2Count->n_i;
		quartResolvedAgreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_paren_i_paren_circ_circ;

		// beta & gamma (part 1)
		quartResolvedAgreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_i_arrow_circ_arrow_square;
		quartResolvedAgreeUpper += (INTTYPE_N4) Util::binom2(c1Count->n_i) * (c2->n_circ_arrow_square - c2Count->n_i_arrow_circ - c2Count->n_circ_arrow_i);
		quartResolvedAgreeUpper += (INTTYPE_N4) c1Count->n_i * (c2->n_paren_circ_circ_arrow_square - c2Count->n_paren_ii_arrow_circ - c2Count->n_paren_circ_circ_arrow_i);
		quartResolvedAgreeUpper += (INTTYPE_N4) (c1->n_bracket_circ_paren_square_square - c1Count->n_bracket_i_paren_circ_circ - c1Count->n_bracket_circ_paren_ii) * c2Count->n_i;
		quartResolvedAgreeUpper += (INTTYPE_N4) c1Count->n_i * (c2->n_paren_circ_paren_square_square - c2Count->n_paren_i_paren_circ_circ - c2Count->n_paren_circ_paren_ii);

		// beta & gamma (part 2)
		quartResolvedAgreeUpper += (INTTYPE_N4) c1Count->n_i * (c2->n_circ_arrow_square_arrow_square - c2Count->n_i_arrow_circ_arrow_circ - c2Count->n_circ_arrow_i_arrow_i);
		quartResolvedAgreeUpper += (INTTYPE_N4) (c1->n_bracket_circ_square - c1Count->n_bracket_i_circ) * c2Count->n_i_arrow_i;
		quartResolvedAgreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_paren_circ_square_arrow_i;
		quartResolvedAgreeUpper += (INTTYPE_N4) c1Count->n_bracket_i_paren_circ_square * c2Count->n_i;
		quartResolvedAgreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_paren_i_paren_circ_square;

#ifndef calcE
		//
		// +------------------------------------------+
		// | New sums for calculating B (Added by us) |
		// +------------------------------------------+
		//

		// alpha & beta (continues from above)
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_arrow_i_circ;
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_paren_i_circ;

		// alpha & gamma (continues from above)
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_arrow_i_arrow_circ;
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_arrow_circ_arrow_i;
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_paren_i_circ_arrow_circ;
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_bracket_circ_paren_i_circ * c2Count->n_i;
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_paren_circ_paren_i_circ;

		// beta & gamma (part 1) (continues from above)
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_arrow_i_arrow_square;
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * (c2->n_circ_arrow_circ_arrow_square - c2Count->n_i_arrow_i_arrow_circ - c2Count->n_circ_arrow_circ_arrow_i);
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_paren_i_circ_arrow_square;

		// beta & gamma (part 2) (continues from above)
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_circ_arrow_square_arrow_i;
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * (c2->n_circ_arrow_square_arrow_circ - c2Count->n_i_arrow_circ_arrow_i - c2Count->n_circ_arrow_i_arrow_circ);
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * (c2->n_paren_circ_square_arrow_circ - c2Count->n_paren_i_circ_arrow_i - c2Count->n_paren_i_circ_arrow_circ);
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_bracket_circ_paren_i_square * c2Count->n_i;
		quartResolvedDisagreeUpper += (INTTYPE_N4) c1Count->n_i * c2Count->n_paren_circ_paren_i_square;
#endif
#endif

#ifdef calcE
		//
		// +------------------------------------------+
		// | New sums for calculating E (Added by us) |
		// +------------------------------------------+
		//

		// delta & delta
		quartSumE += (INTTYPE_N4) c1Count->n_i * (c2->n_circ_square_triangle - c2Count->n_i_circ_square);

		// delta & epsilon
		quartSumE += (INTTYPE_N4) c1Count->n_i * (c2->n_circ_square_arrow_triangle - c2Count->n_i_circ_arrow_square - c2Count->n_circ_square_arrow_i);
		quartSumE += (INTTYPE_N4) (c1->n_bracket_circ_square_triangle - c1Count->n_bracket_i_circ_square) * c2Count->n_i;
		quartSumE += (INTTYPE_N4) c1Count->n_i * (c2->n_paren_circ_square_triangle - c2Count->n_paren_i_circ_square);
		
		// epsilon & delta (countinues below)
		quartSumE += (INTTYPE_N4) c1Count->n_i * (c2->n_0_circ_square - c2Count->n_0_i_circ);

		// epsilon & epsilon (part 1) (continues below)
		quartSumE += (INTTYPE_N4) c1Count->n_i * (c2->n_circ_square_arrow_0 - c2Count->n_i_circ_arrow_0);

		// epsilon & epsilon (part 2) (continues below)
		quartSumE += (INTTYPE_N4) c1Count->n_i * (c2->n_0_circ_arrow_square - c2Count->n_0_i_arrow_circ - c2Count->n_0_circ_arrow_i);
		quartSumE += (INTTYPE_N4) (c1->n_bracket_0_circ_square - c1Count->n_bracket_0_i_circ) * c2Count->n_i;
		quartSumE += (INTTYPE_N4) c1Count->n_i * (c2->n_paren_0_circ_square - c2Count->n_paren_0_i_circ);
#endif
#endif

		// Go to next on children unless we're done
		if (c1Next == NULL && c2Next == NULL)
			ourCount->type = CountingLinkedList::End;
		else
		{
			// Go to next one (there's more!)
			ourCount->type = CountingLinkedList::Regular;
			if (ourCount->next == NULL) ourCount->next = factory->getLL();
			ourCount = ourCount->next;
		}
	}
	n_circ_square /= 2;

	// Quartets
#ifdef quartetsToo
	n_paren_circ_square /= 2;
	n_bracket_circ_square /= 2;

#ifndef calcE
	// gamma & beta (part 1) (continued from inside the loop)
	quartResolvedDisagree += (INTTYPE_N4) c1Zero * c2->n_circ_arrow_circ_square;

	// gamma & beta (part 2) (continued from inside the loop)
	quartResolvedDisagree += (INTTYPE_N4) c1Zero * c2->n_circ_paren_circ_square;

	// gamma & gamma (part 1) (continued from inside the loop)
	quartResolvedDisagreeDiag += (INTTYPE_N4) c1Zero * c2->n_circ_arrow_circ_arrow_square;

	// gamma & gamma (part 2) (continued from inside the loop)
	quartResolvedDisagreeDiag += (INTTYPE_N4) c1Zero * c2->n_circ_arrow_square_arrow_circ;

	// gamma & gamma (part 3) (continued from inside the loop)
	quartResolvedDisagreeDiag += (INTTYPE_N4) c1Zero * c2->n_paren_circ_paren_circ_square;

	// gamma & gamma (part 4) (continued from inside the loop)
	quartResolvedDisagreeDiag += (INTTYPE_N4) c1Zero * c2->n_paren_circ_square_arrow_circ;
#endif

#ifdef calcE
	// New sums for calculating E
	// epsilon & delta (continued from inside the loop)
	quartSumE += (INTTYPE_N4) c1Zero * c2->n_circ_square_triangle;

	// epsilon & epsilon (part 1) (continued from inside the loop)
	quartSumE += (INTTYPE_N4) c1->n_bracket_circ_square_triangle * c2Zero;
	quartSumE += (INTTYPE_N4) c1Zero * c2->n_paren_circ_square_triangle;

	// epsilon & epsilon (part 2) (continued from inside the loop)
	quartSumE += (INTTYPE_N4) c1Zero * c2->n_circ_square_arrow_triangle;

	// Div E sum-counters :)
	if (n_bracket_circ_square_triangle % 3 != 0)
	{
		cout << "n_bracket_circ_square_triangle mod 3 check failed :(" << endl;
	}
	n_bracket_circ_square_triangle /= 3;

	if (n_bracket_0_circ_square % 2 != 0)
	{
		cout << "n_bracket_0_circ_square mod 2 check failed :(" << endl;
	}
	n_bracket_0_circ_square /= 2;

	if (n_circ_square_arrow_0 % 2 != 0)
	{
		cout << "n_circ_square_arrow_0 mod 2 check failed :(" << endl;
	}
	n_circ_square_arrow_0 /= 2;

	if (n_0_arrow_circ_square % 2 != 0)
	{
		cout << "n_0_arrow_circ_square mod 2 check failed :(" << endl;
	}
	n_0_arrow_circ_square /= 2;
#endif
#endif
}