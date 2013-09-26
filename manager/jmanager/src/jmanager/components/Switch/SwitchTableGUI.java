package jmanager.components.Switch;

import java.awt.Component;
import java.awt.Color;
import java.util.Observable;
import java.util.Observer;
import javax.swing.JTable;
import javax.swing.table.DefaultTableCellRenderer;

public class SwitchTableGUI extends javax.swing.JScrollPane implements Observer{

	private javax.swing.JTable table;
	private SharedTeamInfo teamInfo = null;

	public SwitchTableGUI(SharedTeamInfo info){
		this.teamInfo = info;
		initComponents();
	}

	private void initComponents() {

		table = new javax.swing.JTable();
		
		table.setDefaultRenderer( Object.class, new MiRender());

		table.setModel(new javax.swing.table.DefaultTableModel(
				new Object [][] {
						{1, 1, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{1, 2, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{1, 3, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{1, 4, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{2, 1, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{2, 2, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{2, 3, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{2, 4, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{3, 1, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{3, 2, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{3, 3, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{3, 4, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{4, 1, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{4, 2, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{4, 3, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
						{4, 4, 0, 0, 0.0f, "Unknow", 0.0f, 0, "Unknow", 0.0f, 0.0f, 0.0f, 0.0f, "Offline"},
				},
				new String [] {
						"From", "To", "X", "Y", "Th", "Pos Rel", "Ball dist", "Ball time", "Ball Rel",
						"Goalie", "Kicker", "Defender", "Supporter", "Role"
				}
		) {

			Class[] types = new Class [] {
					java.lang.Integer.class, java.lang.Integer.class, java.lang.Integer.class, java.lang.Integer.class, java.lang.Float.class, java.lang.String.class,
					java.lang.Float.class, java.lang.Integer.class, java.lang.String.class, java.lang.Float.class, java.lang.Float.class, java.lang.Float.class, 
                                        java.lang.Float.class, java.lang.String.class
			};

			boolean[] canEdit = new boolean [] {
					false, false, false, false, false, false, false, false, false, false, false, false, false, false
			};

                        @Override
			public Class getColumnClass(int columnIndex) {
				return types [columnIndex];
			}

                        @Override
			public boolean isCellEditable(int rowIndex, int columnIndex) {
				return canEdit [columnIndex];
			}
		});

		table.setRowHeight(20);
		this.setViewportView(table);
		table.getColumnModel().getColumn(0).setResizable(false);
		table.getColumnModel().getColumn(0).setPreferredWidth(70);
		//table.getColumnModel().getColumn(0).setCellRenderer(new renderTable());
		table.getColumnModel().getColumn(1).setResizable(false);
		table.getColumnModel().getColumn(1).setPreferredWidth(30);
		table.getColumnModel().getColumn(2).setResizable(false);
		table.getColumnModel().getColumn(3).setResizable(false);
		table.getColumnModel().getColumn(4).setResizable(false);
                table.getColumnModel().getColumn(7).setPreferredWidth(70);
		table.getColumnModel().getColumn(5).setResizable(false);
          	table.getColumnModel().getColumn(5).setPreferredWidth(100);
		table.getColumnModel().getColumn(6).setResizable(false);
		table.getColumnModel().getColumn(6).setPreferredWidth(120);
		table.getColumnModel().getColumn(7).setResizable(false);
		table.getColumnModel().getColumn(7).setPreferredWidth(120);
		table.getColumnModel().getColumn(8).setResizable(false);
                table.getColumnModel().getColumn(8).setPreferredWidth(100);
		table.getColumnModel().getColumn(9).setResizable(false);
                table.getColumnModel().getColumn(9).setPreferredWidth(120);
		table.getColumnModel().getColumn(10).setResizable(false);
                table.getColumnModel().getColumn(10).setPreferredWidth(120);
		table.getColumnModel().getColumn(11).setResizable(false);
		table.getColumnModel().getColumn(11).setPreferredWidth(120);
		table.getColumnModel().getColumn(12).setResizable(false);	
		table.getColumnModel().getColumn(12).setPreferredWidth(120);
                table.getColumnModel().getColumn(13).setResizable(false);
		table.getColumnModel().getColumn(13).setPreferredWidth(100);
	}

	/*public class renderTable implements TableCellRenderer{

		public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column){

			JLabel label = new JLabel();				

			if (value instanceof String){

				String s = ((String)value).toString();
				//label.setIcon(new ImageIcon( getClass().getResource("images/"+ s +".png")) );

				label.setToolTipText (s);
			}
			return label;
		}
	}*/

	public class MiRender extends DefaultTableCellRenderer
	{
		public Component getTableCellRendererComponent(JTable table,
				Object value,
				boolean isSelected,
				boolean hasFocus,
				int row,
				int column)
		{
                        this.setForeground(Color.black);
			super.getTableCellRendererComponent (table, value, isSelected, hasFocus, row, column);
			if ( (row / 4) % 2 == 0)
			{
				//this.setOpaque(true);
				this.setBackground(Color.LIGHT_GRAY);
				//this.setForeground(Color.YELLOW);
			} else {
                                this.setBackground(Color.WHITE);
                        }

                        if (value instanceof String){

				String s = ((String)value).toString();
                                if ((s.equals("Unknow")) || (s.equals("Penalized")) ||(s.equals("Unreliable")) || (s.equals("Low")))
                                        this.setForeground(Color.RED);
                                else if (s.equals("Medium") || (s.equals("Defender")))
                                        this.setForeground(Color.ORANGE);
                                else if ((s.equals("High")) || (s.equals("Most")) || (s.equals("Kicker")))
                                        this.setForeground(Color.GREEN);
                                else if (s.equals("Supporter"))
                                        this.setForeground(Color.BLUE);                                
			}

                        // Highlight role selected by every robot                       
                        Color color = this.getBackground();
                        if ( ((row == 0) || (row == 5) || (row == 10) || (row == 15)) &&
                             (column == 13))
                                this.setBackground(Color.gray);
                        else
                                this.setBackground(color);
                        

                        return this;
		}
	}

	public void insertTable(RobotOpinion opinion, int from, int to){		
		int position = (from - 1) * SharedTeamInfo.NUM_ROBOTS + to - 1;   
		table.setValueAt(opinion.getObserverRobot(), position, 0);
		table.setValueAt(opinion.getObservedRobot(), position, 1);
		table.setValueAt(opinion.getX(), position, 2);
		table.setValueAt(opinion.getY(), position, 3);
		table.setValueAt(opinion.getTheta(), position, 4);
		table.setValueAt(opinion.getPosReliability(), position, 5);
		table.setValueAt(opinion.getBallDistance(), position, 6);
		table.setValueAt(opinion.getBallTimeSinceLastSeen(), position, 7);
		table.setValueAt(opinion.getBallReliability(), position, 8);
		table.setValueAt(opinion.getUtilityGoalie(), position, 9);
		table.setValueAt(opinion.getUtilityKicker(), position, 10);
		table.setValueAt(opinion.getUtilityDefender(), position, 11);
		table.setValueAt(opinion.getUtilitySupporter(), position, 12);
                switch (opinion.getRole()) {
                    case 0:
                        table.setValueAt("Kicker", position, 13);
                        break;
                    case 1:
                        table.setValueAt("Defender", position, 13);
                        break;
                    case 2:
                        table.setValueAt("Supporter", position, 13);
                        break;
                    case 99:
                        table.setValueAt("Offline", position, 13);
                        break;
                    default:
                        System.out.println("SwitchTableGUI::insertTable() Incorrect role ID received (" +
                                opinion.getRole() + ")\n");
                        break;
                }
	}

	public void update(Observable arg0, Object arg1) {
		if (arg0 == teamInfo) {
			for(int from = 1; from <= SharedTeamInfo.NUM_ROBOTS; from++){
				for (int to = 1; to <= SharedTeamInfo.NUM_ROBOTS; to++) {
					insertTable(teamInfo.getRobotOpinion(from, to), from, to);
				}
			}
		}
	}
}
