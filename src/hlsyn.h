#include <string>

const string dataTypes[12] = {"Int1", "UInt1", "Int2", "UInt2", "Int8", "UInt8",
						"Int16", "UInt16", "Int32", "UInt32", "Int64", "UInt64" };
const string errorMsg[5] = {"","ERROR: Invalid Format Detected in cFile",
					  "ERROR: Invalid Type Provided in cFile",
                      "ERROR: Not implemented yet",
	                  "ERROR: Too many commas"};

void topModuleWrite(ofstream &finalOutFile, string name, const map<string, vector<string> > &my_map);

bool checkType(string type);
bool checkKey(string key, const map<string, vector<string> > &my_map);
int grabVariables(string line, map<string, vector<string> > &my_map);
