#include "AugmentedWindow.h"
#include "UR10.h"

#if USE_SIMULATOR == true
RTDEReceiveInterface rtde_receive("192.168.116.128"); //has to be initialized here, doesn't work otherwise
#endif // USE_SIMULATOR

Vector3 axe_x = Vector3(1, 0, 0);
Vector3 axe_y = Vector3(0, 1, 0);
Vector3 axe_z = Vector3(0, 0, 1);



UR10::UR10(Ogre::SceneManager* sceneMgr, Ogre::SceneNode* parentNode)
{
	printf("init of UR10 in Ogre environment\n");
	UR10_node[0] = parentNode->createChildSceneNode();
	buildUR10(sceneMgr);
#if USE_SIMULATOR == true
	mRTDEreceive = &rtde_receive;
#else 
	printf("WARNING: The simulator is not activated, many feature will be disactivated!\n");
#endif// USE_SIMULATOR
	printf("init of UR10 in Ogre environment is done!\n");
}

UR10::~UR10()
{

}

void UR10::updatePosition()
{
#if USE_SIMULATOR == true
	std::vector<double> joint_angles = getJointAngles();
	for (uint8_t i = 0; i < nb_piece_UR10 - 1; i++)
	{
		UR10_node[i + 1]->setOrientation(Quaternion(Radian(UR10_initOrientation[i + 1] + joint_angles[i]), UR10_axes[i + 1]));
	}
#endif //USE_SIMULATOR
}

void UR10::setBasePosition(Ogre::Vector3 pos)
{
	UR10_node[0]->setPosition(pos);
}

void UR10::setBaseOrientation(Ogre::Quaternion orientation)
{
	UR10_node[0]->setOrientation(orientation);
}


std::vector<double> UR10::getJointAngles()
{
#if USE_SIMULATOR == true
	return mRTDEreceive->getActualQ();
#else
	return std::vector < double> { 0 };
#endif //USE_SIMULATOR
}

Ogre::Vector3 UR10::getBasePosition()
{
	return UR10_node[0]->getPosition();
}

Ogre::Vector3 UR10::getToolPosition()
{
#if USE_SIMULATOR == true
	std::vector<double> joint_positions = mRTDEreceive->getActualTCPPose();
	return Ogre::Vector3(joint_positions[0], joint_positions[1], joint_positions[2]);
#else
	if (this)
		return UR10_node[nb_piece_UR10 - 1]->getPosition();
	else return Vector3::ZERO;
#endif //USE_SIMULATOR
}

Ogre::Vector3 UR10::getToolSpeed()
{
#if USE_SIMULATOR == true
	std::vector<double> joint_positions = mRTDEreceive->getActualTCPSpeed();
	return Ogre::Vector3(joint_positions[0], joint_positions[1], joint_positions[2]);
#else
	return Vector3::ZERO;
#endif //USE_SIMULATOR
}

Ogre::Vector3 UR10::getToolOrientation()
{
#if USE_SIMULATOR == true
	std::vector<double> joint_positions = mRTDEreceive->getActualTCPPose();
	return Ogre::Vector3(joint_positions[3], joint_positions[4], joint_positions[5]);
#else
	return Vector3::ZERO;
#endif
}



void UR10::buildUR10(Ogre::SceneManager* sceneMgr)
{
	
	UR10_entity[0] = sceneMgr->createEntity("1_Base_centered.mesh");
	UR10_entity[1] = sceneMgr->createEntity("2_Shoulder_center.mesh");
	UR10_entity[2] = sceneMgr->createEntity("3_UpperArm_centered.mesh");
	UR10_entity[3] = sceneMgr->createEntity("4_LowerArm_centered.mesh");
	UR10_entity[4] = sceneMgr->createEntity("5_Wrist_1_centered.mesh");
	UR10_entity[5] = sceneMgr->createEntity("6_Wrist_2.mesh");
	
	//The 0 can be used to place the robot/base where we want in the space
	UR10_position[1] = Ogre::Vector3(0, 38, 0);		//base to shoulder
	UR10_position[2] = Ogre::Vector3(0, 89.8, -87.3);	//shoulder to upper_arm
	UR10_position[3] = Ogre::Vector3(0, 612.1, -29);	//upper_arm to lower_arm
	UR10_position[4] = Ogre::Vector3(0, 572.2, 7.71);	//lower_arm to wrist_1
	UR10_position[5] = Ogre::Vector3(0, 60.4, -55.3);	//wrist_1 to wrist_2

	UR10_axes[0] = axe_y;
	UR10_axes[1] = axe_y;
	UR10_axes[2] = -axe_z;
	UR10_axes[3] = -axe_z;
	UR10_axes[4] = -axe_z;
	UR10_axes[5] = axe_y;

	UR10_initOrientation[0] = 0; // Degree(90).valueRadians();
	UR10_initOrientation[1] = Degree(90).valueRadians(); // Degree(180).valueRadians();
	UR10_initOrientation[2] = Degree(90).valueRadians();
	UR10_initOrientation[3] = 0;
	UR10_initOrientation[4] = Degree(90).valueRadians();
	UR10_initOrientation[5] = 0;

	UR10_node[0]->attachObject(UR10_entity[0]);
	for (uint8_t i = 1; i < nb_piece_UR10; i++)
	{
		UR10_node[i] = UR10_node[i - 1]->createChildSceneNode();
		UR10_node[i]->attachObject(UR10_entity[i]);
		UR10_node[i]->setPosition(UR10_position[i]);
		UR10_node[i]->setInheritOrientation(true);
	}
	printf("UR10 built\n");
}