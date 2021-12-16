#ifndef COUNTING_LINKED_LIST_H
#define COUNTING_LINKED_LIST_H

#include "int_stuff.h"
#include "counting_linked_list_num_only.h"
#include "hdt_factory.h"

class CountingLinkedList
{
	public:
		// For triplets
		INTTYPE_REST n_i, n_i_circ, n_paren_ii, n_i_arrow_circ;

		// For quartets
#ifdef quartetsToo
		INTTYPE_REST n_0_i, n_ii, n_0_paren_ii, n_circ_paren_ii, n_i_paren_0_circ, n_i_paren_circ_circ, n_i_paren_circ_square;
		INTTYPE_REST n_bracket_i_circ;
		INTTYPE_REST n_paren_0_i, n_paren_i_circ, n_paren_0_paren_ii, n_paren_circ_paren_ii, n_paren_i_paren_0_circ;
		INTTYPE_REST n_bracket_0_paren_ii, n_bracket_circ_paren_ii, n_bracket_i_paren_0_circ;
		INTTYPE_REST n_0_arrow_i, n_i_arrow_0, n_i_arrow_i, n_circ_arrow_i, n_0_arrow_paren_ii, n_i_arrow_paren_0_circ,
			n_i_arrow_paren_circ_square, n_circ_arrow_paren_ii, n_i_arrow_0_circ, n_i_arrow_circ_circ,
			n_i_arrow_circ_square, n_circ_arrow_ii, n_paren_ii_arrow_0, n_paren_ii_arrow_circ, n_paren_circ_circ_arrow_i,
			n_0_arrow_i_arrow_i, n_i_arrow_circ_arrow_0, n_i_arrow_0_arrow_circ, n_circ_arrow_i_arrow_i;

		// Added by us
		INTTYPE_REST n_i_arrow_paren_circ_circ, n_0_arrow_ii, n_paren_0_circ_arrow_i;

		// Figure 15 counters (part 1)
		CountingLinkedListNumOnly *n_i_j; // also used for E calculation
#ifndef calcE
		INTTYPE_REST n_i_paren_0_i, n_circ_paren_0_i, n_0_paren_i_circ, n_circ_paren_i_square, n_i_paren_i_circ, n_circ_paren_i_circ;

		// Figure 15 counters (part 2)
		CountingLinkedListNumOnly *n_paren_i_j;
		//INTTYPE_REST n_paren_i_paren_i_circ
		INTTYPE_REST n_paren_i_paren_0_i, n_paren_0_paren_i_circ, n_paren_circ_paren_0_i;

		// Figure 15 counters (part 3)
		INTTYPE_REST n_bracket_i_paren_i_circ, n_bracket_0_paren_i_circ, n_bracket_i_paren_0_i, n_bracket_circ_paren_0_i;

		// Figure 15 counters (part 4)
#endif
		INTTYPE_REST n_0_arrow_i_circ; // also used for E calculation
#ifndef calcE
		INTTYPE_REST n_circ_arrow_0_i, n_i_arrow_i_circ, n_paren_i_circ_arrow_0, n_paren_i_circ_arrow_i,
			n_paren_0_i_arrow_i, n_paren_0_i_arrow_circ, n_0_arrow_paren_i_circ, n_i_arrow_paren_i_circ, n_i_arrow_paren_0_i,
			n_circ_arrow_paren_0_i, n_circ_arrow_paren_i_square, n_circ_arrow_i_arrow_0, n_i_arrow_i_arrow_0, n_0_arrow_circ_arrow_i,
			n_circ_arrow_0_arrow_i, n_i_arrow_0_arrow_i, n_0_arrow_i_arrow_circ, n_i_arrow_i_arrow_circ;

		// Added by us
		INTTYPE_REST n_circ_arrow_paren_i_circ;
		INTTYPE_REST n_circ_arrow_i_square;
		INTTYPE_REST n_circ_arrow_i_circ;
		INTTYPE_REST n_i_arrow_0_i;
		INTTYPE_REST n_i_arrow_circ_arrow_i;
#endif

#ifdef quartetsNoSwap
		// Added by us for filling out tables
		// A
		INTTYPE_REST n_paren_i_paren_circ_circ;
		INTTYPE_REST n_bracket_i_paren_circ_circ;
		CountingLinkedListNumOnly *n_j_arrow_i;
		INTTYPE_REST n_paren_i_paren_circ_square;
		INTTYPE_REST n_bracket_i_paren_circ_square;
		INTTYPE_REST n_i_arrow_circ_arrow_circ;
		INTTYPE_REST n_i_arrow_circ_arrow_square;
		INTTYPE_REST n_paren_circ_square_arrow_i;
		CountingLinkedListNumOnly *n_i_arrow_j;

#ifndef calcE
		// B
		INTTYPE_REST n_circ_arrow_i_arrow_circ;
		INTTYPE_REST n_circ_arrow_circ_arrow_i;
		INTTYPE_REST n_paren_i_circ_arrow_circ;
		INTTYPE_REST n_paren_circ_paren_i_circ;
		INTTYPE_REST n_circ_arrow_i_arrow_square;
		INTTYPE_REST n_paren_i_circ_arrow_square;
		INTTYPE_REST n_paren_i_paren_i_circ;
		INTTYPE_REST n_circ_arrow_square_arrow_i;
		INTTYPE_REST n_paren_circ_paren_i_square;
		CountingLinkedListNumOnly *n_i_paren_i_j;
		CountingLinkedListNumOnly *n_paren_i_paren_i_j;
		CountingLinkedListNumOnly *n_bracket_i_paren_i_j;
		INTTYPE_REST n_bracket_circ_paren_i_circ;
		INTTYPE_REST n_bracket_circ_paren_i_square;
#endif
#endif
#ifdef calcE
		// New counters for calculating E
		INTTYPE_REST n_i_circ_square;
		INTTYPE_REST n_i_circ_arrow_square;
		INTTYPE_REST n_circ_square_arrow_i;
		INTTYPE_REST n_paren_i_circ_square;
		INTTYPE_REST n_0_i_circ;
		INTTYPE_REST n_i_circ_arrow_0;
		INTTYPE_REST n_0_i_arrow_circ;
		INTTYPE_REST n_0_circ_arrow_i;
		INTTYPE_REST n_paren_0_i_circ;
		INTTYPE_REST n_bracket_i_circ_square;
		INTTYPE_REST n_bracket_0_i_circ;
#endif
#endif

