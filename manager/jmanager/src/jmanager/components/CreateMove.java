/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jmanager.components;
import java.awt.TextField;
import java.awt.event.WindowEvent;
import jmanager.*;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.jdesktop.application.Action;

import java.io.File;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileFilter;
import java.io.FileNotFoundException;
import java.io.FileReader;

import java.io.FileWriter;
import java.io.IOException;
import java.util.Arrays;
import javax.swing.JOptionPane;
import javax.swing.JFileChooser;

import java.util.Hashtable;
import javax.swing.JDialog;

/**
 *
 * @author rbenitez
 */
public class CreateMove extends javax.swing.JPanel implements java.beans.Customizer {

	private Object bean;
	private String JoinNames="HeadYaw;HeadPitch;RShoulderPitch;RShoulderRoll;RElbowYaw;RElbowRoll;LShoulderPitch;LShoulderRoll;LElbowYaw;LElbowRoll;RHipYawPitch;RHipRoll;RHipPitch;RKneePitch;RAnklePitch;RAnkleRoll;LHipYawPitch;LHipRoll;LHipPitch;LKneePitch;LAnklePitch;LAnkleRoll";
	private String Head="HeadYaw;HeadPitch";
	private String RArm="RShoulderPitch;RShoulderRoll;RElbowYaw;RElbowRoll";
	private String LArm="LShoulderPitch;LShoulderRoll;LElbowYaw;LElbowRoll";
	private String RLeg="RHipYawPitch;RHipRoll;RHipPitch;RKneePitch;RAnklePitch;RAnkleRoll";
	private String LLeg="LHipYawPitch;LHipRoll;LHipPitch;LKneePitch;LAnklePitch;LAnkleRoll";	

	File lastdir = null;
	static class JoinMov{
		String JoinName;
		String[] movs = new String[22];
		String[] times= new String[22];
	}
	static class Movements{
		String Name;
		int NumMove;
		int NumJoin;
		//  List<JoinMov> JoinList ;
		JoinMov[] JoinList = new JoinMov[1000];
	}

	Movements mimov;
	Movements miNewmov;
	Hashtable HTJoinName;

	static boolean CreateNewMove = false;
	static boolean Modify = false;
	static int NMove=-1;
	static int NewNMove =0;
	static int NPosition=0;
	static int NewNumJoinName= -1;
	static boolean isInsert = false;



	/** Creates new customizer CreateMove */
	public CreateMove() {
		initComponents();
		SetEnabled(JoinNames.split(";"),false);
		btnAnt.setEnabled(false);
		btnSig.setEnabled(false);
	}

	public void setObject(Object bean) {
		this.bean = bean;
	}





	private void SendMovementRobot(Movements mimov, int IniNumMovement,int FinNumMovement){
		/*FinNumMovement siempre será el siguiente o el total de movimientos
         Si queremos reproducir el movimiento 0 la llamada la funcion será SendMovementRobot(0,1)
         Si queremos reproducir todos los movimientos será SendMovementRobot(0,mimov.NumMove)
		 */
		/*Numero de movimientos que quiero hacer.*/
		int NumMov=FinNumMovement - IniNumMovement;


		/*Ver forma de neviar numero de movimientos:MOVIMIENTOS:de todos los joins*/
		System.out.println("Movimiento desde "+ IniNumMovement + " hasta "+ FinNumMovement + " NumJoin" + mimov.NumJoin                 );

		//dataResp resp = JmanagerView.root.getConnection().send("testinRobot:InitTest:");
		JmanagerView.root.getConnection().moviePrx.initTest();
		
		String JoinNameValues="";
		for (int i=0;i<mimov.NumJoin+1;i++)
			JoinNameValues = JoinNameValues + ":" + mimov.JoinList[i].JoinName;
		System.out.println("testinRobot:JoinName"+JoinNameValues+":");
		//resp = JmanagerView.root.getConnection().send("testinRobot:JoinName"+JoinNameValues+":");
		JmanagerView.root.getConnection().moviePrx.jointName(JoinNameValues);

		String AnglesValues="";
		String TimesValues="";

		/*Revisar que como mandamos el Angulo */
		for (int i=0;i<mimov.NumJoin+1;i++)
			for (int k=IniNumMovement;k<FinNumMovement;k++)
				AnglesValues = AnglesValues + ":" + mimov.JoinList[i].movs[k];
		System.out.println("testinRobot:Angles:"+ NumMov +AnglesValues+":");
		//resp = JmanagerView.root.getConnection().send("testinRobot:Angles:"+ NumMov +AnglesValues+":");
		JmanagerView.root.getConnection().moviePrx.setAngles(NumMov + AnglesValues);


		for (int i=0;i<mimov.NumJoin+1;i++)
			for (int k=IniNumMovement;k<FinNumMovement;k++)
				TimesValues = TimesValues + ":" + mimov.JoinList[i].times[k];
		System.out.println("testinRobot:Times:" + NumMov +TimesValues+":");
		//resp = JmanagerView.root.getConnection().send("testinRobot:Times:" + NumMov +TimesValues+":");
		JmanagerView.root.getConnection().moviePrx.setTimes(NumMov +TimesValues);

		JmanagerView.root.getConnection().moviePrx.run();
		//resp = JmanagerView.root.getConnection().send("testinRobot:Run:");
		//try {
		//	Thread.sleep(500);
		//} catch (InterruptedException ex) {
		//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
		//}


	}

	private boolean isJoinSave(){
		boolean isSave=false;

		for (int i=0;i<miNewmov.NumJoin+1;i++){
			isSave=true;
			try{

				System.out.println("comprobando "+ miNewmov.JoinList[i].JoinName + " ->" +i + " NewNMove " + NewNMove);
				if (miNewmov.JoinList[i].movs[NewNMove-1] == null){
					isSave = false;
					break;
				}
			}

			catch(Exception e){

				System.out.println(e.toString());
				System.out.println("exception en i:" + i + " NumNMove:" + NewNMove );
				isSave = false;
				break;
			}   
		}
		return isSave;
	}

	private void SetSlidersbyMov(String JoinName, float Mov, float Time){


		//     System.out.println( levels[i]);

		if (JoinName.equals("HeadYaw")){
			HeadYaw.setValue((int)(Mov*100));
			txtTimeHeadYaw.setText(String.valueOf(Time));
		}else if (JoinName.equals("HeadPitch")){
			HeadPitch.setValue((int)(Mov*100));
			txtTimeHeadPitch.setText(String.valueOf(Time));
		}else if (JoinName.equals("RShoulderPitch")){
			RShoulderPitch.setValue((int)(Mov*100));
			txtTimeRShoulderPitch.setText(String.valueOf(Time));
		}else if (JoinName.equals("RShoulderRoll")){
			RShoulderRoll.setValue((int)(Mov*100));
			txtTimeRShoulderRoll.setText(String.valueOf(Time));
		}else if (JoinName.equals("RElbowYaw")){
			RElbowYaw.setValue((int)(Mov*100));
			txtTimeRElbowYaw.setText(String.valueOf(Time));
		}else if (JoinName.equals("RElbowRoll")){
			RElbowRoll.setValue((int)(Mov*100));
			txtTimeRElbowRoll.setText(String.valueOf(Time));
		}else if (JoinName.equals("LShoulderPitch")){
			LShoulderPitch.setValue((int)(Mov*100));
			txtTimeLShoulderPitch.setText(String.valueOf(Time));
		}else if (JoinName.equals("LShoulderRoll")){
			LShoulderRoll.setValue((int)(Mov*100));
			txtTimeLShoulderRoll.setText(String.valueOf(Time));
		}else if (JoinName.equals("LElbowYaw")){
			LElbowYaw.setValue((int)(Mov*100));
			txtTimeLElbowYaw.setText(String.valueOf(Time));
		}else if (JoinName.equals("LElbowRoll")){
			LElbowRoll.setValue((int)(Mov*100));
			txtTimeLElbowRoll.setText(String.valueOf(Time));
		}else if (JoinName.equals("RHipYawPitch")){
			RHipYawPitch.setValue((int)(Mov*100));
			txtTimeRHipYawPitch.setText(String.valueOf(Time));
		}else if (JoinName.equals("RHipRoll")){
			RHipRoll.setValue((int)(Mov*100));
			txtTimeRHipRoll.setText(String.valueOf(Time));
		}else if (JoinName.equals("RHipPitch")){
			RHipPitch.setValue((int)(Mov*100));
			txtTimeRHipPitch.setText(String.valueOf(Time));
		}else if (JoinName.equals("RKneePitch")){
			RKneePitch.setValue((int)(Mov*100));
			txtTimeRKneePitch.setText(String.valueOf(Time));
		}else if (JoinName.equals("RAnklePitch")){
			RAnklePitch.setValue((int)(Mov*100));
			txtTimeRAnklePitch.setText(String.valueOf(Time));
		}else if (JoinName.equals("RAnkleRoll")){
			RAnkleRoll.setValue((int)(Mov*100));
			txtTimeRAnkleRoll.setText(String.valueOf(Time));
		}else if (JoinName.equals("LHipYawPitch")){
			LHipYawPitch.setValue((int)(Mov*100));
			txtTimeLHipYawPitch.setText(String.valueOf(Time));
		}else if (JoinName.equals("LHipRoll")){
			LHipRoll.setValue((int)(Mov*100));
			txtTimeLHipRoll.setText(String.valueOf(Time));
		}else if (JoinName.equals("LHipPitch")){
			LHipPitch.setValue((int)(Mov*100));
			txtTimeLHipPitch.setText(String.valueOf(Time));
		}else if (JoinName.equals("LKneePitch")){
			LKneePitch.setValue((int)(Mov*100));
			txtTimeLKneePitch.setText(String.valueOf(Time));
		}else if (JoinName.equals("LAnklePitch")){
			LAnklePitch.setValue((int)(Mov*100));
			txtTimeLAnklePitch.setText(String.valueOf(Time));
		}else if (JoinName.equals("LAnkleRoll")){
			LAnkleRoll.setValue((int)(Mov*100));
			txtTimeLAnkleRoll.setText(String.valueOf(Time));
		}



	}
	private void setRobotSliders(String resp) {
		//String resp = new String(respdata.data);

		String[] levels = resp.split(":");
		System.out.println("Levels: " + levels.length);
		for (int i=0;i<levels.length;i++)
			System.out.println("Level:" + levels[i]);

		/*Head*/
		HeadYaw.setValue(Integer.parseInt(levels[0]));
		HeadPitch.setValue(Integer.parseInt(levels[1]));
		/*RArm*/
		RShoulderPitch.setValue(Integer.parseInt(levels[2]));
		RShoulderRoll.setValue(Integer.parseInt(levels[3]));
		RElbowYaw.setValue(Integer.parseInt(levels[4]));
		RElbowRoll.setValue(Integer.parseInt(levels[5]));

		/*LArm*/
		LShoulderPitch.setValue(Integer.parseInt(levels[6]));
		LShoulderRoll.setValue(Integer.parseInt(levels[7]));
		LElbowYaw.setValue(Integer.parseInt(levels[8]));
		LElbowRoll.setValue(Integer.parseInt(levels[9]));

		/*RLeg*/
		RHipYawPitch.setValue(Integer.parseInt(levels[10]));
		RHipRoll.setValue(Integer.parseInt(levels[11]));
		RHipPitch.setValue(Integer.parseInt(levels[12]));
		RKneePitch.setValue(Integer.parseInt(levels[13]));
		RAnklePitch.setValue(Integer.parseInt(levels[14]));
		RAnkleRoll.setValue(Integer.parseInt(levels[15]));

		/*LLeg*/
		LHipYawPitch.setValue(Integer.parseInt(levels[16]));
		LHipRoll.setValue(Integer.parseInt(levels[17]));
		LHipPitch.setValue(Integer.parseInt(levels[18]));
		LKneePitch.setValue(Integer.parseInt(levels[19]));
		LAnklePitch.setValue(Integer.parseInt(levels[20]));
		LAnkleRoll.setValue(Integer.parseInt(levels[21]));



	}

	private void setRobotSlidersbyJoin(String resp,String Type) {
		//String resp = new String(respdata.data);

		String[] levels = resp.split(":");
		System.out.println("Levels: " + levels.length);
		for (int i=0;i<levels.length;i++)
			System.out.println("Level:" + levels[i]);

		/*Head*/
		if (Type=="Head"){
			HeadYaw.setValue(Integer.parseInt(levels[0]));
			HeadPitch.setValue(Integer.parseInt(levels[1]));
		}
		/*RArm*/
		if (Type=="RArm"){
			RShoulderPitch.setValue(Integer.parseInt(levels[0]));
			RShoulderRoll.setValue(Integer.parseInt(levels[1]));
			RElbowYaw.setValue(Integer.parseInt(levels[2]));
			RElbowRoll.setValue(Integer.parseInt(levels[3]));
		}
		/*LArm*/
		if (Type=="LArm"){
			LShoulderPitch.setValue(Integer.parseInt(levels[0]));
			LShoulderRoll.setValue(Integer.parseInt(levels[1]));
			LElbowYaw.setValue(Integer.parseInt(levels[2]));
			LElbowRoll.setValue(Integer.parseInt(levels[3]));
		}


		/*RLeg*/
		if (Type=="RLeg"){
			RHipYawPitch.setValue(Integer.parseInt(levels[0]));
			RHipRoll.setValue(Integer.parseInt(levels[1]));
			RHipPitch.setValue(Integer.parseInt(levels[2]));
			RKneePitch.setValue(Integer.parseInt(levels[3]));
			RAnklePitch.setValue(Integer.parseInt(levels[4]));
			RAnkleRoll.setValue(Integer.parseInt(levels[5]));
		}
		/*LLeg*/
		if (Type=="LLeg"){
			LHipYawPitch.setValue(Integer.parseInt(levels[0]));
			LHipRoll.setValue(Integer.parseInt(levels[1]));
			LHipPitch.setValue(Integer.parseInt(levels[2]));
			LKneePitch.setValue(Integer.parseInt(levels[3]));
			LAnklePitch.setValue(Integer.parseInt(levels[4]));
			LAnkleRoll.setValue(Integer.parseInt(levels[5]));
		}


	}


	private void SetEnabled(String[] JoinName,boolean type){

		for (int i=0;i<JoinName.length;i++){
			//     System.out.println( JoinName[i]);

			if (JoinName[i].equals("HeadYaw")){
				HeadYaw.setEnabled(type);
				doHead.setSelected(type);
			}else if (JoinName[i].equals("HeadPitch")){
				HeadPitch.setEnabled(type);
				doHead.setSelected(type);
			}else if (JoinName[i].equals("RShoulderPitch")){
				RShoulderPitch.setEnabled(type);
				doRArm.setSelected(type);
			}else if (JoinName[i].equals("RShoulderRoll")){
				RShoulderRoll.setEnabled(type);
				doRArm.setSelected(type);
			}else if (JoinName[i].equals("RElbowYaw")){
				RElbowYaw.setEnabled(type);
				doRArm.setSelected(type);
			}else if (JoinName[i].equals("RElbowRoll")){
				RElbowRoll.setEnabled(type);
				doRArm.setSelected(type);
			}else if (JoinName[i].equals("LShoulderPitch")){
				LShoulderPitch.setEnabled(type);
				doLArm.setSelected(type);
			}else if (JoinName[i].equals("LShoulderRoll")){
				LShoulderRoll.setEnabled(type);
				doLArm.setSelected(type);
			}else if (JoinName[i].equals("LElbowYaw")){
				LElbowYaw.setEnabled(type);
				doLArm.setSelected(type);
			}else if (JoinName[i].equals("LElbowRoll")){
				LElbowRoll.setEnabled(type);
				doLArm.setSelected(type);
			}else if (JoinName[i].equals("RHipYawPitch")){
				RHipYawPitch.setEnabled(type);
				doRLeg.setSelected(type);
			}else if (JoinName[i].equals("RHipRoll")){
				RHipRoll.setEnabled(type);
				doRLeg.setSelected(type);
			}else if (JoinName[i].equals("RHipPitch")){
				RHipPitch.setEnabled(type);
				doRLeg.setSelected(type);
			}else if (JoinName[i].equals("RKneePitch")){
				RKneePitch.setEnabled(type);
				doRLeg.setSelected(type);
			}else if (JoinName[i].equals("RAnklePitch")){
				RAnklePitch.setEnabled(type);
				doRLeg.setSelected(type);
			}else if (JoinName[i].equals("RAnkleRoll")){
				RAnkleRoll.setEnabled(type);
				doRLeg.setSelected(type);
			}else if (JoinName[i].equals("LHipYawPitch")){
				LHipYawPitch.setEnabled(type);
				doLLeg.setSelected(type);
			}else if (JoinName[i].equals("LHipRoll")){
				LHipRoll.setEnabled(type);
				doLLeg.setSelected(type);
			}else if (JoinName[i].equals("LHipPitch")){
				LHipPitch.setEnabled(type);
				doLLeg.setSelected(type);
			}else if (JoinName[i].equals("LKneePitch")){
				LKneePitch.setEnabled(type);
				doLLeg.setSelected(type);
			}else if (JoinName[i].equals("LAnklePitch")){
				LAnklePitch.setEnabled(type);
				doLLeg.setSelected(type);
			}else if (JoinName[i].equals("LAnkleRoll")){
				LAnkleRoll.setEnabled(type);
				doLLeg.setSelected(type);
			}


		}
	}

