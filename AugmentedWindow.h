#pragma once //to include the library only once

//#include <ur_rtde/rtde_control_interface.h>
#include <ur_rtde/rtde_receive_interface.h>
//#include <OgreCompositorManager.h>
//#include <OgreCompositorInstance.h>
//#include <OgreCompositor.h>
#include <SdkSample.h>
#include <OgreTrays.h>
#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreFrameListener.h"
#include <iostream>
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include "UR10.h"




//-----------GENERAL SETTINGS-----------------------
#define AMBIENT_BRIGHTNESS 0.3
#define ENABLE_COLLABORATOR_TEXT_BOX true
#define ENABLE_UR10_TEXT_BOX false

//-----------MOVE SETTINGS-----------------------
#define ROTATE_SCALE Ogre::Math::PI/180
#define TRANSLATE_SCALE 20

//-----------CIRCLE LIGH ON COLLABORATORS-----------
#define CIRCLE_LIGHT_BRIGHTNESS 2.
#define DIST_COLOR_CHANGE 1000.
#define DIST_SPREAD_COLOR 400.
#define HEIGHT_LIGHT 150
#define RADIUS_LIGHT 145 //max 180, increase HEIGHT otherwise

using namespace Ogre;
using namespace OgreBites;
using namespace ur_rtde;


class AugmentedWindow
	: public ApplicationContext
	, public InputListener
	, public TrayListener
//	, public FrameListener //should be removable
	, public OIS::KeyListener
	, public OIS::MouseListener
{
public:
	AugmentedWindow();
	virtual ~AugmentedWindow();

	virtual void setup();
	virtual bool frameRenderingQueued(const FrameEvent& fe);

	//----general display--------------------------
	void updateRobotTextBox();
	void updateColaboratorTextBox();
	void updateSafetyBox();
    void updateInfoBox();
	void updateCircleLight();

	//----Camera-----------------------------------
	void moveCamera();

	//----Safety calculation-----------------------
	Ogre::Vector3 getRelativeSpeedCollaboratorRobot_v(UR10* robot);//Relative speed of the collaborator with regards to the Robot
	Ogre::Vector3 getRelativeDistanceCollaboratorRobot_v(UR10* robot);

    void highlight(Ogre::Entity* entity);
    void unhighlight(Ogre::Entity* entity);
   

    
	double timeBeforeCollision(UR10* robot, float radius); //return 0 if no colision
	
	//todo : should it be virtual?
	bool keyPressed(const OIS::KeyEvent& keyEventRef);
	bool keyReleased(const OIS::KeyEvent& keyEventRef);

	bool mouseMoved(const OIS::MouseEvent& evt);
	bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id);

	
protected:
	bool processUnbufferedInput(const FrameEvent& fe);
	void setupBackground();
	void setupCamera();
	void setupTextBoxes();

	Ogre::SceneNode* mCameraNode;
	Ogre::SceneNode* mCameraYawNode;
	Ogre::SceneNode* mCameraPitchNode;
	Ogre::SceneNode* mCameraRollNode;
	//Ogre::SceneNode* mCubeNode;

	Ogre::Root* mRoot;
	Ogre::Camera* mCamera;
	Ogre::Viewport* mViewport;
	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mRenderWindow;
	OgreBites::TextBox* mRobotBox;
	OgreBites::TextBox* mColaboratorBox;
	OgreBites::TextBox* mSafetyBox;
    OgreBites::TextBox* mInfoBox;

	OIS::InputManager* mInputMgr;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;
	
	bool mShutdown;
	bool mBreakMove;
	Vector3 mArrowVector;

	UR10* mRobot;
    SceneNode* collabLightNode;
    Light* collabLight;
	Entity* collabEntity;
	SkeletonInstance* collabSkeleton;

    enum collabBonesEnum {
        shin_01L,
        thigh_01L,
        shin_01R,
        thigh_01R,
        hips,
        spine,
        chest,
        neck,
        head,
        shoulderL,
        upper_armL,
        forearmL,
        handL,
        palm_4L,
        f_pinky_1L,
        f_pinky_2L,
        f_pinky_3L,
        palm_3L,
        f_ring_1L,
        f_ring_2L,
        f_ring_3L,
        palm_2L,
        f_middle_1L,
        f_middle_2L,
        f_middle_3L,
        palm_1L,
        f_index_1L,
        f_index_2L,
        f_index_3L,
        thumb_1L,
        thumb_2L,
        thumb_3L,
        shoulderR,
        upper_armR,
        forearmR,
        handR,
        palm_4R,
        f_pinky_1R,
        f_pinky_2R,
        f_pinky_3R,
        palm_3R,
        f_ring_1R,
        f_ring_2R,
        f_ring_3R,
        palm_2R,
        f_middle_1R,
        f_middle_2R,
        f_middle_3R,
        palm_1R,
        f_index_1R,
        f_index_2R,
        f_index_3R,
        thumb_1R,
        thumb_2R,
        thumb_3R,
        thighL,
        shinL,
        footL,
        toeL,
        thighR,
        shinR,
        footR,
        toeR,
        NB_COLLAB_BONES
    };
    /*
    const char* boneNames[NB_COLLAB_BONES] = {
        "shin_01L",
        "thigh_01L",
        "shin_01R",
        "thigh_01R",
        "hips",
        "spine",
        "chest",
        "neck",
        "head",
        "shoulderL",
        "upper_armL",
        "forearmL",
        "handL",
        "palm_4L",
        "f_pinky_1L",
        "f_pinky_2L",
        "f_pinky_3L",
        "palm_3L",
        "f_ring_1L",
        "f_ring_2L",
        "f_ring_3L",
        "palm_2L",
        "f_middle_1L",
        "f_middle_2L",
        "f_middle_3L",
        "palm_1L",
        "f_index_1L",
        "f_index_2L",
        "f_index_3L",
        "thumb_1L",
        "thumb_2L",
        "thumb_3L",
        "shoulderR",
        "upper_armR",
        "forearmR",
        "handR",
        "palm_4R",
        "f_pinky_1R",
        "f_pinky_2R",
        "f_pinky_3R",
        "palm_3R",
        "f_ring_1R",
        "f_ring_2R",
        "f_ring_3R",
        "palm_2R",
        "f_middle_1R",
        "f_middle_2R",
        "f_middle_3R",
        "palm_1R",
        "f_index_1R",
        "f_index_2R",
        "f_index_3R",
        "thumb_1R",
        "thumb_2R",
        "thumb_3R",
        "thighL",
        "shinL",
        "footL",
        "toeL",
        "thighR",
        "shinR",
        "footR",
        "toeR" 
    };
    */
    collabBonesEnum mCurrentBone;
};
