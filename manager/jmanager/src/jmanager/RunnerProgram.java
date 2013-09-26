/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jmanager;

import java.io.IOException;
import java.io.InputStream;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author paco
 */
public class RunnerProgram extends Thread{

    OutputFrame outputframe = null;
    String cmd = null;

    RunnerProgram(String command, OutputFrame frame){
        cmd = command;
        outputframe = frame;

        outputframe.identifyThread(this);
    }

    public void run(){

        
        // create a process for the shell
        ProcessBuilder pb = new ProcessBuilder("bash", "-c", cmd);
        pb.redirectErrorStream(true); // use this to capture messages sent to stderr

        Process shell = null;
        try {
            shell = pb.start();
        } catch (IOException ex) {
            Logger.getLogger(RunnerProgram.class.getName()).log(Level.SEVERE, null, ex);
        }


        InputStream shellIn = shell.getInputStream(); // this captures the output from the command

        try {
            // this captures the output from the command
            Thread.sleep(500);
        } catch (InterruptedException ex) {
            Logger.getLogger(RunnerProgram.class.getName()).log(Level.SEVERE, null, ex);
        }
        int c;
        try {
            while ((c = shellIn.read()) != -1) {
                outputframe.print((char)c+"");//
                //System.out.write(c);
            }
        } catch (IOException ex) {
            Logger.getLogger(OutputFrame.class.getName()).log(Level.SEVERE, null, ex);
        }

        try {

            // this captures the output from the command
            int shellExitStatus = shell.waitFor(); // wait for the shell to finish and get the return code
        } catch (InterruptedException ex) {
            System.err.println("Thread interrumped (normal if kill a task) :)");
        }

        outputframe.print("[Terminado]");

            // close the stream
        try {
            shellIn.close();
        } catch (IOException ignoreMe) {}
        
        outputframe.identifyThread(null);

        try {
            this.finalize();
        } catch (Throwable ex) {
            Logger.getLogger(RunnerProgram.class.getName()).log(Level.SEVERE, null, ex);
        }

       
    }

}
