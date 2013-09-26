#include "Movie.h"

Movie::Movie()
{
	_Body = Body::getInstance();
	_Music = Music::getInstance();
	_TextSpeech = TextSpeech::getInstance();
	_LedsControl = LedsControl::getInstance();
	_FootContact = FootContact::getInstance();
	stiffness = false;
	numBreak = 0;
	lineStart=1;
	state = Initial;
}

Movie::~Movie()
{

}

void Movie::stop(){
	if (not _Music->isStopped()) _Music->stop();
	if (not _TextSpeech->isStopped())_TextSpeech->stop();
	/*PENSAR SI LA PARTE DEL BODY HAY QUE CONTROLAR SI HA TERMINADO ANTES.*/
	if (not _Body->isStopped())_Body->stop();
	IsplayPress = false;
	IsSetFileMovie=false;
	numBreak=-1;
}
int Movie::pausePress(){
	FirstTime = true;
	IsplayPress = false;
	return lineNow;
}

int Movie::previousPress(){

	numBreak--;
	if (numBreak<0)	numBreak=0;
	lineStart = Breakpoint[numBreak];
	FirstTime = true;
	// After it was with false. With true, begin next breakpoint
	IsplayPress = true;
	//	numBreak--;
	lineNow = lineStart;

    playPress(lineNow);

	return lineStart;
}

int Movie::nextPress(){
	numBreak++;
	FirstTime = true;
	// After it was with false. With true, begin next breakpoint
	IsplayPress = true;

	lineStart = Breakpoint[numBreak];
	lineNow = lineStart;

    playPress(lineNow);

	return lineStart;
}

void Movie::playPress(int line)
{
	printf("Press PLAY Start in Line:%d\n",line);
	IsplayPress = true;
	lineStart=line;
	FirstTime = true;
	lineNow = lineStart;
	
	for (int i=0; i<NUM_BREAK; i++) {
		if (line >= Breakpoint[i]) {
			numBreak = i;
			break;
		}
	}
	

	IsPressLeft =true;
    IsPressRight =true;
    IsPressChest =true;


//	infile.close();
//	infile.open (string(fileMovie).c_str(), ifstream::in);
}

std::vector<int> Movie::getBreakpoints() {
	std::vector<int> vector;
	for (int i=0; i<NUM_BREAK; i++) {
		if (Breakpoint[i] == 0) break;
		else vector.push_back(Breakpoint[i]);
	}

	return vector;
}

