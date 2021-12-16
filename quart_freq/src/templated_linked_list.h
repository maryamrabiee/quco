#ifndef TEMPLATED_LINKED_LIST
#define TEMPLATED_LINKED_LIST

#include <iostream>
#ifdef _DEBUG
	#include "nvwa/debug_new.h"
#endif

using namespace std;

template <class dataType>
class TemplatedLinkedList
{
	public:
		dataType data;
		TemplatedLinkedList<dataType> *next;

		inline void initialize()
		{
			next = NULL;
		}
};

#endif