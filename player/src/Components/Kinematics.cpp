#include "Kinematics.h"

#include "Head.h"

Kinematics::Kinematics()
{
	camIntrinsicsUpper.createDictionary();
	camIntrinsicsLower.createDictionary();

	this->escale = 1000.0;
	this->use_lower = 1;

	setFreqTime(SHORT_RATE);

	RTrt.resize(4,4);

	pthread_mutex_init(&mutex, NULL);
}

Kinematics::~Kinematics()
{
}

void
Kinematics::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);
	try {
		this->pmotion = parentBroker->getMotionProxy();
	} catch( ALError& e)	{
		cerr<<"[Kinematics:init()] " << e.toString() << endl;
	}

	this->loadCameraIntrinsicParams(UPPER_CAMERA_KINEMATICS_FILE, LOWER_CAMERA_KINEMATICS_FILE);

	updateKinematics();
}

void
Kinematics::saveCameraIntrinsicParams()
{


	camIntrinsicsUpper.saveToFile(UPPER_CAMERA_KINEMATICS_FILE);
	camIntrinsicsLower.saveToFile(LOWER_CAMERA_KINEMATICS_FILE);
}

void
Kinematics::loadCameraIntrinsicParams(const char *upper_file, const char *lower_file) {

	camIntrinsicsUpper.loadFromFile(upper_file);
	camIntrinsicsLower.loadFromFile(lower_file);

	/*Update parameters*/
	if(!( camIntrinsicsUpper.getValueFloat("u0", &upper_u0) && camIntrinsicsUpper.getValueFloat("v0", &upper_v0) &&
			camIntrinsicsUpper.getValueFloat("fdistx", &upper_fdistx) && camIntrinsicsUpper.getValueFloat("fdisty", &upper_fdisty)&& camIntrinsicsUpper.getValueFloat("roll", &upper_roll)))
		cerr << begin_red << "[Kinematics] Problem loading upper camera parameters from " << upper_file << end_color << endl;

	if(!( camIntrinsicsLower.getValueFloat("u0", &lower_u0) && camIntrinsicsLower.getValueFloat("v0", &lower_v0) &&
			camIntrinsicsLower.getValueFloat("fdistx", &lower_fdistx) && camIntrinsicsLower.getValueFloat("fdisty", &lower_fdisty)&& camIntrinsicsLower.getValueFloat("roll", &lower_roll)))
		cerr << begin_red << "[Kinematics] Problem loading lower camera parameters from " << lower_file << end_color << endl;
}

TPinHoleCamera *
Kinematics::getCamera()
{
	return &(this->camera);
}

TKinematics *
Kinematics::getKinematics()
{
	return &(this->mykinematics);
}

void
Kinematics::cloneKinematics(TKinematics * kinematics_out)
{
	this->cloneKinematics(this->mykinematics, kinematics_out);
}

void
Kinematics::cloneKinematics(TKinematics kinematics, TKinematics * kinematics_out)
{
	kinematics_out->x = kinematics.x;
	kinematics_out->y = kinematics.y;
	kinematics_out->z = kinematics.z;
	kinematics_out->pan = kinematics.pan;
	kinematics_out->tilt = kinematics.tilt;
	kinematics_out->roll = kinematics.roll;
	kinematics_out->foax = kinematics.foax;
	kinematics_out->foay = kinematics.foay;
	kinematics_out->foaz = kinematics.foaz;

	kinematics_out->RT[0] = kinematics.RT[0];
	kinematics_out->RT[1] = kinematics.RT[1];
	kinematics_out->RT[2] = kinematics.RT[2];
	kinematics_out->RT[3] = kinematics.RT[3];
	kinematics_out->RT[4] = kinematics.RT[4];
	kinematics_out->RT[5] = kinematics.RT[5];
	kinematics_out->RT[6] = kinematics.RT[6];
	kinematics_out->RT[7] = kinematics.RT[7];
	kinematics_out->RT[8] = kinematics.RT[8];
	kinematics_out->RT[9] = kinematics.RT[9];
	kinematics_out->RT[10] = kinematics.RT[10];
	kinematics_out->RT[11] = kinematics.RT[11];
}

