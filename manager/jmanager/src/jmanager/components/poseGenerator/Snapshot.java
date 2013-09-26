package jmanager.components.poseGenerator;;

import jmanager.components.poseGenerator.Kick;

public class Snapshot {
	private Kick currentKick;
	private int	currentFrame;
	private int currentComboBoxEntry;
	
	public Snapshot()
	{
		currentKick = new Kick();
		currentFrame = 0;
		currentComboBoxEntry = 0;
	}
	
	public Snapshot(Kick k, int frame, int index)
	{
		currentKick = new Kick();
		currentKick = (Kick)k.clone();
		currentFrame = frame;
		currentComboBoxEntry = index;
	}
	
	public Kick getKick()
	{
		return (Kick)currentKick.clone();
	}
	
	public int getFrame()
	{
		return currentFrame;
	}
	
	public int getComboBoxEntry()
	{
		return currentComboBoxEntry;
	}
	
	public void setKick(Kick k)
	{
		currentKick = (Kick)k.clone();
	}
	
	public void setFrame(int frame)
	{
		currentFrame = frame;
	}
	
	public void setComboBoxEntry(int index)
	{
		currentComboBoxEntry = index;
	}
}
