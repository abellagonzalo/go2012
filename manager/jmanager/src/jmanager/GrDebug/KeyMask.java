/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jmanager.GrDebug;

import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author Carlos Agüero <caguero@gsyc.es>
 */
public class KeyMask {

    private Map<String,Boolean> mp;
    private static final String alphabet = "abcdefghijklmnñopqrstuvwxyz1234567890";

    public KeyMask() {
        mp = new HashMap<String,Boolean>();

        // adding or set elements in Map by put method key and value pair
        for (char c: alphabet.toCharArray()) {
             mp.put(Character.toString(c), true);
        }     
    }

    public void keyPressed( char key) {
        if (mp.containsKey(Character.toString(key))) {
            boolean value = mp.get(Character.toString(key));
            switch (key) {
                case 'a':   // Activate/Deactivate all values
                    for (char c: alphabet.toCharArray()) {
                        mp.put(Character.toString(c), !value);
                    }
                    break;
                default:
                    mp.put(Character.toString(key), !value);
                    break;
            }
        }
    }

    public boolean isActive( String key ) {
        if (mp.containsKey(key))
            return mp.get(key);
        else
            return false;
    }

    void print() {
        for (char c: alphabet.toCharArray()) {
             System.out.println(c + ":" + mp.get(Character.toString(c)));
        }
    }
}