void
Kinematics::updateKinematics()
{

	MatrixCM RTneck(4,4), RTdiff(4,4), RTRoll(4,4), RTcam(4,4), foarel(4,1), foa(4,1);
	vector<float> neckpos;
	vector<float> neckRT, torso2neckRT;
	vector<float> footpos;
	double deg40 = -40.0*M_PI/180.0;

	torsoRT = this->pmotion->getTransform("Torso", 2, USE_SENSOR);

	/*Obtetemos la posición y orientación del cuello*/
	neckpos = this->pmotion->getPosition("Head", 2/*this->pmotion->SPACE_NAO*/, USE_SENSOR);

	/*Obtetemos la posición y orientación del cuello*/
	torso2neckRT = this->pmotion->getPosition("Head", 0, USE_SENSOR);

	/*Obtenemos la transformada homogenea, que nos permite calcular la traslación desde el cuello a la cámara teniendo en cuenta la inclinación de la cámara*/
	neckRT = this->pmotion->getTransform("Head", 2, USE_SENSOR);


	RTrt.sete(0,0,torsoRT[0]);
	RTrt.sete(0,1,torsoRT[1]);
	RTrt.sete(0,2,torsoRT[2]);
	RTrt.sete(0,3,torsoRT[3]);
	RTrt.sete(1,0,torsoRT[4]);
	RTrt.sete(1,1,torsoRT[5]);
	RTrt.sete(1,2,torsoRT[6]);
	RTrt.sete(1,3,torsoRT[7]);
	RTrt.sete(2,0,torsoRT[8]);
	RTrt.sete(2,1,torsoRT[9]);
	RTrt.sete(2,2,torsoRT[10]);
	RTrt.sete(2,3,torsoRT[11]);
	RTrt.sete(3,0,torsoRT[12]);
	RTrt.sete(3,1,torsoRT[13]);
	RTrt.sete(3,2,torsoRT[14]);
	RTrt.sete(3,3,torsoRT[15]);


	/*RT del cuello*/
	RTneck.sete(0,0,neckRT[0]);
	RTneck.sete(0,1,neckRT[1]);
	RTneck.sete(0,2,neckRT[2]);
	RTneck.sete(0,3,neckRT[3]);
	RTneck.sete(1,0,neckRT[4]);
	RTneck.sete(1,1,neckRT[5]);
	RTneck.sete(1,2,neckRT[6]);
	RTneck.sete(1,3,neckRT[7]);
	RTneck.sete(2,0,neckRT[8]);
	RTneck.sete(2,1,neckRT[9]);
	RTneck.sete(2,2,neckRT[10]);
	RTneck.sete(2,3,neckRT[11]);
	RTneck.sete(3,0,neckRT[12]);
	RTneck.sete(3,1,neckRT[13]);
	RTneck.sete(3,2,neckRT[14]);
	RTneck.sete(3,3,neckRT[15]);

	//cerr<<"RTneck "<<endl;

	//RTneck.print();

	//cerr<<"["<<pmotion->getAngles("HeadYaw", false)[0]<<", "<<pmotion->getAngles("HeadPitch", USE_SENSOR)[0]<<"]"<<endl;


	/*RT de la camara respecto al cuello*/
	if(this->use_lower) {				/*Lower camera*/
		RTdiff.sete(0,0,cos(deg40));
		RTdiff.sete(0,1,0.0);
		RTdiff.sete(0,2,-sin(deg40));
		RTdiff.sete(0,3,0.0488);				/*Profundidad*/
		RTdiff.sete(1,0,0.0);
		RTdiff.sete(1,1,1.0);
		RTdiff.sete(1,2,0.0);
		RTdiff.sete(1,3,0.0);					/*Lateral*/
		RTdiff.sete(2,0,sin(deg40));
		RTdiff.sete(2,1,0.0);
		RTdiff.sete(2,2,cos(deg40));
		RTdiff.sete(2,3,0.02381);				/*Altura*/
		RTdiff.sete(3,0,0.0);
		RTdiff.sete(3,1,0.0);
		RTdiff.sete(3,2,0.0);
		RTdiff.sete(3,3,1.0);	
	} else {							/*Top camera*/
		RTdiff.sete(0,0,cos(0.0));
		RTdiff.sete(0,1,0.0);
		RTdiff.sete(0,2,-sin(0.0));
		RTdiff.sete(0,3,0.0539);				/*Profundidad*/
		RTdiff.sete(1,0,0.0);
		RTdiff.sete(1,1,1.0);
		RTdiff.sete(1,2,0.0);
		RTdiff.sete(1,3,0.0);					/*Lateral*/
		RTdiff.sete(2,0,sin(0.0));
		RTdiff.sete(2,1,0.0);
		RTdiff.sete(2,2,cos(0.0));
		RTdiff.sete(2,3,0.0679);				/*Altura*/
		RTdiff.sete(3,0,0.0);
		RTdiff.sete(3,1,0.0);
		RTdiff.sete(3,2,0.0);
		RTdiff.sete(3,3,1.0);	
	}	


	/*RT de la camara*/
	RTcam = RTneck * RTdiff;

	/*Foa en relativas, mirando 1 metro hacia adelante*/
	foarel.sete(0,0,1.0);
	foarel.sete(1,0,0.0);
	foarel.sete(2,0,0.0);
	foarel.sete(3,0,1.0);

	/*Foa en absolutas*/
	foa = RTcam * foarel;

	/*Save kinematics*/
	this->mykinematics.x = (float)RTcam.e(0,3)*this->escale;		/*X*/
	this->mykinematics.y = (float)RTcam.e(1,3)*this->escale;		/*Y*/
	this->mykinematics.z = (float)RTcam.e(2,3)*this->escale;		/*Z*/
	this->mykinematics.pan = (float)neckpos[5];						/*Pan*/
	if(this->use_lower)
		this->mykinematics.tilt = (float)-(neckpos[4] - deg40);		/*Tilt*/
	else
		this->mykinematics.tilt = (float)-neckpos[4];				/*Tilt*/

	//double xroll = 5.0 * M_PI / 180.0;
	if(this->use_lower)
		this->mykinematics.roll = lower_roll;//(float)neckpos[3];					/*Roll*/
	else
		this->mykinematics.roll = upper_roll;//(float)neckpos[3];					/*Roll*/

	this->mykinematics.foax = (float)foa.e(0,0)*this->escale;		/*FoaX*/
	this->mykinematics.foay = (float)foa.e(1,0)*this->escale;		/*FoaY*/
	this->mykinematics.foaz = (float)foa.e(2,0)*this->escale;		/*FoaZ*/

	/*Save RT*/
	this->mykinematics.RT[0] = (float)RTcam.e(0,0);
	this->mykinematics.RT[1] = (float)RTcam.e(0,1);
	this->mykinematics.RT[2] = (float)RTcam.e(0,2);
	this->mykinematics.RT[3] = (float)RTcam.e(0,3);
	this->mykinematics.RT[4] = (float)RTcam.e(1,0);
	this->mykinematics.RT[5] = (float)RTcam.e(1,1);
	this->mykinematics.RT[6] = (float)RTcam.e(1,2);
	this->mykinematics.RT[7] = (float)RTcam.e(1,3);
	this->mykinematics.RT[8] = (float)RTcam.e(2,0);
	this->mykinematics.RT[9] = (float)RTcam.e(2,1);
	this->mykinematics.RT[10] = (float)RTcam.e(2,2);
	this->mykinematics.RT[11] = (float)RTcam.e(2,3);
}

