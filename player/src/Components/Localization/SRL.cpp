#include "SRL.h"


using namespace std;

#define DRAW_POSE
//#define DRAW_PARTICLES
#define DRAW_CORNERS
#define DRAW_LINES
#define DRAW_POSTS
//#define DRAW_FREE_SPACE

#define USE_GOALS
#define USE_CORNERS
#define USE_CIRCLES
#define USE_LINES

#define MIN_NUM_PARTICLES 30
#define SAMPLE_WEIGHT_THRESHOLD 0.5

//Goal post landmarks
#define GOAL_POST_VISIBLE_CONF 0.5
#define GOAL_POST_DISTANCE_MIN_STD_DEV 30
#define GOAL_POST_DISTANCE_MAX_STD_DEV 100
#define GOAL_POST_ANGLE_MIN_STD_DEV 10*M_PI/180
#define GOAL_POST_ANGLE_MAX_STD_DEV 15*M_PI/180
#define GOAL_POST_MAX_STD_DEV_DISTANCE 400

//Corner landmarks
#define CORNER_DISTANCE_MIN_STD_DEV 10
#define CORNER_DISTANCE_MAX_STD_DEV 50
#define CORNER_ANGLE_MIN_STD_DEV 10*M_PI/180
#define CORNER_ANGLE_MAX_STD_DEV 10*M_PI/180
#define CORNER_GLOBAL_ANGLE_STD_DEV 10*M_PI/180
#define CORNER_MAX_STD_DEV_DISTANCE 200

#define CIRCLE_DISTANCE_MIN_STD_DEV 10
#define CIRCLE_DISTANCE_MAX_STD_DEV 30
#define CIRCLE_ANGLE_STD_DEV 10 * M_PI / 180
#define CIRCLE_MAX_STD_DEV_DISTANCE 200

//Line landmarks
#define LINE_DISTANCE_MIN_STD_DEV 40
#define LINE_DISTANCE_MAX_STD_DEV 100
#define LINE_ANGLE_MIN_STD_DEV 30*M_PI/180
#define LINE_ANGLE_MAX_STD_DEV 30*M_PI/180
#define LINE_GLOBAL_ANGLE_STD_DEV 10*M_PI/180
#define LINE_MAX_STD_DEV_DISTANCE 200
#define LINE_MIN_LENGTH 10
#define LINE_MAX_DISTANCE 200

//Line landmarks

//Standard deviations for the motion model
#define USE_MOTION_NOISE
// error per cm
#define MOTION_UNIT_STD_DEV_X 0.3
#define MOTION_UNIT_STD_DEV_Y 0.3
// error in degrees per cm
#define MOTION_UNIT_STD_DEV_ANGLE_PER_Y (1)
// error per degree
#define MOTION_UNIT_STD_DEV_ANGLE (0.4*M_PI/180)

//Resample constants
#define RESAMPLE_UNIFORM_PROB 0.00
#define RESAMPLE_STD_DEV_X 8.0
#define RESAMPLE_STD_DEV_Y 8.0
#define RESAMPLE_STD_DEV_ANGLE 10*M_PI/180

#define FALL_STD_DEV_DIST 20.0

#define ALPHA_SLOW 0.05
#define ALPHA_FAST 0.06

//If confidence is below this threshold we are "lost"
#define LOST_CONFIDENCE 0.3
// deviations in position to determine confidence
#define ZERO_CONFIDENCE_DEVIATION 220.0
#define FULL_CONFIDENCE_DEVIATION 20.0

// average particles within this distance of maximum confidence
#define MIN_POSE_MEAN_DIST 40
#define MAX_JUMP_DIST 30
#define MAX_JUMP_ANGLE (30.0 / 180.0 * M_PI)
#define JUMP_WEIGHT_THRESHOLD 0.1

SRL::SRL()
: pose(), num_particles(MAX_NUM_PARTICLES), w_slow(0.01), w_fast(0.01)
{
	_MovementModel = MovementModel::getInstance();
	_WorldModel = WorldModel::getInstance();

	_GoalDetector = GoalDetector::getInstance();

	resetUniform();
	//resetKickoff();
	particle_cdf = (float*)malloc(sizeof(float) * MAX_NUM_PARTICLES);
}

SRL::~SRL(){
	free(particle_cdf);
}

void SRL::init(const string newName,
		AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	setFreqTime(MEDIUM_RATE);

	try
	{
		pmotion = parentBroker->getMotionProxy();
		lastpos = pmotion->getRobotPosition(USE_SENSOR);
	} catch (AL::ALError& e) {
		lastpos.push_back(0.0);
		lastpos.push_back(0.0);
		lastpos.push_back(0.0);
		lastpos.push_back(0.0);
		lastpos.push_back(0.0);
		lastpos.push_back(0.0);
		cerr << "NEKFLocalization::NEKFLocalization [motion]" << e.toString() << endl;
	}


}
void SRL::UpdateInfo()
{
	//motion
	float diffx, diffy, difft;

	bool apply;

	vector<float> pos = pmotion->getRobotPosition(USE_SENSOR);

	diffx = pos[0] *1000.0 - lastpos[0]*1000.0;
	diffy = pos[1] *1000.0 - lastpos[1]*1000.0;
	difft = normalizePi(pos[2] - lastpos[2]);

	_motion.movx = diffx*cos(-lastpos[2]) - diffy*sin(-lastpos[2]);
	_motion.movy = diffx*sin(-lastpos[2]) + diffy*cos(-lastpos[2]);
	_motion.movt = difft;

	lastpos = pos;

	//perception info
	ObjectState* goalp0r = &(_GoalDetector->goals->p0RightEstimate);
	ObjectState* goalp0l = &(_GoalDetector->goals->p0LeftEstimate);
	ObjectState* goalp1r = &(_GoalDetector->goals->p1RightEstimate);
	ObjectState* goalp1l = &(_GoalDetector->goals->p1LeftEstimate);

	_perception.p0ElapsedTimeSinceLastObs = _GoalDetector->getObj()->p0ElapsedTimeSinceLastObs;
	_perception.p1ElapsedTimeSinceLastObs = _GoalDetector->getObj()->p1ElapsedTimeSinceLastObs;

	_perception.p0r.reliability = goalp0r->getReliability();
	_perception.p0r.x = goalp0r->getPositionInRelativeCoordinates().x;
	_perception.p0r.y = goalp0r->getPositionInRelativeCoordinates().y;

	_perception.p0l.reliability = goalp0l->getReliability();
	_perception.p0l.x = goalp0l->getPositionInRelativeCoordinates().x;
	_perception.p0l.y = goalp0l->getPositionInRelativeCoordinates().y;

	_perception.p1r.reliability = goalp1r->getReliability();
	_perception.p1r.x = goalp1r->getPositionInRelativeCoordinates().x;
	_perception.p1r.y = goalp1r->getPositionInRelativeCoordinates().y;

	_perception.p1l.reliability = goalp1l->getReliability();
	_perception.p1l.x = goalp1l->getPositionInRelativeCoordinates().x;
	_perception.p1l.y = goalp1l->getPositionInRelativeCoordinates().y;

}

void SRL::setInfo(MotionInfo &motion, PerceptionInfo &perception)
{
	memcpy(&_motion, &motion, sizeof(motion));
	memcpy(&_perception, &perception, sizeof(perception));
}

