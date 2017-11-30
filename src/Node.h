#include <string>
#include <vector>
#include <list>

using namespace std;

class Node
{
	bool visitedFlag;		// Lets child node know if node has been completed
	int latency;			// Cycles to execute
	int startTime;			// When the Operation should occure

	int ALAP_start;
	int ASAP_start;

	string nodeOp;
	string outPut;
	list<string> inPuts;
	
public:
	vector<Node*> parents;	// Nodes that happen before
	vector<Node*> children;	// Nodes that are dependent on this node

	Node(string Op);
	~Node();

	void setASAP(int t);
	void setALAP(int t);
	void setOutPut(string val);
	void addInputs(string val);
	void addParent(Node* p);
	void removeParent(Node* p);
	void addChild(Node* c);
	void setStartTime(int t);
	void setFlag(bool v);

	int getLatency();
	int getALAP();
	int getASAP();
	bool getFlag();
	string getOutPut();
	int getStartTime();
	string getVerilogCode();
	bool findChild(string var);
	bool chkIfParent(string var);
};