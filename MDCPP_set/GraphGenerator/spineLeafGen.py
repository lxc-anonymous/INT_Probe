#!/usr/bin/python
import sys
import random

def genSpineLeaf(swSum):  #SNum must be 3,6,9,12,15...
	sys.setrecursionlimit(1000000)

	L1 = int(swSum/3)
	L2 = L1*2

	topoList = [[0 for i in range(swSum)] for i in range(swSum)]

	for i in range(L1):
		for j in range(L1, swSum):
			topoList[i][j] = 1
			topoList[j][i] = 1

	return topoList

def createDepotSetForSpineLeaf(sNum, density):
	depotSet = [0 for i in range(sNum)]
	L1 = int(sNum/3)
	depotMax = L1*2
	depotNum = int(depotMax * density)
	start = L1
	end = sNum -1
	for i in range(depotNum):
		randomDepot = random.randint(start,end)
		while depotSet[randomDepot] == 1:
			randomDepot = random.randint(start,end)
		depotSet[randomDepot] = 1

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
	den = 0.2         #set depot density
	for i in range(2, 390, 9):      #set podsNum
		s = 3*i        #SNum must be larger than 3
		topo = genSpineLeaf(s)
		depotSet = createDepotSetForSpineLeaf(s, den)
		eulerPathCount = calEulerPathNum(topo, s)
		print(topo)
		print(depotSet)
		print(eulerPathCount)
