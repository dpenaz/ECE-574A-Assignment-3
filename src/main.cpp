#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <tuple>
#include "FDS.h"
#include "Verilog_Imp.h"
#include "hlsyn.h"

using namespace std;

int main(int argc, char *argv[])
{
	const char *arg1, *arg2, *arg3;
	if (argc == 1) {
		arg1 = "C:\\Users\\Devan\\Documents\\578 - Computer-Aided Logic Design\\assignment3_testfiles\\standard tests\\hls_test2.c";
		argc = 2;
	}
	else
		arg1 = argv[1];
	if (argc == 2) {
		arg2 = "5";
		argc = 3;
	}
	else
		arg2 = argv[2];
	if (argc == 3) { //DEBUG/DEV CODE
		arg3 = "C:\\Users\\Devan\\Documents\\578 - Computer-Aided Logic Design\\assignment3_testfiles\\out.v";
		argc = 4;
	}
	else
		arg3 = argv[3];

	if (argc != 4) {
		cerr << "Usage: " << argv[0] << " cFile latency verilogFile" << endl;
		return 1;
	}

	stringstream ss{ arg2 };
	int latency;	// in Cycles
	if (!(ss >> latency)) {
		cerr << "Usage: " << argv[0] << " cFile latency verilogFile" << endl;
		cerr << "latency argument invalid" << endl;
		return 1;
	}

	if (latency <= 0) {
		cerr << "latency should be greater than 0";
		return 1;
	}

	ifstream infile(arg1);
	if (infile.fail()) {
		cerr << "Input file " << arg1 << " failed to open" << endl;
		return 1;
	}

	ofstream finalOutFile;
	finalOutFile.open(arg3);
	if (finalOutFile.fail()) {
		cerr << "Output file " << arg3 << " failed to open" << endl;
		return 1;
	}

	string token;
	ostringstream oss;
	
	vector<Node*> myNodes;

	vector<double> multDist;
	vector<double> add_subDist;
	vector<double> logicDist;
	vector<double> div_modDist;

	// Initialize Vector to size of latency
	for (int i = 0; i <= latency; ++i)
	{
		multDist.push_back(0);
		add_subDist.push_back(0);
		logicDist.push_back(0);
		div_modDist.push_back(0);
	}

	map<string, vector<string> > var_map;	// Store variable info ex: variables[name][type, input/output/wire]
	vector<string> storedTokens;

	for (string line; getline(infile, line);)	// Pass through all lines of code
	{
		Node* pNode;
		istringstream iss{line};

		bool eqFound = false, otherFound = false;;
		while (iss >> token)	// Pass through each token in line
		{
			
			if (!token.compare("+")) {	// ADD and INC
				otherFound = true;
				pNode = assign_op_result("+", line, var_map);
				if (pNode == NULL)
					return 1;
				myNodes.push_back(pNode);
				break;
			}
			else if (!token.compare("=")) {	// REG
				eqFound = true;
			}
			else if (!token.compare("-")) {	// SUB and DEC
				otherFound = true;
				pNode = assign_op_result("-", line, var_map);
				if (pNode == NULL)
					return 1;
				myNodes.push_back(pNode);
				break;
			}
			else if (!token.compare("*")) {	// MUL
				otherFound = true;
				pNode = assign_op_result("*", line, var_map);
				if (pNode == NULL)
					return 1;
				myNodes.push_back(pNode);
				break;
			}
			else if (!token.compare(">")) {	// COMP (gt output)
				otherFound = true;
				pNode = assign_op_result(">", line, var_map);
				if (pNode == NULL)
					return 1;
				myNodes.push_back(pNode);
				break;
			}
			else if (!token.compare("<")) {	// COMP (lt output)
				otherFound = true;
				pNode = assign_op_result("<", line, var_map);
				if (pNode == NULL)
					return 1;
				myNodes.push_back(pNode);
				break;
			}
			else if (!token.compare("==")) {// COMP (eq output)
				otherFound = true;
				pNode = assign_op_result("==", line, var_map);
				if (pNode == NULL)
					return 1;
				myNodes.push_back(pNode);
				break;
			}
			else if (!token.compare("?") || !token.compare(":")) {	// MUX2x1
				otherFound = true;
				pNode = MUX2x1_(line, var_map);
				if(pNode == NULL)
					return 1;
				myNodes.push_back(pNode);
				break;
			}
			else if (!token.compare(">>")) { // SHR
				otherFound = true;
				pNode = assign_op_result(">>", line, var_map);
				if (pNode == NULL)
					return 1;
				myNodes.push_back(pNode);
				break;
			}
			else if (!token.compare("<<")) { // SHL
				otherFound = true;
				pNode = assign_op_result("<<", line, var_map);
				if (pNode == NULL)
					return 1;
				myNodes.push_back(pNode);
				break;
			}
			else if (!token.compare("/")) {	// DIV
				otherFound = true;
				pNode = assign_op_result("/", line, var_map);
				if (pNode == NULL)
					return 1;
				myNodes.push_back(pNode);
				break;
			}
			else if (!token.compare("%")) {	// MOD
				otherFound = true;
				pNode = assign_op_result("%", line, var_map);
				if (pNode == NULL)
					return 1;
				myNodes.push_back(pNode);
				break;
			}
			else if (!token.compare("//")) { // COMMENT!!!
				otherFound = true;
				break;
			}
			else if (!token.compare("input")) {	// input variables
				otherFound = true;
				int errorFlag = grabVariables(line, var_map);
				if (errorFlag)
					return errorFlag;
				break;	// finished grabbing line continue to next 
			}
			else if (!token.compare("output")) { // output variables
				otherFound = true;
				int errorFlag = grabVariables(line, var_map);
				if (errorFlag)
					return errorFlag;
				break;	// finished grabbing line continue to next line
			}
			else if (!token.compare("variable")) { // register variables
				otherFound = true;
				int errorFlag = grabVariables(line, var_map);
				if (errorFlag)
					return errorFlag;
				break;	// finished grabbing line continue to next line
			}
			else {
				if (!checkKey(token, var_map))
				{
					cerr << "token " << "'" << token << "' does not represent an input, output or variable" << endl;
					cerr << errorMsg[1] << endl;
					oss << errorMsg[1] << endl;
					return 1;
				}
				storedTokens.push_back(token);
			}
		}
		if (eqFound && !otherFound) {
			pNode = REG_(line, var_map);
			if (pNode == NULL)
				return 1;
			myNodes.push_back(pNode);
		}
	}

	// Begin calculating steps for Forced-Directed Scheduling (FDS)

	connectNodes(myNodes);
	cal_ALAP(myNodes, latency);
	cal_ASAP(myNodes);
	cal_width(myNodes);
	cal_TypeDistribution(myNodes, multDist, add_subDist, logicDist, div_modDist);
	cal_ForceDir(myNodes, multDist, add_subDist, logicDist, div_modDist);  // NOT FULLY IMPLEMENTED...LEFT OFF HERE!!!!!!!!

	printNodes(myNodes);

	printDistribution(multDist);
	printDistribution(add_subDist);
	printDistribution(logicDist);
	printDistribution(div_modDist);

	oss << "endmodule";  //close of module

	infile.close();

	string var = "";
	for (map<string, vector<string> >::const_iterator it = var_map.begin(); it != var_map.end(); ++it) {
		if (!it->second[0].compare("input") ||
			!it->second[0].compare("output"))
		{
			if (var.size() > 0)
				var += ", " + it->first;	// mult variables
			else
				var += it->first;	// first variable
		}
	}

	finalOutFile << "`timescale 1ns / 1ps" << endl << endl;

	finalOutFile << "module HSLM(Clk, Rst, Start, Done, " + var + ");" << endl;

	finalOutFile << oss.str();
	finalOutFile.close();

	return 0;
}


