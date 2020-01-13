#! /usr/bin/python
import sys
import random

def transformSNDGraph(filename):
	topoLists = []
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
				for i in range(sNum):
					oneRow = [0 for i in range(sNum)]
					topoLists.append(oneRow)
			elif textLine.startswith("DEMANDS"):
				flag = 3
			elif len(textLine)>2 and textLine[0] == " ":
				#print(textLine)
				if flag == 1:   #parse nodes
					stringSplit = textLine.split()
					diction[stringSplit[0]] = sNum
					sNum += 1
				elif flag == 2:   #parse links
					stringSplit = textLine.split()
					x = diction[stringSplit[2]]
					y = diction[stringSplit[3]]
					topoLists[x][y] = 1
					topoLists[y][x] = 1
				elif flag == 3:
					break;
		else:
			break
	file.close()
	return topoLists, sNum, diction

def countEdgesNum(topoLists):
	count = 0
	for i in range(len(topoLists)):
		for j in range(len(topoLists)):
			if topoLists[i][j] == 1:
				count += 1
	return count/2

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

def findOdds(topoMatrix, sNum, diction):
	count = 0
	oddsSet = {}
	for i in range(sNum):
		degreeSum = 0
		for j in range(sNum):
			degreeSum += topoMatrix[i][j]
		if degreeSum%2 == 1:
			count += 1
			for key, val in diction.items():
				if val == i:
					oddsSet[i] = key
					break
	if len(oddsSet) != count:
		return {}
	return oddsSet

def createDepotSetWithDifferentPolicy(sNum, diction, policyFlag, depotNum):
	'''
	policyFlag == 1 : select all vertices
	policyFlag == 2 : select dds
	policyFlag == 3 : select evens
	'''
	geoLocationSet1 = ["Vancouver","LosAngeles","SanFrancisco","LasVegas","SaltLakeCity","Calgary","Seattle","Portland","Sacrameto","Phoenix","SanDiego","ElPaso","Dallas","Houston","OklahomaCity","Minneapolis","KansasCity","Denver","Chicago","Indianapolis","Detroit","StLouis","Cincinnati","Memphis","Winnipeg","Nashville","Cleveland","NewYork","Montreal","Charlotte","NewOrleans","Boston","Atlanta","Miami","WashingtonDC","Philadelphia","Toronto","Pittsburgh","Tampa"]
	geoLocationSet2 = ["Calgary","Portland","SaltLakeCity","SanFrancisco","Sacrameto","Phoenix","LosAngeles","Minneapolis","Denver","ElPaso","Dallas","Houston","Detroit","Toronto","WashingtonDC","Tampa"]
	geoLocationSet3 = ["Vancouver","LasVegas","Seattle","SanDiego","OklahomaCity","KansasCity","Chicago","Indianapolis","StLouis","Cincinnati","Memphis","Winnipeg","Nashville","Cleveland","NewYork","Montreal","Charlotte","NewOrleans","Boston","Atlanta","Miami","Philadelphia","Pittsburgh"]


	depotSet = [0 for i in range(sNum)]

	geoLocationSet = []
	if policyFlag == 1:
		geoLocationSet = geoLocationSet1
	elif policyFlag == 2:
		geoLocationSet = geoLocationSet2
	elif policyFlag == 3:
		geoLocationSet = geoLocationSet3

	for i in range(depotNum):
		randomDepot = random.randint(0,len(geoLocationSet)-1)
		while depotSet[diction[geoLocationSet[randomDepot]]] == 1:
			randomDepot = random.randint(0,len(geoLocationSet)-1)
		depotSet[diction[geoLocationSet[randomDepot]]] = 1
	return depotSet

if __name__ == '__main__':
	filename = sys.argv[1]
	topo, sNum, dictional = transformSNDGraph(filename)
	count = countEdgesNum(topo)
	oddsSet = findOdds(topo, sNum, dictional)

	eulerPathCount = calEulerPathNum(topo, sNum)

	for i in range(16):
		depotSet1 = createDepotSetWithDifferentPolicy(sNum, dictional, 1, i+1)
		print topo
		print depotSet1
		print eulerPathCount

		# depotSet2 = createDepotSetWithDifferentPolicy(sNum, dictional, 2, i+1)
		# print topo
		# print depotSet2
		# print eulerPathCount

		# depotSet3 = createDepotSetWithDifferentPolicy(sNum, dictional, 3, i+1)
		# print topo
		# print depotSet3
		# print eulerPathCount