void SRL::step(void) {


	_GoalDetector->step();

	if (isTime2Run()) {
		startDebugInfo();
		UpdateInfo();

		run();
		endDebugInfo();
		//cerr<<"("<<getPose().getPosition().x<<", "<<getPose().getPosition().y<<", "<<getPose().getAngle()<<") "<<getPose().getConfidence()<<endl;
	}
}

void SRL::run()
{
	for(int i=0; i < getNumParticles(); i++)
		motionUpdate(&particles[i]);


	int valid_landmarks = 0;


	if(_perception.p0ElapsedTimeSinceLastObs < ObjectState::LONG_TIME)
	{
		if(_perception.p0l.reliability >= ObjectState::HIGH_RELIABLE)
			valid_landmarks++;
		if(_perception.p0r.reliability >= ObjectState::HIGH_RELIABLE)
			valid_landmarks++;
	}
	if(_perception.p1ElapsedTimeSinceLastObs < ObjectState::LONG_TIME)
	{
		if(_perception.p1l.reliability >= ObjectState::HIGH_RELIABLE)
			valid_landmarks++;
		if(_perception.p1r.reliability >= ObjectState::HIGH_RELIABLE)
			valid_landmarks++;
	}


	for(int i=0; i < getNumParticles(); i++)
	{
		visionUpdate(&particles[i]);
	}

	if (valid_landmarks == 0)
	{
		normalizeWeights();
		pose = getPose();
		return;
	}

	resample();
	normalizeWeights();

	pose = getPose();

	return;
}

/*void SRL::setScanningForGoals(bool scanning)
{
	if (!scanning)
	{
		useWorldModel = true;
	}
}

void SRL::updateWorldFeatures(const WorldFeatures & worldFeatures)
{
	if (!useWorldModel)
		return;
	useWorldModel = false;
	// this apparently only works when close to goal posts
	return;

	std::vector<Vector2D> land1, rel1, land2, rel2;
	GoalWorldObject const * yellow = dynamic_cast<GoalWorldObject const *>(worldFeatures.getFirstWorldObject(WorldObject::YellowGoal));
	GoalWorldObject const * blue = dynamic_cast<GoalWorldObject const *>(worldFeatures.getFirstWorldObject(WorldObject::BlueGoal));

	// find the best landmarks
	if (yellow != NULL && yellow->isValid())
	{
		rel1.push_back(yellow->getLeftGoalPostLocal());
		land1.push_back(field.getYellowGoalPostLeft());
		rel2.push_back(yellow->getLeftGoalPostLocal());
		land2.push_back(field.getYellowGoalPostRight());
	}
	if (blue != NULL && blue->isValid())
	{
		rel1.push_back(blue->getLeftGoalPostLocal());
		land1.push_back(field.getBlueGoalPostLeft());
		rel2.push_back(blue->getLeftGoalPostLocal());
		land2.push_back(field.getBlueGoalPostRight());
	}
	// TODO: one post from each goal

	if (rel1.size() == 0)
		return;

	LOG_INFO("Updating based on world features!\n");
	float reset_prob = 1 - w_fast / w_slow;
	if (reset_prob < 0.0)
		reset_prob = 0.0;

	for (int i = 0; i < getNumParticles(); i++)
	{
		if ((float)rand() / RAND_MAX < reset_prob)
		{
			// choose random position
			int r = (int)(((float)rand() / RAND_MAX) * rel1.size());
			findPoseFromTwoLandmarks(&particles[i], land1[r], rel1[r], land2[r], rel2[r]);
			applyNoiseToParticle(&(particles[i]), RESAMPLE_STD_DEV_X, RESAMPLE_STD_DEV_Y, RESAMPLE_STD_DEV_ANGLE);
		}
	}
}

void SRL::reset(Localization::ResetCase resetCase) {
  if (resetCase == Localization::ourPenaltyBox) {
    resetPenaltyBox(true);
  } else if (resetCase == Localization::opponentPenaltyBox) {
    resetPenaltyBox(false);
  } else if (resetCase == Localization::goalieStartInPenaltyBox) {
    resetGoalieForKickoff();
  } else if (resetCase == Localization::opponentGoalieStartInPenaltyBox) {
    resetOpponentGoalieForKickoff();
  } else if (resetCase == Localization::backFromPenalty) {
    resetFromPenalty();
  } else if (resetCase == Localization::fall) {
    resetFromFall();
  } else if (resetCase == Localization::lifted) {
    resetLifted();
  }
  else if (resetCase == Localization::dribblerPosition) {
    resetDribbler();
  }
  else if (resetCase == Localization::goaliePenaltyKick) {
    resetGoaliePenaltyKick();
  }
  else if (resetCase == Localization::attackerPenaltyKick) {
    resetAttackerPenaltyKick();
  }
}
 */

/*
void SRL::saveDensityDistribution(const VisionFeatures & visionFeatures, const char* filename)
{
	FILE* f = fopen(filename, "w");
	if (f == NULL)
	{
		fprintf(stderr, "Failed to open file %s.\n", filename);
		return;
	}

	for (int j = 0; j < 400; j+=5)
	{
		float y = (-j * 0.01 + 2.0 - 0.005) * 100;
		for (int i = 0; i < 600; i+=5)
		{
			float highest = 0.0;
			float x = (i * 0.01 - 3.0 + 0.005) * 100;
			Particle p;
			p.x = x;
			p.y = y;
			// find angle with highest confidence
			for (int t = 0; t < 20; t++)
			{
				p.weight = 1.0;
				p.angle = t / 20.0 * 2 * M_PI;
				visionUpdate(&p, visionFeatures);
				if (p.weight > highest)
					highest = p.weight;
			}
			fprintf(f, "%g ", highest);
		}
		fprintf(f, "\n");
	}
	fclose(f);
}
 */
void SRL::motionUpdate(Particle* p)
{
	Vector2D translation;
	float rotation;

	translation.x = _motion.movx;
	translation.y = _motion.movy;
	rotation = _motion.movt;
	//TODO: don't apply noise here, apply in resample?
	//TODO: the standard deviation should be different if that variable was supposed to be changed or not
	translation.x += getRandomNoise(translation.x * MOTION_UNIT_STD_DEV_X);
	translation.y += getRandomNoise(translation.y * MOTION_UNIT_STD_DEV_Y);
	rotation += getRandomNoise(translation.y * M_PI / 180.0 * MOTION_UNIT_STD_DEV_ANGLE_PER_Y);
	rotation += getRandomNoise(rotation * 180.0 / M_PI * MOTION_UNIT_STD_DEV_ANGLE);

	float ca = cos(p->angle);
	float sa = sin(p->angle);
	p->x += translation.x * ca - translation.y * sa;
	p->y += translation.x * sa + translation.y * ca;
	p->angle += rotation;
	while (fabs(p->angle) > M_PI)
		p->angle = p->angle - (sign(p->angle)*2*M_PI);

	// TODO: don't change randomly, put on the boundary
	if (isOutOfBounds(p))
		generateParticleAtRandom(p);
}

bool SRL::isOutOfBounds(Particle* p)
{
	return fabs(p->x) > 6000.0 / 2.0 ||
			fabs(p->y) > 4000.0 / 2.0;
}

