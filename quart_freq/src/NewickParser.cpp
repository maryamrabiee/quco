#include <fstream>
#include "newick_parser.h"

using namespace std;

// UnrootedTree* NewickParser::parseFile2(const char* filename)
// {
// 	// Read file
// 	ifstream infile;

// 	// Open file at the end!
// 	infile.open(filename, ifstream::in | ios::ate);
// 	if (infile && infile.tellg() > 0)
// 	{
// 		// Typecast to int to make the compiler stop bugging us!
// 		int size = (int) infile.tellg();

// 		// Make space for and set null-termination-thingy!
// 		char* data = new char[size+1];
// 		data[size] = 0;

// 		infile.seekg(0, ios::beg);
// 		infile.read(data, size); 
// 		infile.close();

// 		// Parse and return
// 		str = string(data);
// 		UnrootedTree *t = parse();
// 		delete[] data;
// 		return t;
// 	}
// 	else
// 	{
// 		// Couldn't open file!
// 		cerr << "Couldn't open file \"" << filename << "\"!";
// 		parseError = true;
// 		return NULL;
// 	}
// }

vector<pair<UnrootedTree*, int> > NewickParser::parseFile(const char* filename)
{
	// Read file
//	vector<UnrootedTree*> list;
	vector<pair<UnrootedTree*, int> > list;
	ifstream infile;
	// Open file at the end!
	infile.open(filename);
	std::string line;
	int freq;
	std::string tree;

	while (infile>>freq>>tree)
	{
		UnrootedTree *t = parseStr(tree);
		list.push_back(make_pair(t,freq));		

	}
	return list;
}




UnrootedTree* NewickParser::parseStr(string inputStr)
{
	str = inputStr;
	return parse();
}

bool NewickParser::isError()
{
	return parseError;
}

int NewickParser::getPos()
{
	if (it == strEnd)
	{
		cerr << "Parse error! String ended! Continuing anyways..." << endl;
		parseError = true;
		return -1;
	}
	return distance(str.begin(), it);
}

UnrootedTree* NewickParser::parse()
{
	parseError = false;
	it = str.begin();
	strEnd = str.end();

	if (*str.rbegin() != ';') return NULL;
	UnrootedTree *t = parseSubTree();
	parseLength();
	if (it == strEnd)
	{
		cerr << "Parse error! String is finished before ';'... Returning anyways!" << endl;
		parseError = true;
	}
	else
	{
		if (*it != ';')
		{
			cerr << "Parse error! Finished before string finished! (Read '" << *it << "' on pos " << getPos() << ", expecting ';'). Returning anyways" << endl;
			parseError = true;
		}
		it++;
		if (it != strEnd)
		{
			cerr << "Parse error! Finished before string finished! (Read '" << *it << "' on pos " << getPos() << ", expected being done). Returning anyways" << endl;
			parseError = true;
		}
	}
	return t;
}

UnrootedTree* NewickParser::parseSubTree()
{
	if (it == strEnd)
	{
		cerr << "Parse error! String ended! Continuing anyways..." << endl;
		parseError = true;
		return new UnrootedTree();
	}

	if (*it == '(') return parseInternal();
	// TODO: Other possibilities than name?!?
	return new UnrootedTree(parseName());
}

UnrootedTree* NewickParser::parseInternal()
{
	if (it == strEnd)
	{
		cerr << "Parse error! String ended! Continuing anyways..." << endl;
		parseError = true;
		return new UnrootedTree();
	}

	// Remove '(' char, create internal node, and recurse
	if (*it != '(')
	{
		cerr << "Parse error! Expected '(' here (got '" << *it << "' on pos " << getPos() << "). Continuing anyways..." << endl;
		parseError = true;
	}
	it++;
	UnrootedTree *internalNode = new UnrootedTree();
	ParseBranchSet(internalNode);

	if (it == strEnd)
	{
		cerr << "Parse error! String ended! Continuing anyways..." << endl;
		parseError = true;
		return internalNode;
	}

	// Remove ')' char, get name
	if (*it != ')')
	{
		cerr << "Parse error! Expected ')' here (got '" << *it << "' on pos " << getPos() << "). Continuing anyways..." << endl;
		parseError = true;
	}
	it++;
	if (it == strEnd)
	{
		cerr << "Parse error! String is finished... Continuing anyways..." << endl;
		parseError = true;
	}
	internalNode->name = parseName();

	return internalNode;
}

void NewickParser::ParseBranchSet(UnrootedTree *parent)
{
	if (it == strEnd)
	{
		cerr << "Parse error! String ended! Continuing anyways..." << endl;
		parseError = true;
		return;
	}

	// Parse arbritrarily many branches (i.e. subtrees with lengths)
	int degreeHere = 0;
	int largestDegreeBelow = 0;
	while(true)
	{
		degreeHere++;
		UnrootedTree *t = parseSubTree();
		largestDegreeBelow = max(largestDegreeBelow, t->maxDegree);
		parent->addEdgeTo(t);
		parseLength();
		if (it != strEnd && *it == ',')
			it++; // and go again!
		else
			break;
	}
	parent->maxDegree = max(degreeHere, largestDegreeBelow);
}

string NewickParser::parseName()
{
	if (it == strEnd)
	{
		cerr << "Parse error! String ended! Continuing anyways..." << endl;
		parseError = true;
		return "";
	}
	int nameStartPos = getPos();
	int numChars = 0;
	while(true)
	{
		char c = *it;
		if (c != '(' && c != ')' && c != ',' && c != ':' && c != ';')
		{
			it++;
			numChars++;
		}
		else break;
		
		if (it == strEnd)
		{
			cerr << "Parse error! String ended! Continuing anyways..." << endl;
			parseError = true;
			break;
		}
	}
	return str.substr(nameStartPos, numChars);
}

void NewickParser::parseLength()
{
	// Do we start a number?
	if (it == strEnd)
	{
		cerr << "Parse error! String ended! Continuing anyways..." << endl;
		parseError = true;
		return;
	}
	if (*it != ':') return;

	// Go past ':'
	it++;
	while(true)
	{
		char c = *it;

		// TODO: Should actually check that this is a number (i.e. [0-9\.]*)
		if (c != '(' && c != ')' && c != ',' && c != ':' && c != ';')
		{
			it++;
		}
		else break;
		if (it == strEnd)
		{
			cerr << "Parse error! String ended! Continuing anyways..." << endl;
			parseError = true;
			break;
		}
	}
}