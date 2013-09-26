#include "Perception.h"

const string Perception::UPPER_CAMERA_CONFIG_FILE =
		"/home/nao/bica/conf/ballPerception/cameraUpper.conf";
const string Perception::LOWER_CAMERA_CONFIG_FILE =
		"/home/nao/bica/conf/ballPerception/cameraLower.conf";


bica::ImageDataPtr
Perception::getImageDataWithFeatures(FeaturesType type, const Ice::Current& c)
{	
	bica::ImageDataPtr reply(new bica::ImageData);
	reply->description = imgDescription;

	int imgSize = ImageInput::IMG_WIDTH * ImageInput::IMG_HEIGHT * ImageInput::IMG_CHANNELS * sizeof(char);

	IceUtil::Time t = IceUtil::Time::now();
	reply->timeStamp.seconds = (long)t.toSeconds();
	reply->timeStamp.useconds = (long)t.toMicroSeconds() - reply->timeStamp.seconds*1000000;
	reply->pixelData.resize(imgSize);

	IplImage* src;

	if((type == Filtered) || (type == HSV)       || (type == Ball)     ||
	   (type == BlueNet)  || (type == YellowNet) || (type == Field)    ||
	   (type == Lines)    || (type == RedMarker)  || (type == BlueMarker)  )
	{
		this->newImage();
		src = cvCreateImage(cvSize(ImageInput::IMG_WIDTH,ImageInput::IMG_HEIGHT),
				ImageInput::BYTES_PER_CHANNEL, ImageInput::IMG_CHANNELS);
	}
	if(type == Filtered)
		_ImageHandler->getImageRGB(src->imageData, true, 0);
	else if(type == HSV)
		_ImageHandler->getImageHSV(src->imageData);
	else if(type == Ball)
		_ImageHandler->getImageRGB(src->imageData, true, ImageInput::CORANGE);
	else if(type == BlueNet)
		_ImageHandler->getImageRGB(src->imageData, true, ImageInput::CBLUE);
	else if(type == YellowNet)
		_ImageHandler->getImageRGB(src->imageData, true, ImageInput::CYELLOW);
	else if(type == Field)
		_ImageHandler->getImageRGB(src->imageData, true, ImageInput::CGREEN);
	else if(type == Lines)
		_ImageHandler->getImageRGB(src->imageData, true, ImageInput::CWHITE);
	else if (type == BlueMarker) {
		_ImageHandler->getImageRGB(src->imageData, true, ImageInput::CCYAN);
	} else if (type == RedMarker)
		_ImageHandler->getImageRGB(src->imageData, true, ImageInput::CMAGENTA);
	else
		{
		this->newImage();
		src = cvCreateImage(cvSize(ImageInput::IMG_WIDTH,ImageInput::IMG_HEIGHT),
				ImageInput::BYTES_PER_CHANNEL, ImageInput::IMG_CHANNELS);
			_ImageHandler->getImageRGB(src->imageData, false, 0);
		}


	if(type == Segments || type == Regions || type == Detected) {

		// TODO: meter un contador para saber cuanto tiempo cuesta hacer
		// todos los steps.
		_BallDetector->step();
		_LineDetector->step();
		_GoalDetector->step();
		_RobotDetector->step();

		if (type == Segments)
			this->drawSegments(src);
		else if (type == Regions)
			this->drawRegions(src);
		else if (type == Detected) {
			_BodyDetector->drawBody(src);
			_Horizon->drawFieldBorder(src);
			this->drawDetected(src);
		}
	}

	memmove( &(reply->pixelData[0]), (char *) src->imageData, imgSize);//copy data to reply

	cvReleaseImage(&src);

	return reply;
}


bica::ImageDataPtr
Perception::getImageData(const Ice::Current& c)
{
	bica::ImageDataPtr reply(new bica::ImageData);

	reply->description = imgDescription;
	IceUtil::Time t = IceUtil::Time::now();
	reply->timeStamp.seconds = (long)t.toSeconds();
	reply->timeStamp.useconds = (long)t.toMicroSeconds() - reply->timeStamp.seconds*1000000;
	reply->pixelData.resize(imgDescription->size);

	return reply;
}