void Movie::Initial_state_code(void)
{
	//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
	//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void Movie::setFileMovie_state_code(void)
{
	//BUILDER COMMENT. DO NOT REMOVE. begin setFileMovie_state_code
	//BUILDER COMMENT. DO NOT REMOVE. end setFileMovie_state_code
}

void Movie::Running_state_code(void)
{
	//BUILDER COMMENT. DO NOT REMOVE. begin Running_state_code
	if (IsplayPress){

		string line;
		if (lineStart!=1 && FirstTime){
			/*Recorrer hasta la posicion -1 que se quiera empezar*/
				printf("vamos a recorrer porque tenemos que empezar a leer por la linea %d y vamos por la linea %d\n",lineStart,lineNow);
			int cont = 1;
			while (!infile.eof() && cont<lineStart){
				getline(infile, line);
				cont++;
			}
			FirstTime=false;
		}

		if (!infile.eof()){
			getline(infile, line);
			get_command(line);
			lineNow++;
			//		printf("lineNow:%d\n",lineNow);
		}else{
			//	printf("Fin de Fichero\n");
			state=Initial;
			infile.close();
		}
	}
	//BUILDER COMMENT. DO NOT REMOVE. end Running_state_code
}

void Movie::Waiting_state_code(void)
{
	//BUILDER COMMENT. DO NOT REMOVE. begin Waiting_state_code

	if (_Music->isStopped()) IsFinishMusic=true;
	if (_TextSpeech->isStopped())IsFinishTalk=true;
	if (_Body->isStopped()) IsFinishMov = true;
	if (_FootContact->isPressFootLeft()) IsPressLeft = true;
	if (_FootContact->isPressFootRight()) IsPressRight= true;
	if (_FootContact->isPressChest()) IsPressChest = true;

	//BUILDER COMMENT. DO NOT REMOVE. end Waiting_state_code
}

bool Movie::Initial2setFileMovie0_transition_code(void)
{
	//BUILDER COMMENT. DO NOT REMOVE. begin Initial2setFileMovie0_transition_code
	if (IsSetFileMovie) return true;
	else return false;//BUILDER COMMENT. DO NOT REMOVE. end Initial2setFileMovie0_transition_code
}

bool Movie::setFileMovie2Running0_transition_code(void)
{
	//BUILDER COMMENT. DO NOT REMOVE. begin setFileMovie2Running0_transition_code
	return true;//BUILDER COMMENT. DO NOT REMOVE. end setFileMovie2Running0_transition_code
}

bool Movie::Running2Waiting0_transition_code(void)
{
	//BUILDER COMMENT. DO NOT REMOVE. begin Running2Waiting0_transition_code

	if (cmd=="wait") return true;
	else return false;//BUILDER COMMENT. DO NOT REMOVE. end Running2Waiting0_transition_code
}

bool Movie::Waiting2Running0_transition_code(void)
{
	//BUILDER COMMENT. DO NOT REMOVE. begin Waiting2Running0_transition_code

	if (OrderWait=="task"){
		if (IsFinishMusic && IsFinishTalk && IsFinishMov)return true;
		else return false;
	}
	else if (OrderWait=="time"){
		time(&timenow);
		if (difftime(timenow,timewait)>=twait) return true;
		else return false;
	}
	else if (OrderWait=="press"){
		if (IsPressLeft && IsPressRight && IsPressChest) return true;
		else return false;
	}
	return true;
	//BUILDER COMMENT. DO NOT REMOVE. end Waiting2Running0_transition_code
}

void
Movie::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2setFileMovie0_transition_code()) {
				state = setFileMovie;
			}
		}

		break;
	case setFileMovie:

		if (isTime2Run()) {
			setFileMovie_state_code();

			if (setFileMovie2Running0_transition_code()) {
				state = Running;
			}
		}

		break;
	case Running:

		if (isTime2Run()) {
			Running_state_code();

			if (Running2Waiting0_transition_code()) {
				state = Waiting;
			}
		}

		_Body->step();
		_Music->step();
		_TextSpeech->step();
		_LedsControl->step();
		break;
	case Waiting:
		_TextSpeech->step();
		_Music->step();
		_Body->step();
		_LedsControl->step();
		if (isTime2Run()) {
			Waiting_state_code();

			if (Waiting2Running0_transition_code()) {
				state = Running;
			}
		}

		break;
	default:
		cout << "[Movie::step()] Invalid state\n";
	}
}