void
Kinematics::updateIntrinsicParams(float u0, float v0, float fdistx, float fdisty, float roll)
{
	if(this->use_lower) {	/*Lower camera*/
		this->camera.u0 = lower_u0 = u0;
		this->camera.v0=lower_v0 = v0;
		this->camera.fdistx=lower_fdistx = fdistx;
		this->camera.fdisty=lower_fdisty = fdisty;
		this->camera.roll=lower_roll = roll;
	} else {														/*Top camera*/
		this->camera.u0=upper_u0 = u0;
		this->camera.v0=upper_v0 = v0;
		this->camera.fdistx=upper_fdistx = fdistx;
		this->camera.fdisty=upper_fdisty = fdisty;
		this->camera.roll=upper_roll = roll;
	}

	if(!( camIntrinsicsUpper.setValueFloat("u0", &upper_u0) && camIntrinsicsUpper.setValueFloat("v0", &upper_v0) &&
			camIntrinsicsUpper.setValueFloat("fdistx", &upper_fdistx) && camIntrinsicsUpper.setValueFloat("fdisty", &upper_fdisty)&& camIntrinsicsUpper.setValueFloat("roll", &upper_roll)))
		cerr << begin_red << "[Kinematics] Problem settings upper camera parameters" << end_color << endl;

	if(!( camIntrinsicsLower.setValueFloat("u0", &lower_u0) && camIntrinsicsLower.setValueFloat("v0", &lower_v0) &&
			camIntrinsicsLower.setValueFloat("fdistx", &lower_fdistx) && camIntrinsicsLower.setValueFloat("fdisty", &lower_fdisty)&& camIntrinsicsLower.setValueFloat("roll", &lower_roll)))
		cerr << begin_red << "[Kinematics] Problem settings lower camera parameters" << end_color << endl;

}