jderobot::ImageDataPtr
Perception::getImageDataJderobot(const Ice::Current& c)
{
	jderobot::ImageDataPtr reply(new jderobot::ImageData);
	reply->description = new jderobot::ImageDescription();
	reply->description->width = ImageInput::IMG_WIDTH;
	reply->description->height = ImageInput::IMG_HEIGHT;
	reply->description->size = ImageInput::IMG_WIDTH * ImageInput::IMG_HEIGHT * ImageInput::IMG_CHANNELS;
	reply->description->format = "RGB8";
	
	int imgSize = ImageInput::IMG_WIDTH * ImageInput::IMG_HEIGHT * ImageInput::IMG_CHANNELS * sizeof(char);

	IceUtil::Time t = IceUtil::Time::now();
	reply->timeStamp.seconds = (long)t.toSeconds();
	reply->timeStamp.useconds = (long)t.toMicroSeconds() - reply->timeStamp.seconds*1000000;
	reply->pixelData.resize(imgSize);

	IplImage* src;

	this->newImage();
	src = cvCreateImage(cvSize(ImageInput::IMG_WIDTH,ImageInput::IMG_HEIGHT),
				ImageInput::BYTES_PER_CHANNEL, ImageInput::IMG_CHANNELS);

	_ImageHandler->getImageRGBInverted(src->imageData, false, 0);

	memmove( &(reply->pixelData[0]), (char *) src->imageData, imgSize);//copy data to reply

	cvReleaseImage(&src);

	return reply;
}

string
Perception::test(const Ice::Current& c)
{
	return string("Hello world!\n");
}

ImageDescriptionPtr
Perception::getImageDescription(const Ice::Current& c)
{
	return imgDescription;
}

HSVFilterPtr
Perception::getHSVFilter (CameraType cam, ObjectsType obj, const Ice::Current& c)
{
	bica::HSVFilterPtr reply(new bica::HSVFilter);
	HSVClrParams orange, blue, yellow, green, white, cyan, magenta, *color=NULL;

	if (cam == UPPERCAMERA)
		colorFilter->getFilterParams(ImageInput::UPPER_CAMERA, orange, blue, yellow, green, white, cyan, magenta);
	else
		colorFilter->getFilterParams(ImageInput::LOWER_CAMERA, orange, blue, yellow, green, white, cyan, magenta);

	if (obj == BallObj)
		color = &orange;
	else if (obj == BlueNetObj)
		color = &blue;
	else if (obj == YellowNetObj)
		color = &yellow;
	else if (obj == FieldObj)
		color = &green;
	else if (obj == LinesObj)
		color = &white;
	else if (obj == BlueMarkerObj)
		color = &cyan;
	else if (obj == RedMarkerObj)
		color = &magenta;
	

	reply->hmin = color->hmin * 100;
	reply->hmax = color->hmax * 100;
	reply->smin = color->smin * 100;
	reply->smax = color->smax * 100;
	reply->vmin = color->vmin * 100;
	reply->vmax = color->vmax * 100;

	return reply;
}

void
Perception::setHSVFilter (CameraType cam, ObjectsType obj, const HSVFilterPtr& newFilter, const Ice::Current& c)
{
	HSVClrParams orange, blue, yellow, green, white, cyan, magenta;

	HSVClrParams color = {newFilter->hmin / 100.0, newFilter->hmax / 100.0, newFilter->smin / 100.0,
			newFilter->smax / 100.0, newFilter->vmin / 100.0, newFilter->vmax / 100.0};

	if (cam == UPPERCAMERA)
		colorFilter->getFilterParams(ImageInput::UPPER_CAMERA, orange, blue, yellow, green, white, cyan, magenta);
	else
		colorFilter->getFilterParams(ImageInput::LOWER_CAMERA, orange, blue, yellow, green, white, cyan, magenta);

	if (obj == BallObj)
		orange = color;
	else if (obj == BlueNetObj)
		blue = color;
	else if (obj == YellowNetObj)
		yellow = color;
	else if (obj == FieldObj)
		green = color;
	else if (obj == LinesObj)
		white = color;
	else if (obj == BlueMarkerObj)
		cyan = color;
	else if (obj == RedMarkerObj)
		magenta = color;
	else {
		cout << "[Perception::setHSVFilter] Invalid object received (" << obj << ")\n";
		return;
	}

	colorFilter->setFilterParams(cam, orange, blue, yellow, green, white, cyan, magenta);
	colorFilter->reloadLut(cam, orange, blue, yellow, green, white, cyan, magenta);
}

