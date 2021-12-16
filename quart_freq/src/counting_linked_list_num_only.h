#ifndef COUNTING_LINKED_LIST_NUM_ONLY_H
#define COUNTING_LINKED_LIST_NUM_ONLY_H

#include "int_stuff.h"
#include "soda13_calc.h"

class CountingLinkedListNumOnly
{
	public:
		INTTYPE_REST value;
		unsigned int num;
		enum NodeType {Regular, End, SkipAndEnd};
		NodeType type;
		CountingLinkedListNumOnly *next, *iterator;

		CountingLinkedListNumOnly()
		{
			INC_RUNTIME_COUNTER
		}

		void initialize()
		{
			next = NULL;
		}

		void resetIterator()
		{
			INC_RUNTIME_COUNTER
			iterator = this;
		}

		unsigned int getIteratorNum()
		{
			INC_RUNTIME_COUNTER
			return iterator->num;
		}

		bool iteratorHasEnded()
		{
			INC_RUNTIME_COUNTER
			return iterator == NULL || iterator->type == SkipAndEnd;
		}

		INTTYPE_REST getIteratorValue(unsigned int j)
		{
			INC_RUNTIME_COUNTER
			while (!iteratorHasEnded() && iterator->num < j)
			{
				INC_RUNTIME_COUNTER
				if (iterator->type == End) iterator = NULL;
				else iterator = iterator->next;
			}
			if (iteratorHasEnded() || iterator->num > j) return 0;
			/*iterator->num == j*/ return iterator->value;
		}
};

#endif