void
Kinematics::updateCamera()
{

	float x, y, z;
	float foax, foay, foaz, roll;

	x = this->mykinematics.x;
	y = this->mykinematics.y;
	z = this->mykinematics.z;
	roll = this->mykinematics.roll;
	foax = this->mykinematics.foax;
	foay = this->mykinematics.foay;
	foaz = this->mykinematics.foaz;

	this->camera.position.X=x;
	this->camera.position.Y=y;
	this->camera.position.Z=z;
	this->camera.position.H=1.0;
	this->camera.foa.X=foax;
	this->camera.foa.Y=foay;
	this->camera.foa.Z=foaz;
	this->camera.foa.H=1.0;
	this->camera.roll=roll;

	/*Set Intrinsecs camera parameters*/
	if(this->use_lower) {	/*Lower camera*/
		this->camera.u0=lower_u0;
		this->camera.v0=lower_v0;
		this->camera.fdistx=lower_fdistx;
		this->camera.fdisty=lower_fdisty;
		this->camera.roll=lower_roll;
	} else {														/*Top camera*/
		this->camera.u0=upper_u0;
		this->camera.v0=upper_v0;
		this->camera.fdistx=upper_fdistx;
		this->camera.fdisty=upper_fdisty;
		this->camera.roll=upper_roll;
	}

	this->camera.skew = 0.0;
	this->camera.rows = ImageInput::IMG_HEIGHT;
	this->camera.columns = ImageInput::IMG_WIDTH;
	update_camera_matrix(&(this->camera));	
}


void
Kinematics::step(void)
{
}

void
Kinematics::forceStep(void)
{
	pthread_mutex_lock(&mutex);
	startDebugInfo();
	/*Update kinematics and camera*/
	this->updateKinematics();
	this->updateCamera();
	endDebugInfo();
	pthread_mutex_unlock(&mutex);
}

void 
Kinematics::pixel2Optical(TPinHoleCamera * camera, HPoint2D * p)
{
	float aux;
	int height;

	height = camera->rows;

	aux = p->x;
	p->x = height-1-p->y;
	p->y = aux;		
}

void 
Kinematics::optical2Pixel(TPinHoleCamera * camera, HPoint2D * p)
{
	float aux;
	int height;

	height = camera->rows;

	aux = p->y;
	p->y = height-1-p->x;
	p->x = aux;	
}

void
Kinematics::get2DPosition(HPoint3D in, HPoint2D &res)
{
	this->get2DPosition(this->getCamera(), in, res);
}

void
Kinematics::get2DPosition(TPinHoleCamera * camera, HPoint3D in, HPoint2D &res)
{
	HPoint2D p2d;

	project(in, &p2d, *camera);
	this->optical2Pixel(camera, &p2d);

	res.x=p2d.x;
	res.y=p2d.y;
	res.h=p2d.h;
}

void
Kinematics::get3DPosition(HPoint3D &res, HPoint2D in)
{
	this->get3DPositionZ(res, in, 0.0);
}