void Movie::get_command(string line){
	/*-Obtenemos el comando que hay que ejecutar
	 *-Modulamos el componente
	 *-Arrancamos el componente
	 *-Los comandos pueden venir:
	 *-
	 * mov Movement (se pondrá el nombre del movimiento, y lo cogerá del fichero correspondiente)
   music file.mp3 (se pondrá la ruta donde esta el movimiento que se quiere ejecutar)
   talk file file.txt (se pondrá la ruta donde esta el movimiento que se quire ejecutar)
   talk text "Hello my name es Nao" (se pondrá el texto que quiere que el robot lea)
   walk Vx Vy Vz (Velocidades para andar)
   lights led_id color (Marcando set, se pondrá el color en RGB y el tiempo que se quiere)
   wait time 3 (El tiempo que hay que esperar para seguir)
   wait task mov,music,walk ... (Las tareas que hay que esperar antes de seguir)
   stop task mov...(Las tareas que hay que parar)
   stop task all
   begin nombre (Define el inicio de un group de comandos y el nombre de este)
   end nombre (Define el fin de un group de comandos y el nombre de este)
	 */

	list<string> tokens = getTokens(line,' ');

	/* args.pop_front();
			string Order = args.front().c_str();*/
	string Order;
	cmd=tokens.front().c_str();

	if (tokens.front().compare("begin") == 0){
	  // TODO Implementarlo
	  printf("Begin Group = %d", lineNow);
	}else if (tokens.front().compare("end") == 0){
	  // TODO Implementarlo
	  printf("End Group = %d", lineNow);
	}else if (tokens.front().compare("mov") == 0){
		if (not stiffness){
			_Body->setStiffness(1.0);
			stiffness = true;
		}
		tokens.pop_front();
		_Body->selKick(tokens.front().c_str());
		//	_Body->doFixMove(tokens.front().c_str());
		_Body->step();
		//	printf("Mov Activado\n");
	}
	else if (tokens.front().compare("music") == 0){
		tokens.pop_front();
		_Music->setFileMP3(tokens.front().c_str());
		_Music->playPress();
		_Music->step();
		//		printf("Music Activado\n");
	}
	else if (tokens.front().compare("talk") == 0){
		tokens.pop_front();
		Order = tokens.front().c_str();
		tokens.pop_front();
		string miline,text;
		if (Order=="file"){ /*Abrir el fichero y pasarselo como texto*/
			ifstream infiletext;
			text="";
			infiletext.open (tokens.front().c_str(), ifstream::in);
			while (!infiletext.eof()){
				getline(infiletext,miline);
				text = text + miline;
			}
			//	printf("Vamos a leer el texto %s\n",text.c_str());
			_TextSpeech->setmiText(text);
			_TextSpeech->step();
		}else if (Order=="text"){/*Juntar todo lo que viene separado por espacios y un sólo string*/
			text="";
			int total=tokens.size();
			for (int i=0;i<total;i++){
				text = text + " " + tokens.front().c_str();
				tokens.pop_front();
			}
			_TextSpeech->setmiText(text);
			_TextSpeech->step();
		}else if (Order=="language"){
			string language = tokens.front().c_str();
			_TextSpeech->setLanguage(language);
		}else if (Order=="voice"){
			string voice = tokens.front().c_str();
			_TextSpeech->setVoice(voice);
		}else if (Order=="volume"){
			int volume = atoi(tokens.front().c_str())/100;
			_TextSpeech->setVolume(volume);

		}
		else printf("Error en la codificación de talk en el fichero.\n Debe ser talk file Ruta or talk text hola como estas..\n");

	}
	else if (tokens.front().compare("walk") == 0){
		if (not stiffness){
			_Body->setStiffness(1.0);
			stiffness = true;
		}
		tokens.pop_front();
		try{
			tokens.pop_front();
			float v = atof(tokens.front().c_str());
			tokens.pop_front();
			float l = atof(tokens.front().c_str());
			tokens.pop_front();
			float w = atof(tokens.front().c_str());
			_Body->setVelV(v);
			_Body->setVelL(l);
			_Body->setVelW(w);
			_Body->selKick(-1);
			//	_Body->step();
		}
		catch (AL::ALError& e) {
			cerr << "Movie::Movie [Walk]" << e.toString() << endl;
		}

		//			printf("walking\n");
	}
	else if (tokens.front().compare("led") == 0){
		tokens.pop_front();
		int led = atoi(tokens.front().c_str());
		tokens.pop_front();
		int modo = atoi(tokens.front().c_str());
		bool automatic = false;
		int color;
		if (modo==2){
			color = rand() % 8;
			automatic=true;
		}else if (modo==1){
			tokens.pop_front();
			color = atoi(tokens.front().c_str());
		}else color = 0;
		_LedsControl->setLed(led,color,true,automatic);
		//	_LedsControl->step();
		//	printf("led\n");
	}
	else if (tokens.front().compare("wait") == 0){
		tokens.pop_front();
		Order = tokens.front().c_str();
		if (Order=="time"){ /*Poner contador a 0 y comparar en Wait el tiempo.*/
			OrderWait="time";
			tokens.pop_front();
			twait = atof(tokens.front().c_str());
			time(&timewait);
			state=Waiting;
		}else if (Order=="task"){/*Separar las tareas por la , en las diferentes*/
			OrderWait="task";
			/*Obtener las tareas*/
			tokens.pop_front();
			list<string> tasks = getTokens(tokens.front().c_str(),',');
			string task;
			for (unsigned int i=0;i<tasks.size();i++){
				task=tasks.front().c_str();
				//printf("Vamos a esperar a %s\n",task.c_str());
				if (task=="music") IsFinishMusic=false;
				else if (task=="talk") IsFinishTalk=false;
				else if (task=="walk") IsFinishWalk=false;
				else if (task=="mov") IsFinishMov=false;

				tasks.pop_front();
			}
			state=Waiting;
		}else if(Order=="press"){/*Espera a que se pulse uno de los botones (right,left,chest*/
			OrderWait="press";
			tokens.pop_front();
			_FootContact->resetPress();
			string button = tokens.front().c_str();
			if (button=="left") IsPressLeft = false;
			else if (button=="right") IsPressRight= false;
			else if (button=="chest") IsPressChest = false;
		}
		else printf("Error en la codificación de walk en el fichero.\n Debe ser wait time X or wait task NameTask\n");

	}
	else if (tokens.front().compare("stiffness") == 0){
		tokens.pop_front();
		Order = tokens.front().c_str();
		if (Order=="on"){ _Body->setStiffness(1.0); stiffness=true;}
		else if (Order=="off"){ _Body->setStiffness(0.0);stiffness=false;}
	}

	else if (tokens.front().compare("stop") == 0){
		tokens.pop_front();
		Order = tokens.front().c_str();
		tokens.pop_front();
		string task = tokens.front().c_str();
		if (Order=="task"){ /*Poner contador a 0 y comparar en Wait el tiempo.*/
			if (task=="all"){ /*Paramos todos los componentes*/
				_Music->stop();
				/*Revisar si hay que comprobar algo antes */
				_Body->stop();
				_TextSpeech->stop();
			}else{ /*Ver que task hay que parar, separados por ','*/
				/*Obtener las tareas*/
				list<string> tasks = getTokens(tokens.front().c_str(),',');
				for (unsigned int i=0;i<tasks.size();i++){

					task=tasks.front().c_str();
					if (task=="music"){
						_Music->stop();
					}
					else if (task=="talk"){
						_TextSpeech->stop();
					}
					else if (task=="walk"){
						_Body->stop();
					}
					else if (task=="mov"){
						_Body->stop();
					}

					tasks.pop_front();
				}
			}
		}else printf("Error en la codificación de stop en el fichero.\n Debe ser stop task Nametask o stop task all\n");

		//	printf("stop\n");
	}


}
bool Movie::saveFile(string file, string cont){
	try{
		ofstream outfile (string(file).c_str(),ofstream::out);
		outfile << cont.c_str() << endl;
		outfile.close();

		/*	 infile.close();
		 infile.open (string(file).c_str(), ifstream::in);*/
		fileMovie = file;
		InitBreakpoint(file);


		IsSetFileMovie = true;

		IsplayPress = false;

		IsFinishMov=true;
		IsFinishMusic=true;
		IsFinishWait=true;
		IsFinishTalk=true;
		IsFinishLeghts=true;
		IsFinishWalk=true;
		IsPressLeft =true;
		IsPressRight =true;
		IsPressChest =true;
		return true;
	}catch(...) {
		return false;
	}
}