void
Perception::setCam (CameraType cam, const Ice::Current& c)
{
	if (cam == UPPERCAMERA)
		setCam(ImageInput::UPPER_CAMERA);
	else
		setCam(ImageInput::LOWER_CAMERA);
}

/*void
Perception::setImageType( FeaturesType type, const Ice::Current& c )
{
	cout << "hola" << endl;
}*/


Perception::Perception() {

	colorFilter = SimpleColorFilter::getInstance();
	_Horizon = Horizon::getInstance();
	_Kinematics = Kinematics::getInstance();
	_BodyDetector = BodyDetector::getInstance();
	_ImageHandler = ImageHandler::getInstance();
	_Segmentation = Segmentation::getInstance();
	_RegionBuilder = RegionBuilder::getInstance();
	_BallDetector = BallDetector::getInstance();
	_LineDetector = LineDetector::getInstance();
	_GoalDetector = GoalDetector::getInstance();
	_RobotDetector = RobotDetector::getInstance();

	camera_initialized = false;

	camConfUpper.createDictionary();
	camConfLower.createDictionary();

	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutexstep, NULL);

	setFreqTime(SHORT_RATE);

	this->imgDebug.arrayReserve(ImageInput::IMG_WIDTH * ImageInput::IMG_HEIGHT * ImageInput::IMG_CHANNELS * sizeof(char));
	this->colorSrc = new char[ImageInput::IMG_WIDTH * ImageInput::IMG_HEIGHT * ImageInput::IMG_CHANNELS];

	bica::ImageDescriptionPtr imgDescription(new bica::ImageDescription);
	imgDescription->width = ImageInput::IMG_WIDTH;
	imgDescription->height = ImageInput::IMG_HEIGHT;
	imgDescription->size = ImageInput::IMG_WIDTH * ImageInput::IMG_HEIGHT * ImageInput::IMG_CHANNELS;
	imgDescription->format = "YUV888";

	newImageTaken = false;

	struct timeval current;
	long current_m;

	gettimeofday(&current, NULL);
	current_m = current.tv_sec * 1000000 + current.tv_usec;

	lastTimeStamp = current_m;//newImage->fTimeStamp;
	wtime=0;

	struct timeval s1;
	gettimeofday(&s1, NULL);
	imagereq_ts = s1.tv_sec * 1000000 + s1.tv_usec;

}

Perception::~Perception() {

	try {
		pcamera->unsubscribe(fLEMname);
	} catch (ALError& e) {
		cerr << "Unable to unsubscribe Lem: " << e.toString() << endl;
	}

	cvReleaseImage(&cvImage);
	cvReleaseImage(&cvAux);

	delete this->colorSrc;
}

void
Perception::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);

	initCamera();

	int rc;

	rc =  pthread_create(&capture_thread, NULL, Perception::EntryPoint, this);

    if (rc){
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
    }
//    ostringstream s;
//    s<<"IC\t"<<getCurrentTime()<<"\t0.0"<<endl;
//	writeLog(s.str());


}

