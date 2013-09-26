/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * ComponentPerceptionPanel.java
 *
 * Created on 28-oct-2009, 13:08:37
 */
package jmanager.components;

import jmanager.*;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author paco
 */
public class Stratego extends javax.swing.JPanel {

	/** Creates new form ComponentGTLocalization */
	public Stratego() {
		initComponents();
	}

	public void refresh() {

	}

	/** This method is called from within the constructor to
	 * initialize the form.
	 * WARNING: Do NOT modify this code. The content of this method is
	 * always regenerated by the Form Editor.
	 */
	@SuppressWarnings("unchecked")
	// <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
	private void initComponents() {

		ActivationCheckBox = new javax.swing.JCheckBox();

		setMinimumSize(new java.awt.Dimension(330, 250));
		setName("Form"); // NOI18N
		setPreferredSize(new java.awt.Dimension(330, 250));

		org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance().getContext().getResourceMap(Stratego.class);
		ActivationCheckBox.setText(resourceMap.getString("ActivationCheckBox.text")); // NOI18N
		ActivationCheckBox.setName("ActivationCheckBox"); // NOI18N
		ActivationCheckBox.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				ActivationCheckBoxActionPerformed(evt);
			}
		});

		javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
		this.setLayout(layout);
		layout.setHorizontalGroup(
				layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
				.addGroup(layout.createSequentialGroup()
						.addContainerGap()
						.addComponent(ActivationCheckBox)
						.addContainerGap(234, Short.MAX_VALUE))
		);
		layout.setVerticalGroup(
				layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
				.addGroup(layout.createSequentialGroup()
						.addContainerGap()
						.addComponent(ActivationCheckBox)
						.addContainerGap(216, Short.MAX_VALUE))
		);
	}// </editor-fold>//GEN-END:initComponents

	private void ActivationCheckBoxActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_ActivationCheckBoxActionPerformed
		if (ActivationCheckBox.isSelected()) {
			JmanagerView.root.getConnection().schedulerPrx.run("Stratego");
			//dataResp resp = JmanagerView.root.getConnection().send("run:Stratego:");

		} else {
			JmanagerView.root.getConnection().schedulerPrx.stop("Stratego");
			//dataResp resp = JmanagerView.root.getConnection().send("pause:Stratego:");
			//printResp(resp);
		}

	}//GEN-LAST:event_ActivationCheckBoxActionPerformed
	/**/
	// Variables declaration - do not modify//GEN-BEGIN:variables
	private javax.swing.JCheckBox ActivationCheckBox;
	// End of variables declaration//GEN-END:variables
}