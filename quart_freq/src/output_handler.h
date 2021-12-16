#ifndef OUTPUT_HANDLER_H
	#define OUTPUT_HANDLER_H

	#include <sstream>
	#include <string>
	#include <vector>
	#ifdef _DEBUG
		#include "nvwa/debug_new.h"
	#endif

	using namespace std;

	class OutputHandler
	{
		public:
			/*
				If save = true then "the committed lines" will be added to a vector
				if not they will be written with cout
			*/
			OutputHandler(bool save = false);
			~OutputHandler();
			void commit();
			vector<string>* extractLinesAndReset();

			stringstream out;

		private:
			bool save;
			vector<string> *lines;
	};
#endif