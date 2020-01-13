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
	topo, sNum, dictional = transformSNDGraph(filename)
	count = countEdgesNum(topo)
	eulerPathCount = calEulerPathNum(topo, sNum)
	for i in range(3,34,1):          #various depotNum
		depotSet = createDepotSetWithPolicy(i, sNum, dictional)
		print(topo)
		print(depotSet)
		print(eulerPathCount)
