#pragma once

#include "GeometryFactory.h"

using namespace FileGDBAPI;
using namespace SpatialIndex;
using namespace std;

class RowStream : public IDataStream
{
public:
	RowStream(Table *table);
	virtual ~RowStream();

	virtual IData* getNext();

	virtual bool hasNext();

	virtual uint32_t size();

	virtual void rewind();

	void readNextRow();

private:
	const double Eps = 1e-3;

	GeometryFactory _geometryFactory;

	Table *_table;
	EnumRows _rows;
	RTree::Data *_nextData;
};

