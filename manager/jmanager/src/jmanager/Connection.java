/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jmanager;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author paco
 */
public class Connection {

    Socket sock = null;
    PrintWriter out;
    //BufferedReader in;
    BufferedInputStream in;
    String respuesta;
    boolean connected;
    private final int MSGMAXSIZE = (160 * 120 * 3) + 128;
    public String currentHost;
    private int currentPort;
    // Ice
    // Estas variables deberían de hacerse private y sólo poder acceder
    // a ellas por medio de los métodos. Para que siga siendo compatible con
    // todo el código anterior las dejamos public.
    int status = 0;
    private Ice.Communicator ic = null;
    public Ice.ObjectPrx base;
    public bica.SchedulerManagerPrx schedulerPrx;
    public bica.ImageSelectorPrx perceptionPrx;
    public bica.ComponentInfoProviderPrx componentPrx;
    public bica.CalibrationProviderPrx calibrationPrx;
    public bica.MovieManagerPrx moviePrx;
    public bica.LocalizationProviderPrx localizationPrx;
    public bica.GHeadManagerPrx gheadPrx;
    //public bica.pruebag2gposManagerPrx pruebag2gposPrx;
    public bica.Go2posManagerPrx go2gposPrx;
    public bica.Go2posManagerPrx go2gposObsPrx;
    public bica.Go2posManagerPrx go2posPrx;
    public bica.HeadMotionManagerPrx headPrx;
    public bica.MusicManagerPrx musicPrx;
    public bica.TextSpeechManagerPrx speechPrx;
    public bica.WorldModelProviderPrx worldModelPrx;
    public bica.TeamInfoProviderPrx teamInfoPrx, teamInfo1Prx = null, teamInfo2Prx = null, teamInfo3Prx = null, teamInfo4Prx = null;
    public bica.BodyMotionManagerPrx bodyPrx;
    public bica.GrDebugProviderPrx debugPrx;
    public bica.BallDetectorManagerPrx ballDetectorPrx;
    public bica.GoalDetectorManagerPrx goalDetectorPrx;
    public bica.LogProviderPrx logProviderPrx;
    // private bica.AleatoryMovePrx aleatoryMovePrx;
// 	public bica.CalibrationBallManagerPrx calibrationballPrx;

//	public bica.PruebaTextSpeechManagerPrx pruebaTextSpeechManagerPrx;
    public Connection() {
        ic = Ice.Util.initialize();
        connected = false;
    }

