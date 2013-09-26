/*
 * Name: BGDetector.h
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 * 
 * Description: Background detector
 *
 * Created on: 21/03/2012
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#include "BGDetector.h"

BGDetector::BGDetector() {

	_Kinematics = Kinematics::getInstance();
	_ImageHandler = ImageHandler::getInstance();
	_Perception = Perception::getInstance();

	this->src_gray = cvCreateImage(cvSize(ImageInput::IMG_WIDTH, ImageInput::IMG_HEIGHT), IPL_DEPTH_8U, 1);
	this->imgSrc = cvCreateImage(cvSize(ImageInput::IMG_WIDTH, ImageInput::IMG_HEIGHT), ImageInput::BYTES_PER_CHANNEL, ImageInput::IMG_CHANNELS);
	this->imgFinal = cvCreateImage(cvSize(ImageInput::IMG_WIDTH, ImageInput::IMG_HEIGHT*2), ImageInput::BYTES_PER_CHANNEL, ImageInput::IMG_CHANNELS);

	this->keypoints_last = 0;
	this->descriptors_last = 0;
	this->keypoints_own = 0;
	this->descriptors_own = 0;
	this->keypoints_other = 0;
	this->descriptors_other = 0;
}

BGDetector::~BGDetector() {
}

void
BGDetector::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	this->setFreqTime(LONG_RATE);

}

void
BGDetector::step() {

	static int it=0;

	//_Perception->step();

	if (!isTime2Run())
		return;

	cout << "it bgdetector es " << it << endl;

	this->startDebugInfo();


	if(it == 5) {
		this->getFeatures(keypoints_own, descriptors_own, true, 1);
	} else if(it == 10) {
		this->getFeatures(keypoints_other, descriptors_other, true, 2);
		this->matching(keypoints_own, descriptors_own, keypoints_other, descriptors_other, true);
	}

	/*if(it == 5) {
		this->saveGoalFeatures(true);
	} else if(it == 20) {
		this->saveGoalFeatures(false);
	} else if(it == 50) {
		this->compareImages();
	}*/

	this->endDebugInfo();

	it++;
}

void
BGDetector::saveGoalFeatures(bool own_goal) {

	if(own_goal)
		this->getFeatures(keypoints_own, descriptors_own, false, 0);
	else
		this->getFeatures(keypoints_other, descriptors_other, false, 0);	
}

int
BGDetector::compareImages() {
	int num1, num2;

	/*Get features*/
	this->getFeatures(keypoints_last, descriptors_last, false, 0);

	num1 = this->matching(keypoints_own, descriptors_own, keypoints_last, descriptors_last, false);
	num2 = this->matching(keypoints_other, descriptors_other, keypoints_last, descriptors_last, false);

	cout << "los valores son " << num1 << " " << num2 << endl;

	if(num1 > num2)
		return BGDETECTOR_OWN;
	else
		return BGDETECTOR_OTHER;
}

void
BGDetector::getFeatures(CvSeq *& keypoints, CvSeq *& descriptors, bool debug, int debug_type) {
	CvSURFParams params = cvSURFParams(500, 1);
	CvMemStorage* storage = cvCreateMemStorage(0);
	int time = 100*1000; //microseconds

	/*Set camera upper and sleep*/
	_Perception->setCam(ImageInput::UPPER_CAMERA);
	usleep(time);
	_Perception->forceStep();

	/*Get curren image*/
	_ImageHandler->getImageRGB(this->imgSrc->imageData, false, 0);

	/*Convert to gayscale*/
	cvCvtColor(this->imgSrc, this->src_gray, CV_BGR2GRAY);

	/*Extract features*/
	cvExtractSURF(this->src_gray, 0, &keypoints, &descriptors, storage, params );

	cout << "num encontradas: " << keypoints->total << endl;

	/*Save image debug*/
	if(debug && debug_type == 1) {
		/*Prepare final img*/
		cvZero(this->imgFinal);
		cvSetImageROI(this->imgFinal, cvRect( 0, 0, ImageInput::IMG_WIDTH, ImageInput::IMG_HEIGHT) );
		cvAdd(this->imgSrc, this->imgFinal, this->imgFinal, NULL );
	} 

	if(debug && debug_type == 2) {
		/*Prepare final img*/
		cvSetImageROI(this->imgFinal, cvRect(0, ImageInput::IMG_HEIGHT, ImageInput::IMG_WIDTH, ImageInput::IMG_HEIGHT) );
		cvAdd(this->imgSrc, this->imgFinal, this->imgFinal, NULL );
		cvResetImageROI(this->imgFinal);
	}

	/*Set camera lower and sleep*/
	_Perception->setCam(ImageInput::LOWER_CAMERA);
	usleep(time);
}

