#include "MarkovGrid.h"

MarkovGrid::MarkovGrid()
{
	init();
}

MarkovGrid::~MarkovGrid()
{

}

void
MarkovGrid::predict(float movx, float movy, float movt)
{

	float angRel;
	float rho, w;

	angRel = atan2(movy, movx);
	rho = sqrt(movx*movx+movy*movy);
	w = rho / (2.0 * CELL_SIZE);

	float bx, by;

	for(int i=0; i<gridrows; i++)
		for(int j=0; j<gridcolumns; j++)
		{
			grid[i][j].theta->predict(movx, movy, movt);

			FromGridtoCoords(i, j, bx, by);

			for(int k=-1; k<=1; k++)
				for(int l=-1; l<=1; l++)
					if(!((k==0) && (l==0)))
						if(((i+k)>0) && ((i+k)<gridrows) && ((j+l)>0) && ((j+l)<gridcolumns))
						{

							float angAbs, ang2me, angDif;
							float c, P, ax, ay;

							FromGridtoCoords(i+k, j+l, ax, ay);

							angAbs =  normalizePi(angRel + grid[i+k][j+l].theta->t());
							ang2me = atan2(by-ax, bx-ax);
							angDif = fabs(normalizePi(ang2me-angAbs));

							P = sqrt(grid[i+k][j+l].theta->ut());
							if(P<(M_PI/16.0)) P=(M_PI/16.0);

							c =  fabs(normalAng(angDif, 0.0, P));

							grid[i][j].prob = grid[i][j].prob  + grid[i+k][j+l].prob * w * c;

						}
		}
	normalize();
}
/*
	float trans=sqrt(movx*movx+movy*movy);
	float dx, dy;
	float w = trans / CELL_SIZE;
	float a;

	dx = trans * sin(grid[i][j].theta);
	dy = trans * cos(grid[i][j].theta);
	a = atan2(dy, dx);

	for(int i=0; i<gridrows; i++)
		for(int j=0; j<gridcolumns; j++)
		{
			float x,y;

			grid[i][j].theta->predict(movx, movy, movt);
			FromGridtoCoords(i, j, x, y);

			for(int k=-1; k<=1; k++)
				for(int l=-1; l<=1; l++)
					if(!((k==0) && (l==0)))
					{
						if(((i+k)>0) && ((i+k)<gridrows) && ((j+l)>0) && ((j+l)<gridcolumns))
						{
							float xn,yn;
							float P;
							float angle;

							FromGridtoCoords(i+k, j+l, xn, yn);
							angle = atan2(yn-y, xn-x);

							P = sqrt(grid[i][j].theta->ut());

							if(P<(M_PI/8.0)) P=(M_PI/8.0);

							float c =  normal(fabs(normalizePi(angle-a)), grid[i][j].theta->t(), P);

							grid[i][j].prob = grid[i][j].prob  + grid[i+k][j+l].prob * w * c;
						}
					}
		}

	normalize();

	for(int i=0; i<gridrows; i++)
		for(int j=0; j<gridcolumns; j++)
		{
			float x,y;
			FromGridtoCoords(i, j, x, y);

			cerr<<"["<<i<<","<<j<<"("<<x<<","<<y<<")] =  "<<grid[i][j].prob<<endl;
		}
}
*/
void
MarkovGrid::correct(HPoint2D &perceived_pos, HPoint2D &official_pos, float angStdev, float disStdev, bool ambiguous)
{

	HPoint2D official_pos2 = official_pos;
	official_pos2.x = -official_pos2.x;
	official_pos2.y = -official_pos2.y;

	for(int i=0; i<gridrows; i++)
		for(int j=0; j<gridcolumns; j++)
		{
			float p;

			if(ambiguous)
			{
				float p1,p2;
				p1 = getLandmarkProb(grid[i][j].pos_x, grid[i][j].pos_y, perceived_pos, official_pos, disStdev);
				p2 = getLandmarkProb(grid[i][j].pos_x, grid[i][j].pos_y, perceived_pos, official_pos2, disStdev);
				if(p2>p1)
				{
					grid[i][j].prob = grid[i][j].prob * p2;
					HPoint3D lm,obs;

					lm.X = official_pos2.x;
					lm.Y = official_pos2.y;
					lm.Z = 0.0;
					obs.X = perceived_pos.x;
					obs.Y = perceived_pos.y;
					obs.Z = 0.0;

					MatrixCM Ro(1);
					Ro.sete(0, 0, angStdev*angStdev);

					grid[i][j].theta->correct(lm, obs, Ro);
				}
				else
				{
					grid[i][j].prob = grid[i][j].prob * p1;
					HPoint3D lm,obs;

					lm.X = official_pos.x;
					lm.Y = official_pos.y;
					lm.Z = 0.0;
					obs.X = perceived_pos.x;
					obs.Y = perceived_pos.y;
					obs.Z = 0.0;

					MatrixCM Ro(1);
					Ro.sete(0, 0, angStdev*angStdev);

					grid[i][j].theta->correct(lm, obs, Ro);
				}

			}else
			{
				float p1;
				p1 = getLandmarkProb(grid[i][j].pos_x, grid[i][j].pos_y, perceived_pos, official_pos, disStdev);
				grid[i][j].prob = grid[i][j].prob * p1;
				HPoint3D lm,obs;

				lm.X = official_pos.x;
				lm.Y = official_pos.y;
				lm.Z = 0.0;
				obs.X = perceived_pos.x;
				obs.Y = perceived_pos.y;
				obs.Z = 0.0;

				MatrixCM Ro(1);
				Ro.sete(0, 0, angStdev*angStdev);

				grid[i][j].theta->correct(lm, obs, Ro);

			}
		}
}

