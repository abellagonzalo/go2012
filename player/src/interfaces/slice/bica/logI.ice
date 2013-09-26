#ifndef LOGI_ICE
#define LOGI_ICE

#include "common.ice"

module bica {
 
	/** 
	* Interface to the log component
	*/
	interface LogProvider
	{
		void startLog( string filename, string comment );
		void stopLog();
		void logOn( string component );
		void logOff( string component );
	};
    
};

#endif //LOGI_ICE
