#include <iostream>
#include "FDS.h"

using namespace std;

void printNodes(vector<Node*> myNodes)
{
	for (int i = 0; i < myNodes.size(); ++i)
	{
		cout << "Node #" << i << endl;
		cout << "Num of Parents: " << myNodes[i]->parents.size() << endl;
		cout << "Output: " << myNodes[i]->getVerilogCode() << endl;
		cout << "ASAP: " << myNodes[i]->getASAP() << endl;
		cout << "ALAP: " << myNodes[i]->getALAP() << endl << endl;
	}
}

void resetFlag(vector<Node*> myNodes)
{
	for (int i = 0; i < myNodes.size(); ++i)
	{
		myNodes[i]->setFlag(false);
	}
}

void connectNodes(vector<Node*> myNodes)
{
	for (int i = 0; i < myNodes.size(); ++i)
	{
		for (int j = i; j < myNodes.size(); ++j)
		{
			if (myNodes[j]->chkIfParent(myNodes[i]->getOutPut()))
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
	while (!done)
	{
		done = true;
		for (int i = 0; i < myNodes.size(); ++i)
		{
			if (myNodes[i]->children.size() == 0) // bottom of tree
			{
				time = cycles - myNodes[i]->getLatency() + 1;
				myNodes[i]->setALAP(time);
			}	
			else
			{
				time = 0;
				temp = 0;
				for (int j = 0; j < myNodes[i]->children.size(); ++j)	//check children to see if scheduled
				{
					if (myNodes[i]->children[j]->getALAP() == 0)
					{
						done = false;
						break;
					}
					temp = myNodes[i]->children[j]->getALAP() - myNodes[i]->children[j]->getLatency();
					if (temp < time)		// get Largest wait time from children
						time = temp;
				}
				if (done)
					myNodes[i]->setALAP(time);
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
		for (int i = 0; i < myNodes.size(); ++i)
		{
			if (myNodes[i]->parents.size() == 0) // Top of tree
				myNodes[i]->setASAP(1);
			else
			{
				time = 0;
				temp = 0;
				for (int j = 0; j < myNodes[i]->parents.size(); ++j)	//check parents to see if scheduled
				{
					if (myNodes[i]->parents[j]->getASAP() == 0)
					{
						done = false;
						break;
					}
					temp = myNodes[i]->parents[j]->getASAP() + myNodes[i]->parents[j]->getLatency();
					if (temp > time)		// get Largest wait time from parents
						time = temp;
				}
				if (done)
					myNodes[i]->setASAP(time);
			}
		}
	}
}

void cal_OpProbability(vector<Node*> myNodes)
{

}

void cal_TypeDistribution(vector<Node*> myNodes)
{

}