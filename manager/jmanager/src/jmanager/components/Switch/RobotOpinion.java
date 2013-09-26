package jmanager.components.Switch;

// Robot opinion about another teammate
public class RobotOpinion {
	
	private int 	observerRobot;
	private int 	observedRobot;
	private int 	x;
	private int 	y;
	private float 	theta;
	private String 	posReliability;
	private float   ballDistance;
	private int 	ballTimeSinceLastSeen;
	private String	ballReliability;
	private float	utilityGoalie;
	private float 	utilityKicker;
	private float	utilityDefender;
	private float	utilitySupporter;
	private short   role;
	
	public RobotOpinion(){
	}
	
	public RobotOpinion(int observerRobot, int observedRobot, int x, int y, float theta, String posReliability,
			float ballDistance, int ballTimeSinceLastSeen, String ballReliability, float utilityGoalie,
			float utilityKicker, float utilityDefender, float utilitySupporter, short role){
		this.observerRobot = observerRobot;
		this.observedRobot = observedRobot;
		this.x = x;
		this.y = y;
		this.theta = theta;
		this.posReliability = posReliability;
		this.ballDistance = ballDistance;
		this.ballTimeSinceLastSeen = ballTimeSinceLastSeen;
		this.ballReliability = ballReliability;
		this.utilityGoalie = utilityGoalie;
		this.utilityKicker = utilityKicker;
		this.utilityDefender = utilityDefender;
		this.utilitySupporter = utilitySupporter;
		this.role = role;
	}	
	
	public void fill(bica.SwitchOpinion obj){			
		//System.out.println("fill() " + obj.observerRobot);
		//System.out.println("fill() " + obj.observedRobot);
		this.observerRobot = obj.observerRobot;
		this.observedRobot = obj.observedRobot;
		this.x = obj.x;
		this.y = obj.y;
		this.theta = obj.theta;
		this.posReliability = obj.posReliability;
		this.ballDistance = obj.ballDistance;
		this.ballTimeSinceLastSeen = obj.ballTimeSinceLastSeen;
		this.ballReliability = obj.ballReliability;
		this.utilityGoalie = obj.utilityGoalie;
		this.utilityKicker = obj.utilityKicker;
		this.utilityDefender = obj.utilityDefender;
		this.utilitySupporter = obj.utilitySupporter;
                this.role = obj.role;
	}
	
	public int getObserverRobot() { return this.observerRobot;};
	public int getObservedRobot() { return this.observedRobot;};
	public int getX() { return this.x;};
	public int getY() { return this.y;};
	public float getTheta() { return this.theta;};
	public String getPosReliability() { return this.posReliability;};
	public float getBallDistance() { return this.ballDistance;};
	public int getBallTimeSinceLastSeen() { return this.ballTimeSinceLastSeen;};
	public String getBallReliability() { return this.ballReliability;};
	public float getUtilityGoalie() { return this.utilityGoalie;};
	public float getUtilityKicker() { return this.utilityKicker;};
	public float getUtilityDefender() { return this.utilityDefender;};
	public float getUtilitySupporter() { return this.utilitySupporter;};
        public short getRole() { return this.role;};
}