    public bica.SchedulerManagerPrx getSchedulerManagerPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (schedulerPrx == null) {
            try {
                objectPrx = ic.stringToProxy("SchedulerManager:default -h " + currentHost + " -p " + currentPort);
                schedulerPrx = bica.SchedulerManagerPrxHelper.checkedCast(objectPrx);

            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());
            }
        }
        return schedulerPrx;
    }

    public bica.ImageSelectorPrx getImageSelectorPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (perceptionPrx == null) {
            try {
                objectPrx = ic.stringToProxy("ImageProvider:default -h " + currentHost + " -p " + currentPort);
                perceptionPrx = bica.ImageSelectorPrxHelper.checkedCast(objectPrx);

            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());

            }
        }
        return perceptionPrx;
    }

    public bica.ComponentInfoProviderPrx getComponentInfoProviderPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (componentPrx == null) {
            try {
                objectPrx = ic.stringToProxy("ComponentInfoProvider:default -h " + currentHost + " -p " + currentPort);
                componentPrx = bica.ComponentInfoProviderPrxHelper.checkedCast(objectPrx);

            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());

            }
        }
        return componentPrx;
    }

    public bica.CalibrationProviderPrx getCalibrationProviderPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (calibrationPrx == null) {
            try {
                objectPrx = ic.stringToProxy("CalibrationProvider:default -h " + currentHost + " -p " + currentPort);
                calibrationPrx = bica.CalibrationProviderPrxHelper.checkedCast(objectPrx);

            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());

            }
        }
        return calibrationPrx;
    }

    public bica.MovieManagerPrx getMovieManagerPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (moviePrx == null) {
            try {
                objectPrx = ic.stringToProxy("MovieManager:default -h " + currentHost + " -p " + currentPort);
                moviePrx = bica.MovieManagerPrxHelper.checkedCast(objectPrx);

            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());

            }
        }
        return moviePrx;
    }

    public bica.LocalizationProviderPrx getLocalizationProviderPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (localizationPrx == null) {
            try {
                objectPrx = ic.stringToProxy("LocalizationProvider:default -h " + currentHost + " -p " + currentPort);
                localizationPrx = bica.LocalizationProviderPrxHelper.checkedCast(objectPrx);

            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());

            }
        }
        return localizationPrx;
    }

    /*
    public bica.GHeadManagerPrx getGHeadManagerPrx() throws RuntimeException {
    Ice.ObjectPrx objectPrx = null;

    if (gheadPrx == null) {
    try {
    objectPrx = ic.stringToProxy("GHeadProvider:default -h " + currentHost + " -p " + currentPort);
    gheadPrx = bica.GHeadManagerPrxHelper.checkedCast(objectPrx);

    } catch (Ice.LocalException ex) {
    ex.printStackTrace();
    throw new RuntimeException(ex.ice_name());

    }
    }
    return gheadPrx;
    }
     * */

    /*
    public bica.pruebag2gposManagerPrx getPruebag2gposManagerPrx() throws RuntimeException {
    Ice.ObjectPrx objectPrx = null;

    if (pruebag2gposPrx == null) {
    try {
    objectPrx = ic.stringToProxy("pruebag2gposManager:default -h " + currentHost + " -p " + currentPort);
    pruebag2gposPrx = bica.pruebag2gposManagerPrxHelper.checkedCast(objectPrx);

    } catch (Ice.LocalException ex) {
    ex.printStackTrace();
    throw new RuntimeException(ex.ice_name());

    }
    }
    return pruebag2gposPrx;
    }*/
    public bica.Go2posManagerPrx getGo2gposManagerPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (go2gposPrx == null) {
            try {
                objectPrx = ic.stringToProxy("Go2gposManager:default -h " + currentHost + " -p " + currentPort);
                go2gposPrx = bica.Go2posManagerPrxHelper.checkedCast(objectPrx);

            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());
            }
        }
        return go2gposPrx;
    }

    /*
    public bica.Go2posManagerPrx getGo2posManagerPrx() throws RuntimeException {
    Ice.ObjectPrx objectPrx = null;

    if (go2gposPrx == null) {
    try {
    objectPrx = ic.stringToProxy("Go2gposManager:default -h " + currentHost + " -p " + currentPort);
    go2gposPrx = bica.Go2posManagerPrxHelper.checkedCast(objectPrx);

    } catch (Ice.LocalException ex) {
    ex.printStackTrace();
    throw new RuntimeException(ex.ice_name());

    }
    }
    return go2gposPrx;
    }
     */
    public bica.Go2posManagerPrx getGo2posManagerObsPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (go2gposObsPrx == null) {
            try {
                objectPrx = ic.stringToProxy("Go2gposObsManager:default -h " + currentHost + " -p " + currentPort);
                go2gposObsPrx = bica.Go2posManagerPrxHelper.checkedCast(objectPrx);

            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());

            }
        }
        return go2gposObsPrx;
    }

    public bica.Go2posManagerPrx getGo2posManagerPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (go2posPrx == null) {
            try {
                objectPrx = ic.stringToProxy("Go2posManager:default -h " + currentHost + " -p " + currentPort);
                go2posPrx = bica.Go2posManagerPrxHelper.checkedCast(objectPrx);

            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());

            }
        }
        return go2posPrx;
    }

    public bica.HeadMotionManagerPrx getHeadMotionMangerPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (headPrx == null) {
            try {
                objectPrx = ic.stringToProxy("HeadMotionManager:default -h " + currentHost + " -p " + currentPort);
                headPrx = bica.HeadMotionManagerPrxHelper.checkedCast(objectPrx);

            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());

            }
        }
        return headPrx;
    }

    public bica.MusicManagerPrx getMusicManagerPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (musicPrx == null) {
            try {
                objectPrx = ic.stringToProxy("MusicManager:default -h " + currentHost + " -p " + currentPort);
                musicPrx = bica.MusicManagerPrxHelper.checkedCast(objectPrx);
            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());
            }
        }
        return musicPrx;
    }

    public bica.TextSpeechManagerPrx getTextSpeechManagerPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (speechPrx == null) {
            try {
                objectPrx = ic.stringToProxy("TextSpeechManager:default -h " + currentHost + " -p " + currentPort);
                speechPrx = bica.TextSpeechManagerPrxHelper.checkedCast(objectPrx);
            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());
            }
        }
        return speechPrx;
    }

    public bica.WorldModelProviderPrx getWorldModelProviderPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (worldModelPrx == null) {
            try {
                objectPrx = ic.stringToProxy("WorldModelProvider:default -h " + currentHost + " -p " + currentPort);
                worldModelPrx = bica.WorldModelProviderPrxHelper.checkedCast(objectPrx);
            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());
            }
        }
        return worldModelPrx;
    }

    public bica.BodyMotionManagerPrx getBodyMotionManager() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (bodyPrx == null) {
            try {
                objectPrx = ic.stringToProxy("BodyMotionManager:default -h " + currentHost + " -p " + currentPort);
                bodyPrx = bica.BodyMotionManagerPrxHelper.checkedCast(objectPrx);
            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());
            }
        }
        return bodyPrx;
    }

    public bica.TeamInfoProviderPrx getTeamInfoProviderPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (teamInfoPrx == null) {
            try {
                objectPrx = ic.stringToProxy("TeamInfoProvider:default -h " + currentHost + " -p " + currentPort);
                teamInfoPrx = bica.TeamInfoProviderPrxHelper.checkedCast(objectPrx);
            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());
            }
        }
        return teamInfoPrx;
    }

    public bica.GrDebugProviderPrx getGrDebugProviderPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (debugPrx == null) {
            try {
                objectPrx = ic.stringToProxy("GrDebugProvider:default -h " + currentHost + " -p " + currentPort);
                debugPrx = bica.GrDebugProviderPrxHelper.checkedCast(objectPrx);
            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());
            }
        }
        return debugPrx;
    }

    public bica.BallDetectorManagerPrx getBallDetectorManagerPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (ballDetectorPrx == null) {
            try {
                objectPrx = ic.stringToProxy("BallDetectorManager:default -h " + currentHost + " -p " + currentPort);
                ballDetectorPrx = bica.BallDetectorManagerPrxHelper.checkedCast(objectPrx);
            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());
            }
        }
        return ballDetectorPrx;
    }

    public bica.GoalDetectorManagerPrx getGoalDetectorManagerPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (goalDetectorPrx == null) {
            try {
                objectPrx = ic.stringToProxy("GoalDetectorManager:default -h " + currentHost + " -p " + currentPort);
                goalDetectorPrx = bica.GoalDetectorManagerPrxHelper.checkedCast(objectPrx);
            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());
            }
        }
        return goalDetectorPrx;
    }

    public bica.LogProviderPrx getLogProviderPrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (logProviderPrx == null) {
            try {
                objectPrx = ic.stringToProxy("LogProvider:default -h " + currentHost + " -p " + currentPort);
                logProviderPrx = bica.LogProviderPrxHelper.checkedCast(objectPrx);
            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());
            }
        }
        return logProviderPrx;
    }

    /**
    public bica.AleatoryMovePrx getAleatoryMovePrx() throws RuntimeException {
        Ice.ObjectPrx objectPrx = null;

        if (aleatoryMovePrx == null) {
            try {
                objectPrx = ic.stringToProxy("AleatoryMoveManager:default -h " + currentHost + " -p " + currentPort);
                aleatoryMovePrx = bica.AleatoryMovePrxHelper.checkedCast(objectPrx);
            } catch (Ice.LocalException ex) {
                ex.printStackTrace();
                throw new RuntimeException(ex.ice_name());
            }
        }
        return aleatoryMovePrx;
    }
    */

    /*        	public bica.CalibrationBallManagerPrx getCalibrationBallManagerPrx() throws RuntimeException {
    Ice.ObjectPrx objectPrx = null;

    if (calibrationballPrx == null) {
    try {
    objectPrx = ic.stringToProxy("CalibrationBallManager:default -h " + currentHost + " -p " + currentPort);
    calibrationballPrx = bica.CalibrationBallManagerPrxHelper.checkedCast(objectPrx);
    } catch (Ice.LocalException ex) {
    ex.printStackTrace();
    throw new RuntimeException(ex.ice_name());
    }
    }
    return calibrationballPrx;
    }
     */
    public void connect(String host, int port) throws Ice.LocalException {
        currentHost = host;
        currentPort = port;

        try {
            // Initialize Ice
            //ic = Ice.Util.initialize();

            // Inicializmos el scheduler
            getSchedulerManagerPrx();

            // Estas inicializaciones no deberían estar aquí. Para que
            // haya compatibilidad con como estaba hecho antes por el
            // momento lo dejamos.
            getImageSelectorPrx();
            getComponentInfoProviderPrx();
            getCalibrationProviderPrx();
            getMovieManagerPrx();
            getLocalizationProviderPrx();
            //getPruebag2gposManagerPrx();
            getGo2posManagerObsPrx();
            getGo2posManagerPrx();
            getGo2gposManagerPrx();
            getHeadMotionMangerPrx();
            getMusicManagerPrx();
            getBodyMotionManager();
            getTextSpeechManagerPrx();
            getWorldModelProviderPrx();
            getTeamInfoProviderPrx();
            getGrDebugProviderPrx();
            getBallDetectorManagerPrx();
            getGoalDetectorManagerPrx();
            getLogProviderPrx();
//			getCalibrationBallManagerPrx();

            connected = true;

            status = 0;

        } catch (Ice.LocalException ex) {
            ic.destroy();
            status = 1;
            System.out.println("Connection: " + ex.ice_name());
            //TODO: todos los métodos que llaman a "connect()", deberían usar try...catch
            //para recoger la excepción que este método puede lanzar.
            throw ex;
        }
    }

    public void disconnect() {
        try {
            connected = false;
            ic.destroy();
            schedulerPrx = null;
        } catch (Ice.LocalException ex) {
            throw ex;
        }
    }

    public boolean isConnected() {
        return connected;
    }

    public dataResp send(String msg) {

        dataResp resp = new dataResp();
        byte[] aux = new byte[MSGMAXSIZE];

        byte[] recvdata = new byte[MSGMAXSIZE];
        String respstring = null;
        int readed = -1;
        int counter = 0;

        out.println(msg);
        out.flush();

        try {
            readed = in.read(recvdata, 0, MSGMAXSIZE);

            System.err.println("recv(" + readed + ")");
            for (int i = 0; i < readed; i++) {
                aux[counter++] = recvdata[i];
            }

            while (in.available() > 0) {
                readed = in.read(recvdata, 0, MSGMAXSIZE);

                for (int i = 0; i < readed; i++) {
                    aux[counter++] = recvdata[i];
                }
            }

            resp.size = counter;

        } catch (IOException ex) {
            System.err.println("Error in receiving: " + ex.toString());
        }

        resp.data = new byte[counter];
        for (int i = 0; i < counter; i++) {
            resp.data[i] = aux[i];
        }

        return resp;
    }

    public dataResp sendImage(String msg, int width, int height, int depth) {

        dataResp resp = new dataResp();
        resp.data = new byte[width * height * depth];
        byte[] recvdata = new byte[MSGMAXSIZE];
        int readed = -1;
        int counter = 0;

        out.println(msg);

        try {

            while (counter < (width * height * depth)) {

                readed = in.read(recvdata, 0, MSGMAXSIZE);

                for (int i = 0; i < readed; i++) {
                    resp.data[counter++] = recvdata[i];
                }
            }

            resp.size = counter;

        } catch (IOException ex) {
            System.err.println("Error in receiving: " + ex.toString());
        }

        return resp;
    }

    public void sendExtImage(byte[] img) {

        //Serialize the img to an array of bytes
		/* ByteArrayOutputStream baos = new ByteArrayOutputStream( 60000 );
        try {
        ImageIO.write(img, "ppm", baos);
        baos.flush();
        } catch (IOException ex) {
        Logger.getLogger(Connection.class.getName()).log(Level.SEVERE, null, ex);
        }

        byte[] resultImageAsRawBytes = baos.toByteArray();*/

        //System.out.println("Longitud: " + img.length);
        //System.out.println(img[0] + " " + img[57653]);

        dataResp resp = JmanagerView.root.getConnection().send("setExtImg:begin:" + img.length + ":");

        JmanagerView.root.getConnection().send(img);

        resp = JmanagerView.root.getConnection().send("setExtImg:end:");
    }

    private void send(byte[] rawbytes) {
        dataResp resp = new dataResp();
        byte[] aux = new byte[MSGMAXSIZE];

        byte[] recvdata = new byte[MSGMAXSIZE];
        String respstring = null;
        int readed = -1;
        int counter = 0;

        try {
            sock.getOutputStream().write(rawbytes);
        } catch (IOException ex) {
            Logger.getLogger(Connection.class.getName()).log(Level.SEVERE, null, ex);
        }

        /*try {

        readed = in.read(recvdata, 0, MSGMAXSIZE);

        System.err.println("recv(" + readed + ")");
        for (int i = 0; i < readed; i++) {
        aux[counter++] = recvdata[i];
        }

        while (in.available() > 0) {
        readed = in.read(recvdata, 0, MSGMAXSIZE);

        for (int i = 0; i < readed; i++) {
        aux[counter++] = recvdata[i];
        }
        }

        resp.size = counter;

        } catch (IOException ex) {
        System.err.println("Error in receiving: " + ex.toString());
        }

        resp.data = new byte[counter];
        for (int i = 0; i < counter; i++) {
        resp.data[i] = aux[i];
        }

        return resp;
         */
    }

    public String getHost() {
        return currentHost;
    }

    /**
     * @return the ic
     */
    public Ice.Communicator getCommunicator() {
        return ic;
    }
}
