/*
 * JmanagerView.java
 */
package jmanager;

import java.util.logging.Level;
import java.util.logging.Logger;
import org.jdesktop.application.Action;
import org.jdesktop.application.ResourceMap;
import org.jdesktop.application.SingleFrameApplication;
import org.jdesktop.application.FrameView;
import org.jdesktop.application.TaskMonitor;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;
import javax.swing.Timer;
import javax.swing.Icon;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import jmanager.GrDebug.GrPrimitivesAbs;
import jmanager.components.Body;
import jmanager.components.Calibration;
import jmanager.components.GeneticLocalization;
import jmanager.components.Go2gpos;
import jmanager.components.Head;
import jmanager.components.LpsViewer;
import jmanager.components.Movie;
import jmanager.components.PerceptionPanel;
import jmanager.components.SwitchViewer;
//import jmanager.field.GLFieldForm;
import jmanager.components.pruebag2gpos;
import jmanager.components.LogExport;
import jmanager.field.GLFieldPanel;
import javax.swing.filechooser.FileNameExtensionFilter;
import jmanager.components.AleatoryMove;
import jmanager.components.Music;



/**
 * The application's main frame.
 */
public class JmanagerView extends FrameView {

	private float _V = 0.0f, _W = 0.0f, _L = 0.0f;


	private class Component {

		private String id;
		private boolean activated = false;
		private boolean debug = false;
		private boolean _log = false;

		public void setId(String newId) {
			id = newId;
		}

		public String getId() {
			return id;
		}

		public void setActivate(boolean act) {
			activated = act;

			if (activated) {
				JmanagerView.root.getConnection().schedulerPrx.run(id);
			} else {
				JmanagerView.root.getConnection().schedulerPrx.stop(id);
			}
		}

		public boolean getActivate() {
			return activated;
		}

		public void setDebug(boolean deb) {
			debug = deb;
			if (debug) {
				JmanagerView.root.getConnection().debugPrx.debugOn(id);
			} else {
				JmanagerView.root.getConnection().debugPrx.debugOff(id);
			}
		}

		public boolean getDebug() {
			return debug;
		}

		public void setLog(boolean log) {
			_log = log;
			if (_log) {
				JmanagerView.root.getConnection().logProviderPrx.logOn(id);
			} else {
				JmanagerView.root.getConnection().logProviderPrx.logOff(id);
			}
		}

		public boolean getLog() {
			return _log;
		}
	}


	private class ComponentsNameComparator implements Comparator<Component> {

		public ComponentsNameComparator() {
		}

		/**
		 * Método compare.
		 * Compara dos componentes lexicográficamente por su nombre (ID).
		 *
		 * @param compt1
		 * @param compt2
		 * @return -1, 0, ó 1 de acuerdo al método compareTo de java.lang.String.
		 */
		public int compare(Component compt1, Component compt2) {
			return compt1.getId().toLowerCase().compareTo(compt2.getId().toLowerCase());
		}
	}

	
	public static JmanagerView root;
	private boolean componentInited = false;
	private List<Component> componentsList = new ArrayList<Component>();

