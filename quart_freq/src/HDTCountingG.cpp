#include <cstdlib> // for exit
#include "hdt.h"
#include "rooted_tree.h"
#include "hdt_factory.h"
#include "counting_linked_list.h"
#include "soda13_calc.h"

void HDT::handleG()
{
	INC_RUNTIME_COUNTER
	// Not a leaf, i.e. a GG->G
	HDT *g1 = this->left;
	HDT *g2 = this->right;
	
	if (!g1->up2date) g1->updateCounters();
	if (!g2->up2date) g2->updateCounters();

	// n_i is just the sum of the 2 children, thus n_circ is also just the sum of the 2 children
	n_circ = g1->n_circ + g2->n_circ;

	// Initialize sum to 0 so we can increment it in the loop
	n_circ_square = 0;

	// Fetch the childrens "result counting data"
	tripResolved = g1->tripResolved + g2->tripResolved;
	tripUnresolved = g1->tripUnresolved + g2->tripUnresolved;

	// Pointer stuff for walking over all children's data
	CountingLinkedList *g1Next = g1->countingVars;
	CountingLinkedList *g1Count = NULL;
	CountingLinkedList *g2Next = g2->countingVars;
	CountingLinkedList *g2Count = NULL;
	CountingLinkedList *ourCount = countingVars;

#ifdef quartetsToo
	INTTYPE_REST g1Zero = 0;
	INTTYPE_REST g2Zero = 0;

	// The list is sorted, i.e. if zero's there it's the first one!
	if (g1Next->num == 0) g1Zero = g1Next->n_i;
	if (g2Next->num == 0) g2Zero = g2Next->n_i;

	// Not dependent on i (begin)
	n_0_circ = g1->n_0_circ + g2->n_0_circ + 
		g1Zero * g2->n_circ +
		g2Zero * g1->n_circ;
	
	n_paren_0_circ = g1->n_paren_0_circ + g2->n_paren_0_circ;
	// Not dependent on i (end)

	// Initialize sums to 0 so we can increment it in the loop
	n_circ_circ = 0;
	n_square_paren_circ_circ = 0;
	n_paren_circ_circ = 0;
	n_paren_circ_square = 0;
	n_bracket_circ_square = 0;

	/*
	// Actually undefined (and not dependent on i)
	n_0_arrow_circ = 0;
	n_circ_arrow_0 = 0;
	n_0_arrow_circ_circ = 0;
	n_circ_arrow_paren_square_square = 0;
	n_circ_arrow_square_square = 0; // Added by us
	*/

#ifndef calcE
	// Figure 15 (not dependent on i)
	n_paren_circ_paren_circ_square = g1->n_paren_circ_paren_circ_square + g2->n_paren_circ_paren_circ_square;

	// Figure 15 sums (initialize)
	n_bracket_circ_paren_circ_square = 0;
	n_circ_paren_circ_square = 0;

	/*
	// Figure 15 sums (undefined)
	n_circ_arrow_circ_square = 0;
	n_paren_circ_square_arrow_circ = 0;
	*/
	/*
	n_paren_circ_arrow_circ_arrow_square = 0;
	*/
#endif

	/*
	// Added by us (Actually undefined!)
	n_circ_arrow_paren_circ_square = 0; 
	n_circ_arrow_circ_arrow_square = 0;
	n_circ_arrow_square_arrow_circ = 0;
	*/

#ifdef quartetsNoSwap
	// Added by us for filling out tables (initialize)
	n_bracket_circ_circ = 0;
	n_paren_circ_paren_square_square = 0;
	n_bracket_circ_paren_square_square = 0;

	/*
	// Added by us for filling out tables (undefined!)
	n_circ_arrow_circ = 0;
	n_circ_arrow_square = 0;
	n_paren_circ_circ_arrow_square = 0;
	n_circ_arrow_square_arrow_square = 0;
	*/
#endif

	// Fetch the childrens "result counting data"
	quartResolvedAgree = g1->quartResolvedAgree + g2->quartResolvedAgree;
	quartResolvedAgreeDiag = g1->quartResolvedAgreeDiag + g2->quartResolvedAgreeDiag;
#ifndef calcE
	quartResolvedDisagree = g1->quartResolvedDisagree + g2->quartResolvedDisagree;
	quartResolvedDisagreeDiag = g1->quartResolvedDisagreeDiag + g2->quartResolvedDisagreeDiag;
#endif
#ifdef quartetsNoSwap
	quartResolvedAgreeUpper = g1->quartResolvedAgreeUpper + g2->quartResolvedAgreeUpper;
#ifndef calcE
	quartResolvedDisagreeUpper = g1->quartResolvedDisagreeUpper + g2->quartResolvedDisagreeUpper;
#endif
#endif

#ifdef calcE
	// Initialize sums and stuff for calculating E
	n_circ_square_triangle = 0;
	n_paren_circ_square_triangle = g1->n_paren_circ_square_triangle + g2->n_paren_circ_square_triangle;
	n_0_circ_square = 0;
	n_paren_0_circ_square = g1->n_paren_0_circ_square + g2->n_paren_0_circ_square;
	n_bracket_circ_square_triangle = 0;
	n_bracket_0_circ_square = 0;

	// Fetch sum for calculating E from children
	quartSumE = g1->quartSumE + g2->quartSumE;
#endif
#endif

	while (g1Next != NULL || g2Next != NULL)
	{
		INC_RUNTIME_COUNTER
		if (g2Next == NULL || (g1Next != NULL && g1Next->num < g2Next->num))
		{
			// Operate only on g1
			g1Count = g1Next;
			g2Count = &CountingLinkedList::dummyLL;
			ourCount->num = g1Count->num;

			if (g1Next->type == CountingLinkedList::End) g1Next = NULL;
			else g1Next = g1Next->next;
		}
		else if (g1Next == NULL || (g2Next != NULL && g2Next->num < g1Next->num))
		{
			// Operate only on g2
			g2Count = g2Next;
			g1Count = &CountingLinkedList::dummyLL;
			ourCount->num = g2Count->num;

			if (g2Next->type == CountingLinkedList::End) g2Next = NULL;
			else g2Next = g2Next->next;
		}
		else //g1Count->num == g2Count->num
		{
			g1Count = g1Next;
			g2Count = g2Next;
			ourCount->num = g1Count->num;

			if (g1Next->type == CountingLinkedList::End) g1Next = NULL;
			else g1Next = g1Next->next;
			if (g2Next->type == CountingLinkedList::End) g2Next = NULL;
			else g2Next = g2Next->next;
		}

		// Update counters (triplets)
		ourCount->n_i = g1Count->n_i + g2Count->n_i;

		if (ourCount->num == 0)
		{
			// Go to next one => We're done!
			if (g1Next == NULL && g2Next == NULL)
				ourCount->type = CountingLinkedList::End;
			else
			{
				// Go to next one (there's more!)
				ourCount->type = CountingLinkedList::Regular;
				if (ourCount->next == NULL) ourCount->next = factory->getLL();
				ourCount = ourCount->next;				
				g1Count = g1Next;
				g2Count = g2Next;
			}

			continue;
		}

		ourCount->n_i_circ = g1Count->n_i_circ + g2Count->n_i_circ +
			g1Count->n_i * (g2->n_circ - g2Count->n_i) +
			g2Count->n_i * (g1->n_circ - g1Count->n_i);

		ourCount->n_paren_ii = g1Count->n_paren_ii + g2Count->n_paren_ii;
		//ourCount->n_i_arrow_circ = 0; // undefined actually!

		// Quartets
#ifdef quartetsToo
		// 2nd group in figure 12 (quartets only)
		ourCount->n_0_i = g1Count->n_0_i + g2Count->n_0_i + 
			g1Zero * g2Count->n_i +
			g2Zero * g1Count->n_i;

		ourCount->n_ii = g1Count->n_ii + g2Count->n_ii + 
			g1Count->n_i * g2Count->n_i; // and not the other way around too as we should then halve it :)

		ourCount->n_0_paren_ii = g1Count->n_0_paren_ii + g2Count->n_0_paren_ii + 
			g1Zero * g2Count->n_paren_ii +
			g2Zero * g1Count->n_paren_ii;
		
		ourCount->n_circ_paren_ii = g1Count->n_circ_paren_ii + g2Count->n_circ_paren_ii + 
			(g1->n_circ - g1Count->n_i) * g2Count->n_paren_ii +
			(g2->n_circ - g2Count->n_i) * g1Count->n_paren_ii;

		ourCount->n_i_paren_0_circ = g1Count->n_i_paren_0_circ + g2Count->n_i_paren_0_circ + 
			g1Count->n_i * (g2->n_paren_0_circ - g2Count->n_paren_0_i) +
			g2Count->n_i * (g1->n_paren_0_circ - g1Count->n_paren_0_i);

		ourCount->n_i_paren_circ_circ = g1Count->n_i_paren_circ_circ + g2Count->n_i_paren_circ_circ + 
			g1Count->n_i * (g2->n_paren_circ_circ - g2Count->n_paren_ii) +
			g2Count->n_i * (g1->n_paren_circ_circ - g1Count->n_paren_ii);

		ourCount->n_i_paren_circ_square = g1Count->n_i_paren_circ_square + g2Count->n_i_paren_circ_square + 
			g1Count->n_i * (g2->n_paren_circ_square - g2Count->n_paren_i_circ) +
			g2Count->n_i * (g1->n_paren_circ_square - g1Count->n_paren_i_circ);

		// In the box
		ourCount->n_bracket_i_circ = ourCount->n_i * (n_circ - ourCount->n_i);

		// 3rd group in figure 12 (quartets only)
		ourCount->n_paren_0_i = g1Count->n_paren_0_i + g2Count->n_paren_0_i;
		ourCount->n_paren_i_circ = g1Count->n_paren_i_circ + g2Count->n_paren_i_circ;
		ourCount->n_paren_0_paren_ii = g1Count->n_paren_0_paren_ii + g2Count->n_paren_0_paren_ii;
		ourCount->n_paren_circ_paren_ii = g1Count->n_paren_circ_paren_ii + g2Count->n_paren_circ_paren_ii;
		ourCount->n_paren_i_paren_0_circ = g1Count->n_paren_i_paren_0_circ + g2Count->n_paren_i_paren_0_circ;

		// 4th group in figure 12 (quartets only)
		ourCount->n_bracket_0_paren_ii = g1Count->n_bracket_0_paren_ii + g2Count->n_bracket_0_paren_ii +
			g1Zero * g2Count->n_paren_ii +
			g2Zero * g1Count->n_paren_ii;

		ourCount->n_bracket_circ_paren_ii = g1Count->n_bracket_circ_paren_ii + g2Count->n_bracket_circ_paren_ii +
			(g1->n_circ - g1Count->n_i) * g2Count->n_paren_ii +
			(g2->n_circ - g2Count->n_i) * g1Count->n_paren_ii;

		ourCount->n_bracket_i_paren_0_circ = g1Count->n_bracket_i_paren_0_circ + g2Count->n_bracket_i_paren_0_circ +
			g1Count->n_i * (g2->n_paren_0_circ - g2Count->n_paren_0_i) +
			g2Count->n_i * (g1->n_paren_0_circ - g1Count->n_paren_0_i);

		/*
		// 5th group in figure 12 (quartets only)
		// These are actually undefined!
		ourCount->n_0_arrow_i = 0;
		ourCount->n_i_arrow_0 = 0;
		ourCount->n_i_arrow_i = 0;
		ourCount->n_circ_arrow_i = 0;
		ourCount->n_0_arrow_paren_ii = 0;
		ourCount->n_i_arrow_paren_0_circ = 0;
		ourCount->n_i_arrow_paren_circ_square = 0;
		ourCount->n_circ_arrow_paren_ii = 0;
		ourCount->n_i_arrow_0_circ = 0;
		ourCount->n_i_arrow_circ_circ = 0;
		ourCount->n_i_arrow_circ_square = 0;
		ourCount->n_circ_arrow_ii = 0;
		ourCount->n_paren_ii_arrow_0 = 0;
		ourCount->n_paren_ii_arrow_circ = 0;
		ourCount->n_paren_circ_circ_arrow_i = 0;
		ourCount->n_0_arrow_i_arrow_i = 0;
		ourCount->n_i_arrow_circ_arrow_0 = 0;
		ourCount->n_i_arrow_0_arrow_circ = 0;
		ourCount->n_circ_arrow_i_arrow_i = 0;

		// Added by us (and undefined)
		ourCount->n_i_arrow_paren_circ_circ = 0;
		ourCount->n_0_arrow_ii = 0;
		ourCount->n_paren_0_circ_arrow_i = 0;
		*/

		// Figure 15 counters (part 1-4, all with j) & Figure 16 sums (with j)
		ourCount->resetIterator();
		g1Count->resetIterator();
		g2Count->resetIterator();
		g1->countingVars->resetIterator();
		g2->countingVars->resetIterator();

#ifndef calcE
		ourCount->n_circ_paren_i_square = g1Count->n_circ_paren_i_square + g2Count->n_circ_paren_i_square;
		ourCount->n_circ_paren_i_circ = g1Count->n_circ_paren_i_circ + g2Count->n_circ_paren_i_circ;

#ifdef quartetsNoSwap
		// Added by us for filling out tables
		// B
		ourCount->n_bracket_circ_paren_i_circ = g1Count->n_bracket_circ_paren_i_circ + g2Count->n_bracket_circ_paren_i_circ;
		ourCount->n_bracket_circ_paren_i_square = g1Count->n_bracket_circ_paren_i_square + g2Count->n_bracket_circ_paren_i_square;
#endif
#endif
#ifdef calcE
		// New counters for calculating E (Continues below)
		ourCount->n_i_circ_square = g1Count->n_i_circ_square + g2Count->n_i_circ_square +
			g1Count->n_i * (g2->n_circ_square - g2Count->n_i_circ) +
			g2Count->n_i * (g1->n_circ_square - g1Count->n_i_circ);

		ourCount->n_bracket_i_circ_square = g1Count->n_bracket_i_circ_square + g2Count->n_bracket_i_circ_square +
			g1Count->n_i * (g2->n_circ_square - g2Count->n_i_circ) +
			g2Count->n_i * (g1->n_circ_square - g1Count->n_i_circ);
#endif

		unsigned int wantedMax = degree+1;
		unsigned int lastJPlus1 = 1;
		if (ourCount->n_i_j != NULL) ourCount->n_i_j->type = CountingLinkedListNumOnly::SkipAndEnd;
#ifndef calcE
		if (ourCount->n_paren_i_j != NULL) ourCount->n_paren_i_j->type = CountingLinkedListNumOnly::SkipAndEnd;
#ifdef quartetsNoSwap
		if (ourCount->n_i_paren_i_j != NULL) ourCount->n_i_paren_i_j->type = CountingLinkedListNumOnly::SkipAndEnd;
		if (ourCount->n_paren_i_paren_i_j != NULL) ourCount->n_paren_i_paren_i_j->type = CountingLinkedListNumOnly::SkipAndEnd;
		if (ourCount->n_bracket_i_paren_i_j != NULL) ourCount->n_bracket_i_paren_i_j->type = CountingLinkedListNumOnly::SkipAndEnd;
#endif
#endif
		while (true)
		{
			INC_RUNTIME_COUNTER
			unsigned int j = wantedMax;
			NEXT_LEAST_J(g1Count->n_i_j)					NEXT_LEAST_J(g2Count->n_i_j)
#ifndef calcE
			NEXT_LEAST_J(g1Count->n_paren_i_j)				NEXT_LEAST_J(g2Count->n_paren_i_j)
#ifdef quartetsNoSwap
			NEXT_LEAST_J(g1Count->n_i_paren_i_j)			NEXT_LEAST_J(g2Count->n_i_paren_i_j)
			NEXT_LEAST_J(g1Count->n_paren_i_paren_i_j)		NEXT_LEAST_J(g2Count->n_paren_i_paren_i_j)
			NEXT_LEAST_J(g1Count->n_bracket_i_paren_i_j)	NEXT_LEAST_J(g2Count->n_bracket_i_paren_i_j)
#endif
#endif

			if (gotoIteratorValueForList(g1->countingVars, lastJPlus1) && g1->countingVars->getIteratorNum() < j) j = g1->countingVars->getIteratorNum();
			if (gotoIteratorValueForList(g2->countingVars, lastJPlus1) && g2->countingVars->getIteratorNum() < j) j = g2->countingVars->getIteratorNum();

			if (j >= wantedMax) break;

			// n_i_j
			INTTYPE_REST new_n_i_j = getIteratorValueForNumList(g1Count->n_i_j, j) + getIteratorValueForNumList(g2Count->n_i_j, j) +
				g1Count->n_i * g2->countingVars->getIteratorValue(j)->n_i +
				g2Count->n_i * g1->countingVars->getIteratorValue(j)->n_i;
			addToNumList(ourCount, i_j, j, new_n_i_j);

#ifndef calcE
			// n_paren_i_j
			INTTYPE_REST new_n_paren_i_j = getIteratorValueForNumList(g1Count->n_paren_i_j, j) + getIteratorValueForNumList(g2Count->n_paren_i_j, j);
			addToNumList(ourCount, paren_i_j, j, new_n_paren_i_j);

#ifdef quartetsNoSwap
			// Added by us for filling out tables
			// B
			// n_i_paren_i_j
			INTTYPE_REST new_n_i_paren_i_j = getIteratorValueForNumList(g1Count->n_i_paren_i_j, j) + getIteratorValueForNumList(g2Count->n_i_paren_i_j, j) +
				g1Count->n_i * getIteratorValueForNumList(g2Count->n_paren_i_j, j) +
				g2Count->n_i * getIteratorValueForNumList(g1Count->n_paren_i_j, j);
			addToNumList(ourCount, i_paren_i_j, j, new_n_i_paren_i_j);

			// n_paren_i_paren_i_j
			INTTYPE_REST new_n_paren_i_paren_i_j = getIteratorValueForNumList(g1Count->n_paren_i_paren_i_j, j) + getIteratorValueForNumList(g2Count->n_paren_i_paren_i_j, j) ;
			addToNumList(ourCount, paren_i_paren_i_j, j, new_n_paren_i_paren_i_j);

			// n_bracket_i_paren_i_j
			INTTYPE_REST new_n_bracket_i_paren_i_j = getIteratorValueForNumList(g1Count->n_bracket_i_paren_i_j, j) + getIteratorValueForNumList(g2Count->n_bracket_i_paren_i_j, j) +
				g1Count->n_i * getIteratorValueForNumList(g2Count->n_paren_i_j, j) +
				g2Count->n_i * getIteratorValueForNumList(g1Count->n_paren_i_j, j);
			addToNumList(ourCount, bracket_i_paren_i_j, j, new_n_bracket_i_paren_i_j);
#endif
#endif

			if (ourCount->num != j)
			{
				// i != j
				INTTYPE_REST part1;
				INTTYPE_REST part2;
#ifndef calcE
				// n_circ_paren_i_square
				part1 = (g1->n_circ - g1Count->n_i - g1->countingVars->getIteratorValue(j)->n_i) * getIteratorValueForNumList(g2Count->n_paren_i_j, j);
				part2 = (g2->n_circ - g2Count->n_i - g2->countingVars->getIteratorValue(j)->n_i) * getIteratorValueForNumList(g1Count->n_paren_i_j, j);
				if (part1 < 0 || part2 < 0)
				{
					cout << "WTF #1?!?" << endl;
				}
				else ourCount->n_circ_paren_i_square += part1 + part2;
#ifdef quartetsNoSwap
				// Added by us for filling out tables (B) start
				ourCount->n_bracket_circ_paren_i_square += part1 + part2; // part1+2 from n_circ_paren_i_square :)
				// Added by us for filling out tables (B) end
#endif

				// n_circ_paren_i_circ (fixed from the article!)
				part1 = g1->countingVars->getIteratorValue(j)->n_i * getIteratorValueForNumList(g2Count->n_paren_i_j, j);
				part2 = g2->countingVars->getIteratorValue(j)->n_i * getIteratorValueForNumList(g1Count->n_paren_i_j, j);

				if (part1 < 0 || part2 < 0)
				{
					cout << "WTF #2?!?" << endl;
				}
				else ourCount->n_circ_paren_i_circ += part1 + part2;

#ifdef quartetsNoSwap
				// Added by us for filling out tables (B) start
				ourCount->n_bracket_circ_paren_i_circ += part1 + part2; // part1+2 from n_circ_paren_i_circ :)
				// Added by us for filling out tables (B) end
#endif
#endif
#ifdef calcE
				// New counters for E (Continued from above)
				part1 = getIteratorValueForNumList(g1Count->n_i_j, j) * (g2->n_circ - g2Count->n_i - g2->countingVars->getIteratorValue(j)->n_i);
				part2 = getIteratorValueForNumList(g2Count->n_i_j, j) * (g1->n_circ - g1Count->n_i - g1->countingVars->getIteratorValue(j)->n_i);
				if (part1 < 0 || part2 < 0)
				{
					cout << "WTF #16?!?" << endl;
				}
				else
				{
					ourCount->n_i_circ_square += part1 + part2;
					ourCount->n_bracket_i_circ_square += part1 + part2; // Yes, the same :)
				}
#endif

#ifndef calcE
				// Figure 16: beta & alpha sum
				quartResolvedDisagree += (INTTYPE_N4) getIteratorValueForNumList(g1Count->n_paren_i_j, j) * (g2Count->n_paren_i_circ - getIteratorValueForNumList(g2Count->n_paren_i_j, j));

				// Figure 16: beta & beta sum #1 (i.e. the one with j)
				quartResolvedDisagreeDiag += (INTTYPE_N4) getIteratorValueForNumList(g1Count->n_paren_i_j, j) * (g2Count->n_i_circ - getIteratorValueForNumList(g2Count->n_i_j, j));
				quartResolvedDisagreeDiag += (INTTYPE_N4) getIteratorValueForNumList(g2Count->n_paren_i_j, j) * (g1Count->n_i_circ - getIteratorValueForNumList(g1Count->n_i_j, j));
#endif
			}

			if (ourCount->num < j)
			{
				// j > i

#ifndef calcE
				// Figure 16: alpha & alpha sum
				quartResolvedDisagreeDiag += (INTTYPE_N4) getIteratorValueForNumList(g1Count->n_paren_i_j, j) * getIteratorValueForNumList(g2Count->n_paren_i_j, j);

#ifdef quartetsNoSwap
				//
				// +------------------------------------------+
				// | New sums for calculating B (Added by us) |
				// +------------------------------------------+
				//

				// alpha & beta (continues below)
				quartResolvedDisagreeUpper += (INTTYPE_N4) getIteratorValueForNumList(g1Count->n_paren_i_j, j) * getIteratorValueForNumList(g2Count->n_i_j, j);
				quartResolvedDisagreeUpper += (INTTYPE_N4) getIteratorValueForNumList(g2Count->n_paren_i_j, j) * getIteratorValueForNumList(g1Count->n_i_j, j);
#endif
#endif
#ifdef calcE

				//
				// +------------------------------------------+
				// | New sums for calculating E (Added by us) |
				// +------------------------------------------+
				//

				// delta & delta
				quartSumE += (INTTYPE_N4) getIteratorValueForNumList(g1Count->n_i_j, j) /*ij*/ *
					(g2->n_circ_square - g2Count->n_i_circ - g2->countingVars->getIteratorValue(j)->n_i_circ /*j_circ*/
					 + getIteratorValueForNumList(g2Count->n_i_j, j) /*ij*/);
#endif
			}

			lastJPlus1 = j+1;
		}

#ifndef calcE
		ourCount->n_i_paren_0_i = g1Count->n_i_paren_0_i + g2Count->n_i_paren_0_i +
			g1Count->n_i * g2Count->n_paren_0_i +
			g2Count->n_i * g1Count->n_paren_0_i;

		ourCount->n_circ_paren_0_i = g1Count->n_circ_paren_0_i + g2Count->n_circ_paren_0_i +
			(g1->n_circ - g1Count->n_i) * g2Count->n_paren_0_i +
			(g2->n_circ - g2Count->n_i) * g1Count->n_paren_0_i;

		ourCount->n_0_paren_i_circ = g1Count->n_0_paren_i_circ + g2Count->n_0_paren_i_circ +
			g1Zero * g2Count->n_paren_i_circ +
			g2Zero * g1Count->n_paren_i_circ;

		ourCount->n_i_paren_i_circ = g1Count->n_i_paren_i_circ + g2Count->n_i_paren_i_circ + 
			g1Count->n_i * g2Count->n_paren_i_circ +
			g2Count->n_i * g1Count->n_paren_i_circ;

		// Figure 15 counters (part 2)
		/*
		ourCount->n_paren_i_paren_i_circ = 0;
		*/
		ourCount->n_paren_i_paren_0_i = g1Count->n_paren_i_paren_0_i + g2Count->n_paren_i_paren_0_i;
		ourCount->n_paren_0_paren_i_circ = g1Count->n_paren_0_paren_i_circ + g2Count->n_paren_0_paren_i_circ;
		ourCount->n_paren_circ_paren_0_i = g1Count->n_paren_circ_paren_0_i + g2Count->n_paren_circ_paren_0_i;

		// Figure 15 counters (part 3)
		ourCount->n_bracket_i_paren_i_circ = g1Count->n_bracket_i_paren_i_circ + g2Count->n_bracket_i_paren_i_circ +
			g1Count->n_paren_i_circ * g2Count->n_i +
			g2Count->n_paren_i_circ * g1Count->n_i;

		ourCount->n_bracket_0_paren_i_circ = g1Count->n_bracket_0_paren_i_circ + g2Count->n_bracket_0_paren_i_circ +
			g1Zero * g2Count->n_paren_i_circ +
			g2Zero * g1Count->n_paren_i_circ;

		ourCount->n_bracket_i_paren_0_i = g1Count->n_bracket_i_paren_0_i + g2Count->n_bracket_i_paren_0_i +
			g1Count->n_i * g2Count->n_paren_0_i +
			g2Count->n_i * g1Count->n_paren_0_i;

		ourCount->n_bracket_circ_paren_0_i = g1Count->n_bracket_circ_paren_0_i + g2Count->n_bracket_circ_paren_0_i +
			g1Count->n_paren_0_i * (g2->n_circ - g2Count->n_i) +
			g2Count->n_paren_0_i * (g1->n_circ - g1Count->n_i);

		/*
		// Figure 15 counters (part 4) (Actually undefined)
		ourCount->n_0_arrow_i_circ = 0;
		ourCount->n_circ_arrow_0_i = 0;
		ourCount->n_i_arrow_i_circ = 0;
		ourCount->n_paren_i_circ_arrow_0 = 0;
		ourCount->n_paren_i_circ_arrow_i = 0;
		ourCount->n_paren_0_i_arrow_i = 0;
		ourCount->n_paren_0_i_arrow_circ = 0;
		ourCount->n_0_arrow_paren_i_circ = 0;
		ourCount->n_i_arrow_paren_i_circ = 0;
		ourCount->n_circ_arrow_paren_0_i = 0;
		ourCount->n_i_arrow_paren_0_i = 0;
		ourCount->n_circ_arrow_paren_i_square = 0;
		ourCount->n_circ_arrow_i_arrow_0 = 0;
		ourCount->n_i_arrow_i_arrow_0 = 0;
		ourCount->n_0_arrow_circ_arrow_i = 0;
		ourCount->n_circ_arrow_0_arrow_i = 0;
		ourCount->n_i_arrow_0_arrow_i = 0;
		ourCount->n_0_arrow_i_arrow_circ = 0;
		ourCount->n_i_arrow_i_arrow_circ = 0;

		// Added by us (Actually undefined)
		ourCount->n_circ_arrow_paren_i_circ = 0; 
		ourCount->n_circ_arrow_i_square = 0;
		ourCount->n_circ_arrow_i_circ = 0;
		ourCount->n_i_arrow_0_i = 0;
		ourCount->n_i_arrow_circ_arrow_i = 0;
		*/

		// Figure 15 sums (count up)
		n_bracket_circ_paren_circ_square += ourCount->n_bracket_i_paren_i_circ;
		n_circ_paren_circ_square += ourCount->n_i_paren_i_circ;
		/*
		n_paren_circ_arrow_circ_arrow_square = 0;
		*/
#endif

#ifdef quartetsNoSwap
		// Added by us for filling out tables
		// A
		ourCount->n_paren_i_paren_circ_circ = g1Count->n_paren_i_paren_circ_circ + g2Count->n_paren_i_paren_circ_circ;

		ourCount->n_bracket_i_paren_circ_circ = g1Count->n_bracket_i_paren_circ_circ + g2Count->n_bracket_i_paren_circ_circ + 
			g1Count->n_i * (g2->n_paren_circ_circ - g2Count->n_paren_ii) +
			g2Count->n_i * (g1->n_paren_circ_circ - g1Count->n_paren_ii);

		//if (ourCount->n_j_arrow_i != NULL) ourCount->n_j_arrow_i->type = CountingLinkedListNumOnly::SkipAndEnd;

		ourCount->n_paren_i_paren_circ_square = g1Count->n_paren_i_paren_circ_square + g2Count->n_paren_i_paren_circ_square;

		ourCount->n_bracket_i_paren_circ_square = g1Count->n_bracket_i_paren_circ_square + g2Count->n_bracket_i_paren_circ_square +
			g1Count->n_i * (g2->n_paren_circ_square - g2Count->n_paren_i_circ) +
			g2Count->n_i * (g1->n_paren_circ_square - g1Count->n_paren_i_circ);

		/*
		// Undefined :)
		ourCount->n_i_arrow_circ_arrow_circ = 0;
		ourCount->n_i_arrow_circ_arrow_square = 0;
		ourCount->n_paren_circ_square_arrow_i = 0;
		if (ourCount->n_i_arrow_j != NULL) ourCount->n_i_arrow_j->type = CountingLinkedListNumOnly::SkipAndEnd;
		*/

#ifndef calcE
		// B
		ourCount->n_paren_circ_paren_i_circ = g1Count->n_paren_circ_paren_i_circ + g2Count->n_paren_circ_paren_i_circ;
		ourCount->n_paren_i_paren_i_circ = g1Count->n_paren_i_paren_i_circ + g2Count->n_paren_i_paren_i_circ;
		ourCount->n_paren_circ_paren_i_square = g1Count->n_paren_circ_paren_i_square + g2Count->n_paren_circ_paren_i_square;

		/*
		// Undefined :)
		ourCount->n_circ_arrow_i_arrow_circ = 0;
		ourCount->n_circ_arrow_circ_arrow_i = 0;
		ourCount->n_paren_i_circ_arrow_circ = 0;
		ourCount->n_circ_arrow_i_arrow_square = 0;
		ourCount->n_paren_i_circ_arrow_square = 0;
		ourCount->n_circ_arrow_square_arrow_i = 0;
		*/
#endif
#endif
#ifdef calcE
		// New counters for calculating E
		ourCount->n_paren_i_circ_square = g1Count->n_paren_i_circ_square + g2Count->n_paren_i_circ_square;

		ourCount->n_0_i_circ = g1Count->n_0_i_circ + g2Count->n_0_i_circ + 
			g1Count->n_0_i * (g2->n_circ - g2Count->n_i) + g1Count->n_i * (g2->n_0_circ - g2Count->n_0_i) + g1Count->n_i_circ * g2Zero + 
			g2Count->n_0_i * (g1->n_circ - g1Count->n_i) + g2Count->n_i * (g1->n_0_circ - g1Count->n_0_i) + g2Count->n_i_circ * g1Zero;

		ourCount->n_paren_0_i_circ = g1Count->n_paren_0_i_circ + g2Count->n_paren_0_i_circ;

		ourCount->n_bracket_0_i_circ = g1Count->n_bracket_0_i_circ + g2Count->n_bracket_0_i_circ +
			g1Count->n_0_i * (g2->n_circ - g2Count->n_i) + g1Count->n_i * (g2->n_0_circ - g2Count->n_0_i) + g1Count->n_i_circ * g2Zero + 
			g2Count->n_0_i * (g1->n_circ - g1Count->n_i) + g2Count->n_i * (g1->n_0_circ - g1Count->n_0_i) + g2Count->n_i_circ * g1Zero;
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
		n_bracket_circ_square += ourCount->n_bracket_i_circ;

#ifdef quartetsNoSwap
		// Added by us for filling out tables (initialize)
		n_bracket_circ_circ += Util::binom2(ourCount->n_i);
		n_paren_circ_paren_square_square += ourCount->n_paren_i_paren_circ_circ;
		n_bracket_circ_paren_square_square += ourCount->n_bracket_i_paren_circ_circ;
#endif
#ifdef calcE
		// New sums and stuff for calculating E
		n_circ_square_triangle += ourCount->n_i_circ_square;
		n_0_circ_square += ourCount->n_0_i_circ;
	
		n_bracket_circ_square_triangle += ourCount->n_bracket_i_circ_square;
		n_bracket_0_circ_square += ourCount->n_bracket_0_i_circ;
#endif
#endif

		// Update resolved/resolved and unresolved/unresolved
		// (Figure 10)
		tripResolved += g1Count->n_paren_ii * (g2->n_circ - g2Count->n_i);
		tripResolved += g2Count->n_paren_ii * (g1->n_circ - g1Count->n_i);

		tripUnresolved += g1Count->n_i * (g2->n_circ_square - g2Count->n_i_circ);
		tripUnresolved += g2Count->n_i * (g1->n_circ_square - g1Count->n_i_circ);

		// Quartets
#ifdef quartetsToo
		//quartResolvedAgree & quartResolvedDisagree

		// alpha & alpha
		quartResolvedAgreeDiag += (INTTYPE_N4) g1Count->n_paren_ii * (g2->n_paren_circ_circ - g2Count->n_paren_ii);

		// beta & alpha
		quartResolvedAgree += (INTTYPE_N4) g1Count->n_paren_ii * (g2->n_paren_circ_square - g2Count->n_paren_i_circ);
		quartResolvedAgree += (INTTYPE_N4) g2Count->n_paren_ii * (g1->n_paren_circ_square - g1Count->n_paren_i_circ);
		
		// beta & beta (part 1)
		quartResolvedAgreeDiag += (INTTYPE_N4) g1Count->n_paren_ii * (g2->n_circ_square - g2Count->n_i_circ);
		quartResolvedAgreeDiag += (INTTYPE_N4) g2Count->n_paren_ii * (g1->n_circ_square - g1Count->n_i_circ);
		quartResolvedAgreeDiag += (INTTYPE_N4) g1Count->n_i * (g2->n_square_paren_circ_circ - g2Count->n_i_paren_circ_circ - g2Count->n_circ_paren_ii);
		quartResolvedAgreeDiag += (INTTYPE_N4) g2Count->n_i * (g1->n_square_paren_circ_circ - g1Count->n_i_paren_circ_circ - g1Count->n_circ_paren_ii);
		
		// beta & beta (part 2)
		quartResolvedAgreeDiag += (INTTYPE_N4) g1Count->n_ii * (g2->n_paren_circ_square - g2Count->n_paren_i_circ);
		quartResolvedAgreeDiag += (INTTYPE_N4) g2Count->n_ii * (g1->n_paren_circ_square - g1Count->n_paren_i_circ);
		quartResolvedAgreeDiag += (INTTYPE_N4) g1Count->n_i * g2Count->n_i_paren_circ_square;
		quartResolvedAgreeDiag += (INTTYPE_N4) g2Count->n_i * g1Count->n_i_paren_circ_square;
		
		// gamma & alpha
		quartResolvedAgree += (INTTYPE_N4) g1Count->n_paren_ii * (g2->n_paren_0_circ - g2Count->n_paren_0_i);
		quartResolvedAgree += (INTTYPE_N4) g2Count->n_paren_ii * (g1->n_paren_0_circ - g1Count->n_paren_0_i);
		
		// gamma & beta (part 1) (continued below the loop!)
		quartResolvedAgree += (INTTYPE_N4) g1Count->n_paren_ii * (g2->n_0_circ - g2Count->n_0_i);
		quartResolvedAgree += (INTTYPE_N4) g2Count->n_paren_ii * (g1->n_0_circ - g1Count->n_0_i);
		quartResolvedAgree += (INTTYPE_N4) (g1->n_circ - g1Count->n_i) * g2Count->n_0_paren_ii;
		quartResolvedAgree += (INTTYPE_N4) (g2->n_circ - g2Count->n_i) * g1Count->n_0_paren_ii;
		
		// gamma & beta (part 2)
		quartResolvedAgree += (INTTYPE_N4) g1Count->n_ii * (g2->n_paren_0_circ - g2Count->n_paren_0_i);
		quartResolvedAgree += (INTTYPE_N4) g2Count->n_ii * (g1->n_paren_0_circ - g1Count->n_paren_0_i);
		quartResolvedAgree += (INTTYPE_N4) g1Count->n_i * g2Count->n_i_paren_0_circ;
		quartResolvedAgree += (INTTYPE_N4) g2Count->n_i * g1Count->n_i_paren_0_circ;
		
		// gamma & gamma (part 1)
		quartResolvedAgreeDiag += (INTTYPE_N4) g1Zero * g2Count->n_paren_circ_paren_ii;
		quartResolvedAgreeDiag += (INTTYPE_N4) g2Zero * g1Count->n_paren_circ_paren_ii;
		
		// gamma & gamma (part 2)
		quartResolvedAgreeDiag += (INTTYPE_N4) (g1->n_circ - g1Count->n_i) * g2Count->n_paren_0_paren_ii;
		quartResolvedAgreeDiag += (INTTYPE_N4) (g2->n_circ - g2Count->n_i) * g1Count->n_paren_0_paren_ii;
		
		// gamma & gamma (part 3)
		quartResolvedAgreeDiag += (INTTYPE_N4) g1Count->n_i * g2Count->n_paren_i_paren_0_circ;
		quartResolvedAgreeDiag += (INTTYPE_N4) g2Count->n_i * g1Count->n_paren_i_paren_0_circ;
		
#ifndef calcE
		// Figure 16
		// alpha & alpha
		// has been moved up

		// beta & alpha
		// has been moved up

		// beta & beta
		// has been moved up

		// Still beta & beta ;o)
		quartResolvedDisagreeDiag += (INTTYPE_N4) g1Count->n_circ_paren_i_square * g2Count->n_i;
		quartResolvedDisagreeDiag += (INTTYPE_N4) g2Count->n_circ_paren_i_square * g1Count->n_i;
		quartResolvedDisagreeDiag += (INTTYPE_N4) g1Count->n_i * (g2->n_circ_paren_circ_square - g2Count->n_i_paren_i_circ - g2Count->n_circ_paren_i_circ);
		quartResolvedDisagreeDiag += (INTTYPE_N4) g2Count->n_i * (g1->n_circ_paren_circ_square - g1Count->n_i_paren_i_circ - g1Count->n_circ_paren_i_circ);

		// gamma & alpha
		quartResolvedDisagree += (INTTYPE_N4) g1Count->n_paren_0_i * g2Count->n_paren_i_circ;
		quartResolvedDisagree += (INTTYPE_N4) g2Count->n_paren_0_i * g1Count->n_paren_i_circ;

		// gamma & beta (part 1)
		quartResolvedDisagree += (INTTYPE_N4) g1Count->n_paren_0_i * g2Count->n_i_circ;
		quartResolvedDisagree += (INTTYPE_N4) g2Count->n_paren_0_i * g1Count->n_i_circ;
		quartResolvedDisagree += (INTTYPE_N4) g1Count->n_i_paren_0_i * (g2->n_circ - g2Count->n_i);
		quartResolvedDisagree += (INTTYPE_N4) g2Count->n_i_paren_0_i * (g1->n_circ - g1Count->n_i);
		quartResolvedDisagree += (INTTYPE_N4) g1Count->n_circ_paren_0_i * g2Count->n_i;
		quartResolvedDisagree += (INTTYPE_N4) g2Count->n_circ_paren_0_i * g1Count->n_i;

		// gamma & beta (part 2) (continues after the loop!)
		quartResolvedDisagree += (INTTYPE_N4) g1Count->n_paren_i_circ * g2Count->n_0_i;
		quartResolvedDisagree += (INTTYPE_N4) g2Count->n_paren_i_circ * g1Count->n_0_i;
		quartResolvedDisagree += (INTTYPE_N4) g1Count->n_0_paren_i_circ * g2Count->n_i;
		quartResolvedDisagree += (INTTYPE_N4) g2Count->n_0_paren_i_circ * g1Count->n_i;

		// gamma & gamma (part 1)
		quartResolvedDisagreeDiag += (INTTYPE_N4) (g1->n_circ - g1Count->n_i) * g2Count->n_paren_i_paren_0_i;
		quartResolvedDisagreeDiag += (INTTYPE_N4) (g2->n_circ - g2Count->n_i) * g1Count->n_paren_i_paren_0_i;

		// gamma & gamma (part 2)
		quartResolvedDisagreeDiag += (INTTYPE_N4) g1Count->n_i * g2Count->n_paren_circ_paren_0_i;
		quartResolvedDisagreeDiag += (INTTYPE_N4) g2Count->n_i * g1Count->n_paren_circ_paren_0_i;

		// gamma & gamma (part 4)
		quartResolvedDisagreeDiag += (INTTYPE_N4) g1Count->n_i * g2Count->n_paren_0_paren_i_circ;
		quartResolvedDisagreeDiag += (INTTYPE_N4) g2Count->n_i * g1Count->n_paren_0_paren_i_circ;
#endif

#ifdef quartetsNoSwap
		//
		// +------------------------------------------+
		// | New sums for calculating A (Added by us) |
		// +------------------------------------------+
		//

		// alpha & beta
		quartResolvedAgreeUpper += (INTTYPE_N4) g1Count->n_paren_ii * (g2->n_circ_circ - g2Count->n_ii);
		quartResolvedAgreeUpper += (INTTYPE_N4) g2Count->n_paren_ii * (g1->n_circ_circ - g1Count->n_ii);
		quartResolvedAgreeUpper += (INTTYPE_N4) g1Count->n_i_paren_circ_circ * g2Count->n_i;
		quartResolvedAgreeUpper += (INTTYPE_N4) g2Count->n_i_paren_circ_circ * g1Count->n_i;

		// alpha & gamma
		quartResolvedAgreeUpper += (INTTYPE_N4) g1Count->n_i * g2Count->n_paren_i_paren_circ_circ;
		quartResolvedAgreeUpper += (INTTYPE_N4) g2Count->n_i * g1Count->n_paren_i_paren_circ_circ;

		// beta & gamma (part 1)
		quartResolvedAgreeUpper += (INTTYPE_N4) g1Count->n_i * (g2->n_paren_circ_paren_square_square - g2Count->n_paren_i_paren_circ_circ - g2Count->n_paren_circ_paren_ii);
		quartResolvedAgreeUpper += (INTTYPE_N4) g2Count->n_i * (g1->n_paren_circ_paren_square_square - g1Count->n_paren_i_paren_circ_circ - g1Count->n_paren_circ_paren_ii);

		// beta & gamma (part 2)
		quartResolvedAgreeUpper += (INTTYPE_N4) g1Count->n_i * g2Count->n_paren_i_paren_circ_square;
		quartResolvedAgreeUpper += (INTTYPE_N4) g2Count->n_i * g1Count->n_paren_i_paren_circ_square;

#ifndef calcE
		//
		// +------------------------------------------+
		// | New sums for calculating B (Added by us) |
		// +------------------------------------------+
		//

		// alpha & beta (continued)
		quartResolvedDisagreeUpper += (INTTYPE_N4) g1Count->n_circ_paren_i_circ * g2Count->n_i;
		quartResolvedDisagreeUpper += (INTTYPE_N4) g2Count->n_circ_paren_i_circ * g1Count->n_i;

		// alpha & gamma
		quartResolvedDisagreeUpper += (INTTYPE_N4) g1Count->n_i * g2Count->n_paren_circ_paren_i_circ;
		quartResolvedDisagreeUpper += (INTTYPE_N4) g2Count->n_i * g1Count->n_paren_circ_paren_i_circ;

		// beta & gamma (part 1)
		quartResolvedDisagreeUpper += (INTTYPE_N4) g1Count->n_i * (g2->n_paren_circ_paren_circ_square - g2Count->n_paren_i_paren_i_circ - g2Count->n_paren_circ_paren_i_circ);
		quartResolvedDisagreeUpper += (INTTYPE_N4) g2Count->n_i * (g1->n_paren_circ_paren_circ_square - g1Count->n_paren_i_paren_i_circ - g1Count->n_paren_circ_paren_i_circ);

		// beta & gamma (part 2)
		quartResolvedDisagreeUpper += (INTTYPE_N4) g1Count->n_i * g2Count->n_paren_circ_paren_i_square;
		quartResolvedDisagreeUpper += (INTTYPE_N4) g2Count->n_i * g1Count->n_paren_circ_paren_i_square;
#endif
#endif

#ifdef calcE
		//
		// +------------------------------------------+
		// | New sums for calculating E (Added by us) |
		// +------------------------------------------+
		//

		// delta & delta
		quartSumE += (INTTYPE_N4) g1Count->n_i * (g2->n_circ_square_triangle - g2Count->n_i_circ_square);
		quartSumE += (INTTYPE_N4) g2Count->n_i * (g1->n_circ_square_triangle - g1Count->n_i_circ_square);

		// delta & epsilon
		quartSumE += (INTTYPE_N4) (g1->n_paren_circ_square_triangle - g1Count->n_paren_i_circ_square) * g2Count->n_i;
		quartSumE += (INTTYPE_N4) (g2->n_paren_circ_square_triangle - g2Count->n_paren_i_circ_square) * g1Count->n_i;

		// epsilon & delta (continues below)
		quartSumE += (INTTYPE_N4) g1Count->n_i * (g2->n_0_circ_square - g2Count->n_0_i_circ);
		quartSumE += (INTTYPE_N4) g2Count->n_i * (g1->n_0_circ_square - g1Count->n_0_i_circ);
		
		quartSumE += (INTTYPE_N4) g1Count->n_0_i * (g2->n_circ_square - g2Count->n_i_circ);
		quartSumE += (INTTYPE_N4) g2Count->n_0_i * (g1->n_circ_square - g1Count->n_i_circ);

		// epsilon & epsilon (part 1) (below)

		// epsilon & epsilon (part 2)
		quartSumE += (INTTYPE_N4) (g1->n_paren_0_circ_square - g1Count->n_paren_0_i_circ) * g2Count->n_i;
		quartSumE += (INTTYPE_N4) (g2->n_paren_0_circ_square - g2Count->n_paren_0_i_circ) * g1Count->n_i;
#endif
#endif

		// Go to next on children unless we're done
		if (g1Next == NULL && g2Next == NULL)
			ourCount->type = CountingLinkedList::End;
		else
		{
			// Go to next one (there's more!)
			ourCount->type = CountingLinkedList::Regular;
			if (ourCount->next == NULL) ourCount->next = factory->getLL();
			ourCount = ourCount->next;
		}
	}

#ifdef quartetsToo
	//gamma & beta (part 1) (continued from inside the loop) (figure 13)
	quartResolvedAgree += (INTTYPE_N4) g1Zero * g2->n_square_paren_circ_circ;
	quartResolvedAgree += (INTTYPE_N4) g2Zero * g1->n_square_paren_circ_circ;

#ifndef calcE
	// gamma & beta (part 2) (continued from inside the loop) (figure 16)
	quartResolvedDisagree += (INTTYPE_N4) g1->n_circ_paren_circ_square * g2Zero;
	quartResolvedDisagree += (INTTYPE_N4) g2->n_circ_paren_circ_square * g1Zero;

	// gamma & gamma (part 3) (continued from inside the loop) (figure 16)
	quartResolvedDisagreeDiag += (INTTYPE_N4) g1Zero * g2->n_paren_circ_paren_circ_square;
	quartResolvedDisagreeDiag += (INTTYPE_N4) g2Zero * g1->n_paren_circ_paren_circ_square;
#endif
#endif

	n_circ_square /= 2;

	// Quartets
#ifdef quartetsToo
	n_paren_circ_square /= 2;
	n_bracket_circ_square /= 2;

#ifdef calcE
	// New stuff for E

	// epsilon & delta (continues from inside the loop)
	quartSumE += (INTTYPE_N4) g1Zero * g2->n_circ_square_triangle;
	quartSumE += (INTTYPE_N4) g2Zero * g1->n_circ_square_triangle;

	// epsilon & epsilon (part 1)
	quartSumE += (INTTYPE_N4) g1->n_paren_circ_square_triangle * g2Zero;
	quartSumE += (INTTYPE_N4) g2->n_paren_circ_square_triangle * g1Zero;

	// Div E sum-counters :)
	if (n_circ_square_triangle % 3 != 0)
	{
		cout << "n_circ_square_triangle mod 3 error!" << endl;
	}
	n_circ_square_triangle /= 3;

	if (n_0_circ_square % 2 != 0)
	{
		cout << "n_0_circ_square mod 2 error!" << endl;
	}
	n_0_circ_square /= 2;

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
#endif
#endif
}