int
BGDetector::matching(CvSeq * keypoints1, CvSeq * descriptors1, CvSeq * keypoints2, CvSeq * descriptors2, bool debug) {
		vector<int> ptpairs;


/*		CvPoint src_corners[4] = {{0,0}, {ImageInput::IMG_WIDTH,0}, {ImageInput::IMG_WIDTH, ImageInput::IMG_HEIGHT}, {0, ImageInput::IMG_HEIGHT}};
		CvPoint dst_corners[4];

		if(this->locatePlanarObject( keypoints1, descriptors1, keypoints2,	descriptors2, src_corners, dst_corners ))		{

			for(int i = 0; i < 4; i++ )	{
				CvPoint r1 = dst_corners[i%4];
				CvPoint r2 = dst_corners[(i+1)%4];
			
				cout << "r1 es " << r1.x << ", " << r1.y << endl;
				cout << "r2 es " << r2.x << ", " << r2.y << endl;

				cvLine(this->imgFinal, cvPoint(r1.x, r1.y+ImageInput::IMG_HEIGHT ),cvPoint(r2.x, r2.y+ImageInput::IMG_HEIGHT ), CV_RGB(0, 255, 0), 1, 8, 0  );
			}
		}*/

		/*Search matching*/
		findPairs(keypoints1, descriptors1, keypoints2, descriptors2, ptpairs);

		/*Draw debug*/
		if(debug) {
			for(int i = 0; i < (int)ptpairs.size(); i += 2 )	{
				CvSURFPoint* r1 = (CvSURFPoint*)cvGetSeqElem( keypoints1, ptpairs[i] );
				CvSURFPoint* r2 = (CvSURFPoint*)cvGetSeqElem( keypoints2, ptpairs[i+1] );
			
				/*Draw line*/
				cvLine(this->imgFinal, cvPointFrom32f(r1->pt), cvPoint(cvRound(r2->pt.x), cvRound(r2->pt.y+ImageInput::IMG_HEIGHT)), CV_RGB(0, 0, 255), 1, 8, 0  );
			}

			/*Save debug image*/
			cvSaveImage("/tmp/img_matching.jpg", this->imgFinal);
		}

		return (int)ptpairs.size();
}



double
BGDetector::compareSURFDescriptors( const float* d1, const float* d2, double best, int length )
{
    double total_cost = 0;
    assert( length % 4 == 0 );
    for( int i = 0; i < length; i += 4 )
    {
        double t0 = d1[i] - d2[i];
        double t1 = d1[i+1] - d2[i+1];
        double t2 = d1[i+2] - d2[i+2];
        double t3 = d1[i+3] - d2[i+3];
        total_cost += t0*t0 + t1*t1 + t2*t2 + t3*t3;
        if( total_cost > best )
            break;
    }
    return total_cost;
}

