#include "Attention.h"

AttentionClient::AttentionClient()
{
	attention = Attention::getInstance();
}

AttentionClient::~AttentionClient(){
	//ToDo: Free all memory
}
