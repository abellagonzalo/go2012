#include "ComponentSnapshot.h"

ComponentSnapshot::ComponentSnapshot ()
{
	_headerId = UNKNOWN_HEADER_ID;
	reset();
}

ComponentSnapshot::~ComponentSnapshot()
{
	_header.clear();
	_body.clear();
}

int
ComponentSnapshot::getHeaderId()
{
	return _headerId;
}

void
ComponentSnapshot::setHeaderId(int newHeaderId)
{
	_headerId = newHeaderId;
}

void
ComponentSnapshot::composeHeader (const string name, const vector<string>& labels, const vector<LogDataType>& types,
		const vector<short>& sizes, const vector<size_t>& numberOfValues)
{
	if (((labels.size() == types.size()) && (labels.size()== sizes.size())) && (labels.size() == numberOfValues.size())) {

		// Fill the header
		_header.clear();

		add( _headerId, &_header );
		add( name, &_header );
		add( labels.size(), &_header );

		for (unsigned int i = 0; i < labels.size(); i++) {
			add ( labels[i], &_header );
			add ( types[i], &_header );
			add ( sizes[i], &_header );
			add ( numberOfValues[i], &_header );
		}

		cerr << "ComponentSnapshot::composeHeader()\n";
	} else {
		//ToDo: use some value to avoid future exceptions returning void
		cerr << begin_red << "ComponentSnapshot::composeHeader() Number of labels (" << labels.size() <<
				"), types (" << types.size() << "), sizes (" << sizes.size() << ") or numberOfValues (" <<
				numberOfValues.size() << ") does not mach\n" << end_color;
		return;
	}
}

vector<char>*
ComponentSnapshot::getHeader()
{
	return &_header;
}

vector<char>*
ComponentSnapshot::getBody()
{
	return &_body;
}

void
ComponentSnapshot::reset()
{
	_body.clear();
}
