#include "SnapshotData.h"

SnapshotData::SnapshotData(SnapshotHeader *myHeader)
{
	_header = myHeader;
}

SnapshotData::~SnapshotData()
{

}



vector<char>
SnapshotData::getSnapshotBody()
{
	return _body;
}

void
SnapshotData::clear()
{
	_body.clear();
}