float SRL::getRandomNoise(float std_dev)
{
	//Assumes Gaussian noise with mean of 0 and given standard deviation

	//Uses Box-Muller transform to turn a pair of uniform random numbers into a pair of gaussian random numbers
	float u1 = rand()/((float)RAND_MAX);
	float u2 = rand()/((float)RAND_MAX);
	float z1 = sqrt(-2*logf(u1)) * sin(2*M_PI*u2);
	//float z2 = sqrt(-2*log(u1)) * cos(2*M_PI*u2);
	float x1 = z1 * std_dev;
	//float x2 = z2 * std_dev;
	return x1;
}

float SRL::getGoalPostsProb(Particle* p)
{


	HPoint3D *lmbl,*lmbr,*lmyl,*lmyr;
	lmbl = &(_WorldModel->getPoint("p49")->p);
	lmbr = &(_WorldModel->getPoint("p51")->p);
	lmyl = &(_WorldModel->getPoint("p55")->p);
	lmyr = &(_WorldModel->getPoint("p53")->p);

	float prob = 1.0;

	if(_perception.p0ElapsedTimeSinceLastObs < ObjectState::LONG_TIME)
	{
		Vector2D official_pos;
		Vector2D perceived_pos;


		if(_perception.p0l.reliability >= ObjectState::HIGH_RELIABLE)
		{
			official_pos.x = lmbl->X;
			official_pos.y = lmbl->Y;
			perceived_pos.x = _perception.p0l.x;
			perceived_pos.y = _perception.p0l.y;

			float w = getLandmarkWeight(p, perceived_pos, official_pos,
					getGoalPostAngleStdDev(perceived_pos), getGoalPostDistanceStdDev(perceived_pos));
			prob *= w;
		}


		if(_perception.p0r.reliability >= ObjectState::HIGH_RELIABLE)
		{
			official_pos.x = lmbr->X;
			official_pos.y = lmbr->Y;
			perceived_pos.x = _perception.p0r.x;
			perceived_pos.y = _perception.p0r.y;

			float w = getLandmarkWeight(p, perceived_pos, official_pos,
					getGoalPostAngleStdDev(perceived_pos), getGoalPostDistanceStdDev(perceived_pos));
			prob *= w;
		}
	}
	if(_perception.p1ElapsedTimeSinceLastObs < ObjectState::LONG_TIME)
	{
		Vector2D official_pos;
		Vector2D perceived_pos;

		if(_perception.p1l.reliability >= ObjectState::HIGH_RELIABLE)
		{
			official_pos.x = lmyl->X;
			official_pos.y = lmyl->Y;
			perceived_pos.x = _perception.p1l.x;
			perceived_pos.y = _perception.p1l.y;

			float w = getLandmarkWeight(p, perceived_pos, official_pos,
					getGoalPostAngleStdDev(perceived_pos), getGoalPostDistanceStdDev(perceived_pos));
			prob *= w;
		}

		if(_perception.p1r.reliability >= ObjectState::HIGH_RELIABLE)
		{
			official_pos.x = lmyr->X;
			official_pos.y = lmyr->Y;
			perceived_pos.x = _perception.p1r.x;
			perceived_pos.y = _perception.p1r.y;

			float w = getLandmarkWeight(p, perceived_pos, official_pos,
					getGoalPostAngleStdDev(perceived_pos), getGoalPostDistanceStdDev(perceived_pos));
			prob *= w;
		}
	}

	return prob;
}
/*
void SRL::getClosestCorner(int type, Vector2D visionGlobalPos,
			Vector2D & landPos, float & landAngle)
{
	switch (type)
	{
		case CornerVisionObject::L:
			if (fabs(visionGlobalPos.x) < 3000.0 - field.getPenaltyBoxLength() / 2)
			{
				if (visionGlobalPos.y < 0.0)
				{
					if (visionGlobalPos.x < 0.0)
					{
						landPos.x = -(3000.0 - field.getPenaltyBoxLength());
						landPos.y = -field.getHalfPenaltyBoxWidth();
						landAngle = 3 * M_PI / 4;
					}
					else
					{
						landPos.x = (3000.0 - field.getPenaltyBoxLength());
						landPos.y = -field.getHalfPenaltyBoxWidth();
						landAngle = M_PI / 4;
					}
				}
				else
				{
					if (visionGlobalPos.x < 0.0)
					{
						landPos.x = -(3000.0 - field.getPenaltyBoxLength());
						landPos.y = field.getHalfPenaltyBoxWidth();
						landAngle = -3 * M_PI / 4;
					}
					else
					{
						landPos.x = (3000.0 - field.getPenaltyBoxLength());
						landPos.y = field.getHalfPenaltyBoxWidth();
						landAngle = -M_PI / 4;
					}
				}
			}
			else
			{
				if (visionGlobalPos.x < 0.0)
				{
					if (visionGlobalPos.y < 0.0)
					{
						landPos.x = -3000.0;
						landPos.y = -2000.0;
						landAngle = M_PI / 4;
					}
					else
					{
						landPos.x = -3000.0;
						landPos.y = 2000.0;
						landAngle = -M_PI / 4;
					}
				}
				else
				{
					if (visionGlobalPos.y < 0.0)
					{
						landPos.x = 3000.0;
						landPos.y = -2000.0;
						landAngle = 3 * M_PI / 4;
					}
					else
					{
						landPos.x = 3000.0;
						landPos.y = 2000.0;
						landAngle = -3 * M_PI / 4;
					}
				}
			}
			break;
		case CornerVisionObject::T:
			// on goal box
			if (fabs(visionGlobalPos.x) > 3000.0 / 2)
			{
				if (visionGlobalPos.y < 0.0)
				{
					if (visionGlobalPos.x < 0.0)
					{
						landPos.x = -3000.0;
						landPos.y = -field.getHalfPenaltyBoxWidth();
						landAngle = 0.0;
					}
					else
					{
						landPos.x = 3000.0;
						landPos.y = -field.getHalfPenaltyBoxWidth();
						landAngle = M_PI;
					}
				}
				else
				{
					if (visionGlobalPos.x < 0.0)
					{
						landPos.x = -3000.0;
						landPos.y = field.getHalfPenaltyBoxWidth();
						landAngle = 0.0;
					}
					else
					{
						landPos.x = 3000.0;
						landPos.y = field.getHalfPenaltyBoxWidth();
						landAngle = M_PI;
					}
				}
			}
			else
			{
				// on the center line
				if (visionGlobalPos.y < 0.0)
				{
					landPos.x = 0.0;
					landPos.y = -2000.0;
					landAngle = M_PI / 2;
				}
				else
				{
					landPos.x = 0.0;
					landPos.y = 2000.0;
					landAngle = -M_PI / 2;
				}
			}
			break;
		case CornerVisionObject::Cross:
			landAngle = 0.0;
			landPos.y = 0.0;
			landPos.x = 0.0;
			// TODO: implement
			break;
		default:
			LOG_WARN("Unexpected corner type.");
			break;
	}
}

float SRL::getCornersProb(Particle* p, const VisionFeatures & visionFeatures)
{
	Pose tpose(Vector2D(p->x, p->y), p->angle);
	const vector<const VisionObject*> corners = visionFeatures.getVisionObjects(VisionObject::Corner);
	float prob = 1.0;

	for(unsigned int j=0; j < corners.size(); j++)
	{
		CornerVisionObject* corner = (CornerVisionObject*)corners.at(j);
		Vector2D rel_pos = corner->getPosition();
		Vector2D pos = tpose.convertRelativeToGlobal(rel_pos);
		float angle = corner->getAngle() + p->angle;
		Vector2D closest;
		float closestAngle;
		getClosestCorner(corner->getCornerType(), pos, closest, closestAngle);
		float w1 = getLandmarkWeight(p, rel_pos, closest,
								getCornerAngleStdDev(rel_pos), getCornerDistanceStdDev(rel_pos));
		float angleDiff = angle_diff(angle, closestAngle);
		float w2 = getProb(angleDiff, CORNER_GLOBAL_ANGLE_STD_DEV);
		prob = prob * w1 * w2;
	}

	return prob;
}

float SRL::getCircleProb(Particle* p, const VisionFeatures & visionFeatures)
{
	const vector<const VisionObject*> circles = visionFeatures.getVisionObjects(VisionObject::Circle);

	float prob = 1.0;

	for(unsigned int j=0; j < circles.size(); j++)
	{
		Vector2D pos = circles[j]->getPosition();
		prob *= getLandmarkWeight(p, pos, Vector2D(0, 0),
						getCircleAngleStdDev(pos), getCircleDistanceStdDev(pos));
	}
	return prob;
}

Vector2D SRL::getClosestPointOnLine(Vector2D p, Vector2D e1, Vector2D e2)
{
	// (x - p) . (e2 - e1) = 0, perpendicular
	// x = (e2 - e1) * t + e1, on line
	// 0 <= t <= 1, limit to line
	// solves to t = ((e1 - p) . (e2 - e1)) / ((e2 - e1) . (e2 - e1))
	Vector2D dir = e2 - e1;
	float t = -(e1 - p).dot(dir) / dir.dot(dir);
	Vector2D temp = e1 + t * dir;
	if (t < 0.0)
		return e1;
	if (t > 1.0)
		return e2;
	return e1 + t * dir;
}

// returns line in global coordinates, input is closest point to robot on line in global coordinates
void SRL::getClosestLine(Vector2D visionClosestGlobal, float visionGlobalAngle,
		Vector2D & closestEnd1, Vector2D & closestEnd2, Vector2D & closestClosest, float & angle)
{
	while (visionGlobalAngle > M_PI / 2)
		visionGlobalAngle -= M_PI;
	while (visionGlobalAngle < -M_PI / 2)
		visionGlobalAngle += M_PI;

	// we have a horizontal line
	if (fabs(visionGlobalAngle) < M_PI / 4)
	{
		// in top half of field
		// x coordinates must go from left to right
		if (visionClosestGlobal.y > 0)
		{
			// high on the top or in the center away from penalty box
			if (visionClosestGlobal.y > (2000.0 + field.getHalfPenaltyBoxWidth()) / 2 ||
					fabs(visionClosestGlobal.x) < 3000.0 - 2 * field.getPenaltyBoxLength())
			{
				closestEnd1 = Vector2D(-3000.0, 2000.0);
				closestEnd2 = Vector2D(3000.0, 2000.0);
			}
			else if (visionClosestGlobal.x < 0.0)
			{
				closestEnd1 = Vector2D(-3000.0, field.getHalfPenaltyBoxWidth());
				closestEnd2 = Vector2D(-3000.0 + field.getPenaltyBoxLength(),
								field.getHalfPenaltyBoxWidth());
			}
			else
			{
				closestEnd2 = Vector2D(3000.0, field.getHalfPenaltyBoxWidth());
				closestEnd1 = Vector2D(3000.0 - field.getPenaltyBoxLength(),
								field.getHalfPenaltyBoxWidth());
			}
		}
		else
		{
			// high on the top or in the center away from penalty box
			if (visionClosestGlobal.y < -(2000.0 + field.getHalfPenaltyBoxWidth()) / 2 ||
					fabs(visionClosestGlobal.x) < 3000.0 - 2 * field.getPenaltyBoxLength())
			{
				closestEnd1 = Vector2D(-3000.0, -2000.0);
				closestEnd2 = Vector2D(3000.0, -2000.0);
			}
			else if (visionClosestGlobal.x < 0.0)
			{
				closestEnd1 = Vector2D(-3000.0, -field.getHalfPenaltyBoxWidth());
				closestEnd2 = Vector2D(-3000.0 + field.getPenaltyBoxLength(),
								-field.getHalfPenaltyBoxWidth());
			}
			else
			{
				closestEnd2 = Vector2D(3000.0, -field.getHalfPenaltyBoxWidth());
				closestEnd1 = Vector2D(3000.0 - field.getPenaltyBoxLength(),
								-field.getHalfPenaltyBoxWidth());
			}
		}

		closestClosest.x = visionClosestGlobal.x;
		closestClosest.y = closestEnd1.y;
		if (closestClosest.x < closestEnd1.x)
			closestClosest.x = closestEnd1.x;
		if (closestClosest.x > closestEnd2.x)
			closestClosest.x = closestEnd2.x;
		angle = 0.0;
	}
	// vertical lines
	else
	{
		// first y must be less than second
		if (fabs(visionClosestGlobal.x) < (3000.0 - field.getPenaltyBoxLength()) / 2.0)
		{
			closestEnd1 = Vector2D(0.0, -2000.0);
			closestEnd2 = Vector2D(0.0, 2000.0);
		}
		else if (fabs(visionClosestGlobal.x) < 3000.0 - field.getPenaltyBoxLength() / 2.0)
		{
			if (visionClosestGlobal.x < 0.0)
			{
				closestEnd1 = Vector2D(-(3000.0 - field.getPenaltyBoxLength()),
										-field.getHalfPenaltyBoxWidth());
				closestEnd2 = Vector2D(-(3000.0 - field.getPenaltyBoxLength()),
										field.getHalfPenaltyBoxWidth());
			}
			else
			{
				closestEnd1 = Vector2D((3000.0 - field.getPenaltyBoxLength()),
										-field.getHalfPenaltyBoxWidth());
				closestEnd2 = Vector2D((3000.0 - field.getPenaltyBoxLength()),
										field.getHalfPenaltyBoxWidth());
			}
		}
		else
		{
			if (visionClosestGlobal.x < 0.0)
			{
				closestEnd1 = Vector2D(-3000.0, -2000.0);
				closestEnd2 = Vector2D(-3000.0, 2000.0);
			}
			else
			{
				closestEnd1 = Vector2D(3000.0, -2000.0);
				closestEnd2 = Vector2D(3000.0, 2000.0);
			}
		}
		closestClosest.x = closestEnd1.x;
		closestClosest.y = visionClosestGlobal.y;
		if (closestClosest.y < closestEnd1.y)
			closestClosest.y = closestEnd1.y;
		if (closestClosest.y > closestEnd2.y)
			closestClosest.y = closestEnd2.y;
		angle = M_PI / 2;
	}
}

float SRL::getLinesProb(Particle* p, const VisionFeatures & visionFeatures)
{
	const vector<const VisionObject*> lines = visionFeatures.getVisionObjects(VisionObject::Line);
	float prob = 1.0;

	Pose tpose(Vector2D(p->x, p->y), p->angle);
	int numLines = 0;

	for(unsigned int i=0; i < lines.size(); i++)
	{
		LineVisionObject* line = (LineVisionObject*)lines.at(i);
		if (line->getEndPoint1().length() > LINE_MAX_DISTANCE ||
			line->getEndPoint2().length() > LINE_MAX_DISTANCE)
			continue;
		Vector2D vis1 = tpose.convertRelativeToGlobal(line->getEndPoint1());
		Vector2D vis2 = tpose.convertRelativeToGlobal(line->getEndPoint2());
		if ((vis2 - vis1).length() < LINE_MIN_LENGTH)
			continue;
		Vector2D closest = getClosestPointOnLine(Vector2D(p->x, p->y), vis1, vis2);
		float visangle = (vis2 - vis1).angle();
		Vector2D land1, land2, landclosest;
		float landangle;
		getClosestLine(closest, (vis2 - vis1).angle(), land1, land2, landclosest, landangle);

		float dist_diff = fabs(closest.length() - landclosest.length());
		float angle_diff = fabs(closest.angle() - landclosest.angle());
		if(angle_diff > M_PI)
			angle_diff = angle_diff - 2 * M_PI;
		if (angle_diff < -M_PI)
			angle_diff = angle_diff + 2 * M_PI;

		float t = getProb(angle_diff, getLineAngleStdDev(closest));
		//printf("a %g\n", t);
	   	t *= getProb(dist_diff, getLineDistanceStdDev(closest));
		//printf("b %g\n", t);

		// get weight of angle
		float diffangle = visangle - landangle;
		while (diffangle > M_PI / 2)
			diffangle -= M_PI;
		while (diffangle < -M_PI / 2)
			diffangle += M_PI;

		t *= getProb(diffangle, LINE_GLOBAL_ANGLE_STD_DEV);
		// don't decrease too much to cancel out everything else
		prob *= 0.9 + t;
		numLines++;
	}

	return prob;//pow(prob, 1.0 / numLines);
}
 */
