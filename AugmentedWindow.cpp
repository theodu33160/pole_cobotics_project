#include "AugmentedWindow.h"

using namespace Ogre;
using namespace OgreBites;
using namespace ur_rtde;

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
	mMoveScale(10),
	mBreakMove(false)
{	
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
	mRobot->updatePosition();
	updateColaboratorTextBox();
	updateSafetyBox();
	mKeyboard->capture(); 
	mMouse->capture();
	if(!mBreakMove) moveCamera();
	return true;
}


void AugmentedWindow::updateColaboratorTextBox()
{
	//Get the position of the camera
	Ogre::Vector3 joint_positions = mCamera->getRealPosition();
	Ogre::UTFString text = "pos user:\t";
	//transform a vector to a displayable text
	for (uint8_t i = 0; i < 2; i++)
	{
		text.append(std::to_string((int)joint_positions[i]));
		text.append(", ");
	}
	text.append(std::to_string((int)joint_positions[2]));
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


void AugmentedWindow::updateRobotTextBox()
{
#if USE_SIMULATOR == true
	//Get the position and the orientation of the tool attached on the robot
	Vector3 toolPositions = mRobot->getToolPosition();
	Vector3 toolOrientation = mRobot->getToolOrientation();
	//transform a vector to a displayable text
	Ogre::UTFString text = "pos robot:\t";
	for (uint8_t i = 0; i < 3; i++)
	{
		text.append(std::to_string((int)toolPositions[i]));
		text.append(", ");
	}
	text.append("\nangles:\t\t");
	for (uint8_t i = 0; i < 2; i++)
	{
		text.append(std::to_string(toolOrientation[i]));
		text.append(", ");
	}
	text.append(std::to_string(toolOrientation[3]));
	//mCubeNode->setPosition(100*joint_positions[1], 100 * joint_positions[2], 100 * joint_positions[0]);
#else
	Ogre::UTFString text = "Universal robot simulator desactivated";
#endif //USE_SIMULATOR
	//Display the resulting values
	mIinformationBox->setText(text);
}

void AugmentedWindow::updateSafetyBox()
{
	Ogre::UTFString text = "Relative speed:";
	text.append(std::to_string(getRelativeDistanceCollaboratorRobot_v(mRobot).length()));
	text.append("\nTime before collision:");
	text.append(std::to_string(timeBeforeCollision(mRobot,100)));
	mSafetyBox->setText(text);
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
	mCameraNode->setPosition(0, 0, 1500); // mooving the camera
	   
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
	mIinformationBox = mTrayMgr->createTextBox(TL_BOTTOM, "InformationTextBox", "information from the robot", 400, 100);
	updateRobotTextBox();

	mColaboratorBox = mTrayMgr->createTextBox(TL_BOTTOM, "ColaboratorTextBox", "information about the colaborator", 400, 100);
	updateColaboratorTextBox();
	
	mSafetyBox = mTrayMgr->createTextBox(TL_TOPLEFT, "SafetyTextBox", "Potential collision with the colaborator", 400, 400);
	updateSafetyBox();
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
	printf("setup background\n");
	setupBackground();

	//Creation of an instance of U10.
	mRobot = new UR10(mSceneMgr, mSceneMgr->getRootSceneNode());

	//seting up the camera  which look at that background so that we can see it on the window
	printf("setup camera\n");
	setupCamera();
	
	printf("setup text boxes\n");
	setupTextBoxes();
}

bool AugmentedWindow::keyPressed(const OIS::KeyEvent& keyEventRef)
{
	if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		mShutdown = true;	// ask to exit
		return true;//false ? todo
	}
	
	// Move camera forward.
	if (mKeyboard->isKeyDown(OIS::KC_UP))
	{
		mTranslationVector.z = - Real(mMoveScale);
		printf("UP pressed\n");
	}
	// Move camera backward.
	if (mKeyboard->isKeyDown(OIS::KC_DOWN))
		mTranslationVector.z = Real(mMoveScale);

	// Move camera up.
	if (mKeyboard->isKeyDown(OIS::KC_PGUP))
		mTranslationVector.y = Real(mMoveScale);
		
	// Move camera down.
	if (mKeyboard->isKeyDown(OIS::KC_PGDOWN))
		mTranslationVector.y = - Real(mMoveScale);

	// Move camera left.
	if (mKeyboard->isKeyDown(OIS::KC_LEFT))
		mTranslationVector.x = - Real(mMoveScale);

	// Move camera right.
	if (mKeyboard->isKeyDown(OIS::KC_RIGHT))
		mTranslationVector.x = Real(mMoveScale);

	if (mKeyboard->isKeyDown(OIS::KC_SPACE))
		mBreakMove = !mBreakMove;

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

Ogre::Vector3 AugmentedWindow::getRelativeSpeedCollaboratorRobot_v(UR10* robot)
{//Relative speed of the collaborator with regards to the Robot
	//we don't have the speed of the collaborator yet
	return robot->getToolSpeed()-0; //0 will be the collaborator speed
}

Ogre::Vector3 AugmentedWindow::getRelativeDistanceCollaboratorRobot_v(UR10* robot)
{
	return robot->getToolPosition() - mCamera->getPosition();
}

double AugmentedWindow::timeBeforeCollision(UR10* robot, float radius)
{//Return -1 if the relative speed is nule or negative
	/* calculus if there will be a collision:
	 * Distance from a point (P) to a line:
	 * A a point of the line, and u a director vector of this line.
	 * The distance from A to P is || AP x u || / ||u||
	 * with words: the norme of the cross product of the vector AP and u divided by the norme of u
	 * wikipedia link: https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Another_vector_formulation
	 *
	 * In our case, u is the speed of the tool of the robot and P is the collaborator 
	 * What point of the collaborator to choose? the middle? the closest?
	 */
	if (getRelativeSpeedCollaboratorRobot_v(robot).length() <= 0)
		return -1; 

	Vector3 relativeDistance_v = getRelativeDistanceCollaboratorRobot_v(robot);
	Vector3 relativeSpeed_v = getRelativeSpeedCollaboratorRobot_v(robot);
	double gabRobotTrajectoryToCollabotor = relativeDistance_v.crossProduct(relativeSpeed_v).length() / relativeSpeed_v.length();
	if (gabRobotTrajectoryToCollabotor > radius) //no Collision
		return -1;

	/* To calculate the time the robot will take to enter in collision with the colabortor, 
	 * We considere that 
	 */
	double collisionDistance = relativeSpeed_v.absDotProduct(relativeDistance_v) / relativeSpeed_v.length() - sqrt( pow(radius,2)- pow(gabRobotTrajectoryToCollabotor,2));
	
	return collisionDistance / relativeSpeed_v.length();
}