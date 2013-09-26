/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jmanager;

/**
 *
 * @author paco
 */
public class Robot {

    Connection connection = null;

    public Robot() {
        connection = new Connection();
    }

    public Connection getConnection() {
        return connection;
    }

    @Override
    public String toString() {
        return connection.getHost();
    }

}