void SRL::visionUpdate(Particle* p)
{
	float prob = 1.0;


	prob *= getGoalPostsProb(p);


	if (prob <= 10e-15)
		prob = 10e-15;
	p->weight *= prob;
}

float SRL::getLandmarkWeight(Particle* p, Vector2D obs,
		Vector2D lm, float ang_std_dev, float dist_std_dev)
{
	//convert global landmark to local coordinates with respect to this particle
	Vector2D rel_lm = lm;
	rel_lm.x -= p->x;
	rel_lm.y -= p->y;
	rel_lm = rel_lm.rotate(-p->angle);

	float dist_diff = fabs(obs.length() - rel_lm.length());
	float angle_diff = fabs(obs.angle() - rel_lm.angle());
	if(angle_diff > M_PI)
		angle_diff = angle_diff - 2 * M_PI;
	if (angle_diff < -M_PI)
		angle_diff = angle_diff + 2 * M_PI;

	float weight = getProb(angle_diff, ang_std_dev) * getProb(dist_diff, dist_std_dev);
	return weight;
}

/*
float SRL::getLineWeight(Particle* p,
				Vector2D obs1, Vector2D obs2, Vector2D lm1, Vector2D lm2)
{
	Pose tpose(Vector2D(p->x, p->y), p->angle);
	//convert global line endpoints to local coordinates with respect to this particle
	Vector2D rel_lm1 = tpose.convertGlobalToRelative(lm1);
	Vector2D rel_lm2 = tpose.convertGlobalToRelative(lm2);

	// find closest point on both lines
	Vector2D obs_dir = obs2 - obs1;
	Vector2D lm_dir = rel_lm2 - rel_lm1;
	float t = -obs_dir.dot(obs1) / obs_dir.dot(obs_dir);
	// bound the point to be on the line
	t = max(min((double)t, 1.0), 0.0);
	Vector2D obs_closest = obs1 + t * obs_dir;

	t = -(lm_dir.dot(rel_lm1) + lm_dir.dot(obs_closest)) / lm_dir.dot(lm_dir);
	t = max(min((double)t, 1.0), 0.0);
	Vector2D lm_closest = rel_lm1 + t * lm_dir;

	// find probability of distance and angle
	float w = getLandmarkWeight(p, obs_closest, tpose.convertRelativeToGlobal(lm_closest),
				getLineAngleStdDev(obs_closest), getLineDistanceStdDev(obs_closest));
	// direction doesn't matter
	float temp1 = getProb(obs_dir.angle() - lm_dir.angle(), LINE_GLOBAL_ANGLE_STD_DEV);
	float temp2 = getProb(obs_dir.angle() - lm_dir.angle() + M_PI, LINE_GLOBAL_ANGLE_STD_DEV);
	// TODO: w *=
	w = max(temp1, temp2);
	return w;
}*/

