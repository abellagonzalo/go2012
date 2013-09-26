package jmanager.components.lps;

public class LPO {
	
	private int type;
	private double dx;
	private double dy;
	private double x;
	private double y;
	private double quality;
	private double time;
	private String reliability;
	
	public LPO(){
	}
	
	public LPO(int type, double dx, double dy, double x, double y, double quality, String reliability, double time){
		this.setType(type);
		this.setDx(dx);
		this.setDy(dy);
		this.setX(x);
		this.setY(y);
		this.setQuality(quality);
		this.setTime(time);
		this.setReliability(reliability);
	}
	
	private String type2String(int type){
		switch(type){
			case 0 : return "Ball";
			case 1 : return "P0Net";
			case 2 : return "P0LeftPost";
			case 3 : return "P0RightPost";
			case 4 : return "P1Net";
			case 5 : return "P1LeftPost";
			case 6 : return "P1RightPost";
			case 7 : return "TeamMate1";
			case 8 : return "TeamMate2";
			case 9 : return "TeamMate3";
			case 10 : return "Opponent1";
			case 11 : return "Opponent2";
			case 12 : return "Opponent3";
			case 13 : return "Opponent4";
			default: return "";
		}
	}
	
	public void fill(bica.VisualMemoryObj obj){
		setX(obj.x);
		setY(obj.y);
		setDx(obj.dx);
		setDy(obj.dy);
		setQuality(obj.quality);
		setTime(obj.time);
		setReliability(obj.reliability);
	}
	
	public void setType(int type){
		this.type = type;
	}
	
	public void setDx(double dx){
		this.dx = dx;
	}
	
	public void setDy(double dy){
		this.dy = dy;
	}
	
	public void setX(double x){
		this.x = x;
	}
	
	public void setY(double y){
		this.y = y;
	}
	
	public void setQuality(double quality){
		this.quality = quality;
	}
	
	public void setTime(double time){
		this.time = time;
	}
	
	public void setReliability(String reliability){
		this.reliability = reliability;
	}

	public int getType(){
		return this.type;
	}

	public double getDx(){
		return this.dx;
	}
	
	public double getDy(){
		return this.dy;
	}
	
	public double getX(){
		return this.x;
	}
	
	public double getY(){
		return this.y;
	}
	
	public double getQuality(){
		return this.quality;
	}
	
	public double getTime(){
		return this.time;
	}
	
	public String getReliability(){
		return this.reliability;
	}
}
