#include "AugmentedWindow.h"

using namespace Ogre;
using namespace OgreBites;
using namespace ur_rtde;

RTDEReceiveInterface rtde_receive("192.168.146.128");

AugmentedWindow::AugmentedWindow()
	: ApplicationContext("OgreTutorialApp"),
	mRoot(0),
	mCamera(0),
	mViewport(0),
	mSceneMgr(0),
	mRenderWindow(0),
	mIinformationBox(0),
	mColaboratorBox(0),
	mInputMgr(0),
	mKeyboard(0),
	mMouse(0),
	mShutdown(false),
	mMoveScale(3),
	mBreakMove(false)
{	
	mRTDEreceive = &rtde_receive;
	mTranslationVector = Vector3::ZERO;
}

AugmentedWindow::~AugmentedWindow()
{
}

bool AugmentedWindow::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
	bool ret = ApplicationContextBase::frameRenderingQueued(fe);
	if (!processUnbufferedInput(fe))
		return false;
	if (mShutdown)
	{
		printf("You pressed escape so we are exiting\n");
		return false; //exit if we shutdown (press escape)
	}
	return ret;
}

bool AugmentedWindow::processUnbufferedInput(const FrameEvent& fe)
{
	//here the code you want to be updated each frame
	updateRobotTextBox();
	updateColaboratorTextBox();
	mKeyboard->capture(); 
	mMouse->capture();
	if(!mBreakMove) moveCamera();
	return true;
}

void AugmentedWindow::updateRobotTextBox()
{
	//Get the position and the orientation of the tool attached on the robot
	std::vector<double> joint_positions = mRTDEreceive->getActualTCPPose();
	//transform a vector to a displayable text
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

	//Display the resulting values
	mIinformationBox->setText(text);
	//printf("\n\npos robot_inside update = %s, %s, %s \t angles robot = %lf, %lf, %lf\n\n", std::to_string(joint_positions[0]), std::to_string(joint_positions[1]), std::to_string(joint_positions[2]), joint_positions[3], joint_positions[4], joint_positions[5]);
}

void AugmentedWindow::updateColaboratorTextBox()
{
	//Get the position of the camera
	Ogre::Vector3 joint_positions = mCamera->getRealPosition();
	Ogre::UTFString text = "pos user:\t";
	//transform a vector to a displayable text
	for (uint8_t i = 0; i < 2; i++)
	{
		text.append(std::to_string(joint_positions[i]));
		text.append(", ");
	}
	text.append(std::to_string(joint_positions[2]));
	text.append("\nangles:\t\t");
	
	//get the Direction of the camera
	joint_positions = mCamera->getRealDirection();
	//transform a vector to a displayable text
	for (uint8_t i = 0; i < 2; i++)
	{
		text.append(std::to_string(joint_positions[i]));
		text.append(", ");
	}
	text.append(std::to_string(joint_positions[2]));
	//Display the resulting values
	mColaboratorBox->setText(text);

}

void AugmentedWindow::setupBackground()
{
	// register our scene with the RTSS
	RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(mSceneMgr);

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
}

void AugmentedWindow::setupCamera()
{
	// Create the camera's top node (which will only handle position).
	mCameraNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	// Create the camera's yaw node as a child of camera's top node.
	mCameraYawNode = mCameraNode->createChildSceneNode();
	// Create the camera's pitch node as a child of camera's yaw node.
	mCameraPitchNode = mCameraYawNode->createChildSceneNode();
	// Create the camera's roll node as a child of camera's pitch node
	mCameraRollNode = mCameraPitchNode->createChildSceneNode();
	

	// create the camera
	mCamera = mSceneMgr->createCamera("myCam");
	mCamera->setNearClipDistance(5); // specific to this sample
	mCamera->setAutoAspectRatio(true);
	mCameraRollNode->attachObject(mCamera);
	mCameraNode->setPosition(0, 47, 222); // mooving the camera
	   
	// and tell it to render into the main window
	mRenderWindow->addViewport(mCamera);
	//! [camera]
	/*mViewport = mRenderWindow->addViewport(mCamera);
	mViewport->setBackgroundColour(ColourValue(0.8f, 0.7f, 0.6f, 1.0f));
	mViewport->setCamera(mCamera);
	*/
}

void AugmentedWindow::setupTextBoxes()
{
	TrayManager* mTrayMgr = new TrayManager("EnvironmentManager", mRenderWindow);
	std::vector<double> joint_positions = mRTDEreceive->getActualQ();
	mIinformationBox = mTrayMgr->createTextBox(TL_TOPLEFT, "InformationTextBox", "information from the robot", 400, 100);
	updateRobotTextBox();

	mColaboratorBox = mTrayMgr->createTextBox(TL_TOPLEFT, "ColaboratorTextBox", "information about the colaborator", 400, 100);
	updateColaboratorTextBox();
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
	mRenderWindow = getRenderWindow();

	size_t hWnd = 0;
	OIS::ParamList paramList;
	mRenderWindow = getRenderWindow();
	mRenderWindow->getCustomAttribute("WINDOW", &hWnd);

	paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

	mInputMgr = OIS::InputManager::createInputSystem(paramList);

	//Setting up the mouse and keyboard
	mKeyboard = static_cast<OIS::Keyboard*>(mInputMgr->createInputObject(OIS::OISKeyboard, true));
	mMouse = static_cast<OIS::Mouse*>(mInputMgr->createInputObject(OIS::OISMouse, true));

	mMouse->getMouseState().height = mRenderWindow->getHeight();
	mMouse->getMouseState().width = mRenderWindow->getWidth();

	mKeyboard->setEventCallback(this);
	mMouse->setEventCallback(this);

	//setting up the background of the window
	setupBackground();

	//seting up the camera  which look at that background so that we can see it on the window
	setupCamera();
	
	setupTextBoxes();
}

