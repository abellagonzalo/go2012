/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * ComponentPerceptionPanel.java
 *
 * Created on 28-oct-2009, 13:08:37
 */
package jmanager.components.poseGenerator;

import java.awt.Component;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.text.ParseException;
import java.util.Collections;
import java.util.Iterator;
import java.util.Properties;
import java.util.Scanner;
import java.util.StringTokenizer;
import java.util.Vector;
import jmanager.*;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JSlider;
import javax.swing.JToggleButton;

/**
 *
 * @author Carlos Agüero and Francisco Martín
 */
public class PoseGenerator extends javax.swing.JPanel {

	private File currentFolder;
	private File currentFile;
	private Integer frame;
	private Vector<JSlider> slidersVector;
	private Vector<Kick> kickVector;
	private KicksHistory history;
	private Snapshot state;
	private int previousIndex;
	private Kick kick;
	private DatagramSocket socket;
	private InetAddress sndaddress;
	private int sndport;
	private InetAddress rcvaddress;
	static public final int MAXBUFFER = 65536;
	private byte[] sndbuffer = new byte[MAXBUFFER];
	private Socket s;
	private PrintWriter out;
	private BufferedReader in;
	private boolean connected = false;
	private final Vector<Component> widgetVector;

	/** Creates new form ComponentGTLocalization */
	public PoseGenerator() {
		initComponents();

		slidersVector = new Vector<JSlider>();
		widgetVector = new Vector<Component>();
		kick = new Kick();
		kickVector = new Vector<Kick>();
		history = new KicksHistory();
		state = new Snapshot();
		previousIndex = -1;

		slidersVector.add(HeadYaw);
		slidersVector.add(HeadPitch);

		slidersVector.add(LShoulderPitch);
		slidersVector.add(LShoulderRoll);
		slidersVector.add(LElbowYaw);
		slidersVector.add(LElbowRoll);

		slidersVector.add(LHipYawPitch);
		slidersVector.add(LHipRoll);
		slidersVector.add(LHipPitch);
		slidersVector.add(LKneePitch);
		slidersVector.add(LAnklePitch);
		slidersVector.add(LAnkleRoll);

		slidersVector.add(RHipYawPitch);
		slidersVector.add(RHipRoll);
		slidersVector.add(RHipPitch);
		slidersVector.add(RKneePitch);
		slidersVector.add(RAnklePitch);
		slidersVector.add(RAnkleRoll);

		slidersVector.add(RShoulderPitch);
		slidersVector.add(RShoulderRoll);
		slidersVector.add(RElbowYaw);
		slidersVector.add(RElbowRoll);

		// Add sliders
		for (int i = 0; i < slidersVector.size(); i++) {
			widgetVector.add(slidersVector.get(i));
		}

		widgetVector.add(headPower);
		widgetVector.add(leftArmPower);
		widgetVector.add(rightArmPower);
		widgetVector.add(leftLegPower);
		widgetVector.add(rightLegPower);
		widgetVector.add(jComboBox1);
		widgetVector.add(jButtonNewKick);

		widgetVector.add(jButtonNext);
		widgetVector.add(jButtonTestMovement);
		widgetVector.add(jButtonNewMovement);
		widgetVector.add(jButtonDeleteMovement);
		widgetVector.add(jSpinnerSpeed);
		widgetVector.add(jButtonRun);
		widgetVector.add(jButtonSave);
		widgetVector.add(jButtonGenerate);
		widgetVector.add(jButtonSavePoseAs);
		widgetVector.add(jButtonSymmetric);
		widgetVector.add(jButtonPoseInit);
		widgetVector.add(jButtonHeadCenter);

		widgetVector.add(HeadYawMinus);
		widgetVector.add(HeadYawPlus);
		widgetVector.add(HeadPitchMinus);
		widgetVector.add(HeadPitchPlus);

		widgetVector.add(LShoulderPitchMinus);
		widgetVector.add(LShoulderPitchPlus);
		widgetVector.add(LShoulderRollMinus);
		widgetVector.add(LShoulderRollPlus);
		widgetVector.add(LElbowYawMinus);
		widgetVector.add(LElbowYawPlus);
		widgetVector.add(LElbowRollMinus);
		widgetVector.add(LElbowRollPlus);

		widgetVector.add(RShoulderPitchMinus);
		widgetVector.add(RShoulderPitchPlus);
		widgetVector.add(RShoulderRollMinus);
		widgetVector.add(RShoulderRollPlus);
		widgetVector.add(RElbowYawMinus);
		widgetVector.add(RElbowYawPlus);
		widgetVector.add(RElbowRollMinus);
		widgetVector.add(RElbowRollPlus);

		widgetVector.add(LHipYawPitchMinus);
		widgetVector.add(LHipYawPitchPlus);
		widgetVector.add(LHipRollMinus);
		widgetVector.add(LHipRollPlus);
		widgetVector.add(LHipPitchMinus);
		widgetVector.add(LHipPitchPlus);
		widgetVector.add(LKneePitchMinus);
		widgetVector.add(LKneePitchPlus);
		widgetVector.add(LAnklePitchMinus);
		widgetVector.add(LAnklePitchPlus);
		widgetVector.add(LAnkleRollMinus);
		widgetVector.add(LAnkleRollPlus);

		widgetVector.add(RHipYawPitchMinus);
		widgetVector.add(RHipYawPitchPlus);
		widgetVector.add(RHipRollMinus);
		widgetVector.add(RHipRollPlus);
		widgetVector.add(RHipPitchMinus);
		widgetVector.add(RHipPitchPlus);
		widgetVector.add(RKneePitchMinus);
		widgetVector.add(RKneePitchPlus);
		widgetVector.add(RAnklePitchMinus);
		widgetVector.add(RAnklePitchPlus);
		widgetVector.add(RAnkleRollMinus);
		widgetVector.add(RAnkleRollPlus);

		loadDefaultKicksFolder();        
	}