		// More general stuff
		unsigned int num;
		enum NodeType {Regular, End, Dummy};
		NodeType type;
		CountingLinkedList *next, *iterator;
		static CountingLinkedList dummyLL;
		bool n_i_j_is_reset;
#ifndef calcE
		bool n_paren_i_j_is_reset;
#endif
#ifdef quartetsNoSwap
		bool n_j_arrow_i_is_reset, n_i_arrow_j_is_reset;
#ifndef calcE
		bool n_i_paren_i_j_is_reset, n_paren_i_paren_i_j_is_reset, n_bracket_i_paren_i_j_is_reset;
#endif
#endif

		CountingLinkedList()
		{
		}

		void initialize()
		{
			next = NULL;
#ifdef quartetsToo
			n_i_j = NULL;
#ifndef calcE
			n_paren_i_j = NULL;
#endif
#ifdef quartetsNoSwap
			n_j_arrow_i = n_i_arrow_j = NULL;
#ifndef calcE
			n_i_paren_i_j = n_paren_i_paren_i_j = n_bracket_i_paren_i_j = NULL;
#endif
#endif
#endif
		}

		CountingLinkedList(bool dummy)
		{
			initialize();

			// Triplets
			n_i = 0;
			n_i_circ = 0;
			n_paren_ii = 0;
			n_i_arrow_circ = 0;

			// Quartets
#ifdef quartetsToo
			n_0_i = 0;
			n_ii = 0;
			n_0_paren_ii = 0;
			n_circ_paren_ii = 0;
			n_i_paren_0_circ = 0;
			n_i_paren_circ_circ = 0;
			n_i_paren_circ_square = 0;

			n_bracket_i_circ = 0;

			n_paren_0_i = 0;
			n_paren_i_circ = 0;
			n_paren_0_paren_ii = 0;
			n_paren_circ_paren_ii = 0;
			n_paren_i_paren_0_circ = 0;

			n_bracket_0_paren_ii = 0;
			n_bracket_circ_paren_ii = 0;
			n_bracket_i_paren_0_circ = 0;

			n_0_arrow_i = 0;
			n_i_arrow_0 = 0;
			n_i_arrow_i = 0;
			n_circ_arrow_i = 0;
			n_0_arrow_paren_ii = 0;
			n_i_arrow_paren_0_circ = 0;
			n_i_arrow_paren_circ_square = 0;
			n_circ_arrow_paren_ii = 0;
			n_i_arrow_0_circ = 0;
			n_i_arrow_circ_circ = 0;
			n_i_arrow_circ_square = 0;
			n_circ_arrow_ii = 0;
			n_paren_ii_arrow_0 = 0;
			n_paren_ii_arrow_circ = 0;
			n_paren_circ_circ_arrow_i = 0;
			n_0_arrow_i_arrow_i = 0;
			n_i_arrow_circ_arrow_0 = 0;
			n_i_arrow_0_arrow_circ = 0;
			n_circ_arrow_i_arrow_i = 0;

			n_i_arrow_paren_circ_circ = 0;
			n_0_arrow_ii = 0;

#ifndef calcE
			// Figure 15 counters
			n_i_paren_0_i = 0;
			n_circ_paren_0_i = 0;
			n_0_paren_i_circ = 0;
			n_circ_paren_i_square = 0;
			n_i_paren_i_circ = 0;
			n_circ_paren_i_circ = 0;

			n_paren_i_paren_0_i = 0;
			n_paren_0_paren_i_circ = 0;
			n_paren_circ_paren_0_i = 0;

			n_bracket_i_paren_i_circ = 0;
			n_bracket_0_paren_i_circ = 0;
			n_bracket_i_paren_0_i = 0;
			n_bracket_circ_paren_0_i = 0;
#endif
			n_0_arrow_i_circ = 0;
#ifndef calcE
			n_circ_arrow_0_i = 0;
			n_i_arrow_i_circ = 0;
			n_paren_i_circ_arrow_0 = 0;
			n_paren_i_circ_arrow_i = 0;
			n_paren_0_i_arrow_i = 0;
			n_paren_0_i_arrow_circ = 0;
			n_0_arrow_paren_i_circ = 0;
			n_i_arrow_paren_i_circ = 0;
			n_i_arrow_paren_0_i = 0;
			n_circ_arrow_paren_0_i = 0;
			n_circ_arrow_paren_i_square = 0;
			n_circ_arrow_i_arrow_0 = 0;
			n_i_arrow_i_arrow_0 = 0;
			n_0_arrow_circ_arrow_i = 0;
			n_circ_arrow_0_arrow_i = 0;
			n_i_arrow_0_arrow_i = 0;
			n_0_arrow_i_arrow_circ = 0;
			n_i_arrow_i_arrow_circ = 0;

			n_circ_arrow_paren_i_circ = 0;
			n_circ_arrow_i_square = 0;
			n_circ_arrow_i_circ = 0;
			n_i_arrow_0_i = 0;
			n_i_arrow_circ_arrow_i = 0;
#endif

#ifdef quartetsNoSwap
			// Added by us for filling out tables
			// A
			n_paren_i_paren_circ_circ = 0;
			n_bracket_i_paren_circ_circ = 0;
			n_paren_i_paren_circ_square = 0;
			n_bracket_i_paren_circ_square = 0;
			n_i_arrow_circ_arrow_circ = 0;
			n_i_arrow_circ_arrow_square = 0;
			n_paren_circ_square_arrow_i = 0;

#ifndef calcE
			// B
			n_circ_arrow_i_arrow_circ = 0;
			n_circ_arrow_circ_arrow_i = 0;
			n_paren_i_circ_arrow_circ = 0;
			n_paren_circ_paren_i_circ = 0;
			n_circ_arrow_i_arrow_square = 0;
			n_paren_i_circ_arrow_square = 0;
			n_paren_i_paren_i_circ = 0;
			n_circ_arrow_square_arrow_i = 0;
			n_paren_circ_paren_i_square = 0;
			n_bracket_circ_paren_i_circ = 0;
			n_bracket_circ_paren_i_square = 0;
#endif
#endif
#ifdef calcE
			// New counters for calculating E
			n_i_circ_square = 0;
			n_i_circ_arrow_square = 0;
			n_circ_square_arrow_i = 0;
			n_paren_i_circ_square = 0;
			n_0_i_circ = 0;
			n_i_circ_arrow_0 = 0;
			n_0_i_arrow_circ = 0;
			n_0_circ_arrow_i = 0;
			n_paren_0_i_circ = 0;
			n_bracket_i_circ_square = 0;
			n_bracket_0_i_circ = 0;
#endif
#endif	
		}

#ifdef quartetsToo
		void resetIterator()
		{
			INC_RUNTIME_COUNTER
			iterator = this;
			if (n_i_j != NULL) n_i_j->resetIterator();
			n_i_j_is_reset = true;
#ifndef calcE
			if (n_paren_i_j != NULL) n_paren_i_j->resetIterator();
			n_paren_i_j_is_reset = true;
#endif
#ifdef quartetsNoSwap
			if (n_j_arrow_i != NULL) n_j_arrow_i->resetIterator();
			n_j_arrow_i_is_reset = true;
			if (n_i_arrow_j != NULL) n_i_arrow_j->resetIterator();
			n_i_arrow_j_is_reset = true;
#ifndef calcE
			if (n_i_paren_i_j != NULL) n_i_paren_i_j->resetIterator();
			n_i_paren_i_j_is_reset = true;
			if (n_paren_i_paren_i_j != NULL) n_paren_i_paren_i_j->resetIterator();
			n_paren_i_paren_i_j_is_reset = true;
			if (n_bracket_i_paren_i_j != NULL) n_bracket_i_paren_i_j->resetIterator();
			n_bracket_i_paren_i_j_is_reset = true;
#endif
#endif
		}

		unsigned int getIteratorNum()
		{
			INC_RUNTIME_COUNTER
			return iterator->num;
		}

		bool iteratorHasEnded()
		{
			INC_RUNTIME_COUNTER
			return iterator == NULL;
		}

		CountingLinkedList* getIteratorValue(unsigned int j)
		{
			INC_RUNTIME_COUNTER
			while (!iteratorHasEnded() && iterator->num < j)
			{
				INC_RUNTIME_COUNTER
				if (iterator->type == End) iterator = NULL;
				else iterator = iterator->next;
			}
			if (iteratorHasEnded() || iterator->num > j) return &dummyLL;
			/*iterator->num == j*/ return iterator;
		}

		CountingLinkedList* getCurrentIteratorValueAndIncrement()
		{
			INC_RUNTIME_COUNTER
			if (iteratorHasEnded()) return NULL;
			CountingLinkedList *result = iterator;
			if (iterator->type == End) iterator = NULL;
			else iterator = iterator->next;
			return result;
		}
#endif
};

#endif