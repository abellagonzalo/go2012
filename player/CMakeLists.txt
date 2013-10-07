cmake_minimum_required(VERSION 2.8)
project(player)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Add GStreamer-0.10
find_package(GStreamer)
include_directories(${GSTREAMER_INCLUDE_DIR})
add_definitions(${GSTREAMER_DEFINITIONS})

# Include for qibuild
include(qibuild.cmake)

option(PLAYER_IS_REMOTE
  "is player remote?"
  ON
)

option(WEBOTS
  "Are you using webots?"
  OFF
)

# Generar los ficheros de Bica y las adaptaciones de Bica para 
# JdeRobot. Los ficheros se crean los respectivos directorios
# en src/interfaces/cpp/ .
add_subdirectory(src/interfaces)

# Todos los directorios con headers
include_directories(
	src/interfaces/cpp/bica 
	src/interfaces/slice/bica 
	src/interfaces/cpp/jderobot 
	src/interfaces/slice/jderobot 
	src/interfaces/cpp 
	src/interfaces/slice 
	src/Components 
	src/Modules 
	src/lib 
	src/lib/Sched 
	src/lib/Localization
	src/lib/Math
	src/lib/vision
	src/lib/vision/recognizer 
	src/lib/vision/segmentation 
	src/lib/estimation
	src/lib/estimation 
	src/lib/Localization
	src/lib/Localization/EKF 
	src/lib/Localization/SRL 
	src/Components/vision/detectors 
	src/Components/Localization 
	src/Components/Attention 
	src/Components/NaoServer 
	src/Components/Attention
	src/Components/AttentionClient
	include
)

