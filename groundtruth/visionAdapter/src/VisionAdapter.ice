/*
 * Name: VisionAdapter.ice
 * @Author: Gonzalo Abella (abellagonzalo@gmail.com)
 * Updated: 25/01/2011 by Gonzalo Abella
 *
 */

#include <localizationI.ice>
#include <componentsI.ice>

module bica {

	struct GTRobot {
		int patternId;
		string robot;
		string team;
		LocalizationProvider* localization;
		BallDetectorManager* ballDetector;
	};
	sequence<GTRobot> GTRobots;

	interface VisionAdapter
	{
		idempotent void start(int freq);
		idempotent void stop();

		void addRobot(GTRobot robot);
		void removeRobot(GTRobot robot);
		GTRobots getRobots();
	};

};
