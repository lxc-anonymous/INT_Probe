#include "MDCPPWithSet.h"
#include "MWPM/PerfectMatching.h"

void MDCPPWithSet::storeInitialGraph(vector<vector<int>> &initialGraph, vector<int> &initialSet)
{
	graph = initialGraph;
	depotSet = initialSet;
}

void MDCPPWithSet::findOddVertices()
{
	for(int i=0; i<graph.size(); ++i)
	{
		int degree = 0;
		for(int j=0; j<graph[i].size(); ++j)
		{
			if(graph[i][j] != 0)
				++degree;
		}
		if((degree & 0x01) == 1 && depotSet[i] == 0)
			oddsInComplementarySet.push_back(i);
	}
}

void MDCPPWithSet::createGp()
{
	int verticesNum = graph.size();
	int oddsNum = oddsInComplementarySet.size();
	for(int i=0; i<oddsNum; ++i)
	{
		int sourceNode = oddsInComplementarySet[i];
		vector<bool> S(verticesNum, false);
		vector<int> Disk(verticesNum, MAX);
		vector<int> Path(verticesNum, -1);
		int toDepotSetMIN = MAX;
		int toDepotSetNode = sourceNode;

		for(int i=0; i<verticesNum; ++i)
		{
			if(graph[sourceNode][i] != 0)
			{
				Disk[i] = graph[sourceNode][i];
				Path[i] = sourceNode;
			}
		}

		S[sourceNode] = true;
		Disk[sourceNode] = 0;

		for(int t=0; t<verticesNum; ++t)
		{
			int shortestDistance = MAX;
			if((sourceNode = FindMin(Disk,S,verticesNum,shortestDistance)) == -1)
				break;
			S[sourceNode] = true;
			for(int j=0; j<verticesNum; ++j)
			{
				if(!S[j] && graph[sourceNode][j]!=0 && (Disk[j]>graph[sourceNode][j]+Disk[sourceNode]))
				{
					Disk[j] = graph[sourceNode][j]+Disk[sourceNode];
					Path[j] = sourceNode;
				}
			}

			if(depotSet[sourceNode] == 1 && toDepotSetMIN > shortestDistance)
			{
				toDepotSetMIN = shortestDistance;
				toDepotSetNode = sourceNode;
				continue;
			}
			for(int k=0; k<oddsNum; ++k)
			{
				if(oddsInComplementarySet[k] == sourceNode && k > i)
				{
					edgesGp.push_back(i);
					edgesGp.push_back(k);
					weightsGp.push_back(shortestDistance);
				}
			}
		}

		shortestPaths.push_back(Path);
		edgesGp.push_back(i);
		edgesGp.push_back(i+oddsNum);
		weightsGp.push_back(toDepotSetMIN);
		GpToInitialGraph[i] = oddsInComplementarySet[i];
		GpToInitialGraph[i+oddsNum] = toDepotSetNode;
	}

	for(int i=0; i<oddsNum; ++i)
	{
		for(int j=i+1; j<oddsNum; ++j)
		{
			edgesGp.push_back(i+oddsNum);
			edgesGp.push_back(j+oddsNum);
			weightsGp.push_back(0);
		}
	}
}

int MDCPPWithSet::FindMin(const vector<int> &Disk, const vector<bool> &S, int n, int &min)
{
	int k = 0;
	for(int i=0; i<n; ++i)
	{
		if(!S[i] && min>Disk[i])
		{
			min = Disk[i];
			k = i;
		}
	}
	if(min == MAX) return -1;
	return k;
}

int MDCPPWithSet::solveMWPM()
{
	int nodeNum = oddsInComplementarySet.size()*2;
	int edgeNum = weightsGp.size();
	int *edges = new int[2*edgeNum];
	if(edges == nullptr) return -1;
	int *weights = new int[edgeNum];
	if(weights == nullptr) return -1;

	struct PerfectMatching::Options options;
	PerfectMatching *pm = new PerfectMatching(nodeNum, edgeNum);
	for(int e=0; e<edgeNum; ++e)
	{
		pm->AddEdge(edgesGp[2*e], edgesGp[2*e+1], weightsGp[e]);
		edges[2*e] = edgesGp[2*e];
		edges[2*e+1] = edgesGp[2*e+1];
		weights[e] = weightsGp[e];
	}
	pm->options = options;
	pm->Solve();
	cost = (int)ComputePerfectMatchingCost(nodeNum, edgeNum, edges, weights, pm);
	for(int i=0; i<oddsInComplementarySet.size(); ++i)
	{
		int j = pm->GetMatch(i);
		if(i < j) GpMatchingRes[i] = j;
	}
	delete pm;
	delete [] edges;
	delete [] weights;
	return 0;
}

int MDCPPWithSet::createNewG()
{
	unordered_map<int,int>::iterator iter;
	for(iter = GpMatchingRes.begin(); iter != GpMatchingRes.end(); ++iter)
	{
		int src = (*iter).first;
		int dst = GpToInitialGraph[(*iter).second];
		int preNode = shortestPaths[src][dst];
		while(preNode != -1)
		{
			if(graph[preNode][dst] == 1)
			{
				++graph[preNode][dst];
				++graph[dst][preNode];
				dst = preNode;
				preNode = shortestPaths[src][dst];
			}
			else
			{
				return -1;
			}
		}
	}
	return 0;
}

