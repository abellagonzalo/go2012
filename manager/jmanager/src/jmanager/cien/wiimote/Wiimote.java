/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jmanager.cien.wiimote;

import java.util.Observable;
import jmanager.cien.RobotWrapper;
import motej.Mote;
import motej.MoteFinder;
import motej.MoteFinderListener;
import motej.event.CoreButtonEvent;
import motej.event.CoreButtonListener;
import motej.event.MoteDisconnectedEvent;
import motej.event.MoteDisconnectedListener;

/**
 *
 * @author gon
 */
public class Wiimote extends Observable implements MoteFinderListener,
                                                   MoteDisconnectedListener<Mote>,
                                                   CoreButtonListener {

    private Mote mote;
    private int estado;

    // Estados
    public static final int conectado = 1;
    public static final int desconectado = 2;
    public static final int buscando = 3;

    private RobotWrapper robotWrapper;
    
    private static float constanteBody = 1.0f;
    private static float constanteHead = 10.0f;
    private boolean girando = false;

    public Wiimote(RobotWrapper robotWrapper) {
        this.robotWrapper = robotWrapper;
        MoteFinder.getMoteFinder().addMoteFinderListener(this);
        setEstado(desconectado);
    }

    public void buscarWiimote() {
        MoteFinder.getMoteFinder().startDiscovery();
        setEstado(buscando);
    }

    public void cancelarBusqueda() {
        MoteFinder.getMoteFinder().stopDiscovery();
        setEstado(desconectado);
    }

    public void desconectar() {
        if (estado == conectado) {
            Thread thread = new Thread(new Runnable() {
                public void run() {
                    mote.disconnect();
                    setEstado(desconectado);
                }
            });
            thread.setDaemon(true);
            thread.start();
        }
    }

    /**
     * @return the estado
     */
    public int getEstado() {
        return estado;
    }

    /**
     * @param estado the estado to set
     */
    private void setEstado(int estado) {
        this.estado = estado;
        setChanged();
        notifyObservers();
    }

    public void moteFound(Mote mote) {
        this.mote = mote;
        setEstado(conectado);
        mote.addCoreButtonListener(this);
        mote.addMoteDisconnectedListener(this);
        mote.setPlayerLeds(new boolean[]{true, false, false, false});
        mote.rumble(2000l);
        MoteFinder.getMoteFinder().stopDiscovery();
    }

    public void moteDisconnected(MoteDisconnectedEvent<Mote> mde) {
        desconectar();
    }

    public void buttonPressed(CoreButtonEvent cbe) {
        // Si pulsamos el botón B movemos la cabeza
        if (cbe.isButtonBPressed()) {

            if (cbe.isButtonAPressed()){
                robotWrapper.movimientoCabezaLateral(0.0f);
                robotWrapper.movimientoCabezaVertical(0.0f);
            }

            if (cbe.isDPadDownPressed()) {
                robotWrapper.movimientoCabezaVertical(robotWrapper.getHeadTilt()-constanteHead);
            }

            if (cbe.isDPadLeftPressed()) {
                robotWrapper.movimientoCabezaLateral(robotWrapper.getHeadPan()-constanteHead);
            }

            if (cbe.isDPadRightPressed()) {
                robotWrapper.movimientoCabezaLateral(robotWrapper.getHeadPan()+constanteHead);
            }

            if (cbe.isDPadUpPressed()) {
                robotWrapper.movimientoCabezaVertical(robotWrapper.getHeadTilt()+constanteHead);
            }

        // Si no pulsamos el botón B movemos el cuerpo
        } else {

            if (cbe.isButtonAPressed()){
                robotWrapper.parar();
            }

            if (cbe.isDPadDownPressed()) {
                //robotWrapper.andar(robotWrapper.getBodyV()-constanteBody);
                robotWrapper.andar(-constanteBody);
            }

            if (cbe.isDPadLeftPressed()) {
                robotWrapper.girar(-constanteBody);
                girando=true;
            }

            if (cbe.isDPadRightPressed()) {
                robotWrapper.girar(constanteBody);
                girando=true;
            }

            if (cbe.isDPadUpPressed()) {
                //robotWrapper.andar(robotWrapper.getBodyV()+constanteBody);
                robotWrapper.andar(constanteBody);
            }

            if (cbe.isNoButtonPressed()) {
                if (girando) {
                    robotWrapper.girar(0.001f);
                    girando=false;
                }
            }
        }

        // Si pulsamos el mas, pasamos al siguiente paso de la terapia
        if (cbe.isButtonPlusPressed()) {
            robotWrapper.pasoSiguiente();
        }
    }
}