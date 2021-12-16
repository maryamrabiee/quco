#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include "fancy_time.h"

using namespace std;

unsigned int ourRound(double d)
{
	unsigned int floored = (unsigned int) floor(d);
	if (d - floored < 0.5) return floored;
	return (unsigned int) ceil(d);
}

vector<int>::iterator left(vector<int>::iterator it, unsigned int *n, unsigned int d1, unsigned int d2)
{
	unsigned int d = d1;
	if (d2 > 0 && d2 > d1) d = rand() % (d2 - d1) + d1;
	
	cout << "(";
	if (*n <= d)
	{
		// Do leafs!
		cout << *it;
		it++;
		(*n)--;

		for(; *n>0; (*n)--, it++)
		{
			cout << "," << *it;
		}
	}
	else
	{
		*n = *n-d+1;
		it = left(it, n, d1, d2);
		for(; d>1; d--, it++)
		{
			cout << "," << *it;
		}
	}

	cout << ")";

	return it;
}

vector<int>::iterator right(vector<int>::iterator it, unsigned int *n, unsigned int d1, unsigned int d2)
{
	unsigned int d = d1;
	if (d2 > 0 && d2 > d1) d = rand() % (d2 - d1) + d1;
	
	cout << "(";
	if (*n <= d)
	{
		// Do leafs!
		(*n)--;
		for(; *n>0; (*n)--, it++)
		{
			cout << *it << ",";
		}
		cout << *it;
		it++;
	}
	else
	{
		*n = *n-d+1;
		for(; d>1; d--, it++)
		{
			cout << *it << ",";
		}
		it = right(it, n, d1, d2);
	}

	cout << ")";

	return it;
}

vector<int>::iterator balanced(vector<int>::iterator it, vector<int>::iterator itD, unsigned int n)
{
	unsigned int d = *itD;
	bool doParens = n > 1;
	if (doParens) cout << "(";
	if (n <= d)
	{
		// Do leafs!
		n--;
		for(; n>0; n--, it++)
		{
			cout << *it << ",";
		}
		cout << *it;
		it++;
	}
	else
	{
		itD++;
		unsigned int totalNums = 0;
		unsigned int lastNums = 0;
		double nOverD = ((double) n) / ((double) d);
		for(unsigned int thisIteration = 1; thisIteration <= d; thisIteration++)
		{
			lastNums = totalNums;
			totalNums = ourRound(nOverD * thisIteration);
			if (totalNums > n) totalNums = n;
			lastNums = totalNums - lastNums;
			if (thisIteration > 1) cout << ",";
			it = balanced(it, itD, lastNums);
		}
	}

	if (doParens) cout << ")";

	return it;
}

vector<int>::iterator notBalanced(vector<int>::iterator it, vector<int>::iterator itD, unsigned int n, float leftBias)
{
	unsigned int d = *itD;
	bool doParens = n > 1;
	if (doParens) cout << "(";
	if (n <= d)
	{
		// Do leafs!
		n--;
		for(; n>0; n--, it++)
		{
			cout << *it << ",";
		}
		cout << *it;
		it++;
	}
	else
	{
		itD++;

		unsigned int firstNums = (unsigned int) (n * leftBias);
		unsigned int totalNums = firstNums;
		unsigned int lastNums;
		double nOverD = ((double) (n - firstNums)) / ((double) (d - 1));

		// First, biased, run
		it = notBalanced(it, itD, firstNums, leftBias);

		// Rest of runs
		for(unsigned int thisIteration = 1; thisIteration < d; thisIteration++)
		{
			lastNums = totalNums;
			totalNums = firstNums + ourRound(nOverD * thisIteration);
			if (totalNums > n) totalNums = n;
			lastNums = totalNums - lastNums;
			if (lastNums == 0)
				continue;
			cout << ",";
			it = notBalanced(it, itD, lastNums, leftBias);
		}
	}

	if (doParens) cout << ")";

	return it;
}

int main(int argc, char** argv)
{
	if (argc < 4)
	{
		cerr << "Syntax: " << argv[0] << " <typeID> <n> <d1>[ <d2>]" << endl;
		cerr << "Where" << endl;
		cerr << "typeID\t[1|2|3|4|5] for left, right, balanced, 75% left biased and 99% left biased" << endl;
		cerr << "n\tthe number of leafs." << endl;
		cerr << "d1 & d2\t<int>, i.e. sets the number of children for an inner node to either d1 or" << endl;
		cerr <<		"\tat random between d1 and d2 (for balanced trees the d will be the same across an entire level)" << endl;
		return -1;
	}

	unsigned int type;
	istringstream(argv[1]) >> type;
	if (type == 0 || type > 5)
	{
		cerr << "Couldn't parse" << argv[1] << " as [1|2|3|4|5]" << endl;
		return -1;
	}
	cerr << "Type: " << type << endl;

	unsigned int n;
	istringstream(argv[2]) >> n;
	cerr << "n: " << n << endl;

	unsigned int d1;
	istringstream(argv[3]) >> d1;
	if (d1 <= 1)
	{
		cerr << "d1 = " << d1 << " which does not compute..." << endl;
		return -1;
	}
	cerr << "d1: " << d1 << endl;

	unsigned int d2 = 0;
	if (argc > 4)
	{
		istringstream(argv[4]) >> d2;
		if (d2 <= d1)
		{
			cerr << "d2 = " << d2 << " (<= d1 = " << d1 << ") which does not compute..." << endl;
			return -1;
		}
		cerr << "d2: " << d2 << endl;
	}

	srand(FancyTime::preciseClock());

	vector<int> data;
	for(unsigned int j = 1; j <= n; j++)
	{
		data.push_back(j);
	}
	random_shuffle(data.begin(), data.end());

	vector<int> dataD;
	// If going to 128, with 75% bias we can go to ~9.000.000.000.000.000 --- with 90% bias we can go to ~700.000
	// Going to 2000, 99% left-biased should be able to go to ~500.000.000
	for(unsigned int j = 1; j <= 2000; j++)
	{
		unsigned int d = d1;
		if (d2 > 0 && d2 > d1) d = rand() % (d2 - d1) + d1;
		dataD.push_back(d);
	}

	switch(type)
	{
		case 1:	left(data.begin(), &n, d1, d2);
				break;
		case 2:	right(data.begin(), &n, d1, d2);
				break;
		case 3:	balanced(data.begin(), dataD.begin(), n);
				break;
		case 4:	notBalanced(data.begin(), dataD.begin(), n, .75f);
				break;
		case 5:	notBalanced(data.begin(), dataD.begin(), n, .99f);
				break;
		default:
			cerr << "We should never get here" << endl;
			break;
	}
	cout << ";";

	return 0;
}
