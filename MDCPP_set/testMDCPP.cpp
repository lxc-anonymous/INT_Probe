#include <iostream>
#include <fstream>
#include <string>
#include "MDCPPWithSet.h"

#include <sys/time.h>

using std::cout;
using std::endl;
using std::string;
using std::ifstream;

void MDCPPWithSet::printSomething()
{
/*	for(int i=0;i<edgesGp.size();++i) cout<<edgesGp[i]<<" ";
	cout<<endl;
	for(int i=0;i<weightsGp.size();++i) cout<<weightsGp[i]<<" ";
	cout<<endl;
	cout<<endl;
	for(auto iter = GpMatchingRes.begin(); iter != GpMatchingRes.end(); ++iter)
	{
		cout<<(*iter).first<<" "<<(*iter).second<<endl;
	}
	cout<<endl;
	for(int i=0;i<graph.size();++i)
	{
		for(int j=0;j<graph[i].size();++j)
			cout<<graph[i][j]<<" ";
		cout<<endl;
	}
	cout<<endl;
	for(int i=0;i<depotSet.size();++i)
	{
		cout<<depotSet[i]<<" ";
	}
	cout<<endl;*/
	for(int i=0;i<pathsResult.size();++i)
	{
		for(int j=0;j<pathsResult[i].size();++j)
		{
			cout<<pathsResult[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<pathsResult.size()<<endl;
}

void MDCPPWithSet::recordData(int &theoryPathNum, int &overlappedNum)
{
	overlappedNum = cost;
	int oddCount = 0;
	for(int i=0; i<depotSet.size(); ++i)
	{
		if(depotSet[i] == 1)
		{
			int degree = 0;
			for(int j=0; j<graph[i].size(); ++j)
			{
				degree += graph[i][j];
			}
			if((degree & 0x01) == 1)
				++oddCount;
		}
	}
	if(oddCount == 0)
		theoryPathNum = 1;
	else if((oddCount & 0x01) == 0)
		theoryPathNum = oddCount/2;
	else
		theoryPathNum = 0;
}

int parseOneDimensionMatrix(vector<int> &result, int &switchNum, const string &matrix)
{
	int stringSize = matrix.size();
	if(stringSize==0 || stringSize%3!=0) return -1;
	switchNum = stringSize/3;
	if(!result.empty()) return -2;
	for(int i=0; i<switchNum; ++i)
	{
		int num = matrix[1+3*i] - '0';
		if(num!=0 && num!=1) return -3;
		result.push_back(num);
	}
	return 0;
}

int main(int argc, char* argv[])
{
/*	vector<vector<int>> initGraph(9, vector<int>(9,0));
	vector<int> initSet={1,0,1,0,1,0,0,0,1};
	// initGraph[0][1] = 2;initGraph[1][0] = 2;
	// initGraph[0][3] = 5;initGraph[3][0] = 5;
	// initGraph[1][2] = 4;initGraph[2][1] = 4;
	// initGraph[1][4] = 3;initGraph[4][1] = 3;
	// initGraph[2][5] = 2;initGraph[5][2] = 2;
	// initGraph[3][4] = 6;initGraph[4][3] = 6;
	// initGraph[3][6] = 5;initGraph[6][3] = 5;
	// initGraph[4][5] = 4;initGraph[5][4] = 4;
	// initGraph[4][7] = 4;initGraph[7][4] = 4;
	// initGraph[5][8] = 4;initGraph[8][5] = 4;
	// initGraph[6][7] = 9;initGraph[7][6] = 9;
	// initGraph[7][8] = 4;initGraph[8][7] = 4;
	initGraph[0][1] = 1;initGraph[1][0] = 1;
	initGraph[0][3] = 1;initGraph[3][0] = 1;
	initGraph[1][2] = 1;initGraph[2][1] = 1;
	initGraph[1][4] = 1;initGraph[4][1] = 1;
	initGraph[2][5] = 1;initGraph[5][2] = 1;
	initGraph[3][4] = 1;initGraph[4][3] = 1;
	initGraph[3][6] = 1;initGraph[6][3] = 1;
	initGraph[4][5] = 1;initGraph[5][4] = 1;
	initGraph[4][7] = 1;initGraph[7][4] = 1;
	initGraph[5][8] = 1;initGraph[8][5] = 1;
	initGraph[6][7] = 1;initGraph[7][6] = 1;
	initGraph[7][8] = 1;initGraph[8][7] = 1;*/
	char* filename = nullptr;
	if(argc != 2){cout<<"wrong parameter."; return -1;}
	filename = argv[1];
	if(!filename){cout<<"wrong parameter."; return -1;}
	ifstream input(filename);
	if(!input){cout<<"wrong parameter."; return -1;}
	string graphMatrix = "";
	string depotMatrix = "";
	string eulerPathStr = "";
	int xAxis;
	while(getline(input, graphMatrix) && getline(input, depotMatrix) && getline(input, eulerPathStr))
	{
		int eulerPathNum = atoi(eulerPathStr.c_str());
		vector<int> initSet;
		int sNum = 0;
		if(parseOneDimensionMatrix(initSet, sNum, depotMatrix) < 0)
		{
			cout<<"wrong depotMatrix."; return -1;
		}
		vector<vector<int>> initGraph;
		int period = sNum*3 + 2;
		int edgesSum = 0;
		for(int k=0; k<sNum; ++k)
		{
			vector<int> oneRow;
			int s = 0;
			if(parseOneDimensionMatrix(oneRow, s, graphMatrix.substr(1+period*k, sNum*3)) < 0 || s!=sNum)
			{
				cout<<"wrong graphMatrix."; return -1;
			}
			for(int ele=0; ele<oneRow.size(); ++ele)
			{
				edgesSum += oneRow[ele];
			}
			initGraph.push_back(oneRow);
		}
		edgesSum /= 2;

		int theoryPathNum = 0, overlappedNum = 0, pathNum = 0;
		struct timeval startTime, endTime;
		gettimeofday(&startTime,NULL);

		MDCPPWithSet testMDCPP;
		int res = testMDCPP.solveMDCPP(initGraph, initSet);
		if(res < 0){
			cout<<"error"<<endl;
			return -1;
		}
		testMDCPP.solveEulerTrails(pathNum);

		gettimeofday(&endTime,NULL);
		long s = startTime.tv_sec*1000000 + startTime.tv_usec;
		long e = endTime.tv_sec*1000000 + endTime.tv_usec;
		long time = e - s;   //usec
		
	//	testMDCPP.printSomething();
		testMDCPP.recordData(theoryPathNum, overlappedNum);
		if(theoryPathNum != pathNum){
			cout<<"error"<<endl;
			cout<<"theoryPathNum: "<<theoryPathNum<<endl;
			cout<<"pathNum: "<<pathNum<<endl;
			cout<<"eulerPathNum: "<<eulerPathNum<<endl;
			testMDCPP.printSomething();
			return -1;
		}

	//	xAxis = sNum;
	//	cout<<xAxis<<"	"<<pathNum<<"	"<<overlappedNum<<"	"<<eulerPathNum<<endl;  //SNDlibGraph
	//	cout<<xAxis<<"	"<<pathNum<<"	"<<overlappedNum<<"	"<<eulerPathNum<<"	"<<edgesSum<<"	"<<(edgesSum+overlappedNum)<<endl; //randomGraph
	//	cout<<xAxis<<"	"<<pathNum<<"	"<<overlappedNum<<"	"<<eulerPathNum<<endl; //randomGraphWithfixedOdd
		xAxis = sNum;  //randomGraph  calculate time with different depotNum
		cout<<xAxis<<"	"<<time<<endl;  //randomGraph  calculate time with different depotNum
	//	cout<<xAxis<<"	"<<pathNum<<"	"<<overlappedNum<<"	"<<eulerPathNum<<endl; //SNDlibWithEdges
	//	xAxis = sNum;  //fatTree and leafSpine
	//	cout<<xAxis<<"	"<<pathNum<<"	"<<overlappedNum<<"	"<<eulerPathNum<<"	"<<"0"<<endl; //fatTree and leafSpine

	//	xAxis += 1;
	}
	return 0;
}
