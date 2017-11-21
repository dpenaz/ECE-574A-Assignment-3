#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <tuple>
#include "Verilog_Imp.h"
#include "hlsyn.h"

using namespace std;

ostringstream oss;

//            type      in names     name    scheduled
vector<tuple<string, vector<string>, string, bool>> graph;

int main(int argc, char *argv[])
{
	if (argc != 4) {
		cout << "Usage: " << argv[0] << " cFile latency verilogFile" << endl;
		return 1;
	}

	ifstream infile(argv[1]);
	if (infile.fail()) {
		cerr << "Input file " << argv[1] << " failed to open" << endl;
		return 1;
	}

	ofstream finalOutFile;
	finalOutFile.open(argv[2]);
	if (finalOutFile.fail()) {
		cerr << "Output file " << argv[2] << " failed to open" << endl;
		return 1;
	}

	string msg = "";
	string type;
	string token;

	int errorFlag = 0;

	map<string, vector<string> > var_map;	// Store variable info ex: variables[name][type, input/output/wire]
	vector<string> storedTokens;

	for (string line; getline(infile, line);)	// Pass through all lines of code
	{
		istringstream iss(line);

		bool eqFound = false, otherFound = false;;
		while (iss >> token)	// Pass through each token in line
		{
			if (!token.compare("+")) {	// ADD and INC
				otherFound = true;
				if (!Add_or_INC(line, var_map))
					return 1;
				break;
			}
			else if (!token.compare("=")) {	// REG
				eqFound = true;
			}
			else if (!token.compare("-")) {	// SUB and DEC
				otherFound = true;
				if (!SUB_or_DEC(line, var_map))
					return 1;
				break;
			}
			else if (!token.compare("*")) {	// MUL
				otherFound = true;
				if (!MUL_(line, var_map))
					return 1;
				break;
			}
			else if (!token.compare(">")) {	// COMP (gt output)
				otherFound = true;
				if (!COMP_gt(line, var_map))
					return 1;
				break;
			}
			else if (!token.compare("<")) {	// COMP (lt output)
				otherFound = true;
				if (!COMP_lt(line, var_map))
					return 1;
				break;
			}
			else if (!token.compare("==")) {// COMP (eq output)
				otherFound = true;
				if (!COMP_eq(line, var_map))
					return 1;
				break;
			}
			else if (!token.compare("?") ||
				!token.compare(":")) {	// MUX2x1
				otherFound = true;
				if (!MUX2x1_(line, var_map))
					return 1;
				break;
			}
			else if (!token.compare(">>")) { // SHR
				otherFound = true;
				if (!SHR_(line, var_map))
					return 1;
				break;
			}
			else if (!token.compare("<<")) { // SHL
				otherFound = true;
				if (!SHL_(line, var_map))
					return 1;
				break;
			}
			else if (!token.compare("/")) {	// DIV
				otherFound = true;
				if (!DIV_(line, var_map))
					return 1;
				break;
			}
			else if (!token.compare("%")) {	// MOD
				otherFound = true;
				if (!MOD_(line, var_map))
					return 1;
				break;
			}
			else if (!token.compare("//")) { // COMMENT!!!
				otherFound = true;
				break;
			}
			else if (!token.compare("input")) {	// input variables
				otherFound = true;
				errorFlag = grabVariables(line, var_map);
				break;	// finished grabbing line continue to next 
			}
			else if (!token.compare("output")) { // output variables
				otherFound = true;
				errorFlag = grabVariables(line, var_map);
				break;	// finished grabbing line continue to next line
			}
			else if (!token.compare("wire")) {	// wire variables
				otherFound = true;
				errorFlag = grabVariables(line, var_map);
				break;	// finished grabbing line continue to next line
			}
			else if (!token.compare("register")) { // register variables
				otherFound = true;
				errorFlag = grabVariables(line, var_map);
				break;	// finished grabbing line continue to next line
			}
			else {
				if (!checkKey(token, var_map))
				{
					// ERROR VARIABLE NOT DEFINED AS INPUT, OUTPUT, REGISTER or WIRE
					errorFlag = 1;
				}
				storedTokens.push_back(token);
				msg = "";
			}

		}
		if (eqFound && !otherFound)
			if (!REG_(line, var_map))
				return 1;

		if (errorFlag != 0) {
			cerr << errorMsg[errorFlag];
			oss << errorMsg[errorFlag];
			break;
		}
		else
			oss << msg;
	}

	oss << "endmodule";  //close of module

	infile.close();

	topModuleWrite(finalOutFile, argv[2], var_map);
	finalOutFile.close();

	return 0;
}


void topModuleWrite(ofstream &finalOutFile, string name, const map<string, vector<string> > &my_map)
{
	string var = "";
	for (map<string, vector<string> >::const_iterator it = my_map.begin(); it != my_map.end(); ++it) {
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
}

bool checkKey(string key, const map<string, vector<string> > &my_map)
{
	return my_map.find(key) != my_map.end();
}

int grabVariables(string line, map<string, vector<string> > &my_map)
{
	int error = 2;
	string func;
	string type;
	string token;
	istringstream iss(line);

	iss >> func;
	iss >> type;

	if (!checkType(type)) {
		return error;
	}

	bool reg = false;
	string outstr;
	if (!func.compare("input"))
		outstr = "input";
	else if (!func.compare("output"))
		outstr = "output";
	else if (!func.compare("wire"))
		outstr = "wire";
	else if (!func.compare("register")) { // TBD
		outstr = "wire";
		reg = true;
	}
	else {
		cerr << "Error: unknown func: '" << func << "' in line " << endl << line << endl;
		return 1;
	}
	if (!type.compare("Int1") || !type.compare("UInt1"))
		outstr = outstr + " ";
	else if (!type.compare("Int2") || !type.compare("UInt2"))
		outstr = outstr + "[1:0] ";
	else if (!type.compare("Int8") || !type.compare("UInt8"))
		outstr = outstr + "[7:0] ";
	else if (!type.compare("Int16") || !type.compare("UInt16"))
		outstr = outstr + "[15:0] ";
	else if (!type.compare("Int32") || !type.compare("UInt32"))
		outstr = outstr + "[31:0] ";
	else if (!type.compare("Int64") || !type.compare("UInt64"))
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
	oss << outstr << ";" << endl;
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