#include "stdafx.h"
#include "SpatialVisitor.h"


SpatialVisitor::SpatialVisitor() : _visitedNodes(0), _visitedData(0)
{
}


SpatialVisitor::~SpatialVisitor()
{
}

void SpatialVisitor::visitNode(const INode& node)
{
	//cout << "Node: " << node.getIdentifier() << " visited" << endl;
	_visitedNodes++;
}


void SpatialVisitor::visitData(const IData& data)
{
	//cout << "Data: " << data.getIdentifier() << " visited" << endl;
	_visitedData++;
}


void SpatialVisitor::visitData(vector<const IData*>& dataList)
{
	for_each(dataList.begin(), dataList.end(), [&](const IData* data) {
		visitData(*data);
	});
}

size_t SpatialVisitor::visitedNodes()
{
	return _visitedNodes;
}

size_t SpatialVisitor::visitedData()
{
	return _visitedData;
}

void SpatialVisitor::resetStatistics()
{
	_visitedNodes = 0;
	_visitedData = 0;
}
