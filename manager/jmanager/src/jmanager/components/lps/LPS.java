package jmanager.components.lps;

import java.util.Observable;

public class LPS extends Observable{
	
	public static final int NUM_OBJECTS = 7; 
	
	public static final int BALL = 0;
	public static final int P0NET = 1;
	public static final int P0LEFTPOST = 2;
	public static final int P0RIGHTPOST = 3;
	public static final int P1NET = 4;
	public static final int P1LEFTPOST = 5;
	public static final int P1RIGHTPOST = 6;
	public static final int TEAMMATE1 = 7;
	public static final int TEAMMATE2 = 8;
	public static final int TEAMMATE3 = 9;
	public static final int OPPONENT1 = 10;
	public static final int OPPONENT2 = 11;
	public static final int OPPONENT3 = 12;
	public static final int OPPONENT4 = 13;
	
	private LPO[] objects;
	
	public LPS(){
		this.objects = new LPO[NUM_OBJECTS];
		initLPS();
	}
	
	public void initLPS(){
		objects[0] = new LPO(BALL,      0.0, 0.0, 0.0, 0.0, 0.0, "Unknow", 0.0);
		objects[1] = new LPO(P0NET, 	0.0, 0.0, 0.0, 0.0, 0.0, "Unknow", 0.0);
		objects[2] = new LPO(P0LEFTPOST,	0.0, 0.0, 0.0, 0.0, 0.0, "Unknow", 0.0);
		objects[3] = new LPO(P0RIGHTPOST,0.0, 0.0, 0.0, 0.0, 0.0, "Unknow", 0.0);
		objects[4] = new LPO(P1NET, 0.0, 0.0, 0.0, 0.0, 0.0, "Unknow", 0.0);
		objects[5] = new LPO(P1LEFTPOST,	0.0, 0.0, 0.0, 0.0, 0.0, "Unknow", 0.0);
		objects[6] = new LPO(P1RIGHTPOST,0.0, 0.0, 0.0, 0.0, 0.0, "Unknow", 0.0);		
		/*objects[7] = new LPO(TEAMMATE1, 0.0, 0.0, 0.0, 0.0, 0.0, "Unknow", 0.0);
		objects[8] = new LPO(TEAMMATE2, 0.0, 0.0, 0.0, 0.0, 0.0, "Unknow", 0.0);
		objects[9] = new LPO(TEAMMATE3, 0.0, 0.0, 0.0, 0.0, 0.0, "Unknow", 0.0);
		objects[10] = new LPO(OPPONENT1, 0.0, 0.0, 0.0, 0.0, 0.0, "Unknow", 0.0);
		objects[11] = new LPO(OPPONENT2, 0.0, 0.0, 0.0, 0.0, 0.0, "Unknow", 0.0);
		objects[12] = new LPO(OPPONENT3, 0.0, 0.0, 0.0, 0.0, 0.0, "Unknow", 0.0);
		objects[13] = new LPO(OPPONENT4, 0.0, 0.0, 0.0, 0.0, 0.0, "Unknow", 0.0);*/
	}
	
	public void setLPO(LPO object){
		this.objects[object.getType()] = object;
		this.setChanged();
		this.notifyObservers();
	}
	
	public LPO getLPO(int index){
		return this.objects[index];
	}
}
