package jmanager.components.lps;

import java.awt.Component;
import java.awt.Color;

import java.util.Observable;
import java.util.Observer;

import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JTable;
import javax.swing.table.TableCellRenderer;

public class LPSTableGUI extends javax.swing.JScrollPane implements Observer{

	private static final long serialVersionUID = 3060868064260712313L;

	private javax.swing.JTable table;
	private LPS kalmanLps = null;

	public LPSTableGUI(LPS kalmanLps){
		this.kalmanLps = kalmanLps;
		initComponents();
	}

	private void initComponents() {

		table = new javax.swing.JTable();

		table.setModel(new javax.swing.table.DefaultTableModel(
				new Object [][] {
						{null, "Ball"     , 0, 0, 0, 0, 0, "Unknow", 0},
						{null, "P0Net"  , 0, 0, 0, 0, 0, "Unknow", 0},
						{null, "P0LeftPost"  , 0, 0, 0, 0, 0, "Unknow", 0},
						{null, "P0RightPost"  , 0, 0, 0, 0, 0, "Unknow", 0},
						{null, "P1Net", 0, 0, 0, 0, 0, "Unknow", 0},
						{null, "P1LeftPost"  , 0, 0, 0, 0, 0, "Unknow", 0},
						{null, "P1RightPost"  , 0, 0, 0, 0, 0, "Unknow", 0},
						/*{null, "TeamMate1", 0, 0, 0, 0, 0, "Unknow", 0},
						{null, "TeamMate2", 0, 0, 0, 0, 0, "Unknow", 0},
						{null, "TeamMate3", 0, 0, 0, 0, 0, "Unknow", 0},
						{null, "Opponent1", 0, 0, 0, 0, 0, "Unknow", 0},
						{null, "Opponent2", 0, 0, 0, 0, 0, "Unknow", 0},
						{null, "Opponent3", 0, 0, 0, 0, 0, "Unknow", 0},
						{null, "Opponent4", 0, 0, 0, 0, 0, "Unknow", 0},*/
				},
				new String [] {
						"", "Name", "X", "Y", "UncX", "UncY", "Quality", "Reliability", "Time"
				}
		) {

			private static final long serialVersionUID = 5768205307291614003L;

			Class[] types = new Class [] {
					java.lang.Object.class, java.lang.String.class, java.lang.Double.class, java.lang.Double.class, java.lang.Double.class, java.lang.Double.class, java.lang.Double.class, java.lang.String.class, java.lang.Double.class
			};

			boolean[] canEdit = new boolean [] {
					false, false, false, false, false, false, false, false, false
			};

			public Class getColumnClass(int columnIndex) {
				return types [columnIndex];
			}

			public boolean isCellEditable(int rowIndex, int columnIndex) {
				return canEdit [columnIndex];
			}
		});

		table.setRowHeight(20);
		this.setViewportView(table);
		table.getColumnModel().getColumn(0).setResizable(false);
		table.getColumnModel().getColumn(0).setPreferredWidth(20);
		table.getColumnModel().getColumn(0).setCellRenderer(new renderTable());
		table.getColumnModel().getColumn(1).setResizable(false);
		table.getColumnModel().getColumn(2).setResizable(false);
		table.getColumnModel().getColumn(3).setResizable(false);
		table.getColumnModel().getColumn(4).setResizable(false);
		table.getColumnModel().getColumn(5).setResizable(false);
		table.getColumnModel().getColumn(6).setResizable(false);
		table.getColumnModel().getColumn(7).setResizable(false);
		table.getColumnModel().getColumn(8).setResizable(false);


		//INIT ICONS
		table.setValueAt("Ball", 0, 0);
		table.setValueAt("YellowNet", 1, 0);
		table.setValueAt("YLeftPost", 2, 0);
		table.setValueAt("YRightPost", 3, 0);
		table.setValueAt("YellowNet", 4, 0);
		table.setValueAt("YLeftPost", 5, 0);
		table.setValueAt("YRightPost", 6, 0);
		/*table.setValueAt("TeamMate1", 7, 0);
		table.setValueAt("TeamMate2", 8, 0);
		table.setValueAt("TeamMate3", 9, 0);
		table.setValueAt("Opponent1", 10, 0);
		table.setValueAt("Opponent2", 11, 0);
		table.setValueAt("Opponent3", 12, 0);
		table.setValueAt("Opponent4", 13, 0);*/
	}

	public class renderTable implements TableCellRenderer{

		public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column){

			JLabel label = new JLabel();				

			if (value instanceof String){
							
				String s = ((String)value).toString();
				label.setIcon(new ImageIcon( getClass().getResource("images/"+ s +".png")) );

				label.setToolTipText (s);
			}
			return label;
		}
	}

	public void insertTable(LPO lpo){
		int position = lpo.getType();   	
		table.setValueAt(lpo.getX(),            position, 2);
		table.setValueAt(lpo.getY(),	 		position, 3);
		table.setValueAt(lpo.getDx(), 			position, 4);
		table.setValueAt(lpo.getDy(), 			position, 5);
		table.setValueAt(lpo.getQuality(), 		position, 6);
		table.setValueAt(lpo.getReliability(),  position, 7);
		table.setValueAt(lpo.getTime(), 		position, 8);
	}

	public void update(Observable arg0, Object arg1) {
		if (arg0 == kalmanLps) {
			for(int i = 0; i < LPS.NUM_OBJECTS; i++){				
				insertTable(kalmanLps.getLPO(i));
			}
		}
	}
}
