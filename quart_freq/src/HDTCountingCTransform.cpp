#include <cstdlib> // for exit
#include "hdt.h"
#include "rooted_tree.h"
#include "hdt_factory.h"
#include "counting_linked_list.h"
#include "soda13_calc.h"

void HDT::handleCTransform()
{
	INC_RUNTIME_COUNTER
	/*
	  Additionally, these counters are unchanged:
	  n_i (and thus also n_0)
	  n_circ
	  n_bracket_i_circ
	  n_bracket_circ_square
	*/

	// Triplets (sum, n_i_circ has been set to 0, i.e. n_circ_squar is also 0!)
	n_circ_square = 0;

	// Quartets
#ifdef quartetsToo
	// Sums initialized to 0, summed in loop below
	n_paren_circ_circ = 0;
	n_paren_circ_square = 0;
	
	// Not dependent on i, thus placed here. Value is 0 (always! (when transforming C to G))
	n_0_circ = 0;

	// Get the number of leafs with color 0
	// The list is sorted, i.e. if zero's there it's the first one!
	INTTYPE_REST gZero = 0;
	if (countingVars->num == 0) gZero = countingVars->n_i;

	// Not dependent on i (n_circ has been updated in handleC!)
	n_paren_0_circ = gZero * n_circ;

	/*
	// Actually undefined (not dependent on i)
	n_0_arrow_circ = 0;
	n_circ_arrow_0 = 0;
	n_0_arrow_circ_circ = 0;

	// Added by us (and undefined and not dependent on i)
	n_circ_arrow_square_square = 0;
	*/

#ifndef calcE
	// Figure 15 (not dependent on i)
	n_paren_circ_paren_circ_square = n_bracket_circ_paren_circ_square;

	// Figure 15 sums (reset as it sums over stuff that becomes 0 here)
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
	/*
	// Added by us for filling out tables (Actually undefined!)
	n_circ_arrow_circ = 0;
	n_circ_arrow_square = 0;
	n_paren_circ_circ_arrow_square = 0;
	n_circ_arrow_square_arrow_square = 0;
	*/

	// Added by us for filling out tables (initialize)
	n_paren_circ_paren_square_square = 0;
#endif
#ifdef calcE
	// Reset sums and stuff for calculating E
	n_circ_square_triangle = 0;
	n_0_circ_square = 0;
	n_paren_circ_square_triangle = 0;
	n_paren_0_circ_square = 0;
#endif
#endif

	CountingLinkedList *current = countingVars;
	if (current->num == 0)
	{
		if (current->type == CountingLinkedList::End) current = NULL;
		else current = current->next; // don't do weird stuff for the n_0 case...
	}

	while(current != NULL)
	{
		INC_RUNTIME_COUNTER
		//current->n_i is unchanged by this transform!
		// Triplets
		current->n_i_circ = 0;
		current->n_paren_ii = Util::binom2(current->n_i);
		//current->n_i_arrow_circ = 0; // undefined actually!

		// Quartets
#ifdef quartetsToo
		// 2nd group in figure 12 (quartets only)
		current->n_0_i = 0;
		current->n_ii = 0;
		current->n_0_paren_ii = 0;
		current->n_circ_paren_ii = 0;
		current->n_i_paren_0_circ = 0;
		current->n_i_paren_circ_circ = 0;
		current->n_i_paren_circ_square = 0;

		// 3nd group in figure 12 (quartets only)
		current->n_paren_0_i = gZero * current->n_i;
		current->n_paren_i_circ = current->n_bracket_i_circ; // Wee, we could spare a calculation here =)
		current->n_paren_0_paren_ii = current->n_bracket_0_paren_ii;
		current->n_paren_circ_paren_ii = current->n_bracket_circ_paren_ii;
		current->n_paren_i_paren_0_circ = current->n_bracket_i_paren_0_circ;

		// 4th group in figure 12 hasn't changed!

		/*
		// 5th group in figure 12 (quartets only) (Actually undefined!)
		current->n_0_arrow_i = 0;
		current->n_i_arrow_0 = 0;
		current->n_i_arrow_i = 0;
		current->n_circ_arrow_i = 0;
		current->n_0_arrow_paren_ii = 0;
		current->n_i_arrow_paren_0_circ = 0;
		current->n_i_arrow_paren_circ_square = 0;
		current->n_circ_arrow_paren_ii = 0;
		current->n_i_arrow_0_circ = 0;
		current->n_i_arrow_circ_circ = 0;
		current->n_i_arrow_circ_square = 0;
		current->n_circ_arrow_ii = 0;
		current->n_paren_ii_arrow_0 = 0;
		current->n_paren_ii_arrow_circ = 0;
		current->n_paren_circ_circ_arrow_i = 0;
		current->n_0_arrow_i_arrow_i = 0;
		current->n_i_arrow_circ_arrow_0 = 0;
		current->n_i_arrow_0_arrow_circ = 0;
		current->n_circ_arrow_i_arrow_i = 0;

		// Made by us
		current->n_i_arrow_paren_circ_circ = 0;
		current->n_0_arrow_ii = 0;
		current->n_paren_0_circ_arrow_i = 0;
		*/

		// Count up the sums
		n_paren_circ_circ += current->n_paren_ii;
		n_paren_circ_square += current->n_paren_i_circ;

		// Figure 15 counters (part 1)
		if (current->n_i_j != NULL) current->n_i_j->type = CountingLinkedListNumOnly::SkipAndEnd;
#ifndef calcE

		current->n_i_paren_0_i = 0;
		current->n_circ_paren_0_i = 0;
		current->n_0_paren_i_circ = 0;
		current->n_circ_paren_i_square = 0;
		current->n_i_paren_i_circ = 0;
		current->n_circ_paren_i_circ = 0;

		// Figure 15 counters (part 2)
		current->resetIterator();
		countingVars->resetIterator();
		unsigned int wantedMax = degree+1;
		unsigned int lastJPlus1 = 1;
		if (current->n_paren_i_j != NULL) current->n_paren_i_j->type = CountingLinkedListNumOnly::SkipAndEnd;
#ifdef quartetsNoSwap
		if (current->n_paren_i_paren_i_j != NULL) current->n_paren_i_paren_i_j->type = CountingLinkedListNumOnly::SkipAndEnd;
#endif
		while (true)
		{
			INC_RUNTIME_COUNTER
			unsigned int j = wantedMax;
#ifdef quartetsNoSwap
			NEXT_LEAST_J(current->n_bracket_i_paren_i_j)
#endif

			if (gotoIteratorValueForList(countingVars, lastJPlus1) && countingVars->getIteratorNum() < j) j = countingVars->getIteratorNum();

			if (j >= wantedMax) break;

			// n_paren_i_j
			addToNumList(current, paren_i_j, j, current->n_i /*i*/ * countingVars->getIteratorValue(j)->n_i /*j*/);

#ifdef quartetsNoSwap
			// n_paren_i_paren_i_j
			INTTYPE_REST new_n_paren_i_paren_i_j = getIteratorValueForNumList(current->n_bracket_i_paren_i_j, j);
			addToNumList(current, paren_i_paren_i_j, j, new_n_paren_i_paren_i_j);
#endif

			lastJPlus1 = j+1;
		}

		/*
		current->n_paren_i_paren_i_circ = 0;
		*/
		current->n_paren_i_paren_0_i = current->n_bracket_i_paren_0_i;
		current->n_paren_0_paren_i_circ = current->n_bracket_0_paren_i_circ;
		current->n_paren_circ_paren_0_i = current->n_bracket_circ_paren_0_i;

		// Figure 15 counters (part 3) are all unchanged!

		/*
		// Figure 15 counters (part 4) (Actually undefined)
		current->n_0_arrow_i_circ = 0;
		current->n_circ_arrow_0_i = 0;
		current->n_i_arrow_i_circ = 0;
		current->n_paren_i_circ_arrow_0 = 0;
		current->n_paren_i_circ_arrow_i = 0;
		current->n_paren_0_i_arrow_i = 0;
		current->n_paren_0_i_arrow_circ = 0;
		current->n_0_arrow_paren_i_circ = 0;
		current->n_i_arrow_paren_i_circ = 0;
		current->n_circ_arrow_paren_0_i = 0;
		current->n_i_arrow_paren_0_i = 0;
		current->n_circ_arrow_paren_i_square = 0;
		current->n_circ_arrow_i_arrow_0 = 0;
		current->n_i_arrow_i_arrow_0 = 0;
		current->n_0_arrow_circ_arrow_i = 0;
		current->n_circ_arrow_0_arrow_i = 0;
		current->n_i_arrow_0_arrow_i = 0;
		current->n_0_arrow_i_arrow_circ = 0;
		current->n_i_arrow_i_arrow_circ = 0;

		// Added by us (actually undefined)
		current->n_circ_arrow_paren_i_circ = 0;
		current->n_circ_arrow_i_square = 0;
		current->n_circ_arrow_i_circ = 0;
		current->n_i_arrow_0_i = 0;
		current->n_i_arrow_circ_arrow_i = 0;
		*/

		/*
		// Figure 15 sums (count up)
		n_paren_circ_arrow_circ_arrow_square = 0;
		*/
#endif

#ifdef quartetsNoSwap
		// Added by us for filling out tables
		// A
		current->n_paren_i_paren_circ_circ = current->n_bracket_i_paren_circ_circ;
		//if (current->n_j_arrow_i != NULL) current->n_j_arrow_i->type = CountingLinkedListNumOnly::SkipAndEnd;
		current->n_paren_i_paren_circ_square = current->n_bracket_i_paren_circ_square;
		//current->n_i_arrow_circ_arrow_circ = 0;
		//current->n_i_arrow_circ_arrow_square = 0;
		//current->n_paren_circ_square_arrow_i = 0;
		//if (current->n_i_arrow_j != NULL) current->n_i_arrow_j->type = CountingLinkedListNumOnly::SkipAndEnd;

#ifndef calcE
		// B
		// Actually undefined
		/*
		current->n_circ_arrow_i_arrow_circ = 0;
		current->n_circ_arrow_circ_arrow_i = 0;
		current->n_paren_i_circ_arrow_circ = 0;
		current->n_circ_arrow_i_arrow_square = 0;
		current->n_paren_i_circ_arrow_square = 0;
		current->n_circ_arrow_square_arrow_i = 0;
		if (current->n_i_paren_i_j != NULL) current->n_i_paren_i_j->type = CountingLinkedListNumOnly::SkipAndEnd;
		*/

		// Defined
		current->n_paren_circ_paren_i_circ = current->n_bracket_circ_paren_i_circ;
		current->n_paren_i_paren_i_circ = current->n_bracket_i_paren_i_circ;
		current->n_paren_circ_paren_i_square = current->n_bracket_circ_paren_i_square;
#endif

		// Added by us for filling out tables (sum)
		// A
		n_paren_circ_paren_square_square += current->n_paren_i_paren_circ_circ;
#endif
#ifdef calcE
		// New counters for calculating E
		current->n_i_circ_square = 0;
		current->n_0_i_circ = 0;
		current->n_paren_i_circ_square = current->n_bracket_i_circ_square;
		current->n_paren_0_i_circ = current->n_bracket_0_i_circ;
		
		// New sums for E
		n_paren_circ_square_triangle += current->n_paren_i_circ_square;
		n_paren_0_circ_square += current->n_paren_0_i_circ;
#endif
#endif

		// Go to next on children unless we're done
		if (current->type != CountingLinkedList::End)
		{
			// Go to next one (there's more!)
			current = current->next;
		}
		else current = NULL;
	}

#ifdef quartetsToo
	// These are all based on stuff that is reset to 0
	n_circ_circ = 0;
	n_square_paren_circ_circ = 0;
	//n_circ_arrow_paren_square_square = 0; // Not actually defined

	// Halve it :)
	n_paren_circ_square /= 2;

#ifdef calcE
	// Take a third of some of the E sums
	if (n_paren_circ_square_triangle % 3 != 0)
	{
		cout << "n_paren_circ_square_triangle mod 3 test... FAIL!!!" << endl;
	}
	n_paren_circ_square_triangle /= 3;

	// Halve some other ones :)
	if (n_paren_0_circ_square % 2 != 0)
	{
		cout << "n_paren_0_circ_square mod 2 test... FAIL!!!" << endl;
	}
	n_paren_0_circ_square /= 2;
#endif
#endif
}