/*Camera parameters*/
void
Perception::loadCameraParams(const string upper_file, const string lower_file)
{
	Dictionary *conf;
	ostringstream s;

	camConfUpper.loadFromFile(upper_file.c_str());
	camConfLower.loadFromFile(lower_file.c_str());

	int newkCameraBrightnessID, newkCameraContrastID, newkCameraSaturationID,
	newkCameraHueID, newkCameraRedChromaID, newkCameraBlueChromaID,
	newkCameraGainID, newkCameraLensXID, newkCameraLensYID,
	newkCameraAutoGainID, newkCameraAutoExpositionID,
	newkCameraAutoWhiteBalanceID;

	for (int cameraID = 1; cameraID >= 0; cameraID--) {

		if (cameraID == ImageInput::LOWER_CAMERA)
			conf = &camConfLower;
		else if (cameraID == ImageInput::UPPER_CAMERA)
			conf = &camConfUpper;
		else
			s << "[Perception::loadCameraParams()] Bad camera identifier (" << cameraID << endl;

		// Read the camera parameters
		if ((conf->getValueInt("kCameraBrightnessID", &newkCameraBrightnessID)) &&
				(conf->getValueInt("kCameraContrastID", &newkCameraContrastID)) &&
				(conf->getValueInt("kCameraSaturationID", &newkCameraSaturationID)) &&
				(conf->getValueInt("kCameraHueID", &newkCameraHueID)) &&
				(conf->getValueInt("kCameraRedChromaID", &newkCameraRedChromaID)) &&
				(conf->getValueInt("kCameraBlueChromaID", &newkCameraBlueChromaID)) &&
				(conf->getValueInt("kCameraGainID", &newkCameraGainID)) &&
				(conf->getValueInt("kCameraLensXID", &newkCameraLensXID)) &&
				(conf->getValueInt("kCameraLensYID", &newkCameraLensYID)) &&
				(conf->getValueInt("kCameraAutoGainID",	&newkCameraAutoGainID)) &&
				(conf->getValueInt("kCameraAutoExpositionID", &newkCameraAutoExpositionID)) &&
				(conf->getValueInt("kCameraAutoWhiteBalanceID", &newkCameraAutoWhiteBalanceID)))
			s << "[Perception] Loaded camera parameters for camera " << cameraID << endl;
		else
			s << "[Perception] Problem loading parameters for camera " << cameraID << endl;

		// Set the camera parameters
		try {
			pcamera->setParam(kCameraSelectID, cameraID);
			usleep(250000);

			pcamera->setParam(kCameraBrightnessID, newkCameraBrightnessID);
			pcamera->setParam(kCameraContrastID, newkCameraContrastID);
			pcamera->setParam(kCameraSaturationID, newkCameraSaturationID);
			pcamera->setParam(kCameraHueID, newkCameraHueID);
			pcamera->setParam(kCameraRedChromaID, newkCameraRedChromaID);
			pcamera->setParam(kCameraBlueChromaID, newkCameraBlueChromaID);
			pcamera->setParam(kCameraGainID, newkCameraGainID);
			pcamera->setParam(kCameraLensXID, newkCameraLensXID);
			pcamera->setParam(kCameraLensYID, newkCameraLensYID);

			// Automatic values
			pcamera->setParam(kCameraAutoGainID, newkCameraAutoGainID);
			pcamera->setParam(kCameraAutoExpositionID, newkCameraAutoExpositionID);
			pcamera->setParam(kCameraAutoWhiteBalanceID, newkCameraAutoWhiteBalanceID);
		} catch (ALError& e) {
			s << "Could not set cam param: " << e.toString() << endl;
		}
	}
	writeLog(s.str());
}

bool
Perception::setCamParam(const int param, const int value)
{
	ostringstream s;
	try {
		pcamera->setParam(param, value);
		return true;
	} catch (ALError& e) {
		s << "Could not set cam param: " << e.toString() << endl;
		return false;
	}
}

int
Perception::getCamParam(const int param)
{
	ostringstream s;
	try {
		return pcamera->getParam(param);
	} catch (ALError& e) {
		s << "Could not set cam param: " << e.toString() << endl;
		return -1;
	}
}

void
Perception::initCamera(void)
{
	try {
		pcamera = new ALVideoDeviceProxy(getParentBroker());
		cam = pcamera->getParam(kCameraSelectID);
		fLEMname = pcamera->subscribe(string("playerGVM"), kQVGA,
				kYUV422InterlacedColorSpace, 30);
	} catch (ALError& e) {
		cerr << "[Perception::initCamera()] " << e.toString() << endl;
	}

	filterParams.arraySetSize(6);

	cvImage = cvCreateImage(cvSize(ImageInput::IMG_WIDTH, ImageInput::IMG_HEIGHT), ImageInput::BYTES_PER_CHANNEL, 2);
	cvAux = cvCreateImage(cvSize(ImageInput::IMG_WIDTH, ImageInput::IMG_HEIGHT), ImageInput::BYTES_PER_CHANNEL, ImageInput::IMG_CHANNELS);

	camera_initialized = true;

	for (int i = 0; i < 2; i++)
		loadCameraParams(UPPER_CAMERA_CONFIG_FILE, LOWER_CAMERA_CONFIG_FILE);

	setCam(ImageInput::LOWER_CAMERA);
}