int
BGDetector::naiveNearestNeighbor( const float* vec, int laplacian,
                      const CvSeq* model_keypoints,
                      const CvSeq* model_descriptors )
{
    int length = (int)(model_descriptors->elem_size/sizeof(float));
    int i, neighbor = -1;
    double d, dist1 = 1e6, dist2 = 1e6;
    CvSeqReader reader, kreader;
    cvStartReadSeq( model_keypoints, &kreader, 0 );
    cvStartReadSeq( model_descriptors, &reader, 0 );

    for( i = 0; i < model_descriptors->total; i++ )
    {
        const CvSURFPoint* kp = (const CvSURFPoint*)kreader.ptr;
        const float* mvec = (const float*)reader.ptr;
        CV_NEXT_SEQ_ELEM( kreader.seq->elem_size, kreader );
        CV_NEXT_SEQ_ELEM( reader.seq->elem_size, reader );
        if( laplacian != kp->laplacian )
            continue;
        d = compareSURFDescriptors( vec, mvec, dist2, length );
        if( d < dist1 )
        {
            dist2 = dist1;
            dist1 = d;
            neighbor = i;
        }
        else if ( d < dist2 )
            dist2 = d;
    }
    if ( dist1 < 0.6*dist2 )
        return neighbor;
    return -1;
}

void
BGDetector::findPairs( const CvSeq* objectKeypoints, const CvSeq* objectDescriptors,
           const CvSeq* imageKeypoints, const CvSeq* imageDescriptors, vector<int>& ptpairs )
{
    int i;
    CvSeqReader reader, kreader;
    cvStartReadSeq( objectKeypoints, &kreader );
    cvStartReadSeq( objectDescriptors, &reader );
    ptpairs.clear();

    for( i = 0; i < objectDescriptors->total; i++ )
    {
        const CvSURFPoint* kp = (const CvSURFPoint*)kreader.ptr;
        const float* descriptor = (const float*)reader.ptr;
        CV_NEXT_SEQ_ELEM( kreader.seq->elem_size, kreader );
        CV_NEXT_SEQ_ELEM( reader.seq->elem_size, reader );
        int nearest_neighbor = naiveNearestNeighbor( descriptor, kp->laplacian, imageKeypoints, imageDescriptors );
        if( nearest_neighbor >= 0 )
        {
            ptpairs.push_back(i);
            ptpairs.push_back(nearest_neighbor);
        }
    }
}

int
BGDetector::locatePlanarObject( const CvSeq* objectKeypoints, const CvSeq* objectDescriptors,
                    const CvSeq* imageKeypoints, const CvSeq* imageDescriptors,
                    const CvPoint src_corners[4], CvPoint dst_corners[4] )
{
    double h[9];
    CvMat _h = cvMat(3, 3, CV_64F, h);
    vector<int> ptpairs;
    vector<CvPoint2D32f> pt1, pt2;
    CvMat _pt1, _pt2;
    int i, n;

    findPairs( objectKeypoints, objectDescriptors, imageKeypoints, imageDescriptors, ptpairs );
    n = ptpairs.size()/2;
    if( n < 4 )
        return 0;

    pt1.resize(n);
    pt2.resize(n);
    for( i = 0; i < n; i++ )
    {
        pt1[i] = ((CvSURFPoint*)cvGetSeqElem(objectKeypoints,ptpairs[i*2]))->pt;
        pt2[i] = ((CvSURFPoint*)cvGetSeqElem(imageKeypoints,ptpairs[i*2+1]))->pt;
    }

    _pt1 = cvMat(1, n, CV_32FC2, &pt1[0] );
    _pt2 = cvMat(1, n, CV_32FC2, &pt2[0] );
    if( !cvFindHomography( &_pt1, &_pt2, &_h, CV_RANSAC, 5 ))
        return 0;

    for( i = 0; i < 4; i++ )
    {
        double x = src_corners[i].x, y = src_corners[i].y;
        double Z = 1./(h[6]*x + h[7]*y + h[8]);
        double X = (h[0]*x + h[1]*y + h[2])*Z;
        double Y = (h[3]*x + h[4]*y + h[5])*Z;
        dst_corners[i] = cvPoint(cvRound(X), cvRound(Y));
    }

    return 1;
}
