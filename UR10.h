#pragma once //to include the library only once

#include "Ogre.h"
#include <ur_rtde/rtde_receive_interface.h>

#define USE_SIMULATOR true //allow the use of the universal robot simulator in vmPlayer


using namespace ur_rtde;

class UR10 //: public Ogre::SceneNode
{
public:
	UR10(Ogre::SceneManager* sceneMgr, Ogre::SceneNode* parentNode);
	~UR10();

	void updatePosition();

	void setBasePosition(Ogre::Vector3 pos);
	void setBaseOrientation(Ogre::Quaternion orientation);
//	void updateToolPosition(Ogre::Vector3* pos); //this function has to calculate possible angles for all the joints. May be useless to implement
	
	std::vector<double> getJointAngles();
	Ogre::Vector3 getBasePosition();
	Ogre::Vector3 getToolPosition();
	Ogre::Vector3 getToolOrientation();


	
protected:
	void buildUR10(Ogre::SceneManager* sceneMgr);


	RTDEReceiveInterface* mRTDEreceive;

	const static uint8_t nb_piece_UR10 = 6;
	Ogre::Entity* UR10_entity[nb_piece_UR10];
	Ogre::SceneNode* UR10_node[nb_piece_UR10];
	Ogre::Vector3 UR10_position[nb_piece_UR10];
	Ogre::Vector3 UR10_axes[nb_piece_UR10];
	Ogre::Real UR10_initOrientation[nb_piece_UR10];
	


};