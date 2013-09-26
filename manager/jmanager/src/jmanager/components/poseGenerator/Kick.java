package jmanager.components.poseGenerator;

import java.util.*;
import javax.swing.table.*;

public class Kick extends AbstractTableModel implements Cloneable{
	
	
	private static final int VEL_COLUMN = 22;
	private Vector<String> columnNames = null;
	private Vector initPositions;
	private Vector data = null; 
	private String kickName;
	private int kickID;
	private int kickXmin;
	private int kickYmin;
	private int kickXMax;
	private int kickYMax;
	private int kickDx;
	private int kickDy;
	private int kickDth;
	
    public Kick() {
		super();
		
		kickName = new String("DEFAULT_NAME");
		kickID = -1;
		kickXmin = -1;
		kickYmin = -1;
		kickXMax = -1;
		kickYMax = -1;
		kickDx = -1;
		kickDy = -1;
		kickDth = -1;
		
		columnNames = new Vector<String>();
		
		columnNames.addElement("HeadYaw");//0
		columnNames.addElement("HeadPitch");//1
		
		columnNames.addElement("LShoulderPitch");//2
		columnNames.addElement("LShoulderRoll");//3
		columnNames.addElement("LElbowYaw");//4
		columnNames.addElement("LElbowRoll");//5
		
		columnNames.addElement("LHipYawPitch");//6
		columnNames.addElement("LHipRoll");//7
		columnNames.addElement("LHipPitch");//8
		columnNames.addElement("LKneePitch");//9
		columnNames.addElement("LAnklePitch");//10
		columnNames.addElement("LAnkleRoll");//11
		
		columnNames.addElement("RHipYawPitch");//12
		columnNames.addElement("RHipRoll");//13
		columnNames.addElement("RHipPitch");//14
		columnNames.addElement("RKneePitch");//15
		columnNames.addElement("RAnklePitch");//16
		columnNames.addElement("RAnkleRoll");//17
		
		columnNames.addElement("RShoulderPitch");//18
		columnNames.addElement("RShoulderRoll");//19
		columnNames.addElement("RElbowYaw");//20
		columnNames.addElement("RElbowRoll");//21
		
		columnNames.addElement("Velocity");
		
		createInitPositions();
		data = new Vector();
	}//End Kick()
	
	public void createInitPositions() {
		initPositions = new Vector<Float>();
		for(int i=0;i<getColumnCount() - 1;i++)
			initPositions.addElement(new Float(0.0f));
		initPositions.addElement(new Float(10.0f)); //Default velocity (1seg)
	}//End createInitPositions() 
	
	public void setName(String name) {
		kickName = new String(name);
	}//End setName()
	
	public String getName() {
		return kickName;
	}//End getName()
	
	public void setID(int id) {
		kickID = id;
	}//End setID()
	
	public int getID() {
		return kickID;
	}//End getID()
	
	
	//NEW four arguments
	public void setXmin(int Xmin) {
		kickXmin = Xmin;
	}//End setXmin()	
	public int getXmin() {
		return kickXmin;
	}//End getXmin()
	
	public void setYmin(int Ymin) {
		kickYmin = Ymin;
	}//End setYmin()
	public int getYmin() {
		return kickYmin;
	}//End getYmin()
	
	public void setXMax(int XMax) {
		kickXMax = XMax;
	}//End setXMax()
	public int getXMax() {
		return kickXMax;
	}//End getXMax()
	
	public void setYMax(int YMax) {
		kickYMax = YMax;
	}//End setYMax()
	public int getYMax() {
		return kickYMax;
	}//End getYMax()
	//-------------------------------
	
	//Functions for x, y and theta
	public void setDx(int Dx) {
		kickDx = Dx;
	}//End setDx()
	public int getDx() {
		return kickDx;
	}//End getDx()
	
	public void setDy(int Dy) {
		kickDy = Dy;
	}//End setDy()
	public int getDy() {
		return kickDy;
	}//End getDy()
	
	public void setDth(int Dth) {
		kickDth = Dth;
	}//End setDth()
	public int getDth() {
		return kickDth;
	}//End getDth()
	//-------------------------------
	
	public int getColumnCount() {
		return columnNames.size();
	}
	
	public int getRowCount() {
		return (data.size());
	}
	
	public String getColumnName(int col) {
		return columnNames.get(col).toString();
	}
	
    public Object getValueAt(int row, int col) {
		Vector temp = (Vector) data.get(row);
		return temp.get(col);
    }
	
	/*
     * JTable uses this method to determine the default renderer/
     * editor for each cell.  If we didn't implement this method,
     * then the last column would contain text ("true"/"false"),
     * rather than a check box.
     */
	public Class getColumnClass(int col) {
		return getValueAt(0, col).getClass();
       }
	 
	 
	/*
     * Don't need to implement this method unless your table's
     * data can change.
     */
    public void setValueAt(Object value, int row, int col) {
		Vector temp = (Vector)data.get(row);
		temp.set(col,value);
        fireTableCellUpdated(row, col);
	}
	
