/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jmanager.cien;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Observable;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author gon
 */
public class SoundsQueue extends Observable {

    private List<String> soundsList;
    private List<String> movesList;
    private RobotWrapper robotWrapper;
    private boolean stopSound;

    public SoundsQueue(RobotWrapper robotWrapper) {
        this.robotWrapper = robotWrapper;
        soundsList = Collections.synchronizedList(new ArrayList());
        stopSound = false;
    }

    public String getSound() {
        return soundsList.get(0);
    }

    public void addSound(String sound) {
        if (soundsList.isEmpty()) {
            soundsList.add(sound);
            Thread thread = new Thread(new SoundsQueueThread());
            thread.setDaemon(true);
            thread.start();
        } else {
            soundsList.add(sound);
        }
        setChanged();
        notifyObservers();
    }
    
    public void removeSound(int position) {
        if (position==0) {
            robotWrapper.pararSonido();
        } else {
            soundsList.remove(position);
        }
        setChanged();
        notifyObservers();
    }

    public void clear() {
        soundsList.clear();
        setChanged();
        notifyObservers();
    }

    public boolean isEmpty() {
        return soundsList.isEmpty();
    }

    public String[] getQueue() {
        return soundsList.toArray(new String[soundsList.size()]);
    }

    /*
    private final class Element {

        private String sound;
        private String move;

        public Element(String sound) {
            this.sound = sound;
            this.move = "";
        }

        public Element(String sound, String move) {
            this.sound = sound;
            this.move = move;
        }

        public String getSound() {
            return sound;
        }

        public String getMove() {
            return move;
        }
    }
     * */

    private final class SoundsQueueThread implements Runnable {

        public void run() {
            while (!soundsList.isEmpty()) {
                // Reproducir sonido
                String soundFile = "/home/nao/mp3/" + getSound().toLowerCase() + ".mp3";
                robotWrapper.reproducirSonido(soundFile);

                // Esperar a que termine de reproducirse el sonido
                while (robotWrapper.estaReproduciendo()) {
                    try {
                        Thread.sleep(100);
                    } catch (InterruptedException ex) {
                        Logger.getLogger(SoundsQueue.class.getName()).log(Level.SEVERE, null, ex);
                    }
                }

                // Quitar de la lista
                soundsList.remove(0);
                setChanged();
                notifyObservers();
            }
        }
    }
}