float
MarkovGrid::getLandmarkProb(float x, float y, HPoint2D &obs, HPoint2D &lm, float dist_std_dev)
{
	//convert global landmark to local coordinates with respect to this coordinate
	HPoint2D rel_lm = lm;
	rel_lm.x -= x;
	rel_lm.y -= y;

	float obslenght = sqrt(obs.x*obs.x+obs.y*obs.y);
	float rellenght = sqrt(rel_lm.x*rel_lm.x+rel_lm.y*rel_lm.y);

	float dist_diff = fabs(obslenght - rellenght);

	float prob = getProb(dist_diff, dist_std_dev);

	return prob;
}

float
MarkovGrid::getProb(float x, float std_dev)
{
	//get probability from a normal pdf with mean at 0
	//float normalizer = (1.0 / ((double)std_dev * sqrt(2*M_PI)));
	float exponential = exp(-pow((double)x,2) / (2*pow((double)std_dev,2)));

	// is it better with or without normalization? this way it is between 0 and 1,
	// good for apply weight to noise
	return exponential;
}


void
MarkovGrid::normalize()
{
	float sumprobs = 0.0;

	for(int i=0; i<gridrows; i++)
		for(int j=0; j<gridcolumns; j++)
			sumprobs += grid[i][j].prob ;

	if(sumprobs!=0.0)
	{
		for(int i=0; i<gridrows; i++)
			for(int j=0; j<gridcolumns; j++)
			{
				grid[i][j].prob = grid[i][j].prob / sumprobs;
				if(grid[i][j].prob < 0.001/(gridrows*gridcolumns)) grid[i][j].prob =  0.001/(gridrows*gridcolumns);
			}
	}else
	{
		resetUniform();
	}

//	for(int i=0; i<gridrows; i++)
//		for(int j=0; j<gridcolumns; j++)
	//			cerr<<"["<<i<<","<<j<<"] "<<grid[i][j].prob<<"\t";
	//	cerr<<endl;
}


void
MarkovGrid::correct(HPoint3D &pos, MatrixCM R)
{
	float x, y, t;
	int si, sj;
	ToGridfromCoords(pos.X, pos.Y, si, sj);


	for(int i=0; i<gridrows; i++)
		for(int j=0; j<gridcolumns; j++)
		{
			if((i==si) && (j==sj))
			{
				grid[i][j].prob = grid[i][j].prob * 10.0;
				MatrixCM Ro(1);
				Ro.sete(0, 0, R.e(3,3));
				grid[i][j].theta->correct(pos.Z, Ro);
			}
			else
				grid[i][j].prob = grid[i][j].prob * 0.1;
		}

	normalize();
}


void
MarkovGrid::restart()
{
	resetUniform();
}


void
MarkovGrid::addPosition(float x, float y, float theta)
{
	int si, sj;
	ToGridfromCoords(x, y, si, sj);

	grid[si][sj].prob = 1.0;
	grid[si][sj].theta->t(theta);
	grid[si][sj].theta->ut(0.01);

}

