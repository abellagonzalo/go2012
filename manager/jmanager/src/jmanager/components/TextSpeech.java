/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * TextSpeech.java
 *
 * Created on 12-abr-2010, 11:05:22
 */

package jmanager.components;
import java.awt.TextField;
import jmanager.*;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.jdesktop.application.Action;

import java.io.File;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import javax.swing.JFileChooser;

/**
 *
 * @author rbenitez
 */
public class TextSpeech extends javax.swing.JPanel {

	/** Creates new form TextSpeech */
	public TextSpeech() {
		initComponents();
	}

	/** This method is called from within the constructor to
	 * initialize the form.
	 * WARNING: Do NOT modify this code. The content of this method is
	 * always regenerated by the Form Editor.
	 */
	@SuppressWarnings("unchecked")
	// <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
	private void initComponents() {

		jPanel1 = new javax.swing.JPanel();
		SendText = new javax.swing.JButton();
		ListLanguage = new javax.swing.JComboBox();
		ListVoice = new javax.swing.JComboBox();
		jLabel1 = new javax.swing.JLabel();
		jLabel2 = new javax.swing.JLabel();
		ActivationTextSpeech = new javax.swing.JCheckBox();
		jLabel3 = new javax.swing.JLabel();
		Volume = new javax.swing.JSlider();
		jLabel4 = new javax.swing.JLabel();
		jLabel5 = new javax.swing.JLabel();
		txtLanguage = new javax.swing.JTextField();
		txtVoice = new javax.swing.JTextField();
		miText = new javax.swing.JTextField();
		getValue = new javax.swing.JButton();
		jLabel6 = new javax.swing.JLabel();
		ListEffect = new javax.swing.JComboBox();
		jLabel7 = new javax.swing.JLabel();
		jLabel8 = new javax.swing.JLabel();
		valueEffect = new javax.swing.JSlider();

		setName("Form"); // NOI18N

		jPanel1.setName("jPanel1"); // NOI18N

		org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(jmanager.JmanagerApp.class).getContext().getResourceMap(TextSpeech.class);
		SendText.setText(resourceMap.getString("SendText.text")); // NOI18N
		SendText.setName("SendText"); // NOI18N
		SendText.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				SendTextActionPerformed(evt);
			}
		});

		ListLanguage.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "English", "Spanish" }));
		ListLanguage.setName("ListLanguage"); // NOI18N
		ListLanguage.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				ListLanguageActionPerformed(evt);
			}
		});

		ListVoice.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Heather22Enhanced" }));
		ListVoice.setName("ListVoice"); // NOI18N
		ListVoice.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				ListVoiceActionPerformed(evt);
			}
		});

		jLabel1.setText(resourceMap.getString("jLabel1.text")); // NOI18N
		jLabel1.setName("jLabel1"); // NOI18N

		jLabel2.setText(resourceMap.getString("jLabel2.text")); // NOI18N
		jLabel2.setName("jLabel2"); // NOI18N

		ActivationTextSpeech.setText(resourceMap.getString("ActivationTextSpeech.text")); // NOI18N
		ActivationTextSpeech.setName("ActivationTextSpeech"); // NOI18N
		ActivationTextSpeech.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				ActivationTextSpeechActionPerformed(evt);
			}
		});

		jLabel3.setText(resourceMap.getString("jLabel3.text")); // NOI18N
		jLabel3.setName("jLabel3"); // NOI18N

		Volume.setValue(70);
		Volume.setName("Volume"); // NOI18N
		Volume.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				VolumeMouseReleased(evt);
			}
		});

		jLabel4.setText(resourceMap.getString("jLabel4.text")); // NOI18N
		jLabel4.setName("jLabel4"); // NOI18N

		jLabel5.setText(resourceMap.getString("jLabel5.text")); // NOI18N
		jLabel5.setName("jLabel5"); // NOI18N

		txtLanguage.setEnabled(false);
		txtLanguage.setName("txtLanguage"); // NOI18N

		txtVoice.setEnabled(false);
		txtVoice.setName("txtVoice"); // NOI18N

		miText.setText(resourceMap.getString("miText.text")); // NOI18N
		miText.setName("miText"); // NOI18N

		getValue.setText(resourceMap.getString("getValue.text")); // NOI18N
		getValue.setName("getValue"); // NOI18N
		getValue.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				getValueActionPerformed(evt);
			}
		});

		jLabel6.setText(resourceMap.getString("jLabel6.text")); // NOI18N
		jLabel6.setName("jLabel6"); // NOI18N

		ListEffect.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "pitchShift", "doubleVoice", "doubleVoiceLevel", "doubleVoiceTimeShift" }));
		ListEffect.setName("ListEffect"); // NOI18N
		ListEffect.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				ListEffectActionPerformed(evt);
			}
		});

		jLabel7.setText(resourceMap.getString("jLabel7.text")); // NOI18N
		jLabel7.setName("jLabel7"); // NOI18N

		jLabel8.setText(resourceMap.getString("jLabel8.text")); // NOI18N
		jLabel8.setName("jLabel8"); // NOI18N

		valueEffect.setMaximum(200);
		valueEffect.setValue(130);
		valueEffect.setName("valueEffect"); // NOI18N
		valueEffect.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseReleased(java.awt.event.MouseEvent evt) {
				valueEffectMouseReleased(evt);
			}
		});

		javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
		jPanel1.setLayout(jPanel1Layout);
		jPanel1Layout.setHorizontalGroup(
				jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
				.addGroup(jPanel1Layout.createSequentialGroup()
						.addContainerGap()
						.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
								.addGroup(jPanel1Layout.createSequentialGroup()
										.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
												.addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
														.addComponent(jLabel3)
														.addGap(26, 26, 26)
														.addComponent(Volume, javax.swing.GroupLayout.PREFERRED_SIZE, 140, javax.swing.GroupLayout.PREFERRED_SIZE))
														.addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
																.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
																		.addComponent(jLabel1)
																		.addComponent(ActivationTextSpeech)
																		.addComponent(jLabel2))
																		.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
																		.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
																				.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
																						.addComponent(ListLanguage, 0, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
																						.addComponent(ListVoice, 0, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
																						.addComponent(getValue))))
																						.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
																								.addGroup(jPanel1Layout.createSequentialGroup()
																										.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 269, Short.MAX_VALUE)
																										.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
																												.addGroup(jPanel1Layout.createSequentialGroup()
																														.addComponent(jLabel4)
																														.addGap(27, 27, 27)
																														.addComponent(txtLanguage, javax.swing.GroupLayout.PREFERRED_SIZE, 105, javax.swing.GroupLayout.PREFERRED_SIZE))
																														.addGroup(jPanel1Layout.createSequentialGroup()
																																.addComponent(jLabel5)
																																.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
																																.addComponent(txtVoice, javax.swing.GroupLayout.PREFERRED_SIZE, 105, javax.swing.GroupLayout.PREFERRED_SIZE))))
																																.addGroup(jPanel1Layout.createSequentialGroup()
																																		.addGap(56, 56, 56)
																																		.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
																																				.addGroup(jPanel1Layout.createSequentialGroup()
																																						.addComponent(jLabel7)
																																						.addGap(42, 42, 42))
																																						.addGroup(jPanel1Layout.createSequentialGroup()
																																								.addComponent(jLabel6)
																																								.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)))
																																								.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
																																										.addGroup(jPanel1Layout.createSequentialGroup()
																																												.addGap(2, 2, 2)
																																												.addComponent(valueEffect, javax.swing.GroupLayout.PREFERRED_SIZE, 140, javax.swing.GroupLayout.PREFERRED_SIZE))
																																												.addGroup(jPanel1Layout.createSequentialGroup()
																																														.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
																																														.addComponent(ListEffect, javax.swing.GroupLayout.PREFERRED_SIZE, 115, javax.swing.GroupLayout.PREFERRED_SIZE))))))
																																														.addGroup(jPanel1Layout.createSequentialGroup()
																																																.addGap(305, 305, 305)
																																																.addComponent(SendText)))
																																																.addContainerGap())
																																																.addGroup(jPanel1Layout.createSequentialGroup()
																																																		.addGap(29, 29, 29)
																																																		.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
																																																				.addGroup(jPanel1Layout.createSequentialGroup()
																																																						.addComponent(jLabel8)
																																																						.addContainerGap())
																																																						.addComponent(miText, javax.swing.GroupLayout.DEFAULT_SIZE, 715, Short.MAX_VALUE)))
		);
		jPanel1Layout.setVerticalGroup(
				jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
				.addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
						.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
								.addGroup(jPanel1Layout.createSequentialGroup()
										.addGap(94, 94, 94)
										.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
												.addComponent(ListEffect, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
												.addComponent(jLabel6))
												.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
												.addComponent(valueEffect, javax.swing.GroupLayout.PREFERRED_SIZE, 38, javax.swing.GroupLayout.PREFERRED_SIZE))
												.addGroup(jPanel1Layout.createSequentialGroup()
														.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
																.addGroup(jPanel1Layout.createSequentialGroup()
																		.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
																				.addGroup(jPanel1Layout.createSequentialGroup()
																						.addComponent(ActivationTextSpeech)
																						.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 35, Short.MAX_VALUE))
																						.addGroup(jPanel1Layout.createSequentialGroup()
																								.addContainerGap()
																								.addComponent(getValue)
																								.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)))
																								.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
																										.addComponent(Volume, javax.swing.GroupLayout.PREFERRED_SIZE, 38, javax.swing.GroupLayout.PREFERRED_SIZE)
																										.addComponent(jLabel3))
																										.addGap(12, 12, 12)
																										.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
																												.addComponent(jLabel1)
																												.addComponent(ListLanguage, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
																												.addGroup(jPanel1Layout.createSequentialGroup()
																														.addGap(19, 19, 19)
																														.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
																																.addComponent(txtLanguage, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
																																.addComponent(jLabel4))
																																.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
																																.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
																																		.addComponent(jLabel5)
																																		.addComponent(txtVoice, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
																																		.addGap(57, 57, 57)))
																																		.addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
																																		.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
																																				.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
																																						.addComponent(ListVoice, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
																																						.addComponent(jLabel7))
																																						.addComponent(jLabel2))))
																																						.addGap(22, 22, 22)
																																						.addComponent(jLabel8)
																																						.addGap(6, 6, 6)
																																						.addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
																																								.addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
																																										.addComponent(miText, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
																																										.addGap(187, 187, 187))
																																										.addComponent(SendText, javax.swing.GroupLayout.Alignment.TRAILING))
																																										.addGap(57, 57, 57))
		);

		javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
		this.setLayout(layout);
		layout.setHorizontalGroup(
				layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
				.addGroup(layout.createSequentialGroup()
						.addContainerGap()
						.addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
						.addGap(120, 120, 120))
		);
		layout.setVerticalGroup(
				layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
				.addGroup(layout.createSequentialGroup()
						.addContainerGap()
						.addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
						.addGap(24, 24, 24))
		);
	}// </editor-fold>//GEN-END:initComponents

	private void SendTextActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_SendTextActionPerformed
		// TODO add your handling code here:
		System.out.println("Text: " + miText.getText());
		JmanagerView.root.getConnection().speechPrx.setmiText(miText.getText());
		//dataResp resp = JmanagerView.root.getConnection().send("textspeech:setsendText:" + miText.getText() + ":");

	}//GEN-LAST:event_SendTextActionPerformed

	private void ActivationTextSpeechActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_ActivationTextSpeechActionPerformed
		if (ActivationTextSpeech.isSelected()) {
			JmanagerView.root.getConnection().schedulerPrx.run("TextSpeech");
			//dataResp resp = JmanagerView.root.getConnection().send("run:TextSpeech:");
			//try {
			//	Thread.sleep(3000);
			//} catch (InterruptedException ex) {
			//	Logger.getLogger(Music.class.getName()).log(Level.SEVERE, null, ex);
			//}
		} else {
			JmanagerView.root.getConnection().schedulerPrx.stop("TextSpeech");
			//dataResp resp = JmanagerView.root.getConnection().send("pause:TextSpeech:");
		}



	}//GEN-LAST:event_ActivationTextSpeechActionPerformed

	private void VolumeMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_VolumeMouseReleased
		// TODO add your handling code here:
		float vol = Float.valueOf(Volume.getValue()) / 100;
		//dataResp resp = JmanagerView.root.getConnection().send("textspeech:setVolume:" + vol + ":");
		JmanagerView.root.getConnection().speechPrx.setVolume(vol);
	}//GEN-LAST:event_VolumeMouseReleased

	private void ListLanguageActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_ListLanguageActionPerformed
		// TODO add your handling code here:
		System.out.println("Language:" + ListLanguage.getSelectedItem());
		//dataResp resp = JmanagerView.root.getConnection().send("textspeech:setLanguage:" + ListLanguage.getSelectedItem() + ":");
		JmanagerView.root.getConnection().speechPrx.setLanguage((String) ListLanguage.getSelectedItem());
	}//GEN-LAST:event_ListLanguageActionPerformed

	private void ListVoiceActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_ListVoiceActionPerformed
		// TODO add your handling code here:
		System.out.println("Voice:" + ListVoice.getSelectedItem());
		//dataResp resp = JmanagerView.root.getConnection().send("textspeech:setVoice:" + ListVoice.getSelectedItem() + ":");
		JmanagerView.root.getConnection().speechPrx.setVoice((String)ListVoice.getSelectedItem());
	}//GEN-LAST:event_ListVoiceActionPerformed

	private void getValueActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_getValueActionPerformed
		// TODO add your handling code here:
		//dataResp resp = JmanagerView.root.getConnection().send("textspeech:getValue:");

		String respuesta = new String(JmanagerView.root.getConnection().speechPrx.getInfo());
		String[] Values = respuesta.split(":");

		txtLanguage.setText(Values[0]);
		txtVoice.setText(Values[1]);
		Volume.setValue(Integer.parseInt(Values[2])*100);
	}//GEN-LAST:event_getValueActionPerformed

	private void ListEffectActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_ListEffectActionPerformed
		// TODO add your handling code here:

	}//GEN-LAST:event_ListEffectActionPerformed

	private void valueEffectMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_valueEffectMouseReleased
		// TODO add your handling code here:
		float value = Float.valueOf(valueEffect.getValue()) / 100;
		System.out.println("Efecto:" + ListEffect.getSelectedItem() + " Valor" + String.valueOf(value));
		//dataResp resp = JmanagerView.root.getConnection().send("textspeech:setParameter:" + ListEffect.getSelectedItem() + ":" + value +":");
		JmanagerView.root.getConnection().speechPrx.setParameter((String)ListEffect.getSelectedItem(), value);
	}//GEN-LAST:event_valueEffectMouseReleased


	// Variables declaration - do not modify//GEN-BEGIN:variables
	private javax.swing.JCheckBox ActivationTextSpeech;
	private javax.swing.JComboBox ListEffect;
	private javax.swing.JComboBox ListLanguage;
	private javax.swing.JComboBox ListVoice;
	private javax.swing.JButton SendText;
	private javax.swing.JSlider Volume;
	private javax.swing.JButton getValue;
	private javax.swing.JLabel jLabel1;
	private javax.swing.JLabel jLabel2;
	private javax.swing.JLabel jLabel3;
	private javax.swing.JLabel jLabel4;
	private javax.swing.JLabel jLabel5;
	private javax.swing.JLabel jLabel6;
	private javax.swing.JLabel jLabel7;
	private javax.swing.JLabel jLabel8;
	private javax.swing.JPanel jPanel1;
	private javax.swing.JTextField miText;
	private javax.swing.JTextField txtLanguage;
	private javax.swing.JTextField txtVoice;
	private javax.swing.JSlider valueEffect;
	// End of variables declaration//GEN-END:variables

}
