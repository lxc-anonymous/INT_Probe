#include<iostream>  
#include<cstdlib>
#include<sys/time.h>
#include<hiredis/hiredis.h>
#include<vector>
#define NUM 30 
#define CHOSE 15
using namespace std;  

int path14[CHOSE];

vector<vector<int> > next_hop,latency_data;

struct latency{
	int latency_14[NUM];
	int latency_23[NUM][NUM]; 
	int judge;
	int chosed_14[2][CHOSE];
	int a[2][NUM];
};

void swap(int a[][NUM], int index,int low) {

	int temp1 = a[0][index],temp2=a[1][index];
	a[0][index]=a[0][low];
	a[1][index]=a[1][low];
	a[0][low]=temp1;
	a[1][low]=temp2;
}

int rand(int low, int high) {
	int size = high - low + 1;
	return low + rand() % size;
}

int random_partition(int a[][NUM], int low, int high) {
	int index = rand(low, high);
	swap(a,index,low);
	int key = a[1][low];
	int i = low;
	for(int j = low + 1; j <= high; j++) {
		if(a[1][j] <= key) {
			if(i != j) {
				i = i + 1;
				swap(a,i,j);
			}
		}
	}
	swap(a,low,i);
	return i;
}

int random_select(int a[][NUM], int low, int high, int k) 
{
	if(k < 1 || k > (high - low + 1))
		return -1;
	int pos = random_partition(a, low, high);
	int m = pos - low + 1;
	if(m == k) {
		return a[0][pos];
	} else if(k < m) {
		return random_select(a, low, pos - 1, k);
	} else {
		return random_select(a, pos + 1, high, k - m);
	}
}

int judgeOnePod(int start_id,int end_id)//true--in one pod
{
	if(end_id-start_id>=30)return 0;
	else if((end_id>=1110&&start_id<1110)||(end_id>=1140&&start_id<1140)||(end_id>=1170&&start_id<1170)||(end_id>=1200&&start_id<1200)||(end_id>=1230&&start_id<1259))
		return 0;
	else return 1;
}
void *topk(void *arg)
{
	int start = *(int *)arg;
	cout<<"start_id : "<<start<<endl;
	struct latency latency_n;
	for(int start_id=start;start_id<start+30;start_id++)
	{
		for(int end_id=start;end_id<start+30;end_id++)
		{
			if(start_id==end_id)continue;
			latency_n.judge = judgeOnePod(start_id,end_id);
			for(int x=0;x<latency_data[start_id].size();x++)
			{
				latency_n.latency_14[x] = latency_data[start_id][x]+latency_data[end_id][x];
				int start_next = next_hop[start_id][x],before_end = next_hop[end_id][x];
				for(int y=0;y<latency_data[start_next].size();y++)
				{
					latency_n.latency_23[x][y] = latency_data[start_next][y]+latency_data[before_end][y];
				}
			}

			for(int i=0;i<NUM;i++)
			{
				latency_n.a[0][i]=i;
				latency_n.a[1][i]=latency_n.latency_14[i];
			}
			int result = random_select((latency_n.a), 0, NUM - 1, CHOSE);

			cout<<endl<<" "<<start<<" 1+4 : "<<endl;
			for(int i=0;i<15;i++)
			{
				latency_n.chosed_14[0][i] = latency_n.a[0][i];
				latency_n.chosed_14[1][i] = latency_n.a[1][i];
				path14[i]=latency_n.a[0][i];
				cout<<path14[i]<<" ";
			}
			if(latency_n.judge==0)
			{
				for(int j=0;j<CHOSE;j++)
				{
					int p=path14[j];
					for(int i=0;i<NUM;i++) 
					{
						latency_n.a[0][i]=i;
						latency_n.a[1][i]=latency_n.latency_23[p][i];
					}
					result = random_select((latency_n.a), 0, NUM - 1, CHOSE);

					for(int i=0;i<NUM;i++)
					{
						cout<<" "<<latency_n.a[1][i];
					}
					cout<<endl<<j<<"2+3:"<<endl;
					for(int i=0;i<CHOSE;i++)
					{
						cout<<latency_n.a[0][i]<<" "<<latency_n.a[1][i]<<endl;
					}
					cout<<endl;
				}
			}
		}
	}
	cout<<"calculate "<<start<<" end!"<<endl;
}

void readDatabase()
{
	vector<int> temp1,temp2;
	char cmd[50]={0};
	redisContext *c = redisConnect("127.0.0.1",6379);
	if(c->err)
	{
		redisFree(c);
		printf("connect fail");
	}
	printf("connect success!\n");
	redisReply *reply = (redisReply*)redisCommand(c, "select 4");
	for(int j=0;j<1260;j++)	
	{
		snprintf(cmd,sizeof(cmd),"lrange %d 0 -1",j);	
		reply = (redisReply*)redisCommand(c, cmd);
	
		for(int i=0;i<reply->elements;i++)
		{
			redisReply *childReply = reply->element[i];
			if(childReply->type==REDIS_REPLY_STRING)
			{
				
				
				if(((i-2)%4==0)&&(i>=2))//next_hop 
				{
					temp1.push_back(atoi(childReply->str));
				}
				if(((i-4)%4==0)&&(i>=4))//next_hop 
				{
					temp2.push_back(atoi(childReply->str));
				}
			}
			
		}
		latency_data.push_back(temp2);
		next_hop.push_back(temp1);
		temp1.clear();
		temp2.clear();
	}
	freeReplyObject(reply);
	redisFree(c);
}
int main()
{
	struct latency latency_1,latency_2;
	struct latency latency_input[NUM];
	readDatabase();
	cout<<endl<<"----------------topk begin-------------"<<endl;
	pthread_t topk_1_id,topk_2_id;
	int start_id_1=1080;
	int tret1 = pthread_create(&topk_1_id, NULL, topk, &start_id_1);
	if(tret1)
	{
		cout << "topk_1 error!!!!" << endl;
      		return 1;
	}
	int start_id_2=1110;
	int tret2 = pthread_create(&topk_2_id, NULL, topk, &start_id_2);
	if(tret2)
	{
		cout << "topk_2 error!!!!" << endl;
        return 1;
	}
	pthread_join(topk_1_id, NULL);
	pthread_join(topk_2_id, NULL);
}
