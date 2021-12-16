#include <algorithm>
#include "naive_calc.h"

INTTYPE_REST NaiveCalc::calculateTripletDistance(RootedTree *t1, RootedTree *t2)
{
	oh = new OutputHandler(true);
	vector<string> *res1, *res2;

	calcTriplets(t1);
	res1 = oh->extractLinesAndReset();
	calcTriplets(t2);
	res2 = oh->extractLinesAndReset();

	delete oh;
	INTTYPE_REST result = compareVectors(res1, res2);

	delete res1;
	delete res2;

	return result;
}

INTTYPE_N4 NaiveCalc::calculateQuartetDistance(UnrootedTree *t1, UnrootedTree *t2)
{
	oh = new OutputHandler(true);
	vector<string> *res1, *res2;

//	calcQuartets(t1,true,1,NULL,NULL);
	res1 = oh->extractLinesAndReset();
//	calcQuartets(t2,true,  1, NULL,NULL);
	res2 = oh->extractLinesAndReset();

	delete oh;
	INTTYPE_N4 result = compareVectors(res1, res2);

	delete res1;
	delete res2;

	return result;
}

void NaiveCalc::printTriplets(RootedTree *t)
{
	oh = new OutputHandler(false);
	calcTriplets(t);
	delete oh;
}

void NaiveCalc::printQuartets(UnrootedTree *t,int freq, int **allfreq,unordered_map <string, int>& mapping)
{
	oh = new OutputHandler(false);
	calcQuartets(t, false, freq, allfreq, mapping);
	delete oh;
}

INTTYPE_N4 NaiveCalc::compareVectors(vector<string> *resOne, vector<string> *resTwo)
{
	if (resOne->size() != resTwo->size())
	{
		// Not the same output size?!? Can't compare!
		return -1;
	}

	// Compare by sorting both, running through both lists simultaneously
	sort(resOne->begin(), resOne->end());
	sort(resTwo->begin(), resTwo->end());

	unsigned int i = 0;
	unsigned int j = 0;
	INTTYPE_N4 equals = 0;
	while(i < resOne->size() && j < resOne->size())
	{
		string s1 = resOne->at(i);
		string s2 = resTwo->at(j);
		int comp = s1.compare(s2);
		if (comp == 0)
		{
			i++;
			j++;
			equals++;
		}
		else if (comp < 0)
		{
			i++;
		}
		else // comp > 0
		{
			j++;
		}
	}
	return resOne->size() - equals;
}

void NaiveCalc::calcTriplets(RootedTree *t)
{
	string easySorter[3];

	vector<RootedTree*> *list = t->getList();
	for (unsigned int i1 = 0; i1 < list->size()-2; i1++)
	{
		RootedTree *t1 = list->at(i1);
		for (unsigned int i2 = i1+1; i2 < list->size()-1; i2++)
		{
			RootedTree *t2 = list->at(i2);
			for (unsigned int i3 = i2+1; i3 < list->size(); i3++)
			{
				RootedTree *t3 = list->at(i3);

				RootedTree *p1 = t1->getParent();
				RootedTree *p2 = t2->getParent();
				RootedTree *p3 = t3->getParent();

				while ((p1 != p2 && p1 != p3 && p2 != p3) || (p1 == p2 && p1->level < p3->level) || (p1 == p3 && p1->level < p2->level) || (p2 == p3 && p2->level < p1->level))
				{
					if (p1->level == p2->level && p2->level == p3->level)
					{
						if (p1->getParent() != NULL)
							p1 = p1->getParent();
						if (p2->getParent() != NULL)
							p2 = p2->getParent();
						if (p3->getParent() != NULL)
							p3 = p3->getParent();
					}
					else
					{
						unsigned int max = RootedTree::max(p1, p2, p3);
						if (max == p1->level)
							p1 = p1->getParent();
						if (max == p2->level)
							p2 = p2->getParent();
						if (max == p3->level)
							p3 = p3->getParent();
					}
				}

				if (p1 == p2 && p2 == p3)
				{
					easySorter[0] = t1->name;
					easySorter[1] = t2->name;
					easySorter[2] = t3->name;
					sort(easySorter, easySorter+3);
					oh->out << easySorter[0] << " " << easySorter[1] << " " << easySorter[2];
					oh->commit();
				}
				else
				{
					if (p1 == p2)
					{
						easySorter[0] = t3->name;
						easySorter[1] = t1->name;
						easySorter[2] = t2->name;
					}
					else if (p1 == p3)
					{
						easySorter[0] = t2->name;
						easySorter[1] = t1->name;
						easySorter[2] = t3->name;
					}
					else if (p2 == p3)
					{
						easySorter[0] = t1->name;
						easySorter[1] = t2->name;
						easySorter[2] = t3->name;
					}
					else
					{
						cout << "WTF?!?" << endl;
					}
					sort(easySorter+1,easySorter+3);

					oh->out << easySorter[0] << " | " << easySorter[1] << " " << easySorter[2];
					oh->commit();
				}
			}
		}
	}
	delete list;
}

