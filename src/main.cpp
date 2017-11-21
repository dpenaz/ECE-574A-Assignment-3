#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <tuple>
#include "Verilog_Imp.h"
#include "dpgen.h"

using namespace std;

ofstream outfile;
bool regFound;

vector<tuple<string, vector<string>, string, bool>> graph;
const map<string, double> latencies = {
	{ "REG1", 2.616 },{ "REG2", 2.644 } ,{ "REG8", 2.879 },{ "REG16",3.061 },{ "REG32",3.602 },{ "REG64",3.966 },
	{ "SREG1", 2.616 },{ "SREG2", 2.644 } ,{ "SREG8", 2.879 },{ "SREG16",3.061 },{ "SREG32",3.602 },{ "SREG64",3.966 },

	{ "ADD1", 2.704 },{ "ADD2", 3.713 } ,{ "ADD8", 4.924 },{ "ADD16",5.638 },{ "ADD32",7.270 },{ "ADD64",9.566 },
	{ "SADD1", 2.704 },{ "SADD2", 3.713 } ,{ "SADD8", 4.924 },{ "SADD16",5.638 },{ "SADD32",7.270 },{ "SADD64",9.566 },

	{ "SUB1", 3.024 },{ "SUB2", 3.412 } ,{ "SUB8", 4.890 },{ "SUB16",5.569 },{ "SUB32",7.253 },{ "SUB64",9.566 },
	{ "SSUB1", 3.024 },{ "SSUB2", 3.412 } ,{ "SSUB8", 4.890 },{ "SSUB16",5.569 },{ "SSUB32",7.253 },{ "SSUB64",9.566 },

	{ "MUL1", 2.438 },{ "MUL2", 3.651 } ,{ "MUL8", 7.453 },{ "MUL16",7.811 },{ "MUL32",12.395 },{ "MUL64",15.354 },
	{ "SMUL1", 2.438 },{ "SMUL2", 3.651 } ,{ "SMUL8", 7.453 },{ "SMUL16",7.811 },{ "SMUL32",12.395 },{ "SMUL64",15.354 },

	{ "COMP1", 3.031 },{ "COMP2", 3.934 } ,{ "COMP8", 5.949 },{ "COMP16",6.256 },{ "COMP32",7.264 },{ "COMP64",8.416 },
	{ "SCOMP1", 3.031 },{ "SCOMP2", 3.934 } ,{ "SCOMP8", 5.949 },{ "SCOMP16",6.256 },{ "SCOMP32",7.264 },{ "SCOMP64",8.416 },

	{ "MUX2x11", 4.083 },{ "MUX2x12", 4.115 } ,{ "MUX2x18", 4.815 },{ "MUX2x116",5.623 },{ "MUX2x132",8.079 },{ "MUX2x164",8.766 },
	{ "SMUX2x11", 4.083 },{ "SMUX2x12", 4.115 } ,{ "SMUX2x18", 4.815 },{ "SMUX2x116",5.623 },{ "SMUX2x132",8.079 },{ "SMUX2x164",8.766 },

	{ "SHR1", 3.644 },{ "SHR2", 4.007 } ,{ "SHR8", 5.178 },{ "SHR16",6.460 },{ "SHR32",8.819 },{ "SHR64",11.095 },
	{ "SSHR1", 3.644 },{ "SSHR2", 4.007 } ,{ "SSHR8", 5.178 },{ "SSHR16",6.460 },{ "SSHR32",8.819 },{ "SSHR64",11.095 },

	{ "SHL1", 3.614 },{ "SHL2", 3.980 } ,{ "SHL8", 5.152 },{ "SHL16",6.549 },{ "SHL32",8.565 },{ "SHL64",11.220 },
	{ "SSHL1", 3.614 },{ "SSHL2", 3.980 } ,{ "SSHL8", 5.152 },{ "SSHL16",6.549 },{ "SSHL32",8.565 },{ "SSHL64",11.220 },

	{ "DIV1", 0.619 },{ "DIV2", 2.144 } ,{ "DIV8", 15.439 },{ "DIV16",33.093 },{ "DIV32",86.312 },{ "DIV64",243.233 },
	{ "SDIV1", 0.619 },{ "SDIV2", 2.144 } ,{ "SDIV8", 15.439 },{ "SDIV16",33.093 },{ "SDIV32",86.312 },{ "SDIV64",243.233 },

	{ "MOD1", 0.758 },{ "MOD2", 2.149 } ,{ "MOD8", 16.078 },{ "MOD16",35.563 },{ "MOD32",88.142 },{ "MOD64",250.583 },
	{ "SMOD1", 0.758 },{ "SMOD2", 2.1494 } ,{ "SMOD8", 16.078 },{ "SMOD16",35.563 },{ "SMOD32",88.142 },{ "SMOD64",250.583 },

	{ "INC1", 1.792 },{ "INC2", 2.218 } ,{ "INC8", 3.111 },{ "INC16",3.471 },{ "INC32",4.347 },{ "INC64",6.200 },
	{ "SINC1", 1.792 },{ "SINC2", 2.218 } ,{ "SINC8", 3.111 },{ "SINC16",3.471 },{ "SINC32",4.347 },{ "SINC64",6.200 },

	{ "DEC1", 1.792 },{ "DEC2", 2.218 } ,{ "DEC8", 3.108 },{ "DEC16",3.701 },{ "DEC32",4.685 },{ "DEC64",6.503 },
	{ "SDEC1", 1.792 },{ "SDEC2", 2.218 } ,{ "SDEC8", 3.108 },{ "SDEC16",3.701 },{ "SDEC32",4.685 },{ "SDEC64",6.503 }
};