	private String getAngle(String JoinName){
		int Angle=0;


		//     System.out.println( levels[i]);

		if (JoinName.equals("HeadYaw")){
			Angle = HeadYaw.getValue();
		}else if (JoinName.equals("HeadPitch")){
			Angle = HeadPitch.getValue();
		}else if (JoinName.equals("RShoulderPitch")){
			Angle = RShoulderPitch.getValue();
		}else if (JoinName.equals("RShoulderRoll")){
			Angle = RShoulderRoll.getValue();
		}else if (JoinName.equals("RElbowYaw")){
			Angle = RElbowYaw.getValue();
		}else if (JoinName.equals("RElbowRoll")){
			Angle = RElbowRoll.getValue();
		}else if (JoinName.equals("LShoulderPitch")){
			Angle = LShoulderPitch.getValue();
		}else if (JoinName.equals("LShoulderRoll")){
			Angle = LShoulderRoll.getValue();
		}else if (JoinName.equals("LElbowYaw")){
			Angle = LElbowYaw.getValue();
		}else if (JoinName.equals("LElbowRoll")){
			Angle = LElbowRoll.getValue();
		}else if (JoinName.equals("RHipYawPitch")){
			Angle = RHipYawPitch.getValue();
		}else if (JoinName.equals("RHipRoll")){
			Angle = RHipRoll.getValue();
		}else if (JoinName.equals("RHipPitch")){
			Angle = RHipPitch.getValue();
		}else if (JoinName.equals("RKneePitch")){
			Angle = RKneePitch.getValue();
		}else if (JoinName.equals("RAnklePitch")){
			Angle = RAnklePitch.getValue();
		}else if (JoinName.equals("RAnkleRoll")){
			Angle =     RAnkleRoll.getValue();
		}else if (JoinName.equals("LHipYawPitch")){
			Angle =     LHipYawPitch.getValue();
		}else if (JoinName.equals("LHipRoll")){
			Angle = LHipRoll.getValue();
		}else if (JoinName.equals("LHipPitch")){
			Angle = LHipPitch.getValue();
		}else if (JoinName.equals("LKneePitch")){
			Angle = LKneePitch.getValue();
		}else if (JoinName.equals("LAnklePitch")){
			Angle = LAnklePitch.getValue();
		}else if (JoinName.equals("LAnkleRoll")){
			Angle = LAnkleRoll.getValue();
		}

		return String.valueOf(Angle);

	}

