# Motion Editor Server
# Author : Carlos Aguero caguero@gsyc.es
# Year : 2009


import socket
import select
import time
import sys

from motion_CurrentConfig import *

def poseInit():
	# Feel free to experiment with these values
	kneeAngle  = 40
	torsoAngle =  0
	wideAngle  =  0

	#----------------------------- prepare the angles ----------------------------
	#Get the Number of Joints
	NumJoints = len(motionProxy.getJointNames("Body"))
	#print NumJoints

	# Define The Initial Position
	Head     = [0, 0]
	LeftArm  = [120,  15, -90, -80]
	LeftLeg  = [0,  wideAngle, -kneeAngle/2-torsoAngle, kneeAngle, -kneeAngle/2, -wideAngle]
	RightLeg = [0, -wideAngle, -kneeAngle/2-torsoAngle, kneeAngle, -kneeAngle/2,  wideAngle]
	RightArm = [120, -15,  90,  80]

	# If we have hands, we need to add angles for wrist and open/close hand
	if (NumJoints == 26):
	    LeftArm  += [0, 0]
	    RightArm += [0, 0]

	# Gather the joints together
	pTargetAngles = Head + LeftArm + LeftLeg + RightLeg + RightArm		

	# Convert to radians
	pTargetAngles = [ x * motion.TO_RAD for x in pTargetAngles]

	#------------------------------ send the commands -----------------------------
	#We use the "Body" name to signify the collection of all joints
	pNames = "Body"
	#We set the fraction of max speed
	pMaxSpeedFraction = 0.2
	#Ask motion to do this with a blocking call
	motionProxy.angleInterpolationWithSpeed(pNames, pTargetAngles, pMaxSpeedFraction)
	return

editorPort = 50000

jointName = ["HeadYaw","HeadPitch","LShoulderPitch","LShoulderRoll","LElbowYaw","LElbowRoll","LHipYawPitch","LHipRoll" ,"LHipPitch" , "LKneePitch" , "LAnklePitch" , "LAnkleRoll" , "RHipYawPitch" , "RHipRoll" , "RHipPitch" , "RKneePitch" , "RAnklePitch" , "RAnkleRoll" , "RShoulderPitch" , "RShoulderRoll" , "RElbowYaw" , "RElbowRoll" ]

pose = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
error = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]

####
# Create python broker
try:	
	broker = ALBroker("pythonBroker","127.0.0.1",9999,IP,PORT)
	print("--------------------------------------\nConnected with NaoQi on " + str(IP) + ":" + str(PORT))	

except RuntimeError,e:
	print("Cannot connect with NaoQi on " + str(IP) + ":" + str(PORT))
	exit(1)

####
# Create motion proxy
try:
	motionProxy = ALProxy("ALMotion")	
	motionProxy.setStiffnesses('Body', 1.0)
except Exception,e:
	print "Error when creating motion proxy:"
	print str(e)
	exit(1)

# ==================== Socket Connection ==========================


kmeSocket = socket.socket ( socket.AF_INET, socket.SOCK_STREAM )

for i in range(10):
	try:
		kmeSocket.bind ( ( '', editorPort ) )
		break
	except Exception:
		editorPort += 1

print( "Waiting PoseGenerator at Port :" + str( editorPort ) + "\n--------------------------------------")

try:
	kmeSocket.listen( 1 )
except KeyboardInterrupt,e:
	print( "\nExitting...")
	print( str( e ) )	
	exit(1)

channel, details = kmeSocket.accept()
print( "Connection opened with" + str( details ))

received=" "

while True:

	try:		
		received = channel.recv ( 10240 )

		if( received == ""):
			break			
		
		elif( received.find("PoseInit") != -1 ):
			poseInit()

		elif( received.find("load") != -1 ):			
			sendValues = ""
						
			pose = motionProxy.getAngles('Body', True)			

			for index in range(len(pose)):
				sendValues += str( pose[index] ) + "%" 				
	
			sendValues += "\n"
			channel.send( sendValues )							
			
		elif (received.find( "setBodyAngles" ) != -1):						
			mytime = []
			allTimes = []
						
			values = received.split('%')			
			
			#Declare the array
			allJoints = []
			cont = 0
			for i in range(len(values)-2):
				frame = values[i+1].split('&')
				joints = (frame[0].split('$'))
				cont = cont + 1
			
			for i in range(len(joints)): # Articulaciones
				allJoints.append([0]*cont)
			
			#print(allJoints)
			
			time = 0.0
			for i in range(len(values)-2):   # Pasos
				frame = values[i+1].split('&')
				joints = (frame[0].split('$'))
				myjoints = []
				for j in range(len(joints)): # Articulaciones
					allJoints[j][i] = float(joints[j])
				time = time + float(frame[1])
				mytime.append(time)
			
			for i in range(len(joints)):
				allTimes.append(mytime)
			
			#print("Joints:\n")	
			#print(allJoints)
			#print("Times:\n")
			#print(allTimes)
						
			motionProxy.angleInterpolation(jointName, allJoints, allTimes, True)
			
		elif (received.find("setAngle") != -1):			
			values = received.split('%')			
			joint = str(values[1])
			angle = float(values[2])
		
			motionProxy.post.angleInterpolation(joint, angle, 0.5, True)
		
		elif( received.find("stif") != -1):						
			
			if(received.find("stif_on") != -1 ):
				
				values = received.split('%')
				stifValue = float(values[1])
				
				if( received.find("head_stif_on") != -1 ):
					motionProxy.setStiffnesses("Head",stifValue)
				
				elif( received.find("l_leg_stif_on") != -1 ):
					motionProxy.setStiffnesses("LLeg",stifValue)
			
				elif( received.find("r_leg_stif_on") != -1 ):
					motionProxy.setStiffnesses("RLeg",stifValue)							
					
				elif( received.find("r_arm_stif_on") != -1 ):
					motionProxy.setStiffnesses("RArm",stifValue)
					
				elif( received.find("l_arm_stif_on") != -1 ):
					motionProxy.setStiffnesses("LArm",stifValue)
					
				elif( received.find("stif_on") != -1 ):
					motionProxy.setStiffnesses("Body",stifValue)
					
			else:
				
				if( received.find("head_stif_off") != -1 ):
					motionProxy.setStiffnesses("Head",0.0)
				
				elif( received.find("l_arm_stif_off") != -1 ):	
					motionProxy.setStiffnesses("LArm",0.0)

				elif( received.find("r_arm_stif_off") != -1 ):	
					motionProxy.setStiffnesses("RArm",0.0)

				elif( received.find("l_leg_stif_off") != -1 ):
					motionProxy.setStiffnesses("LLeg",0.0)

				elif( received.find("r_leg_stif_off") != -1 ):	
					motionProxy.setStiffnesses("RLeg",0.0)

				elif( received.find("stif_off") != -1 ):	
					motionProxy.setStiffnesses("Body",0.0)
		

		received =" "
	except KeyboardInterrupt,e:
		print( "\n\nClosing Connection..." )
		print( str( e ) )
		break


channel.shutdown( 0 )
channel.close()

print( "\n\nConnection Closed!")