void
Kinematics::get3DPosition(TPinHoleCamera * camera, HPoint3D &res, HPoint2D in)
{
	this->get3DPositionZ(camera, res, in, 0.0);
}

void
Kinematics::get3DPositionX(HPoint3D &res, HPoint2D in, float X = 0.0)
{
	this->get3DPositionX(this->getCamera(), res, in, X);
}

void
Kinematics::get3DPositionX(TPinHoleCamera * camera, HPoint3D &res, HPoint2D in, float X = 0.0)
{
	HPoint2D p2d;
	HPoint3D p3d;
	float x, y, z;
	float xfinal, yfinal, zfinal;

	pthread_mutex_lock(&mutex);

	x = camera->position.X;
	y = camera->position.Y;
	z = camera->position.Z;

	p2d.x = in.x;
	p2d.y = in.y;
	p2d.h = in.h;

	this->pixel2Optical(camera, &p2d);
	backproject(&p3d, p2d, *camera);
	pthread_mutex_unlock(&mutex);

	/*Check division by zero*/
	if((p3d.X-x) == 0.0) {
		res.H = 0.0;
		return;
	}

	xfinal = X;

	/*Linear equation (X-x)/(p3d.X-x) = (Y-y)/(p3d.Y-y) = (Z-z)/(p3d.Z-z)*/
	yfinal = y + (p3d.Y - y)*(xfinal - x)/(p3d.X-x);	
	zfinal = z + (p3d.Z - z)*(xfinal - x)/(p3d.X-x);

	res.X = xfinal;
	res.Y = yfinal;
	res.Z = zfinal;
	res.H = 1.0;
}

void
Kinematics::get3DPositionY(HPoint3D &res, HPoint2D in, float Y = 0.0)
{
	this->get3DPositionY(this->getCamera(), res, in, Y);
}

void
Kinematics::get3DPositionY(TPinHoleCamera * camera, HPoint3D &res, HPoint2D in, float Y = 0.0)
{
	HPoint2D p2d;
	HPoint3D p3d;
	float x, y, z;
	float xfinal, yfinal, zfinal;

	pthread_mutex_lock(&mutex);
	x = camera->position.X;
	y = camera->position.Y;
	z = camera->position.Z;

	p2d.x = in.x;
	p2d.y = in.y;
	p2d.h = in.h;

	this->pixel2Optical(camera, &p2d);
	backproject(&p3d, p2d, *camera);
	pthread_mutex_unlock(&mutex);

	/*Check division by zero*/
	if((p3d.Y-y) == 0.0) {
		res.H = 0.0;
		return;
	}

	yfinal = Y;

	/*Linear equation (X-x)/(p3d.X-x) = (Y-y)/(p3d.Y-y) = (Z-z)/(p3d.Z-z)*/
	xfinal = x + (p3d.X - x)*(yfinal - y)/(p3d.Y-y);
	zfinal = z + (p3d.Z - z)*(yfinal - y)/(p3d.Y-y);

	res.X = xfinal;
	res.Y = yfinal;
	res.Z = zfinal;
	res.H = 1.0;
}

void
Kinematics::get3DPositionZ(HPoint3D &res, HPoint2D in, float Z = 0.0)
{
	this->get3DPositionZ(this->getCamera(), res, in, Z);
}

void
Kinematics::get3DPositionZ(TPinHoleCamera * camera, HPoint3D &res, HPoint2D in, float Z = 0.0)
{
	HPoint2D p2d;
	HPoint3D p3d;
	float x, y, z;
	float xfinal, yfinal, zfinal;


	x = camera->position.X;
	y = camera->position.Y;
	z = camera->position.Z;

	//cerr<<"camera: "<<x<<", "<<y<<", "<<z<<endl;

	p2d.x = in.x;
	p2d.y = in.y;
	p2d.h = in.h;
	//cerr<<"p2d: "<<in.x<<", "<<in.y<<", "<<in.h<<endl;
	pthread_mutex_lock(&mutex);
	this->pixel2Optical(camera, &p2d);
	backproject(&p3d, p2d, *camera);
	//cerr<<"p3d: "<<p3d.X<<", "<<p3d.Y<<", "<<p3d.Z<<endl;
	pthread_mutex_unlock(&mutex);
	/*Check division by zero*/
	if((p3d.Z-z) == 0.0) {
		res.H = 0.0;
		return;
	}

	zfinal = Z;

	/*Linear equation (X-x)/(p3d.X-x) = (Y-y)/(p3d.Y-y) = (Z-z)/(p3d.Z-z)*/
	xfinal = x + (p3d.X - x)*(zfinal - z)/(p3d.Z-z);
	yfinal = y + (p3d.Y - y)*(zfinal - z)/(p3d.Z-z);	

	res.X = xfinal;
	res.Y = yfinal;
	res.Z = zfinal;
	res.H = 1.0;
}