	private void SetPositionStable(String JoinName){
		//, "0", String.valueOf(Float.valueOf(txtTimeHeadYaw.getText())+2));
		Float Time = Float.valueOf(0);
		String Movs = "";
		/*Guardar una posicion estable.
		 *HeadYaw = 0     *  HeadPitch = 0
		 *LShoulderPitch = 209     *  LShoulderRoll = 34     *  LElbowYaw = -139     *  LElbowRoll = -139
		 *LHipYawPitch = 0     * LHipRoll = 0     * LHipPitch = -34     * LKneePitch = 69     * LAnklePitch = -34     * LAnkleRoll = 52
		 *RShoulderPitch = 209     * RShoulderRoll = -34     * RElbowYaw = 139     * RElbowRoll = 139
		 *RHipYawPitch = 0     * RHipRoll = 0     * RHipPitch = -34     * RKneePitch = 69     * RAnklePitch =-34     * RAnkleRoll = 0
		 * */
		if (JoinName.equals("HeadYaw")){

			if (txtTimeHeadYaw.getText().isEmpty()) Time=Float.valueOf(1);
			else Time = Float.valueOf(txtTimeHeadYaw.getText())+2;
			Movs="0";
		}else if (JoinName.equals("HeadPitch")){
			if (txtTimeHeadPitch.getText().isEmpty()) Time=Float.valueOf(1);
			else  Time = Float.valueOf(txtTimeHeadPitch.getText())+2;
			Movs="0";
		}else if (JoinName.equals("RShoulderPitch")){
			if (txtTimeRShoulderPitch.getText().isEmpty()) Time=Float.valueOf(1);
			else  Time = Float.valueOf(txtTimeRShoulderPitch.getText())+2;
			Movs="209";
		}else if (JoinName.equals("RShoulderRoll")){
			if (txtTimeRShoulderRoll.getText().isEmpty()) Time=Float.valueOf(1);
			else  Time = Float.valueOf(txtTimeRShoulderRoll.getText())+2;
			Movs="-34";
		}else if (JoinName.equals("RElbowYaw")){
			if (txtTimeRElbowYaw.getText().isEmpty()) Time=Float.valueOf(1);
			else  Time = Float.valueOf(txtTimeRElbowYaw.getText())+2;
			Movs="139";
		}else if (JoinName.equals("RElbowRoll")){
			if (txtTimeRElbowRoll.getText().isEmpty()) Time=Float.valueOf(1);
			else  Time = Float.valueOf(txtTimeRElbowRoll.getText())+2;
			Movs="139";
		}else if (JoinName.equals("LShoulderPitch")){
			if (txtTimeLShoulderPitch.getText().isEmpty()) Time=Float.valueOf(1);
			else  Time = Float.valueOf(txtTimeLShoulderPitch.getText())+2;
			Movs="209";
		}else if (JoinName.equals("LShoulderRoll")){
			if (txtTimeLShoulderRoll.getText().isEmpty()) Time=Float.valueOf(1);
			else  Time = Float.valueOf(txtTimeLShoulderRoll.getText())+2;
			Movs="34";
		}else if (JoinName.equals("LElbowYaw")){
			if (txtTimeLElbowYaw.getText().isEmpty()) Time=Float.valueOf(1);
			else   Time = Float.valueOf(txtTimeLElbowYaw.getText())+2;
			Movs="-139";
		}else if (JoinName.equals("LElbowRoll")){
			if (txtTimeLElbowRoll.getText().isEmpty()) Time=Float.valueOf(1);
			else   Time = Float.valueOf(txtTimeLElbowRoll.getText())+2;
			Movs="-139";
		}else if (JoinName.equals("RHipYawPitch")){
			if (txtTimeRHipYawPitch.getText().isEmpty()) Time=Float.valueOf(1);
			else  Time = Float.valueOf(txtTimeRHipYawPitch.getText())+2;
			Movs="0";
		}else if (JoinName.equals("RHipRoll")){
			if (txtTimeRHipRoll.getText().isEmpty()) Time=Float.valueOf(1);
			else  Time = Float.valueOf(txtTimeRHipRoll.getText())+2;
			Movs="0";
		}else if (JoinName.equals("RHipPitch")){
			if (txtTimeRHipPitch.getText().isEmpty()) Time=Float.valueOf(1);
			else  Time = Float.valueOf(txtTimeRHipPitch.getText())+2;
			Movs="-34";
		}else if (JoinName.equals("RKneePitch")){
			if (txtTimeRKneePitch.getText().isEmpty()) Time=Float.valueOf(1);
			else Time = Float.valueOf(txtTimeRKneePitch.getText())+2;
			Movs="69";
		}else if (JoinName.equals("RAnklePitch")){
			if (txtTimeRAnklePitch.getText().isEmpty()) Time=Float.valueOf(1);
			else   Time = Float.valueOf(txtTimeRAnklePitch.getText())+2;
			Movs="-34";
		}else if (JoinName.equals("RAnkleRoll")){
			if (txtTimeRAnkleRoll.getText().isEmpty()) Time=Float.valueOf(1);
			else  Time = Float.valueOf(txtTimeRAnkleRoll.getText())+2;
			Movs="0";
		}else if (JoinName.equals("LHipYawPitch")){
			if (txtTimeLHipYawPitch.getText().isEmpty()) Time=Float.valueOf(1);
			else   Time = Float.valueOf(txtTimeLHipYawPitch.getText())+2;
			Movs="0";
		}else if (JoinName.equals("LHipRoll")){
			if (txtTimeLHipRoll.getText().isEmpty()) Time=Float.valueOf(1);
			else   Time = Float.valueOf(txtTimeLHipRoll.getText())+2;
			Movs="0";
		}else if (JoinName.equals("LHipPitch")){
			if (txtTimeLHipPitch.getText().isEmpty()) Time=Float.valueOf(1);
			else   Time = Float.valueOf(txtTimeLHipPitch.getText())+2;
			Movs="-34";
		}else if (JoinName.equals("LKneePitch")){
			if (txtTimeLKneePitch.getText().isEmpty()) Time=Float.valueOf(1);
			else   Time = Float.valueOf(txtTimeLKneePitch.getText())+2;
			Movs="69";
		}else if (JoinName.equals("LAnklePitch")){
			if (txtTimeLAnklePitch.getText().isEmpty()) Time=Float.valueOf(1);
			else   Time = Float.valueOf(txtTimeLAnklePitch.getText())+2;
			Movs="-34";
		}else if (JoinName.equals("LAnkleRoll")){
			if (txtTimeLAnkleRoll.getText().isEmpty()) Time=Float.valueOf(1);
			else    Time = Float.valueOf(txtTimeLAnkleRoll.getText())+2;
			Movs="10";

		}


		JoinMov joinmov = new JoinMov();
		joinmov.JoinName = JoinName;
		for (int i=0;i<miNewmov.NumMove-1;i++){
			joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
			joinmov.times[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
		}
		joinmov.movs[NPosition] = String.valueOf(Float.valueOf(Movs)/100);
		joinmov.times[NPosition] = String.valueOf(Time);

		miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;

	}

	private String getTime(String JoinName){
		String Time="";


		//     System.out.println( levels[i]);

		if (JoinName.equals("HeadYaw")){
			Time = txtTimeHeadYaw.getText();
		}else if (JoinName.equals("HeadPitch")){
			Time = txtTimeHeadPitch.getText();
		}else if (JoinName.equals("RShoulderPitch")){
			Time = txtTimeRShoulderPitch.getText();
		}else if (JoinName.equals("RShoulderRoll")){
			Time = txtTimeRShoulderRoll.getText();
		}else if (JoinName.equals("RElbowYaw")){
			Time = txtTimeRElbowYaw.getText();
		}else if (JoinName.equals("RElbowRoll")){
			Time = txtTimeRElbowRoll.getText();
		}else if (JoinName.equals("LShoulderPitch")){
			Time = txtTimeLShoulderPitch.getText();
		}else if (JoinName.equals("LShoulderRoll")){
			Time = txtTimeLShoulderRoll.getText();
		}else if (JoinName.equals("LElbowYaw")){
			Time = txtTimeLElbowYaw.getText();
		}else if (JoinName.equals("LElbowRoll")){
			Time = txtTimeLElbowRoll.getText();
		}else if (JoinName.equals("RHipYawPitch")){
			Time = txtTimeRHipYawPitch.getText();
		}else if (JoinName.equals("RHipRoll")){
			Time = txtTimeRHipRoll.getText();
		}else if (JoinName.equals("RHipPitch")){
			Time = txtTimeRHipPitch.getText();
		}else if (JoinName.equals("RKneePitch")){
			Time = txtTimeRKneePitch.getText();
		}else if (JoinName.equals("RAnklePitch")){
			Time = txtTimeRAnklePitch.getText();
		}else if (JoinName.equals("RAnkleRoll")){
			Time = txtTimeRAnkleRoll.getText();
		}else if (JoinName.equals("LHipYawPitch")){
			Time = txtTimeLHipYawPitch.getText();
		}else if (JoinName.equals("LHipRoll")){
			Time = txtTimeLHipRoll.getText();
		}else if (JoinName.equals("LHipPitch")){
			Time = txtTimeLHipPitch.getText();
		}else if (JoinName.equals("LKneePitch")){
			Time = txtTimeLKneePitch.getText();
		}else if (JoinName.equals("LAnklePitch")){
			Time = txtTimeLAnklePitch.getText();
		}else if (JoinName.equals("LAnkleRoll")){
			Time = txtTimeLAnkleRoll.getText();

		}

		return Time;

	}
	private void printResp(dataResp r) {

		//sbyte[] util = new byte[r.size];

		if (r.size < 500) {

			System.out.println("Respuesta: [" + new String(r.data) + "] (" + r.size + ")");
		} else {
			System.out.println("Respuesta: [X] (" + r.size + ")");
		}

	}
	/** This method is called from within the constructor to
	 * initialize the form.
	 * WARNING: Do NOT modify this code. The content of this method is
	 * always regenerated by the FormEditor.
	 */
	// <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
	private void initComponents() {

		jLayeredPane1 = new javax.swing.JLayeredPane();
		GetValueRobot = new javax.swing.JButton();
		SendSlidertoRobotHead = new javax.swing.JCheckBox();
		doHead = new javax.swing.JCheckBox();
		jLabel5 = new javax.swing.JLabel();
		jLabel7 = new javax.swing.JLabel();
		HeadYaw = new javax.swing.JSlider();
		txtTimeHeadYaw = new javax.swing.JTextField();
		jLabel27 = new javax.swing.JLabel();
		HeadPitch = new javax.swing.JSlider();
		txtTimeHeadPitch = new javax.swing.JTextField();
		SaveValueHead = new javax.swing.JButton();
		jLabel6 = new javax.swing.JLabel();
		jLabel10 = new javax.swing.JLabel();
		doLArm = new javax.swing.JCheckBox();
		SendSlidertoRobotLArm = new javax.swing.JCheckBox();
		jLabel12 = new javax.swing.JLabel();
		jLabel11 = new javax.swing.JLabel();
		jLabel13 = new javax.swing.JLabel();
		jLabel14 = new javax.swing.JLabel();
		LElbowRoll = new javax.swing.JSlider();
		LElbowYaw = new javax.swing.JSlider();
		LShoulderRoll = new javax.swing.JSlider();
		LShoulderPitch = new javax.swing.JSlider();
		jLabel18 = new javax.swing.JLabel();
		txtTimeLShoulderPitch = new javax.swing.JTextField();
		txtTimeLShoulderRoll = new javax.swing.JTextField();
		txtTimeLElbowYaw = new javax.swing.JTextField();
		txtTimeLElbowRoll = new javax.swing.JTextField();
		SaveValueLArm = new javax.swing.JButton();
		jLabel32 = new javax.swing.JLabel();
		SendSlidertoRobotLLeg = new javax.swing.JCheckBox();
		doLLeg = new javax.swing.JCheckBox();
		jLabel34 = new javax.swing.JLabel();
		jLabel33 = new javax.swing.JLabel();
		jLabel42 = new javax.swing.JLabel();
		jLabel35 = new javax.swing.JLabel();
		jLabel36 = new javax.swing.JLabel();
		jLabel43 = new javax.swing.JLabel();
		LAnkleRoll = new javax.swing.JSlider();
		LAnklePitch = new javax.swing.JSlider();
		LKneePitch = new javax.swing.JSlider();
		LHipPitch = new javax.swing.JSlider();
		LHipRoll = new javax.swing.JSlider();
		LHipYawPitch = new javax.swing.JSlider();
		jLabel19 = new javax.swing.JLabel();
		txtTimeLHipYawPitch = new javax.swing.JTextField();
		txtTimeLHipRoll = new javax.swing.JTextField();
		txtTimeLHipPitch = new javax.swing.JTextField();
		txtTimeLKneePitch = new javax.swing.JTextField();
		txtTimeLAnklePitch = new javax.swing.JTextField();
		txtTimeLAnkleRoll = new javax.swing.JTextField();
		SaveValueLLeg = new javax.swing.JButton();
		jLabel20 = new javax.swing.JLabel();
		SendSlidertoRobotRLeg = new javax.swing.JCheckBox();
		doRLeg = new javax.swing.JCheckBox();
		jLabel22 = new javax.swing.JLabel();
		jLabel21 = new javax.swing.JLabel();
		jLabel30 = new javax.swing.JLabel();
		jLabel23 = new javax.swing.JLabel();
		jLabel24 = new javax.swing.JLabel();
		jLabel31 = new javax.swing.JLabel();
		RHipYawPitch = new javax.swing.JSlider();
		RHipRoll = new javax.swing.JSlider();
		RHipPitch = new javax.swing.JSlider();
		RKneePitch = new javax.swing.JSlider();
		RAnklePitch = new javax.swing.JSlider();
		RAnkleRoll = new javax.swing.JSlider();
		txtTimeRAnkleRoll = new javax.swing.JTextField();
		SaveValueRLeg = new javax.swing.JButton();
		txtTimeRAnklePitch = new javax.swing.JTextField();
		txtTimeRKneePitch = new javax.swing.JTextField();
		txtTimeRHipPitch = new javax.swing.JTextField();
		txtTimeRHipRoll = new javax.swing.JTextField();
		txtTimeRHipYawPitch = new javax.swing.JTextField();
		jLabel26 = new javax.swing.JLabel();
		jLabel2 = new javax.swing.JLabel();
		SendSlidertoRobotRArm = new javax.swing.JCheckBox();
		doRArm = new javax.swing.JCheckBox();
		jLabel4 = new javax.swing.JLabel();
		jLabel3 = new javax.swing.JLabel();
		jLabel8 = new javax.swing.JLabel();
		jLabel9 = new javax.swing.JLabel();
		RShoulderPitch = new javax.swing.JSlider();
		RShoulderRoll = new javax.swing.JSlider();
		RElbowYaw = new javax.swing.JSlider();
		RElbowRoll = new javax.swing.JSlider();
		txtTimeRElbowRoll = new javax.swing.JTextField();
		txtTimeRElbowYaw = new javax.swing.JTextField();
		txtTimeRShoulderRoll = new javax.swing.JTextField();
		txtTimeRShoulderPitch = new javax.swing.JTextField();
		jLabel25 = new javax.swing.JLabel();
		SaveValueRArm = new javax.swing.JButton();
		jLabel28 = new javax.swing.JLabel();
		NewMov = new javax.swing.JButton();
		txtNewFileMov = new javax.swing.JTextField();
		txtNewJoinName = new javax.swing.JTextField();
		jLabel29 = new javax.swing.JLabel();
		jLabel37 = new javax.swing.JLabel();
		txtNewNumberMove = new javax.swing.JTextField();
		jLabel38 = new javax.swing.JLabel();
		txtNewMovPosition = new javax.swing.JTextField();
		btnNewAnt = new javax.swing.JButton();
		btnAdd = new javax.swing.JButton();
		NewTestInRobot = new javax.swing.JButton();
		NewPlayMovement = new javax.swing.JButton();
		jLabel1 = new javax.swing.JLabel();
		LoadMov = new javax.swing.JButton();
		jLabel15 = new javax.swing.JLabel();
		jLabel16 = new javax.swing.JLabel();
		jLabel17 = new javax.swing.JLabel();
		txtMovPosition = new javax.swing.JTextField();
		btnAnt = new javax.swing.JButton();
		btnSig = new javax.swing.JButton();
		PlayMovement = new javax.swing.JButton();
		TestInRobot = new javax.swing.JButton();
		txtNumberMove = new javax.swing.JTextField();
		jLabel39 = new javax.swing.JLabel();
		txtNumJoin = new javax.swing.JTextField();
		txtJoinName = new javax.swing.JTextField();
		txtFileMov = new javax.swing.JTextField();
		jSeparator1 = new javax.swing.JSeparator();
		jSeparator2 = new javax.swing.JSeparator();
		jSeparator3 = new javax.swing.JSeparator();
		jSeparator4 = new javax.swing.JSeparator();
		jSeparator5 = new javax.swing.JSeparator();
		jLabel40 = new javax.swing.JLabel();
		txtNewNumJoin = new javax.swing.JTextField();
		FinalizeMovement = new javax.swing.JButton();
		InitPositionRobot = new javax.swing.JButton();
		ModifyMovement = new javax.swing.JButton();
		SaveMov = new javax.swing.JButton();
		btnNewNext = new javax.swing.JButton();
		btnInsert = new javax.swing.JButton();
		btnDelete = new javax.swing.JButton();
		getAngleHead = new javax.swing.JButton();
		getAngleLArm = new javax.swing.JButton();
		getAngleLLeg = new javax.swing.JButton();
		getAngleRArm = new javax.swing.JButton();
		getAngleRLeg = new javax.swing.JButton();
		DisableStiffnessHead = new javax.swing.JCheckBox();
		DisableStiffnessLLeg = new javax.swing.JCheckBox();
		DisableStiffnessLArm = new javax.swing.JCheckBox();
		DisableStiffnessRArm = new javax.swing.JCheckBox();
		DisableStiffnessRLeg = new javax.swing.JCheckBox();

		setLayout(new java.awt.BorderLayout());

		GetValueRobot.setText("Get Value Robot");
		GetValueRobot.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				GetValueRobotActionPerformed(evt);
			}
		});
		GetValueRobot.setBounds(10, 10, 130, 27);
		jLayeredPane1.add(GetValueRobot, javax.swing.JLayeredPane.DEFAULT_LAYER);

		SendSlidertoRobotHead.setText("Send to Robot");
		SendSlidertoRobotHead.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				SendSlidertoRobotHeadActionPerformed(evt);
			}
		});
		SendSlidertoRobotHead.setBounds(10, 70, 140, 22);
		jLayeredPane1.add(SendSlidertoRobotHead, javax.swing.JLayeredPane.DEFAULT_LAYER);

		doHead.setText("Do");
		doHead.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				doHeadActionPerformed(evt);
			}
		});
		doHead.setBounds(240, 60, 50, 22);
		jLayeredPane1.add(doHead, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel5.setText("Head");
		jLabel5.setBounds(10, 50, 45, 15);
		jLayeredPane1.add(jLabel5, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel7.setText("HeadYaw [-1.57 .. 1.57]");
		jLabel7.setBounds(10, 100, 160, 15);
		jLayeredPane1.add(jLabel7, javax.swing.JLayeredPane.DEFAULT_LAYER);

		HeadYaw.setMaximum(157);
		HeadYaw.setMinimum(-157);
		HeadYaw.setValue(0);
		HeadYaw.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				HeadYawMouseReleased(evt);
			}
		});
		HeadYaw.addKeyListener(new java.awt.event.KeyAdapter() {
			public void keyReleased(java.awt.event.KeyEvent evt) {
				HeadYawKeyReleased(evt);
			}
		});
		HeadYaw.setBounds(200, 80, 120, 40);
		jLayeredPane1.add(HeadYaw, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeHeadYaw.setBounds(330, 90, 40, 25);
		jLayeredPane1.add(txtTimeHeadYaw, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel27.setText("Time");
		jLabel27.setBounds(330, 70, 53, 15);
		jLayeredPane1.add(jLabel27, javax.swing.JLayeredPane.DEFAULT_LAYER);

		HeadPitch.setMaximum(79);
		HeadPitch.setMinimum(-79);
		HeadPitch.setValue(0);
		HeadPitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				HeadPitchMouseReleased(evt);
			}
		});
		HeadPitch.setBounds(200, 110, 120, 40);
		jLayeredPane1.add(HeadPitch, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeHeadPitch.setBounds(330, 120, 40, 25);
		jLayeredPane1.add(txtTimeHeadPitch, javax.swing.JLayeredPane.DEFAULT_LAYER);

		SaveValueHead.setText("Save Value");
		SaveValueHead.setEnabled(false);
		SaveValueHead.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				SaveValueHeadActionPerformed(evt);
			}
		});
		SaveValueHead.setBounds(380, 110, 90, 27);
		jLayeredPane1.add(SaveValueHead, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel6.setText("HeadPitch [-0.79 .. 0.79]");
		jLabel6.setBounds(10, 130, 180, 15);
		jLayeredPane1.add(jLabel6, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel10.setText("Left Arm");
		jLabel10.setBounds(10, 170, 80, 15);
		jLayeredPane1.add(jLabel10, javax.swing.JLayeredPane.DEFAULT_LAYER);

		doLArm.setText("Do");
		doLArm.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				doLArmActionPerformed(evt);
			}
		});
		doLArm.setBounds(130, 190, 50, 22);
		jLayeredPane1.add(doLArm, javax.swing.JLayeredPane.DEFAULT_LAYER);

		SendSlidertoRobotLArm.setText("Send to Robot");
		SendSlidertoRobotLArm.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				SendSlidertoRobotLArmActionPerformed(evt);
			}
		});
		SendSlidertoRobotLArm.setBounds(10, 190, 140, 22);
		jLayeredPane1.add(SendSlidertoRobotLArm, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel12.setText("LShoulderPitch [-2.09 .. 2.09]");
		jLabel12.setBounds(10, 220, 200, 15);
		jLayeredPane1.add(jLabel12, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel11.setText("LShoulderRoll [0.00 .. 1.66]");
		jLabel11.setBounds(10, 250, 180, 15);
		jLayeredPane1.add(jLabel11, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel13.setText("LElbowYaw [-2.09 .. 2.09]");
		jLabel13.setBounds(10, 280, 180, 15);
		jLayeredPane1.add(jLabel13, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel14.setText("LElbowRoll [-1.66 .. 0.00]");
		jLabel14.setBounds(10, 310, 180, 15);
		jLayeredPane1.add(jLabel14, javax.swing.JLayeredPane.DEFAULT_LAYER);

		LElbowRoll.setMaximum(0);
		LElbowRoll.setMinimum(-166);
		LElbowRoll.setValue(-100);
		LElbowRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				LElbowRollMouseReleased(evt);
			}
		});
		LElbowRoll.setBounds(200, 290, 120, 40);
		jLayeredPane1.add(LElbowRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);

		LElbowYaw.setMaximum(209);
		LElbowYaw.setMinimum(-209);
		LElbowYaw.setValue(0);
		LElbowYaw.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				LElbowYawMouseReleased(evt);
			}
		});
		LElbowYaw.setBounds(200, 260, 120, 40);
		jLayeredPane1.add(LElbowYaw, javax.swing.JLayeredPane.DEFAULT_LAYER);

		LShoulderRoll.setMaximum(166);
		LShoulderRoll.setValue(100);
		LShoulderRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				LShoulderRollMouseReleased(evt);
			}
		});
		LShoulderRoll.setBounds(200, 230, 120, 40);
		jLayeredPane1.add(LShoulderRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);

		LShoulderPitch.setMaximum(209);
		LShoulderPitch.setMinimum(-209);
		LShoulderPitch.setValue(0);
		LShoulderPitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				LShoulderPitchMouseReleased(evt);
			}
		});
		LShoulderPitch.setBounds(200, 200, 120, 40);
		jLayeredPane1.add(LShoulderPitch, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel18.setText("Time");
		jLabel18.setBounds(330, 190, 53, 15);
		jLayeredPane1.add(jLabel18, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeLShoulderPitch.setBounds(330, 210, 40, 25);
		jLayeredPane1.add(txtTimeLShoulderPitch, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeLShoulderRoll.setBounds(330, 240, 40, 25);
		jLayeredPane1.add(txtTimeLShoulderRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeLElbowYaw.setBounds(330, 270, 40, 25);
		jLayeredPane1.add(txtTimeLElbowYaw, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeLElbowRoll.setBounds(330, 300, 40, 25);
		jLayeredPane1.add(txtTimeLElbowRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);

		SaveValueLArm.setText("Save Value");
		SaveValueLArm.setEnabled(false);
		SaveValueLArm.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				SaveValueLArmActionPerformed(evt);
			}
		});
		SaveValueLArm.setBounds(380, 300, 90, 27);
		jLayeredPane1.add(SaveValueLArm, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel32.setText("Left Leg");
		jLabel32.setBounds(10, 340, 70, 20);
		jLayeredPane1.add(jLabel32, javax.swing.JLayeredPane.DEFAULT_LAYER);

		SendSlidertoRobotLLeg.setText("Send to Robot");
		SendSlidertoRobotLLeg.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				SendSlidertoRobotLLegActionPerformed(evt);
			}
		});
		SendSlidertoRobotLLeg.setBounds(10, 360, 140, 22);
		jLayeredPane1.add(SendSlidertoRobotLLeg, javax.swing.JLayeredPane.DEFAULT_LAYER);

		doLLeg.setText("Do");
		doLLeg.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				doLLegActionPerformed(evt);
			}
		});
		doLLeg.setBounds(130, 360, 50, 22);
		jLayeredPane1.add(doLLeg, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel34.setText("LHipYawPitch [-0.94 .. 0.68]");
		jLabel34.setBounds(10, 390, 200, 15);
		jLayeredPane1.add(jLabel34, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel33.setText("LHipRoll [-0.44 .. 0.79]");
		jLabel33.setBounds(10, 420, 180, 15);
		jLayeredPane1.add(jLabel33, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel42.setText("LHipPitch [-1.57 .. 0.52]");
		jLabel42.setBounds(10, 450, 180, 15);
		jLayeredPane1.add(jLabel42, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel35.setText("LKneePitch [0.00 .. 2.27]");
		jLabel35.setBounds(10, 480, 180, 15);
		jLayeredPane1.add(jLabel35, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel36.setText("LAnklePitch [-1.22 .. 0.79]");
		jLabel36.setBounds(10, 510, 180, 15);
		jLayeredPane1.add(jLabel36, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel43.setText("LAnkleRoll [-0.79 .. 0.79]");
		jLabel43.setBounds(10, 540, 180, 15);
		jLayeredPane1.add(jLabel43, javax.swing.JLayeredPane.DEFAULT_LAYER);

		LAnkleRoll.setMaximum(79);
		LAnkleRoll.setMinimum(-79);
		LAnkleRoll.setValue(0);
		LAnkleRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				LAnkleRollMouseReleased(evt);
			}
		});
		LAnkleRoll.setBounds(200, 520, 120, 40);
		jLayeredPane1.add(LAnkleRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);

		LAnklePitch.setMaximum(79);
		LAnklePitch.setMinimum(-122);
		LAnklePitch.setValue(0);
		LAnklePitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				LAnklePitchMouseReleased(evt);
			}
		});
		LAnklePitch.setBounds(200, 490, 120, 40);
		jLayeredPane1.add(LAnklePitch, javax.swing.JLayeredPane.DEFAULT_LAYER);

		LKneePitch.setMaximum(227);
		LKneePitch.setValue(100);
		LKneePitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				LKneePitchMouseReleased(evt);
			}
		});
		LKneePitch.setBounds(200, 460, 120, 40);
		jLayeredPane1.add(LKneePitch, javax.swing.JLayeredPane.DEFAULT_LAYER);

		LHipPitch.setMaximum(52);
		LHipPitch.setMinimum(-157);
		LHipPitch.setValue(0);
		LHipPitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				LHipPitchMouseReleased(evt);
			}
		});
		LHipPitch.setBounds(200, 440, 120, 30);
		jLayeredPane1.add(LHipPitch, javax.swing.JLayeredPane.DEFAULT_LAYER);

		LHipRoll.setMaximum(79);
		LHipRoll.setMinimum(-44);
		LHipRoll.setValue(0);
		LHipRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				LHipRollMouseReleased(evt);
			}
		});
		LHipRoll.setBounds(200, 400, 120, 43);
		jLayeredPane1.add(LHipRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);

		LHipYawPitch.setMaximum(68);
		LHipYawPitch.setMinimum(-94);
		LHipYawPitch.setValue(0);
		LHipYawPitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				LHipYawPitchMouseReleased(evt);
			}
		});
		LHipYawPitch.setBounds(200, 370, 120, 40);
		jLayeredPane1.add(LHipYawPitch, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel19.setText("Time");
		jLabel19.setBounds(330, 360, 53, 15);
		jLayeredPane1.add(jLabel19, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeLHipYawPitch.setBounds(330, 380, 40, 25);
		jLayeredPane1.add(txtTimeLHipYawPitch, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeLHipRoll.setBounds(330, 410, 40, 25);
		jLayeredPane1.add(txtTimeLHipRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeLHipPitch.setBounds(330, 440, 40, 25);
		jLayeredPane1.add(txtTimeLHipPitch, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeLKneePitch.setBounds(330, 470, 40, 25);
		jLayeredPane1.add(txtTimeLKneePitch, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeLAnklePitch.setBounds(330, 500, 40, 25);
		jLayeredPane1.add(txtTimeLAnklePitch, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeLAnkleRoll.setBounds(330, 530, 40, 25);
		jLayeredPane1.add(txtTimeLAnkleRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);

		SaveValueLLeg.setText("Save Value");
		SaveValueLLeg.setEnabled(false);
		SaveValueLLeg.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				SaveValueLLegActionPerformed(evt);
			}
		});
		SaveValueLLeg.setBounds(380, 530, 90, 27);
		jLayeredPane1.add(SaveValueLLeg, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel20.setText("Rigth Leg");
		jLabel20.setBounds(480, 340, 70, 20);
		jLayeredPane1.add(jLabel20, javax.swing.JLayeredPane.DEFAULT_LAYER);

		SendSlidertoRobotRLeg.setText("Send to Robot");
		SendSlidertoRobotRLeg.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				SendSlidertoRobotRLegActionPerformed(evt);
			}
		});
		SendSlidertoRobotRLeg.setBounds(480, 360, 140, 22);
		jLayeredPane1.add(SendSlidertoRobotRLeg, javax.swing.JLayeredPane.DEFAULT_LAYER);

		doRLeg.setText("Do");
		doRLeg.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				doRLegActionPerformed(evt);
			}
		});
		doRLeg.setBounds(620, 360, 50, 22);
		jLayeredPane1.add(doRLeg, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel22.setText("RHipYawPitch [-0.94 .. 0.68]");
		jLabel22.setBounds(480, 390, 200, 15);
		jLayeredPane1.add(jLabel22, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel21.setText("RHipRoll [-0.79 .. 0.44]");
		jLabel21.setBounds(480, 420, 180, 15);
		jLayeredPane1.add(jLabel21, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel30.setText("RHipPitch [-1.57 .. 0.52]");
		jLabel30.setBounds(480, 450, 180, 15);
		jLayeredPane1.add(jLabel30, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel23.setText("RKneePitch [0.00 .. 2.27]");
		jLabel23.setBounds(480, 480, 180, 15);
		jLayeredPane1.add(jLabel23, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel24.setText("RAnklePitch [-1.22 .. 0.79]");
		jLabel24.setBounds(480, 510, 180, 15);
		jLayeredPane1.add(jLabel24, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel31.setText("RAnkleRoll [-0.79 .. 0.79]");
		jLabel31.setBounds(480, 540, 180, 15);
		jLayeredPane1.add(jLabel31, javax.swing.JLayeredPane.DEFAULT_LAYER);

		RHipYawPitch.setMaximum(68);
		RHipYawPitch.setMinimum(-94);
		RHipYawPitch.setValue(0);
		RHipYawPitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				RHipYawPitchMouseReleased(evt);
			}
		});
		RHipYawPitch.setBounds(670, 370, 120, 40);
		jLayeredPane1.add(RHipYawPitch, javax.swing.JLayeredPane.DEFAULT_LAYER);

		RHipRoll.setMaximum(44);
		RHipRoll.setMinimum(-79);
		RHipRoll.setValue(0);
		RHipRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				RHipRollMouseReleased(evt);
			}
		});
		RHipRoll.setBounds(670, 400, 120, 40);
		jLayeredPane1.add(RHipRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);

		RHipPitch.setMaximum(52);
		RHipPitch.setMinimum(-157);
		RHipPitch.setValue(0);
		RHipPitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				RHipPitchMouseReleased(evt);
			}
		});
		RHipPitch.setBounds(670, 430, 120, 40);
		jLayeredPane1.add(RHipPitch, javax.swing.JLayeredPane.DEFAULT_LAYER);

		RKneePitch.setMaximum(227);
		RKneePitch.setValue(100);
		RKneePitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				RKneePitchMouseReleased(evt);
			}
		});
		RKneePitch.setBounds(670, 460, 120, 40);
		jLayeredPane1.add(RKneePitch, javax.swing.JLayeredPane.DEFAULT_LAYER);

		RAnklePitch.setMaximum(79);
		RAnklePitch.setMinimum(-122);
		RAnklePitch.setValue(0);
		RAnklePitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				RAnklePitchMouseReleased(evt);
			}
		});
		RAnklePitch.setBounds(670, 490, 120, 40);
		jLayeredPane1.add(RAnklePitch, javax.swing.JLayeredPane.DEFAULT_LAYER);

		RAnkleRoll.setMaximum(79);
		RAnkleRoll.setMinimum(-79);
		RAnkleRoll.setValue(0);
		RAnkleRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				RAnkleRollMouseReleased(evt);
			}
		});
		RAnkleRoll.setBounds(670, 520, 120, 40);
		jLayeredPane1.add(RAnkleRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeRAnkleRoll.setBounds(800, 530, 40, 25);
		jLayeredPane1.add(txtTimeRAnkleRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);

		SaveValueRLeg.setText("Save Value");
		SaveValueRLeg.setEnabled(false);
		SaveValueRLeg.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				SaveValueRLegActionPerformed(evt);
			}
		});
		SaveValueRLeg.setBounds(850, 530, 90, 27);
		jLayeredPane1.add(SaveValueRLeg, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeRAnklePitch.setBounds(800, 500, 40, 25);
		jLayeredPane1.add(txtTimeRAnklePitch, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeRKneePitch.setBounds(800, 470, 40, 25);
		jLayeredPane1.add(txtTimeRKneePitch, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeRHipPitch.setBounds(800, 440, 40, 25);
		jLayeredPane1.add(txtTimeRHipPitch, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeRHipRoll.setBounds(800, 410, 40, 25);
		jLayeredPane1.add(txtTimeRHipRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeRHipYawPitch.setBounds(800, 380, 40, 25);
		jLayeredPane1.add(txtTimeRHipYawPitch, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel26.setText("Time");
		jLabel26.setBounds(800, 360, 53, 15);
		jLayeredPane1.add(jLabel26, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel2.setText("Rigth Arm");
		jLabel2.setBounds(480, 170, 70, 20);
		jLayeredPane1.add(jLabel2, javax.swing.JLayeredPane.DEFAULT_LAYER);

		SendSlidertoRobotRArm.setText("Send to Robot");
		SendSlidertoRobotRArm.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				SendSlidertoRobotRArmActionPerformed(evt);
			}
		});
		SendSlidertoRobotRArm.setBounds(480, 190, 140, 22);
		jLayeredPane1.add(SendSlidertoRobotRArm, javax.swing.JLayeredPane.DEFAULT_LAYER);

		doRArm.setText("Do");
		doRArm.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				doRArmActionPerformed(evt);
			}
		});
		doRArm.setBounds(620, 190, 50, 22);
		jLayeredPane1.add(doRArm, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel4.setText("RShoulderPitch [-2.09 .. 2.09]");
		jLabel4.setBounds(480, 220, 200, 15);
		jLayeredPane1.add(jLabel4, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel3.setText("RShoulderRoll [-1.66 .. 0.00]");
		jLabel3.setBounds(480, 250, 180, 15);
		jLayeredPane1.add(jLabel3, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel8.setText("RElbowYaw [-2.09 .. 2.09]");
		jLabel8.setBounds(480, 280, 180, 15);
		jLayeredPane1.add(jLabel8, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel9.setText("RElbowRoll [0.00 .. 1.66]");
		jLabel9.setBounds(480, 310, 180, 15);
		jLayeredPane1.add(jLabel9, javax.swing.JLayeredPane.DEFAULT_LAYER);

		RShoulderPitch.setMaximum(209);
		RShoulderPitch.setMinimum(-209);
		RShoulderPitch.setValue(0);
		RShoulderPitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				RShoulderPitchMouseReleased(evt);
			}
		});
		RShoulderPitch.setBounds(670, 200, 120, 40);
		jLayeredPane1.add(RShoulderPitch, javax.swing.JLayeredPane.DEFAULT_LAYER);

		RShoulderRoll.setMaximum(0);
		RShoulderRoll.setMinimum(-166);
		RShoulderRoll.setValue(-100);
		RShoulderRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				RShoulderRollMouseReleased(evt);
			}
		});
		RShoulderRoll.setBounds(670, 230, 120, 40);
		jLayeredPane1.add(RShoulderRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);

		RElbowYaw.setMaximum(209);
		RElbowYaw.setMinimum(-209);
		RElbowYaw.setValue(0);
		RElbowYaw.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				RElbowYawMouseReleased(evt);
			}
		});
		RElbowYaw.setBounds(670, 260, 120, 40);
		jLayeredPane1.add(RElbowYaw, javax.swing.JLayeredPane.DEFAULT_LAYER);

		RElbowRoll.setMaximum(166);
		RElbowRoll.setValue(100);
		RElbowRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				RElbowRollMouseReleased(evt);
			}
		});
		RElbowRoll.setBounds(670, 290, 120, 40);
		jLayeredPane1.add(RElbowRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeRElbowRoll.setBounds(800, 300, 40, 25);
		jLayeredPane1.add(txtTimeRElbowRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeRElbowYaw.setBounds(800, 270, 40, 25);
		jLayeredPane1.add(txtTimeRElbowYaw, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeRShoulderRoll.setBounds(800, 240, 40, 25);
		jLayeredPane1.add(txtTimeRShoulderRoll, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtTimeRShoulderPitch.setBounds(800, 210, 40, 25);
		jLayeredPane1.add(txtTimeRShoulderPitch, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel25.setText("Time");
		jLabel25.setBounds(800, 190, 53, 15);
		jLayeredPane1.add(jLabel25, javax.swing.JLayeredPane.DEFAULT_LAYER);

		SaveValueRArm.setText("Save Value");
		SaveValueRArm.setEnabled(false);
		SaveValueRArm.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				SaveValueRArmActionPerformed(evt);
			}
		});
		SaveValueRArm.setBounds(850, 300, 90, 27);
		jLayeredPane1.add(SaveValueRArm, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel28.setText("Create New Movement");
		jLabel28.setBounds(480, 10, 170, 15);
		jLayeredPane1.add(jLabel28, javax.swing.JLayeredPane.DEFAULT_LAYER);

		NewMov.setText("New .mov");
		NewMov.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				NewMovActionPerformed(evt);
			}
		});
		NewMov.setBounds(480, 30, 100, 27);
		jLayeredPane1.add(NewMov, javax.swing.JLayeredPane.DEFAULT_LAYER);

		txtNewFileMov.setEnabled(false);
		txtNewFileMov.setBounds(590, 30, 160, 25);
		jLayeredPane1.add(txtNewFileMov, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtNewJoinName.setBounds(580, 60, 100, 25);
		jLayeredPane1.add(txtNewJoinName, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel29.setText("Movement:");
		jLabel29.setBounds(480, 70, 80, 15);
		jLayeredPane1.add(jLabel29, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel37.setText("Number Move:");
		jLabel37.setBounds(690, 70, 100, 15);
		jLayeredPane1.add(jLabel37, javax.swing.JLayeredPane.DEFAULT_LAYER);
		txtNewNumberMove.setBounds(790, 60, 40, 25);
		jLayeredPane1.add(txtNewNumberMove, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel38.setText("Mov Position:");
		jLabel38.setBounds(480, 100, 90, 15);
		jLayeredPane1.add(jLabel38, javax.swing.JLayeredPane.DEFAULT_LAYER);

		txtNewMovPosition.setEnabled(false);
		txtNewMovPosition.setBounds(570, 90, 40, 25);
		jLayeredPane1.add(txtNewMovPosition, javax.swing.JLayeredPane.DEFAULT_LAYER);

		btnNewAnt.setText("Previous");
		btnNewAnt.setEnabled(false);
		btnNewAnt.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				btnNewAntActionPerformed(evt);
			}
		});
		btnNewAnt.setBounds(480, 120, 70, 27);
		jLayeredPane1.add(btnNewAnt, javax.swing.JLayeredPane.DEFAULT_LAYER);

		btnAdd.setText("Add");
		btnAdd.setEnabled(false);
		btnAdd.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				btnAddActionPerformed(evt);
			}
		});
		btnAdd.setBounds(610, 120, 37, 27);
		jLayeredPane1.add(btnAdd, javax.swing.JLayeredPane.DEFAULT_LAYER);

		NewTestInRobot.setText("Test Robot");
		NewTestInRobot.setEnabled(false);
		NewTestInRobot.setFocusPainted(false);
		NewTestInRobot.setFocusable(false);
		NewTestInRobot.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				NewTestInRobotActionPerformed(evt);
			}
		});
		NewTestInRobot.setBounds(710, 120, 80, 27);
		jLayeredPane1.add(NewTestInRobot, javax.swing.JLayeredPane.DEFAULT_LAYER);

		NewPlayMovement.setText("Play Mov.");
		NewPlayMovement.setEnabled(false);
		NewPlayMovement.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				NewPlayMovementActionPerformed(evt);
			}
		});
		NewPlayMovement.setBounds(790, 120, 80, 27);
		jLayeredPane1.add(NewPlayMovement, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel1.setText("Load Movements");
		jLabel1.setBounds(880, 10, 120, 15);
		jLayeredPane1.add(jLabel1, javax.swing.JLayeredPane.DEFAULT_LAYER);

		LoadMov.setText("Load .mov");
		LoadMov.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				LoadMovActionPerformed(evt);
			}
		});
		LoadMov.setBounds(880, 30, 100, 27);
		jLayeredPane1.add(LoadMov, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel15.setText("Movement:");
		jLabel15.setBounds(880, 70, 80, 15);
		jLayeredPane1.add(jLabel15, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel16.setText("Number Move:");
		jLabel16.setBounds(880, 100, 100, 15);
		jLayeredPane1.add(jLabel16, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel17.setText("Mov Position:");
		jLabel17.setBounds(880, 130, 90, 15);
		jLayeredPane1.add(jLabel17, javax.swing.JLayeredPane.DEFAULT_LAYER);

		txtMovPosition.setEnabled(false);
		txtMovPosition.setBounds(970, 120, 40, 25);
		jLayeredPane1.add(txtMovPosition, javax.swing.JLayeredPane.DEFAULT_LAYER);

		btnAnt.setText("Previous");
		btnAnt.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				btnAntActionPerformed(evt);
			}
		});
		btnAnt.setBounds(1020, 120, 66, 27);
		jLayeredPane1.add(btnAnt, javax.swing.JLayeredPane.DEFAULT_LAYER);

		btnSig.setText("Next");
		btnSig.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				btnSigActionPerformed(evt);
			}
		});
		btnSig.setBounds(1090, 120, 42, 27);
		jLayeredPane1.add(btnSig, javax.swing.JLayeredPane.DEFAULT_LAYER);

		PlayMovement.setText("Play Mov.");
		PlayMovement.setEnabled(false);
		PlayMovement.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				PlayMovementActionPerformed(evt);
			}
		});
		PlayMovement.setBounds(1060, 150, 80, 27);
		jLayeredPane1.add(PlayMovement, javax.swing.JLayeredPane.DEFAULT_LAYER);

		TestInRobot.setText("Test Robot");
		TestInRobot.setEnabled(false);
		TestInRobot.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				TestInRobotActionPerformed(evt);
			}
		});
		TestInRobot.setBounds(960, 150, 80, 27);
		jLayeredPane1.add(TestInRobot, javax.swing.JLayeredPane.DEFAULT_LAYER);

		txtNumberMove.setEnabled(false);
		txtNumberMove.setBounds(990, 90, 40, 25);
		jLayeredPane1.add(txtNumberMove, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel39.setText("Number Join:");
		jLabel39.setBounds(1030, 100, 90, 15);
		jLayeredPane1.add(jLabel39, javax.swing.JLayeredPane.DEFAULT_LAYER);

		txtNumJoin.setEnabled(false);
		txtNumJoin.setBounds(1140, 100, 40, 25);
		jLayeredPane1.add(txtNumJoin, javax.swing.JLayeredPane.DEFAULT_LAYER);

		txtJoinName.setEnabled(false);
		txtJoinName.setBounds(990, 60, 100, 25);
		jLayeredPane1.add(txtJoinName, javax.swing.JLayeredPane.DEFAULT_LAYER);

		txtFileMov.setEnabled(false);
		txtFileMov.setBounds(990, 30, 160, 25);
		jLayeredPane1.add(txtFileMov, javax.swing.JLayeredPane.DEFAULT_LAYER);
		jSeparator1.setBounds(0, 330, 940, 10);
		jLayeredPane1.add(jSeparator1, javax.swing.JLayeredPane.DEFAULT_LAYER);
		jSeparator2.setBounds(870, 190, 290, 6);
		jLayeredPane1.add(jSeparator2, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jSeparator3.setOrientation(javax.swing.SwingConstants.VERTICAL);
		jSeparator3.setBounds(470, 10, 10, 560);
		jLayeredPane1.add(jSeparator3, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jSeparator4.setOrientation(javax.swing.SwingConstants.VERTICAL);
		jSeparator4.setBounds(870, 10, 10, 180);
		jLayeredPane1.add(jSeparator4, javax.swing.JLayeredPane.DEFAULT_LAYER);
		jSeparator5.setBounds(0, 170, 870, 10);
		jLayeredPane1.add(jSeparator5, javax.swing.JLayeredPane.DEFAULT_LAYER);

		jLabel40.setText("Number Join:");
		jLabel40.setBounds(620, 100, 90, 15);
		jLayeredPane1.add(jLabel40, javax.swing.JLayeredPane.DEFAULT_LAYER);

		txtNewNumJoin.setEnabled(false);
		txtNewNumJoin.setBounds(700, 90, 40, 25);
		jLayeredPane1.add(txtNewNumJoin, javax.swing.JLayeredPane.DEFAULT_LAYER);

		FinalizeMovement.setText("Finalize");
		FinalizeMovement.setEnabled(false);
		FinalizeMovement.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				FinalizeMovementActionPerformed(evt);
			}
		});
		FinalizeMovement.setBounds(750, 90, 59, 27);
		jLayeredPane1.add(FinalizeMovement, javax.swing.JLayeredPane.DEFAULT_LAYER);

		InitPositionRobot.setText("Init Position Robot");
		InitPositionRobot.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				InitPositionRobotActionPerformed(evt);
			}
		});
		InitPositionRobot.setBounds(170, 10, 130, 27);
		jLayeredPane1.add(InitPositionRobot, javax.swing.JLayeredPane.DEFAULT_LAYER);

		ModifyMovement.setText("Modify Movement");
		ModifyMovement.setEnabled(false);
		ModifyMovement.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				ModifyMovementActionPerformed(evt);
			}
		});
		ModifyMovement.setBounds(740, 0, 125, 27);
		jLayeredPane1.add(ModifyMovement, javax.swing.JLayeredPane.DEFAULT_LAYER);

		SaveMov.setText("Save");
		SaveMov.setEnabled(false);
		SaveMov.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				SaveMovActionPerformed(evt);
			}
		});
		SaveMov.setBounds(810, 90, 44, 27);
		jLayeredPane1.add(SaveMov, javax.swing.JLayeredPane.DEFAULT_LAYER);

		btnNewNext.setText("Next");
		btnNewNext.setEnabled(false);
		btnNewNext.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				btnNewNextActionPerformed(evt);
			}
		});
		btnNewNext.setBounds(550, 120, 50, 27);
		jLayeredPane1.add(btnNewNext, javax.swing.JLayeredPane.DEFAULT_LAYER);

		btnInsert.setText("Insert");
		btnInsert.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				btnInsertActionPerformed(evt);
			}
		});
		btnInsert.setBounds(650, 120, 50, 27);
		jLayeredPane1.add(btnInsert, javax.swing.JLayeredPane.DEFAULT_LAYER);

		btnDelete.setText("Delete");
		btnDelete.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				btnDeleteActionPerformed(evt);
			}
		});
		btnDelete.setBounds(710, 150, 54, 20);
		jLayeredPane1.add(btnDelete, javax.swing.JLayeredPane.DEFAULT_LAYER);

		getAngleHead.setText("Get Angle");
		getAngleHead.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				getAngleHeadActionPerformed(evt);
			}
		});
		getAngleHead.setBounds(380, 80, 90, 27);
		jLayeredPane1.add(getAngleHead, javax.swing.JLayeredPane.DEFAULT_LAYER);

		getAngleLArm.setText("Get Angle");
		getAngleLArm.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				getAngleLArmActionPerformed(evt);
			}
		});
		getAngleLArm.setBounds(380, 270, 90, 27);
		jLayeredPane1.add(getAngleLArm, javax.swing.JLayeredPane.DEFAULT_LAYER);

		getAngleLLeg.setText("Get Angle");
		getAngleLLeg.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				getAngleLLegActionPerformed(evt);
			}
		});
		getAngleLLeg.setBounds(380, 500, 90, 27);
		jLayeredPane1.add(getAngleLLeg, javax.swing.JLayeredPane.DEFAULT_LAYER);

		getAngleRArm.setText("Get Angle");
		getAngleRArm.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				getAngleRArmActionPerformed(evt);
			}
		});
		getAngleRArm.setBounds(850, 270, 90, 27);
		jLayeredPane1.add(getAngleRArm, javax.swing.JLayeredPane.DEFAULT_LAYER);

		getAngleRLeg.setText("Get Angle");
		getAngleRLeg.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				getAngleRLegActionPerformed(evt);
			}
		});
		getAngleRLeg.setBounds(850, 500, 90, 27);
		jLayeredPane1.add(getAngleRLeg, javax.swing.JLayeredPane.DEFAULT_LAYER);

		DisableStiffnessHead.setText("Disable Stiffness");
		DisableStiffnessHead.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				DisableStiffnessHeadActionPerformed(evt);
			}
		});
		DisableStiffnessHead.setBounds(60, 50, 140, 22);
		jLayeredPane1.add(DisableStiffnessHead, javax.swing.JLayeredPane.DEFAULT_LAYER);

		DisableStiffnessLLeg.setText("Disable Stiffness");
		DisableStiffnessLLeg.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				DisableStiffnessLLegActionPerformed(evt);
			}
		});
		DisableStiffnessLLeg.setBounds(70, 340, 140, 22);
		jLayeredPane1.add(DisableStiffnessLLeg, javax.swing.JLayeredPane.DEFAULT_LAYER);

		DisableStiffnessLArm.setText("Disable Stiffness");
		DisableStiffnessLArm.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				DisableStiffnessLArmActionPerformed(evt);
			}
		});
		DisableStiffnessLArm.setBounds(70, 170, 150, 22);
		jLayeredPane1.add(DisableStiffnessLArm, javax.swing.JLayeredPane.DEFAULT_LAYER);

		DisableStiffnessRArm.setText("Disable Stiffness");
		DisableStiffnessRArm.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				DisableStiffnessRArmActionPerformed(evt);
			}
		});
		DisableStiffnessRArm.setBounds(560, 170, 140, 22);
		jLayeredPane1.add(DisableStiffnessRArm, javax.swing.JLayeredPane.DEFAULT_LAYER);

		DisableStiffnessRLeg.setText("Disable Stiffness");
		DisableStiffnessRLeg.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				DisableStiffnessRLegActionPerformed(evt);
			}
		});
		DisableStiffnessRLeg.setBounds(550, 340, 150, 22);
		jLayeredPane1.add(DisableStiffnessRLeg, javax.swing.JLayeredPane.DEFAULT_LAYER);

		add(jLayeredPane1, java.awt.BorderLayout.CENTER);
	}// </editor-fold>//GEN-END:initComponents

	private void RShoulderRollMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_RShoulderRollMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotRArm.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("RShoulderRoll:" + RShoulderRoll.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:RShoulderRoll:" + RShoulderRoll.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_RShoulderRollMouseReleased

	private void SendSlidertoRobotRArmActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_SendSlidertoRobotRArmActionPerformed
		// TODO add your handling code here:
	}//GEN-LAST:event_SendSlidertoRobotRArmActionPerformed

	private void doRArmActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_doRArmActionPerformed
		// TODO add your handling code here:
		if (!doRArm.isSelected()) {
			SetEnabled(RArm.split(";"),false);
		} else{
			SetEnabled(RArm.split(";"),true);
			if (CreateNewMove){
				NewNumJoinName++;
				HTJoinName.put("RShoulderPitch",NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("RShoulderRoll", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("RElbowYaw", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("RElbowRoll",NewNumJoinName);
				miNewmov.NumJoin = NewNumJoinName;
			}
		}
	}//GEN-LAST:event_doRArmActionPerformed

	private void RShoulderPitchMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_RShoulderPitchMouseReleased
		// TODO add your handling code here:
		/*Enviamos el valor al robot*/
		if (SendSlidertoRobotRArm.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("RShoulderPitch:" + RShoulderPitch.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:RShoulderPitch:" + RShoulderPitch.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_RShoulderPitchMouseReleased

	private void SaveValueRArmActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_SaveValueRArmActionPerformed
		// TODO add your handling code here:
		float diff = 0;
		if (CreateNewMove){
			//if (!Modify)
			miNewmov.NumMove = NewNMove;

			JoinMov joinmov = new JoinMov();
			joinmov.JoinName = "RShoulderPitch";
			int NumMoveAux = miNewmov.NumMove-1;
			if (NumMoveAux>0) NumMoveAux = NumMoveAux + 1;
			if (isInsert){
				diff = Float.valueOf(txtTimeRShoulderPitch.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;
				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(RShoulderPitch.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeRShoulderPitch.getText());
			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;


			joinmov = new JoinMov();
			joinmov.JoinName = "RShoulderRoll";
			if (isInsert){
				diff = Float.valueOf(txtTimeRShoulderRoll.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;
				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(RShoulderRoll.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeRShoulderRoll.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;

			joinmov = new JoinMov();
			joinmov.JoinName = "RElbowYaw";
			if (isInsert){
				diff = Float.valueOf(txtTimeRElbowYaw.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;
				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(RElbowYaw.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeRElbowYaw.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;

			joinmov = new JoinMov();
			joinmov.JoinName = "RElbowRoll";
			if (isInsert){
				diff = Float.valueOf(txtTimeRElbowRoll.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;
				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(RElbowRoll.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeRElbowRoll.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;
		}
	}//GEN-LAST:event_SaveValueRArmActionPerformed

	private void RElbowYawMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_RElbowYawMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotRArm.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("RElbowYaw:" + RElbowYaw.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:RElbowYaw:" + RElbowYaw.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_RElbowYawMouseReleased

	private void RElbowRollMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_RElbowRollMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotRArm.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("RElbowRoll:" + RElbowRoll.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:RElbowRoll:" + RElbowRoll.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_RElbowRollMouseReleased

	private void GetValueRobotActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_GetValueRobotActionPerformed
		// TODO add your handling code here:
		/*Recoger los valores de cada uno de los Sliders en el Response del Robot
		 */

		//  String _JoinNames = new String("hola");
		//System.out.println(_JoinNames);
		String[] JoinName = JoinNames.split(";");

		//System.out.println("JoinNames: " + JoinName.length);
		SetEnabled(JoinName,false);

		setRobotSliders(JmanagerView.root.getConnection().moviePrx.getAngleRobot("All:"));
		//setRobotSliders(JmanagerView.root.getConnection().send("getAngleRobot:All:"));
	}//GEN-LAST:event_GetValueRobotActionPerformed

	private void HeadPitchMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_HeadPitchMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotHead.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("HeadPitch:" + HeadPitch.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:HeadPitch:" + HeadPitch.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_HeadPitchMouseReleased

	private void SendSlidertoRobotHeadActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_SendSlidertoRobotHeadActionPerformed
		// TODO add your handling code here:
	}//GEN-LAST:event_SendSlidertoRobotHeadActionPerformed

	private void doHeadActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_doHeadActionPerformed
		// TODO add your handling code here:
		if (!doHead.isSelected()) {
			SetEnabled(Head.split(";"),false);
		} else{
			SetEnabled(Head.split(";"),true);
			/*Añanadimos a HT.*/
			if (CreateNewMove){
				NewNumJoinName++;
				HTJoinName.put("HeadYaw",  NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("HeadPitch",  NewNumJoinName);
				miNewmov.NumJoin = NewNumJoinName;
				txtNewNumJoin.setText(String.valueOf( NewNumJoinName+1));
			}

		}
	}//GEN-LAST:event_doHeadActionPerformed

	private void HeadYawMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_HeadYawMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotHead.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("HeadYaw:" + HeadYaw.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:HeadYaw:" + HeadYaw.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_HeadYawMouseReleased

	private void HeadYawKeyReleased(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_HeadYawKeyReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotHead.isSelected()){
			System.out.printf("ESTA SELECCIONADO ENVIO AL ROBOT");
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("HeadYaw:" + HeadYaw.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:HeadYaw:" + HeadYaw.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}else System.out.println("NO ES MARCADO NO SE ENVIA AL ROBOT");
	}//GEN-LAST:event_HeadYawKeyReleased

	private void SaveValueHeadActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_SaveValueHeadActionPerformed
		// TODO add your handling code here:
		float diff=0;
		if (CreateNewMove){
			System.out.println("MiNewMov " + miNewmov.NumMove + " NewMov " + NewNMove + " NPosition " + NPosition);
			//if (!Modify)
			miNewmov.NumMove = NewNMove;
			System.out.println("MiNewMov " + miNewmov.NumMove + " NewMov " + NewNMove + " NPosition " + NPosition);
			JoinMov joinmov = new JoinMov();
			joinmov.JoinName = "HeadYaw";
			int NumMoveAux = miNewmov.NumMove-1;//Estaba sin -1
			if (NumMoveAux>0) NumMoveAux = NumMoveAux + 1;//EStaba en  + 1;

			if (isInsert){
				diff = Float.valueOf(txtTimeHeadYaw.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;
				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
				//System.out.println("Time:"+ time);
				//joinmov.times[i]= String.valueOf(time);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(HeadYaw.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeHeadYaw.getText());
			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;
			System.out.println("JoinList:"+ Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName))) + " NewNMove" + NewNMove);

			joinmov = new JoinMov();
			joinmov.JoinName = "HeadPitch";
			if (isInsert){
				diff = Float.valueOf(txtTimeHeadPitch.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;
				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(HeadPitch.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeHeadPitch.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;
		}


	}//GEN-LAST:event_SaveValueHeadActionPerformed

	private void LShoulderRollMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_LShoulderRollMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotLArm.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("LShoulderRoll:" + LShoulderRoll.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:LShoulderRoll:" + LShoulderRoll.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_LShoulderRollMouseReleased

	private void SendSlidertoRobotLArmActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_SendSlidertoRobotLArmActionPerformed
		// TODO add your handling code here:
	}//GEN-LAST:event_SendSlidertoRobotLArmActionPerformed

	private void doLArmActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_doLArmActionPerformed
		// TODO add your handling code here:
		if (!doLArm.isSelected()) {
			SetEnabled(LArm.split(";"),false);
		} else{
			SetEnabled(LArm.split(";"),true);
			if (CreateNewMove){
				NewNumJoinName++;
				HTJoinName.put("LShoulderPitch", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("LShoulderRoll", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("LElbowYaw", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("LElbowRoll", NewNumJoinName);
				miNewmov.NumJoin = NewNumJoinName;
				txtNewNumJoin.setText(String.valueOf(NewNumJoinName+1));
			}
		}
	}//GEN-LAST:event_doLArmActionPerformed

	private void LShoulderPitchMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_LShoulderPitchMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotLArm.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("LShoulderPitch:" + LShoulderPitch.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:LShoulderPitch:" + LShoulderPitch.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_LShoulderPitchMouseReleased

	private void SaveValueLArmActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_SaveValueLArmActionPerformed
		// TODO add your handling code here:
		float diff = 0;
		if (CreateNewMove){
			//if (!Modify)
			miNewmov.NumMove = NewNMove;

			JoinMov joinmov = new JoinMov();
			joinmov.JoinName = "LShoulderPitch";
			int NumMoveAux = miNewmov.NumMove-1;
			if (NumMoveAux>0) NumMoveAux = NumMoveAux + 1;
			if (isInsert){
				diff = Float.valueOf(txtTimeLShoulderPitch.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);
			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;
				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}

			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(LShoulderPitch.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeLShoulderPitch.getText());
			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;


			joinmov = new JoinMov();
			joinmov.JoinName = "LShoulderRoll";
			if (isInsert){
				diff = Float.valueOf(txtTimeLShoulderRoll.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);
				System.out.println("Diff: " + diff +" " + txtTimeHeadYaw.getText()+ "-"+ Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]));
			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(LShoulderRoll.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeLShoulderRoll.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;

			joinmov = new JoinMov();
			joinmov.JoinName = "LElbowYaw";
			if (isInsert){
				diff = Float.valueOf(txtTimeLElbowYaw.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);
				System.out.println("Diff: " + diff +" " + txtTimeHeadYaw.getText()+ "-"+ Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]));
			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(LElbowYaw.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeLElbowYaw.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;

			joinmov = new JoinMov();
			joinmov.JoinName = "LElbowRoll";
			if (isInsert){
				diff = Float.valueOf(txtTimeLElbowRoll.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);
				System.out.println("Diff: " + diff +" " + txtTimeHeadYaw.getText()+ "-"+ Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]));
			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(LElbowRoll.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeLElbowRoll.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;
		}
	}//GEN-LAST:event_SaveValueLArmActionPerformed

	private void LElbowYawMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_LElbowYawMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotLArm.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("LElbowYaw:" + LElbowYaw.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:LElbowYaw:" + LElbowYaw.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_LElbowYawMouseReleased

	private void LElbowRollMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_LElbowRollMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotLArm.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("LElbowRoll:" + LElbowRoll.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:LElbowRoll:" + LElbowRoll.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_LElbowRollMouseReleased

	private void RHipRollMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_RHipRollMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotRLeg.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("RHipRoll:" + RHipRoll.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:RHipRoll:" + RHipRoll.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_RHipRollMouseReleased

	private void SendSlidertoRobotRLegActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_SendSlidertoRobotRLegActionPerformed
		// TODO add your handling code here:
	}//GEN-LAST:event_SendSlidertoRobotRLegActionPerformed

	private void doRLegActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_doRLegActionPerformed
		// TODO add your handling code here:
		if (!doRLeg.isSelected()) {
			SetEnabled(RLeg.split(";"),false);
		} else{
			SetEnabled(RLeg.split(";"),true);
			if (CreateNewMove){
				NewNumJoinName++;
				HTJoinName.put("RHipYawPitch", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("RHipRoll", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("RHipPitch", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("RKneePitch", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("RAnklePitch", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("RAnkleRoll", NewNumJoinName);
				miNewmov.NumJoin = NewNumJoinName;
				txtNewNumJoin.setText(String.valueOf(NewNumJoinName+1));
			}
		}
	}//GEN-LAST:event_doRLegActionPerformed

	private void RHipYawPitchMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_RHipYawPitchMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotRLeg.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("RHipYawPitch:" + RHipYawPitch.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:RHipYawPitch:" + RHipYawPitch.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_RHipYawPitchMouseReleased

	private void SaveValueRLegActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_SaveValueRLegActionPerformed
		// TODO add your handling code here:
		float diff=0;
		if (CreateNewMove){
			// if (!Modify)
			miNewmov.NumMove = NewNMove;

			JoinMov joinmov = new JoinMov();
			joinmov.JoinName = "RHipYawPitch";
			int NumMoveAux = miNewmov.NumMove-1;
			if (NumMoveAux>0) NumMoveAux = NumMoveAux + 1;
			if (isInsert){
				diff = Float.valueOf(txtTimeRHipYawPitch.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(RHipYawPitch.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeRHipYawPitch.getText());
			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;


			joinmov = new JoinMov();
			joinmov.JoinName = "RHipRoll";
			if (isInsert){
				diff = Float.valueOf(txtTimeRHipRoll.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(RHipRoll.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeRHipRoll.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;

			joinmov = new JoinMov();
			joinmov.JoinName = "RHipPitch";
			if (isInsert){
				diff = Float.valueOf(txtTimeRHipPitch.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(RHipPitch.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeRHipPitch.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;

			joinmov = new JoinMov();
			joinmov.JoinName = "RKneePitch";
			if (isInsert){
				diff = Float.valueOf(txtTimeRKneePitch.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(RKneePitch.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeRKneePitch.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;

			joinmov = new JoinMov();
			joinmov.JoinName = "RAnklePitch";
			if (isInsert){
				diff = Float.valueOf(txtTimeRAnklePitch.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(RAnklePitch.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeRAnklePitch.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;

			joinmov = new JoinMov();
			joinmov.JoinName = "RAnkleRoll";
			if (isInsert){
				diff = Float.valueOf(txtTimeRAnkleRoll.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(RAnkleRoll.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeRAnkleRoll.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;
		}
	}//GEN-LAST:event_SaveValueRLegActionPerformed

	private void RHipPitchMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_RHipPitchMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotRLeg.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("RHipPitch:" + RHipPitch.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:RHipPitch:" + RHipPitch.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_RHipPitchMouseReleased

	private void RKneePitchMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_RKneePitchMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotRLeg.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("RKneePitch:" + RKneePitch.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:RKneePitch:" + RKneePitch.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_RKneePitchMouseReleased

	private void RAnklePitchMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_RAnklePitchMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotRLeg.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("RAnklePitch:" + RAnklePitch.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:RAnklePitch:" + RAnklePitch.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_RAnklePitchMouseReleased

	private void RAnkleRollMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_RAnkleRollMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotRLeg.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("RAnkleRoll:" + RAnkleRoll.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:RAnkleRoll:" + RAnkleRoll.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_RAnkleRollMouseReleased

	private void SendSlidertoRobotLLegActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_SendSlidertoRobotLLegActionPerformed
		// TODO add your handling code here:
	}//GEN-LAST:event_SendSlidertoRobotLLegActionPerformed

	private void doLLegActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_doLLegActionPerformed
		// TODO add your handling code here:
		if (!doLLeg.isSelected()) {
			SetEnabled(LLeg.split(";"),false);
		} else{
			SetEnabled(LLeg.split(";"),true);
			if (CreateNewMove){
				NewNumJoinName++;
				HTJoinName.put("LHipYawPitch", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("LHipRoll", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("LHipPitch", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("LKneePitch", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("LAnklePitch", NewNumJoinName);
				NewNumJoinName++;
				HTJoinName.put("LAnkleRoll", NewNumJoinName);
				miNewmov.NumJoin = NewNumJoinName;
				txtNewNumJoin.setText(String.valueOf(NewNumJoinName+1));
			}
		}
	}//GEN-LAST:event_doLLegActionPerformed

	private void LHipYawPitchMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_LHipYawPitchMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotLLeg.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("LHipYawPitch:" + LHipYawPitch.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:LHipYawPitch:" + LHipYawPitch.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_LHipYawPitchMouseReleased

	private void SaveValueLLegActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_SaveValueLLegActionPerformed
		// TODO add your handling code here:

		float diff = 0;
		if (CreateNewMove){
			//if (!Modify)
			miNewmov.NumMove = NewNMove;

			JoinMov joinmov = new JoinMov();
			joinmov.JoinName = "LHipYawPitch";
			int NumMoveAux = miNewmov.NumMove-1;
			if (NumMoveAux>0) NumMoveAux = NumMoveAux + 1;
			if (isInsert){
				diff = Float.valueOf(txtTimeLHipYawPitch.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(LHipYawPitch.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeLHipYawPitch.getText());
			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;


			joinmov = new JoinMov();
			joinmov.JoinName = "LHipRoll";
			if (isInsert){
				diff = Float.valueOf(txtTimeLHipRoll.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(LHipRoll.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeLHipRoll.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;

			joinmov = new JoinMov();
			joinmov.JoinName = "LHipPitch";
			if (isInsert){
				diff = Float.valueOf(txtTimeLHipPitch.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(LHipPitch.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeLHipPitch.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;

			joinmov = new JoinMov();
			joinmov.JoinName = "LKneePitch";
			if (isInsert){
				diff = Float.valueOf(txtTimeLKneePitch.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(LKneePitch.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeLKneePitch.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;

			joinmov = new JoinMov();
			joinmov.JoinName = "LAnklePitch";
			if (isInsert){
				diff = Float.valueOf(txtTimeLAnklePitch.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(LAnklePitch.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeLAnklePitch.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;

			joinmov = new JoinMov();
			joinmov.JoinName = "LAnkleRoll";
			if (isInsert){
				diff = Float.valueOf(txtTimeLAnkleRoll.getText()) - Float.valueOf(miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[NPosition-1]);

			}

			for (int i=0;i<NumMoveAux;i++){
				float diffAux=0;

				joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
				if (i>NPosition) diffAux = diff;
				String miTime = miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
				if (isInsert)   miTime = String.valueOf(Float.valueOf(miTime) + diffAux);
				joinmov.times[i]= String.valueOf(miTime);
			}
			joinmov.movs[NPosition] = String.valueOf(Float.valueOf(LAnkleRoll.getValue())/100);
			joinmov.times[NPosition] = String.valueOf(txtTimeLAnkleRoll.getText());

			miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;
		}

	}//GEN-LAST:event_SaveValueLLegActionPerformed

	private void LHipPitchMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_LHipPitchMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotLLeg.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("LHipPitch:" + LHipPitch.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:LHipPitch:" + LHipPitch.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_LHipPitchMouseReleased

	private void LKneePitchMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_LKneePitchMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotLLeg.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("LKneePitch:" + LKneePitch.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:LKneePitch:" + LKneePitch.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_LKneePitchMouseReleased

	private void LAnklePitchMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_LAnklePitchMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotLLeg.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("LAnklePitch:" + LAnklePitch.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:LAnklePitch:" + LAnklePitch.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_LAnklePitchMouseReleased

	private void LAnkleRollMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_LAnkleRollMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotLLeg.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("LAnkleRoll:" + LAnkleRoll.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:LAnkleRoll:" + LAnkleRoll.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_LAnkleRollMouseReleased

	private void LHipRollMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_LHipRollMouseReleased
		// TODO add your handling code here:
		if (SendSlidertoRobotLLeg.isSelected()){
			JmanagerView.root.getConnection().moviePrx.setAngleRobot("LHipRoll:" + LHipRoll.getValue() + ":");
			//dataResp resp = JmanagerView.root.getConnection().send("setAngleRobot:LHipRoll:" + LHipRoll.getValue() + ":");
			//try {
			//	Thread.sleep(500);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(CreateMove.class.getName()).log(Level.SEVERE, null, ex);
			//}
		}
	}//GEN-LAST:event_LHipRollMouseReleased

	private void LoadMovActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_LoadMovActionPerformed
		// TODO add your handling code here:
		ModifyMovement.setEnabled(true);
		mimov = new Movements();
		CreateNewMove = false;
		Modify = false;
		File archivo = null;
		FileReader fr = null;
		BufferedReader br = null;

		final JFileChooser fc;
		if (lastdir != null) {
			fc = new JFileChooser(lastdir);
		} else {
			fc = new JFileChooser();
		}


		// fc.addChoosableFileFilter(filter);


		int returnVal = fc.showOpenDialog(CreateMove.this);

		if (returnVal == JFileChooser.APPROVE_OPTION) {
			File file =fc.getSelectedFile();
			lastdir = fc.getCurrentDirectory();



			try{
				// Apertura del fichero y creacion de BufferedReader para poder
				// hacer una lectura comoda (disponer del metodo readLine()).
				archivo = new File(file.toString());
				fr = new FileReader(archivo);
				br = new BufferedReader(fr);
				txtFileMov.setColumns(file.toString().length());
				txtFileMov.setText(file.toString());


				// Lectura del fichero

				String linea = br.readLine();
				String levels[] = linea.split(" ");
				mimov.Name = levels[0];
				mimov.NumMove = Integer.parseInt(levels[1]);
				txtJoinName.setText(mimov.Name);
				txtNumberMove.setText(String.valueOf(mimov.NumMove));


				linea = br.readLine();
				levels = linea.split(" ");
				mimov.NumJoin = levels.length-1;
				txtNumJoin.setText(String.valueOf(mimov.NumJoin+1));


				String times[];
				String movs[];
				HTJoinName = new Hashtable();
				for (int i=0;i<mimov.NumJoin+1;i++){
					JoinMov joinmov = new JoinMov();
					joinmov.JoinName = levels[i];
					HTJoinName.put(joinmov.JoinName,i);
					mimov.JoinList[i]=joinmov;
					/*Vamos leyendo los movimientos y los vamos guardando con su JoinName*/
					linea = br.readLine();
					//joinmov.movs = linea.split(" ");
					movs = linea.split(" ");
					for (int j=0;j<movs.length;j++)
						mimov.JoinList[i].movs[j] = movs[j];

				}

				for (int i=0;i<mimov.NumJoin+1;i++){
					/*Vamos leyendo los movimientos y los vamos guardando con su JoinName*/

					linea = br.readLine();
					times = linea.split(" ");
					for (int j=0;j<times.length;j++)
						mimov.JoinList[i].times[j] = times[j];
					//System.out.println("Times:" + times[j]);
					//mimov.JoinList[i].times =times;

				}

				/*Mostramos los datos y actualizamos los Slider*/
				/*   System.out.println("Nombre:" + mimov.Name + ";Numero pasos:"+mimov.NumMove);
         for (int i=0;i<mimov.NumJoin;i++){
             System.out.println("JoinName:" + mimov.JoinList[i].JoinName);
             for (int j=0;j<mimov.NumMove;j++){
                System.out.println("---->Mov["+j+"]:"+ mimov.JoinList[i].movs[j]);
             }
             for (int j=0;j<mimov.NumMove;j++){
                System.out.println("---->Times["+j+"]:"+ mimov.JoinList[i].times[j]);
             }
         }*/

				btnSig.setEnabled(true);
				TestInRobot.setEnabled(true);
				PlayMovement.setEnabled(true);


				SetEnabled(JoinNames.split(";"),false);
				SetEnabled(levels,true);
				System.out.println("NumMoves" + mimov.NumMove);

				/*while((linea=br.readLine())!=null)
                    System.out.println(linea);*/
			} catch(Exception e){

				txtFileMov.setText("")        ;;
				JOptionPane.showMessageDialog(this,"Check the file format.", "Format Error", JOptionPane.ERROR_MESSAGE);

				//e.printStackTrace();
			}
		} else {
			System.out.println("Open command cancelled by user.");
		}
	}//GEN-LAST:event_LoadMovActionPerformed

	private void btnAntActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnAntActionPerformed
		// TODO add your handling code here:
		NMove--;
		txtMovPosition.setText(String.valueOf(NMove));
		System.out.println("NMOVE:"+NMove);
		if (NMove==0)
			btnAnt.setEnabled(false);
		else{

			btnSig.setEnabled(true);

		}
		for (int i=0;i<mimov.NumJoin+1;i++){
			System.out.println("JoinName:" + mimov.JoinList[i].JoinName);
			System.out.println("---->Mov["+NMove+"]:"+ mimov.JoinList[i].movs[NMove]);
			System.out.println("---->Times["+NMove+"]:"+ mimov.JoinList[i].times[NMove]);
			SetSlidersbyMov(mimov.JoinList[i].JoinName,Float.parseFloat(mimov.JoinList[i].movs[NMove]),Float.parseFloat(mimov.JoinList[i].times[NMove]));

		}
		/*Cargar los Sliders*/
	}//GEN-LAST:event_btnAntActionPerformed

	private void btnSigActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSigActionPerformed
		// TODO add your handling code here:

		NMove++;
		System.out.println("NMOVE:"+NMove);
		txtMovPosition.setText(String.valueOf(NMove));
		if (NMove==mimov.NumMove-1)
			btnSig.setEnabled(false);

		else{

			btnAnt.setEnabled(true);


		}
		for (int i=0;i<mimov.NumJoin+1;i++){
			System.out.println("JoinName:" + mimov.JoinList[i].JoinName);
			System.out.println("---->Mov["+NMove+"]:"+ mimov.JoinList[i].movs[NMove]);
			System.out.println("---->Times["+NMove+"]:"+ mimov.JoinList[i].times[NMove]);
			SetSlidersbyMov(mimov.JoinList[i].JoinName,Float.parseFloat(mimov.JoinList[i].movs[NMove]),Float.parseFloat(mimov.JoinList[i].times[NMove]));
		}
		/*Cargar los Sliders*/
	}//GEN-LAST:event_btnSigActionPerformed

	private void TestInRobotActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_TestInRobotActionPerformed
		// TODO add your handling code here:
		SendMovementRobot(mimov,NMove,NMove+1);
	}//GEN-LAST:event_TestInRobotActionPerformed

	private void PlayMovementActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_PlayMovementActionPerformed
		// TODO add your handling code here:
		SendMovementRobot(mimov,0,mimov.NumMove);
	}//GEN-LAST:event_PlayMovementActionPerformed

	private void NewMovActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_NewMovActionPerformed
		// TODO add your handling code here:
		ModifyMovement.setEnabled(false);
		miNewmov = new Movements();
		CreateNewMove = true;
		Modify = false;
		HTJoinName = new Hashtable();
		txtNewFileMov.setText("");
		txtNewJoinName.setText("");
		txtNewMovPosition.setText("");
		txtNewNumJoin.setText("");
		txtNewNumberMove.setText("");
		NewNMove=1;
		NPosition=0;
		NewNumJoinName=-1;

		final JFileChooser fc;
		if (lastdir != null) {
			fc = new JFileChooser(lastdir);
		} else {
			fc = new JFileChooser();
		}


		int returnVal = fc.showSaveDialog(CreateMove.this);

		if (returnVal == JFileChooser.APPROVE_OPTION) {
			File file =fc.getSelectedFile();
			lastdir = fc.getCurrentDirectory();
			txtNewFileMov.setText(file.toString());

			miNewmov.NumMove = NewNMove;
			txtNewNumberMove.setText(String.valueOf(miNewmov.NumMove));

			/*Activación de los botones implicados*/
			SaveValueHead.setEnabled(true);
			SaveValueLArm.setEnabled(true);
			SaveValueRArm.setEnabled(true);
			SaveValueLLeg.setEnabled(true);
			SaveValueRLeg.setEnabled(true);
			FinalizeMovement.setEnabled(true);
			NewTestInRobot.setEnabled(true);
			NewPlayMovement.setEnabled(true);
			btnAdd.setEnabled(true);
			SaveMov.setEnabled(true);
			btnNewNext.setEnabled(true);
			doHead.setSelected(false);
			doLArm.setSelected(false);
			doRArm.setSelected(false);
			doLLeg.setSelected(false);
			doRLeg.setSelected(false);
		}
		else {
			System.out.println("Open command cancelled by user.");
		}
	}//GEN-LAST:event_NewMovActionPerformed

	private void btnNewAntActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnNewAntActionPerformed
		// TODO add your handling code here:
		NPosition--;
		if (NPosition==0) btnNewAnt.setEnabled(false);

		//txtNewNumberMove.setText(String.valueOf(NewNMove));
		txtNewMovPosition.setText(String.valueOf(NPosition));

		for (int i=0;i<miNewmov.NumJoin+1;i++){
			SetSlidersbyMov(miNewmov.JoinList[i].JoinName,Float.parseFloat(miNewmov.JoinList[i].movs[NPosition]),Float.parseFloat(miNewmov.JoinList[i].times[NPosition]));
		}


	}//GEN-LAST:event_btnNewAntActionPerformed

	private void btnAddActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnAddActionPerformed
		// TODO add your handling code here:
		/*Control para no aumentar NewNMove sino se ha salvado la posicion
         Se deberian controlar todos los JoinList.*/


		if ( !isJoinSave()){ 
			JOptionPane.showMessageDialog(this,"Save all join.", "Error", JOptionPane.ERROR_MESSAGE);
		}else
		{
			NewNMove++;
			NPosition = NewNMove-1;
			if (NPosition>0) btnNewAnt.setEnabled(true);
			txtNewNumberMove.setText(String.valueOf(NewNMove));
			txtNewMovPosition.setText(String.valueOf(NPosition));

			/*      if (miNewmov.JoinList[0].movs[NewNMove]!=null){
              for (int i=0;i<miNewmov.NumJoin+1;i++){
                    SetSlidersbyMov(miNewmov.JoinList[i].JoinName,Float.parseFloat(miNewmov.JoinList[i].movs[NewNMove]),Float.parseFloat(miNewmov.JoinList[i].times[NewNMove]));
                }
            }*/
		}
		/*     }else System.out.println("Rellena el Valor");*/
		/*Guardar los valores en miNewmov*/

	}//GEN-LAST:event_btnAddActionPerformed

	private void NewTestInRobotActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_NewTestInRobotActionPerformed
		// TODO add your handling code here:
		System.out.println("Empezando el Test");
		SendMovementRobot(miNewmov,NewNMove-1,NewNMove);
		System.out.println("Empezando el Test");

	}//GEN-LAST:event_NewTestInRobotActionPerformed

	private void NewPlayMovementActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_NewPlayMovementActionPerformed
		// TODO add your handling code here:
		System.out.println("Empezando el Play");
		SendMovementRobot(miNewmov,0,miNewmov.NumMove);
		System.out.println("Finalizando el Play");
	}//GEN-LAST:event_NewPlayMovementActionPerformed





	private void FinalizeMovementActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_FinalizeMovementActionPerformed
		// TODO add your handling code here:
		/*Grabar movimiento que este en los slider
     Grabar movimiento del robot estable + 3 seg
     Guardar en el fichero todos los datos.*/
		if ( !isJoinSave()){ 
			JOptionPane.showMessageDialog(this,"Save all join.", "Error", JOptionPane.ERROR_MESSAGE);
		}else
		{
			System.out.print("NumJoin:"+miNewmov.NumJoin);
			for (int i=0;i<miNewmov.NumJoin+1;i++){
				for (int j=0;j<miNewmov.NumMove;j++)//Estaba en +1
				{
					System.out.println("Nombre:" + miNewmov.JoinList[i].JoinName + " Mov["+j+"]:"+miNewmov.JoinList[i].movs[j] + " Time["+j+"]:"+miNewmov.JoinList[i].times[j]);
				}
			}
			NewNMove++;
			miNewmov.NumMove = NewNMove;
			NPosition = NewNMove - 1; //Esto no etaba
			if (NewNMove>0) btnNewAnt.setEnabled(true);
			txtNewNumberMove.setText(String.valueOf(NewNMove));
			txtNewMovPosition.setText(String.valueOf(NPosition));
			/*Guardar una posicion estable.
			 *HeadYaw = 0     *  HeadPitch = 0
			 *LShoulderPitch = 209     *  LShoulderRoll = 34     *  LElbowYaw = -139     *  LElbowRoll = -139
			 *LHipYawPitch = 0     * LHipRoll = 0     * LHipPitch = -34     * LKneePitch = 69     * LAnklePitch = -34     * LAnkleRoll = 52
			 *RShoulderPitch = 209     * RShoulderRoll = -34     * RElbowYaw = 139     * RElbowRoll = 139
			 *RHipYawPitch = 0     * RHipRoll = 0     * RHipPitch = -34     * RKneePitch = 69     * RAnklePitch =-34     * RAnkleRoll = 0
			 * */
			for (int i=0;i<miNewmov.NumJoin+1;i++)
				SetPositionStable(miNewmov.JoinList[i].JoinName);


			/*Guardar en fichero los datos.*/
			BufferedWriter bufferedWriter = null;

			try {
				bufferedWriter = new BufferedWriter(new FileWriter(txtNewFileMov.getText()));

				/*Primera Linea, Nombre y Número de Pasos*/
				miNewmov.Name = txtNewJoinName.getText();
				int NumMoveAux = miNewmov.NumMove;
				Float time = Float.valueOf(miNewmov.JoinList[0].times[NumMoveAux-1]) + 2;
				String Line = miNewmov.Name + " " + NumMoveAux + " "+time;// txtNewNumberMove.getText();

				//String Line = miNewmov.Name + " " + txtNewNumberMove.getText();
				bufferedWriter.write(Line);
				bufferedWriter.newLine();

				/*Nombre de los JoinName*/
				Line = "";
				System.out.print("-----NumMove"+ miNewmov.NumMove);
				for (int i=0;i<miNewmov.NumJoin+1;i++)
					Line=Line + miNewmov.JoinList[i].JoinName + " ";
				bufferedWriter.write(Line);
				bufferedWriter.newLine();


				for (int i=0;i<miNewmov.NumJoin+1;i++){
					Line="";
					for (int j=0;j<miNewmov.NumMove;j++)//Estaba con +1
						Line= Line + String.valueOf((Float.valueOf(miNewmov.JoinList[i].movs[j]))) + " ";
					bufferedWriter.write(Line);
					bufferedWriter.newLine();

				}

				for (int i=0;i<miNewmov.NumJoin+1;i++){
					Line="";
					for (int j=0;j<miNewmov.NumMove;j++) //Estaba con +1
						Line= Line + miNewmov.JoinList[i].times[j] + " ";
					bufferedWriter.write(Line);
					bufferedWriter.newLine();

				}


			} catch (FileNotFoundException ex) {
				ex.printStackTrace();
			} catch (IOException ex) {
				ex.printStackTrace();
			} finally {
				//Close the BufferedWriter
				try {
					if (bufferedWriter != null) {
						bufferedWriter.flush();
						bufferedWriter.close();
					}
				} catch (IOException ex) {
					ex.printStackTrace();
				}
			}

		}

	}//GEN-LAST:event_FinalizeMovementActionPerformed

	private void InitPositionRobotActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_InitPositionRobotActionPerformed
		// TODO add your handling code here:
		miNewmov = new Movements();
		miNewmov.NumMove=1;
		miNewmov.NumJoin = 0;
		HTJoinName = new Hashtable();
		String[] JoinName = JoinNames.split(";");
		miNewmov.NumJoin = JoinName.length-1;

		for (int i=0;i<JoinName.length;i++){
			System.out.println("Nombre:" +  JoinName[i]);
			HTJoinName.put(JoinName[i],i);
			SetPositionStable(JoinName[i]);
			SetSlidersbyMov(JoinName[i], Float.valueOf(miNewmov.JoinList[i].movs[0]),Float.valueOf("1.5"));
		}

		SendMovementRobot(miNewmov, 0, 1);

	}//GEN-LAST:event_InitPositionRobotActionPerformed

	private void ModifyMovementActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_ModifyMovementActionPerformed
		// TODO add your handling code here:
		miNewmov = new Movements();
		System.out.println("miNewmov.Length:" + miNewmov.JoinList.length);

		miNewmov = mimov;

		System.out.println("miNewmov.Length des:" + miNewmov.JoinList.length);


		System.out.println("miNewmov.Length New:" + miNewmov.JoinList[0].movs.length);
		CreateNewMove = true;
		Modify = true;
		System.out.println("Modify:" + miNewmov.JoinList[0].JoinName + " NumMove " + miNewmov.NumMove);
		//miNewmov.NumMove = miNewmov.NumMove - 1; //Estaba Antes
		NewNMove = miNewmov.NumMove;
		NPosition = 0;

		txtNewFileMov.setText(txtFileMov.getText());
		txtNewJoinName.setText(txtJoinName.getText());
		txtNewNumJoin.setText(txtNumJoin.getText());
		txtNewNumberMove.setText(txtNumberMove.getText());
		FinalizeMovement.setEnabled(true);
		NewTestInRobot.setEnabled(true);
		NewPlayMovement.setEnabled(true);
		SaveMov.setEnabled(true);
		btnAdd.setEnabled(true);
		btnNewNext.setEnabled(true);
		SaveValueHead.setEnabled(true);
		SaveValueRArm.setEnabled(true);
		SaveValueLArm.setEnabled(true);
		SaveValueRLeg.setEnabled(true);
		SaveValueLLeg.setEnabled(true);


		/*Ocultar la parte de Load.*/
	}//GEN-LAST:event_ModifyMovementActionPerformed

	private void SaveMovActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_SaveMovActionPerformed
		// TODO add your handling code here:
		if ( !isJoinSave()){ 
			JOptionPane.showMessageDialog(this,"Save all join.", "Error", JOptionPane.ERROR_MESSAGE);
		}else
		{
			/*Guardar en fichero los datos.*/
			BufferedWriter bufferedWriter = null;

			try {
				bufferedWriter = new BufferedWriter(new FileWriter(txtNewFileMov.getText()));

				/*Primera Linea, Nombre y Número de Pasos*/
				miNewmov.NumMove = NewNMove;

				int NumMoveAux = miNewmov.NumMove; //+1;



				miNewmov.Name = txtNewJoinName.getText();

				float time = Float.valueOf(miNewmov.JoinList[0].times[NumMoveAux-1]) + 2;

				String Line = miNewmov.Name + " " + NumMoveAux + " "+time;// txtNewNumberMove.getText();
				bufferedWriter.write(Line);
				bufferedWriter.newLine();

				/*Nombre de los JoinName*/
				Line = "";
				System.out.print("-----NumMove"+ miNewmov.NumMove);
				for (int i=0;i<miNewmov.NumJoin+1;i++)
					Line=Line + miNewmov.JoinList[i].JoinName + " ";
				bufferedWriter.write(Line);
				bufferedWriter.newLine();


				// if (Modify) NumMoveAux = NumMoveAux +1;
				System.out.print("NumMoveAux : " + NumMoveAux);
				for (int i=0;i<miNewmov.NumJoin+1;i++){
					Line="";
					//     for (int j=0;j<miNewmov.NumMove+1;j++)
					for (int j=0;j<NumMoveAux;j++){
						System.out.print("NumMoveAux : " + NumMoveAux + " j: " + j );
						Line= Line + String.valueOf((Float.valueOf(miNewmov.JoinList[i].movs[j]))) + " ";
					}
					bufferedWriter.write(Line);
					bufferedWriter.newLine();

				}

				for (int i=0;i<miNewmov.NumJoin+1;i++){
					Line="";
					//  for (int j=0;j<miNewmov.NumMove+1;j++)
					for (int j=0;j<NumMoveAux;j++)
						Line= Line + miNewmov.JoinList[i].times[j] + " ";
					bufferedWriter.write(Line);
					bufferedWriter.newLine();

				}


			} catch (FileNotFoundException ex) {
				ex.printStackTrace();
			} catch (IOException ex) {
				ex.printStackTrace();
			} finally {
				//Close the BufferedWriter
				try {
					if (bufferedWriter != null) {
						bufferedWriter.flush();
						bufferedWriter.close();
					}
				} catch (IOException ex) {
					ex.printStackTrace();
				}
			}

		}
	}//GEN-LAST:event_SaveMovActionPerformed

	private void btnNewNextActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnNewNextActionPerformed
		// TODO add your handling code here:
		isInsert= false;
		NPosition++;
		if ( NPosition>=NewNMove){ 
			JOptionPane.showMessageDialog(this,"This position isn't saved.", "Error", JOptionPane.ERROR_MESSAGE);
			NPosition--;
		}else
		{

			System.out.println("NPosition" + NPosition + " New "+ NewNMove);         
			if (NPosition>0) btnNewAnt.setEnabled(true);
			//   txtNewNumberMove.setText(String.valueOf(NewNMove));
			txtNewMovPosition.setText(String.valueOf(NPosition));

			if (miNewmov.JoinList[0].movs[NPosition]!=null){
				for (int i=0;i<miNewmov.NumJoin+1;i++){
					SetSlidersbyMov(miNewmov.JoinList[i].JoinName,Float.parseFloat(miNewmov.JoinList[i].movs[NPosition]),Float.parseFloat(miNewmov.JoinList[i].times[NPosition]));
				}
			}

		}
	}//GEN-LAST:event_btnNewNextActionPerformed

	private void btnInsertActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnInsertActionPerformed
		// TODO add your handling code here:


		/*  if ( !isJoinSave()){
              JOptionPane.showMessageDialog(this,"Save all join.", "Error", JOptionPane.ERROR_MESSAGE);
        }else
        {
		 *
		 *
		 */
		/*Inserta en la siguiente posición y mueve en +1 a todos los que estan despues.
            Se suma la diferencia de tiempo del siguiente con el de ahora.
		 */
		System.out.println("******Posicion: " + NPosition + " Total:" + (NewNMove+1) + " Inserto en " + (NPosition +1) + " miNewMov.NumMove: " + miNewmov.NumMove);

		NPosition++;
		NewNMove++;
		isInsert = true;
		int PosUlt = miNewmov.NumMove-1; //tenia -1
		//   if (Modify){ PosUlt--;
		//                 miNewmov.NumMove++;
		//  }
		txtNewNumberMove.setText(String.valueOf(NewNMove));
		txtNewMovPosition.setText(String.valueOf(NPosition));

		/*Copiamos en un Joinmov nuevo el valor de la última posición.*/
		/*     JoinMov joinmov = new JoinMov();
           for (int i=0;i<miNewmov.NumJoin+1;i++){
            joinmov.JoinName = miNewmov.JoinList[i].JoinName;
            int NumMoveAux = miNewmov.NumMove-1;//Estaba sin -1
            if (NumMoveAux>0) NumMoveAux = NumMoveAux + 1;//EStaba en  + 1;

            }
            for (int i=0;i<NumMoveAux;i++){
                  joinmov.movs[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].movs[i];
                  joinmov.times[i]= miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))].times[i];
            }
            joinmov.movs[NPosition] = String.valueOf(Float.valueOf(HeadYaw.getValue())/100);
            joinmov.times[NPosition] = String.valueOf(txtTimeHeadYaw.getText());
            miNewmov.JoinList[Integer.parseInt(String.valueOf(HTJoinName.get(joinmov.JoinName)))] = joinmov;
      hol

           /*Movemos todos.*/
		for (int i=0;i<miNewmov.NumJoin+1;i++){
			System.out.println("*********Nombre" + miNewmov.JoinList[i].JoinName + " NUmMOv:" + PosUlt + " NPosition:" + NPosition);
			for (int j=PosUlt;j>=NPosition;j--){

				System.out.println("J: " + j + " J+1: " + (j+1));
				miNewmov.JoinList[i].movs[j+1]= miNewmov.JoinList[i].movs[j];
				System.out.println("********Ultimo:" + j + " Mov:" + miNewmov.JoinList[i].times[j]);
				miNewmov.JoinList[i].times[j+1]= miNewmov.JoinList[i].times[j];
			}
		}


		/*     if (NPosition>0) btnNewAnt.setEnabled(true);
            txtNewNumberMove.setText(String.valueOf(NewNMove));
            txtNewMovPosition.setText(String.valueOf(NewNMove));

        }*/

	}//GEN-LAST:event_btnInsertActionPerformed

	private void btnDeleteActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnDeleteActionPerformed
		// TODO add your handling code here:



		NewNMove--;
		System.out.println("******Posicion: " + NPosition + " Total:" + (NewNMove) + " Elimino en " + (NPosition ));
		/* if (!Modify) txtNewNumberMove.setText(String.valueOf(NewNMove+1));
           else */

		if (NPosition == NewNMove){
			NPosition--;
		}
		else{
			/*Movemos todos.*/
			for (int i=0;i<miNewmov.NumJoin+1;i++){
				System.out.println("*********Nombre" + miNewmov.JoinList[i].JoinName + " NUmMOv:" + miNewmov.NumMove + " NPosition:" + NPosition);
				float diff;
				if (NPosition==0) diff = Float.valueOf(miNewmov.JoinList[i].times[NPosition]);
				else diff = Float.valueOf(miNewmov.JoinList[i].times[NPosition]) - Float.valueOf(miNewmov.JoinList[i].times[NPosition-1]);
				for (int j=NPosition;j<miNewmov.NumMove-1;j++){
					miNewmov.JoinList[i].movs[j]= miNewmov.JoinList[i].movs[j+1];
					miNewmov.JoinList[i].times[j]= String.valueOf(Float.valueOf(miNewmov.JoinList[i].times[j+1])-diff);
				}
			}
		}

		txtNewNumberMove.setText(String.valueOf(NewNMove));
		txtNewMovPosition.setText(String.valueOf(NPosition));

		if (miNewmov.JoinList[0].movs[NPosition]!=null){
			for (int i=0;i<miNewmov.NumJoin+1;i++){
				SetSlidersbyMov(miNewmov.JoinList[i].JoinName,Float.parseFloat(miNewmov.JoinList[i].movs[NPosition]),Float.parseFloat(miNewmov.JoinList[i].times[NPosition]));
			}
		}

	}//GEN-LAST:event_btnDeleteActionPerformed

	private void getAngleHeadActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_getAngleHeadActionPerformed
		// TODO add your handling code here:

		setRobotSlidersbyJoin(JmanagerView.root.getConnection().moviePrx.getAngleRobot("Head:"),"Head");
		//setRobotSlidersbyJoin(JmanagerView.root.getConnection().send("getAngleRobot:Head:"),"Head");

	}//GEN-LAST:event_getAngleHeadActionPerformed

	private void getAngleLArmActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_getAngleLArmActionPerformed
		// TODO add your handling code here:
		setRobotSlidersbyJoin(JmanagerView.root.getConnection().moviePrx.getAngleRobot("LArm:"),"LArm");
		//setRobotSlidersbyJoin(JmanagerView.root.getConnection().send("getAngleRobot:LArm:"),"LArm");
	}//GEN-LAST:event_getAngleLArmActionPerformed

	private void getAngleLLegActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_getAngleLLegActionPerformed
		// TODO add your handling code here:
		setRobotSlidersbyJoin(JmanagerView.root.getConnection().moviePrx.getAngleRobot("LLeg:"),"LLeg");
		//setRobotSlidersbyJoin(JmanagerView.root.getConnection().send("getAngleRobot:LLeg:"),"LLeg");
	}//GEN-LAST:event_getAngleLLegActionPerformed

	private void getAngleRArmActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_getAngleRArmActionPerformed
		// TODO add your handling code here:
		setRobotSlidersbyJoin(JmanagerView.root.getConnection().moviePrx.getAngleRobot("RArm:"),"RArm");
		//setRobotSlidersbyJoin(JmanagerView.root.getConnection().send("getAngleRobot:RArm:"),"RArm");
	}//GEN-LAST:event_getAngleRArmActionPerformed

	private void getAngleRLegActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_getAngleRLegActionPerformed
		// TODO add your handling code here:
		setRobotSlidersbyJoin(JmanagerView.root.getConnection().moviePrx.getAngleRobot("RLeg:"),"RLeg");
		//setRobotSlidersbyJoin(JmanagerView.root.getConnection().send("getAngleRobot:RLeg:"),"RLeg");
	}//GEN-LAST:event_getAngleRLegActionPerformed

	private void DisableStiffnessHeadActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_DisableStiffnessHeadActionPerformed
		// TODO add your handling code here:
		dataResp resp;
		String miJoin="";
		String[] JoinName = Head.split(";");
		for (int i=0;i<JoinName.length;i++)
			miJoin= miJoin + JoinName[i] + ":";

		if (!DisableStiffnessHead.isSelected()){//Activar Stiffness
			JmanagerView.root.getConnection().moviePrx.disableStiffness("On:" + miJoin);
			//resp = JmanagerView.root.getConnection().send("disableStiffness:On:" + miJoin);
		}else{//Desactivar Stiffness
			JmanagerView.root.getConnection().moviePrx.disableStiffness("Off:" + miJoin);
			//resp = JmanagerView.root.getConnection().send("disableStiffness:Off:" + miJoin);
		}

	}//GEN-LAST:event_DisableStiffnessHeadActionPerformed

	private void DisableStiffnessLLegActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_DisableStiffnessLLegActionPerformed
		// TODO add your handling code here:
		dataResp resp;
		String miJoin="";
		String[] JoinName = LLeg.split(";");
		for (int i=0;i<JoinName.length;i++)
			miJoin= miJoin + JoinName[i] + ":";

		if (!DisableStiffnessLLeg.isSelected()){//Activar Stiffness
			JmanagerView.root.getConnection().moviePrx.disableStiffness("On:" + miJoin);
			//resp = JmanagerView.root.getConnection().send("disableStiffness:On:" + miJoin);
		}else{//Desactivar Stiffness
			JmanagerView.root.getConnection().moviePrx.disableStiffness("Off:" + miJoin);
			//resp = JmanagerView.root.getConnection().send("disableStiffness:Off:" + miJoin);
		}

	}//GEN-LAST:event_DisableStiffnessLLegActionPerformed

	private void DisableStiffnessLArmActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_DisableStiffnessLArmActionPerformed
		// TODO add your handling code here:
		dataResp resp;
		String miJoin="";
		String[] JoinName = LArm.split(";");
		for (int i=0;i<JoinName.length;i++)
			miJoin= miJoin + JoinName[i] + ":";

		if (!DisableStiffnessLArm.isSelected()){//Activar Stiffness
			JmanagerView.root.getConnection().moviePrx.disableStiffness("On:" + miJoin);
			//resp = JmanagerView.root.getConnection().send("disableStiffness:On:" + miJoin);
		}else{//Desactivar Stiffness
			JmanagerView.root.getConnection().moviePrx.disableStiffness("Off:" + miJoin);
			//resp = JmanagerView.root.getConnection().send("disableStiffness:Off:" + miJoin);
		}
	}//GEN-LAST:event_DisableStiffnessLArmActionPerformed

	private void DisableStiffnessRArmActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_DisableStiffnessRArmActionPerformed
		// TODO add your handling code here:
		dataResp resp;
		String miJoin="";
		String[] JoinName = RArm.split(";");
		for (int i=0;i<JoinName.length;i++)
			miJoin= miJoin + JoinName[i] + ":";

		if (!DisableStiffnessRArm.isSelected()){//Activar Stiffness
			JmanagerView.root.getConnection().moviePrx.disableStiffness("On:" + miJoin);
			//resp = JmanagerView.root.getConnection().send("disableStiffness:On:" + miJoin);
		}else{//Desactivar Stiffness
			JmanagerView.root.getConnection().moviePrx.disableStiffness("Off:" + miJoin);
			//resp = JmanagerView.root.getConnection().send("disableStiffness:Off:" + miJoin);
		}
	}//GEN-LAST:event_DisableStiffnessRArmActionPerformed

	private void DisableStiffnessRLegActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_DisableStiffnessRLegActionPerformed
		// TODO add your handling code here:
		dataResp resp;
		String miJoin="";
		String[] JoinName = RLeg.split(";");
		for (int i=0;i<JoinName.length;i++)
			miJoin= miJoin + JoinName[i] + ":";

		if (!DisableStiffnessRLeg.isSelected()){//Activar Stiffness
			JmanagerView.root.getConnection().moviePrx.disableStiffness("On:" + miJoin);
			//resp = JmanagerView.root.getConnection().send("disableStiffness:On:" + miJoin);
		}else{//Desactivar Stiffness
			JmanagerView.root.getConnection().moviePrx.disableStiffness("Off:" + miJoin);
			//resp = JmanagerView.root.getConnection().send("disableStiffness:Off:" + miJoin);
		}
	}//GEN-LAST:event_DisableStiffnessRLegActionPerformed


	// Variables declaration - do not modify//GEN-BEGIN:variables
	private javax.swing.JCheckBox DisableStiffnessHead;
	private javax.swing.JCheckBox DisableStiffnessLArm;
	private javax.swing.JCheckBox DisableStiffnessLLeg;
	private javax.swing.JCheckBox DisableStiffnessRArm;
	private javax.swing.JCheckBox DisableStiffnessRLeg;
	private javax.swing.JButton FinalizeMovement;
	private javax.swing.JButton GetValueRobot;
	private javax.swing.JSlider HeadPitch;
	private javax.swing.JSlider HeadYaw;
	private javax.swing.JButton InitPositionRobot;
	private javax.swing.JSlider LAnklePitch;
	private javax.swing.JSlider LAnkleRoll;
	private javax.swing.JSlider LElbowRoll;
	private javax.swing.JSlider LElbowYaw;
	private javax.swing.JSlider LHipPitch;
	private javax.swing.JSlider LHipRoll;
	private javax.swing.JSlider LHipYawPitch;
	private javax.swing.JSlider LKneePitch;
	private javax.swing.JSlider LShoulderPitch;
	private javax.swing.JSlider LShoulderRoll;
	private javax.swing.JButton LoadMov;
	private javax.swing.JButton ModifyMovement;
	private javax.swing.JButton NewMov;
	private javax.swing.JButton NewPlayMovement;
	private javax.swing.JButton NewTestInRobot;
	private javax.swing.JButton PlayMovement;
	private javax.swing.JSlider RAnklePitch;
	private javax.swing.JSlider RAnkleRoll;
	private javax.swing.JSlider RElbowRoll;
	private javax.swing.JSlider RElbowYaw;
	private javax.swing.JSlider RHipPitch;
	private javax.swing.JSlider RHipRoll;
	private javax.swing.JSlider RHipYawPitch;
	private javax.swing.JSlider RKneePitch;
	private javax.swing.JSlider RShoulderPitch;
	private javax.swing.JSlider RShoulderRoll;
	private javax.swing.JButton SaveMov;
	private javax.swing.JButton SaveValueHead;
	private javax.swing.JButton SaveValueLArm;
	private javax.swing.JButton SaveValueLLeg;
	private javax.swing.JButton SaveValueRArm;
	private javax.swing.JButton SaveValueRLeg;
	private javax.swing.JCheckBox SendSlidertoRobotHead;
	private javax.swing.JCheckBox SendSlidertoRobotLArm;
	private javax.swing.JCheckBox SendSlidertoRobotLLeg;
	private javax.swing.JCheckBox SendSlidertoRobotRArm;
	private javax.swing.JCheckBox SendSlidertoRobotRLeg;
	private javax.swing.JButton TestInRobot;
	private javax.swing.JButton btnAdd;
	private javax.swing.JButton btnAnt;
	private javax.swing.JButton btnDelete;
	private javax.swing.JButton btnInsert;
	private javax.swing.JButton btnNewAnt;
	private javax.swing.JButton btnNewNext;
	private javax.swing.JButton btnSig;
	private javax.swing.JCheckBox doHead;
	private javax.swing.JCheckBox doLArm;
	private javax.swing.JCheckBox doLLeg;
	private javax.swing.JCheckBox doRArm;
	private javax.swing.JCheckBox doRLeg;
	private javax.swing.JButton getAngleHead;
	private javax.swing.JButton getAngleLArm;
	private javax.swing.JButton getAngleLLeg;
	private javax.swing.JButton getAngleRArm;
	private javax.swing.JButton getAngleRLeg;
	private javax.swing.JLabel jLabel1;
	private javax.swing.JLabel jLabel10;
	private javax.swing.JLabel jLabel11;
	private javax.swing.JLabel jLabel12;
	private javax.swing.JLabel jLabel13;
	private javax.swing.JLabel jLabel14;
	private javax.swing.JLabel jLabel15;
	private javax.swing.JLabel jLabel16;
	private javax.swing.JLabel jLabel17;
	private javax.swing.JLabel jLabel18;
	private javax.swing.JLabel jLabel19;
	private javax.swing.JLabel jLabel2;
	private javax.swing.JLabel jLabel20;
	private javax.swing.JLabel jLabel21;
	private javax.swing.JLabel jLabel22;
	private javax.swing.JLabel jLabel23;
	private javax.swing.JLabel jLabel24;
	private javax.swing.JLabel jLabel25;
	private javax.swing.JLabel jLabel26;
	private javax.swing.JLabel jLabel27;
	private javax.swing.JLabel jLabel28;
	private javax.swing.JLabel jLabel29;
	private javax.swing.JLabel jLabel3;
	private javax.swing.JLabel jLabel30;
	private javax.swing.JLabel jLabel31;
	private javax.swing.JLabel jLabel32;
	private javax.swing.JLabel jLabel33;
	private javax.swing.JLabel jLabel34;
	private javax.swing.JLabel jLabel35;
	private javax.swing.JLabel jLabel36;
	private javax.swing.JLabel jLabel37;
	private javax.swing.JLabel jLabel38;
	private javax.swing.JLabel jLabel39;
	private javax.swing.JLabel jLabel4;
	private javax.swing.JLabel jLabel40;
	private javax.swing.JLabel jLabel42;
	private javax.swing.JLabel jLabel43;
	private javax.swing.JLabel jLabel5;
	private javax.swing.JLabel jLabel6;
	private javax.swing.JLabel jLabel7;
	private javax.swing.JLabel jLabel8;
	private javax.swing.JLabel jLabel9;
	private javax.swing.JLayeredPane jLayeredPane1;
	private javax.swing.JSeparator jSeparator1;
	private javax.swing.JSeparator jSeparator2;
	private javax.swing.JSeparator jSeparator3;
	private javax.swing.JSeparator jSeparator4;
	private javax.swing.JSeparator jSeparator5;
	private javax.swing.JTextField txtFileMov;
	private javax.swing.JTextField txtJoinName;
	private javax.swing.JTextField txtMovPosition;
	private javax.swing.JTextField txtNewFileMov;
	private javax.swing.JTextField txtNewJoinName;
	private javax.swing.JTextField txtNewMovPosition;
	private javax.swing.JTextField txtNewNumJoin;
	private javax.swing.JTextField txtNewNumberMove;
	private javax.swing.JTextField txtNumJoin;
	private javax.swing.JTextField txtNumberMove;
	private javax.swing.JTextField txtTimeHeadPitch;
	private javax.swing.JTextField txtTimeHeadYaw;
	private javax.swing.JTextField txtTimeLAnklePitch;
	private javax.swing.JTextField txtTimeLAnkleRoll;
	private javax.swing.JTextField txtTimeLElbowRoll;
	private javax.swing.JTextField txtTimeLElbowYaw;
	private javax.swing.JTextField txtTimeLHipPitch;
	private javax.swing.JTextField txtTimeLHipRoll;
	private javax.swing.JTextField txtTimeLHipYawPitch;
	private javax.swing.JTextField txtTimeLKneePitch;
	private javax.swing.JTextField txtTimeLShoulderPitch;
	private javax.swing.JTextField txtTimeLShoulderRoll;
	private javax.swing.JTextField txtTimeRAnklePitch;
	private javax.swing.JTextField txtTimeRAnkleRoll;
	private javax.swing.JTextField txtTimeRElbowRoll;
	private javax.swing.JTextField txtTimeRElbowYaw;
	private javax.swing.JTextField txtTimeRHipPitch;
	private javax.swing.JTextField txtTimeRHipRoll;
	private javax.swing.JTextField txtTimeRHipYawPitch;
	private javax.swing.JTextField txtTimeRKneePitch;
	private javax.swing.JTextField txtTimeRShoulderPitch;
	private javax.swing.JTextField txtTimeRShoulderRoll;
	// End of variables declaration//GEN-END:variables

}