	//FB methods
	public void newRow(){
		Vector temp = new Vector();
		for(int i=0;i<getColumnCount()-1;i++)
			temp.addElement(new Float(0.0f));
		temp.addElement(new Float(10.0f)); //Default velocity
		data.addElement(temp);
		fireTableRowsInserted(getRowCount(),getRowCount());
	}//End newRow()
	
	public void newRow(int row){
		
		//Insert row
		Vector temp = new Vector();
		for(int i=0;i<getColumnCount()-1;i++)
			temp.addElement(new Float(0.0f));
		temp.addElement(new Float(10.0f)); //Default velocity
		data.add(row,temp);
		//fireTableRowsInserted(row,row);
		fireTableRowsUpdated(row,getRowCount());
	}//End newRow()
	
	public void newRow(Vector rowData) {
		data.addElement(rowData);
	}//End modifyRow()
	
	public void removeRow(int row) {
		if (row != -1) {
			data.removeElementAt(row);
			//fireTableRowsDeleted(row,row);
			fireTableRowsUpdated(row,getRowCount());
		}
	}
	
	
	public String getRow(int row) {
		//all columns but index columns are included 
		String str = new String();
		for(int col=0;col<getColumnCount()-1;col++) {
			Float temp = new Float((Float)getValueAt(row,col));
			str = str.concat(temp.toString() + "  ");
		}
	
		Float temp_b = new Float((Float)getValueAt(row,getColumnCount()-1));
		str = str.concat(temp_b.toString());

		return str;
	}//End getRow()
	
	public String getMainFields(){
		return (kickName+"  " +kickID+" "+data.size());
	}//End getMainFields
	
	public String toString() {
		String str = new String(kickName+"  " +kickID+" "+data.size());
		for(int row=0;row<getRowCount();row++) 
			str = str.concat("\n"+getRow(row));
		return str;
	}//End toString()
	
	public Object clone() {
		Kick newKick = new Kick();
		newKick.setName(new String(kickName));
		newKick.setID(kickID);
		//New 
		newKick.setXmin(kickXmin);
		newKick.setXMax(kickXMax);
		newKick.setYmin(kickYmin);		
		newKick.setYMax(kickYMax);
		newKick.setDx(kickDx);
		newKick.setDy(kickDy);
		newKick.setDth(kickDth);
		
		
		//newKick.data = (Vector)data.clone();
		newKick.data = new Vector();
		for(int i=0;i<data.size();i++) {
			Vector v = new Vector();
			Vector source = (Vector)data.get(i);
			for(int j=0;j<source.size();j++)
				v.addElement(new Float((Float)source.get(j)));
			newKick.newRow(v);
		}
		
		return newKick;
	}//End clone()

	public Kick cloneSimetry() {
		Kick newKick = new Kick();
		newKick.setName(new String(kickName));
		newKick.setID(kickID);
		//New 
		newKick.setXmin(kickXmin);
		newKick.setXMax(kickXMax);
		newKick.setYmin(kickYmin);		
		newKick.setYMax(kickYMax);
		newKick.setDx(kickDx);
		newKick.setDy(kickDy);
		newKick.setDth(kickDth);
		
		
		//newKick.data = (Vector)data.clone();
		newKick.data = new Vector();
		for(int i=0;i<data.size();i++) {
			Vector<Float> v = new Vector<Float>();
			Vector source = (Vector)data.get(i);

				
/*			for(int j = 0; j<22;j++)
				v.addElement(new Float((Float)source.get(j)));
*/
			v.addElement(new Float(-(Float)source.get(0)));
			v.addElement(new Float((Float)source.get(1)));
			v.addElement(new Float((Float)source.get(18)));
			v.addElement(new Float(-(Float)source.get(19)));
			v.addElement(new Float(-(Float)source.get(20)));
			v.addElement(new Float(-(Float)source.get(21)));
			v.addElement(new Float((Float)source.get(12)));
			v.addElement(new Float(-(Float)source.get(13)));
			v.addElement(new Float((Float)source.get(14)));
			v.addElement(new Float((Float)source.get(15)));
			v.addElement(new Float((Float)source.get(16)));
			v.addElement(new Float(-(Float)source.get(17)));
			v.addElement(new Float((Float)source.get(6)));
			v.addElement(new Float(-(Float)source.get(7)));
			v.addElement(new Float((Float)source.get(8)));
			v.addElement(new Float((Float)source.get(9)));
			v.addElement(new Float((Float)source.get(10)));
			v.addElement(new Float(-(Float)source.get(11)));
			v.addElement(new Float((Float)source.get(2)));
			v.addElement(new Float(-(Float)source.get(3)));
			v.addElement(new Float(-(Float)source.get(4)));
			v.addElement(new Float(-(Float)source.get(5)));

			//Speed
			v.addElement(new Float((Float)source.get(22)));

//			for(int j=0;j<source.size();j++)
//				v.addElement(new Float((Float)source.get(j)));
			
			newKick.newRow(v);
		}
		
		return newKick;
		}
	
}