int nChooseK(int n, int k)
{
	if (n < k)
		return 0;
    int res = 1;
 
    // Since C(n, k) = C(n, n-k)
    if (k > n - k)
        k = n - k;
 
    // Calculate value of
    // [n * (n-1) *---* (n-k+1)] / [k * (k-1) *----* 1]
    for (int i = 0; i < k; ++i) {
        res *= (n - i);
        res /= (i + 1);
    }
 
    return res;
}

int findPermIndex(string quartet[4], int n, unordered_map <string, int>& mapping)
{
	int quartet2[4];
	for(int i=0;i<4;i++){
		quartet2[i]=mapping[quartet[i]];
	//	cout<<quartet[i]<<" "<<quartet2[i]<<endl;
	}
	sort(quartet2,quartet2+4);

//	cout<<to_string(quartet[0]))<<to_string(stoi(quartet[1]))<<quartet[2]<<quartet[3]<<n<<" ";
//	cout<<to_string(quartet2[0])<<to_string(quartet2[1])<<quartet2[2]<<quartet2[3]<<" ";
//	cout<<nChooseK(n,4)<<" "<<nChooseK(n - stoi(quartet[0]),4)<<" "<<nChooseK(n-stoi(quartet[1]),3)<<" "<<nChooseK(n-stoi(quartet[2]),2)<<" ";
	return nChooseK(n,4)-(nChooseK(n - quartet2[0],4) + nChooseK(n-quartet2[1],3)+ nChooseK(n-quartet2[2],2) + n - quartet2[3] + 1);

}

int findTopologyIndex(int quartet[4])
{
	sort(quartet,quartet+2);
	sort(quartet+2,quartet+4);
	if (quartet[1] < quartet[2])
		return 0;
	else if (quartet[1] < quartet[3])
		return 1;
	else
		return 2;
	

}


unordered_map <string, int> NaiveCalc::createMapping(vector<UnrootedTree*> *list)
{
	unordered_map <string, int> mapping;
	int i=1;

	for (unsigned int i3 = 0; i3 < list->size(); i3++){
	//	cout<<list->at(i3)->name<<" "<<list->at(i3)->level<<"*";
		mapping[list->at(i3)->name] = i;
		i++;
	}
	return mapping;
}

