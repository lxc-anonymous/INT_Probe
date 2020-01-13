#!/usr/bin/python
import random
import sys

def createRandomTopo(sNum):
	sys.setrecursionlimit(1000000)
	#create adjaMatrix
	topoMatrix = [[0 for i in range(sNum)] for i in range(sNum)]
	visited = [0 for i in range(sNum)]
	#create topo randomly
	for i in range(sNum):
		for j in range(i+1, sNum):
			link = random.randint(0,1)
			topoMatrix[i][j] = link
			topoMatrix[j][i] = link
	#DFS
	def DFS(v):
		visited[v] = 1
		for j in range(sNum):
			if topoMatrix[v][j] == 1 and visited[j] == 0:
				DFS(j)
	#check the network connectivity using DFS
	disconNode = []
	for i in range(sNum):
		if visited[i] == 0:
			DFS(i)
			disconNode.append(i)
	#if the network is unconnected, connect each disconNode
	for i in range(len(disconNode)-1):
		topoMatrix[disconNode[i]][disconNode[i+1]] = 1
		topoMatrix[disconNode[i+1]][disconNode[i]] = 1

	oddCount = calOddNum(topoMatrix, sNum)

	return topoMatrix, oddCount

def calOddNum(topoMatrix, sNum):
	count = 0
	for i in range(sNum):
		degreeSum = 0
		for j in range(sNum):
			degreeSum += topoMatrix[i][j]
		if degreeSum%2 == 1:
			count += 1
	return count

def createDepotSet(depotNum, sNum):
	depotSet = [0 for i in range(sNum)]
	for i in range(depotNum):
		randomDepot = random.randint(0,sNum-1)
		while depotSet[randomDepot] == 1:
			randomDepot = random.randint(0,sNum-1)
		depotSet[randomDepot] = 1
	return depotSet

def makeOddArray(sNum, topo):
	oddArray = [0 for i in range(sNum)]
	for i in range(sNum):
		degreeSum = 0
		for j in range(sNum):
			degreeSum += topo[i][j]
		if degreeSum%2 == 1:
			oddArray[i] = 1
	return oddArray

def createDepotSetWithOddDensity(depotNum, sNum, oddArray, oddInDepotNum, oddNum):
	depotSet = [0 for i in range(sNum)]
	oddInDepotCnt = 0
	oddRandArr = []
	# find odds in depotSet, the total number is oddInDepotNum
	while oddInDepotCnt < oddInDepotNum:
		randomNum = random.randint(1, oddNum)
		if randomNum not in oddRandArr:
			oddRandArr.append(randomNum)
			oddInDepotCnt += 1
	if len(oddRandArr) != oddInDepotNum:
		return []
	oddOrderNum = 0
	for i in range(sNum):
		if oddArray[i] == 1:
			oddOrderNum += 1
			if oddOrderNum in oddRandArr:
				depotSet[i] = 1
	# verify the odd in depotSet
	c = 0
	for i in range(len(depotSet)):
		if depotSet[i] == 1:
			if oddArray[i] == 1:
				c += 1
			else:
				return []
	if c != oddInDepotNum:
		return []

	# select other vertice in depot randomly
	for i in range(depotNum - oddInDepotNum):
		randomDepot = random.randint(0,sNum-1)
		while depotSet[randomDepot] == 1:
			randomDepot = random.randint(0,sNum-1)
		depotSet[randomDepot] = 1
	# verify the depotNum
	cc = 0
	for i in range(len(depotSet)):
		if depotSet[i] == 1:
			cc += 1
	if cc != depotNum:
		return []
	return depotSet

if __name__ == '__main__':
	
	# generate solution A
#	for i in range(20, 151, 2):        #various sNum
	for i in range(110, 513, 2):        #various sNum
		topo, oddCount = createRandomTopo(i)
#		depotSet = createDepotSet(12, i)     #set depotNum
#		depotSet = createDepotSet(24, i)     #set depotNum
#		depotSet = createDepotSet(48, i)     #set depotNum
		depotSet = createDepotSet(96, i)     #set depotNum
		if oddCount == 0:
			eulerPathCount = 1
		else:
			eulerPathCount = oddCount/2
		print(topo)
		print(depotSet)
		print(eulerPathCount)
	
	'''
	# generate solution B
	sNum = 256
	depotNum = 128
	topo, oddCount = createRandomTopo(sNum)
	oddArray = makeOddArray(sNum, topo)
	if oddCount <= 125:
		for i in xrange(0,11,1):
			oddDensityInDepot = float(i/10.0)
			oddInDepotNum = int(oddCount * oddDensityInDepot)
			depotSet = createDepotSetWithOddDensity(depotNum, sNum, oddArray, oddInDepotNum, oddCount)
			if not depotSet:
				print "error"
			if oddCount == 0:
				eulerPathCount = 1
			else:
				eulerPathCount = oddCount/2
			print topo
			print depotSet
			print eulerPathCount
	'''