float SRL::getProb(float x, float std_dev)
{
	//get probability from a normal pdf with mean at 0
	//float normalizer = (1.0 / ((double)std_dev * sqrt(2*M_PI)));
	float exponential = exp(-pow((double)x,2) / (2*pow((double)std_dev,2)));

	// is it better with or without normalization? this way it is between 0 and 1,
	// good for apply weight to noise
	return exponential;
}

inline void SRL::swapParticles(int p1, int p2)
{
	Particle temp;
	memcpy(&temp, &particles[p1], sizeof(Particle));
	memcpy(&particles[p1], &particles[p2], sizeof(Particle));
	memcpy(&particles[p2], &temp, sizeof(Particle));
}

// quickselect
int SRL::partitionParticleWeights(int left, int right, int pivot)
{
	float pivotValue = particles[pivot].weight;
	swapParticles(pivot, right-1);
	int s = left;
	for (int i = left; i < right - 1; i++)
		if (particles[i].weight < pivotValue)
		{
			swapParticles(s, i);
			s++;
		}
	// put pivot back
	swapParticles(right-1, s);
	return s;
}

void SRL::selectParticle(int left, int right, int k)
{
	while (1)
	{
		int pivot = left + (int)(((float)rand() / RAND_MAX) * (right - left));
		pivot = partitionParticleWeights(left, right, pivot);
		if (pivot == k)
			return;
		else if (k < pivot)
			right = pivot;
		else
			left = pivot + 1;
	}
}

float SRL::findKthHighestWeight(int k)
{
	selectParticle(0, getNumParticles(), k);

	return particles[k].weight;
}

void SRL::applyNoiseToParticle(Particle* p, float std_dev_x, float std_dev_y, float std_dev_theta)
{
	float rand_x = getRandomNoise(std_dev_x);
	float rand_y = getRandomNoise(std_dev_y);
	float rand_angle = getRandomNoise(std_dev_theta);
	p->x = p->x + rand_x;
	p->y = p->y + rand_y;
	p->angle = p->angle + rand_angle;
	p->weight = getProb(rand_x, std_dev_x) * getProb(
			rand_y, std_dev_y) *
			getProb(rand_angle, std_dev_theta) * p->weight;
}

void SRL::compute_cdf(void)
{
	particle_cdf[0] = particles[0].weight;
	for (int i = 0; i < getNumParticles(); i++)
	{
		particle_cdf[i] = particle_cdf[i-1] + particles[i].weight;
	}
}

int SRL::choose_particle_at_random(void)
{
	//Draw new particle from old particle set according to the particle weights
	float rand_num = ((float)rand()/RAND_MAX) * particle_cdf[getNumParticles() - 1];
	int left = 0, right = getNumParticles();
	// use binary search
	while (left < right - 1)
	{
		int mid = (left + right) / 2;
		if (particle_cdf[mid] <= rand_num)
		{
			left = mid;
		}
		else
		{
			right = mid;
		}
	}
	// special case
	if (left == 0 && particle_cdf[0] >= rand_num)
		right = 0;

	return right;
}