# Todas las fuentes del proyecto
set(_srcs
src/interfaces/cpp/bica/exceptions.cpp src/interfaces/cpp/bica/exceptions.h
src/interfaces/cpp/bica/containers.cpp src/interfaces/cpp/bica/containers.h
src/interfaces/cpp/bica/datetime.cpp src/interfaces/cpp/bica/datetime.h
src/interfaces/cpp/bica/common.cpp src/interfaces/cpp/bica/common.h
src/interfaces/cpp/bica/image.cpp src/interfaces/cpp/bica/image.h
src/interfaces/cpp/bica/schedulerManager.cpp src/interfaces/cpp/bica/schedulerManager.h
src/interfaces/cpp/bica/localizationI.cpp src/interfaces/cpp/bica/localizationI.h
src/interfaces/cpp/bica/teamInfoI.cpp src/interfaces/cpp/bica/teamInfoI.h
src/interfaces/cpp/bica/componentsI.cpp src/interfaces/cpp/bica/componentsI.h
src/interfaces/cpp/bica/motionManagerI.cpp src/interfaces/cpp/bica/motionManagerI.h
src/interfaces/cpp/bica/multimediaI.cpp src/interfaces/cpp/bica/multimediaI.h
src/interfaces/cpp/bica/debug.cpp src/interfaces/cpp/bica/debug.h
src/interfaces/cpp/bica/logI.cpp src/interfaces/cpp/bica/logI.h
src/interfaces/cpp/bica/demencia.cpp src/interfaces/cpp/bica/demencia.h
src/interfaces/cpp/jderobot/exceptions_jderobot.cpp src/interfaces/cpp/jderobot/exceptions_jderobot.h
src/interfaces/cpp/jderobot/containers_jderobot.cpp src/interfaces/cpp/jderobot/containers_jderobot.h
src/interfaces/cpp/jderobot/datetime_jderobot.cpp src/interfaces/cpp/jderobot/datetime_jderobot.h
src/interfaces/cpp/jderobot/common_jderobot.cpp src/interfaces/cpp/jderobot/common_jderobot.h
src/interfaces/cpp/jderobot/image_jderobot.cpp src/interfaces/cpp/jderobot/image_jderobot.h
src/interfaces/cpp/jderobot/camera.cpp src/interfaces/cpp/jderobot/camera.h
src/interfaces/cpp/jderobot/motors.cpp src/interfaces/cpp/jderobot/motors.h
src/interfaces/cpp/jderobot/encoders.cpp src/interfaces/cpp/jderobot/encoders.h
src/interfaces/cpp/jderobot/pose3dmotors.cpp src/interfaces/cpp/jderobot/pose3dmotors.h
src/interfaces/cpp/jderobot/pose3dencoders.cpp src/interfaces/cpp/jderobot/pose3dencoders.h
src/lib/vision/VisionDefinitions.h
src/lib/vision/ImageInput.cpp src/lib/vision/ImageInput.h
src/lib/vision/ImageHandler.cpp src/lib/vision/ImageHandler.h
src/lib/vision/Lut.cpp src/lib/vision/Lut.h
src/lib/vision/colorFilter/ColorFilter.cpp src/lib/vision/colorFilter/ColorFilter.h
src/lib/vision/colorFilter/simpleColorFilter/SimpleColorFilter.cpp src/lib/vision/colorFilter/simpleColorFilter/SimpleColorFilter.h
src/lib/vision/segmentation/Segment.cpp src/lib/vision/segmentation/Segment.h
src/lib/vision/segmentation/Region.cpp src/lib/vision/segmentation/Region.h
src/lib/vision/segmentation/Segmentation.cpp src/lib/vision/segmentation/Segmentation.h
src/lib/vision/segmentation/RegionBuilder.cpp src/lib/vision/segmentation/RegionBuilder.h
src/lib/vision/segmentation/RegionGeometry.cpp src/lib/vision/segmentation/RegionGeometry.h
src/lib/vision/segmentation/AbstractRegion.cpp src/lib/vision/segmentation/AbstractRegion.h
src/lib/vision/segmentation/BallRegion.cpp src/lib/vision/segmentation/BallRegion.h
src/lib/vision/segmentation/LineRegion.cpp src/lib/vision/segmentation/LineRegion.h
src/lib/vision/segmentation/RobotRegion.cpp src/lib/vision/segmentation/RobotRegion.h
src/lib/vision/recognizer/Horizon.cpp src/lib/vision/recognizer/Horizon.h
src/lib/vision/recognizer/Recognizer.cpp src/lib/vision/recognizer/Recognizer.h
src/lib/vision/recognizer/BallRecognizer.cpp src/lib/vision/recognizer/BallRecognizer.h
src/lib/Localization/ObservationModel.cpp src/lib/Localization/ObservationModel.h
src/lib/Localization/MovementModel.cpp src/lib/Localization/MovementModel.h
src/lib/Localization/LocalizationHealth.cpp src/lib/Localization/LocalizationHealth.h
src/lib/Localization/EKF/EKF.cpp src/lib/Localization/EKF/EKF.h
src/lib/Localization/EKF/EKF_3D.cpp src/lib/Localization/EKF/EKF_3D.h
src/lib/Localization/EKF/EKF_1D.cpp src/lib/Localization/EKF/EKF_1D.h
src/lib/Localization/EKF/MarkovGrid.cpp src/lib/Localization/EKF/MarkovGrid.h
src/lib/Localization/SRL/Pose.cpp src/lib/Localization/SRL/Pose.h
src/lib/Localization/SRL/num_util.cpp src/lib/Localization/SRL/num_util.h
src/lib/Localization/SRL/Vector2D.h
src/lib/Math/Geometry.cpp src/lib/Math/Geometry.h
src/lib/Math/GaussianDistribution2D.cpp src/lib/Math/GaussianDistribution2D.h
src/lib/Net/TcpServer.cpp src/lib/Net/TcpServer.h 
src/lib/Net/UdpServer.cpp src/lib/Net/UdpServer.h 
src/lib/Net/UdpClient.cpp src/lib/Net/UdpClient.h 
src/lib/Sched/CascadeScheduler.cpp src/lib/Sched/CascadeScheduler.h
src/lib/PIDController.cpp src/lib/PIDController.h
src/lib/Leds.cpp src/lib/Leds.h
src/lib/Dictionary.cpp src/lib/Dictionary.h
src/lib/Matrix.cpp src/lib/Matrix.h
src/lib/RoboCupGameControlData.h
src/lib/progeo.cpp src/lib/progeo.h
src/lib/GameController.cpp src/lib/GameController.h
src/lib/WorldModel.cpp src/lib/WorldModel.h
src/lib/Calibration.cpp src/lib/Calibration.h
src/lib/JointControl.cpp src/lib/JointControl.h
src/lib/FSR.cpp src/lib/FSR.h
src/lib/Speech.cpp src/lib/Speech.h
src/lib/Register.cpp src/lib/Register.h
src/lib/Common.h
src/lib/estimation/ObjectState.h
src/lib/estimation/SingleObjectModel.h src/lib/estimation/SingleObjectModel.cpp
src/lib/estimation/GoalsModel.h src/lib/estimation/GoalsModel.cpp
src/lib/estimation/RobotModel.h src/lib/estimation/RobotModel.cpp
src/lib/estimation/kalman/Kalman.h src/lib/estimation/kalman/Kalman.cpp
src/lib/estimation/JPDAF.h src/lib/estimation/JPDAF.cpp
src/lib/log/ComponentSnapshot.h src/lib/log/ComponentSnapshot.cpp
src/Components/vision/detectors/Detector.cpp src/Components/vision/detectors/Detector.h
src/Components/vision/detectors/BallDetector.cpp src/Components/vision/detectors/BallDetector.h
src/Components/vision/detectors/W3DDetector.cpp src/Components/vision/detectors/W3DDetector.h
src/Components/vision/detectors/LineDetector.cpp src/Components/vision/detectors/LineDetector.h
src/Components/vision/detectors/GoalDetector.cpp src/Components/vision/detectors/GoalDetector.h
src/Components/vision/detectors/BGDetector.cpp src/Components/vision/detectors/BGDetector.h
src/Components/vision/detectors/RobotDetector.cpp src/Components/vision/detectors/RobotDetector.h
src/Components/Localization/Localization.cpp src/Components/Localization/Localization.h
src/Components/Localization/AbstractLocalization.cpp src/Components/Localization/AbstractLocalization.h
src/Components/Localization/GeneticLocalization.cpp src/Components/Localization/GeneticLocalization.h
src/Components/Localization/MontecarloLocalization.cpp src/Components/Localization/MontecarloLocalization.h
src/Components/Localization/GTLocalization.cpp src/Components/Localization/GTLocalization.h
src/Components/Localization/NEKFLocalization.cpp src/Components/Localization/NEKFLocalization.h
src/Components/Localization/EKFLocalization.cpp src/Components/Localization/EKFLocalization.h
src/Components/Localization/SRL.cpp src/Components/Localization/SRL.h
src/Components/NaoServer/NaoServerCamera.cpp src/Components/NaoServer/NaoServerCamera.h
src/Components/NaoServer/NaoServerMotors.cpp src/Components/NaoServer/NaoServerMotors.h
src/Components/NaoServer/NaoServerEncoders.cpp src/Components/NaoServer/NaoServerEncoders.h
src/Components/NaoServer/NaoServerPose3DMotors.cpp src/Components/NaoServer/NaoServerPose3DMotors.h
src/Components/NaoServer/NaoServerPose3DEncoders.cpp src/Components/NaoServer/NaoServerPose3DEncoders.h
src/Components/Debug.cpp src/Components/Debug.h
src/Components/Striker2.cpp src/Components/Striker2.h
src/Components/Component.cpp src/Components/Component.h
src/Components/Perception.cpp src/Components/Perception.h
src/Components/FollowBall.cpp src/Components/FollowBall.h
src/Components/SearchBall.cpp src/Components/SearchBall.h
src/Components/SearchNet.cpp src/Components/SearchNet.h
src/Components/Body.cpp src/Components/Body.h
src/Components/Head.cpp src/Components/Head.h
src/Components/Fallen.cpp src/Components/Fallen.h
src/Components/Kinematics.cpp src/Components/Kinematics.h
src/Components/pruebag2gpos.cpp src/Components/pruebag2gpos.h
src/Components/PruebaAttention.cpp src/Components/PruebaAttention.h
src/Components/Go2gpos.cpp src/Components/Go2gpos.h
src/Components/Go2gposObs.cpp src/Components/Go2gposObs.h
src/Components/Go2pos.cpp src/Components/Go2pos.h
src/Components/BodyDetector.cpp src/Components/BodyDetector.h
src/Components/Kick.cpp src/Components/Kick.h
src/Components/Attention/Attention.cpp src/Components/Attention/Attention.h
src/Components/Attention/AbstractAttention.cpp src/Components/Attention/AbstractAttention.h
src/Components/Attention/AttentionRR.cpp src/Components/Attention/AttentionRR.h
src/Components/Attention/AttentionQ.cpp src/Components/Attention/AttentionQ.h
src/Components/Attention/AttentionP.cpp src/Components/Attention/AttentionP.h
src/Components/AttentionClient/AttentionClient.cpp src/Components/AttentionClient/AttentionClient.h
src/Components/AttentionClient/BallAttentionClient.cpp src/Components/AttentionClient/BallAttentionClient.h
src/Components/AttentionClient/GoalAttentionClient.cpp src/Components/AttentionClient/GoalAttentionClient.h
src/Components/AttentionClient/W3DAttentionClient.cpp src/Components/AttentionClient/W3DAttentionClient.h
src/Components/AttentionClient/StrategyAttentionClient.cpp src/Components/AttentionClient/StrategyAttentionClient.h
src/Components/AttentionClient/StrategyAttentionClientBallQ.cpp src/Components/AttentionClient/StrategyAttentionClientBallQ.h
src/Components/AttentionClient/StrategyAttentionClientBallRR.cpp src/Components/AttentionClient/StrategyAttentionClientBallRR.h
src/Components/AttentionClient/StrategyAttentionClientBallP.cpp src/Components/AttentionClient/StrategyAttentionClientBallP.h
src/Components/AttentionClient/StrategyAttentionClientGoalQ.cpp src/Components/AttentionClient/StrategyAttentionClientGoalQ.h
src/Components/AttentionClient/StrategyAttentionClientGoalRR.cpp src/Components/AttentionClient/StrategyAttentionClientGoalRR.h
src/Components/AttentionClient/StrategyAttentionClientGoalP.cpp src/Components/AttentionClient/StrategyAttentionClientGoalP.h
src/Components/AttentionClient/StrategyAttentionClientW3D.cpp src/Components/AttentionClient/StrategyAttentionClientW3D.h
src/Components/Stratego.cpp src/Components/Stratego.h
src/Components/MatchPlayer.cpp src/Components/MatchPlayer.h
src/Components/StrategyRoles.cpp src/Components/StrategyRoles.h
src/Components/SharedTeamInfo.cpp src/Components/SharedTeamInfo.h
src/Components/Music.cpp src/Components/Music.h
src/Components/TextSpeech.cpp src/Components/TextSpeech.h
src/Components/Movie.cpp src/Components/Movie.h
src/Components/LedsControl.cpp src/Components/LedsControl.h
src/Components/FootContact.cpp src/Components/FootContact.h
src/Components/ApproachBall.cpp src/Components/ApproachBall.h
src/Components/GoalieURJC.cpp src/Components/GoalieURJC.h
src/Components/PGoalie.cpp src/Components/PGoalie.h
src/Components/AuxRole.cpp src/Components/AuxRole.h
src/Components/AuxPenalty.cpp src/Components/AuxPenalty.h
src/Components/RobotLog.cpp src/Components/RobotLog.h
src/Components/WorldState.cpp src/Components/WorldState.h
src/Components/SensorRecorderNao.cpp src/Components/SensorRecorderNao.h
src/Components/RobotUSDetector.cpp src/Components/RobotUSDetector.h
src/Components/WorldState.cpp src/Components/WorldState.h
src/Components/RecorderSession.cpp src/Components/RecorderSession.h
src/Components/PlayerSession.cpp src/Components/PlayerSession.h
#src/Components/demencia/AleatoryMoveComponent.cpp src/Components/demencia/AleatoryMoveComponent.h
src/motions/forwardKick.cpp
src/motions/forwardDribble.cpp
src/motions/forwardStrong.cpp
src/motions/obliqueKick.cpp
src/motions/sideKickOut.cpp
src/motions/sideKickMiniIn.cpp
src/motions/StandUp.cpp
src/motions/sideKickStrongIn.cpp
src/motions/backwardKickLeft.cpp
src/motions/backwardKickRight.cpp
src/motions/suicide.cpp
src/motions/fisioCabezaArribaAbajo.cpp
src/motions/fisioCabezaIzquierdaDerecha.cpp
src/motions/fisioBrazosParalelosArribaAbajo.cpp
src/motions/fisioBrazosAlternosArribaAbajo.cpp
src/motions/fisioBrazosCruzAbrirCerrar.cpp
src/motions/fisioBrazosTocarHombro.cpp
src/motions/fisioMunecaPalmaVuelta.cpp
src/Modules/Coach.cpp
src/Player.cpp src/Player.h
)

if(WEBOTS)
	 add_definitions(" -DWEBOTS ")
endif()

if(PLAYER_IS_REMOTE)
  add_definitions(" -DPLAYER_IS_REMOTE ")
  qi_create_bin(player ${_srcs})
else()
  qi_create_lib(player ${_srcs} SHARED SUBFOLDER naoqi)
endif()

target_link_libraries(player ${GSTREAMER_LIBRARIES})
qi_use_lib(player ALCOMMON LIBICEE OPENCV)

