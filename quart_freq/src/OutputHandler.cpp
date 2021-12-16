#include <iostream>
#include "output_handler.h"

using namespace std;

OutputHandler::OutputHandler(bool save)
{
	lines = NULL;
	if (save) lines = new vector<string>();
	this->save = save;
}

OutputHandler::~OutputHandler()
{
	if (save) delete lines;
}

vector<string>* OutputHandler::extractLinesAndReset()
{
	if (save)
	{
		vector<string>* result = lines;
		lines = new vector<string>();
		return result;
	}
	return NULL;
}

void OutputHandler::commit()
{
	if (!save)
		cout << out.str() << endl;
	else
		lines->push_back(out.str());

	// Clear it!
	out.str(string());
}