void SRL::resample()
{
	// reduce weight of outliers, as in normalize function
	int num = getNumParticles();
	float total_old_weight = 0.0;
	float maximum_weight = findKthHighestWeight(num / 2) * 10;
	for (int i = 0; i < num; i++)
	{
		if (particles[i].weight > maximum_weight)
			particles[i].weight = maximum_weight;
		total_old_weight += particles[i].weight;
	}
	for (int i = 0; i < num; i++)
	{
		particles[i].weight = particles[i].weight / total_old_weight;
	}

	compute_cdf();

	// from Rofer's self-localization paper
	float w_avg = total_old_weight / num;
	w_slow = w_slow + ALPHA_SLOW * (w_avg - w_slow);
	w_fast = w_fast + ALPHA_FAST * (w_avg - w_fast);

	float reset_prob = 1 - w_fast / w_slow;
	if (reset_prob < 0.0)
		reset_prob = 0.0;

	// use the normalized total weight
	float total_new_weight = particle_cdf[num - 1];


	// sample from temp_particles, since particles will change
	Particle* temp_particles = (Particle*)malloc(sizeof(Particle) * num);
	memcpy(temp_particles, particles, sizeof(Particle) * num);


	float threshold = SAMPLE_WEIGHT_THRESHOLD / total_old_weight;
	float total_weight = 0.0;
	int i = 0;
	for (i=0; i < MAX_NUM_PARTICLES; i++)
	{
		int particle_generated = 0;

		if (((float)rand() / RAND_MAX) <= RESAMPLE_UNIFORM_PROB)
		{
			generateParticleAtRandom(&(particles[i]));
			particles[i].weight = total_new_weight / num;
			visionUpdate(&particles[i]);
			particle_generated = 1;
		}
		if ((float)rand() / RAND_MAX < reset_prob)
		{
			if (!generateParticleFromVision(&(particles[i])))
			{
				if (!isOutOfBounds(&(particles[i])))
				{
					particles[i].weight = total_new_weight / num;
					particle_generated = 1;
					visionUpdate(&particles[i]);
				}
			}
		}
		if (!particle_generated)
		{
			int right = choose_particle_at_random();
			memcpy(&(particles[i]), &(temp_particles[right]), sizeof(Particle));
			applyNoiseToParticle(&(particles[i]), RESAMPLE_STD_DEV_X, RESAMPLE_STD_DEV_Y, RESAMPLE_STD_DEV_ANGLE);
		}

		// TODO: adjust threshold
		total_weight += particles[i].weight;
		if (total_weight > threshold && i + 1 >= MIN_NUM_PARTICLES)
		{
			i++;
			break;
		}
	}
	num_particles = i;
	free(temp_particles);
}

void SRL::normalizeWeights()
{
	float total = 0.0;
	int num = getNumParticles();
	// TODO: play with constant
	// the idea is no particle should count for more than 20 others
	// eliminate outliers in normalization
	float maximum_weight = findKthHighestWeight(num / 2) * 20;
	for (int i = 0; i < num; i++)
	{
		if (particles[i].weight > maximum_weight)
			particles[i].weight = maximum_weight;
		total += particles[i].weight;
	}
	for (int i = 0; i < num; i++)
	{
		particles[i].weight = particles[i].weight / total;
	}
}

void SRL::computeLocalStatistics(Pose & pose,
		Vector2D* mean_pos, Vector2D* mean_dir, float* total_weight)
{
	for (int i = 0; i < getNumParticles(); i++)
	{
		float dist = pow(pose.getPosition().x - particles[i].x, 2) +
				pow(pose.getPosition().y - particles[i].y, 2);
		if (dist > MIN_POSE_MEAN_DIST * MIN_POSE_MEAN_DIST)
			continue;
		mean_pos->x += particles[i].weight * particles[i].x;
		mean_pos->y += particles[i].weight * particles[i].y;
		*total_weight += particles[i].weight;
		mean_dir->x += particles[i].weight * cos(particles[i].angle);
		mean_dir->y += particles[i].weight * sin(particles[i].angle);
	}
	*mean_pos = 1.0 / *total_weight * (*mean_pos);
	*mean_dir = 1.0 / *total_weight * (*mean_dir);
}

Pose SRL::getPose()
{
	Pose temp;
	Pose overall;
	//TODO: maybe return of the average position of the cluster with the highest weight instead of the particle with the highest weight
	for(int i=0; i < getNumParticles(); i++)
	{
		float dist = pow(pose.getPosition().x - particles[i].x, 2) +
				pow(pose.getPosition().y - particles[i].y, 2);
		float angle = fabs(pose.getAngle() - particles[i].angle);
		// we can jump if our weight is less than average
		if (particles[i].weight > overall.getConfidence())
		{
			overall.setPosition(Vector2D(particles[i].x, particles[i].y));
			overall.setAngle(particles[i].angle);
			overall.setConfidence(particles[i].weight);
		}
		if (dist > MAX_JUMP_DIST * MAX_JUMP_DIST)
			continue;
		if (angle > MAX_JUMP_ANGLE)
			continue;
		if(particles[i].weight > temp.getConfidence())
		{
			temp.setPosition(Vector2D(particles[i].x, particles[i].y));
			temp.setAngle(particles[i].angle);
			temp.setConfidence(particles[i].weight);
		}
	}

	if (temp.getConfidence() == 0.0)
		temp = overall;

	float total_weight = 0.0;
	Vector2D mean_pos(0, 0);
	Vector2D mean_dir(0, 0);
	computeLocalStatistics(temp, &mean_pos, &mean_dir, &total_weight);
	if (total_weight < JUMP_WEIGHT_THRESHOLD)
	{
		mean_pos.x = 0.0;
		mean_pos.y = 0.0;
		mean_dir.x = 0.0;
		mean_dir.y = 0.0;
		temp = overall;
		computeLocalStatistics(temp, &mean_pos, &mean_dir, &total_weight);
	}

	pose.setPosition(mean_pos);
	if (mean_dir.x != 0.0 || mean_dir.y != 0.0)
		pose.setAngle(mean_dir.angle());

	// compute confidence, variance from pose
	float variance = 0.0;
	for (int i = 0; i < getNumParticles(); i++)
	{
		variance += particles[i].weight * (
				pow(particles[i].x - pose.getPosition().x, 2) +
				pow(particles[i].y - pose.getPosition().y, 2));
	}
	variance /= total_weight;

	pose.setConfidence(max(min((ZERO_CONFIDENCE_DEVIATION - sqrt(variance)) /
			(ZERO_CONFIDENCE_DEVIATION - FULL_CONFIDENCE_DEVIATION),
			1.0), 0.0));
	// TODO: include angle? probably not necessary
	pose.setLost(pose.getConfidence() < LOST_CONFIDENCE);

	compute_cdf();
	for (int i = 0; i < POSE_NUM_RANDOM_SAMPLES; i++)
	{
		int t = choose_particle_at_random();
		pose.setGuess(i, particles[t].x, particles[t].y, particles[t].angle);
	}

	return pose;
}

void SRL::generateParticleAtRandom(Particle* p)
{
	//Add random particle
	p->x = ((float)rand() / RAND_MAX) * 6000.0 - 3000.0;
	p->y = ((float)rand() / RAND_MAX) * 4000.0 - 2000.0;
	p->angle = ((float)rand() / RAND_MAX) * 2 * M_PI - M_PI;
	p->weight = 1.0 / getNumParticles();
}

