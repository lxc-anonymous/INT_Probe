#include <vector>
#include <unordered_map>
#include <iterator>
#include <algorithm>
using std::vector;
using std::unordered_map;
using std::iterator;

#define MAX 1000000000

class MDCPPWithSet
{
public:
	int solveMDCPP(vector<vector<int>> initialGraph, vector<int> initialSet);
	void solveEulerTrails(int &pathNum);
	void printSomething();
	void recordData(int &theoryPathNum, int &overlappedNum);
private:
	void storeInitialGraph(vector<vector<int>> &initialGraph, vector<int> &initialSet);
	void findOddVertices();
	void createGp();
	int solveMWPM();
	int createNewG();
	int FindMin(const vector<int> &Disk, const vector<bool> &S, int n, int &min);
	void judgeForEulerTrails(vector<vector<int>> &graphForEulerTrails, int n, vector<int> &temp);
	void dfsForEulerTrails(int cur, vector <int> &temp, int n, vector<vector<int>> &graphForEulerTrails);
	void connectPathForEulerTrails();

	vector<vector<int>> graph;
	vector<int> depotSet;
	vector<int> oddsInComplementarySet;
	vector<vector<int>> shortestPaths;
	unordered_map<int,int> GpToInitialGraph;
	vector<int> edgesGp;
	vector<int> weightsGp;
	int cost = 0;
	unordered_map<int,int> GpMatchingRes;
	vector<vector<int>> pathsResult;
};
