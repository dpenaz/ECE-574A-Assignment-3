#include <string>
#include <vector>
#include <list>

using namespace std;

class Node
{
	bool doneFlag;			// Lets child node know if node has been completed
	int cycleDelay;
	int startTime;			// When the Operation should occure

	string nodeOp;
	string outPut;
	list<string> inPuts;
	
	vector<Node*> parents;	// Nodes that happen before
	vector<Node*> children;	// Nodes that are dependent on this node

public:
	Node(string Op);
	~Node();

	void setOutPut(string val);
	void addInputs(string val);
	void addParent(Node* p);
	void removeParent(Node* p);
	void addChild(Node* c);
	void setStartTime(int t);
	void complete();

	string getOutPut();
	int getStartTime();
	int getParentsSize();
	string getVerilogCode();
	bool findChild(string var);
	bool chkIfParent(string var);
};