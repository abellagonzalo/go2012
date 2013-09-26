package jmanager.components.poseGenerator;

import java.io.*;
import java.util.*;

public class Parser {
	Properties properties;
	int count;
		
	public Parser(File file) throws IOException{
		properties = new Properties();
		properties.load(new FileInputStream(file));
		//kickProperties.list(System.out);
		String aux;
		if ((aux = properties.getProperty("KICK_COUNT")) != null)
			count = Integer.parseInt(aux);
		else
			count = 0;
	}//End KickMasterParser()
	
	/**
	 * Gets the total number of kicks
	 */
	 public int getCount() {
	 	return count;
	 }//End getKicksCount()
	
	/**
	 *  Gets a specified kick from a file
	 */ 
	public Kick getKick(String key) {
		String s;
		
		//No kick withs key value
		if ((s = (properties.getProperty(key))) == null)
			return null;
		
		//Retrieve the kick
		Kick kick = new Kick();
		StringTokenizer st = new StringTokenizer(s);
		kick.setName(st.nextToken());
		kick.setID(Integer.parseInt(st.nextToken()));
		//New: four calls
		kick.setXmin(Integer.parseInt(st.nextToken()));
		kick.setXMax(Integer.parseInt(st.nextToken()));
		kick.setYmin(Integer.parseInt(st.nextToken()));
		kick.setYMax(Integer.parseInt(st.nextToken()));
		
		//Values of x, y and theta
		kick.setDx(Integer.parseInt(st.nextToken()));
		kick.setDy(Integer.parseInt(st.nextToken()));
		kick.setDth(Integer.parseInt(st.nextToken()));
		
		int steps = Integer.parseInt(st.nextToken());
		for(int i=0;i<steps;i++) {
			Vector v = new Vector();
			s = (properties.getProperty(key+"_"+i));
			st = new StringTokenizer(s);
			
			//System.out.println("Desde fichero:");
			for(int j=0;j<kick.getColumnCount();j++)
			{
				Float f=new Float(st.nextToken());
				//System.out.print(" " + f);
				v.addElement(f);
			}
			//System.out.println();
			
			kick.newRow(v);
		}
		return kick;
	}//End getKick()

}//End class KickMasterParser