bool AugmentedWindow::keyPressed(const OIS::KeyEvent& keyEventRef)
{
	if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		mShutdown = true;	// ask to exit
		return true;//false ? todo
	}

	if (mKeyboard->isKeyDown(OIS::KC_M))
	{
		static int mode = 0;

		if (mode == 2)
		{
			mCamera->setPolygonMode(PM_SOLID);
			mode = 0;
		}
		else if (mode == 0)
		{
			mCamera->setPolygonMode(PM_WIREFRAME);
			mode = 1;
		}
		else if (mode == 1)
		{
			mCamera->setPolygonMode(PM_POINTS);
			mode = 2;
		}
	}
	
	// Move camera forward.
	if (mKeyboard->isKeyDown(OIS::KC_UP))
	{
		mTranslationVector.z = -mMoveScale;
		printf("UP pressed\n");
	}
	// Move camera backward.
	if (mKeyboard->isKeyDown(OIS::KC_DOWN))
		mTranslationVector.z = mMoveScale;

	// Move camera up.
	if (mKeyboard->isKeyDown(OIS::KC_PGUP))
		mTranslationVector.y = mMoveScale;
		
	// Move camera down.
	if (mKeyboard->isKeyDown(OIS::KC_PGDOWN))
		mTranslationVector.y = -mMoveScale;

	// Move camera left.
	if (mKeyboard->isKeyDown(OIS::KC_LEFT))
		mTranslationVector.x = -mMoveScale;

	// Move camera right.
	if (mKeyboard->isKeyDown(OIS::KC_RIGHT))
		mTranslationVector.x = mMoveScale;

	if (mKeyboard->isKeyDown(OIS::KC_SPACE))
		mBreakMove = !mBreakMove;


	/*
	mTranslationVector.x = (Real) mKeyboard->isKeyDown(OIS::KC_RIGHT) - (Real) mKeyboard->isKeyDown(OIS::KC_LEFT);
	mTranslationVector.y = (Real) mKeyboard->isKeyDown(OIS::KC_DOWN) - (Real) mKeyboard->isKeyDown(OIS::KC_UP)
	mTranslationVector *= mMoveScale;
	*/
	return true;
}

bool AugmentedWindow::keyReleased(const OIS::KeyEvent& keyEventRef)
{
	mTranslationVector = Vector3::ZERO;
	return true;
}

bool AugmentedWindow::mouseMoved(const OIS::MouseEvent& evt)
{
	mCameraYawNode->yaw(Degree(evt.state.X.rel * -0.1f));
	mCameraPitchNode->pitch(Degree(evt.state.Y.rel * -0.1f));

	return true;
}

bool AugmentedWindow::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	return true;
}

bool AugmentedWindow::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	return true;
}


/*
bool AugmentedWindow::keyPressed(const OIS::KeyEvent& keyEventRef) { return true; }
bool AugmentedWindow::keyReleased(const OIS::KeyEvent& keyEventRef) { return true; }
bool AugmentedWindow::mouseMoved(const OIS::MouseEvent& evt) { return true; }
bool AugmentedWindow::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id) { return true; }
bool AugmentedWindow::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id) { return true; }
*/

void AugmentedWindow::moveCamera()
{
	Vector3 mTranslation = mCameraYawNode->getOrientation() * mCameraPitchNode->getOrientation() * mTranslationVector;
	mCameraNode->translate(mTranslation, Ogre::SceneNode::TS_LOCAL); 

	// Angle of rotation around the X-axis.
	float pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(this->mCameraPitchNode->getOrientation().w)).valueDegrees());

	// Just to determine the sign of the angle we pick up above, the
	// value itself does not interest us.
	float pitchAngleSign = this->mCameraPitchNode->getOrientation().x;

	// Limit the pitch between -90 degress and +90 degrees, Quake3-style.
	if (pitchAngle > 90.0f)
	{
		if (pitchAngleSign > 0)
			// Set orientation to 90 degrees on X-axis.
			this->mCameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
				Ogre::Math::Sqrt(0.5f), 0, 0));
		else if (pitchAngleSign < 0)
			// Sets orientation to -90 degrees on X-axis.
			this->mCameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
				-Ogre::Math::Sqrt(0.5f), 0, 0));
	} 
}

void printVector(Vector3* vec, char* txt)
{
	printf("%s: %f, %f, %f\n", txt, vec->x, vec->y, vec->z);
}