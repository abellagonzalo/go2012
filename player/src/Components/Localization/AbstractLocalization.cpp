#include "AbstractLocalization.h"

AbstractLocalization::AbstractLocalization()
{
	_GameController = GameController::getInstance();
}

AbstractLocalization::~AbstractLocalization()
{
}

void
AbstractLocalization::reset()
{
	this->resetLocalization = 1;
}

int
AbstractLocalization::getPlayerColor() {
	return (int) _GameController->getMyColor();
}