void
Kinematics::configureCamera(TPinHoleCamera * camera, float px, float py, float theta)
{
	this->configureCamera(this->mykinematics, camera, px, py, theta);
}

void
Kinematics::configureCamera(TKinematics kinematics, TPinHoleCamera * camera, float px, float py, float theta)
{

	MatrixCM robotRT(4,4), absRT(4,4), RTcam(4,4), foarel(4,1), foa(4,1);

	/*Absolute RT*/
	absRT.sete(0,0,cos(theta));
	absRT.sete(0,1,-sin(theta));
	absRT.sete(0,2,0.0);
	absRT.sete(0,3,px/this->escale);
	absRT.sete(1,0,sin(theta));
	absRT.sete(1,1,cos(theta));
	absRT.sete(1,2,0.0);
	absRT.sete(1,3,py/this->escale);
	absRT.sete(2,0,0.0);
	absRT.sete(2,1,0.0);
	absRT.sete(2,2,1.0);
	absRT.sete(2,3,0.0);
	absRT.sete(3,0,0.0);
	absRT.sete(3,1,0.0);
	absRT.sete(3,2,0.0);
	absRT.sete(3,3,1.0);

	/*Create robot RT*/
	robotRT.sete(0,0,kinematics.RT[0]);
	robotRT.sete(0,1,kinematics.RT[1]);
	robotRT.sete(0,2,kinematics.RT[2]);
	robotRT.sete(0,3,kinematics.RT[3]);
	robotRT.sete(1,0,kinematics.RT[4]);
	robotRT.sete(1,1,kinematics.RT[5]);
	robotRT.sete(1,2,kinematics.RT[6]);
	robotRT.sete(1,3,kinematics.RT[7]);
	robotRT.sete(2,0,kinematics.RT[8]);
	robotRT.sete(2,1,kinematics.RT[9]);
	robotRT.sete(2,2,kinematics.RT[10]);
	robotRT.sete(2,3,kinematics.RT[11]);
	robotRT.sete(3,0,0.0);
	robotRT.sete(3,1,0.0);
	robotRT.sete(3,2,0.0);
	robotRT.sete(3,3,1.0);

	/*Camera RT*/
	RTcam = absRT * robotRT;

	/*Relative Foa, looking at 1 meter in front of the robot*/
	foarel.sete(0,0,1.0);
	foarel.sete(1,0,0.0);
	foarel.sete(2,0,0.0);
	foarel.sete(3,0,1.0);

	/*Absolute Foa*/
	foa = RTcam * foarel;	

	camera->position.X=(float)RTcam.e(0,3)*this->escale;
	camera->position.Y=(float)RTcam.e(1,3)*this->escale;
	camera->position.Z=(float)RTcam.e(2,3)*this->escale;
	camera->position.H=1.0;		
	camera->foa.X=(float)foa.e(0,0)*this->escale;
	camera->foa.Y=(float)foa.e(1,0)*this->escale;
	camera->foa.Z=(float)foa.e(2,0)*this->escale;
	camera->foa.H=1.0;
	camera->roll=kinematics.roll;

	/*Set Intrinsecs camera parameters*/
	camera->u0=this->getCamera()->u0;
	camera->v0=this->getCamera()->v0;
	camera->fdistx=this->getCamera()->fdistx;
	camera->fdisty=this->getCamera()->fdisty;
	camera->skew=this->getCamera()->skew;
	camera->rows = this->getCamera()->rows;
	camera->columns = this->getCamera()->columns;
	update_camera_matrix(camera);
}