long
Perception::getWaitingTime()
{
	return wtime;
};

void *Perception::EntryPoint(void * pthis)
{
	Perception *pt = Perception::getInstance();
    pt->Capture();
}
void Perception::Capture()
{
	struct timeval s1;
	long s,e;


	while(true)
	{
		gettimeofday(&s1, NULL);
		s = s1.tv_sec * 1000000 + s1.tv_usec;

		while((s-imagereq_ts)>1000000)
		{
			newImageTaken = false;
			usleep(10000); //don't waste cpu if not active
			gettimeofday(&s1, NULL);
			s = s1.tv_sec * 1000000 + s1.tv_usec;
		}

	#ifndef PLAYER_IS_REMOTE		//Get new image (local mode)
		//cerr<<"+"<<endl;
		ALImage* newImage = NULL;

		/*gettimeofday(&s1, NULL);
		s = s1.tv_sec * 1000000 + s1.tv_usec;
		cerr<<"cs ["<<s<<"]"<<endl;
		*/
		try {

#ifdef WEBOTS
			//cerr<<"*";
			usleep(25000); //webots does not block for 25 ms
#endif
			newImage = (ALImage*) (pcamera->getDirectRawImageLocal(fLEMname));
		} catch( ALError& e) {
			cerr << "[Perception::getImageLocal()] Error: " << e.toString() << endl;
		}

		/*gettimeofday(&s1, NULL);
		e = s1.tv_sec * 1000000 + s1.tv_usec;
		cerr<<"ce ["<<s<<"] "<<e-s<<endl;
		 */
		pthread_mutex_lock(&mutex);
		memcpy(imgBuff, (char*)(newImage->getFrame()), ImageInput::IMG_WIDTH * ImageInput::IMG_HEIGHT * ImageInput::IMG_CHANNELS_YUV);


		pthread_mutex_unlock(&mutex);

		gettimeofday(&s1, NULL);
		newImageTaken = true;
		image_ts = s1.tv_sec * 1000000 + s1.tv_usec;

		try {
			pcamera->releaseDirectRawImage(fLEMname);
		}catch (ALError& e) {
			cerr << "Error in releasing image: " << e.toString() << endl;
		}



	#else							//Get new image (remote mode)
		//cerr<<"-"<<endl;
		ALValue newImage;

		try {
#ifdef WEBOTS
			//cerr<<"*";
			usleep(25000); //webots does not block for 25 ms
#endif
			newImage = pcamera->getImageRemote(fLEMname);

		} catch (ALError& e) {
			cerr << "[Perception::getImageRemote()] Error: " << e.toString() << endl;
		}
//		ostringstream s;
//		s<<"IC\t"<<getCurrentTime()<<"\t0.0"<<endl;
//		s<<"IC\t"<<getCurrentTime()<<"\t1.0"<<endl;
//		s<<"IC\t"<<getCurrentTime()<<"\t0.0"<<endl;
//		writeLog(s.str());


		pthread_mutex_lock(&mutex);
		memcpy(imgBuff, (char*) static_cast<const char*> (newImage[6].GetBinary()), ImageInput::IMG_WIDTH * ImageInput::IMG_HEIGHT * ImageInput::IMG_CHANNELS_YUV);
		pthread_mutex_unlock(&mutex);

		gettimeofday(&s1, NULL);
		newImageTaken = true;
		image_ts = s1.tv_sec * 1000000 + s1.tv_usec;

	#endif

	}

}


bool
Perception::newImage(bool every)
{
	struct timeval s1;

	gettimeofday(&s1, NULL);
	imagereq_ts = s1.tv_sec * 1000000 + s1.tv_usec;

	if(newImageTaken)
	{

		long end, start;


		pthread_mutex_lock(&mutex);

		gettimeofday(&s1, NULL);
		start = s1.tv_sec * 1000000 + s1.tv_usec;
		_ImageHandler->setImage(cam, imgBuff, colorFilter);
		gettimeofday(&s1, NULL);
		end = s1.tv_sec * 1000000 + s1.tv_usec;

		_Kinematics->forceStep();
		_BodyDetector->forceStep();

		pthread_mutex_unlock(&mutex);

		newImageTaken = false;

		wtime = (end-start);
		return true;
	}else
	{
		return false;
	}

}