int main(int argc, char *argv[])
{
	if (argc < 3) {
		cout << "Usage: " << argv[0] << " netlistFile verilogFile" << endl;
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
	outfile.open("temp.txt");

	regFound = false;
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
			outfile << errorMsg[errorFlag];
			break;
		}
		else
			outfile << msg;
	}

	outfile << "endmodule";  //close of module

	infile.close();
	outfile.close();

	topModuleWrite(finalOutFile, argv[2], var_map);
	finalOutFile.close();

	double max_time = 0;
	map<string, double> graphLatencies;
	vector<bool> scheduled;
	for (auto var : var_map) {
		if (!var.second[0].compare("input") || !var.second[0].compare("register"))
			graphLatencies[var.first] = 0;
	}
	for (auto& node : graph) {
		if (!get<3>(node)) {
			vector<string> vins = get<1>(node);
			bool ready = true;
			double max_in_time = 0;
			//check if all inputs are ready
			for (auto vin : vins) {
				if (vin.size() > 4) {
					if (vin.substr(vin.size() - 4, vin.size()) == "_out")
						vin.erase(vin.size() - 4, vin.size());
				}
				auto it = graphLatencies.find(vin);
				if (it == graphLatencies.end())
					ready = false;
				else {
					double t = it->second;
					max_in_time = max_in_time > t ? max_in_time : t;
				}
			}
			//if this node is ready to be scheduled
			if (ready) {
				//this node is now scheduled
				get<3>(node) = true;
				//find out time
				double node_out_time = max_in_time + latencies.find(get<0>(node))->second;
				max_time = max_time > node_out_time ? max_time : node_out_time;
				graphLatencies[get<2>(node)] = node_out_time;
			}
		}
	}

	cout << "Critical Path: " << max_time << " ns" << endl;
	return 0;
}


void topModuleWrite(ofstream &finalOutFile, string name, const map<string, vector<string> > &my_map)
{
	ifstream infile("temp.txt");
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
	if (regFound)
		var += ", clk, rst";

	finalOutFile << "`timescale 1ns / 1ps" << endl << endl;

	if (regFound)
		finalOutFile << "module circuit(" + var + ");" << endl << "input clk, rst;" << endl;
	else
		finalOutFile << "module circuit(" + var + ");" << endl;


	for (string line; getline(infile, line);)	// Pass through all lines of code
	{
		finalOutFile << line << endl;
	}
	infile.close();
	remove("temp.txt");
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
	outfile << outstr << ";" << endl;
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