int
Kinematics::drawLine(TPinHoleCamera * camera, HPoint2D * p1, HPoint2D * p2) {

	HPoint2D tmp1, tmp2;

	this->pixel2Optical(camera, p1);
	this->pixel2Optical(camera, p2);

	if(!displayline(*p1,*p2,&tmp1,&tmp2,*camera)==1)
		return 0;

	this->optical2Pixel(camera, &tmp1);
	this->optical2Pixel(camera, &tmp2);

	p1->x = tmp1.x;
	p1->y = tmp1.y;
	p1->h = tmp1.h;
	p2->x = tmp2.x;
	p2->y = tmp2.y;
	p2->h = tmp2.h;

	return 1;
}

void
Kinematics::get3DCameraPosition(HPoint3D &res)
{
	res.X = getCamera()->position.X;
	res.Y = getCamera()->position.Y;
	res.Z = getCamera()->position.Z;
}

void
Kinematics::getNeckAngles(float x, float y, float z, float &pan, float &tilt)
{
	MatrixCM RTtorso(4,4), RTdiff(4,4), RTneck(4,4), RTcam(4,4);
	float deg40 = (40.0*M_PI/180.0);

	RTtorso.sete(0,0,torsoRT[0]);
	RTtorso.sete(0,1,torsoRT[1]);
	RTtorso.sete(0,2,torsoRT[2]);
	RTtorso.sete(0,3,torsoRT[3]);
	RTtorso.sete(1,0,torsoRT[4]);
	RTtorso.sete(1,1,torsoRT[5]);
	RTtorso.sete(1,2,torsoRT[6]);
	RTtorso.sete(1,3,torsoRT[7]);
	RTtorso.sete(2,0,torsoRT[8]);
	RTtorso.sete(2,1,torsoRT[9]);
	RTtorso.sete(2,2,torsoRT[10]);
	RTtorso.sete(2,3,torsoRT[11]);
	RTtorso.sete(3,0,0.0);
	RTtorso.sete(3,1,0.0);
	RTtorso.sete(3,2,0.0);
	RTtorso.sete(3,3,1.0);

	RTdiff.sete(0,0,1.0);
	RTdiff.sete(0,1,0.0);
	RTdiff.sete(0,2,0.0);
	RTdiff.sete(0,3,0.0);				/*Profundidad*/
	RTdiff.sete(1,0,0.0);
	RTdiff.sete(1,1,1.0);
	RTdiff.sete(1,2,0.0);
	RTdiff.sete(1,3,0.0);			/*Lateral*/
	RTdiff.sete(2,0,0.0);
	RTdiff.sete(2,1,0.0);
	RTdiff.sete(2,2,1.0);
	RTdiff.sete(2,3,0.1265);				/*Altura*/
	RTdiff.sete(3,0,0.0);
	RTdiff.sete(3,1,0.0);
	RTdiff.sete(3,2,0.0);
	RTdiff.sete(3,3,1.0);

	RTcam.sete(0,0, 1.0);
	RTcam.sete(0,1, 0.0);
	RTcam.sete(0,2, 0.0);
	RTcam.sete(0,3,-0.0488);				/*Profundidad*/
	RTcam.sete(1,0,0.0);
	RTcam.sete(1,1,1.0);
	RTcam.sete(1,2,0.0);
	RTcam.sete(1,3,0.0);			/*Lateral*/
	RTcam.sete(2,0,0.0);
	RTcam.sete(2,1,0.0);
	RTcam.sete(2,2,1.0);
	RTcam.sete(2,3,-0.02381);				/*Altura*/
	RTcam.sete(3,0,0.0);
	RTcam.sete(3,1,0.0);
	RTcam.sete(3,2,0.0);
	RTcam.sete(3,3,1.0);

	float tx,ty,tz;
	MatrixCM  p3ds1(4,1), p3ds2(4,1);

	RTneck = RTtorso * RTdiff * RTcam;

	p3ds1.sete(0,0,x/this->escale);
	p3ds1.sete(1,0,y/this->escale);
	p3ds1.sete(2,0,-z/this->escale);
	p3ds1.sete(3,0,1.0);

	p3ds2 = RTneck * p3ds1;

	tx = p3ds2.e(0,0)/p3ds2.e(3,0);
	ty = p3ds2.e(1,0)/p3ds2.e(3,0);
	tz = p3ds2.e(2,0)/p3ds2.e(3,0);

	pan = atan2(ty,tx);
	tilt = atan2(tz,tx)- deg40;
}