void
MarkovGrid::restart(list<Pose2D> &initPoses)
{
	resetUniform();

	list<Pose2D>::iterator it;
	for(it=initPoses.begin(); it!=initPoses.end();++it)
	{
		float newposx, newposy, newpost;

		newposx = (*it).translation.x;
		newposy = (*it).translation.y;
		newpost = (*it).rotation;

		addPosition(newposx, newposy, newpost);
	}

	normalize();
}

void
MarkovGrid::init()
{
	gridrows = (int)(6000.0/CELL_SIZE);
	gridcolumns = (int)(4000.0/CELL_SIZE);

	grid = new tgridcell*[gridrows];
	for(int i = 0; i < gridrows; i++)
	{
		grid[i] = new tgridcell[gridcolumns];
		for(int j = 0; j < gridcolumns; j++)
		{
			grid[i][j].theta = new EKF_1D();

			float x,y;
			FromGridtoCoords(i,j,x,y);

			grid[i][j].pos_x = x;
			grid[i][j].pos_y = y;
			grid[i][j].prob = 1.0/((float)(gridrows*gridcolumns));

			MatrixCM sinit(1);
			sinit.sete(0,0,0.0);
			MatrixCM Pinit(1);
			sinit.sete(0,0,M_PI*M_PI);

			grid[i][j].theta->init(sinit, Pinit, x, y);

		}
	}

	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
		{
			int x, y;

			x = (i-1) * CELL_SIZE;
			y = (j-1) * CELL_SIZE;

			if((i!=1) && (j!=1))
			nangles[i*3+j] = atan2((float)y, (float)x);
		}


}

void
MarkovGrid::resetUniform()
{
	for(int i = 0; i < gridrows; i++)
		for(int j = 0; j < gridcolumns; j++)
		{
			grid[i][j].prob = 1.0/((float)(i*j));
			grid[i][j].theta->restart();
		}
}

void
MarkovGrid::resetClear()
{
	for(int i = 0; i < gridrows; i++)
		for(int j = 0; j < gridcolumns; j++)
		{
			grid[i][j].prob = 0.001/(gridrows*gridcolumns);
			grid[i][j].theta->restart();
		}
}


float MarkovGrid::evaluate(float x, float y, float t)
{
	/*float tq;

	float media = 1.0/(gridrows*gridcolumns);
	tgridcell *cell = getCell(x, y);

	tq = cell->prob;

	if(tq>1.0)
		tq = 1.0;
*/

	//cerr<<"evaluation for ("<<x<<", "<<y<<", "<<t<<") ";
	float tq = 0.0;
	float dx, dy;
	for(dx = -CELL_SIZE; dx<=CELL_SIZE; dx=dx+CELL_SIZE)
		for(dy = -CELL_SIZE; dy<=CELL_SIZE; dy=dy+CELL_SIZE)
		{
			tgridcell *cell = getCell(x+dx, y+dy);
			//cerr<<" "<<cell->prob<<" ";
			tq = tq + cell->prob * (M_PI-normalizePi(fabs(t-cell->theta->t())))/M_PI * normalAng(normalizePi(fabs(t-cell->theta->t())), 0.0, t-cell->theta->ut());

			if(tq<cell->prob) tq = cell->prob;
			//tq = tq + cell->prob;// * (M_PI-normalizePi(fabs(t-cell->theta->t())))/M_PI * normal(normalizePi(fabs(t-cell->theta->t())), 0.0, t-cell->theta->ut());
		}
	//tq = tq/9.0;

	//cerr<<" = "<<tq<<endl;
	return tq;
}

void MarkovGrid::ToGridfromCoords(float x, float y, int &i, int &j)
{
	i = (int)rint((x - (CELL_SIZE/2.0) + 3000.0)/CELL_SIZE);
	j = (int)rint((y - (CELL_SIZE/2.0) + 2000.0)/CELL_SIZE);

	if(i<0) i = 0;
	if(i>=gridrows-1) i = gridrows-1;
	if(j<0) j = 0;
	if(j>=gridcolumns-1) j = gridcolumns-1;

}

void MarkovGrid::FromGridtoCoords(int i, int j, float &x, float &y)
{
	x = ((float)i * CELL_SIZE) + (CELL_SIZE/2.0) - 3000.0;
	y = ((float)j * CELL_SIZE) + (CELL_SIZE/2.0) - 2000.0;
}

float
MarkovGrid::getProbIJ(int i, int j)
{
	return grid[i][j].prob;
}

