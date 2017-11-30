#include <string>
#include <vector>
#include <queue>
#include "Node.h"

using namespace std;

Node::Node(string Op)
{
	this->nodeOp = Op;

	/*Initialize ASAP & ALAP*/
	this->ASAP_start = 0;	
	this->ALAP_start = 0;

	if (!Op.compare("*"))			// Set Cycle Delay depending on Operation
		this->latency = 2;
	else if (!Op.compare("/") || !Op.compare("%"))
		this->latency = 3;
	else
		this->latency = 1;
}

Node::~Node() {}

int Node::getLatency()
{
	return this->latency;
}

void Node::setOutPut(string val)
{
	this->outPut = val;
}

void Node::addInputs(string val)
{
	this->inPuts.push_back(val);
}

void Node::addParent(Node* p)
{
	this->parents.push_back(p);
}

void Node::removeParent(Node* p)
{
	for (int i = 0; i < this->parents.size(); ++i)
		if (this->parents[i] == p)
		{
			this->parents.erase(parents.begin() + i);
			return;
		}
}

void Node::addChild(Node* c)
{
	this->children.push_back(c);
}

void Node::setStartTime(int t)
{
	this->startTime = t;
}

int Node::getStartTime()
{
	return this->startTime;
}

string Node::getVerilogCode()
{
	// Needs to be implimented
	return this->outPut;
}

string Node::getOutPut()
{
	return this->outPut;
}

bool Node::chkIfParent(string var)
{
	auto it = find(this->inPuts.begin(), this->inPuts.end(), var);
	if (it != this->inPuts.end())
		return true;
	return false;
}

bool Node::findChild(string var)
{
	if (!var.compare(this->outPut))
		return true;
	return false;
}

void Node::setALAP(int t)
{
	this->ALAP_start = t;
}

int Node::getALAP()
{
	return this->ALAP_start;
}

void Node::setFlag(bool v)
{
	this->visitedFlag = v;
}

bool Node::getFlag()
{
	return this->visitedFlag;
}

void Node::setASAP(int v)
{
	this->ASAP_start = v;
}

int Node::getASAP()
{
	return this->ASAP_start;
}