void
Kinematics::getFrustum (HPoint3D &upperLeft3D, HPoint3D &upperRight3D,
		HPoint3D &lowerLeft3D, HPoint3D &lowerRight3D)
{
	HPoint2D upperLeft2D, upperRight2D, lowerLeft2D, lowerRight2D;

	upperLeft2D.h = upperRight2D.h = lowerLeft2D.h = lowerRight2D.h = 1.0;
	upperLeft2D.x = 0;
	upperLeft2D.y = 0;
	upperRight2D.x = 320;
	upperRight2D.y = 0;
	lowerLeft2D.x = 0;
	lowerLeft2D.y = 240;
	lowerRight2D.x = 320;
	lowerRight2D.y = 240;

	this->get3DPositionZ(upperLeft3D, upperLeft2D, 0.0);
	this->get3DPositionZ(upperRight3D, upperRight2D, 0.0);
	this->get3DPositionZ(lowerLeft3D, lowerLeft2D, 0.0);
	this->get3DPositionZ(lowerRight3D, lowerRight2D, 0.0);

	// Upper points cut behind the robot
	if (upperLeft3D.X < 0) {
		// Calculate the angle between the robot and lowerLeft3D
		float alpha = atan2(lowerLeft3D.Y, lowerLeft3D.X);

		// Calculate a virtual point with the previous angle and long distance (10000 mm)
		upperLeft3D.X = cos(alpha) * 10000;
		upperLeft3D.Y = sin(alpha) * 10000;

		alpha = atan2(lowerRight3D.Y, lowerRight3D.X);
		upperRight3D.X = cos(alpha) * 10000;
		upperRight3D.Y = sin(alpha) * 10000;
	}
}

bool
Kinematics::pointInPolygon (int nvert, float *vertx, float *verty, float testx, float testy)
{
	int i, j, c = 0;
	for (i = 0, j = nvert-1; i < nvert; j = i++) {
		if ( ((verty[i]>testy) != (verty[j]>testy)) &&
				(testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
			c = !c;
	}
	return c == 1;
}

bool
Kinematics::groundPointInImage(HPoint3D *testpoint)
{
	HPoint3D p1, p2, p3, p4;
	HPoint2D p1i, p2i, p3i, p4i;

	p1i.x = (float)ImageInput::IMG_WIDTH * 0.25f;
	p1i.y = (float)ImageInput::IMG_HEIGHT * 0.25f;
	p1i.h = 1.0f;

	p2i.x = (float)ImageInput::IMG_WIDTH * 0.75f;
	p2i.y = (float)ImageInput::IMG_HEIGHT * 0.25f;
	p2i.h = 1.0f;

	p3i.x = (float)ImageInput::IMG_WIDTH * 0.25f;
	p3i.y = (float)ImageInput::IMG_HEIGHT;
	p3i.h = 1.0f;

	p4i.x = (float)ImageInput::IMG_WIDTH * 0.75f;
	p4i.y = (float)ImageInput::IMG_HEIGHT;
	p4i.h = 1.0f;


	this->get3DPositionZ(p1, p1i, 0.0);
	this->get3DPositionZ(p2, p2i, 0.0);
	this->get3DPositionZ(p3, p3i, 0.0);
	this->get3DPositionZ(p4, p4i, 0.0);

	float px[4], py[4];

	px[0] = p1.X / p1.H;
	px[1] = p2.X / p2.H;
	px[2] = p3.X / p3.H;
	px[3] = p4.X / p4.H;
	py[0] = p1.Y / p1.H;
	py[1] = p2.Y / p2.H;
	py[2] = p3.Y / p3.H;
	py[3] = p4.Y / p4.H;

	return this->pointInPolygon(4, px, py, testpoint->X, testpoint->Y);
}