float
MarkovGrid::getProbXY(float x, int y)
{
	int i,j;
	ToGridfromCoords(x,y, i,j);

	return getProbIJ(i,j);
}
float
MarkovGrid::getThetaIJ(int i, int j)
{
	return grid[i][j].theta->t();
}

float
MarkovGrid::getThetaXY(float x, int y)
{
	int i,j;
	ToGridfromCoords(x,y, i,j);

	return grid[i][j].theta->t();
}


void
MarkovGrid::getGridPos(list<tcellcand> &cands)
{
	list<tcellcand>::iterator icands, isimilar;
	float diff = 0.2;

	float higher=0.0;

	cands.clear();

	for(int i=0; i<gridrows; i++)
		for(int j=0; j<gridcolumns; j++)
		{
			float x,y,dist,diff;
			bool add = true;
			FromGridtoCoords(i, j, x, y);

			float prob = grid[i][j].prob;
			if(higher<prob) higher=prob;

			if(prob>0.2)
			{
				if(cands.size()==0)
				{
					tcellcand aux;
					aux.prob = prob;
					aux.i = i;
					aux.j = j;
					aux.x = x;
					aux.y = y;
					aux.uxy = CELL_SIZE*CELL_SIZE;
					aux.ut =  grid[i][j].theta->ut();
					aux.t = grid[i][j].theta->t();
					aux.covered = false;
					cands.push_back(aux);

				}else
				{
					for(icands = cands.begin(); icands!=cands.end();++icands)
					{
						dist = ((*icands).x-x)*((*icands).x-x)+((*icands).y-y)*((*icands).y-y);
						diff = fabs(normalizePi(grid[i][j].theta->t() - (*icands).t));

						if((dist<1000.0) && (diff<pi_2))
						{
							if((*icands).prob < prob)
							{
								(*icands).prob = prob;
								(*icands).i = i;
								(*icands).j = j;
								(*icands).x = x;
								(*icands).y = y;
								(*icands).uxy = CELL_SIZE*CELL_SIZE;
								(*icands).ut =  grid[i][j].theta->ut();
								(*icands).t = grid[i][j].theta->t();
								(*icands).covered = false;
							}
						}else
						{
							tcellcand aux;
							aux.prob = prob;
							aux.i = i;
							aux.j = j;
							aux.x = x;
							aux.y = y;
							aux.uxy = CELL_SIZE*CELL_SIZE;
							aux.ut =  grid[i][j].theta->ut();
							aux.t = grid[i][j].theta->t();

							//cerr<<"New Cand at ["<<x<<","<<y<<","<<grid[i][j].theta->t()<<"]: "<<aux.t<<endl;

							aux.covered = false;
							cands.push_back(aux);
						}
					}
				}
			}
		}
	/*cerr<<"================================"<<endl;
	for(icands = cands.begin(); icands!=cands.end();++icands)
	{
		cerr<<"Candidato en ["<<(*icands).x<<","<<(*icands).y<<"]"<<endl;
	}*/

}

tgridcell *
MarkovGrid::getCell(float x, float y)
{

	int si, sj;
	ToGridfromCoords(x, y, si, sj);

	//cerr<<"{"<<si<<","<<sj<<"}";
	return &(grid[si][sj]);

}


void
MarkovGrid::addDebugInfo(bica::ShapeList &lista)
{
	for(int i=0; i<gridrows; i++)
		for(int j=0; j<gridcolumns; j++)
		{
			bica::RectanglePtr a(new bica::Rectangle);
			bica::Point3DPtr p2d(new bica::Point3D);
			bica::Point3DPtr p22(new bica::Point3D);

			p2d->x=(i*CELL_SIZE)-3000.0;
			p2d->y=(j*CELL_SIZE)-2000.0;
			p2d->z=500.0;

			p22->x = ((i+1)*CELL_SIZE)-3000.0;
			p22->y = ((j+1)*CELL_SIZE)-2000.0;
			p22->z=500.0;


			//cerr<<" ["<<grid[i][j].theta->t()<<"] ";
			float media;

			media = 1.0/((float)gridrows*(float)gridcolumns);

			short value = ((short)((grid[i][j].prob/media)*255.0))/2;

			if (value>255)
				value = 255;
			if (value<0.0)
				value = 0;

			//cerr<<" "<<value;
			a->p1 = p2d;
			a->p2 = p22;
			a->color = bica::RED;
			a->filled = true;
			a->opacity = value;
			a->accKey = "c";
			a->label = "KF";
			lista.push_back(a);
		}
	//cerr<<endl;
}
