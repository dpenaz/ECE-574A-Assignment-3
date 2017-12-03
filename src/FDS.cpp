#include <iostream>
#include <iomanip>
#include "FDS.h"

using namespace std;

void printNodes(vector<Node*> myNodes)
{
	for (unsigned int i = 0; i < myNodes.size(); ++i)
	{
		cout << "Node #" << i << endl;
		cout << "Num of Parents: " << myNodes[i]->parents.size() << endl;
		cout << "Num of Children: " << myNodes[i]->children.size() << endl;
		cout << "Output: " << myNodes[i]->getVerilogCode() << endl;
		cout << "ASAP: " << myNodes[i]->ASAP_start << endl;
		cout << "ALAP: " << myNodes[i]->ALAP_start << endl;
		cout << "Width: " << myNodes[i]->width << endl;
		cout << "Prob Val: " << myNodes[i]->prob_val << endl  << endl;
	}
}

void printDistribution(vector<double> v)
{
	for (unsigned int col = 0; col < v.size(); ++col)
	{
		cout << col << "\t";
		for (unsigned int row = 1; row < v.size(); ++row)
		{
			if (col == 0)
				cout << row << "\t";
			else
				cout << setprecision(2) << v[row] << "\t";
		}
		cout << endl;
	}
	cout << endl << endl;
}

void resetFlag(vector<Node*> myNodes)
{
	for (unsigned int i = 0; i < myNodes.size(); ++i)
	{
		myNodes[i]->visitedFlag = false;
	}
}

void connectNodes(vector<Node*> myNodes)
{
	for (unsigned int i = 0; i < myNodes.size(); ++i)
	{
		for (unsigned int j = i; j < myNodes.size(); ++j)
		{
			if (myNodes[j]->chkIfParent(myNodes[i]->outPut))
			{
				myNodes[j]->addParent(myNodes[i]);
				myNodes[i]->addChild(myNodes[j]);
			}
		}
	}
}

void cal_ALAP(vector<Node*> myNodes, int cycles)
{
	int time, temp;
	bool done = false;
	bool ready;
	while (!done)
	{
		done = true;
		for (unsigned int i = 0; i < myNodes.size(); ++i)
		{
			ready = true;
			if (myNodes[i]->children.size() == 0) // bottom of tree
			{
				time = cycles - myNodes[i]->latency + 1;
				myNodes[i]->ALAP_start = time;
			}	
			else
			{
				time = 100000;
				temp = 0;
				for (unsigned int j = 0; j < myNodes[i]->children.size(); ++j)	//check children to see if scheduled
				{
					if (myNodes[i]->children[j]->ALAP_start == 0)
					{
						done = false;
						ready = false;
						break;
					}
					temp = myNodes[i]->children[j]->ALAP_start - myNodes[i]->latency;
					if (temp < time)		// get Largest wait time from children
						time = temp;
				}
				if (ready)
					myNodes[i]->ALAP_start = time;
			}
		}
	}
}

void cal_ASAP(vector<Node*> myNodes)
{
	int time, temp;
	bool done = false;
	while (!done)
	{
		done = true;
		for (unsigned int i = 0; i < myNodes.size(); ++i)
		{
			if (myNodes[i]->parents.size() == 0) // Top of tree
				myNodes[i]->ASAP_start = 1;
			else
			{
				time = 0;
				temp = 0;
				for (unsigned int j = 0; j < myNodes[i]->parents.size(); ++j)	//check parents to see if scheduled
				{
					if (myNodes[i]->parents[j]->ASAP_start == 0)
					{
						done = false;
						break;
					}
					temp = myNodes[i]->parents[j]->ASAP_start + myNodes[i]->parents[j]->latency;
					if (temp > time)		// get Largest wait time from parents
						time = temp;
				}
				if (done)
					myNodes[i]->ASAP_start = time;
			}
		}
	}
}

void cal_width(vector<Node*> myNodes)
{
	for (unsigned int i = 0; i < myNodes.size(); ++i)
	{
		myNodes[i]->setWidth();
	}
}

void cal_TypeDistribution(vector<Node*> myNodes, vector<double> &mult, vector<double> &add_sub,
	                      vector<double> &logic, vector<double> &div_mod)
{
	for (unsigned int i = 0; i < myNodes.size(); ++i)
	{
		for (int j = myNodes[i]->ASAP_start; j <= myNodes[i]->ALAP_start; ++j)
		{
			if (!myNodes[i]->nodeOp.compare("*"))	// multiplier resource
				mult[j] += myNodes[i]->prob_val;
			else if (!myNodes[i]->nodeOp.compare("+") || !myNodes[i]->nodeOp.compare("-"))	// adder/subtractor resource
				add_sub[j] += myNodes[i]->prob_val;
			else if (!myNodes[i]->nodeOp.compare("/") || !myNodes[i]->nodeOp.compare("%"))	// divider/modulo resource
				div_mod[j] += myNodes[i]->prob_val;
			else	// logic/logical resource
				logic[j] += myNodes[i]->prob_val;
		}
	}
}

void cal_ForceDir(vector<Node*> myNodes, vector<double> &mult, vector<double> &add_sub,
	              vector<double> &logic, vector<double> &div_mod)
{
	for (unsigned int i = 0; i < myNodes.size(); ++i)
	{
		if (!myNodes[i]->nodeOp.compare("*"))	// multiplier resource
		{
			for (int j = myNodes[i]->ASAP_start; j <= myNodes[i]->ALAP_start; ++j)
			{
				
			}
		}
		else if (!myNodes[i]->nodeOp.compare("+") || !myNodes[i]->nodeOp.compare("-"))	// adder/subtractor resource
		{

		}
		else if (!myNodes[i]->nodeOp.compare("/") || !myNodes[i]->nodeOp.compare("%"))	// divider/modulo resource
		{

		}
		else	// logic/logical resource
		{

		}
	}
}
