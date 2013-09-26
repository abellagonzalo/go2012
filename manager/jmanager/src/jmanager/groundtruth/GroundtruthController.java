/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jmanager.groundtruth;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Vector;
import jmanager.Robot;

/**
 *
 * @author Gonzalo Abella (abellagonzalo@gmail.com)
 */
public class GroundtruthController {

    private GroundtruthPanel view;
    private Vector<Robot> robots;
    private Ice.ObjectPrx object;
    private Ice.Communicator ic = null;

    public GroundtruthController(Ice.Communicator ic, GroundtruthPanel view, Vector<Robot> robots) {
        this.ic = ic;
        this.view = view;
        this.robots = robots;

        // Add listeners
        view.addAddRobotListener(new AddRobotListener());
        view.addRemoveRobotListener(new RemoveRobotListener());
        view.addRefreshListener(new RefreshListener());
        view.addStartListener(new StartListener());
        view.addStopListener(new StopListener());
        view.addTestListener(new TestListener());
    }

    private bica.VisionAdapterPrx getVisionAdapterPrx() {

        bica.VisionAdapterPrx visionAdapter = null;
        try {
            if (object == null) {
                object = ic.stringToProxy("VisionAdapter:default -h "
                        + view.getGroundtruthHost()
                        + " -p "
                        + view.getGroundtruthPort());
            }
            visionAdapter = bica.VisionAdapterPrxHelper.checkedCast(object);


        } catch (Ice.DNSException ex) {
            view.showErrorDialog("No se ha podido resolver el nombre del host "
                    + view.getGroundtruthHost()
                    + ":"
                    + view.getGroundtruthPort());
            object = null;
        } catch (Ice.ConnectFailedException ex) {
            view.showErrorDialog("No se ha podido establecer una conexión con el host "
                    + view.getGroundtruthHost()
                    + ":"
                    + view.getGroundtruthPort());
            object = null;
        } catch (Ice.LocalException ex) {
            ex.printStackTrace();
            view.showErrorDialog("Error desconocido");
            object = null;
        }

        return visionAdapter;
    }

    private class AddRobotListener implements ActionListener {

        public void actionPerformed(ActionEvent ae) {

            try {
                // get VisionAdapterPrx
                bica.VisionAdapterPrx gt = getVisionAdapterPrx();
                if (gt == null) {
                    return;
                }

                // Set pattern Ids
                Vector<Integer> patternIds = new Vector();
                for (int i = 0; i < 12; i++) {
                    patternIds.add(i);
                }

                // Show addRobot Dialog
                Vector<Robot> connectedRobots = new Vector();
                for (int i = 0; i<robots.size(); i++) {
                    Robot robot = robots.get(i);
                    if (robot.getConnection().isConnected()) {
                        connectedRobots.add(robot);
                    }
                }
                Vector newRobot = view.showAddRobotDialog(patternIds, connectedRobots);
                if (newRobot.isEmpty()) return;
                else if (newRobot.get(1) == null) return;

                // Set robot variables
                Integer patternId = (Integer) newRobot.get(0);
                Robot robot = (Robot) newRobot.get(1);
                Boolean ballDetector = (Boolean) newRobot.get(2);
                Boolean localization = (Boolean) newRobot.get(3);

                // Set teamColor
                String teamColor = robot.getConnection().getTeamInfoProviderPrx().getTeamColor();
                bica.BallDetectorManagerPrx ballDetectorPrx = null;
                if (ballDetector) {
                    ballDetectorPrx = robot.getConnection().getBallDetectorManagerPrx();
                }

                // Get localizationPrx
                bica.LocalizationProviderPrx localizationPrx = null;
                if (localization) {
                    localizationPrx = robot.getConnection().getLocalizationProviderPrx();
                }

                // Add robot
                bica.GTRobot gtRobot = new bica.GTRobot(patternId,
                        robot.toString(),
                        teamColor,
                        localizationPrx,
                        ballDetectorPrx);
                gt.addRobot(gtRobot);

                // Refresh table
                view.updateRobotsTable(gt.getRobots());

            } catch (Exception ex) {
                ex.printStackTrace();
                view.showErrorDialog(ex.getMessage());
            }
        }
    }

    private class RemoveRobotListener implements ActionListener {

        public void actionPerformed(ActionEvent ae) {
            try {
                // get VisionAdapterPrx
                bica.VisionAdapterPrx gt = getVisionAdapterPrx();
                if (gt == null) {
                    return;
                }

                bica.GTRobot robot = view.getSelectedRobot();
                gt.removeRobot(robot);

                // Refresh robots table
                view.updateRobotsTable(gt.getRobots());

            } catch (Exception ex) {
                ex.printStackTrace();
                view.showErrorDialog(ex.getMessage());
            }
        }
    }

    private class RefreshListener implements ActionListener {

        public void actionPerformed(ActionEvent ae) {
            try {
                // get VisionAdapterPrx
                bica.VisionAdapterPrx gt = getVisionAdapterPrx();
                if (gt == null) {
                    return;
                }

                // Refresh robots table
                view.updateRobotsTable(gt.getRobots());

            } catch (Exception ex) {
                ex.printStackTrace();
                view.showErrorDialog(ex.getMessage());
            }
        }
    }

    private class StartListener implements ActionListener {

        public void actionPerformed(ActionEvent ae) {
            String frec = "";
            try {
                // get VisionAdapterPrx
                bica.VisionAdapterPrx gt = getVisionAdapterPrx();
                if (gt == null) {
                    return;
                }

                frec = view.showStartDialog();
                if (frec == null) {
                    return;
                }

                gt.start(Integer.parseInt(frec));

                // Refresh table
                view.updateRobotsTable(gt.getRobots());

            } catch (NumberFormatException ex) {
                view.showErrorDialog("Error en el formato del numero.\n\""
                        + frec + "\" no es un número.");
            } catch (Exception ex) {
                ex.printStackTrace();
                view.showErrorDialog(ex.getMessage());
            }
        }
    }

    private class StopListener implements ActionListener {

        public void actionPerformed(ActionEvent ae) {
            try {
                /// get VisionAdapterPrx
                bica.VisionAdapterPrx gt = getVisionAdapterPrx();
                if (gt == null) {
                    return;
                }

                gt.stop();

                // Refresh table
                view.updateRobotsTable(gt.getRobots());

            } catch (Exception ex) {
                ex.printStackTrace();
                view.showErrorDialog(ex.getMessage());
            }
        }
    }

    private class TestListener implements ActionListener {

        public void actionPerformed(ActionEvent ae) {
            try {
                // get VisionAdapterPrx
                bica.VisionAdapterPrx gt = getVisionAdapterPrx();

                if (gt == null) {
                    return;

                } else {
                    // Refresh robots table
                    view.updateRobotsTable(gt.getRobots());

                    // Refresh table
                    view.updateRobotsTable(gt.getRobots());
                    
                    view.showMessageDialog("Se ha conectado correctamente a "
                            + view.getGroundtruthHost()
                            + ":"
                            + view.getGroundtruthPort());
                }

            } catch (Exception ex) {
                ex.printStackTrace();
                view.showErrorDialog(ex.getMessage());
            }
        }
    }
}





