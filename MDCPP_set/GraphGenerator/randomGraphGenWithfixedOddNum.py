#!/usr/bin/python
import random
import sys
import numpy as np
import copy

def createRandomTopoWithFixedOdds(sNum, fixedOddNum):
	sys.setrecursionlimit(1000000)
	topoLists = []

	topoMatrix = g_generator_edge(10000, sNum)
	for i in topoMatrix:
		oddCount = calOddNum(i, sNum)
		if oddCount == fixedOddNum:
			for j in range(sNum):
				oneRow = [0 for k in range(sNum)]
				topoLists.append(oneRow)
			for layer1 in range(len(i)):
				for layer2 in range(len(i)):
					if i[layer1][layer2] == 1:
						topoLists[layer1][layer2] = 1
			break

	return topoLists

def calOddNum(topoMatrix, sNum):
	count = 0
	for i in range(sNum):
		degreeSum = 0
		for j in range(sNum):
			degreeSum += topoMatrix[i][j]
		if degreeSum%2 == 1:
			count += 1
	return count

def g_generator_edge(NUM_GRAPHS,NUM_NODES,edge_incre=1):
	op=[]
	NUM_GRAPHS=min(NUM_GRAPHS,int(NUM_NODES*(NUM_NODES-1)/2-NUM_NODES))
	for i in range(NUM_GRAPHS):
		if(i==0):
			network_matrix=np.zeros([NUM_NODES, NUM_NODES])
			for j in range(NUM_NODES-1):
				network_matrix[j][j+1] = 1
				network_matrix[j+1][j]=1
			network_matrix[0][NUM_NODES-1]=1
			network_matrix[NUM_NODES-1][0]=1
			op.append(network_matrix)
		else:
			network_matrix=copy.deepcopy(op[i-1])
			select=[]
			for j in range(NUM_NODES):
				for k in range(j+1,NUM_NODES):
					if(network_matrix[j][k]==0):
						select.append([j,k])
			for l in range(edge_incre):
				temp=select.pop(random.randint(0,len(select)-1))
				network_matrix[temp[0]][temp[1]]=1
				network_matrix[temp[1]][temp[0]]=1
			op.append(network_matrix)
	return op

def createDepotSet(depotNum, sNum):
	depotSet = [0 for i in range(sNum)]
	for i in range(depotNum):
		randomDepot = random.randint(0,sNum-1)
		while depotSet[randomDepot] == 1:
			randomDepot = random.randint(0,sNum-1)
		depotSet[randomDepot] = 1
	return depotSet

if __name__ == '__main__':
	fixedOddNum = 20        #set fixedOddNum
	for i in range(40, 151, 2):         #various sNum
		topo = createRandomTopoWithFixedOdds(i, fixedOddNum)
		depotSet = createDepotSet(12, i)      #set depotNum
		if len(topo) == 0:
			print("error")
			break
		print(topo)
		print(depotSet)
		print(fixedOddNum/2)