int MDCPPWithSet::solveMDCPP(vector<vector<int>> initialGraph, vector<int> initialSet)
{
	if(initialGraph.size() == 0 || initialSet.size() == 0)
		return -1;
	storeInitialGraph(initialGraph, initialSet);
	findOddVertices();
	if(oddsInComplementarySet.size() == 0)   //there is no odds in ComplementarySet;
		return 1;
	createGp();
	if(shortestPaths.size()==0 || GpToInitialGraph.size()==0 || edgesGp.size()==0 || weightsGp.size()==0)
		return -2;
	if(solveMWPM() == -1)
		return -3;
	if(GpMatchingRes.size()==0 || cost<=0)
		return -4;
	if(createNewG() == -1)
		return -5;
	return 0;
}

void MDCPPWithSet::dfsForEulerTrails(int cur, vector <int> &temp, int n, vector<vector<int>> &graphForEulerTrails)
{
	temp.push_back(cur);
	//cout<<cur+1<<" ";
	for(int i=0; i<n; ++i)
	{
		//判断当前顶点cur到顶点i是否有边,并判断顶点i是否已经访问过
		if(graphForEulerTrails[cur][i] != 0)
		{
			graphForEulerTrails[cur][i] -= 1;  //消去经历过的边
			graphForEulerTrails[i][cur] -= 1;
			dfsForEulerTrails(i, temp, n, graphForEulerTrails);
			break;
		}
	}
}

void MDCPPWithSet::judgeForEulerTrails(vector<vector<int>> &graphForEulerTrails, int n, vector<int> &temp)
{
	int zi[n];
	int cur;
	
	int flag_du = 0;   //度的数量
	for(int i=0; i<n; ++i)
	{
		for(int j=0; j<n; ++j)
		{
			if(graphForEulerTrails[i][j] != 0)
			{
				flag_du += graphForEulerTrails[i][j];
			}
		}
		zi[i] = flag_du;  //将度的数量存入数组
		flag_du = 0;
	}
	for(int i=0; i<n; ++i)
	{
		if(zi[i] % 2)  //寻找度数为奇数
		{
			cur = i;
			break;
		}
		else if(i == n-1) //若度数都为偶数，则从一号节点开始遍历
		{
			for(int k=0; k<n; ++k)
			{
				if(zi[k] != 0)
				{
					cur = k;
					break;
				}
			}
		}
	}
	dfsForEulerTrails(cur, temp, n, graphForEulerTrails);
}

void MDCPPWithSet::connectPathForEulerTrails()
{
	for(vector<vector<int>>::iterator pathIter=pathsResult.begin(); pathIter!=pathsResult.end();)
	{
		int doConnectFlag = 0;
		if(*((*pathIter).begin()) == *((*pathIter).end()-1))
		{
			// for one euler circuit, do the connecting work
			vector<int> keyVec = *pathIter;
			sort(keyVec.begin(), keyVec.end());
			keyVec.erase(unique(keyVec.begin(), keyVec.end()), keyVec.end());
			int breakFlag = 0;
			for(vector<vector<int>>::iterator pathIter2=pathsResult.begin(); pathIter2!=pathsResult.end(); ++pathIter2)
			{
				if(pathIter2 != pathIter)
				{
					for(int k=0; k<keyVec.size(); ++k)
					{
						vector<int>::iterator got = find((*pathIter2).begin(), (*pathIter2).end(), keyVec[k]);
						if(got != (*pathIter2).end())
						{
							// find a path which contains this euler circuit's vertex
							vector<int> eulerCircuit;
							vector<int>::iterator eulerCircuitBegin = find((*pathIter).begin(), (*pathIter).end(), keyVec[k]);
							eulerCircuit.insert(eulerCircuit.end(), eulerCircuitBegin, (*pathIter).end());
							eulerCircuit.insert(eulerCircuit.end(), (*pathIter).begin()+1, eulerCircuitBegin+1);
							(*pathIter2).insert(got, eulerCircuit.begin(), eulerCircuit.end()-1);
							pathIter = pathsResult.erase(pathIter);
							doConnectFlag = 1;
							breakFlag = 1;
							break;
						}
					}
				}
				if(breakFlag == 1)
					break;
			}
		}
		if(doConnectFlag == 0)
			++pathIter;
	}
}

void MDCPPWithSet::solveEulerTrails(int &pathNum)
{
	vector<vector<int>> graphForEulerTrails;
	graphForEulerTrails = graph;

	int n = graphForEulerTrails.size();

	for(int i=0; i<n; ++i)  //遍历数组，直到所有边耗尽
	{
		for(int j=0; j<n; ++j)
		{
			if(graphForEulerTrails[i][j] != 0)
			{
				vector<int> temp;
				judgeForEulerTrails(graphForEulerTrails, n, temp);
				pathsResult.push_back(temp);
			}
		}
	}

	connectPathForEulerTrails();
	pathNum = pathsResult.size();
}