void Movie::InitBreakpoint(string file){
	filebreak.close();
	filebreak.open(string(file).c_str(),ifstream::in);
	string line;
	int miline=1;
	numBreak=1;
	Breakpoint[0]=1;
	for (int i=1;i<NUM_BREAK;i++) Breakpoint[i]=0;

	while (!filebreak.eof()){
		getline(filebreak, line);
		list<string> tokens = getTokens(line,' ');
		if (tokens.front().compare("begin") == 0){
			Breakpoint[numBreak]=miline;
			numBreak++;
		}
		miline++;
	}
	numBreak=-1;

}
void Movie::setFile(string file)
{
	fileMovie = file;
	//	printf("Empezando....SetFileMovie\n");
	IsSetFileMovie = true;
	string line;
	//	infile.open (string(("/home/nao/movie/demo.movie")).c_str(), ifstream::in);
	/*	infile.close();
	infile.open (string(file).c_str(), ifstream::in);*/
	
	InitBreakpoint(file);

	IsFinishMov=true;
	IsFinishMusic=true;
	IsFinishWait=true;
	IsFinishTalk=true;
	IsFinishLeghts=true;
	IsFinishWalk=true;
	IsPressLeft =true;
	IsPressRight =true;
	IsPressChest =true;

	IsplayPress = false;
	
	// Open file
        infile.close();
        infile.open (string(fileMovie).c_str(), ifstream::in);

}


list<string> Movie::getTokens(string data,char delimiter) {
	list<string> ret;
	string msg = data;
	istringstream iss(msg);
	string token;

	while (getline(iss, token, delimiter))
		ret.push_back(token);
	return ret;
}
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
void
Movie::setFile(const string& file, const Ice::Current& c)
{
	setFile(file);
}

void
Movie::stop(const Ice::Current& c)
{
	stop();
}

void
Movie::playPress(int line, const Ice::Current& c)
{
	playPress(line);
}

int
Movie::pausePress(const Ice::Current& c)
{
	return pausePress();
}

int
Movie::previousPress(const Ice::Current& c)
{
	return previousPress();
}

