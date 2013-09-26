/*
 * NewGenericNodeDialog.java
 *
 * Created on 06-dic-2011, 16:55:19
 */

package jmanager.VICODE.dialogs;

import jmanager.VICODE.BuilderGUI;

/**
 *
 * @author jesus
 */
public abstract class NewGenericNodeDialog extends javax.swing.JDialog {

	protected BuilderGUI builderGUI;

    /** Creates new form NewGenericNodeDialog */
    public NewGenericNodeDialog() {
		builderGUI = BuilderGUI.getInstance();
        initComponents();
		
		setLocationRelativeTo(null);
		setModal(false);
		setTitle("Create New Node");
		msgLabel.setText("");
		setVisible(false);
    }


    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        newTypeNodeLabel = new javax.swing.JLabel();
        idLabel = new javax.swing.JLabel();
        idTextField = new javax.swing.JTextField();
        cancelButton = new javax.swing.JButton();
        createButton = new javax.swing.JButton();
        msgLabel = new javax.swing.JLabel();

        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
        setResizable(false);
        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosed(java.awt.event.WindowEvent evt) {
                formWindowClosed(evt);
            }
        });

        newTypeNodeLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        newTypeNodeLabel.setText("New Item");
        newTypeNodeLabel.setName("newTypeNodeLabel"); // NOI18N

        idLabel.setText("Id:");
        idLabel.setName("idLabel"); // NOI18N

        idTextField.setText("NoName");
        idTextField.setName("idTextField"); // NOI18N

        cancelButton.setText("cancel");
        cancelButton.setName("cancelButton"); // NOI18N
        cancelButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cancelButtonActionPerformed(evt);
            }
        });

        createButton.setText("create");
        createButton.setName("createButton"); // NOI18N
        createButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                createButtonActionPerformed(evt);
            }
        });

        msgLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        msgLabel.setText("msgLabel");
        msgLabel.setName("msgLabel"); // NOI18N

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addGap(15, 15, 15)
                        .addComponent(newTypeNodeLabel, javax.swing.GroupLayout.PREFERRED_SIZE, 111, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(msgLabel, javax.swing.GroupLayout.PREFERRED_SIZE, 260, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(layout.createSequentialGroup()
                        .addGap(67, 67, 67)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                            .addGroup(layout.createSequentialGroup()
                                .addComponent(cancelButton)
                                .addGap(12, 12, 12)
                                .addComponent(createButton))
                            .addGroup(layout.createSequentialGroup()
                                .addComponent(idLabel)
                                .addGap(18, 18, 18)
                                .addComponent(idTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 320, javax.swing.GroupLayout.PREFERRED_SIZE)))))
                .addGap(22, 22, 22))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGap(25, 25, 25)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(msgLabel, javax.swing.GroupLayout.PREFERRED_SIZE, 21, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(newTypeNodeLabel))
                .addGap(20, 20, 20)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(idTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(idLabel))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(cancelButton)
                    .addComponent(createButton))
                .addGap(15, 15, 15))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents


	protected abstract void createButtonActionPerformed(java.awt.event.ActionEvent evt);//GEN-FIRST:event_createButtonActionPerformed
	//GEN-LAST:event_createButtonActionPerformed

	
	private void formWindowClosed(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_formWindowClosed
		//Si se cierra la ventana en la X,
		//quita el texto que hubiera en la etiqueta de "Alredy exists".
		msgLabel.setText("");
	}//GEN-LAST:event_formWindowClosed

	private void cancelButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cancelButtonActionPerformed
		msgLabel.setText("");
		setVisible(false);
		builderGUI.attemptChangeToNormalMode();
}//GEN-LAST:event_cancelButtonActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    protected javax.swing.JButton cancelButton;
    protected javax.swing.JButton createButton;
    private javax.swing.JLabel idLabel;
    protected javax.swing.JTextField idTextField;
    protected javax.swing.JLabel msgLabel;
    protected javax.swing.JLabel newTypeNodeLabel;
    // End of variables declaration//GEN-END:variables

}
