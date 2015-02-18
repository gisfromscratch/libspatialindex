#pragma once

#include <spatialindex/SpatialIndex.h>

using namespace SpatialIndex;
using namespace std;

class SpatialVisitor : public IVisitor
{
public:
	SpatialVisitor();
	~SpatialVisitor();

public:
	void visitNode(const INode& node) override;

	void visitData(const IData& data) override;

	void visitData(vector<const IData*>& dataList) override;

	size_t visitedNodes();

	size_t visitedData();

	void resetStatistics();

private:
	size_t _visitedNodes;
	size_t _visitedData;
};

