package jmanager.components.Switch;

import java.util.Observable;

public class SharedTeamInfo extends Observable{

	public static final int NUM_ROBOTS = 4;

	private RobotOpinion[][] opinions;

	public SharedTeamInfo(){
		opinions = new RobotOpinion[NUM_ROBOTS][NUM_ROBOTS];
		init();
	}

	public void init(){
		for (int i = 1; i <= NUM_ROBOTS; i++) {
			for (int j = 1; j <= NUM_ROBOTS; j++) {
				opinions[i-1][j-1] = new RobotOpinion(i, j, 0, 0, 0.0f, "Unknow",
						0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, (short)99);
			}
		}		
	}

	public void setRobotOpinionsList(bica.SwitchOpinion[] opinionsList){
		for (int i = 0; i < opinionsList.length; i++){
			//System.out.println("Registrando opinion " + i);
			RobotOpinion opinion = new RobotOpinion();
			opinion.fill(opinionsList[i]);
			setRobotOpinion(opinion);
		}
		this.setChanged();
		this.notifyObservers();
	}

	public void setRobotOpinion(RobotOpinion object){
		this.opinions[object.getObserverRobot() - 1][object.getObservedRobot() - 1] = object;
		//this.setChanged();
		//this.notifyObservers();
	}

	public RobotOpinion getRobotOpinion(int observerRobot, int observedRobot){
		return this.opinions[observerRobot - 1][observedRobot - 1];
	}
}
