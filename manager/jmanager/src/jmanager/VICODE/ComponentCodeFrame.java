/*
 * Frame.java
 *
 * Created on 13-ene-2010, 19:26:44
 */
package jmanager.VICODE;

/**
 *
 * @author paco
 */
public class ComponentCodeFrame extends javax.swing.JFrame {

	private Component component = null;

	/** Creates new form Frame */
	public ComponentCodeFrame() {
		initComponents();
		setDefaultCloseOperation(javax.swing.WindowConstants.HIDE_ON_CLOSE);
	}

	@SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        idTextField = new javax.swing.JTextField();
        msgLabel = new javax.swing.JLabel();

        setName("Form"); // NOI18N
        setResizable(false);

        idTextField.setName("idTextField"); // NOI18N
        idTextField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                idTextFieldActionPerformed(evt);
            }
        });

        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance().getContext().getResourceMap(ComponentCodeFrame.class);
        msgLabel.setText(resourceMap.getString("msgLabel.text")); // NOI18N
        msgLabel.setName("msgLabel"); // NOI18N

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGap(12, 12, 12)
                .addComponent(idTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 196, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addComponent(msgLabel, javax.swing.GroupLayout.PREFERRED_SIZE, 270, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(16, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                    .addComponent(msgLabel, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(idTextField, javax.swing.GroupLayout.Alignment.LEADING))
                .addContainerGap(59, Short.MAX_VALUE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void idTextFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_idTextFieldActionPerformed
		if (component != null) {
			if (! component.setId(idTextField.getText().trim())) {
				msgLabel.setText("Component already exist");
			} else {
				msgLabel.setText("");
			}
		}
    }//GEN-LAST:event_idTextFieldActionPerformed
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JTextField idTextField;
    private javax.swing.JLabel msgLabel;
    // End of variables declaration//GEN-END:variables

	public void setIdTextField(String id) {
		idTextField.setText(id);
	}

	public void setComponent(Component aThis) {
		component = aThis;
	}
}
