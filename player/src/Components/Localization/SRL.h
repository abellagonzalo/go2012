#ifndef SRL_H
#define SRL_H

#include "AbstractLocalization.h"
#include "WorldModel.h"
#include "MovementModel.h"
#include "Singleton.h"
#include "GameController.h"

#include "Perception.h"
#include "GoalDetector.h"

#include <IceE/IceE.h>
#include <debug.h>
#include "Debug.h"
#include <componentsI.h>

#include <pthread.h>

#include "alproxies/almotionproxy.h"

#include <vector>
#include "Vector2D.h"
#include "Pose.h"


#define MAX_NUM_PARTICLES 300

typedef struct
{
	float x;
	float y;
	float angle;
	float weight;
} Particle;


class SRL : public AbstractLocalization, public Singleton<SRL>, public DebugIRel, public DebugIAbs
{
	public:
		SRL();
		virtual ~SRL();

		void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
		void step();
		void run();

		//virtual void updateWorldFeatures(const WorldFeatures & worldFeatures);
		//virtual void reset(Localization::ResetCase resetCase);
		//virtual void resetFromPenalty();
		//SPECIAL CASE FUNCTIONS
		void resetUniform();
		//void resetPenaltyBox(bool ourPenaltyBox);
		//void resetLifted();
		//void resetFromFall();

		int getNumParticles(void);
		Particle getParticle(int num);

		bica::ShapeList getGrDebugAbs();
		bica::ShapeList getGrDebugRel();

		float getReliability();
		void resetToPosition(int state);
		void getPosition(double &xpos, double &ypos, double &thetapos);
		void getPosition(vector<tRobotHipothesis> &hipotheses);
		string getInfo();
		bica::ImageDataPtr getImgDebug();

		void setInfo(MotionInfo &motion, PerceptionInfo &perception);
		void UpdateInfo();

	protected:
		virtual void motionUpdate(Particle* p);
		virtual bool isOutOfBounds(Particle* p);
		//virtual void gameControllerUpdate(const GameState & gameState);
		
		virtual void visionUpdate(Particle* p);
		
		float getRandomNoise(float std_dev);
		float getGoalPostsProb(Particle* p);
		//void getClosestCorner(int type, Vector2D visionGlobalPos,
		//		Vector2D & landPos, float & landAngle);
		float getLandmarkWeight(Particle* p, Vector2D obs, Vector2D lm, float ang_std_dev, float dist_std_dev);
		float getProb(float x, float std_dev);

		float getGoalPostDistanceStdDev(Vector2D & rel);
		float getGoalPostAngleStdDev(Vector2D & rel);
		/*float getCornerDistanceStdDev(Vector2D & rel);
		float getCornerAngleStdDev(Vector2D & rel);
		float getCircleDistanceStdDev(Vector2D & rel);
		float getCircleAngleStdDev(Vector2D & rel);
		float getLineDistanceStdDev(Vector2D & rel);
		float getLineAngleStdDev(Vector2D & rel);*/

		void normalizeWeights();
		inline void swapParticles(int p1, int p2);
		int partitionParticleWeights(int left, int right, int pivot);
		void selectParticle(int left, int right, int k);
		float findKthHighestWeight(int k);
		void applyNoiseToParticle(Particle* p, float std_dev_x, float std_dev_y, float std_dev_theta);
		void applyNoiseToReading(Vector2D & rel, float dist_std_dev, float ang_std_dev);

		virtual void generateParticleAtRandom(Particle* p);
		virtual void resample();
		// computes mean position, direction and weight within a circle
		void computeLocalStatistics(Pose & pose, Vector2D* mean_pos,
					Vector2D* mean_dir, float* total_weight);
		Pose getPose();

		int generateParticleFromVision(Particle* p);
		int findBestTwoLandmarks(Vector2D & landmark1, Vector2D & relPos1, Vector2D & landmark2, Vector2D & relPos2);
		int findPoseFromTwoLandmarks(Particle* p, Vector2D landmark1, Vector2D relPos1, Vector2D landmark2, Vector2D relPos2);
		int findPoseFromOneLandmark(Particle* p, Vector2D landmark, Vector2D relPos);

		void scatterAboutPoint(Particle *p, float x, float y, float ang);
		
		void compute_cdf(void);
		int choose_particle_at_random(void);

		Particle particles[MAX_NUM_PARTICLES];
		Pose pose;
		int num_particles;
		float w_slow, w_fast;

		float* particle_cdf;

		WorldModel *_WorldModel;
		MovementModel  *_MovementModel;

		GoalDetector *_GoalDetector;

		AL::ALPtr<AL::ALMotionProxy> pmotion;
		vector<float> lastpos;

		int GameEvent;
		MotionInfo _motion;
		PerceptionInfo _perception;

};

#endif