float SRL::getGoalPostDistanceStdDev(Vector2D & rel)
{
	return GOAL_POST_DISTANCE_MIN_STD_DEV + (rel.length() / GOAL_POST_MAX_STD_DEV_DISTANCE) *
			(GOAL_POST_DISTANCE_MAX_STD_DEV - GOAL_POST_DISTANCE_MIN_STD_DEV);
}

float SRL::getGoalPostAngleStdDev(Vector2D & rel)
{
	return GOAL_POST_ANGLE_MIN_STD_DEV + (rel.length() / GOAL_POST_MAX_STD_DEV_DISTANCE) *
			(GOAL_POST_ANGLE_MAX_STD_DEV - GOAL_POST_ANGLE_MIN_STD_DEV);
}

/*
float SRL::getCornerDistanceStdDev(Vector2D & rel)
{
	return CORNER_DISTANCE_MIN_STD_DEV + (rel.length() / CORNER_MAX_STD_DEV_DISTANCE) *
		(CORNER_DISTANCE_MAX_STD_DEV - CORNER_DISTANCE_MIN_STD_DEV);
}

float SRL::getCornerAngleStdDev(Vector2D & rel)
{
	return CORNER_ANGLE_MIN_STD_DEV + (rel.length() / CORNER_MAX_STD_DEV_DISTANCE) *
		(CORNER_ANGLE_MAX_STD_DEV - CORNER_ANGLE_MIN_STD_DEV);
}

float SRL::getCircleDistanceStdDev(Vector2D & rel)
{
	return CIRCLE_DISTANCE_MIN_STD_DEV + (rel.length() / CIRCLE_MAX_STD_DEV_DISTANCE) *
		(CIRCLE_DISTANCE_MAX_STD_DEV - CIRCLE_DISTANCE_MIN_STD_DEV);
}

float SRL::getCircleAngleStdDev(Vector2D & rel)
{
	return CIRCLE_ANGLE_STD_DEV;
}

float SRL::getLineDistanceStdDev(Vector2D & rel)
{
	return LINE_DISTANCE_MIN_STD_DEV + (rel.length() / LINE_MAX_STD_DEV_DISTANCE) *
		(LINE_DISTANCE_MAX_STD_DEV - LINE_DISTANCE_MIN_STD_DEV);
}

float SRL::getLineAngleStdDev(Vector2D & rel)
{
	return LINE_ANGLE_MIN_STD_DEV + (rel.length() / LINE_MAX_STD_DEV_DISTANCE) *
		(LINE_ANGLE_MAX_STD_DEV - LINE_ANGLE_MIN_STD_DEV);
}
 */
void SRL::applyNoiseToReading(Vector2D & rel, float dist_std_dev, float ang_std_dev)
{
	float dist = rel.length();
	float ang = rel.angle();
	dist += getRandomNoise(dist_std_dev);
	ang += getRandomNoise(ang_std_dev);
	rel.x = dist * cos(ang);
	rel.y = dist * sin(ang);
}

int SRL::generateParticleFromVision(Particle* p)
{
	Vector2D land1, rel1, land2, rel2;

	int landmarks = findBestTwoLandmarks(land1, rel1, land2, rel2);
	// TODO: use corners to find position

	if (landmarks >= 1)
		applyNoiseToReading(rel1, getGoalPostDistanceStdDev(rel1), getGoalPostAngleStdDev(rel1));
	if (landmarks >= 2)
		applyNoiseToReading(rel2, getGoalPostDistanceStdDev(rel2), getGoalPostAngleStdDev(rel2));

	if (landmarks == 2)
		return findPoseFromTwoLandmarks(p, land1, rel1, land2, rel2);

	if(landmarks == 1)
		return findPoseFromOneLandmark(p, land1, rel1);

	//LOG_WARN("No landmarks found.\n");
	return -1;
}

int SRL::findBestTwoLandmarks(Vector2D & landmark1, Vector2D & relPos1, Vector2D & landmark2, Vector2D & relPos2)
{
	int ret = 0;

	ObjectState* goalp0r = &(_GoalDetector->goals->p0RightEstimate);
	ObjectState* goalp0l = &(_GoalDetector->goals->p0LeftEstimate);
	ObjectState* goalp1r = &(_GoalDetector->goals->p1RightEstimate);
	ObjectState* goalp1l = &(_GoalDetector->goals->p1LeftEstimate);

	HPoint3D *lmbl,*lmbr,*lmyl,*lmyr;
	lmbl = &(_WorldModel->getPoint("p49")->p);
	lmbr = &(_WorldModel->getPoint("p51")->p);
	lmyl = &(_WorldModel->getPoint("p55")->p);
	lmyr = &(_WorldModel->getPoint("p53")->p);



	if(_GoalDetector->getObj()->p0ElapsedTimeSinceLastObs < _GoalDetector->getObj()->p1ElapsedTimeSinceLastObs)
	{
		if(goalp0l->getReliability()>goalp0r->getReliability())
		{
			relPos1.x =  goalp0l->getPositionInRelativeCoordinates().x;
			relPos1.y =  goalp0l->getPositionInRelativeCoordinates().y;
			landmark1.x = lmbl->X;
			landmark1.y = lmbl->Y;
			relPos2.x =  goalp0r->getPositionInRelativeCoordinates().x;
			relPos2.y =  goalp0r->getPositionInRelativeCoordinates().y;
			landmark2.x = lmbr->X;
			landmark2.y = lmbr->Y;
		}else{
			relPos1.x =  goalp0r->getPositionInRelativeCoordinates().x;
			relPos1.y =  goalp0r->getPositionInRelativeCoordinates().y;
			landmark1.x = lmbr->X;
			landmark1.y = lmbr->Y;
			relPos2.x =  goalp0l->getPositionInRelativeCoordinates().x;
			relPos2.y =  goalp0l->getPositionInRelativeCoordinates().y;
			landmark2.x = lmbl->X;
			landmark2.y = lmbl->Y;
		}


		if(goalp0l->getReliability() >= ObjectState::HIGH_RELIABLE)
			ret++;
		if(goalp0r->getReliability() >= ObjectState::HIGH_RELIABLE)
			ret++;
	}else
	{
		if(goalp1l->getReliability()>goalp1r->getReliability())
		{
			relPos1.x =  goalp1l->getPositionInRelativeCoordinates().x;
			relPos1.y =  goalp1l->getPositionInRelativeCoordinates().y;
			landmark1.x = lmyl->X;
			landmark1.y = lmyl->Y;
			relPos2.x =  goalp1r->getPositionInRelativeCoordinates().x;
			relPos2.y =  goalp1r->getPositionInRelativeCoordinates().y;
			landmark2.x = lmyr->X;
			landmark2.y = lmyr->Y;
		}else{
			relPos1.x =  goalp1r->getPositionInRelativeCoordinates().x;
			relPos1.y =  goalp1r->getPositionInRelativeCoordinates().y;
			landmark1.x = lmyr->X;
			landmark1.y = lmyr->Y;
			relPos2.x =  goalp1l->getPositionInRelativeCoordinates().x;
			relPos2.y =  goalp1l->getPositionInRelativeCoordinates().y;
			landmark2.x = lmyl->X;
			landmark2.y = lmyl->Y;
		}


		if(goalp0l->getReliability() >= ObjectState::HIGH_RELIABLE)
			ret++;
		if(goalp0r->getReliability() >= ObjectState::HIGH_RELIABLE)
			ret++;
	}

	return ret;
}

