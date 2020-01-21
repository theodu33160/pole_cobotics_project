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

//using namespace Ogre;
//using namespace OgreBites;
//using namespace ur_rtde;


class AugmentedWindow
	: public OgreBites::ApplicationContext
	, public OgreBites::InputListener
	, public OgreBites::TrayListener
	, public Ogre::FrameListener
{
public:
	AugmentedWindow();
	virtual ~AugmentedWindow() {}

	virtual void setup();
	virtual bool keyPressed(const OgreBites::KeyboardEvent& evt);
	void updateRobotTextBox();
	void updateColaboratorTextBox();
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);

	
protected:
	bool processUnbufferedInput(const Ogre::FrameEvent& fe);
	void setupBackground();
	void setupCamera();
	void setupTextBoxes();

	Ogre::Root* mRoot;
	Ogre::Camera* mCamera;
	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mRenderWindow;
	ur_rtde::RTDEReceiveInterface* mRTDEreceive;
	OgreBites::TextBox* mIinformationBox;
	OgreBites::TextBox* mColaboratorBox;
	
	Ogre::SceneNode* cameraNode;
	Ogre::SceneNode* cameraYawNode;
	Ogre::SceneNode* cameraPitchNode;
	Ogre::SceneNode* cameraRollNode;
	//test in the feature/camera branch

};