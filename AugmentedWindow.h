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
//#include <OgreRenderWindow.h>
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
//#include "OgreFramework.hpp"

using namespace Ogre;
using namespace OgreBites;
using namespace ur_rtde;


class AugmentedWindow
	: public ApplicationContext
	, public InputListener
	, public TrayListener
	, public FrameListener
	, public OIS::KeyListener
	, public OIS::MouseListener
{
public:
	AugmentedWindow();
	virtual ~AugmentedWindow();

	virtual void setup();
	void updateRobotTextBox();
	void updateRobotPosition();
	void updateColaboratorTextBox();
	void moveCamera();
	virtual bool frameRenderingQueued(const FrameEvent& fe);

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
	OgreBites::TextBox* mIinformationBox;
	OgreBites::TextBox* mColaboratorBox;

	OIS::InputManager* mInputMgr;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;
	
	bool mShutdown;
	bool mBreakMove;
	uint8_t mMoveScale;
	Vector3 mTranslationVector;

	
	RTDEReceiveInterface* mRTDEreceive;

	//---------------UR10 data
	const static uint8_t nb_piece_UR10 = 6;
	Entity* UR10_entity[nb_piece_UR10];
	SceneNode* UR10_node[nb_piece_UR10];
	Ogre::Vector3 UR10_position[nb_piece_UR10];
	Ogre::Vector3 UR10_axes[nb_piece_UR10];
	Ogre::Real UR10_initOrientation[nb_piece_UR10];
	


};

void printVector(Vector3*, char*);