#! /usr/bin/python
import sys
import random
import numpy as np
import copy

def transformSNDGraph(filename):
	file = open(filename)
	diction = {}
	sNum = 0
	flag = 0
	while True:
		textLine = file.readline()
		if textLine:
			if textLine.startswith("NODES"):
				flag = 1
			elif textLine.startswith("LINKS"):
				flag = 2
			elif len(textLine)>2 and textLine[0] == " ":
				#print(textLine)
				if flag == 1:   #parse nodes
					stringSplit = textLine.split()
					diction[stringSplit[0]] = sNum
					sNum += 1
				elif flag == 2:
					break
		else:
			break
	file.close()
	return sNum, diction

def createRandomTopo(sNum, edgeNum):
	sys.setrecursionlimit(1000000)
	#create adjaMatrix
	topoLists = []
	topoMatrix = g_generator_edge(10000, sNum)
	for i in topoMatrix:
		edgeCount = countEdgesNum(i)
		if edgeCount == edgeNum:
			for j in range(sNum):
				oneRow = [0 for k in range(sNum)]
				topoLists.append(oneRow)
			for layer1 in range(len(i)):
				for layer2 in range(len(i)):
					if i[layer1][layer2] == 1:
						topoLists[layer1][layer2] = 1
			break

	if len(topoLists) == 0:  #not found
		return []

	#check the network connectivity using DFS
	visited = [0 for i in range(sNum)]
	def DFS(v):
		visited[v] = 1
		for j in range(sNum):
			if topoLists[v][j] == 1 and visited[j] == 0:
				DFS(j)
	disconNode = []
	for i in range(sNum):
		if visited[i] == 0:
			DFS(i)
			disconNode.append(i)
	if len(disconNode) != 1:
		print("error")
		return []

	return topoLists

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

def countEdgesNum(topoLists):
	count = 0
	for i in range(len(topoLists)):
		for j in range(len(topoLists)):
			if topoLists[i][j] == 1:
				count += 1
	return count/2

def createDepotSetWithPolicy(depotNum, sNum, diction):
	geoLocationSet1 = ["Vancouver","LosAngeles","SanFrancisco","LasVegas","SaltLakeCity","Calgary","Seattle","Portland","Sacrameto","Phoenix","SanDiego"]
	geoLocationSet2 = ["ElPaso","Dallas","Houston","OklahomaCity","Minneapolis","KansasCity","Denver","Chicago","Indianapolis","Detroit","StLouis","Cincinnati","Memphis","Winnipeg"]
	geoLocationSet3 = ["Nashville","Cleveland","NewYork","Montreal","Charlotte","NewOrleans","Boston","Atlanta","Miami","WashingtonDC","Philadelphia","Toronto","Pittsburgh","Tampa"]
	
	depotSet = [0 for i in range(sNum)]
	depotNumforSet1 = depotNumforSet2 = depotNumforSet3 = depotNum/3
	restNum = depotNum%3
	if restNum == 1:
		depotNumforSet3 += 1
	elif restNum == 2:
		depotNumforSet1 += 1
		depotNumforSet3 += 1
	for i in range(depotNumforSet1):
		randomDepot = random.randint(0,len(geoLocationSet1)-1)
		while depotSet[diction[geoLocationSet1[randomDepot]]] == 1:
			randomDepot = random.randint(0,len(geoLocationSet1)-1)
		depotSet[diction[geoLocationSet1[randomDepot]]] = 1
	for i in range(depotNumforSet2):
		randomDepot = random.randint(0,len(geoLocationSet2)-1)
		while depotSet[diction[geoLocationSet2[randomDepot]]] == 1:
			randomDepot = random.randint(0,len(geoLocationSet2)-1)
		depotSet[diction[geoLocationSet2[randomDepot]]] = 1
	for i in range(depotNumforSet3):
		randomDepot = random.randint(0,len(geoLocationSet3)-1)
		while depotSet[diction[geoLocationSet3[randomDepot]]] == 1:
			randomDepot = random.randint(0,len(geoLocationSet3)-1)
		depotSet[diction[geoLocationSet3[randomDepot]]] = 1
	return depotSet

def calEulerPathNum(topoMatrix, sNum):
	count = 0
	for i in range(sNum):
		degreeSum = 0
		for j in range(sNum):
			degreeSum += topoMatrix[i][j]
		if degreeSum%2 == 1:
			count += 1
	if count == 0:
		count = 1
	else:
		count /= 2
	return count

if __name__ == '__main__':
	filename = sys.argv[1]
	sNum, dictional = transformSNDGraph(filename)
	depotSet = createDepotSetWithPolicy(12, sNum, dictional)    #set depotNum
	for i in range(50,721,10):        #various edgeNum
		topo = createRandomTopo(sNum, i)
		if len(topo) == 0 or countEdgesNum(topo) != i:
			print("error")
			break
		eulerPathCount = calEulerPathNum(topo, sNum)
		print(topo)
		print(depotSet)
		print(eulerPathCount)
