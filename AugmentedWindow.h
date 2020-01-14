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

using namespace Ogre;
using namespace OgreBites;
using namespace ur_rtde;


class AugmentedWindow
	: public ApplicationContext
	, public InputListener
	, public TrayListener
	, public FrameListener
{
public:
	AugmentedWindow();
	virtual ~AugmentedWindow() {}

	virtual void setup();
	virtual bool keyPressed(const KeyboardEvent& evt);
	void updateRobotTextBox();
	void updateColaboratorTextBox();
	virtual bool frameRenderingQueued(const FrameEvent& fe);

	
protected:
	bool processUnbufferedInput(const FrameEvent& fe);

	Ogre::Root* mRoot;
	Ogre::Camera* mCamera;
	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mRenderWindow;


};