void
Perception::step(void)
{
	pthread_mutex_lock(&mutexstep);

	if (isTime2Run()) {
		startDebugInfo();

		/*Get a new image*/
		if(this->newImage())
		{
			/*Calculate the horizon*/
			_Horizon->process();

			/*Analyze the image to create Segments*/
			_Segmentation->analyzeImage(_Horizon);

			/*Create Regions with the Segments*/
			_RegionBuilder->createRegions();

		}
		endDebugInfo();
	}
	pthread_mutex_unlock(&mutexstep);

}

void
Perception::shortStep(void)
{

	pthread_mutex_lock(&mutexstep);

	if(isTime2Run()){
		/*Get a new image*/
		this->newImage(true);
	}
	pthread_mutex_unlock(&mutexstep);

}

void
Perception::forceStep(void)
{
	pthread_mutex_lock(&mutexstep);

	/*Get a new image*/
	this->newImage(true);

	pthread_mutex_unlock(&mutexstep);
}

void Perception::setCam(const int cam) {
	if (!camera_initialized)
		initCamera();

	try {
		pcamera->setParam(kCameraSelectID, cam);
		this->cam = cam;
	} catch (ALError& e) {
		cerr << "Unable to change camera: " << e.toString() << endl;
	}
}


//Para usar este método, desde el jmanager habrá que llamar iterando
// sobre todos los tipos de imagen (src, hsv y filtrada). El método devuelve
// la imagen de manera similar a como lo hace getImg()
void
Perception::loadExternalImg(char *img)
{
	int cont = 0;
	char *data;

	// Remove .PPM header (15 bytes)
	data = img + 15;

	for (int i = 0; i < ImageInput::IMG_HEIGHT; i++) {
		for (int j = 0; j < ImageInput::IMG_WIDTH; j++) {

			unsigned char y, u, v, r, g, b;
			int index = (i * ImageInput::IMG_WIDTH + j) * ImageInput::IMG_CHANNELS;
			// RGB->YUV
			r = data[index];
			g = data[index + 1];
			b = data[index + 2];
			ColorModelConversions::rgb2yuv(r, g, b, y, u, v);

			index = (i * ImageInput::IMG_WIDTH + j) * 2;
			// Store in YUV422
			cvImage->imageData[index] = y;
			if (cont % 2 == 0) {
				cvImage->imageData[index + 1] = u;
				cvImage->imageData[index + 3] = v;
			}
			cont++;
		}
	}
}


void
Perception::setHSVFilter(const int cameraSel, const string objectSel,
		const float _hmin, const float _hmax, const float _smin,
		const float _smax, const float _vmin, const float _vmax) {
	HSVClrParams orange, blue, yellow, green, white, cyan, magenta;

	HSVClrParams color = {_hmin / 100.0, _hmax / 100.0, _smin / 100.0,
			_smax / 100.0, _vmin / 100.0, _vmax / 100.0};

	colorFilter->getFilterParams(cameraSel, orange, blue, yellow, green, white, cyan, magenta);

	if (objectSel == "Ball")
		orange = color;
	else if (objectSel == "Blue Net")
		blue = color;
	else if (objectSel == "Yellow Net")
		yellow = color;
	else if (objectSel == "Field")
		green = color;
	else if (objectSel == "Lines")
		white = color;
	else if (objectSel == "Blue Marker")
		cyan = color;
	else if (objectSel == "Red Marker")
		magenta = color;
	else {
		cout << "[Perception::setHSVFilter] Invalid object received (" << objectSel << ")\n";
		return;
	}

	colorFilter->setFilterParams(cam, orange, blue, yellow, green, white, cyan, magenta);
	colorFilter->reloadLut(cam, orange, blue, yellow, green, white, cyan, magenta);
}

void
Perception::drawSegments(IplImage* src)
{
	_Segmentation->drawSegments(src);
}

void
Perception::drawRegions(IplImage* src)
{
	_RegionBuilder->drawRegions(src);
}

void
Perception::drawDetected(IplImage* src)
{
	_BallDetector->drawDetected(src);
	_LineDetector->drawDetected(src);
	_GoalDetector->drawDetected(src);
	_RobotDetector->drawDetected(src);
}
