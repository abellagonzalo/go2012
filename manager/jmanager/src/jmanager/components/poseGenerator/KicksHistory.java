package jmanager.components.poseGenerator;

import java.util.Stack;
import jmanager.components.poseGenerator.Snapshot;

public class KicksHistory {
	private Stack history;
	
	public KicksHistory()
	{
		history = new Stack();
	}
	
	public KicksHistory(Snapshot currentState)
	{
		history = new Stack();
		history.push(currentState);
	}
	
	public void update(Snapshot currentState)
	{
		history.push(currentState);
	}
	
	public Snapshot undo()
	{
		return (Snapshot)history.pop();
	}

	public boolean isEmpty()
	{
		return history.empty();
	}
	
	public void print()
	{
		Stack tmp = new Stack();
		tmp = (Stack)history.clone();
		Snapshot e = new Snapshot();
		while(!tmp.empty())
		{
			e = (Snapshot)tmp.pop();
			System.out.println("Frame: " + e.getFrame());
		}
	}
}