	/**
	 * Constructor de JmanagerView.
	 * @param app
	 */
	public JmanagerView(SingleFrameApplication app) {
		super(app);
		try {
			// Es una interfaz más agradable que la que viene por defecto.
                        // Esto es mejor cambiarlo en los resources.
			//UIManager.setLookAndFeel("com.sun.java.swing.plaf.nimbus.NimbusLookAndFeel");
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		initComponents();
		root = this;

		// status bar initialization - message timeout, idle icon and busy animation, etc
		ResourceMap resourceMap = getResourceMap();
		int messageTimeout = resourceMap.getInteger("StatusBar.messageTimeout");
		messageTimer = new Timer(messageTimeout, new ActionListener() {

			public void actionPerformed(ActionEvent e) {
				statusMessageLabel.setText("");
			}
		});
		messageTimer.setRepeats(false);
		int busyAnimationRate = resourceMap.getInteger("StatusBar.busyAnimationRate");
		for (int i = 0; i < busyIcons.length; i++) {
			busyIcons[i] = resourceMap.getIcon("StatusBar.busyIcons[" + i + "]");
		}
		busyIconTimer = new Timer(busyAnimationRate, new ActionListener() {

			public void actionPerformed(ActionEvent e) {
				busyIconIndex = (busyIconIndex + 1) % busyIcons.length;
				statusAnimationLabel.setIcon(busyIcons[busyIconIndex]);
			}
		});
		idleIcon = resourceMap.getIcon("StatusBar.idleIcon");
		statusAnimationLabel.setIcon(idleIcon);
		progressBar.setVisible(false);

		// connecting action tasks to status bar via TaskMonitor
		TaskMonitor taskMonitor = new TaskMonitor(getApplication().getContext());
		taskMonitor.addPropertyChangeListener(new java.beans.PropertyChangeListener() {

			public void propertyChange(java.beans.PropertyChangeEvent evt) {
				String propertyName = evt.getPropertyName();
				if ("started".equals(propertyName)) {
					if (!busyIconTimer.isRunning()) {
						statusAnimationLabel.setIcon(busyIcons[0]);
						busyIconIndex = 0;
						busyIconTimer.start();
					}
					progressBar.setVisible(true);
					progressBar.setIndeterminate(true);
				} else if ("done".equals(propertyName)) {
					busyIconTimer.stop();
					statusAnimationLabel.setIcon(idleIcon);
					progressBar.setVisible(false);
					progressBar.setValue(0);
				} else if ("message".equals(propertyName)) {
					String text = (String) (evt.getNewValue());
					statusMessageLabel.setText((text == null) ? "" : text);
					messageTimer.restart();
				} else if ("progress".equals(propertyName)) {
					int value = (Integer) (evt.getNewValue());
					progressBar.setVisible(true);
					progressBar.setIndeterminate(false);
					progressBar.setValue(value);
				}
			}
		});
	}

	@Action
	public void showAboutBox() {
		if (aboutBox == null) {
			JFrame mainFrame = JmanagerApp.getApplication().getMainFrame();
			aboutBox = new JmanagerAboutBox(mainFrame);
			aboutBox.setLocationRelativeTo(mainFrame);
		}
		JmanagerApp.getApplication().show(aboutBox);
	}

	/** This method is called from within the constructor to
	 * initialize the form.
	 * WARNING: Do NOT modify this code. The content of this method is
	 * always regenerated by the Form Editor.
	 */
	@SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        mainPanel = new javax.swing.JPanel();
        HFSMBuilder = new javax.swing.JTabbedPane();
        connectionPanel1 = new jmanager.ConnectionPanel();
        manageRobotSoftPanel1 = new jmanager.ManageRobotSoftPanel();
        poseGenerator2 = new jmanager.components.poseGenerator.PoseGenerator();
        builder = new jmanager.VICODE.Builder();
        jPanel2 = new javax.swing.JPanel();
        ComponentCB = new javax.swing.JComboBox();
        CompActivateCheck = new javax.swing.JCheckBox();
        DebugActivateCheck = new javax.swing.JCheckBox();
        ShowGUIButton = new javax.swing.JButton();
        CalibrateB = new javax.swing.JButton();
        RelativeViewB = new javax.swing.JButton();
        FieldViewB = new javax.swing.JButton();
        startLogButton = new javax.swing.JToggleButton();
        exportLogButton = new javax.swing.JToggleButton();
        logActivateCheck = new javax.swing.JCheckBox();
        jPanel3 = new javax.swing.JPanel();
        FisioterapiaLevelButton = new javax.swing.JButton();
        jLabel1 = new javax.swing.JLabel();
        jLabel2 = new javax.swing.JLabel();
        LenguajeModeradoButton = new javax.swing.JButton();
        LenguajeLeveButton = new javax.swing.JButton();
        MusicoterapiaLeveButton = new javax.swing.JButton();
        FisioterapiaModeradoButton = new javax.swing.JButton();
        VSlider = new javax.swing.JSlider();
        LSlider = new javax.swing.JSlider();
        WSlider = new javax.swing.JSlider();
        jLabel3 = new javax.swing.JLabel();
        jLabel4 = new javax.swing.JLabel();
        jButton1 = new javax.swing.JButton();
        jButtonLK2 = new javax.swing.JButton();
        jButtonRK2 = new javax.swing.JButton();
        jButtonLK5 = new javax.swing.JButton();
        jButtonLK6 = new javax.swing.JButton();
        menuBar = new javax.swing.JMenuBar();
        javax.swing.JMenu fileMenu = new javax.swing.JMenu();
        javax.swing.JMenuItem exitMenuItem = new javax.swing.JMenuItem();
        javax.swing.JMenu helpMenu = new javax.swing.JMenu();
        javax.swing.JMenuItem aboutMenuItem = new javax.swing.JMenuItem();
        statusPanel = new javax.swing.JPanel();
        javax.swing.JSeparator statusPanelSeparator = new javax.swing.JSeparator();
        statusMessageLabel = new javax.swing.JLabel();
        statusAnimationLabel = new javax.swing.JLabel();
        progressBar = new javax.swing.JProgressBar();

        mainPanel.setMinimumSize(new java.awt.Dimension(1024, 640));
        mainPanel.setName("mainPanel"); // NOI18N
        mainPanel.setPreferredSize(new java.awt.Dimension(1024, 640));

        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance().getContext().getResourceMap(JmanagerView.class);
        HFSMBuilder.setToolTipText(resourceMap.getString("MainTabPanel.toolTipText")); // NOI18N
        HFSMBuilder.setName("MainTabPanel"); // NOI18N

        connectionPanel1.setName("connectionPanel1"); // NOI18N
        HFSMBuilder.addTab(resourceMap.getString("connectionPanel1.TabConstraints.tabTitle"), connectionPanel1); // NOI18N

        manageRobotSoftPanel1.setName("manageRobotSoftPanel1"); // NOI18N
        HFSMBuilder.addTab(resourceMap.getString("manageRobotSoftPanel1.TabConstraints.tabTitle"), manageRobotSoftPanel1); // NOI18N

        poseGenerator2.setName("poseGenerator2"); // NOI18N
        HFSMBuilder.addTab(resourceMap.getString("poseGenerator2.TabConstraints.tabTitle"), poseGenerator2); // NOI18N

        builder.setName("builder"); // NOI18N
        HFSMBuilder.addTab(resourceMap.getString("builder.TabConstraints.tabTitle"), builder); // NOI18N

        jPanel2.setName("LuncherPanel"); // NOI18N
        jPanel2.addComponentListener(new java.awt.event.ComponentAdapter() {
            public void componentShown(java.awt.event.ComponentEvent evt) {
                jPanel2ComponentShown(evt);
            }
        });

        ComponentCB.setName("ComponentCB"); // NOI18N
        ComponentCB.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                ComponentCBActionPerformed(evt);
            }
        });

        CompActivateCheck.setText(resourceMap.getString("CompActivateCheck.text")); // NOI18N
        CompActivateCheck.setName("CompActivateCheck"); // NOI18N
        CompActivateCheck.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                CompActivateCheckActionPerformed(evt);
            }
        });

        DebugActivateCheck.setText(resourceMap.getString("DebugActivateCheck.text")); // NOI18N
        DebugActivateCheck.setName("DebugActivateCheck"); // NOI18N
        DebugActivateCheck.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                DebugActivateCheckActionPerformed(evt);
            }
        });

        ShowGUIButton.setText(resourceMap.getString("ShowGUIButton.text")); // NOI18N
        ShowGUIButton.setName("ShowGUIButton"); // NOI18N
        ShowGUIButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                ShowGUIButtonActionPerformed(evt);
            }
        });

        CalibrateB.setText(resourceMap.getString("CalibrateB.text")); // NOI18N
        CalibrateB.setName("CalibrateB"); // NOI18N
        CalibrateB.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                CalibrateBActionPerformed(evt);
            }
        });

        RelativeViewB.setText(resourceMap.getString("RelativeViewB.text")); // NOI18N
        RelativeViewB.setName("RelativeViewB"); // NOI18N
        RelativeViewB.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                RelativeViewBActionPerformed(evt);
            }
        });

        FieldViewB.setText(resourceMap.getString("FieldViewB.text")); // NOI18N
        FieldViewB.setName("FieldViewB"); // NOI18N
        FieldViewB.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                FieldViewBActionPerformed(evt);
            }
        });

        startLogButton.setText(resourceMap.getString("startLogButton.text")); // NOI18N
        startLogButton.setName("startLogButton"); // NOI18N
        startLogButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                startLogButtonActionPerformed(evt);
            }
        });

        exportLogButton.setText(resourceMap.getString("exportLogButton.text")); // NOI18N
        exportLogButton.setName("exportLogButton"); // NOI18N
        exportLogButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                exportLogButtonActionPerformed(evt);
            }
        });

        logActivateCheck.setText(resourceMap.getString("logActivateCheck.text")); // NOI18N
        logActivateCheck.setName("logActivateCheck"); // NOI18N
        logActivateCheck.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                logActivateCheckActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(CalibrateB, javax.swing.GroupLayout.DEFAULT_SIZE, 167, Short.MAX_VALUE)
                    .addComponent(RelativeViewB, javax.swing.GroupLayout.DEFAULT_SIZE, 167, Short.MAX_VALUE)
                    .addComponent(FieldViewB, javax.swing.GroupLayout.DEFAULT_SIZE, 167, Short.MAX_VALUE))
                .addGap(28, 28, 28)
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                        .addComponent(ShowGUIButton, javax.swing.GroupLayout.DEFAULT_SIZE, 132, Short.MAX_VALUE)
                        .addComponent(startLogButton, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(exportLogButton, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                    .addComponent(logActivateCheck)
                    .addComponent(DebugActivateCheck)
                    .addComponent(CompActivateCheck))
                .addContainerGap(803, Short.MAX_VALUE))
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addGap(12, 12, 12)
                .addComponent(ComponentCB, javax.swing.GroupLayout.PREFERRED_SIZE, 178, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(952, Short.MAX_VALUE))
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addGap(29, 29, 29)
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(ComponentCB, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(CompActivateCheck))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(DebugActivateCheck)
                .addGap(9, 9, 9)
                .addComponent(logActivateCheck)
                .addGap(18, 18, 18)
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel2Layout.createSequentialGroup()
                        .addComponent(CalibrateB)
                        .addGap(18, 18, 18)
                        .addComponent(RelativeViewB)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(FieldViewB))
                    .addGroup(jPanel2Layout.createSequentialGroup()
                        .addComponent(ShowGUIButton)
                        .addGap(18, 18, 18)
                        .addComponent(startLogButton)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(exportLogButton)))
                .addContainerGap(448, Short.MAX_VALUE))
        );

        HFSMBuilder.addTab(resourceMap.getString("LuncherPanel.TabConstraints.tabTitle"), jPanel2); // NOI18N

        jPanel3.setName("jPanel3"); // NOI18N

        FisioterapiaLevelButton.setText(resourceMap.getString("FisioterapiaLevelButton.text")); // NOI18N
        FisioterapiaLevelButton.setName("FisioterapiaLevelButton"); // NOI18N
        FisioterapiaLevelButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                FisioterapiaLevelButtonActionPerformed(evt);
            }
        });

        jLabel1.setText(resourceMap.getString("jLabel1.text")); // NOI18N
        jLabel1.setName("jLabel1"); // NOI18N

        jLabel2.setText(resourceMap.getString("jLabel2.text")); // NOI18N
        jLabel2.setName("jLabel2"); // NOI18N

        LenguajeModeradoButton.setText(resourceMap.getString("LenguajeModeradoButton.text")); // NOI18N
        LenguajeModeradoButton.setName("LenguajeModeradoButton"); // NOI18N
        LenguajeModeradoButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                LenguajeModeradoButtonActionPerformed(evt);
            }
        });

        LenguajeLeveButton.setText(resourceMap.getString("LenguajeLeveButton.text")); // NOI18N
        LenguajeLeveButton.setName("LenguajeLeveButton"); // NOI18N
        LenguajeLeveButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                LenguajeLeveButtonActionPerformed(evt);
            }
        });

        MusicoterapiaLeveButton.setText(resourceMap.getString("MusicoterapiaLeveButton.text")); // NOI18N
        MusicoterapiaLeveButton.setName("MusicoterapiaLeveButton"); // NOI18N
        MusicoterapiaLeveButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                MusicoterapiaLeveButtonActionPerformed(evt);
            }
        });

        FisioterapiaModeradoButton.setText(resourceMap.getString("FisioterapiaModeradoButton.text")); // NOI18N
        FisioterapiaModeradoButton.setName("FisioterapiaModeradoButton"); // NOI18N
        FisioterapiaModeradoButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                FisioterapiaModeradoButtonActionPerformed(evt);
            }
        });

        VSlider.setMinimum(-100);
        VSlider.setOrientation(javax.swing.JSlider.VERTICAL);
        VSlider.setPaintTicks(true);
        VSlider.setToolTipText(resourceMap.getString("VSlider.toolTipText")); // NOI18N
        VSlider.setValue(0);
        VSlider.setName("VSlider"); // NOI18N
        VSlider.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseReleased(java.awt.event.MouseEvent evt) {
                VSliderMouseReleased(evt);
            }
        });

        LSlider.setMinimum(-100);
        LSlider.setPaintTicks(true);
        LSlider.setValue(0);
        LSlider.setName("LSlider"); // NOI18N
        LSlider.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseReleased(java.awt.event.MouseEvent evt) {
                LSliderMouseReleased(evt);
            }
        });

        WSlider.setMinimum(-100);
        WSlider.setPaintTicks(true);
        WSlider.setValue(0);
        WSlider.setName("WSlider"); // NOI18N
        WSlider.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseReleased(java.awt.event.MouseEvent evt) {
                WSliderMouseReleased(evt);
            }
        });

        jLabel3.setText(resourceMap.getString("jLabel3.text")); // NOI18N
        jLabel3.setName("jLabel3"); // NOI18N

        jLabel4.setText(resourceMap.getString("jLabel4.text")); // NOI18N
        jLabel4.setName("jLabel4"); // NOI18N

        jButton1.setText(resourceMap.getString("jButton1.text")); // NOI18N
        jButton1.setName("jButton1"); // NOI18N
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton1ActionPerformed(evt);
            }
        });

        jButtonLK2.setText(resourceMap.getString("jButtonLK2.text")); // NOI18N
        jButtonLK2.setName("jButtonLK2"); // NOI18N
        jButtonLK2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonLK2ActionPerformed(evt);
            }
        });

        jButtonRK2.setText(resourceMap.getString("jButtonRK2.text")); // NOI18N
        jButtonRK2.setName("jButtonRK2"); // NOI18N
        jButtonRK2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonRK2ActionPerformed(evt);
            }
        });

        jButtonLK5.setText(resourceMap.getString("jButtonLK5.text")); // NOI18N
        jButtonLK5.setName("jButtonLK5"); // NOI18N
        jButtonLK5.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonLK5ActionPerformed(evt);
            }
        });

        jButtonLK6.setText(resourceMap.getString("jButtonLK6.text")); // NOI18N
        jButtonLK6.setName("jButtonLK6"); // NOI18N
        jButtonLK6.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonLK6ActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel3Layout = new javax.swing.GroupLayout(jPanel3);
        jPanel3.setLayout(jPanel3Layout);
        jPanel3Layout.setHorizontalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel3Layout.createSequentialGroup()
                .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel3Layout.createSequentialGroup()
                        .addGap(42, 42, 42)
                        .addComponent(jLabel1))
                    .addGroup(jPanel3Layout.createSequentialGroup()
                        .addGap(23, 23, 23)
                        .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                            .addComponent(FisioterapiaLevelButton, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(MusicoterapiaLeveButton, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(LenguajeLeveButton, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 141, Short.MAX_VALUE))))
                .addGap(79, 79, 79)
                .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(jLabel2)
                    .addComponent(LenguajeModeradoButton, javax.swing.GroupLayout.DEFAULT_SIZE, 138, Short.MAX_VALUE)
                    .addComponent(FisioterapiaModeradoButton, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 95, Short.MAX_VALUE)
                .addComponent(VSlider, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jLabel4)
                    .addComponent(LSlider, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                        .addGroup(javax.swing.GroupLayout.Alignment.LEADING, jPanel3Layout.createSequentialGroup()
                            .addComponent(jLabel3)
                            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(jButton1, javax.swing.GroupLayout.PREFERRED_SIZE, 89, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addComponent(WSlider, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addGap(417, 417, 417))
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel3Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                    .addComponent(jButtonLK5, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(jButtonLK2, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(jButtonLK6, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(jButtonRK2, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addGap(292, 292, 292))
        );
        jPanel3Layout.setVerticalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel3Layout.createSequentialGroup()
                .addGap(30, 30, 30)
                .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel3Layout.createSequentialGroup()
                        .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(jLabel1)
                            .addComponent(jLabel2))
                        .addGap(29, 29, 29)
                        .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(LenguajeLeveButton, javax.swing.GroupLayout.PREFERRED_SIZE, 39, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(LenguajeModeradoButton, javax.swing.GroupLayout.PREFERRED_SIZE, 40, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addGap(7, 7, 7)
                        .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(MusicoterapiaLeveButton, javax.swing.GroupLayout.PREFERRED_SIZE, 39, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(FisioterapiaModeradoButton, javax.swing.GroupLayout.PREFERRED_SIZE, 39, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(FisioterapiaLevelButton, javax.swing.GroupLayout.PREFERRED_SIZE, 39, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addComponent(VSlider, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addGroup(jPanel3Layout.createSequentialGroup()
                        .addGap(12, 12, 12)
                        .addComponent(jLabel4)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(LSlider, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(18, 18, 18)
                        .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                            .addComponent(jLabel3)
                            .addComponent(jButton1, javax.swing.GroupLayout.PREFERRED_SIZE, 41, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(WSlider, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addGap(55, 55, 55)
                .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButtonLK2)
                    .addComponent(jButtonRK2))
                .addGap(18, 18, 18)
                .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButtonLK5)
                    .addComponent(jButtonLK6))
                .addContainerGap(331, Short.MAX_VALUE))
        );

        HFSMBuilder.addTab(resourceMap.getString("jPanel3.TabConstraints.tabTitle"), jPanel3); // NOI18N

        javax.swing.GroupLayout mainPanelLayout = new javax.swing.GroupLayout(mainPanel);
        mainPanel.setLayout(mainPanelLayout);
        mainPanelLayout.setHorizontalGroup(
            mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(mainPanelLayout.createSequentialGroup()
                .addComponent(HFSMBuilder, javax.swing.GroupLayout.DEFAULT_SIZE, 1154, Short.MAX_VALUE)
                .addContainerGap())
        );
        mainPanelLayout.setVerticalGroup(
            mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(mainPanelLayout.createSequentialGroup()
                .addComponent(HFSMBuilder, javax.swing.GroupLayout.DEFAULT_SIZE, 735, Short.MAX_VALUE)
                .addContainerGap())
        );

        HFSMBuilder.getAccessibleContext().setAccessibleName(resourceMap.getString("MainTabPanel.AccessibleContext.accessibleName")); // NOI18N

        menuBar.setName("menuBar"); // NOI18N

        fileMenu.setText(resourceMap.getString("fileMenu.text")); // NOI18N
        fileMenu.setName("fileMenu"); // NOI18N

        exitMenuItem.setName("exitMenuItem"); // NOI18N
        fileMenu.add(exitMenuItem);

        menuBar.add(fileMenu);

        helpMenu.setText(resourceMap.getString("helpMenu.text")); // NOI18N
        helpMenu.setName("helpMenu"); // NOI18N

        aboutMenuItem.setName("aboutMenuItem"); // NOI18N
        helpMenu.add(aboutMenuItem);

        menuBar.add(helpMenu);

        statusPanel.setName("statusPanel"); // NOI18N

        statusPanelSeparator.setName("statusPanelSeparator"); // NOI18N

        statusMessageLabel.setName("statusMessageLabel"); // NOI18N

        statusAnimationLabel.setHorizontalAlignment(javax.swing.SwingConstants.LEFT);
        statusAnimationLabel.setName("statusAnimationLabel"); // NOI18N

        progressBar.setName("progressBar"); // NOI18N

        javax.swing.GroupLayout statusPanelLayout = new javax.swing.GroupLayout(statusPanel);
        statusPanel.setLayout(statusPanelLayout);
        statusPanelLayout.setHorizontalGroup(
            statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(statusPanelSeparator, javax.swing.GroupLayout.DEFAULT_SIZE, 1166, Short.MAX_VALUE)
            .addGroup(statusPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(statusMessageLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 982, Short.MAX_VALUE)
                .addComponent(progressBar, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(statusAnimationLabel)
                .addContainerGap())
        );
        statusPanelLayout.setVerticalGroup(
            statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(statusPanelLayout.createSequentialGroup()
                .addComponent(statusPanelSeparator, javax.swing.GroupLayout.PREFERRED_SIZE, 2, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addGroup(statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(statusMessageLabel)
                    .addComponent(statusAnimationLabel)
                    .addComponent(progressBar, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(3, 3, 3))
        );

        setComponent(mainPanel);
        setMenuBar(menuBar);
        setStatusBar(statusPanel);
    }// </editor-fold>//GEN-END:initComponents

	private void readComponentsFile() {
		File file = new File("conf/components.conf");

		try {
			BufferedReader bis = new BufferedReader(new FileReader(file));
			String line = null;

			while ((line = bis.readLine()) != null) {
				Component auxcl = new Component();
				auxcl.setId(line);
				componentsList.add(auxcl);
			}

			bis.close();

			//Ordena la lista de componentes por nombre.
			Collections.sort(componentsList, new ComponentsNameComparator());

		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

    private void jPanel2ComponentShown(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_jPanel2ComponentShown
		if (!componentInited) {
			System.out.print("Iniciando Componentes:\n");
			componentInited = true;

			readComponentsFile();

			Iterator iter = componentsList.iterator();
			while (iter.hasNext()) {
				Component aux = (Component) iter.next();
				System.out.println("" + aux.getId());
				ComponentCB.addItem(aux.getId());
			}
			System.out.println();//Separa una línea en la consola.
		}
    }//GEN-LAST:event_jPanel2ComponentShown

    private void FieldViewBActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_FieldViewBActionPerformed
        String title = ((javax.swing.JButton)evt.getSource()).getText();
        javax.swing.JFrame compFrame = new javax.swing.JFrame(title);

        GrPrimitivesAbs grPrimitivesAbs = new GrPrimitivesAbs();
        GLFieldPanel fldv = new GLFieldPanel(grPrimitivesAbs);
        compFrame.add(fldv);
        compFrame.setSize(fldv.getPreferredSize());
        compFrame.setVisible(true);
        compFrame.addWindowListener(fldv);
}//GEN-LAST:event_FieldViewBActionPerformed

    private void RelativeViewBActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_RelativeViewBActionPerformed
        String title = ((javax.swing.JButton)evt.getSource()).getText();
        javax.swing.JFrame compFrame = new javax.swing.JFrame(title);
        LpsViewer lpsv = new LpsViewer();
        lpsv.setFrame(compFrame);
        compFrame.add(lpsv);
        compFrame.setSize(1110, 450);
        compFrame.setVisible(true);
        compFrame.addWindowListener(lpsv);
}//GEN-LAST:event_RelativeViewBActionPerformed

    private void CalibrateBActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_CalibrateBActionPerformed
		javax.swing.JFrame compFrame = new javax.swing.JFrame();
		Calibration calibration = new Calibration();
		compFrame.add(calibration);
		compFrame.setSize(800, 600);
		compFrame.setVisible(true);
}//GEN-LAST:event_CalibrateBActionPerformed

    private void ShowGUIButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_ShowGUIButtonActionPerformed

		Component aux = getSelectedCL();

		if (aux.getId().equals(new String("Body"))) {
			javax.swing.JFrame compFrame = new javax.swing.JFrame("Body");
			Body body = new Body();
			compFrame.add(body);
			compFrame.setSize(800, 600);
			compFrame.setVisible(true);
		}

		if (aux.getId().equals(new String("Movie"))) {
			javax.swing.JFrame compFrame = new javax.swing.JFrame("Movie");
			Movie movie = new Movie();
			compFrame.add(movie);
			compFrame.setSize(800, 600);
			compFrame.setVisible(true);
		}

                if (aux.getId().equals(new String("Music"))) {
			javax.swing.JFrame compFrame = new javax.swing.JFrame("Music");
			Music music = new Music();
			compFrame.add(music);
			compFrame.setSize(300, 200);
			compFrame.setVisible(true);
		}

		if (aux.getId().equals(new String("Head"))) {
			javax.swing.JFrame compFrame = new javax.swing.JFrame("Head");
			Head head = new Head();
			compFrame.add(head);
			compFrame.setSize(250, 400);
			compFrame.setVisible(true);
		}
		if (aux.getId().equals(new String("Perception"))) {
			javax.swing.JFrame compFrame = new javax.swing.JFrame("Perception");
			PerceptionPanel perception = new PerceptionPanel();
			compFrame.add(perception);
			compFrame.setSize(800, 600);
			compFrame.setVisible(true);
		}
		if (aux.getId().equals(new String("pruebag2gpos"))) {
			javax.swing.JFrame compFrame = new javax.swing.JFrame("Pruebag2gpos");
			pruebag2gpos Pruebag2gpos = new pruebag2gpos();
			compFrame.add(Pruebag2gpos);
			compFrame.setSize(800, 600);
			compFrame.setVisible(true);
		}
		if (aux.getId().equals(new String("Calibration"))) {
			javax.swing.JFrame compFrame = new javax.swing.JFrame("Calibration");
			Calibration calibration = new Calibration();
			compFrame.add(calibration);
			compFrame.setSize(800, 600);
			compFrame.setVisible(true);
		}
		if (aux.getId().equals(new String("SharedTeamInfo"))) {
			javax.swing.JFrame compFrame = new javax.swing.JFrame("SharedTeamInfo");
			SwitchViewer switchViewer = new SwitchViewer();
			compFrame.add(switchViewer);
			switchViewer.setFrame(compFrame);
			compFrame.setSize(790, 380);
			compFrame.setTitle("Switch! Team Info");
			compFrame.addWindowListener(switchViewer);
			compFrame.setVisible(true);
		}
		if (aux.getId().equals(new String("GeneticLocalization"))) {
			javax.swing.JFrame compFrame = new javax.swing.JFrame("GeneticLocalization");
			GeneticLocalization glocalization = new GeneticLocalization();
			compFrame.add(glocalization);
			compFrame.setSize(800, 600);
			compFrame.setVisible(true);
		}
		if (aux.getId().equals(new String("Go2gpos"))) {
			javax.swing.JFrame compFrame = new javax.swing.JFrame("Go2gpos");
			Go2gpos go2gpos = new Go2gpos();
			compFrame.add(go2gpos);
			compFrame.setSize(800, 600);
			compFrame.setVisible(true);
		}

                if (aux.getId().equals(new String("AleatoryMove"))) {
                    javax.swing.JFrame compFrame = new javax.swing.JFrame("AleatoryMove");
                    AleatoryMove aleatoryMove = new AleatoryMove();
                    compFrame.add(aleatoryMove);
                    compFrame.setSize(800, 600);
                    compFrame.setVisible(true);

                }
		//Componente PruebaTextSpeech.
//		if (aux.getId().equals(new String("PruebaTextSpeech"))) {
//			javax.swing.JDialog compFrame = new javax.swing.JDialog();
//			PruebaTextSpeech pruebaTextSpeech = new PruebaTextSpeech();
//			compFrame.add(pruebaTextSpeech);
//			compFrame.setSize(800, 600);
//			compFrame.setTitle("Componente PruebaTextSpeech");
//			compFrame.setLocationRelativeTo(null);
//			//compFrame.setModal(true);
//			compFrame.setVisible(true);
//		}

   }//GEN-LAST:event_ShowGUIButtonActionPerformed

    private void DebugActivateCheckActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_DebugActivateCheckActionPerformed
		Component aux = getSelectedCL();
		aux.setDebug(DebugActivateCheck.isSelected());
    }//GEN-LAST:event_DebugActivateCheckActionPerformed

    private void CompActivateCheckActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_CompActivateCheckActionPerformed
		Component aux = getSelectedCL();
		aux.setActivate(CompActivateCheck.isSelected());
    }//GEN-LAST:event_CompActivateCheckActionPerformed

    private void ComponentCBActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_ComponentCBActionPerformed
		Component aux = getSelectedCL();

		DebugActivateCheck.setSelected(aux.getDebug());
		CompActivateCheck.setSelected(aux.getActivate());
                logActivateCheck.setSelected(aux.getLog());
    }//GEN-LAST:event_ComponentCBActionPerformed

    private void LenguajeLeveButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_LenguajeLeveButtonActionPerformed
		JmanagerView.root.getConnection().schedulerPrx.run("Movie");

		try {
			Thread.sleep(500);
		} catch (InterruptedException ex) {
			Logger.getLogger(JmanagerView.class.getName()).log(Level.SEVERE, null, ex);
		}

		JmanagerView.root.getConnection().moviePrx.setFile(new String("/home/nao/movie/lenguajeI.movie"));

		try {
			Thread.sleep(500);
		} catch (InterruptedException ex) {
			Logger.getLogger(JmanagerView.class.getName()).log(Level.SEVERE, null, ex);
		}

		JmanagerView.root.getConnection().moviePrx.playPress(1);

    }//GEN-LAST:event_LenguajeLeveButtonActionPerformed

    private void LenguajeModeradoButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_LenguajeModeradoButtonActionPerformed
		JmanagerView.root.getConnection().schedulerPrx.run("Movie");

		try {
			Thread.sleep(500);
		} catch (InterruptedException ex) {
			Logger.getLogger(JmanagerView.class.getName()).log(Level.SEVERE, null, ex);
		}

		JmanagerView.root.getConnection().moviePrx.setFile(new String("/home/nao/movie/lenguajemoderado.movie"));

		try {
			Thread.sleep(500);
		} catch (InterruptedException ex) {
			Logger.getLogger(JmanagerView.class.getName()).log(Level.SEVERE, null, ex);
		}

		JmanagerView.root.getConnection().moviePrx.playPress(1);

    }//GEN-LAST:event_LenguajeModeradoButtonActionPerformed

    private void MusicoterapiaLeveButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_MusicoterapiaLeveButtonActionPerformed
		JmanagerView.root.getConnection().schedulerPrx.run("Movie");

		try {
			Thread.sleep(500);
		} catch (InterruptedException ex) {
			Logger.getLogger(JmanagerView.class.getName()).log(Level.SEVERE, null, ex);
		}

		JmanagerView.root.getConnection().moviePrx.setFile(new String("/home/nao/movie/musicoterapia.movie"));

		try {
			Thread.sleep(500);
		} catch (InterruptedException ex) {
			Logger.getLogger(JmanagerView.class.getName()).log(Level.SEVERE, null, ex);
		}

		JmanagerView.root.getConnection().moviePrx.playPress(1);
    }//GEN-LAST:event_MusicoterapiaLeveButtonActionPerformed

    private void FisioterapiaModeradoButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_FisioterapiaModeradoButtonActionPerformed
		JmanagerView.root.getConnection().schedulerPrx.run("Movie");

		try {
			Thread.sleep(500);
		} catch (InterruptedException ex) {
			Logger.getLogger(JmanagerView.class.getName()).log(Level.SEVERE, null, ex);
		}

		JmanagerView.root.getConnection().moviePrx.setFile(new String("/home/nao/movie/fisioterapia.movie"));

		try {
			Thread.sleep(500);
		} catch (InterruptedException ex) {
			Logger.getLogger(JmanagerView.class.getName()).log(Level.SEVERE, null, ex);
		}

		JmanagerView.root.getConnection().moviePrx.playPress(1);
    }//GEN-LAST:event_FisioterapiaModeradoButtonActionPerformed

    private void FisioterapiaLevelButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_FisioterapiaLevelButtonActionPerformed
		JmanagerView.root.getConnection().schedulerPrx.run("Movie");

		try {
			Thread.sleep(500);
		} catch (InterruptedException ex) {
			Logger.getLogger(JmanagerView.class.getName()).log(Level.SEVERE, null, ex);
		}

		JmanagerView.root.getConnection().moviePrx.setFile(new String("/home/nao/movie/fisioterapia.movie"));

		try {
			Thread.sleep(500);
		} catch (InterruptedException ex) {
			Logger.getLogger(JmanagerView.class.getName()).log(Level.SEVERE, null, ex);
		}

		JmanagerView.root.getConnection().moviePrx.playPress(1);
    }//GEN-LAST:event_FisioterapiaLevelButtonActionPerformed

    private void VSliderMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_VSliderMouseReleased

		_V = ((float) VSlider.getValue()) / 100.0f;

		updateSpeed();
}//GEN-LAST:event_VSliderMouseReleased

    private void LSliderMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_LSliderMouseReleased

		_L = ((float) LSlider.getValue()) / 100.0f;

		updateSpeed();
    }//GEN-LAST:event_LSliderMouseReleased

    private void WSliderMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_WSliderMouseReleased
		_W = ((float) WSlider.getValue()) / 100.0f;

		updateSpeed();
    }//GEN-LAST:event_WSliderMouseReleased

    private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
		_V = _W = _L = 0.001f;

		updateSpeed();

		VSlider.setValue(0);
		WSlider.setValue(0);
		LSlider.setValue(0);

    }//GEN-LAST:event_jButton1ActionPerformed

    private void jButtonLK2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonLK2ActionPerformed

		JmanagerView.root.getConnection().bodyPrx.doMove("TXTFWSTR");
		//dataResp resp = JmanagerView.root.getConnection().send("Body:doKick:RFOOT:0.0:0.0:0.0:");

    }//GEN-LAST:event_jButtonLK2ActionPerformed

    private void jButtonRK2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonRK2ActionPerformed

		JmanagerView.root.getConnection().bodyPrx.doMove("TXTFWSTL");
		//dataResp resp = JmanagerView.root.getConnection().send("Body:doKick:RFOOT:0.0:0.0:0.0:");

    }//GEN-LAST:event_jButtonRK2ActionPerformed

    private void jButtonLK5ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonLK5ActionPerformed

		JmanagerView.root.getConnection().bodyPrx.doMove("TXTSIDE2RSI");
		//dataResp resp = JmanagerView.root.getConnection().send("Body:doKick:RFOOT:0.0:0.0:0.0:");

    }//GEN-LAST:event_jButtonLK5ActionPerformed

    private void jButtonLK6ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonLK6ActionPerformed

		JmanagerView.root.getConnection().bodyPrx.doMove("TXTSIDE2LSI");
		//dataResp resp = JmanagerView.root.getConnection().send("Body:doKick:RFOOT:0.0:0.0:0.0:");

    }//GEN-LAST:event_jButtonLK6ActionPerformed

	private void logActivateCheckActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_logActivateCheckActionPerformed
		Component aux = getSelectedCL();
		aux.setLog(logActivateCheck.isSelected());
	}//GEN-LAST:event_logActivateCheckActionPerformed

	private void startLogButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_startLogButtonActionPerformed
		if (startLogButton.isSelected()) {
			JFileChooser chooser = new JFileChooser();
			FileNameExtensionFilter filter = new FileNameExtensionFilter(
					"Log files", "log");
			chooser.setFileFilter(filter);
			int returnVal = chooser.showSaveDialog(jPanel2);
			if (returnVal == JFileChooser.APPROVE_OPTION) {
				startLogButton.setText("Stop Log Session");
				JmanagerView.root.getConnection().logProviderPrx.startLog( chooser.getSelectedFile().getAbsolutePath(),
						"Log comment" );
			}
		} else {
			startLogButton.setText("Start Log Session");
			JmanagerView.root.getConnection().logProviderPrx.stopLog();
		}
	}//GEN-LAST:event_startLogButtonActionPerformed

	private void exportLogButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_exportLogButtonActionPerformed
		// Choose the log file
		JFileChooser chooser = new JFileChooser();
		FileNameExtensionFilter filter = new FileNameExtensionFilter("Log files", "log");
		chooser.setFileFilter(filter);
		int returnVal = chooser.showOpenDialog(jPanel2);
		
		if (returnVal == JFileChooser.APPROVE_OPTION) {
			javax.swing.JFrame compFrame = new javax.swing.JFrame();
			LogExport logExport = new LogExport(chooser.getSelectedFile().getAbsolutePath());
			compFrame.add(logExport);
			compFrame.setSize(415, 325);
			compFrame.setLocationRelativeTo(null);
			compFrame.setVisible(true);
		}
	}//GEN-LAST:event_exportLogButtonActionPerformed

        private void jButton2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton2ActionPerformed

        }//GEN-LAST:event_jButton2ActionPerformed

	private void updateSpeed() {
		JmanagerView.root.getConnection().bodyPrx.setVel(_V, _W, _L);
		//dataResp resp = JmanagerView.root.getConnection().send("Body:setVel:"+_V+":"+_W+":"+_L+":");
	}

	private Component getSelectedCL() {
		Component ret = null;
		String selected = null;

		if (ComponentCB.getItemCount() > 0) {
			selected = (String) ComponentCB.getItemAt(ComponentCB.getSelectedIndex());
		}

		if (selected != null) {

			Iterator iter = componentsList.iterator();
			while (iter.hasNext()) {
				Component aux = (Component) iter.next();
				if (aux.getId().equals(selected)) {
					ret = aux;
				}
			}
		}
		return ret;
	}
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton CalibrateB;
    private javax.swing.JCheckBox CompActivateCheck;
    private javax.swing.JComboBox ComponentCB;
    private javax.swing.JCheckBox DebugActivateCheck;
    private javax.swing.JButton FieldViewB;
    private javax.swing.JButton FisioterapiaLevelButton;
    private javax.swing.JButton FisioterapiaModeradoButton;
    private javax.swing.JTabbedPane HFSMBuilder;
    private javax.swing.JSlider LSlider;
    private javax.swing.JButton LenguajeLeveButton;
    private javax.swing.JButton LenguajeModeradoButton;
    private javax.swing.JButton MusicoterapiaLeveButton;
    private javax.swing.JButton RelativeViewB;
    private javax.swing.JButton ShowGUIButton;
    private javax.swing.JSlider VSlider;
    private javax.swing.JSlider WSlider;
    private jmanager.VICODE.Builder builder;
    private jmanager.ConnectionPanel connectionPanel1;
    private javax.swing.JToggleButton exportLogButton;
    private javax.swing.JButton jButton1;
    private javax.swing.JButton jButtonLK2;
    private javax.swing.JButton jButtonLK5;
    private javax.swing.JButton jButtonLK6;
    private javax.swing.JButton jButtonRK2;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JCheckBox logActivateCheck;
    private javax.swing.JPanel mainPanel;
    private jmanager.ManageRobotSoftPanel manageRobotSoftPanel1;
    private javax.swing.JMenuBar menuBar;
    private jmanager.components.poseGenerator.PoseGenerator poseGenerator2;
    private javax.swing.JProgressBar progressBar;
    private javax.swing.JToggleButton startLogButton;
    private javax.swing.JLabel statusAnimationLabel;
    private javax.swing.JLabel statusMessageLabel;
    private javax.swing.JPanel statusPanel;
    // End of variables declaration//GEN-END:variables
	private final Timer messageTimer;
	private final Timer busyIconTimer;
	private final Icon idleIcon;
	private final Icon[] busyIcons = new Icon[15];
	private int busyIconIndex = 0;
	private JDialog aboutBox;

	public Connection getConnection() {
		return connectionPanel1.getConnection();
	}

	public ConnectionPanel getConnectionPanel() {
		return connectionPanel1;
	}


}
