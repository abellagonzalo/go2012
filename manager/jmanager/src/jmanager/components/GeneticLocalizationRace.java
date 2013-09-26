/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jmanager.components;

import java.util.ArrayList;

/**
 *
 * @author eperdes
 */
public class GeneticLocalizationRace {
    
    private ArrayList<GeneticLocalizationParticle> exploiters;
    private GeneticLocalizationParticle race;
    private int wins;
    private int life;

    public GeneticLocalizationRace() {
        this.exploiters = new ArrayList<GeneticLocalizationParticle>();
    }
    
    
    public GeneticLocalizationParticle getRace() {
        return race;
    }

    public void setRace(GeneticLocalizationParticle race) {
        this.race = race;
    }

    public int getWins() {
        return wins;
    }

    public void setWins(int wins) {
        this.wins = wins;
    }

    public int getLife() {
        return life;
    }

    public void setLife(int life) {
        this.life = life;
    }
    
    public void addExploiter(GeneticLocalizationParticle p) {
        this.exploiters.add(p);
    }

    public GeneticLocalizationParticle getExploiter(int i) {
        return this.exploiters.get(i);
    }

    public int numExploiters() {
        return this.exploiters.size();
    }
}