int SRL::findPoseFromTwoLandmarks(Particle* p, Vector2D landmark1, Vector2D relPos1, Vector2D landmark2, Vector2D relPos2)
{
	//find the two intersection points of two circles
	float r1 = relPos1.length();
	float r2 = relPos2.length();
	float d = (landmark1 - landmark2).length();

	if((d > r1 + r2) || d < fabs(r1 - r2) || (d == 0 && r1 == r2))
	{
		return -1;
	}

	float a = (pow(r1,2) - pow(r2,2) + pow(d,2))/(2*d);
	float h = sqrt(pow(r1,2) - pow(a,2));

	Vector2D midpoint = landmark1 + a*(landmark2 - landmark1)/d;

	float twoLandmark_x1 = midpoint.x + h*(landmark2.y - landmark1.y)/d;
	float twoLandmark_y1 = midpoint.y - h*(landmark2.x - landmark1.x)/d;
	float twoLandmark_x2 = midpoint.x - h*(landmark2.y - landmark1.y)/d;
	float twoLandmark_y2 = midpoint.y + h*(landmark2.x - landmark1.x)/d;


	//choose the point that is inside the field.bounds
	if (fabs(twoLandmark_x1) < 6000.0 / 2.0 && fabs(twoLandmark_y1) < 4000.0 / 2.0 &&
			fabs(twoLandmark_x2) < 6000.0 / 2.0 && fabs(twoLandmark_y2) < 4000.0 / 2.0)
	{
		//both points are in the bounds so randomly pick one of them
		if (rand() % 2 == 0)
		{
			p->x = twoLandmark_x1;
			p->y = twoLandmark_y1;
		}
		else
		{
			p->x = twoLandmark_x2;
			p->y = twoLandmark_y2;
		}
	}
	else if(fabs(twoLandmark_x1) < 6000.0 / 2.0 && fabs(twoLandmark_y1) < 4000.0 / 2.0)
	{
		p->x = twoLandmark_x1;
		p->y = twoLandmark_y1;
	}
	else if(fabs(twoLandmark_x2) < 6000.0 / 2.0 && fabs(twoLandmark_y2) < 4000.0 / 2.0)
	{
		p->x = twoLandmark_x2;
		p->y = twoLandmark_y2;
	}
	else
	{
		return -1;
	}

	//determine the angle of the particle
	float angle = atan2(landmark1.y - p->y, landmark1.x - p->x);
	angle -= relPos1.angle();
	if(angle > M_PI)
		angle -= 2*M_PI;
	else if(angle < -M_PI)
		angle += 2*M_PI;
	p->angle = angle;

	return 0;
}

int SRL::findPoseFromOneLandmark(Particle* p, Vector2D landmark, Vector2D relPos)
{
	//assume all landmarks are on the border of the field
	//this guarentees us that the part of the circle within the bounds is a single unbroken arc

	float r = relPos.length();

	for (int i = 0; i < 50; i++)
	{
		float random_angle = ((float)rand() / RAND_MAX) * 2 * M_PI;
		//use this angle and the radius to compute the x and y of the particle
		Vector2D pos(landmark.x + r * cos(random_angle), landmark.y + r * sin(random_angle));
		if (pos.x < -3000.0 || pos.x > 3000.0 ||
				pos.y < -2000.0  || pos.y > 2000.0)
			continue;

		//determine the angle of the particle
		float angle = atan2(landmark.y - p->y, landmark.x - p->x);
		angle -= relPos.angle();
		while (angle > M_PI) angle -= 2*M_PI;
		while (angle < -M_PI) angle += 2*M_PI;

		p->x = pos.x;
		p->y = pos.y;
		p->angle = angle;
		p->weight = p->weight;

		return 0;
	}

	return -1;
}

///////////////////////////////////////////////////////////////
/////////////////////////SPECIAL CASES/////////////////////////
///////////////////////////////////////////////////////////////

void SRL::scatterAboutPoint(Particle *p, float x, float y, float ang){
	float rand_x = getRandomNoise(RESAMPLE_STD_DEV_X);
	float rand_y = getRandomNoise(RESAMPLE_STD_DEV_Y);
	float rand_angle = getRandomNoise(RESAMPLE_STD_DEV_ANGLE);
	p->x = x + rand_x;
	p->y = y + rand_y;
	p->angle = ang + rand_angle;
	p->weight = getProb(rand_x, RESAMPLE_STD_DEV_X) * getProb(rand_y, RESAMPLE_STD_DEV_Y)* getProb(rand_angle, RESAMPLE_STD_DEV_ANGLE);
}



void SRL::resetUniform()
{
	num_particles = MAX_NUM_PARTICLES;
	for(int i=0; i < getNumParticles(); i++)
	{
		generateParticleAtRandom(&particles[i]);
	}
	normalizeWeights();
}

int SRL::getNumParticles(void)
{
	return num_particles;
}

Particle SRL::getParticle(int num)
{
	return particles[num];
}

bica::ShapeList
SRL::getGrDebugAbs()
{
	shapeListAbs.clear();

	//cerr<<"============================================================\nparticles: "<<getNumParticles()<<endl;

	for(int i=0; i < getNumParticles(); i++)
	{
		bica::Point3DPtr src(new bica::Point3D);
		bica::Point3DPtr dst(new bica::Point3D);
		bica::ArrowPtr a(new bica::Arrow);

		src->x = particles[i].x;
		src->y = particles[i].y;
		src->z = 600.0;
		dst->x = (float)particles[i].x + 200.0 * cos(particles[i].angle);
		dst->y = (float)particles[i].y + 200.0 * sin(particles[i].angle);
		dst->z = 600.0;

		//cerr<<"("<<particles[i].x<<", "<<particles[i].y<<", "<<	particles[i].angle<<") ";

		a->src = src;
		a->dst = dst;
		a->width = 10.0;
		a->color = bica::RED;
		a->filled = true;
		a->opacity = 255;
		a->accKey = "c";
		a->label = "SRL";
		shapeListAbs.push_back(a);
	}


	return shapeListAbs;
}

bica::ShapeList
SRL::getGrDebugRel()
{


	shapeListRel.clear();


	return shapeListRel;
}


float
SRL::getReliability()
{
	return getPose().getConfidence();
}

void
SRL::resetToPosition(int state)
{
	GameEvent = state;
}

void
SRL::getPosition(double &xpos, double &ypos, double &thetapos)
{
	xpos = (double)getPose().getPosition().x;
	ypos = (double)getPose().getPosition().y;
	thetapos = (double)getPose().getAngle();
}
void
SRL::getPosition(vector<tRobotHipothesis> &hipotheses)
{
	hipotheses.clear();

	tRobotHipothesis aux;

	aux.x = (double)getPose().getPosition().x;
	aux.y = (double)getPose().getPosition().y;
	aux.t = (double)getPose().getAngle();
	aux.prob = getReliability();
	aux.id = 0;
	hipotheses.push_back(aux);
	aux.x = -(double)getPose().getPosition().x;
	aux.y = -(double)getPose().getPosition().y;
	aux.t = normalizePi((double)getPose().getAngle()+M_PI);
	aux.prob = getReliability();
	aux.id = 0;
	hipotheses.push_back(aux);

}

string
SRL::getInfo()
{
	return "NULL";
}

bica::ImageDataPtr
SRL::getImgDebug()
{
	return 0;
}

