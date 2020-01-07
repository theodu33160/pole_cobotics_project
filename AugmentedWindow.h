#pragma once

//#include <ur_rtde/rtde_control_interface.h>
#include <ur_rtde/rtde_receive_interface.h>
#include <OgreCompositorManager.h>
#include <OgreCompositorInstance.h>
#include <OgreCompositor.h>
#include <SdkSample.h>
#include <OgreTrays.h>
#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include <iostream>
//#include <OgreRenderWindow.h>

using namespace Ogre;
using namespace OgreBites;
using namespace ur_rtde;


class AugmentedWindow
	: public ApplicationContext
	, public InputListener
	, public TrayListener
{
public:
	AugmentedWindow();
	virtual ~AugmentedWindow() {}

	void setup();
	bool keyPressed(const KeyboardEvent& evt);
	static void updateRobotTextBox();
	static void updateColaboratorTextBox(Camera* colab);

	Ogre::RenderWindow* m_renderWindow;

private:


};