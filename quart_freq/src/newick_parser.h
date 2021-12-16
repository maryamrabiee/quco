#ifndef NEWICK_PARSER_H
	#define NEWICK_PARSER_H

	#include <string>
	#include "unrooted_tree.h"
	#ifdef _DEBUG
		#include "nvwa/debug_new.h"
	#endif

	using namespace std;

	class NewickParser
	{
		public:
			~NewickParser() {};
			UnrootedTree *parseStr(string inputStr);
			vector<std::pair<UnrootedTree*, int> >  parseFile(const char* filename);
			bool isError();

		private:
			UnrootedTree *parseSubTree();
			UnrootedTree *parseInternal();
			void ParseBranchSet(UnrootedTree *parent);
			string parseName();
			void parseLength();
			UnrootedTree *parse();
			int getPos();

			string str;
			string::iterator it;
			string::iterator strEnd;
			bool parseError;
	};
#endif