int
Movie::nextPress(const Ice::Current& c)
{
	return nextPress();
}

bool
Movie::saveFile(const string& file, const string& cont, const Ice::Current& c)
{
	return saveFile(file, cont);
}

int 
Movie::getLineNow(const Ice::Current& c){
	return getLineNow();
}

int
Movie::getBatteryLevel(){
    psentinel = getParentBroker()->getProxy("ALSentinel");
    return psentinel->call<int>("getBatteryLevel");
}

int
Movie::getBatteryLevel(const Ice::Current& c){
    return getBatteryLevel();
}


string
Movie::getAngleRobot(const string& jointsChain, const Ice::Current& c)
{
	list<string> args = getTokens(jointsChain, ':');

	ALValue img;
	float HeadYaw,HeadPitch,RShoulderPitch,RShoulderRoll,RElbowYaw,RElbowRoll,
	LShoulderPitch,LShoulderRoll,LElbowYaw,LElbowRoll, RHipYawPitch,RHipRoll,RHipPitch,
	RKneePitch,RAnklePitch,RAnkleRoll,LHipYawPitch,LHipRoll,LHipPitch,LKneePitch,LAnklePitch,LAnkleRoll;

	args.pop_front();
	string type = args.front().c_str();

	try {
		AL::ALPtr<AL::ALMotionProxy> pmotion = getParentBroker()->getMotionProxy();

		// IMPORTANTE: Quitar!! Puesto temporalmente...
		//bool USE_SENSOR = true;

		printf("--Obteniendo Angulos.....\n");
		std::stringstream out;
		printf("Type : %s", type.c_str());
		if (type=="All" || type=="Head"){
			HeadYaw = pmotion->getAngles("HeadYaw",USE_SENSOR)[0]*100;
			HeadPitch = pmotion->getAngles("HeadPitch",USE_SENSOR)[0]*100;
			out << int(HeadYaw) << ":" << int(HeadPitch) << ":";
		}
		if (type=="All" || type=="RArm"){
			//CameraSelect = pmotion->getAngle("CameraSelect")*100;
			RShoulderPitch = pmotion->getAngles("RShoulderPitch",USE_SENSOR)[0]*100;
			RShoulderRoll = pmotion->getAngles("RShoulderRoll",USE_SENSOR)[0]*100;
			RElbowYaw = pmotion->getAngles("RElbowYaw",USE_SENSOR)[0]*100;
			RElbowRoll = pmotion->getAngles("RElbowRoll",USE_SENSOR)[0]*100;
			out << int(RShoulderPitch) << ":"<< int(RShoulderRoll) << ":" <<int(RElbowYaw) << ":"<< int(RElbowRoll) << ":";
		}
		if (type=="All" || type=="LArm"){
			LShoulderPitch = pmotion->getAngles("LShoulderPitch",USE_SENSOR)[0]*100;
			LShoulderRoll = pmotion->getAngles("LShoulderRoll",USE_SENSOR)[0]*100;
			LElbowYaw = pmotion->getAngles("LElbowYaw",USE_SENSOR)[0]*100;
			LElbowRoll	= pmotion->getAngles("LElbowRoll",USE_SENSOR)[0]*100;
			out << int(LShoulderPitch) << ":"<< int(LShoulderRoll) << ":"<< int(LElbowYaw) << ":"<< int(LElbowRoll) << ":";
		}
		if (type=="All" || type=="RLeg"){
			RHipYawPitch = pmotion->getAngles("RHipYawPitch",USE_SENSOR)[0]*100;
			RHipRoll = pmotion->getAngles("RHipRoll",USE_SENSOR)[0]*100;
			RHipPitch = pmotion->getAngles("RHipPitch",USE_SENSOR)[0]*100;
			RKneePitch = pmotion->getAngles("RKneePitch",USE_SENSOR)[0]*100;
			RAnklePitch = pmotion->getAngles("RAnklePitch",USE_SENSOR)[0]*100;
			RAnkleRoll = pmotion->getAngles("RAnkleRoll",USE_SENSOR)[0]*100;
			out << int( RHipYawPitch) << ":"<< int(RHipRoll) << ":"<< int(RHipPitch) << ":"<< int(RKneePitch) << ":"<< int(RAnklePitch) << ":"<< int(RAnkleRoll) << ":";
		}
		if (type=="All" || type=="LLeg"){
			LHipYawPitch = pmotion->getAngles("LHipYawPitch",USE_SENSOR)[0]*100;
			LHipRoll = pmotion->getAngles("LHipRoll",USE_SENSOR)[0]*100;
			LHipPitch = pmotion->getAngles("LHipPitch",USE_SENSOR)[0]*100;
			LKneePitch = pmotion->getAngles("LKneePitch",USE_SENSOR)[0]*100;
			LAnklePitch = pmotion->getAngles("LAnklePitch",USE_SENSOR)[0]*100;
			LAnkleRoll = pmotion->getAngles("LAnkleRoll",USE_SENSOR)[0]*100;
			out << int(LHipYawPitch) << ":"<< int(LHipRoll) << ":"<< int(LHipPitch) << ":"<< int(LKneePitch) << ":"<< int(LAnklePitch) << ":"<< int(LAnkleRoll) << ":";
		}
		out << endl;


		return out.str();
	}
	catch (AL::ALError& e) {
		cerr << "Movie::getAngleRobot()" << e.toString() << endl;
	}
	return "";
}