bool checkKey(string key, const map<string, vector<string> > &my_map)
{
	return my_map.find(key) != my_map.end();
}

int grabVariables(string line, map<string, vector<string> > &my_map)
{
	string func;
	string type;
	string token;
	istringstream iss(line);

	iss >> func;
	iss >> type;

	if (!checkType(type)) {
		return 2;
	}

	bool reg = false;
	string outstr;
	if (!func.compare("input"))
		outstr = "input";
	else if (!func.compare("output"))
		outstr = "output";
	else if (!func.compare("variable")) {
		outstr = "reg";
		reg = true;
	}
	else {
		cerr << "Error: unknown func: '" << func << "' in line " << endl << line << endl;
		return 1;
	}
	if (!type.compare("Int1"))
		outstr = outstr + " signed ";
	else if(!type.compare("UInt1"))
		outstr = outstr + " ";
	else if (!type.compare("Int2"))
		outstr = outstr + " signed [1:0] ";
	else if (!type.compare("UInt2"))
		outstr = outstr + "[1:0] ";
	else if (!type.compare("Int8"))
		outstr = outstr + " signed [7:0] ";
	else if (!type.compare("UInt8"))
		outstr = outstr + "[7:0] ";
	else if (!type.compare("Int16"))
		outstr = outstr + " signed [15:0] ";
	else if (!type.compare("UInt16"))
		outstr = outstr + "[15:0] ";
	else if (!type.compare("Int32"))
		outstr = outstr + " signed [31:0] ";
	else if (!type.compare("UInt32"))
		outstr = outstr + "[31:0] ";
	else if (!type.compare("Int64"))
		outstr = outstr + " signed [63:0] ";
	else if (!type.compare("UInt64"))
		outstr = outstr + "[63:0] ";
	else {
		cerr << "Error: unknown type: '" << type << "' in line: " << endl << line << endl;
		return 2;
	}

	while (iss >> token)	// grab all tokens after input
	{
		if (reg) {
			string rtok = token;
			if (token.back() == ',') {
				token.pop_back();
				rtok = token;
				token.append("_out,");
			}
			else
				token.append("_out");
			vector<string> newVector2;
			newVector2.push_back("register");
			newVector2.push_back(type);
			my_map[rtok] = newVector2;
		}
		outstr = outstr + token;
		if (token.back() == ',')
			token.pop_back();
		vector<string> newVector;
		newVector.push_back(reg ? "wire" : func);
		newVector.push_back(type);
		my_map[token] = newVector;
	}
	if (outstr.back() == ',') {
		cerr << "Error: expected another variable in line:" << endl << line << endl;
		return 4;
	}

	return 0;
}

bool checkType(string type)
{
	for (int i = 0; i < 12; ++i) {
		if (!type.compare(dataTypes[i]))
			return true;
	}
	return false;
};