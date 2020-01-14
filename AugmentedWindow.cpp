#include "AugmentedWindow.h"


using namespace Ogre;
using namespace OgreBites;
using namespace ur_rtde;

RTDEReceiveInterface rtde_receive("192.168.146.128");
TextBox* informationBox;
TextBox* colaboratorBox;

AugmentedWindow::AugmentedWindow()
	: ApplicationContext("OgreTutorialApp"),
	mRoot(0),
	mCamera(0),
	mSceneMgr(0),
	mRenderWindow(0)
{
}

bool AugmentedWindow::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
	bool ret = ApplicationContextBase::frameRenderingQueued(fe);
	if (!processUnbufferedInput(fe))
		return false;
	return ret;
}

bool AugmentedWindow::processUnbufferedInput(const FrameEvent& fe)
{
	//here the code you want to be updated each frame
	updateRobotTextBox();
	updateColaboratorTextBox();
	return true;
}

void AugmentedWindow::updateRobotTextBox()
{
	std::vector<double> joint_positions = rtde_receive.getActualTCPPose();
	Ogre::UTFString text = "pos robot:\t";
	for (uint8_t i = 0; i < 3; i++)
	{
		text.append(std::to_string(joint_positions[i]));
		text.append(", ");
	}
	text.append("\nangles:\t\t");
	for (uint8_t i = 3; i < 5; i++)
	{
		text.append(std::to_string(joint_positions[i]));
		text.append(", ");
	}
	text.append(std::to_string(joint_positions[0]));
	informationBox->setText(text);
	//printf("\n\npos robot_inside update = %s, %s, %s \t angles robot = %lf, %lf, %lf\n\n", std::to_string(joint_positions[0]), std::to_string(joint_positions[1]), std::to_string(joint_positions[2]), joint_positions[3], joint_positions[4], joint_positions[5]);
}

void AugmentedWindow::updateColaboratorTextBox()
{
	Ogre::Vector3 joint_positions = mCamera->getRealPosition();
	Ogre::UTFString text = "pos robot:\t";
	for (uint8_t i = 0; i < 2; i++)
	{
		text.append(std::to_string(joint_positions[i]));
		text.append(", ");
	}
	text.append(std::to_string(joint_positions[2]));
	/*
	text.append("\nangles:\t\t");
	for (uint8_t i = 3; i < 5; i++)
	{
		text.append(std::to_string(joint_positions[i]));
		text.append(", ");
	}
	text.append(std::to_string(joint_positions[0]));
	*/
	colaboratorBox->setText(text);
	//this->mRenderWindow->setDebugText("jeg elser å spise!");

}

void AugmentedWindow::setupBackground()
{
	// register our scene with the RTSS
	RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(mSceneMgr);

	// -- tutorial section start --
	//! [turnlights]
	mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
	//! [turnlights]

	//! [newlight]
	Light* light = mSceneMgr->createLight("MainLight");
	SceneNode* lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	lightNode->attachObject(light);
	//! [newlight]

	//! [lightpos]
	lightNode->setPosition(20, 80, 50);
	//! [lightpos]

	//! [entity1]
	Entity* ogreEntity = mSceneMgr->createEntity("ogrehead.mesh");
	//! [entity1]

	//! [entity1node]
	SceneNode* ogreNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	//! [entity1node]

	//! [entity1nodeattach]
	ogreNode->attachObject(ogreEntity);
	//! [entity1nodeattach]

	//! [entity2]
	Entity* ogreEntity2 = mSceneMgr->createEntity("ogrehead.mesh");
	SceneNode* ogreNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(84, 48, 0));
	ogreNode2->attachObject(ogreEntity2);
	//! [entity2]

	//! [entity3]
	Entity* ogreEntity3 = mSceneMgr->createEntity("ogrehead.mesh");
	SceneNode* ogreNode3 = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ogreNode3->setPosition(0, 104, 0);
	ogreNode3->setScale(2, 1.2, 1);
	ogreNode3->attachObject(ogreEntity3);
	//! [entity3]

	//! [entity4]
	Entity* ogreEntity4 = mSceneMgr->createEntity("ogrehead.mesh");
	SceneNode* ogreNode4 = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ogreNode4->setPosition(-84, 48, 0);
	ogreNode4->roll(Degree(-90));
	ogreNode4->attachObject(ogreEntity4);
	//! [entity4]
	// -- tutorial section end --
}

void AugmentedWindow::setupCamera()
{
	//! [camera]
	SceneNode* camNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	
	// create the camera
	mCamera = mSceneMgr->createCamera("myCam");
	mCamera->setNearClipDistance(5); // specific to this sample
	mCamera->setAutoAspectRatio(true);
	camNode->attachObject(mCamera);
	camNode->setPosition(0, 47, 222); // mooving the camera



	// and tell it to render into the main window
	mRenderWindow = getRenderWindow();
	mRenderWindow->addViewport(mCamera);
	//! [camera]
}

void AugmentedWindow::setup()
{
	// do not forget to call the base first
	ApplicationContext::setup();
	addInputListener(this);


	// get a pointer to the already created root
	mRoot = getRoot();
	mSceneMgr = mRoot->createSceneManager();
	mRoot->addFrameListener(this);
	mSceneMgr->addRenderQueueListener(mOverlaySystem);

	//setting up the background of the window
	setupBackground();

	//seting up the camera  which look at that background so that we can see it on the window
	setupCamera();
	

	TrayManager* mTrayMgr = new TrayManager("InterfaceName", ApplicationContext::getRenderWindow());
	//addInputListener(mTrayMgr);
	std::vector<double> joint_positions = rtde_receive.getActualQ();
	informationBox = mTrayMgr->createTextBox(TL_TOPLEFT, "InformationTextBox", "information from the robot", 400, 100);
	updateRobotTextBox();

	colaboratorBox = mTrayMgr->createTextBox(TL_TOPLEFT, "ColaboratorTextBox", "information about the colaborator", 400, 80);
	updateColaboratorTextBox();
	//add a button
	/*
	OgreBites::Button* mButton;
	mButton = mTrayMgr->createButton(TL_LEFT, "MyButton", "click me");
	//mButton->addEventHandler(QuickGUI::Widget::EVENT_MOUSE_BUTTON_UP, &txs::main_admin_screen::AugmentedWindow::testButton, this);
	*/
}


bool AugmentedWindow::keyPressed(const KeyboardEvent& evt)
{
	if (evt.keysym.sym == SDLK_ESCAPE)
	{
		getRoot()->queueEndRendering();
	}
	return true;
}

/*
void Root::startRendering(void)
{
	OgreAssert(mActiveRenderer != 0, "no RenderSystem");

	mActiveRenderer->_initRenderTargets();

	// Clear event times
	clearEventTimes();

	// Infinite loop, until broken out of by frame listeners
	// or break out by calling queueEndRendering()
	mQueuedEnd = false;

	while (!mQueuedEnd)
	{
		AugmentedWindow::updateRobotTextBox();
		if (!renderOneFrame())
			break;
	}
}
*/