	/** This method is called from within the constructor to
	 * initialize the form.
	 * WARNING: Do NOT modify this code. The content of this method is
	 * always regenerated by the Form Editor.
	 */
	@SuppressWarnings("unchecked")
	// <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
	private void initComponents() {

		jPanelCommands = new javax.swing.JPanel();
		jLabelServerPort = new javax.swing.JLabel();
		jTextFieldServerPort = new javax.swing.JTextField();
		jCheckBoxActivate = new javax.swing.JCheckBox();
		jButtonNewKick = new javax.swing.JButton();
		jButtonGenerate = new javax.swing.JButton();
		jButtonSavePoseAs = new javax.swing.JButton();
		jButtonSymmetric = new javax.swing.JButton();
		jButtonPoseInit = new javax.swing.JButton();
		jPanelJoints = new javax.swing.JPanel();
		jPanelHead = new javax.swing.JPanel();
		jLabelIconHead = new javax.swing.JLabel();
		headPower = new javax.swing.JToggleButton();
		HeadYawPlus = new javax.swing.JButton();
		HeadYaw = new javax.swing.JSlider();
		HeadYawMinus = new javax.swing.JButton();
		HeadPitchMinus = new javax.swing.JButton();
		HeadPitch = new javax.swing.JSlider();
		HeadPitchPlus = new javax.swing.JButton();
		jButtonHeadCenter = new javax.swing.JButton();
		jPanelArms = new javax.swing.JPanel();
		jPanelLeftArms = new javax.swing.JPanel();
		jLabelIconLeftArm = new javax.swing.JLabel();
		LShoulderPitch = new javax.swing.JSlider();
		LShoulderRoll = new javax.swing.JSlider();
		LElbowYaw = new javax.swing.JSlider();
		LElbowRoll = new javax.swing.JSlider();
		LElbowRollPlus = new javax.swing.JButton();
		LShoulderPitchPlus = new javax.swing.JButton();
		LShoulderRollPlus = new javax.swing.JButton();
		LElbowYawPlus = new javax.swing.JButton();
		LShoulderPitchMinus = new javax.swing.JButton();
		LShoulderRollMinus = new javax.swing.JButton();
		LElbowYawMinus = new javax.swing.JButton();
		LElbowRollMinus = new javax.swing.JButton();
		leftArmPower = new javax.swing.JToggleButton();
		jPanelRightArms = new javax.swing.JPanel();
		jLabelIconRightArm = new javax.swing.JLabel();
		RShoulderPitch = new javax.swing.JSlider();
		RShoulderRoll = new javax.swing.JSlider();
		RElbowYaw = new javax.swing.JSlider();
		RElbowRoll = new javax.swing.JSlider();
		RShoulderRollPlus = new javax.swing.JButton();
		RElbowRollPlus = new javax.swing.JButton();
		RElbowYawPlus = new javax.swing.JButton();
		RShoulderPitchPlus = new javax.swing.JButton();
		RShoulderRollMinus = new javax.swing.JButton();
		RElbowRollMinus = new javax.swing.JButton();
		RElbowYawMinus = new javax.swing.JButton();
		RShoulderPitchMinus = new javax.swing.JButton();
		rightArmPower = new javax.swing.JToggleButton();
		jPanelLegs = new javax.swing.JPanel();
		jPanelLeftLeg = new javax.swing.JPanel();
		jPanel1 = new javax.swing.JPanel();
		jLabelIconLeftLeg = new javax.swing.JLabel();
		LHipYawPitchMinus = new javax.swing.JButton();
		LHipYawPitch = new javax.swing.JSlider();
		LHipYawPitchPlus = new javax.swing.JButton();
		LHipRollMinus = new javax.swing.JButton();
		LHipRoll = new javax.swing.JSlider();
		LHipRollPlus = new javax.swing.JButton();
		LHipPitchMinus = new javax.swing.JButton();
		LHipPitch = new javax.swing.JSlider();
		LHipPitchPlus = new javax.swing.JButton();
		jPanel2 = new javax.swing.JPanel();
		leftLegPower = new javax.swing.JToggleButton();
		LKneePitchMinus = new javax.swing.JButton();
		LKneePitch = new javax.swing.JSlider();
		LKneePitchPlus = new javax.swing.JButton();
		LAnkleRollMinus = new javax.swing.JButton();
		LAnkleRoll = new javax.swing.JSlider();
		LAnkleRollPlus = new javax.swing.JButton();
		LAnklePitchMinus = new javax.swing.JButton();
		LAnklePitch = new javax.swing.JSlider();
		LAnklePitchPlus = new javax.swing.JButton();
		jPanelRightLeg = new javax.swing.JPanel();
		jPanel3 = new javax.swing.JPanel();
		jLabelIconRightLeg = new javax.swing.JLabel();
		RHipYawPitchMinus = new javax.swing.JButton();
		RHipYawPitch = new javax.swing.JSlider();
		RHipYawPitchPlus = new javax.swing.JButton();
		RHipRollMinus = new javax.swing.JButton();
		RHipRoll = new javax.swing.JSlider();
		RHipRollPlus = new javax.swing.JButton();
		RHipPitchMinus = new javax.swing.JButton();
		RHipPitch = new javax.swing.JSlider();
		RHipPitchPlus = new javax.swing.JButton();
		jPanel4 = new javax.swing.JPanel();
		rightLegPower = new javax.swing.JToggleButton();
		RKneePitchMinus = new javax.swing.JButton();
		RKneePitch = new javax.swing.JSlider();
		RKneePitchPlus = new javax.swing.JButton();
		RAnkleRollMinus = new javax.swing.JButton();
		RAnkleRoll = new javax.swing.JSlider();
		RAnkleRollPlus = new javax.swing.JButton();
		RAnklePitchMinus = new javax.swing.JButton();
		RAnklePitch = new javax.swing.JSlider();
		RAnklePitchPlus = new javax.swing.JButton();
		jPanelMovement = new javax.swing.JPanel();
		jLabel1 = new javax.swing.JLabel();
		jComboBox1 = new javax.swing.JComboBox();
		jButtonRun = new javax.swing.JButton();
		jButtonSave = new javax.swing.JButton();
		jButtonUndo = new javax.swing.JButton();
		jLabel2 = new javax.swing.JLabel();
		jSpinnerSpeed = new javax.swing.JSpinner();
		jButtonNewMovement = new javax.swing.JButton();
		jButtonPrev = new javax.swing.JButton();
		jButtonTestMovement = new javax.swing.JButton();
		jButtonNext = new javax.swing.JButton();
		jButtonDeleteMovement = new javax.swing.JButton();
		jLabelFrame = new javax.swing.JLabel();
		jPanelLog = new javax.swing.JPanel();
		jLabelLog = new javax.swing.JLabel();

		setFocusable(false);
		setMinimumSize(new java.awt.Dimension(330, 250));
		setName("Form"); // NOI18N
		setPreferredSize(new java.awt.Dimension(900, 600));
		setLayout(new org.netbeans.lib.awtextra.AbsoluteLayout());

		org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance().getContext().getResourceMap(PoseGenerator.class);
		jPanelCommands.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("jPanelCommands.border.title"))); // NOI18N
		jPanelCommands.setFocusable(false);
		jPanelCommands.setMinimumSize(new java.awt.Dimension(397, 40));
		jPanelCommands.setName("jPanelCommands"); // NOI18N
		jPanelCommands.setPreferredSize(new java.awt.Dimension(851, 60));
		jPanelCommands.setRequestFocusEnabled(false);
		jPanelCommands.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.CENTER, 5, 0));

		jLabelServerPort.setText(resourceMap.getString("jLabelServerPort.text")); // NOI18N
		jLabelServerPort.setName("jLabelServerPort"); // NOI18N
		jPanelCommands.add(jLabelServerPort);

		jTextFieldServerPort.setText(resourceMap.getString("jTextFieldServerPort.text")); // NOI18N
		jTextFieldServerPort.setName("jTextFieldServerPort"); // NOI18N
		jPanelCommands.add(jTextFieldServerPort);

		jCheckBoxActivate.setText(resourceMap.getString("jCheckBoxActivate.text")); // NOI18N
		jCheckBoxActivate.setName("jCheckBoxActivate"); // NOI18N
		jCheckBoxActivate.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				jCheckBoxActivateActionPerformed(evt);
			}
		});
		jPanelCommands.add(jCheckBoxActivate);

		jButtonNewKick.setText(resourceMap.getString("jButtonNewKick.text")); // NOI18N
		jButtonNewKick.setEnabled(false);
		jButtonNewKick.setName("jButtonNewKick"); // NOI18N
		jButtonNewKick.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				newPose(evt);
			}
		});
		jPanelCommands.add(jButtonNewKick);

		jButtonGenerate.setText(resourceMap.getString("jButtonGenerate.text")); // NOI18N
		jButtonGenerate.setEnabled(false);
		jButtonGenerate.setName("jButtonGenerate"); // NOI18N
		jButtonGenerate.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				generatePressed(evt);
			}
		});
		jPanelCommands.add(jButtonGenerate);

		jButtonSavePoseAs.setText(resourceMap.getString("jButtonSavePoseAs.text")); // NOI18N
		jButtonSavePoseAs.setEnabled(false);
		jButtonSavePoseAs.setName("jButtonSavePoseAs"); // NOI18N
		jButtonSavePoseAs.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				savePoseAsPressed(evt);
			}
		});
		jPanelCommands.add(jButtonSavePoseAs);

		jButtonSymmetric.setText(resourceMap.getString("jButtonSymmetric.text")); // NOI18N
		jButtonSymmetric.setEnabled(false);
		jButtonSymmetric.setName("jButtonSymmetric"); // NOI18N
		jButtonSymmetric.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				generateSymmetricPressed(evt);
			}
		});
		jPanelCommands.add(jButtonSymmetric);

		jButtonPoseInit.setText(resourceMap.getString("jButtonPoseInit.text")); // NOI18N
		jButtonPoseInit.setEnabled(false);
		jButtonPoseInit.setName("jButtonPoseInit"); // NOI18N
		jButtonPoseInit.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				poseInitReleased(evt);
			}
		});
		jPanelCommands.add(jButtonPoseInit);

		add(jPanelCommands, new org.netbeans.lib.awtextra.AbsoluteConstraints(0, 0, 828, -1));

		jPanelJoints.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("jPanelJoints.border.title"))); // NOI18N
		jPanelJoints.setFocusable(false);
		jPanelJoints.setName("jPanelJoints"); // NOI18N
		jPanelJoints.setPreferredSize(new java.awt.Dimension(851, 400));
		jPanelJoints.setLayout(new javax.swing.BoxLayout(jPanelJoints, javax.swing.BoxLayout.PAGE_AXIS));

		jPanelHead.setBorder(null);
		jPanelHead.setFocusable(false);
		jPanelHead.setFont(resourceMap.getFont("jPanelHead.font")); // NOI18N
		jPanelHead.setName("jPanelHead"); // NOI18N
		jPanelHead.setPreferredSize(new java.awt.Dimension(851, 80));

		jLabelIconHead.setIcon(resourceMap.getIcon("jLabelIconHead.icon")); // NOI18N
		jLabelIconHead.setText(resourceMap.getString("jLabelIconHead.text")); // NOI18N
		jLabelIconHead.setName("jLabelIconHead"); // NOI18N
		jPanelHead.add(jLabelIconHead);

		headPower.setIcon(resourceMap.getIcon("headPower.icon")); // NOI18N
		headPower.setSelected(true);
		headPower.setText(resourceMap.getString("headPower.text")); // NOI18N
		headPower.setEnabled(false);
		headPower.setName("headPower"); // NOI18N
		headPower.setPreferredSize(new java.awt.Dimension(30, 30));
		headPower.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				powerButtonPressed(evt);
			}
		});
		jPanelHead.add(headPower);

		HeadYawPlus.setFont(resourceMap.getFont("HeadYawPlus.font")); // NOI18N
		HeadYawPlus.setText(resourceMap.getString("HeadYawPlus.text")); // NOI18N
		HeadYawPlus.setEnabled(false);
		HeadYawPlus.setMinimumSize(new java.awt.Dimension(20, 20));
		HeadYawPlus.setName("HeadYawPlus"); // NOI18N
		HeadYawPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		HeadYawPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelHead.add(HeadYawPlus);

		HeadYaw.setMaximum(119);
		HeadYaw.setMinimum(-119);
		HeadYaw.setValue(0);
		HeadYaw.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("HeadYaw.border.title"))); // NOI18N
		HeadYaw.setEnabled(false);
		HeadYaw.setInverted(true);
		HeadYaw.setName("HeadYaw"); // NOI18N
		HeadYaw.setPreferredSize(new java.awt.Dimension(50, 70));
		HeadYaw.setRequestFocusEnabled(false);
		HeadYaw.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanelHead.add(HeadYaw);

		HeadYawMinus.setFont(resourceMap.getFont("HeadYawMinus.font")); // NOI18N
		HeadYawMinus.setText(resourceMap.getString("HeadYawMinus.text")); // NOI18N
		HeadYawMinus.setEnabled(false);
		HeadYawMinus.setName("HeadYawMinus"); // NOI18N
		HeadYawMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		HeadYawMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelHead.add(HeadYawMinus);

		HeadPitchMinus.setFont(resourceMap.getFont("HeadYawMinus.font")); // NOI18N
		HeadPitchMinus.setText(resourceMap.getString("HeadPitchMinus.text")); // NOI18N
		HeadPitchMinus.setEnabled(false);
		HeadPitchMinus.setName("HeadPitchMinus"); // NOI18N
		HeadPitchMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		HeadPitchMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelHead.add(HeadPitchMinus);

		HeadPitch.setMaximum(29);
		HeadPitch.setMinimum(-38);
		HeadPitch.setValue(0);
		HeadPitch.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("HeadPitch.border.title"))); // NOI18N
		HeadPitch.setEnabled(false);
		HeadPitch.setName("HeadPitch"); // NOI18N
		HeadPitch.setPreferredSize(new java.awt.Dimension(50, 70));
		HeadPitch.setRequestFocusEnabled(false);
		HeadPitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanelHead.add(HeadPitch);

		HeadPitchPlus.setFont(resourceMap.getFont("HeadYawMinus.font")); // NOI18N
		HeadPitchPlus.setText(resourceMap.getString("HeadPitchPlus.text")); // NOI18N
		HeadPitchPlus.setEnabled(false);
		HeadPitchPlus.setName("HeadPitchPlus"); // NOI18N
		HeadPitchPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		HeadPitchPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelHead.add(HeadPitchPlus);

		jButtonHeadCenter.setText(resourceMap.getString("jButtonHeadCenter.text")); // NOI18N
		jButtonHeadCenter.setEnabled(false);
		jButtonHeadCenter.setName("jButtonHeadCenter"); // NOI18N
		jButtonHeadCenter.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				centerButtonReleased(evt);
			}
		});
		jPanelHead.add(jButtonHeadCenter);

		jPanelJoints.add(jPanelHead);

		jPanelArms.setBorder(null);
		jPanelArms.setName("jPanelArms"); // NOI18N
		jPanelArms.setPreferredSize(new java.awt.Dimension(850, 100));
		jPanelArms.setLayout(new javax.swing.BoxLayout(jPanelArms, javax.swing.BoxLayout.LINE_AXIS));

		jPanelLeftArms.setBorder(null);
		jPanelLeftArms.setName("jPanelLeftArms"); // NOI18N
		jPanelLeftArms.setPreferredSize(new java.awt.Dimension(440, 99));
		jPanelLeftArms.setLayout(null);

		jLabelIconLeftArm.setIcon(resourceMap.getIcon("jLabelIconLeftArm.icon")); // NOI18N
		jLabelIconLeftArm.setText(resourceMap.getString("jLabelIconLeftArm.text")); // NOI18N
		jLabelIconLeftArm.setName("jLabelIconLeftArm"); // NOI18N
		jLabelIconLeftArm.setRequestFocusEnabled(false);
		jPanelLeftArms.add(jLabelIconLeftArm);
		jLabelIconLeftArm.setBounds(10, 10, 30, 50);

		LShoulderPitch.setMaximum(119);
		LShoulderPitch.setMinimum(-119);
		LShoulderPitch.setValue(0);
		LShoulderPitch.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("LShoulderPitch.border.title"))); // NOI18N
		LShoulderPitch.setEnabled(false);
		LShoulderPitch.setName("LShoulderPitch"); // NOI18N
		LShoulderPitch.setPreferredSize(new java.awt.Dimension(50, 70));
		LShoulderPitch.setRequestFocusEnabled(false);
		LShoulderPitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanelLeftArms.add(LShoulderPitch);
		LShoulderPitch.setBounds(60, 10, 50, 70);

		LShoulderRoll.setMaximum(94);
		LShoulderRoll.setMinimum(1);
		LShoulderRoll.setValue(45);
		LShoulderRoll.setAlignmentY(-2.5F);
		LShoulderRoll.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("LShoulderRoll.border.title"))); // NOI18N
		LShoulderRoll.setEnabled(false);
		LShoulderRoll.setName("LShoulderRoll"); // NOI18N
		LShoulderRoll.setPreferredSize(new java.awt.Dimension(50, 70));
		LShoulderRoll.setRequestFocusEnabled(false);
		LShoulderRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanelLeftArms.add(LShoulderRoll);
		LShoulderRoll.setBounds(150, 10, 50, 70);

		LElbowYaw.setMaximum(119);
		LElbowYaw.setMinimum(-119);
		LElbowYaw.setValue(0);
		LElbowYaw.setAlignmentY(-2.5F);
		LElbowYaw.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("LElbowYaw.border.title"))); // NOI18N
		LElbowYaw.setEnabled(false);
		LElbowYaw.setName("LElbowYaw"); // NOI18N
		LElbowYaw.setPreferredSize(new java.awt.Dimension(50, 70));
		LElbowYaw.setRequestFocusEnabled(false);
		LElbowYaw.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanelLeftArms.add(LElbowYaw);
		LElbowYaw.setBounds(240, 10, 50, 70);

		LElbowRoll.setMaximum(-1);
		LElbowRoll.setMinimum(-89);
		LElbowRoll.setAlignmentY(-2.5F);
		LElbowRoll.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("LElbowRoll.border.title"))); // NOI18N
		LElbowRoll.setEnabled(false);
		LElbowRoll.setName("LElbowRoll"); // NOI18N
		LElbowRoll.setPreferredSize(new java.awt.Dimension(50, 70));
		LElbowRoll.setRequestFocusEnabled(false);
		LElbowRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanelLeftArms.add(LElbowRoll);
		LElbowRoll.setBounds(330, 10, 50, 70);

		LElbowRollPlus.setFont(resourceMap.getFont("LElbowRollPlus.font")); // NOI18N
		LElbowRollPlus.setText(resourceMap.getString("LElbowRollPlus.text")); // NOI18N
		LElbowRollPlus.setEnabled(false);
		LElbowRollPlus.setName("LElbowRollPlus"); // NOI18N
		LElbowRollPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		LElbowRollPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelLeftArms.add(LElbowRollPlus);
		LElbowRollPlus.setBounds(380, 40, 20, 20);

		LShoulderPitchPlus.setFont(resourceMap.getFont("LShoulderPitchPlus.font")); // NOI18N
		LShoulderPitchPlus.setText(resourceMap.getString("LShoulderPitchPlus.text")); // NOI18N
		LShoulderPitchPlus.setEnabled(false);
		LShoulderPitchPlus.setName("LShoulderPitchPlus"); // NOI18N
		LShoulderPitchPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		LShoulderPitchPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelLeftArms.add(LShoulderPitchPlus);
		LShoulderPitchPlus.setBounds(110, 40, 20, 20);

		LShoulderRollPlus.setFont(resourceMap.getFont("LShoulderRollPlus.font")); // NOI18N
		LShoulderRollPlus.setText(resourceMap.getString("LShoulderRollPlus.text")); // NOI18N
		LShoulderRollPlus.setEnabled(false);
		LShoulderRollPlus.setName("LShoulderRollPlus"); // NOI18N
		LShoulderRollPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		LShoulderRollPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelLeftArms.add(LShoulderRollPlus);
		LShoulderRollPlus.setBounds(200, 40, 20, 20);

		LElbowYawPlus.setFont(resourceMap.getFont("LElbowYawPlus.font")); // NOI18N
		LElbowYawPlus.setText(resourceMap.getString("LElbowYawPlus.text")); // NOI18N
		LElbowYawPlus.setEnabled(false);
		LElbowYawPlus.setName("LElbowYawPlus"); // NOI18N
		LElbowYawPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		LElbowYawPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelLeftArms.add(LElbowYawPlus);
		LElbowYawPlus.setBounds(290, 40, 20, 20);

		LShoulderPitchMinus.setFont(resourceMap.getFont("LShoulderPitchMinus.font")); // NOI18N
		LShoulderPitchMinus.setText(resourceMap.getString("LShoulderPitchMinus.text")); // NOI18N
		LShoulderPitchMinus.setEnabled(false);
		LShoulderPitchMinus.setName("LShoulderPitchMinus"); // NOI18N
		LShoulderPitchMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		LShoulderPitchMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelLeftArms.add(LShoulderPitchMinus);
		LShoulderPitchMinus.setBounds(40, 40, 20, 20);

		LShoulderRollMinus.setFont(resourceMap.getFont("LShoulderRollMinus.font")); // NOI18N
		LShoulderRollMinus.setText(resourceMap.getString("LShoulderRollMinus.text")); // NOI18N
		LShoulderRollMinus.setEnabled(false);
		LShoulderRollMinus.setName("LShoulderRollMinus"); // NOI18N
		LShoulderRollMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		LShoulderRollMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelLeftArms.add(LShoulderRollMinus);
		LShoulderRollMinus.setBounds(130, 40, 20, 20);

		LElbowYawMinus.setFont(resourceMap.getFont("LElbowYawMinus.font")); // NOI18N
		LElbowYawMinus.setText(resourceMap.getString("LElbowYawMinus.text")); // NOI18N
		LElbowYawMinus.setEnabled(false);
		LElbowYawMinus.setName("LElbowYawMinus"); // NOI18N
		LElbowYawMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		LElbowYawMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelLeftArms.add(LElbowYawMinus);
		LElbowYawMinus.setBounds(220, 40, 20, 20);

		LElbowRollMinus.setFont(resourceMap.getFont("LElbowRollMinus.font")); // NOI18N
		LElbowRollMinus.setText(resourceMap.getString("LElbowRollMinus.text")); // NOI18N
		LElbowRollMinus.setEnabled(false);
		LElbowRollMinus.setName("LElbowRollMinus"); // NOI18N
		LElbowRollMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		LElbowRollMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelLeftArms.add(LElbowRollMinus);
		LElbowRollMinus.setBounds(310, 40, 20, 20);

		leftArmPower.setIcon(resourceMap.getIcon("leftArmPower.icon")); // NOI18N
		leftArmPower.setSelected(true);
		leftArmPower.setEnabled(false);
		leftArmPower.setName("leftArmPower"); // NOI18N
		leftArmPower.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				powerButtonPressed(evt);
			}
		});
		jPanelLeftArms.add(leftArmPower);
		leftArmPower.setBounds(10, 60, 30, 30);

		jPanelArms.add(jPanelLeftArms);

		jPanelRightArms.setBorder(null);
		jPanelRightArms.setName("jPanelRightArms"); // NOI18N
		jPanelRightArms.setPreferredSize(new java.awt.Dimension(440, 96));
		jPanelRightArms.setRequestFocusEnabled(false);
		jPanelRightArms.setLayout(null);

		jLabelIconRightArm.setIcon(resourceMap.getIcon("jLabelIconRightArm.icon")); // NOI18N
		jLabelIconRightArm.setText(resourceMap.getString("jLabelIconRightArm.text")); // NOI18N
		jLabelIconRightArm.setName("jLabelIconRightArm"); // NOI18N
		jPanelRightArms.add(jLabelIconRightArm);
		jLabelIconRightArm.setBounds(10, 10, 30, 50);

		RShoulderPitch.setMaximum(119);
		RShoulderPitch.setMinimum(-119);
		RShoulderPitch.setValue(0);
		RShoulderPitch.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("RShoulderPitch.border.title"))); // NOI18N
		RShoulderPitch.setEnabled(false);
		RShoulderPitch.setName("RShoulderPitch"); // NOI18N
		RShoulderPitch.setPreferredSize(new java.awt.Dimension(50, 70));
		RShoulderPitch.setRequestFocusEnabled(false);
		RShoulderPitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanelRightArms.add(RShoulderPitch);
		RShoulderPitch.setBounds(60, 10, 50, 70);

		RShoulderRoll.setMaximum(-1);
		RShoulderRoll.setMinimum(-94);
		RShoulderRoll.setValue(-45);
		RShoulderRoll.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("RShoulderRoll.border.title"))); // NOI18N
		RShoulderRoll.setEnabled(false);
		RShoulderRoll.setName("RShoulderRoll"); // NOI18N
		RShoulderRoll.setPreferredSize(new java.awt.Dimension(50, 70));
		RShoulderRoll.setRequestFocusEnabled(false);
		RShoulderRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanelRightArms.add(RShoulderRoll);
		RShoulderRoll.setBounds(150, 10, 50, 70);

		RElbowYaw.setMaximum(119);
		RElbowYaw.setMinimum(-119);
		RElbowYaw.setValue(0);
		RElbowYaw.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("RElbowYaw.border.title"))); // NOI18N
		RElbowYaw.setEnabled(false);
		RElbowYaw.setName("RElbowYaw"); // NOI18N
		RElbowYaw.setPreferredSize(new java.awt.Dimension(50, 70));
		RElbowYaw.setRequestFocusEnabled(false);
		RElbowYaw.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanelRightArms.add(RElbowYaw);
		RElbowYaw.setBounds(240, 10, 50, 70);

		RElbowRoll.setMaximum(89);
		RElbowRoll.setMinimum(1);
		RElbowRoll.setValue(45);
		RElbowRoll.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("RElbowRoll.border.title"))); // NOI18N
		RElbowRoll.setEnabled(false);
		RElbowRoll.setName("RElbowRoll"); // NOI18N
		RElbowRoll.setPreferredSize(new java.awt.Dimension(50, 70));
		RElbowRoll.setRequestFocusEnabled(false);
		RElbowRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanelRightArms.add(RElbowRoll);
		RElbowRoll.setBounds(330, 10, 50, 70);

		RShoulderRollPlus.setFont(resourceMap.getFont("RElbowRollPlus.font")); // NOI18N
		RShoulderRollPlus.setText(resourceMap.getString("RShoulderRollPlus.text")); // NOI18N
		RShoulderRollPlus.setEnabled(false);
		RShoulderRollPlus.setName("RShoulderRollPlus"); // NOI18N
		RShoulderRollPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		RShoulderRollPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelRightArms.add(RShoulderRollPlus);
		RShoulderRollPlus.setBounds(200, 40, 20, 20);

		RElbowRollPlus.setFont(resourceMap.getFont("RElbowRollPlus.font")); // NOI18N
		RElbowRollPlus.setText(resourceMap.getString("RElbowRollPlus.text")); // NOI18N
		RElbowRollPlus.setEnabled(false);
		RElbowRollPlus.setName("RElbowRollPlus"); // NOI18N
		RElbowRollPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		RElbowRollPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelRightArms.add(RElbowRollPlus);
		RElbowRollPlus.setBounds(380, 40, 20, 20);

		RElbowYawPlus.setFont(resourceMap.getFont("RElbowRollPlus.font")); // NOI18N
		RElbowYawPlus.setText(resourceMap.getString("RElbowYawPlus.text")); // NOI18N
		RElbowYawPlus.setEnabled(false);
		RElbowYawPlus.setName("RElbowYawPlus"); // NOI18N
		RElbowYawPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		RElbowYawPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelRightArms.add(RElbowYawPlus);
		RElbowYawPlus.setBounds(290, 40, 20, 20);

		RShoulderPitchPlus.setFont(resourceMap.getFont("RShoulderPitchPlus.font")); // NOI18N
		RShoulderPitchPlus.setText(resourceMap.getString("RShoulderPitchPlus.text")); // NOI18N
		RShoulderPitchPlus.setEnabled(false);
		RShoulderPitchPlus.setName("RShoulderPitchPlus"); // NOI18N
		RShoulderPitchPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		RShoulderPitchPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelRightArms.add(RShoulderPitchPlus);
		RShoulderPitchPlus.setBounds(110, 40, 20, 20);

		RShoulderRollMinus.setFont(resourceMap.getFont("RShoulderRollMinus.font")); // NOI18N
		RShoulderRollMinus.setText(resourceMap.getString("RShoulderRollMinus.text")); // NOI18N
		RShoulderRollMinus.setEnabled(false);
		RShoulderRollMinus.setName("RShoulderRollMinus"); // NOI18N
		RShoulderRollMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		RShoulderRollMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelRightArms.add(RShoulderRollMinus);
		RShoulderRollMinus.setBounds(130, 40, 20, 20);

		RElbowRollMinus.setFont(resourceMap.getFont("RElbowRollPlus.font")); // NOI18N
		RElbowRollMinus.setText(resourceMap.getString("RElbowRollMinus.text")); // NOI18N
		RElbowRollMinus.setEnabled(false);
		RElbowRollMinus.setName("RElbowRollMinus"); // NOI18N
		RElbowRollMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		RElbowRollMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelRightArms.add(RElbowRollMinus);
		RElbowRollMinus.setBounds(310, 40, 20, 20);

		RElbowYawMinus.setFont(resourceMap.getFont("RElbowRollPlus.font")); // NOI18N
		RElbowYawMinus.setText(resourceMap.getString("RElbowYawMinus.text")); // NOI18N
		RElbowYawMinus.setEnabled(false);
		RElbowYawMinus.setName("RElbowYawMinus"); // NOI18N
		RElbowYawMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		RElbowYawMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelRightArms.add(RElbowYawMinus);
		RElbowYawMinus.setBounds(220, 40, 20, 20);

		RShoulderPitchMinus.setFont(resourceMap.getFont("RShoulderPitchMinus.font")); // NOI18N
		RShoulderPitchMinus.setText(resourceMap.getString("RShoulderPitchMinus.text")); // NOI18N
		RShoulderPitchMinus.setEnabled(false);
		RShoulderPitchMinus.setName("RShoulderPitchMinus"); // NOI18N
		RShoulderPitchMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		RShoulderPitchMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanelRightArms.add(RShoulderPitchMinus);
		RShoulderPitchMinus.setBounds(40, 40, 20, 20);

		rightArmPower.setIcon(resourceMap.getIcon("rightArmPower.icon")); // NOI18N
		rightArmPower.setSelected(true);
		rightArmPower.setEnabled(false);
		rightArmPower.setName("rightArmPower"); // NOI18N
		rightArmPower.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				powerButtonPressed(evt);
			}
		});
		jPanelRightArms.add(rightArmPower);
		rightArmPower.setBounds(10, 60, 30, 30);

		jPanelArms.add(jPanelRightArms);

		jPanelJoints.add(jPanelArms);

		jPanelLegs.setBorder(null);
		jPanelLegs.setName("jPanelLegs"); // NOI18N
		jPanelLegs.setPreferredSize(new java.awt.Dimension(850, 179));
		jPanelLegs.setLayout(new javax.swing.BoxLayout(jPanelLegs, javax.swing.BoxLayout.LINE_AXIS));

		jPanelLeftLeg.setBorder(null);
		jPanelLeftLeg.setName("jPanelLeftLeg"); // NOI18N
		jPanelLeftLeg.setPreferredSize(new java.awt.Dimension(440, 175));

		jPanel1.setName("jPanel1"); // NOI18N

		jLabelIconLeftLeg.setIcon(resourceMap.getIcon("jLabelIconLeftLeg.icon")); // NOI18N
		jLabelIconLeftLeg.setText(resourceMap.getString("jLabelIconLeftLeg.text")); // NOI18N
		jLabelIconLeftLeg.setName("jLabelIconLeftLeg"); // NOI18N
		jPanel1.add(jLabelIconLeftLeg);

		LHipYawPitchMinus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		LHipYawPitchMinus.setText(resourceMap.getString("LHipYawPitchMinus.text")); // NOI18N
		LHipYawPitchMinus.setEnabled(false);
		LHipYawPitchMinus.setName("LHipYawPitchMinus"); // NOI18N
		LHipYawPitchMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		LHipYawPitchMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel1.add(LHipYawPitchMinus);

		LHipYawPitch.setMaximum(42);
		LHipYawPitch.setMinimum(-65);
		LHipYawPitch.setValue(0);
		LHipYawPitch.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("LHipYawPitch.border.title"))); // NOI18N
		LHipYawPitch.setEnabled(false);
		LHipYawPitch.setName("LHipYawPitch"); // NOI18N
		LHipYawPitch.setPreferredSize(new java.awt.Dimension(55, 70));
		LHipYawPitch.setRequestFocusEnabled(false);
		LHipYawPitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanel1.add(LHipYawPitch);

		LHipYawPitchPlus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		LHipYawPitchPlus.setText(resourceMap.getString("LHipYawPitchPlus.text")); // NOI18N
		LHipYawPitchPlus.setEnabled(false);
		LHipYawPitchPlus.setName("LHipYawPitchPlus"); // NOI18N
		LHipYawPitchPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		LHipYawPitchPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel1.add(LHipYawPitchPlus);

		LHipRollMinus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		LHipRollMinus.setText(resourceMap.getString("LHipRollMinus.text")); // NOI18N
		LHipRollMinus.setEnabled(false);
		LHipRollMinus.setName("LHipRollMinus"); // NOI18N
		LHipRollMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		LHipRollMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel1.add(LHipRollMinus);

		LHipRoll.setMaximum(45);
		LHipRoll.setMinimum(-21);
		LHipRoll.setValue(0);
		LHipRoll.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("LHipRoll.border.title"))); // NOI18N
		LHipRoll.setEnabled(false);
		LHipRoll.setName("LHipRoll"); // NOI18N
		LHipRoll.setPreferredSize(new java.awt.Dimension(55, 70));
		LHipRoll.setRequestFocusEnabled(false);
		LHipRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanel1.add(LHipRoll);

		LHipRollPlus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		LHipRollPlus.setText(resourceMap.getString("LHipRollPlus.text")); // NOI18N
		LHipRollPlus.setEnabled(false);
		LHipRollPlus.setName("LHipRollPlus"); // NOI18N
		LHipRollPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		LHipRollPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel1.add(LHipRollPlus);

		LHipPitchMinus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		LHipPitchMinus.setText(resourceMap.getString("LHipPitchMinus.text")); // NOI18N
		LHipPitchMinus.setEnabled(false);
		LHipPitchMinus.setName("LHipPitchMinus"); // NOI18N
		LHipPitchMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		LHipPitchMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel1.add(LHipPitchMinus);

		LHipPitch.setMaximum(27);
		LHipPitch.setMinimum(-101);
		LHipPitch.setValue(0);
		LHipPitch.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("LHipPitch.border.title"))); // NOI18N
		LHipPitch.setEnabled(false);
		LHipPitch.setName("LHipPitch"); // NOI18N
		LHipPitch.setPreferredSize(new java.awt.Dimension(55, 70));
		LHipPitch.setRequestFocusEnabled(false);
		LHipPitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanel1.add(LHipPitch);

		LHipPitchPlus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		LHipPitchPlus.setText(resourceMap.getString("LHipPitchPlus.text")); // NOI18N
		LHipPitchPlus.setEnabled(false);
		LHipPitchPlus.setName("LHipPitchPlus"); // NOI18N
		LHipPitchPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		LHipPitchPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel1.add(LHipPitchPlus);

		jPanel2.setName("jPanel2"); // NOI18N

		leftLegPower.setIcon(resourceMap.getIcon("leftLegPower.icon")); // NOI18N
		leftLegPower.setSelected(true);
		leftLegPower.setEnabled(false);
		leftLegPower.setName("leftLegPower"); // NOI18N
		leftLegPower.setPreferredSize(new java.awt.Dimension(30, 30));
		leftLegPower.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				powerButtonPressed(evt);
			}
		});
		jPanel2.add(leftLegPower);

		LKneePitchMinus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		LKneePitchMinus.setText(resourceMap.getString("LKneePitchMinus.text")); // NOI18N
		LKneePitchMinus.setEnabled(false);
		LKneePitchMinus.setName("LKneePitchMinus"); // NOI18N
		LKneePitchMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		LKneePitchMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel2.add(LKneePitchMinus);

		LKneePitch.setMaximum(121);
		LKneePitch.setMinimum(-5);
		LKneePitch.setValue(0);
		LKneePitch.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("LKneePitch.border.title"))); // NOI18N
		LKneePitch.setEnabled(false);
		LKneePitch.setName("LKneePitch"); // NOI18N
		LKneePitch.setPreferredSize(new java.awt.Dimension(55, 70));
		LKneePitch.setRequestFocusEnabled(false);
		LKneePitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanel2.add(LKneePitch);

		LKneePitchPlus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		LKneePitchPlus.setText(resourceMap.getString("LKneePitchPlus.text")); // NOI18N
		LKneePitchPlus.setEnabled(false);
		LKneePitchPlus.setName("LKneePitchPlus"); // NOI18N
		LKneePitchPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		LKneePitchPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel2.add(LKneePitchPlus);

		LAnkleRollMinus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		LAnkleRollMinus.setText(resourceMap.getString("LAnkleRollMinus.text")); // NOI18N
		LAnkleRollMinus.setEnabled(false);
		LAnkleRollMinus.setName("LAnkleRollMinus"); // NOI18N
		LAnkleRollMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		LAnkleRollMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel2.add(LAnkleRollMinus);

		LAnkleRoll.setMaximum(22);
		LAnkleRoll.setMinimum(-44);
		LAnkleRoll.setValue(0);
		LAnkleRoll.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("LAnkleRoll.border.title"))); // NOI18N
		LAnkleRoll.setEnabled(false);
		LAnkleRoll.setName("LAnkleRoll"); // NOI18N
		LAnkleRoll.setPreferredSize(new java.awt.Dimension(55, 70));
		LAnkleRoll.setRequestFocusEnabled(false);
		LAnkleRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanel2.add(LAnkleRoll);

		LAnkleRollPlus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		LAnkleRollPlus.setText(resourceMap.getString("LAnkleRollPlus.text")); // NOI18N
		LAnkleRollPlus.setEnabled(false);
		LAnkleRollPlus.setName("LAnkleRollPlus"); // NOI18N
		LAnkleRollPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		LAnkleRollPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel2.add(LAnkleRollPlus);

		LAnklePitchMinus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		LAnklePitchMinus.setText(resourceMap.getString("LAnklePitchMinus.text")); // NOI18N
		LAnklePitchMinus.setEnabled(false);
		LAnklePitchMinus.setName("LAnklePitchMinus"); // NOI18N
		LAnklePitchMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		LAnklePitchMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel2.add(LAnklePitchMinus);

		LAnklePitch.setMaximum(52);
		LAnklePitch.setMinimum(-68);
		LAnklePitch.setValue(0);
		LAnklePitch.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("LAnklePitch.border.title"))); // NOI18N
		LAnklePitch.setEnabled(false);
		LAnklePitch.setName("LAnklePitch"); // NOI18N
		LAnklePitch.setPreferredSize(new java.awt.Dimension(55, 70));
		LAnklePitch.setRequestFocusEnabled(false);
		LAnklePitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanel2.add(LAnklePitch);

		LAnklePitchPlus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		LAnklePitchPlus.setText(resourceMap.getString("LAnklePitchPlus.text")); // NOI18N
		LAnklePitchPlus.setEnabled(false);
		LAnklePitchPlus.setName("LAnklePitchPlus"); // NOI18N
		LAnklePitchPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		LAnklePitchPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel2.add(LAnklePitchPlus);

		javax.swing.GroupLayout jPanelLeftLegLayout = new javax.swing.GroupLayout(jPanelLeftLeg);
		jPanelLeftLeg.setLayout(jPanelLeftLegLayout);
		jPanelLeftLegLayout.setHorizontalGroup(
				jPanelLeftLegLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
				.addGroup(jPanelLeftLegLayout.createSequentialGroup()
						.addGroup(jPanelLeftLegLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
								.addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, 398, javax.swing.GroupLayout.PREFERRED_SIZE)
								.addComponent(jPanel2, javax.swing.GroupLayout.PREFERRED_SIZE, 383, javax.swing.GroupLayout.PREFERRED_SIZE))
								.addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
		);
		jPanelLeftLegLayout.setVerticalGroup(
				jPanelLeftLegLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
				.addGroup(jPanelLeftLegLayout.createSequentialGroup()
						.addContainerGap()
						.addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
						.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
						.addComponent(jPanel2, javax.swing.GroupLayout.PREFERRED_SIZE, 88, Short.MAX_VALUE))
		);

		jPanelLegs.add(jPanelLeftLeg);

		jPanelRightLeg.setBorder(null);
		jPanelRightLeg.setName("jPanelRightLeg"); // NOI18N
		jPanelRightLeg.setPreferredSize(new java.awt.Dimension(440, 175));

		jPanel3.setName("jPanel3"); // NOI18N

		jLabelIconRightLeg.setIcon(resourceMap.getIcon("jLabelIconRightLeg.icon")); // NOI18N
		jLabelIconRightLeg.setText(resourceMap.getString("jLabelIconRightLeg.text")); // NOI18N
		jLabelIconRightLeg.setName("jLabelIconRightLeg"); // NOI18N
		jPanel3.add(jLabelIconRightLeg);

		RHipYawPitchMinus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		RHipYawPitchMinus.setText(resourceMap.getString("RHipYawPitchMinus.text")); // NOI18N
		RHipYawPitchMinus.setEnabled(false);
		RHipYawPitchMinus.setName("RHipYawPitchMinus"); // NOI18N
		RHipYawPitchMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		RHipYawPitchMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel3.add(RHipYawPitchMinus);

		RHipYawPitch.setMaximum(42);
		RHipYawPitch.setMinimum(-65);
		RHipYawPitch.setValue(0);
		RHipYawPitch.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("RHipYawPitch.border.title"))); // NOI18N
		RHipYawPitch.setEnabled(false);
		RHipYawPitch.setName("RHipYawPitch"); // NOI18N
		RHipYawPitch.setPreferredSize(new java.awt.Dimension(55, 70));
		RHipYawPitch.setRequestFocusEnabled(false);
		RHipYawPitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanel3.add(RHipYawPitch);

		RHipYawPitchPlus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		RHipYawPitchPlus.setText(resourceMap.getString("RHipYawPitchPlus.text")); // NOI18N
		RHipYawPitchPlus.setEnabled(false);
		RHipYawPitchPlus.setName("RHipYawPitchPlus"); // NOI18N
		RHipYawPitchPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		RHipYawPitchPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel3.add(RHipYawPitchPlus);

		RHipRollMinus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		RHipRollMinus.setText(resourceMap.getString("RHipRollMinus.text")); // NOI18N
		RHipRollMinus.setEnabled(false);
		RHipRollMinus.setName("RHipRollMinus"); // NOI18N
		RHipRollMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		RHipRollMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel3.add(RHipRollMinus);

		RHipRoll.setMaximum(45);
		RHipRoll.setMinimum(-21);
		RHipRoll.setValue(0);
		RHipRoll.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("RHipRoll.border.title"))); // NOI18N
		RHipRoll.setEnabled(false);
		RHipRoll.setName("RHipRoll"); // NOI18N
		RHipRoll.setPreferredSize(new java.awt.Dimension(55, 70));
		RHipRoll.setRequestFocusEnabled(false);
		RHipRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanel3.add(RHipRoll);

		RHipRollPlus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		RHipRollPlus.setText(resourceMap.getString("RHipRollPlus.text")); // NOI18N
		RHipRollPlus.setEnabled(false);
		RHipRollPlus.setName("RHipRollPlus"); // NOI18N
		RHipRollPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		RHipRollPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel3.add(RHipRollPlus);

		RHipPitchMinus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		RHipPitchMinus.setText(resourceMap.getString("RHipPitchMinus.text")); // NOI18N
		RHipPitchMinus.setEnabled(false);
		RHipPitchMinus.setName("RHipPitchMinus"); // NOI18N
		RHipPitchMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		RHipPitchMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel3.add(RHipPitchMinus);

		RHipPitch.setMaximum(27);
		RHipPitch.setMinimum(-101);
		RHipPitch.setValue(0);
		RHipPitch.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("RHipPitch.border.title"))); // NOI18N
		RHipPitch.setEnabled(false);
		RHipPitch.setName("RHipPitch"); // NOI18N
		RHipPitch.setPreferredSize(new java.awt.Dimension(55, 70));
		RHipPitch.setRequestFocusEnabled(false);
		RHipPitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanel3.add(RHipPitch);

		RHipPitchPlus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		RHipPitchPlus.setText(resourceMap.getString("RHipPitchPlus.text")); // NOI18N
		RHipPitchPlus.setEnabled(false);
		RHipPitchPlus.setName("RHipPitchPlus"); // NOI18N
		RHipPitchPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		RHipPitchPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel3.add(RHipPitchPlus);

		jPanel4.setName("jPanel4"); // NOI18N

		rightLegPower.setIcon(resourceMap.getIcon("rightLegPower.icon")); // NOI18N
		rightLegPower.setSelected(true);
		rightLegPower.setEnabled(false);
		rightLegPower.setName("rightLegPower"); // NOI18N
		rightLegPower.setPreferredSize(new java.awt.Dimension(30, 30));
		rightLegPower.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				powerButtonPressed(evt);
			}
		});
		jPanel4.add(rightLegPower);

		RKneePitchMinus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		RKneePitchMinus.setText(resourceMap.getString("RKneePitchMinus.text")); // NOI18N
		RKneePitchMinus.setEnabled(false);
		RKneePitchMinus.setName("RKneePitchMinus"); // NOI18N
		RKneePitchMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		RKneePitchMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel4.add(RKneePitchMinus);

		RKneePitch.setMaximum(121);
		RKneePitch.setMinimum(-5);
		RKneePitch.setValue(0);
		RKneePitch.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("RKneePitch.border.title"))); // NOI18N
		RKneePitch.setEnabled(false);
		RKneePitch.setName("RKneePitch"); // NOI18N
		RKneePitch.setPreferredSize(new java.awt.Dimension(55, 70));
		RKneePitch.setRequestFocusEnabled(false);
		RKneePitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanel4.add(RKneePitch);

		RKneePitchPlus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		RKneePitchPlus.setText(resourceMap.getString("RKneePitchPlus.text")); // NOI18N
		RKneePitchPlus.setEnabled(false);
		RKneePitchPlus.setName("RKneePitchPlus"); // NOI18N
		RKneePitchPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		RKneePitchPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel4.add(RKneePitchPlus);

		RAnkleRollMinus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		RAnkleRollMinus.setText(resourceMap.getString("RAnkleRollMinus.text")); // NOI18N
		RAnkleRollMinus.setEnabled(false);
		RAnkleRollMinus.setName("RAnkleRollMinus"); // NOI18N
		RAnkleRollMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		RAnkleRollMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel4.add(RAnkleRollMinus);

		RAnkleRoll.setMaximum(22);
		RAnkleRoll.setMinimum(-44);
		RAnkleRoll.setValue(0);
		RAnkleRoll.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("RAnkleRoll.border.title"))); // NOI18N
		RAnkleRoll.setEnabled(false);
		RAnkleRoll.setName("RAnkleRoll"); // NOI18N
		RAnkleRoll.setPreferredSize(new java.awt.Dimension(55, 70));
		RAnkleRoll.setRequestFocusEnabled(false);
		RAnkleRoll.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanel4.add(RAnkleRoll);

		RAnkleRollPlus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		RAnkleRollPlus.setText(resourceMap.getString("RAnkleRollPlus.text")); // NOI18N
		RAnkleRollPlus.setEnabled(false);
		RAnkleRollPlus.setName("RAnkleRollPlus"); // NOI18N
		RAnkleRollPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		RAnkleRollPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel4.add(RAnkleRollPlus);

		RAnklePitchMinus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		RAnklePitchMinus.setText(resourceMap.getString("RAnklePitchMinus.text")); // NOI18N
		RAnklePitchMinus.setEnabled(false);
		RAnklePitchMinus.setName("RAnklePitchMinus"); // NOI18N
		RAnklePitchMinus.setPreferredSize(new java.awt.Dimension(20, 20));
		RAnklePitchMinus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel4.add(RAnklePitchMinus);

		RAnklePitch.setMaximum(52);
		RAnklePitch.setMinimum(-68);
		RAnklePitch.setValue(0);
		RAnklePitch.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("RAnklePitch.border.title"))); // NOI18N
		RAnklePitch.setEnabled(false);
		RAnklePitch.setName("RAnklePitch"); // NOI18N
		RAnklePitch.setPreferredSize(new java.awt.Dimension(55, 70));
		RAnklePitch.setRequestFocusEnabled(false);
		RAnklePitch.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				sliderReleased(evt);
			}
		});
		jPanel4.add(RAnklePitch);

		RAnklePitchPlus.setFont(resourceMap.getFont("LAnkleRollMinus.font")); // NOI18N
		RAnklePitchPlus.setText(resourceMap.getString("RAnklePitchPlus.text")); // NOI18N
		RAnklePitchPlus.setEnabled(false);
		RAnklePitchPlus.setName("RAnklePitchPlus"); // NOI18N
		RAnklePitchPlus.setPreferredSize(new java.awt.Dimension(20, 20));
		RAnklePitchPlus.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				fineAdjustReleased(evt);
			}
		});
		jPanel4.add(RAnklePitchPlus);

		javax.swing.GroupLayout jPanelRightLegLayout = new javax.swing.GroupLayout(jPanelRightLeg);
		jPanelRightLeg.setLayout(jPanelRightLegLayout);
		jPanelRightLegLayout.setHorizontalGroup(
				jPanelRightLegLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
				.addGroup(jPanelRightLegLayout.createSequentialGroup()
						.addGroup(jPanelRightLegLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
								.addGroup(jPanelRightLegLayout.createSequentialGroup()
										.addGap(12, 12, 12)
										.addComponent(jPanel3, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
										.addComponent(jPanel4, javax.swing.GroupLayout.PREFERRED_SIZE, 403, javax.swing.GroupLayout.PREFERRED_SIZE))
										.addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
		);
		jPanelRightLegLayout.setVerticalGroup(
				jPanelRightLegLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
				.addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanelRightLegLayout.createSequentialGroup()
						.addGap(6, 6, 6)
						.addComponent(jPanel3, javax.swing.GroupLayout.PREFERRED_SIZE, 74, Short.MAX_VALUE)
						.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
						.addComponent(jPanel4, javax.swing.GroupLayout.PREFERRED_SIZE, 90, javax.swing.GroupLayout.PREFERRED_SIZE)
						.addGap(4, 4, 4))
		);

		jPanelLegs.add(jPanelRightLeg);

		jPanelJoints.add(jPanelLegs);

		add(jPanelJoints, new org.netbeans.lib.awtextra.AbsoluteConstraints(0, 62, 828, 410));

		jPanelMovement.setBorder(javax.swing.BorderFactory.createTitledBorder(resourceMap.getString("jPanelMovement.border.title"))); // NOI18N
		jPanelMovement.setName("jPanelMovement"); // NOI18N
		jPanelMovement.setPreferredSize(new java.awt.Dimension(851, 75));

		jLabel1.setText(resourceMap.getString("jLabel1.text")); // NOI18N
		jLabel1.setName("jLabel1"); // NOI18N

		jComboBox1.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Item 1", "Item 2", "Item 3", "Item 4" }));
		jComboBox1.setEnabled(false);
		jComboBox1.setName("jComboBox1"); // NOI18N
		jComboBox1.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				jComboBox1ActionPerformed(evt);
			}
		});

		jButtonRun.setText(resourceMap.getString("jButtonRun.text")); // NOI18N
		jButtonRun.setEnabled(false);
		jButtonRun.setName("jButtonRun"); // NOI18N
		jButtonRun.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				runMovement(evt);
			}
		});

		jButtonSave.setText(resourceMap.getString("jButtonSave.text")); // NOI18N
		jButtonSave.setEnabled(false);
		jButtonSave.setName("jButtonSave"); // NOI18N
		jButtonSave.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				saveMovement(evt);
			}
		});

		jButtonUndo.setText(resourceMap.getString("jButtonUndo.text")); // NOI18N
		jButtonUndo.setEnabled(false);
		jButtonUndo.setName("jButtonUndo"); // NOI18N
		jButtonUndo.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				undoMovement(evt);
			}
		});

		jLabel2.setText(resourceMap.getString("jLabel2.text")); // NOI18N
		jLabel2.setName("jLabel2"); // NOI18N

		jSpinnerSpeed.setModel(new javax.swing.SpinnerNumberModel(Float.valueOf(1.0f), Float.valueOf(0.1f), null, Float.valueOf(0.1f)));
		jSpinnerSpeed.setEnabled(false);
		jSpinnerSpeed.setName("jSpinnerSpeed"); // NOI18N

		jButtonNewMovement.setText(resourceMap.getString("jButtonNewMovement.text")); // NOI18N
		jButtonNewMovement.setEnabled(false);
		jButtonNewMovement.setName("jButtonNewMovement"); // NOI18N
		jButtonNewMovement.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				newMovement(evt);
			}
		});

		jButtonPrev.setIcon(resourceMap.getIcon("jButtonPrev.icon")); // NOI18N
		jButtonPrev.setText(resourceMap.getString("jButtonPrev.text")); // NOI18N
		jButtonPrev.setEnabled(false);
		jButtonPrev.setName("jButtonPrev"); // NOI18N
		jButtonPrev.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				prevMovement(evt);
			}
		});

		jButtonTestMovement.setIcon(resourceMap.getIcon("jButtonTestMovement.icon")); // NOI18N
		jButtonTestMovement.setText(resourceMap.getString("jButtonTestMovement.text")); // NOI18N
		jButtonTestMovement.setEnabled(false);
		jButtonTestMovement.setName("jButtonTestMovement"); // NOI18N
		jButtonTestMovement.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				testMovement(evt);
			}
		});

		jButtonNext.setIcon(resourceMap.getIcon("jButtonNext.icon")); // NOI18N
		jButtonNext.setText(resourceMap.getString("jButtonNext.text")); // NOI18N
		jButtonNext.setEnabled(false);
		jButtonNext.setName("jButtonNext"); // NOI18N
		jButtonNext.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				nextMovement(evt);
			}
		});

		jButtonDeleteMovement.setText(resourceMap.getString("jButtonDeleteMovement.text")); // NOI18N
		jButtonDeleteMovement.setEnabled(false);
		jButtonDeleteMovement.setName("jButtonDeleteMovement"); // NOI18N
		jButtonDeleteMovement.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				deleteMovement(evt);
			}
		});

		jLabelFrame.setText(resourceMap.getString("jLabelFrame.text")); // NOI18N
		jLabelFrame.setName("jLabelFrame"); // NOI18N

		javax.swing.GroupLayout jPanelMovementLayout = new javax.swing.GroupLayout(jPanelMovement);
		jPanelMovement.setLayout(jPanelMovementLayout);
		jPanelMovementLayout.setHorizontalGroup(
				jPanelMovementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
				.addGroup(jPanelMovementLayout.createSequentialGroup()
						.addContainerGap()
						.addComponent(jLabel1)
						.addGap(18, 18, 18)
						.addComponent(jComboBox1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
						.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
						.addComponent(jButtonRun)
						.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
						.addComponent(jButtonSave)
						.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
						.addComponent(jButtonUndo)
						.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
						.addComponent(jLabel2)
						.addGap(18, 18, 18)
						.addComponent(jSpinnerSpeed, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
						.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
						.addComponent(jButtonNewMovement)
						.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
						.addComponent(jButtonPrev)
						.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
						.addComponent(jButtonTestMovement)
						.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
						.addComponent(jButtonNext)
						.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
						.addComponent(jButtonDeleteMovement)
						.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
						.addComponent(jLabelFrame)
						.addContainerGap(174, Short.MAX_VALUE))
		);
		jPanelMovementLayout.setVerticalGroup(
				jPanelMovementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
				.addGroup(jPanelMovementLayout.createSequentialGroup()
						.addGroup(jPanelMovementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
								.addComponent(jLabel1)
								.addComponent(jComboBox1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
								.addComponent(jButtonRun)
								.addComponent(jButtonSave)
								.addComponent(jButtonUndo)
								.addComponent(jLabel2)
								.addComponent(jSpinnerSpeed, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
								.addComponent(jButtonNewMovement)
								.addComponent(jButtonPrev)
								.addComponent(jButtonTestMovement)
								.addComponent(jButtonNext)
								.addComponent(jButtonDeleteMovement)
								.addComponent(jLabelFrame))
								.addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
		);

		add(jPanelMovement, new org.netbeans.lib.awtextra.AbsoluteConstraints(0, 470, 828, 70));

		jPanelLog.setBorder(javax.swing.BorderFactory.createEtchedBorder());
		jPanelLog.setName("jPanelLog"); // NOI18N
		jPanelLog.setPreferredSize(new java.awt.Dimension(851, 54));

		jLabelLog.setBackground(resourceMap.getColor("jLabelLog.background")); // NOI18N
		jLabelLog.setText(resourceMap.getString("jLabelLog.text")); // NOI18N
		jLabelLog.setName("jLabelLog"); // NOI18N

		javax.swing.GroupLayout jPanelLogLayout = new javax.swing.GroupLayout(jPanelLog);
		jPanelLog.setLayout(jPanelLogLayout);
		jPanelLogLayout.setHorizontalGroup(
				jPanelLogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
				.addGroup(jPanelLogLayout.createSequentialGroup()
						.addContainerGap()
						.addComponent(jLabelLog, javax.swing.GroupLayout.DEFAULT_SIZE, 800, Short.MAX_VALUE)
						.addContainerGap())
		);
		jPanelLogLayout.setVerticalGroup(
				jPanelLogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
				.addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanelLogLayout.createSequentialGroup()
						.addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
						.addComponent(jLabelLog)
						.addContainerGap())
		);

		add(jPanelLog, new org.netbeans.lib.awtextra.AbsoluteConstraints(0, 540, 828, 40));
	}// </editor-fold>//GEN-END:initComponents

	private void jComboBox1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jComboBox1ActionPerformed
		if (jComboBox1.isFocusOwner()) {
			loadKick();
		}
	}//GEN-LAST:event_jComboBox1ActionPerformed

	private void newMovement(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_newMovement
		kick.newRow(frame);
		for (Integer i = 0; i < kick.getColumnCount(); i++) {
			kick.setValueAt(new Float((Float) kick.getValueAt(frame - 1, i)), frame, i);
		}
		//saveKickFile(currentFile);
		frame++;
		jButtonPrev.setEnabled(true);

		jLabelFrame.setText("Movement: " + frame + " / " + kick.getRowCount());
		jLabelLog.setText("New movement created at position " + frame);
	}//GEN-LAST:event_newMovement

	private void prevMovement(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_prevMovement
		frame--;
		jLabelFrame.setText("Movement: " + frame + " / " + kick.getRowCount());
		updateSliders(kick, frame - 1);
		if (frame == 1) {
			jButtonPrev.setEnabled(false);
		}
		jButtonNext.setEnabled(true);
	}//GEN-LAST:event_prevMovement

	private void testMovement(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_testMovement
		String command = new String("setBodyAngles%");

		for (int i = 0; i < kick.getColumnCount() - 1; i++) {
			Float f = new Float((Float) kick.getValueAt(frame - 1, i));
			if (i == kick.getColumnCount() - 2) {
				command += String.valueOf(f) + "&";
			} else {
				command += String.valueOf(f) + "$";
			}
		}
		float speed = (Float) kick.getValueAt(frame - 1, kick.getColumnCount() - 1);
		command += String.valueOf(speed + "%");

		// Send the actual joints snapshot
		out.println(command);

		jLabelLog.setText("Movement " + frame.toString() + " has been sent");
	}//GEN-LAST:event_testMovement

	private void nextMovement(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_nextMovement
		frame++;
		jLabelFrame.setText("Movement: " + frame + " / " + kick.getRowCount());
		updateSliders(kick, frame - 1);
		if (frame == kick.getRowCount()) {
			jButtonNext.setEnabled(false);
		}
		jButtonPrev.setEnabled(true);
	}//GEN-LAST:event_nextMovement

	private void deleteMovement(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_deleteMovement
		if (kick.getRowCount() > 1) {
			kick.removeRow(frame - 1);
			//saveKickFile(currentFile);

			jLabelLog.setText("Movement deleted at position " + frame);
			if (frame == kick.getRowCount() + 1) {
				frame--;
			}
			updateSliders(kick, frame - 1);

			jLabelFrame.setText("Movement: " + frame + " / " + kick.getRowCount());
		}
	}//GEN-LAST:event_deleteMovement

	private void runMovement(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_runMovement
		String command = new String("setBodyAngles%");

		//System.out.println("Num frames: " + kick.getRowCount());
		for (int j = 0; j < kick.getRowCount(); j++) {
			for (int i = 0; i < kick.getColumnCount() - 1; i++) {
				Float f = new Float((Float) kick.getValueAt(j, i));
				if (i == kick.getColumnCount() - 2) {
					command += String.valueOf(f) + "&";
				} else {
					command += String.valueOf(f) + "$";
				}
				//System.out.print(" "+f);
			}

			//Speed
			float speed = (Float) kick.getValueAt(j, kick.getColumnCount() - 1);
			command += String.valueOf(speed + "%");
		}

		// Send all the joints to the server
		System.out.println(command);
		out.println(command);


		jLabelLog.setText("Running " + jComboBox1.getSelectedItem() + " in the robot");
	}//GEN-LAST:event_runMovement

	private void saveMovement(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_saveMovement
		for (int i = 0; i < kick.getColumnCount() - 1; i++) {
			kick.setValueAt(new Float(Math.toRadians(((JSlider) (slidersVector.get(i))).getValue())), frame - 1, i);
			//System.out.print(kick.getValueAt(frame - 1, i) + " ");
		}

		//System.out.println();
		try {
			//TODO: cambiar esto
			jSpinnerSpeed.commitEdit();
		} catch (ParseException ex) {
			Logger.getLogger(PoseGenerator.class.getName()).log(Level.SEVERE, null, ex);
		}

		kick.setValueAt(((Float) jSpinnerSpeed.getValue()).floatValue(), frame - 1, kick.getColumnCount() - 1);
		//System.out.println("Valor del spinner: " + (Double)(jSpinnerSpeed.getValue()));

		saveKickFile(currentFile);
		jLabelLog.setText("Movement " + frame + " has been saved");
	}//GEN-LAST:event_saveMovement

	private void undoMovement(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_undoMovement
		state = history.undo();
		kick = state.getKick();
		frame = state.getFrame();
		jComboBox1.setSelectedIndex(state.getComboBoxEntry());
		updateSliders(kick, frame - 1);
		if (history.isEmpty()) {
			jButtonUndo.setEnabled(false);
		}

		jLabelFrame.setText("Movement: " + frame + " / " + kick.getRowCount());
		if (frame == kick.getRowCount()) {
			jButtonNext.setEnabled(false);
		}
		jButtonPrev.setEnabled(true);

		jLabelLog.setText("Undo!");
	}//GEN-LAST:event_undoMovement

	private void jCheckBoxActivateActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jCheckBoxActivateActionPerformed
		if (!connected) {
			connect();
		} else {
			disconnect();
		}
	}//GEN-LAST:event_jCheckBoxActivateActionPerformed

	private void sliderReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_sliderReleased
		if (connected) {
			out.println("setAngle%" + ((JSlider) evt.getSource()).getName() +
					"%" + Math.toRadians(((JSlider) evt.getSource()).getValue()) + "%");
		}
	}//GEN-LAST:event_sliderReleased

	private void centerButtonReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_centerButtonReleased
		if (connected) {
			HeadYaw.setValue(0);
			out.println("setAngle%HeadYaw%0.0%");
			HeadPitch.setValue(0);
			out.println("setAngle%HeadPitch%0.0%");
		}
	}//GEN-LAST:event_centerButtonReleased

	private void fineAdjustReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_fineAdjustReleased
		if (((JButton) evt.getSource()).getName().equals("HeadYawMinus")) {
			HeadYaw.setValue(HeadYaw.getValue() - 1);
			out.println("setAngle%HeadYaw%" + Math.toRadians(HeadYaw.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("HeadYawPlus")) {
			HeadYaw.setValue(HeadYaw.getValue() + 1);
			out.println("setAngle%HeadYaw%" + Math.toRadians(HeadYaw.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("HeadPitchMinus")) {
			HeadPitch.setValue(HeadPitch.getValue() - 1);
			out.println("setAngle%HeadPitch%" + Math.toRadians(HeadPitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("HeadPitchPlus")) {
			HeadPitch.setValue(HeadPitch.getValue() + 1);
			out.println("setAngle%HeadPitch%" + Math.toRadians(HeadPitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LElbowRollMinus")) {
			LElbowRoll.setValue(LElbowRoll.getValue() - 1);
			out.println("setAngle%LElbowRoll%" + Math.toRadians(LElbowRoll.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LElbowRollPlus")) {
			LElbowRoll.setValue(LElbowRoll.getValue() + 1);
			out.println("setAngle%LElbowRoll%" + Math.toRadians(LElbowRoll.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LShoulderPitchMinus")) {
			LShoulderPitch.setValue(LShoulderPitch.getValue() - 1);
			out.println("setAngle%LShoulderPitch%" + Math.toRadians(LShoulderPitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LShoulderPitchPlus")) {
			LShoulderPitch.setValue(LShoulderPitch.getValue() + 1);
			out.println("setAngle%LShoulderPitch%" + Math.toRadians(LShoulderPitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LShoulderRollMinus")) {
			LShoulderRoll.setValue(LShoulderRoll.getValue() - 1);
			out.println("setAngle%LShoulderRoll%" + Math.toRadians(LShoulderRoll.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LShoulderRollPlus")) {
			LShoulderRoll.setValue(LShoulderRoll.getValue() + 1);
			out.println("setAngle%LShoulderRoll%" + Math.toRadians(LShoulderRoll.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LElbowYawMinus")) {
			LElbowYaw.setValue(LElbowYaw.getValue() - 1);
			out.println("setAngle%LElbowYaw%" + Math.toRadians(LElbowYaw.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LElbowYawPlus")) {
			LElbowYaw.setValue(LElbowYaw.getValue() + 1);
			out.println("setAngle%LElbowYaw%" + Math.toRadians(LElbowYaw.getValue()) + "%");

		} else if (((JButton) evt.getSource()).getName().equals("RElbowRollMinus")) {
			RElbowRoll.setValue(RElbowRoll.getValue() - 1);
			out.println("setAngle%RElbowRoll%" + Math.toRadians(RElbowRoll.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RElbowRollPlus")) {
			RElbowRoll.setValue(RElbowRoll.getValue() + 1);
			out.println("setAngle%RElbowRoll%" + Math.toRadians(RElbowRoll.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RShoulderPitchMinus")) {
			RShoulderPitch.setValue(RShoulderPitch.getValue() - 1);
			out.println("setAngle%RShoulderPitch%" + Math.toRadians(RShoulderPitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RShoulderPitchPlus")) {
			RShoulderPitch.setValue(RShoulderPitch.getValue() + 1);
			out.println("setAngle%RShoulderPitch%" + Math.toRadians(RShoulderPitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RShoulderRollMinus")) {
			RShoulderRoll.setValue(RShoulderRoll.getValue() - 1);
			out.println("setAngle%RShoulderRoll%" + Math.toRadians(RShoulderRoll.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RShoulderRollPlus")) {
			RShoulderRoll.setValue(RShoulderRoll.getValue() + 1);
			out.println("setAngle%RShoulderRoll%" + Math.toRadians(RShoulderRoll.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RElbowYawMinus")) {
			RElbowYaw.setValue(RElbowYaw.getValue() - 1);
			out.println("setAngle%RElbowYaw%" + Math.toRadians(RElbowYaw.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RElbowYawPlus")) {
			RElbowYaw.setValue(RElbowYaw.getValue() + 1);
			out.println("setAngle%RElbowYaw%" + Math.toRadians(RElbowYaw.getValue()) + "%");

		} else if (((JButton) evt.getSource()).getName().equals("LHipYawPitchMinus")) {
			LHipYawPitch.setValue(LHipYawPitch.getValue() - 1);
			out.println("setAngle%LHipYawPitch%" + Math.toRadians(LHipYawPitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LHipYawPitchPlus")) {
			LHipYawPitch.setValue(LHipYawPitch.getValue() + 1);
			out.println("setAngle%LHipYawPitch%" + Math.toRadians(LHipYawPitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LHipRollMinus")) {
			LHipRoll.setValue(LHipRoll.getValue() - 1);
			out.println("setAngle%LHipRoll%" + Math.toRadians(LHipRoll.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LHipRollPlus")) {
			LHipRoll.setValue(LHipRoll.getValue() + 1);
			out.println("setAngle%LHipRoll%" + Math.toRadians(LHipRoll.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LHipPitchMinus")) {
			LHipPitch.setValue(LHipPitch.getValue() - 1);
			out.println("setAngle%LHipPitch%" + Math.toRadians(LHipPitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LHipPitchPlus")) {
			LHipPitch.setValue(LHipPitch.getValue() + 1);
			out.println("setAngle%LHipPitch%" + Math.toRadians(LHipPitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LKneePitchMinus")) {
			LKneePitch.setValue(LKneePitch.getValue() - 1);
			out.println("setAngle%LKneePitch%" + Math.toRadians(LKneePitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LKneePitchPlus")) {
			LKneePitch.setValue(LKneePitch.getValue() + 1);
			out.println("setAngle%LKneePitch%" + Math.toRadians(LKneePitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LAnklePitchMinus")) {
			LAnklePitch.setValue(LAnklePitch.getValue() - 1);
			out.println("setAngle%LAnklePitch%" + Math.toRadians(LAnklePitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LAnklePitchPlus")) {
			LAnklePitch.setValue(LAnklePitch.getValue() + 1);
			out.println("setAngle%LAnklePitch%" + Math.toRadians(LAnklePitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LAnkleRollMinus")) {
			LAnkleRoll.setValue(LAnkleRoll.getValue() - 1);
			out.println("setAngle%LAnkleRoll%" + Math.toRadians(LAnkleRoll.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("LAnkleRollPlus")) {
			LAnkleRoll.setValue(LAnkleRoll.getValue() + 1);
			out.println("setAngle%LAnkleRoll%" + Math.toRadians(LAnkleRoll.getValue()) + "%");

		} else if (((JButton) evt.getSource()).getName().equals("RHipYawPitchMinus")) {
			RHipYawPitch.setValue(RHipYawPitch.getValue() - 1);
			out.println("setAngle%RHipYawPitch%" + Math.toRadians(RHipYawPitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RHipYawPitchPlus")) {
			RHipYawPitch.setValue(RHipYawPitch.getValue() + 1);
			out.println("setAngle%RHipYawPitch%" + Math.toRadians(RHipYawPitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RHipRollMinus")) {
			RHipRoll.setValue(RHipRoll.getValue() - 1);
			out.println("setAngle%RHipRoll%" + Math.toRadians(RHipRoll.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RHipRollPlus")) {
			RHipRoll.setValue(RHipRoll.getValue() + 1);
			out.println("setAngle%RHipRoll%" + Math.toRadians(RHipRoll.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RHipPitchMinus")) {
			RHipPitch.setValue(RHipPitch.getValue() - 1);
			out.println("setAngle%RHipPitch%" + Math.toRadians(RHipPitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RHipPitchPlus")) {
			RHipPitch.setValue(RHipPitch.getValue() + 1);
			out.println("setAngle%RHipPitch%" + Math.toRadians(RHipPitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RKneePitchMinus")) {
			RKneePitch.setValue(RKneePitch.getValue() - 1);
			out.println("setAngle%RKneePitch%" + Math.toRadians(RKneePitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RKneePitchPlus")) {
			RKneePitch.setValue(RKneePitch.getValue() + 1);
			out.println("setAngle%RKneePitch%" + Math.toRadians(RKneePitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RAnklePitchMinus")) {
			RAnklePitch.setValue(RAnklePitch.getValue() - 1);
			out.println("setAngle%RAnklePitch%" + Math.toRadians(RAnklePitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RAnklePitchPlus")) {
			RAnklePitch.setValue(RAnklePitch.getValue() + 1);
			out.println("setAngle%RAnklePitch%" + Math.toRadians(RAnklePitch.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RAnkleRollMinus")) {
			RAnkleRoll.setValue(RAnkleRoll.getValue() - 1);
			out.println("setAngle%RAnkleRoll%" + Math.toRadians(RAnkleRoll.getValue()) + "%");
		} else if (((JButton) evt.getSource()).getName().equals("RAnkleRollPlus")) {
			RAnkleRoll.setValue(RAnkleRoll.getValue() + 1);
			out.println("setAngle%RAnkleRoll%" + Math.toRadians(RAnkleRoll.getValue()) + "%");
		}
	}//GEN-LAST:event_fineAdjustReleased

	private void poseInitReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_poseInitReleased
		if (connected) {
			out.println("PoseInit%");

			try {
				Thread.sleep(2);
			} catch (InterruptedException ex) {
				Logger.getLogger(PoseGenerator.class.getName()).log(Level.SEVERE, null, ex);
			}

			synchroSliders();

			// Enable widgets
			for (int i = 0; i < widgetVector.size(); i++) {
				((Component) widgetVector.get(i)).setEnabled(true);
			}

			headPower.setSelected(true);
			leftArmPower.setSelected(true);
			rightArmPower.setSelected(true);
			leftLegPower.setSelected(true);
			rightLegPower.setSelected(true);

			jLabelLog.setText("Pose init sent");
		}
	}//GEN-LAST:event_poseInitReleased

	private void powerButtonPressed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_powerButtonPressed
		if (connected) {
			Vector<Component> sliderChainVector = new Vector<Component>();
			String chain = null;

			if (((JToggleButton) evt.getSource()).getName().equals("headPower")) {
				chain = "head";
				sliderChainVector.add(HeadYaw);
				sliderChainVector.add(HeadPitch);
				sliderChainVector.add(HeadYawMinus);
				sliderChainVector.add(HeadYawPlus);
				sliderChainVector.add(HeadPitchMinus);
				sliderChainVector.add(HeadPitchPlus);

			} else if (((JToggleButton) evt.getSource()).getName().equals("leftArmPower")) {
				chain = "l_arm";
				sliderChainVector.add(LShoulderPitch);
				sliderChainVector.add(LShoulderRoll);
				sliderChainVector.add(LElbowYaw);
				sliderChainVector.add(LElbowRoll);
				sliderChainVector.add(LShoulderPitchMinus);
				sliderChainVector.add(LShoulderPitchPlus);
				sliderChainVector.add(LShoulderRollMinus);
				sliderChainVector.add(LShoulderRollPlus);
				sliderChainVector.add(LElbowYawMinus);
				sliderChainVector.add(LElbowYawPlus);
				sliderChainVector.add(LElbowRollMinus);
				sliderChainVector.add(LElbowRollPlus);
			} else if (((JToggleButton) evt.getSource()).getName().equals("rightArmPower")) {
				chain = "r_arm";
				sliderChainVector.add(RShoulderPitch);
				sliderChainVector.add(RShoulderRoll);
				sliderChainVector.add(RElbowYaw);
				sliderChainVector.add(RElbowRoll);
				sliderChainVector.add(RShoulderPitchMinus);
				sliderChainVector.add(RShoulderPitchPlus);
				sliderChainVector.add(RShoulderRollMinus);
				sliderChainVector.add(RShoulderRollPlus);
				sliderChainVector.add(RElbowYawMinus);
				sliderChainVector.add(RElbowYawPlus);
				sliderChainVector.add(RElbowRollMinus);
				sliderChainVector.add(RElbowRollPlus);
			} else if (((JToggleButton) evt.getSource()).getName().equals("leftLegPower")) {
				chain = "l_leg";
				sliderChainVector.add(LHipYawPitch);
				sliderChainVector.add(LHipRoll);
				sliderChainVector.add(LHipPitch);
				sliderChainVector.add(LKneePitch);
				sliderChainVector.add(LAnklePitch);
				sliderChainVector.add(LAnkleRoll);
				sliderChainVector.add(LHipYawPitchMinus);
				sliderChainVector.add(LHipYawPitchPlus);
				sliderChainVector.add(LHipRollMinus);
				sliderChainVector.add(LHipRollPlus);
				sliderChainVector.add(LHipPitchMinus);
				sliderChainVector.add(LHipPitchPlus);
				sliderChainVector.add(LKneePitchMinus);
				sliderChainVector.add(LKneePitchPlus);
				sliderChainVector.add(LAnklePitchMinus);
				sliderChainVector.add(LAnklePitchPlus);
				sliderChainVector.add(LAnkleRollMinus);
				sliderChainVector.add(LAnkleRollPlus);
			} else if (((JToggleButton) evt.getSource()).getName().equals("rightLegPower")) {
				chain = "r_leg";
				sliderChainVector.add(RHipYawPitch);
				sliderChainVector.add(RHipRoll);
				sliderChainVector.add(RHipPitch);
				sliderChainVector.add(RKneePitch);
				sliderChainVector.add(RAnklePitch);
				sliderChainVector.add(RAnkleRoll);
				sliderChainVector.add(RHipYawPitchMinus);
				sliderChainVector.add(RHipYawPitchPlus);
				sliderChainVector.add(RHipRollMinus);
				sliderChainVector.add(RHipRollPlus);
				sliderChainVector.add(RHipPitchMinus);
				sliderChainVector.add(RHipPitchPlus);
				sliderChainVector.add(RKneePitchMinus);
				sliderChainVector.add(RKneePitchPlus);
				sliderChainVector.add(RAnklePitchMinus);
				sliderChainVector.add(RAnklePitchPlus);
				sliderChainVector.add(RAnkleRollMinus);
				sliderChainVector.add(RAnkleRollPlus);
			}

			if (((JToggleButton) evt.getSource()).isSelected()) {
				out.println(chain + "_stif_on%1.0");
				try {
					Thread.sleep(1);
				} catch (InterruptedException ex) {
					Logger.getLogger(PoseGenerator.class.getName()).log(Level.SEVERE, null, ex);
				}

				((JToggleButton) evt.getSource()).setIcon(
						new ImageIcon("src/jmanager/components/poseGenerator/images/powerGreenIcon.png"));

				synchroSliders();
				for (int i = 0; i < sliderChainVector.size(); i++) {
					((Component) sliderChainVector.get(i)).setEnabled(true);
				}
			} else {
				out.println(chain + "_stif_off%1.0");

				((JToggleButton) evt.getSource()).setIcon(
						new ImageIcon("src/jmanager/components/poseGenerator/images/powerRedIcon.png"));

				for (int i = 0; i < sliderChainVector.size(); i++) {
					((Component) sliderChainVector.get(i)).setEnabled(false);
				}
			}
		}
	}//GEN-LAST:event_powerButtonPressed

	private void generatePressed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_generatePressed
		if (connected) {
			generate();
		}
	}//GEN-LAST:event_generatePressed

	private void generateSymmetricPressed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_generateSymmetricPressed
		if (connected) {
			generateSymmetric();
		}
	}//GEN-LAST:event_generateSymmetricPressed

	private void savePoseAsPressed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_savePoseAsPressed
		if (connected) {
			saveAs();
		}
	}//GEN-LAST:event_savePoseAsPressed

	private void newPose(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_newPose
		if (connected) {
			//Create a file chooser
			final JFileChooser fc = new JFileChooser("src/jmanager/components/poseGenerator/kicks/");
			fc.setDialogTitle("Set the location of the new pose");

			//In response to a button click:
			int returnVal = fc.showSaveDialog(this);

			if (returnVal == JFileChooser.APPROVE_OPTION) {
				File file = fc.getSelectedFile();

				// Extract the extension of the filename
				int dot = file.getName().lastIndexOf('.');
				if (file.getName().substring(dot + 1).equals("kick")) {

					String filename = file.getName().substring(0, dot);

					// Load default kick
					loadKickFile(new File(currentFolder.toString() + "/" + ".default.conf"));
					try {						

						previousIndex = jComboBox1.getSelectedIndex();

						Parser parser = new Parser(currentFile);
						kick = parser.getKick("KICK_0");
						kickVector.addElement(kick);
						frame = 1;
						jLabelFrame.setText("Movement: 1 / " + kick.getRowCount());
						updateSliders(kick, 0);						
					} catch (IOException e) {
						e.printStackTrace();
					}


					Kick newkick = (Kick) kick.clone();
					Kick currentkick = kick;
					newkick.setName(filename);
					kick = newkick;
					//saveKickFile(currentFile);
					saveKickFile(new File(currentFolder.toString() + "/" + file.getName()));
					kick = currentkick;

					jLabelLog.setText(fc.getCurrentDirectory() + "/" + file.getName() + " created");

					loadDefaultKicksFolder();

					// Load default kick
					loadKickFile(new File(currentFolder.toString() + "/" + file.getName()));

					// Select new kick in comboox
					int selIx = -1;					
					for (int i=0; i<jComboBox1.getModel().getSize(); i++) {
						if (filename.equals(jComboBox1.getModel().getElementAt(i))) {
							selIx = i;
							break;
						}
					}					
					if (selIx != -1)
						jComboBox1.setSelectedIndex(selIx);

					try {						
						previousIndex = jComboBox1.getSelectedIndex();

						Parser parser = new Parser(currentFile);
						kick = parser.getKick("KICK_0");
						kickVector.addElement(kick);
						frame = 1;
						jLabelFrame.setText("Movement: 1 / " + kick.getRowCount());
						updateSliders(kick, 0);
						jButtonPrev.setEnabled(false);
						jButtonNext.setEnabled(false);

					} catch (IOException e) {
						e.printStackTrace();
					}

				} else {
					jLabelLog.setText("Incorrect filename (should have .kick extension)");
				}
			}
		}
	}//GEN-LAST:event_newPose
	/**/
	// Variables declaration - do not modify//GEN-BEGIN:variables
	private javax.swing.JSlider HeadPitch;
	private javax.swing.JButton HeadPitchMinus;
	private javax.swing.JButton HeadPitchPlus;
	private javax.swing.JSlider HeadYaw;
	private javax.swing.JButton HeadYawMinus;
	private javax.swing.JButton HeadYawPlus;
	private javax.swing.JSlider LAnklePitch;
	private javax.swing.JButton LAnklePitchMinus;
	private javax.swing.JButton LAnklePitchPlus;
	private javax.swing.JSlider LAnkleRoll;
	private javax.swing.JButton LAnkleRollMinus;
	private javax.swing.JButton LAnkleRollPlus;
	private javax.swing.JSlider LElbowRoll;
	private javax.swing.JButton LElbowRollMinus;
	private javax.swing.JButton LElbowRollPlus;
	private javax.swing.JSlider LElbowYaw;
	private javax.swing.JButton LElbowYawMinus;
	private javax.swing.JButton LElbowYawPlus;
	private javax.swing.JSlider LHipPitch;
	private javax.swing.JButton LHipPitchMinus;
	private javax.swing.JButton LHipPitchPlus;
	private javax.swing.JSlider LHipRoll;
	private javax.swing.JButton LHipRollMinus;
	private javax.swing.JButton LHipRollPlus;
	private javax.swing.JSlider LHipYawPitch;
	private javax.swing.JButton LHipYawPitchMinus;
	private javax.swing.JButton LHipYawPitchPlus;
	private javax.swing.JSlider LKneePitch;
	private javax.swing.JButton LKneePitchMinus;
	private javax.swing.JButton LKneePitchPlus;
	private javax.swing.JSlider LShoulderPitch;
	private javax.swing.JButton LShoulderPitchMinus;
	private javax.swing.JButton LShoulderPitchPlus;
	private javax.swing.JSlider LShoulderRoll;
	private javax.swing.JButton LShoulderRollMinus;
	private javax.swing.JButton LShoulderRollPlus;
	private javax.swing.JSlider RAnklePitch;
	private javax.swing.JButton RAnklePitchMinus;
	private javax.swing.JButton RAnklePitchPlus;
	private javax.swing.JSlider RAnkleRoll;
	private javax.swing.JButton RAnkleRollMinus;
	private javax.swing.JButton RAnkleRollPlus;
	private javax.swing.JSlider RElbowRoll;
	private javax.swing.JButton RElbowRollMinus;
	private javax.swing.JButton RElbowRollPlus;
	private javax.swing.JSlider RElbowYaw;
	private javax.swing.JButton RElbowYawMinus;
	private javax.swing.JButton RElbowYawPlus;
	private javax.swing.JSlider RHipPitch;
	private javax.swing.JButton RHipPitchMinus;
	private javax.swing.JButton RHipPitchPlus;
	private javax.swing.JSlider RHipRoll;
	private javax.swing.JButton RHipRollMinus;
	private javax.swing.JButton RHipRollPlus;
	private javax.swing.JSlider RHipYawPitch;
	private javax.swing.JButton RHipYawPitchMinus;
	private javax.swing.JButton RHipYawPitchPlus;
	private javax.swing.JSlider RKneePitch;
	private javax.swing.JButton RKneePitchMinus;
	private javax.swing.JButton RKneePitchPlus;
	private javax.swing.JSlider RShoulderPitch;
	private javax.swing.JButton RShoulderPitchMinus;
	private javax.swing.JButton RShoulderPitchPlus;
	private javax.swing.JSlider RShoulderRoll;
	private javax.swing.JButton RShoulderRollMinus;
	private javax.swing.JButton RShoulderRollPlus;
	private javax.swing.JToggleButton headPower;
	private javax.swing.JButton jButtonDeleteMovement;
	private javax.swing.JButton jButtonGenerate;
	private javax.swing.JButton jButtonHeadCenter;
	private javax.swing.JButton jButtonNewKick;
	private javax.swing.JButton jButtonNewMovement;
	private javax.swing.JButton jButtonNext;
	private javax.swing.JButton jButtonPoseInit;
	private javax.swing.JButton jButtonPrev;
	private javax.swing.JButton jButtonRun;
	private javax.swing.JButton jButtonSave;
	private javax.swing.JButton jButtonSavePoseAs;
	private javax.swing.JButton jButtonSymmetric;
	private javax.swing.JButton jButtonTestMovement;
	private javax.swing.JButton jButtonUndo;
	private javax.swing.JCheckBox jCheckBoxActivate;
	private javax.swing.JComboBox jComboBox1;
	private javax.swing.JLabel jLabel1;
	private javax.swing.JLabel jLabel2;
	private javax.swing.JLabel jLabelFrame;
	private javax.swing.JLabel jLabelIconHead;
	private javax.swing.JLabel jLabelIconLeftArm;
	private javax.swing.JLabel jLabelIconLeftLeg;
	private javax.swing.JLabel jLabelIconRightArm;
	private javax.swing.JLabel jLabelIconRightLeg;
	private javax.swing.JLabel jLabelLog;
	private javax.swing.JLabel jLabelServerPort;
	private javax.swing.JPanel jPanel1;
	private javax.swing.JPanel jPanel2;
	private javax.swing.JPanel jPanel3;
	private javax.swing.JPanel jPanel4;
	private javax.swing.JPanel jPanelArms;
	private javax.swing.JPanel jPanelCommands;
	private javax.swing.JPanel jPanelHead;
	private javax.swing.JPanel jPanelJoints;
	private javax.swing.JPanel jPanelLeftArms;
	private javax.swing.JPanel jPanelLeftLeg;
	private javax.swing.JPanel jPanelLegs;
	private javax.swing.JPanel jPanelLog;
	private javax.swing.JPanel jPanelMovement;
	private javax.swing.JPanel jPanelRightArms;
	private javax.swing.JPanel jPanelRightLeg;
	private javax.swing.JSpinner jSpinnerSpeed;
	private javax.swing.JTextField jTextFieldServerPort;
	private javax.swing.JToggleButton leftArmPower;
	private javax.swing.JToggleButton leftLegPower;
	private javax.swing.JToggleButton rightArmPower;
	private javax.swing.JToggleButton rightLegPower;
	// End of variables declaration//GEN-END:variables

	/**
	 * Used to connect the module to the dog and to activate teleoperation.
	 * This method must be implemented due to this class inherits from ChaosModule one.
	 *
	 * @param connection ChaosConnection between one dog and ChaosManager
	 */
	public void connect() {

		/*try {
            Runtime.getRuntime().exec("python src/jmanager/components/poseGenerator/server/motionEditor.py");
        } catch (IOException ex) {
            Logger.getLogger(PoseGenerator.class.getName()).log(Level.SEVERE, null, ex);
        }
        try {
            Thread.sleep(3);
        } catch (InterruptedException ex) {
            Logger.getLogger(PoseGenerator.class.getName()).log(Level.SEVERE, null, ex);
        }*/

		try {
			s = new Socket("127.0.0.1", Integer.valueOf(jTextFieldServerPort.getText()));
			out = new PrintWriter(s.getOutputStream(), true);
			in = new BufferedReader(new InputStreamReader(
					s.getInputStream()));

		} catch (Exception e) {
			jLabelLog.setText("Cannot connect");
			return;
		}

		// Enable widgets
		for (int i = 0; i < widgetVector.size(); i++) {
			((Component) widgetVector.get(i)).setEnabled(true);
		}

		if (kick.getRowCount() < 2) {
			jButtonNext.setEnabled(false);
		}

		// Load the current value of the NAO joints and update the sliders
		synchroSliders();

		connected = true;
		jLabelLog.setText("Connected");
		jTextFieldServerPort.setEnabled(false);
	}

	/**
	 * Disconnect a connection from the robot
	 */
	public void disconnect() {
		if (connected) {
			//Close the socket
			out.close();

			try {
				s.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
			jLabelLog.setText("Disconnected");
			connected = false;

			// Disable widgets
			for (int i = 0; i < widgetVector.size(); i++) {
				((Component) widgetVector.get(i)).setEnabled(false);
			}

			jTextFieldServerPort.setEnabled(true);
		}
	}

	private void printResp(dataResp r) {

		//sbyte[] util = new byte[r.size];

		if (r.size < 500) {

			System.out.println("Respuesta: [" + new String(r.data) + "] (" + r.size + ")");
		} else {
			System.out.println("Respuesta: [X] (" + r.size + ")");
		}

	}

	/**
	 * Specifies the default kicks folder and fill the combo widget with all kicks contained in the folder.
	 */
	private void loadDefaultKicksFolder() {
		//System.out.println(System.getProperty("user.dir"));

		currentFolder = new File("src/jmanager/components/poseGenerator/kicks/");
		if (currentFolder.exists()) {
			this.fillKicksCombo(this.currentFolder);
		}
	}

	/**
	 * Fill kicks combo widget with all files ".kick" contained in the kicks directory.
	 *
	 * @param kicksDir Path to the kicks directory
	 */
	private void fillKicksCombo(File kicksDir) {
		//Add all ".kick" files into the ComboBox
		jComboBox1.removeAllItems();
		File[] files = kicksDir.listFiles();

		Vector<String> vector = new Vector<String>();

		for (int i = 0; i < files.length; i++) {
			//String fullName = files[i].toString().toLowerCase();
			String fullName = files[i].toString();
			if (fullName.endsWith(".kick")) {
				StringTokenizer st = new StringTokenizer(fullName, "/");
				String name = new String();
				while (st.hasMoreTokens()) {
					name = st.nextToken();
				}
				StringTokenizer st2 = new StringTokenizer(name, ".");
				vector.add(st2.nextToken());
			}
		}
		Collections.sort(vector);
		Iterator it = vector.iterator();
		while (it.hasNext()) {
			jComboBox1.addItem(it.next());
		}

		if (jComboBox1.getItemCount() > 0) {
			loadKick();
		}
	}

	private void loadKick() {

		loadKickFile(new File(currentFolder.toString() + "/" + jComboBox1.getSelectedItem() + ".kick"));

		try {
			//if (previousIndex != -1)
			//saveHistory();

			previousIndex = jComboBox1.getSelectedIndex();

			Parser parser = new Parser(currentFile);
			kick = parser.getKick("KICK_0");
			kickVector.addElement(kick);
			frame = 1;
			jLabelFrame.setText("Movement: 1 / " + kick.getRowCount());
			updateSliders(kick, 0);
			jButtonPrev.setEnabled(false);

			if (connected) {
				if (kick.getRowCount() > 1) {
					jButtonNext.setEnabled(true);
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Loads an specific kick from a file ".kick"
	 *
	 * @param file the file
	 */
	private void loadKickFile(File file) {
		this.currentFile = file;
		jLabelLog.setText("File " + this.currentFolder.toString() + "/" + jComboBox1.getSelectedItem() + ".kick" + " selected");
	}

	private void saveHistory() {
		history.update(new Snapshot(kick, frame, previousIndex));
		if (!history.isEmpty()) {
			jButtonUndo.setEnabled(true);
		}
	}

	/**
	 * Sends a message to the robot that will reply with all its joints
	 */
	private void synchroSliders() {
		String poses = null;

		// Request the position of all the joints
		out.println("load");
		try {
			poses = in.readLine();
		} catch (IOException ex) {
			Logger.getLogger(PoseGenerator.class.getName()).log(Level.SEVERE, null, ex);
			jLabelLog.setText("An exception has occurred, see console");
		}

		Scanner scanner = new Scanner(poses);
		scanner.useDelimiter("%");

		// Update the value of the sliders
		for (int i = 0; i < slidersVector.size(); i++) {
			String value = scanner.next();
			double jointValue = Math.toDegrees(Double.valueOf(value));
			((JSlider) (slidersVector.get(i))).setValue((int) jointValue);
		}
		scanner.close();
	}

	/**
	 * Given an specific row of a kick object, the method updates all sliders
	 * according to the values found in the row.
	 *
	 * @param row Row of the kick (aka frame)
	 * @param kick The kick object
	 */
	private void updateSliders(Kick kick, int row) {
		//System.out.println("ColumnCount:" + kick.getColumnCount());
		//System.out.println("Actualizando sliders (valor de kick):");
		for (int i = 0; i < kick.getColumnCount() - 1; i++) {
			((JSlider) (slidersVector.get(i))).setValue(new Float(Math.toDegrees((Float) (kick.getValueAt(row, i)))).intValue());


			//System.out.println((Float)kick.getValueAt(row,i));
			//System.out.println(((Float)kick.getValueAt(row,i)).intValue());
		}

		//System.out.println("-----");

		// Velocity Spinner
		jSpinnerSpeed.setValue((Float) kick.getValueAt(row, kick.getColumnCount() - 1));
	}

	/**
	 * Save the actual kick contained in the "kick" object
	 * into the "file" specified
	 *
	 * @param file Destination file
	 */
	public void saveKickFile(File file) {
		try {
			Properties kickProperties = new Properties();

			//Check if file exists
			if (file.exists()) {
				kickProperties.load(new FileInputStream(file));
				//remove all kicks from this file
				String s;
				if ((s = kickProperties.getProperty("KICK_COUNT")) != null) {
					int oldCount = Integer.parseInt(s);
					for (int i = 0; i < oldCount; i++) {
						s = kickProperties.getProperty("KICK_" + i);
						StringTokenizer st = new StringTokenizer(s);
						st.nextToken();//Name
						st.nextToken();//ID
						//New: four values
						st.nextToken();//Xmin
						st.nextToken();//XMax
						st.nextToken();//Ymin
						st.nextToken();//YMax
						st.nextToken();//Dx
						st.nextToken();//Dy
						st.nextToken();//Dth
						int steps = Integer.parseInt(st.nextToken());//Steps
						kickProperties.remove("KICK_" + i);
						for (int j = 0; j < steps; j++) {
							kickProperties.remove("KICK_" + i + "_" + j);
						}
					}
				}
			}

			//System.out.println("1");

			//Introduce the new kicks
			kickProperties.setProperty("KICK_COUNT", "1");

			//New: four arguments added
			kickProperties.setProperty("KICK_" + 0, kick.getName() + " " + kick.getID() + " " + kick.getXmin() + " " + kick.getXMax() + " " + kick.getYmin() + " " + kick.getYMax() + " " + kick.getDx() + " " + kick.getDy() + " " + kick.getDth() + " " + kick.getRowCount());

			//System.out.println("2");

			for (int j = 0; j < kick.getRowCount(); j++) {
				kickProperties.setProperty("KICK_" + 0 + "_" + j, kick.getRow(j));
			}

			//System.out.println("3");

			kickProperties.store(new FileOutputStream(file), "");
		} catch (Exception e) {
			System.out.println(e.toString());
		}
	}

	public void generate() {
		FileWriter outputStream = null;
		File file = null;
		float speed = 0.0f;

		//Create a file chooser
		final JFileChooser fc = new JFileChooser("src/jmanager/components/poseGenerator/kicks/output/");
		fc.setSelectedFile(new File(kick.getName() + ".mov"));
		fc.setDialogTitle("Set the location of the generated movement");

		//In response to a button click:
		int returnVal = fc.showSaveDialog(this);

		if (returnVal == JFileChooser.APPROVE_OPTION) {
			file = fc.getSelectedFile();

			try {
				outputStream = new FileWriter(fc.getCurrentDirectory() + "/" + file.getName());
				System.out.println("A escribir en " + file.getName());
				outputStream.write(kick.getName() + " " + kick.getRowCount() + "\n");
				System.out.println("Nombre: " + kick.getName());
				System.out.println("Filas: " + kick.getRowCount());
				outputStream.write("LShoulderPitch LShoulderRoll LElbowYaw LElbowRoll " +
						"LHipYawPitch LHipRoll LHipPitch LKneePitch LAnklePitch LAnkleRoll " +
						"RHipYawPitch RHipRoll RHipPitch RKneePitch RAnklePitch RAnkleRoll " +
				"RShoulderPitch RShoulderRoll RElbowYaw RElbowRoll\n");

				for (int i = 2; i < kick.getColumnCount() - 1; i++) {
					for (int j = 0; j < kick.getRowCount(); j++) {
						Float f = new Float((Float) kick.getValueAt(j, i));
						outputStream.write(String.valueOf(f) + " ");
					}
					outputStream.write("\n");
				}

				for (int i = 2; i < kick.getColumnCount() - 1; i++) {
					for (int j = 0; j < kick.getRowCount(); j++) {
						//Time
						speed += (Float) kick.getValueAt(j, kick.getColumnCount() - 1);
						outputStream.write(speed + " ");
					}
					speed = 0.0f;
					outputStream.write("\n");
				}

				outputStream.close();
				jLabelLog.setText(fc.getCurrentDirectory() + "/" + file.getName() + " generated");
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	public void saveAs() {
		//Create a file chooser
		final JFileChooser fc = new JFileChooser("src/jmanager/components/poseGenerator/kicks/");
		fc.setDialogTitle("Set the location of the symmetric movement");

		//In response to a button click:
		int returnVal = fc.showSaveDialog(this);

		if (returnVal == JFileChooser.APPROVE_OPTION) {
			File file = fc.getSelectedFile();

			// Extract the extension of the filename
			int dot = file.getName().lastIndexOf('.');
			if (file.getName().substring(dot + 1).equals("kick")) {

				String filename = file.getName().substring(0, dot);

				Kick newkick = (Kick) kick.clone();
				Kick currentkick = kick;
				newkick.setName(filename);
				kick = newkick;
				//saveKickFile(currentFile);
				saveKickFile(new File(currentFolder.toString() + "/" + file.getName()));
				kick = currentkick;

				jLabelLog.setText(fc.getCurrentDirectory() + "/" + file.getName() + " created");

				loadDefaultKicksFolder();

				// Load default kick
				loadKickFile(new File(currentFolder.toString() + "/" + file.getName()));

				// Select new kick in comboox
				int selIx = -1;					
				for (int i=0; i<jComboBox1.getModel().getSize(); i++) {
					if (filename.equals(jComboBox1.getModel().getElementAt(i))) {
						selIx = i;
						break;
					}
				}					
				if (selIx != -1)
					jComboBox1.setSelectedIndex(selIx);

				try {						
					previousIndex = jComboBox1.getSelectedIndex();

					Parser parser = new Parser(currentFile);
					kick = parser.getKick("KICK_0");
					kickVector.addElement(kick);
					frame = 1;
					jLabelFrame.setText("Movement: 1 / " + kick.getRowCount());
					updateSliders(kick, 0);
					jButtonPrev.setEnabled(false);
					if (kick.getRowCount() > 1) {
						jButtonNext.setEnabled(true);
					}

				} catch (IOException e) {
					e.printStackTrace();
				}


			} else {
				jLabelLog.setText("Incorrect filename (should have .kick extension)");
			}
		}
	}

	public void generateSymmetric() {

		File file = null;

		//Create a file chooser
		final JFileChooser fc = new JFileChooser("src/jmanager/components/poseGenerator/kicks/");
		fc.setDialogTitle("Set the location of the symmetric movement");

		//In response to a button click:
		int returnVal = fc.showSaveDialog(this);

		if (returnVal == JFileChooser.APPROVE_OPTION) {
			file = fc.getSelectedFile();

			// Extract the extension of the filename
			int dot = file.getName().lastIndexOf('.');
			if (file.getName().substring(dot + 1).equals("kick")) {

				String filename = file.getName().substring(0, dot);

				Kick newkick = ((Kick) kick).cloneSimetry();
				Kick currentkick = kick;
				newkick.setName(filename);
				kick = newkick;
				//saveKickFile(currentFile);
				saveKickFile(new File(currentFolder.toString() + "/" + file.getName()));
				kick = currentkick;

				jLabelLog.setText(fc.getCurrentDirectory() + "/" + file.getName() + " created");


				loadDefaultKicksFolder();

				// Load default kick
				loadKickFile(new File(currentFolder.toString() + "/" + file.getName()));

				// Select new kick in comboox
				int selIx = -1;					
				for (int i=0; i<jComboBox1.getModel().getSize(); i++) {
					if (filename.equals(jComboBox1.getModel().getElementAt(i))) {
						selIx = i;
						break;
					}
				}					
				if (selIx != -1)
					jComboBox1.setSelectedIndex(selIx);

				try {						
					previousIndex = jComboBox1.getSelectedIndex();

					Parser parser = new Parser(currentFile);
					kick = parser.getKick("KICK_0");
					kickVector.addElement(kick);
					frame = 1;
					jLabelFrame.setText("Movement: 1 / " + kick.getRowCount());
					updateSliders(kick, 0);
					jButtonPrev.setEnabled(false);
					if (kick.getRowCount() > 1) {
						jButtonNext.setEnabled(true);
					}

				} catch (IOException e) {
					e.printStackTrace();
				}
			} else {
				jLabelLog.setText("Incorrect filename (should have .kick extension)");

			}
		}
	}
}