void NaiveCalc::calcQuartets(UnrootedTree *ut, bool printAnchor, int freq, int **allfreq ,unordered_map <string, int>& mapping)
{
	string easySorter[4];
	RootedTree *t = ut->convertToRootedTree(NULL);

	vector<RootedTree*> *list = t->getList();
//	qi = 0;


	for (unsigned int i1 = 0; i1 < list->size()-3; i1++)
	{
		RootedTree *t1 = list->at(i1);
		for (unsigned int i2 = i1+1; i2 < list->size()-2; i2++)
		{
			RootedTree *t2 = list->at(i2);
			for (unsigned int i3 = i2+1; i3 < list->size()-1; i3++)
			{
				RootedTree *t3 = list->at(i3);
				for(unsigned int i4 = i3+1; i4 < list->size(); i4++)
				{
//					qi++;
					RootedTree *t4 = list->at(i4);

					RootedTree *p1 = t1->getParent();
					RootedTree *p2 = t2->getParent();
					RootedTree *p3 = t3->getParent();
					RootedTree *p4 = t4->getParent();
					while (((p1 != p2 && p1 != p3 && p1 != p4 && p2 != p3 && p2 != p4 && p3 != p4) ||
						 (p1 == p2 && (p1->level < p3->level || p1->level < p4->level)) ||
						 (p1 == p3 && (p1->level < p2->level || p1->level < p4->level)) ||
						 (p1 == p4 && (p1->level < p2->level || p1->level < p3->level)) ||
						 (p2 == p3 && (p2->level < p1->level || p2->level < p4->level)) ||
						 (p2 == p4 && (p2->level < p1->level || p2->level < p3->level)) ||
						 (p3 == p4 && (p3->level < p1->level || p3->level < p2->level))) &&
						 !((p1 == p2 && p3 == p4) || (p1 == p3 && p2 == p4) || (p1 == p4 && p2 == p3))
						 )
					{
						if (p1->level == p2->level && p2->level == p3->level && p3->level == p4->level)
						{
							if (p1->getParent() != NULL)
								p1 = p1->getParent();
							if (p2->getParent() != NULL)
								p2 = p2->getParent();
							if (p3->getParent() != NULL)
								p3 = p3->getParent();
							if (p4->getParent() != NULL)
								p4 = p4->getParent();
						}
						else
						{
							unsigned int max = RootedTree::max(p1, p2, p3, p4);
							if (max == p1->level)
								p1 = p1->getParent();
							if (max == p2->level)
								p2 = p2->getParent();
							if (max == p3->level)
								p3 = p3->getParent();
							if (max == p4->level)
								p4 = p4->getParent();
						}
					}

					if ((p1 == p2 && (p2 == p3 || p2 == p4)) ||
						(p3 == p4 && (p3 == p1 || p3 == p2)))
					{
						easySorter[0] = t1->name;
						easySorter[1] = t2->name;
						easySorter[2] = t3->name;
						easySorter[3] = t4->name;
						sort(easySorter, easySorter+4);
						
						if (printAnchor && (p1 == p2 && (p2 == p3 || p2 == p4)))
						{
							if (p1 == p2 && p2 == p3 && p3 == p4)
								oh->out << p1->name << ", delta: ";
							else
								oh->out << p1->name << ", epsilon: ";
						}
						else if (printAnchor)
						{
							oh->out << p3->name << ", epsilon: ";
						}
						//if (!printAnchor)
						{
							oh->out << easySorter[0] << " " << easySorter[1] << " " << easySorter[2] << " " << easySorter[3];
							oh->commit();
							//cout << easySorter[0] << " " << easySorter[1] << " " << easySorter[2] << " " << easySorter[3] << endl;
						}
					}
					else
					{
						if (p1 == p2 || p3 == p4)
						{
							easySorter[0] = t1->name;
							easySorter[1] = t2->name;
							easySorter[2] = t3->name;
							easySorter[3] = t4->name;
						}
						else if (p1 == p3 || p2 == p4)
						{
							easySorter[0] = t1->name;
							easySorter[1] = t3->name;
							easySorter[2] = t2->name;
							easySorter[3] = t4->name;
						}
						else if (p1 == p4 || p2 == p3)
						{
							easySorter[0] = t1->name;
							easySorter[1] = t4->name;
							easySorter[2] = t2->name;
							easySorter[3] = t3->name;
						}
						else
						{
							cout << "WTF?!?" << endl;
						}

						if (printAnchor)
						{
							bool isAlphaBool = false;
							while ((!((p1 == p2 && (p2 == p3 || p2 == p4)) || (p3 == p4 && (p3 == p1 || p3 == p2)))) ||
								(p1 == p2 && (p2 == p3 || p2 == p4) && (p1->level < p3->level || p1->level < p4->level)) ||
								(p3 == p4 && (p3 == p1 || p3 == p2) && (p3->level < p1->level || p3->level < p2->level))
								)
							{
								isAlphaBool = isAlphaBool || isAlpha(p1, p2, p3, p4);
								if (p1->level == p2->level && p2->level == p3->level && p3->level == p4->level)
								{
									if (p1->getParent() != NULL)
										p1 = p1->getParent();
									if (p2->getParent() != NULL)
										p2 = p2->getParent();
									if (p3->getParent() != NULL)
										p3 = p3->getParent();
									if (p4->getParent() != NULL)
										p4 = p4->getParent();
								}
								else
								{
									unsigned int max = RootedTree::max(p1, p2, p3, p4);
									if (max == p1->level)
										p1 = p1->getParent();
									if (max == p2->level)
										p2 = p2->getParent();
									if (max == p3->level)
										p3 = p3->getParent();
									if (max == p4->level)
										p4 = p4->getParent();
								}
							}

							if (isAlphaBool)
							{
								oh->out << p1->name  << ", alpha: ";
							}
							else if (p1 == p2 && p2 == p3 && p3 == p4)
							{
								oh->out << p1->name  <<", beta: ";
							}
							else
							{
								oh->out << p3->name  <<", gamma: ";
							}
						}

						sort(easySorter,easySorter+2);
						sort(easySorter+2,easySorter+4);
						
						
						string quartet[4];
						if (easySorter[0] < easySorter[2]){
							std::copy(std::begin(easySorter), std::end(easySorter), std::begin(quartet));
					//		oh->out << freq << ": "<< easySorter[0] << " " << easySorter[1] << " | " << easySorter[2] << " " << easySorter[3];
						}
						else{
							quartet[0] = easySorter[2];
							quartet[1] = easySorter[3];
							quartet[2] = easySorter[0];
							quartet[3] = easySorter[1];
					//		oh->out << freq << ": " << easySorter[2] << " " << easySorter[3] << " | " << easySorter[0] << " " << easySorter[1];
						}

						int quartet2[4];
						for(int i=0;i<4;i++){
							quartet2[i]=mapping[quartet[i]];
					//		cout<<quartet[i]<<" "<<quartet2[i]<<endl;
						}
						int top_index = findTopologyIndex(quartet2);

						int permindex = findPermIndex(quartet,list->size(),mapping);

						allfreq[top_index][permindex] += freq;
					//	cout<<allfreq[top_index][permindex]<<" "<<freq<<" "<<top_index<<" "<< permindex<<endl;
						//

					//	oh->commit();

					}
				}
			}
		}
	}

	delete t->factory;
	delete list;
}

bool NaiveCalc::isAlpha(RootedTree *p1, RootedTree *p2, RootedTree *p3, RootedTree *p4)
{
	// Has 3 or more met?
	if ((p1 == p2 && (p2 == p3 || p2 == p4)) ||
		(p3 == p4 && (p3 == p1 || p3 == p2)))
		return false;

	if (!(p1->level == p2->level && p2->level == p3->level && p3->level == p4->level)) return false;

	unsigned int meets = 0;
	if (p1 == p2 || p1 == p3 || p1 == p4) meets++;
	if (p2 == p3 || p2 == p4) meets++;
	if (p3 == p4) meets++;

	return meets == 2;
}
