/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jmanager.cien;

import com.jcraft.jsch.Channel;
import com.jcraft.jsch.ChannelExec;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Session;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Observable;
import jmanager.Robot;

/**
 *
 * @author gon
 */
public class RobotWrapper extends Observable {

    private Robot robot;

    /**
     * Estado de la conexion del robot;
     */
    public enum ConexionRobot {

        conectado, buscando, desconectado
    };
    private ConexionRobot conexionRobot = ConexionRobot.desconectado;
    /**
     * Variables para caminar.
     */
    private float bodyV = 0.0f, bodyW = 0.0f, bodyL = 0.0f;
    /**
     * Variables para mover la cabeza.
     */
    private float velocidadCabeza = 0.5f, headPan = 0.0f, headTilt = 0.0f;

    /**
     * Variables para la ejecucion de un guion.
     */
    public enum EstadoTerapia {

        ejecutando, cargada, parada
    };
    private EstadoTerapia estadoTerapia = EstadoTerapia.parada;
    private String terapiaCargada = "";
    private int[] breakpoints;

    /*
     * Variables para el ssh. Esto es un poco cutre.
     */
    private String sshUser = "nao";
    private String sshPassword = "nao";

    public void conectar(String nombre, int manualPort) {

        // Estado de la conexion: buscando
        setConexionRobot(ConexionRobot.buscando);

        try {

            // Puerto del robot
            InetAddress[] inetAddresses = InetAddress.getAllByName(nombre);
            int port = 10000;
            for (int i = 0; i < inetAddresses.length; i++) {
                String ip = inetAddresses[i].getHostAddress();
                int lastNumber = Integer.parseInt(ip.substring(ip.length() - 1));
                port += lastNumber;
            }

            robot = new Robot();
            robot.getConnection().connect(nombre, manualPort);

            // Activar modulos
            robot.getConnection().getSchedulerManagerPrx().run("Head");
            robot.getConnection().getSchedulerManagerPrx().run("Music");
            robot.getConnection().getSchedulerManagerPrx().run("Movie");
            robot.getConnection().getSchedulerManagerPrx().run("Body");

            setConexionRobot(ConexionRobot.conectado);

        } catch (UnknownHostException ex) {
            setConexionRobot(ConexionRobot.desconectado);
            throw new RuntimeException(ex);
        } catch (RuntimeException ex) {
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void desconectar() {
        try {
            setConexionRobot(ConexionRobot.desconectado);
            robot.getConnection().disconnect();

        } catch (RuntimeException ex) {
            throw ex;
        }
    }

    public void andar(float velocidad) {
        try {
            // Security checks
            if (getRobot().getConnection().getBodyMotionManager().isMoving()) return;

            if (velocidad > 1.0f) {
                velocidad = 1.0f;
            }
            if (velocidad < -1.0f) {
                velocidad = -1.0f;
            }
            bodyV = velocidad;
            robot.getConnection().getBodyMotionManager().setVel(getBodyV(), getBodyW() * -1, getBodyL() * -1);
            setChanged();
            notifyObservers();

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void girar(float velocidad) {
        try {
            // Security checks
            if (getRobot().getConnection().getBodyMotionManager().isMoving()) return;

            if (velocidad > 1.0f) {
                velocidad = 1.0f;
            }
            if (velocidad < -1.0f) {
                velocidad = -1.0f;
            }
            bodyW = velocidad;
            getRobot().getConnection().getBodyMotionManager().setVel(getBodyV(), getBodyW() * -1, getBodyL() * -1);
            setChanged();
            notifyObservers();

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void andarLateral(float velocidad) {
        try {
            // Security checks
            if (getRobot().getConnection().getBodyMotionManager().isMoving()) return;
            
            if (velocidad > 1.0f) {
                velocidad = 1.0f;
            }
            if (velocidad < -1.0f) {
                velocidad = -1.0f;
            }
            bodyL = velocidad;
            getRobot().getConnection().getBodyMotionManager().setVel(getBodyV(), getBodyW() * -1, getBodyL() * -1);
            setChanged();
            notifyObservers();

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void parar() {
        try {
            bodyV = 0.001f;
            bodyW = 0.001f;
            bodyL = 0.001f;
            getRobot().getConnection().getBodyMotionManager().setVel(getBodyV(), getBodyW() * -1, getBodyL() * -1);
            setChanged();
            notifyObservers();

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void movimientoCabezaLateral(float posicion) {
        try {
            headPan = posicion;
            posicion = (float) (Math.toRadians(posicion / -100.0f * 120.0f));
            getRobot().getConnection().getHeadMotionMangerPrx().setPanPos(posicion, getVelocidadCabeza());
            setChanged();
            notifyObservers();

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void movimientoCabezaVertical(float posicion) {
        try {
            headTilt = posicion;
            posicion = (float) (Math.toRadians(posicion / -100.0 * 39.0));
            if (posicion > Math.toRadians(30)) {
                posicion = (float) Math.toRadians(30);
            }
            getRobot().getConnection().getHeadMotionMangerPrx().setTiltPos(posicion, getVelocidadCabeza());
            setChanged();
            notifyObservers();

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void cargarTerapia(String file) {
        try {
            // Set file
            getRobot().getConnection().getMovieManagerPrx().setFile(file);

            // Get breakpoints array
            breakpoints = getRobot().getConnection().getMovieManagerPrx().getBreakpoints();

            // Change estadoTerapia
            terapiaCargada = file;
            setEstadoTerapia(EstadoTerapia.cargada);

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void ejecutarTerapia() {
        try {
            getRobot().getConnection().getMovieManagerPrx().playPress(1);
            setEstadoTerapia(EstadoTerapia.ejecutando);

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void pasoSiguiente() {
        try {
            // Terapia cargada
            if (estadoTerapia == EstadoTerapia.cargada) {
                ejecutarTerapia();
            }
            
            // Terapia ya empezada
            if (estadoTerapia == EstadoTerapia.ejecutando) {
                int linea = getRobot().getConnection().getMovieManagerPrx().pausePress();
                getRobot().getConnection().moviePrx.setFile(terapiaCargada);
                getRobot().getConnection().moviePrx.playPress(linea);
            }

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void irALinea(int num) throws IndexOutOfBoundsException {
        try {
            getRobot().getConnection().getMovieManagerPrx().pausePress();
            getRobot().getConnection().getMovieManagerPrx().setFile(terapiaCargada);
            getRobot().getConnection().getMovieManagerPrx().playPress(breakpoints[num - 1]);

            // Change state
            setEstadoTerapia(EstadoTerapia.ejecutando);

            System.out.println("Voy a linea " + num);

        } catch (IndexOutOfBoundsException ex) {
            throw ex;

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void pararTerapia() {
        try {
            setEstadoTerapia(EstadoTerapia.parada);
            terapiaCargada = "";
            if (estadoTerapia == EstadoTerapia.ejecutando) {
                getRobot().getConnection().getMovieManagerPrx().stop();
            }

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void reproducirSonido(String file) {
        try {
            getRobot().getConnection().getMusicManagerPrx().setFileMP3(file);
            getRobot().getConnection().getMusicManagerPrx().playPress();

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void pararSonido() {
        try {
            getRobot().getConnection().getMusicManagerPrx().stopPress();

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public boolean estaReproduciendo() {
        try {
            return !getRobot().getConnection().getMusicManagerPrx().isStopped();

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void setVolumen(int volumen) {
        try {
            float volume = (float)volumen / 100.f;
            getRobot().getConnection().getMusicManagerPrx().setVolume(volume);

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public int getVolumen() {
        float volume = getRobot().getConnection().musicPrx.getVolume()*100f;
        return (int)volume;
    }

    public void hacerMovimiento(String movimiento) {
        try {
            // Security check
            if (getRobot().getConnection().getBodyMotionManager().isWalking()) return;

            getRobot().getConnection().getBodyMotionManager().doMove(movimiento);
            

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void apagarRobot() {
        try {
            desconectar();
            
            String host = getRobot().getConnection().getHost();
            JSch jsch = new JSch();
            Session session = jsch.getSession(sshUser, host);
            session.setPassword(sshPassword);
            java.util.Properties config = new java.util.Properties();
            config.put("StrictHostKeyChecking", "no");
            session.setConfig(config);
            session.connect();

            Channel channel = session.openChannel("exec");
            ((ChannelExec) channel).setCommand("shutdown -h now");
            channel.connect();

            channel.disconnect();
            session.disconnect();

        } catch (JSchException ex) {
            System.out.println("Ha ocurrido un error apagando el robot.\n"
                    + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw new RuntimeException(ex);

        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(ConexionRobot.desconectado);
            throw ex;
        }
    }

    public void disableStiffness() {
        try {
            System.out.println("Disablle stiffness");
            getRobot().getConnection().getMovieManagerPrx().disableStiffness("Body");



        } catch (RuntimeException ex) {
            System.out.println("Error en la conexión. " + ex.getMessage());
            setConexionRobot(
                    ConexionRobot.desconectado);


            throw ex;


        }
    }

    /**
     * @return the robot
     */
    public Robot getRobot() {
        return robot;


    }

    /**
     * @return the bodyV
     */
    public float getBodyV() {
        return bodyV;


    }

    /**
     * @return the bodyW
     */
    public float getBodyW() {
        return bodyW;


    }

    /**
     * @return the bodyL
     */
    public float getBodyL() {
        return bodyL;


    }

    /**
     * @return the velocidadCabeza
     */
    public float getVelocidadCabeza() {
        return velocidadCabeza;


    }

    /**
     * @param velocidadCabeza the velocidadCabeza to set
     */
    public void setVelocidadCabeza(float velocidadCabeza) {
        this.velocidadCabeza = velocidadCabeza;


    }

    /**
     * @return the headPan
     */
    public float getHeadPan() {
        return headPan;


    }

    /**
     * @return the headTilt
     */
    public float getHeadTilt() {
        return headTilt;


    }

    /**
     * @return true si el robot esta conectado
     */
    public boolean isRobotConectado() {
        return (conexionRobot == ConexionRobot.conectado);


    }

    /**
     * @return true si el robot esta desconectado
     */
    public boolean isRobotDesconectado() {
        return (conexionRobot == ConexionRobot.desconectado);


    }

    /**
     * @return true si se esta buscando el robot
     */
    public boolean isRobotBuscando() {
        return (conexionRobot == ConexionRobot.buscando);


    }

    /**
     * @param conexionRobot the conexionRobot to set
     */
    public void setConexionRobot(ConexionRobot conexionRobot) {
        this.conexionRobot = conexionRobot;


        if (conexionRobot == ConexionRobot.desconectado) {
            setEstadoTerapia(EstadoTerapia.parada);


        }
        setChanged();
        notifyObservers();


    }

    /**
     * @return the estadoTerapia
     */
    public EstadoTerapia getEstadoTerapia() {
        return estadoTerapia;


    }

    /**
     * @param estadoTerapia the estadoTerapia to set
     */
    private void setEstadoTerapia(EstadoTerapia estadoTerapia) {
        this.estadoTerapia = estadoTerapia;
        setChanged();
        notifyObservers();


    }

    /**
     * @return the terapiaCargada
     */
    public String getTerapiaCargada() {
        return terapiaCargada;

    }
}