void
Movie::setAngleRobot(const string& jointName, const Ice::Current& c)
{
	list<string> args = getTokens(jointName, ':');

	try {
		args.pop_front();

		string JoinName = args.front().c_str();
		args.pop_front();
		float angle = atof(args.front().c_str());
		angle = angle/100;

		AL::ALPtr<AL::ALMotionProxy> pmotion = getParentBroker()->getMotionProxy();
		pmotion->post.angleInterpolationWithSpeed(JoinName,angle, 1.0);
	}
	catch (AL::ALError& e) {
		cerr << "Coach::Coach [motion]" << e.toString() << endl;
	}
}

void
Movie::initTest(const Ice::Current& c)
{
	allAngles.clear();
	allTimes.clear();
}

void
Movie::jointName(const string& joints, const Ice::Current& c)
{
	list<string> args = getTokens(joints, ':');

	int total = args.size()-1;

	motors.clear();
	for (int i=0;i<total;i++){
		motors.arrayPush(args.front());
		args.pop_front();
	}
}

void
Movie::setAngles(const string& anglesValues, const Ice::Current& c)
{
	list<string> args = getTokens(anglesValues, ':');

	int nummov = atoi(args.front().c_str());
	args.pop_front();
	int total = (args.size()-1)/nummov;

	for (int i=0;i<total;i++){
		angles.clear();
		for (int j=0;j<nummov;j++){
			angles.arrayPush(atof(args.front().c_str()));
			args.pop_front();
		}
		allAngles.arrayPush(angles);
	}
}

void
Movie::setTimes(const string& timesValues, const Ice::Current& c)
{
	list<string> args = getTokens(timesValues, ':');

	int nummov = atoi(args.front().c_str());
	args.pop_front();
	int total = (args.size()-1)/nummov;

	for (int i=0;i<total;i++){
		times.clear();
		for (int j=0;j<nummov;j++){
			times.arrayPush(atof(args.front().c_str()));
			args.pop_front();
		}
		allTimes.arrayPush(times);
	}
}

void
Movie::run(const Ice::Current& c)
{
	AL::ALPtr<AL::ALMotionProxy> pmotion = getParentBroker()->getMotionProxy();
	//	pmotion->post.doMove(motors,allAngles,allTimes,1);
	pmotion->post.angleInterpolation(motors,allAngles,allTimes,true);
}

void
Movie::disableStiffness(const string& joints, const Ice::Current& c)
{
	list<string> args = getTokens(joints, ':');

	args.pop_front();
	string Order = args.front().c_str();
	args.pop_front();
	int total=args.size()-1;
	vector<string> pJointName;
	vector<float> pStiffness;
	for (int i=0;i<total;i++){
		pJointName.push_back(args.front());
		if (Order == "On") pStiffness.push_back(1.0);
		else pStiffness.push_back(0.0);
		args.pop_front();
	}
	printf("Enviando setJointStiffnesses...\n");
	AL::ALPtr<AL::ALMotionProxy> pmotion = getParentBroker()->getMotionProxy();
	pmotion->setStiffnesses(pJointName,pStiffness);
}

std::vector<int> 
Movie::getBreakpoints(const Ice::Current& c) {
	return getBreakpoints();
}

int
Movie::getLineNow(){
  //printf("getLineNow : %d", lineNow);
	return lineNow;
}
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
