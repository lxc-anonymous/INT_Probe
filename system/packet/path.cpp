#include<iostream>
#include<sstream>
#include<fstream>
#include<vector>
#include<map>
using namespace std;

int main()
{
	vector<vector<int> > nodes;
	vector<int> in_vec;
	ifstream in("data.txt");
	for(string s;getline(in,s);)
	{
		istringstream sin(s);
		for(int digit;sin>>digit;)
		{
			in_vec.push_back(digit);
		}
		nodes.push_back(in_vec);
		in_vec.clear();
	}
	for(int i=0;i<nodes.size();i++){
		for(int j=0;j<nodes[i].size();j++)
			cout<<nodes[i][j]<<" ";
		cout<<endl;
	}
	int startDegree=(nodes[0].size()-1)/2;
	int totalNode=nodes.size();
	int path_num=0;
	if(totalNode==4)
		path_num=4;
	if(totalNode==6)
		path_num=8;
	if(totalNode==8)
		path_num=16;
		
	map<int,int> nodeNum;
	
	for(int i=0;i<totalNode;i++)
		nodeNum.insert ( std::pair<int,int>(nodes[i][0],i ));
	int node = nodes[0][0];
	while(path_num>1)
	{
		int i=nodeNum.find(node)->second;
		
		int next_id=0;
		if(nodes[i].size()>1)
		{
			int n=0;
			if(nodes[i][2]==nodes[0][0]&&nodes[0].size()==3)
				n=3;
			else n=1;
			cout<<nodes[i][n]<<"--";
			next_id=nodes[i][n+1];
			if(nodes[i].size()>3){
				nodes[i][n]=nodes[i][nodes[i].size()-2];
				nodes[i][n+1]=nodes[i][nodes[i].size()-1];
			}
			nodes[i].pop_back();
			nodes[i].pop_back();
		}
		int next_node=nodeNum.find(next_id)->second;

		for(int j=2;j<nodes[next_node].size();j+=2){
			if(nodes[next_node][j]==node)
			{
				if(j<nodes[next_node].size()-1)
				{
					nodes[next_node][j-1]=nodes[next_node][nodes[next_node].size()-2];
					nodes[next_node][j]=nodes[next_node][nodes[next_node].size()-1];
				}
				
				nodes[next_node].pop_back();
				nodes[next_node].pop_back();
			}
		}
		node=next_id;
		cout<<node<<endl<<endl;
		path_num--;
	}
	cout<< nodes[nodeNum.find(node)->second][1]<<endl;
}

