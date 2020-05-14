#include "AugmentedWindow.h"

using namespace Ogre;
using namespace OgreBites;
using namespace ur_rtde;

Ogre::SceneNode* mProjectorNode;
Ogre::Frustum* mDecalFrustum;
Ogre::Frustum* mFilterFrustum;

const Matrix3 arrowToCamera = Matrix3(1,0,0, 0,1,0, 0,0,-1);
const Matrix3 arrowToBone   = Matrix3(0,0,1, 0,1,0, 1,0,0);

AugmentedWindow::AugmentedWindow()
	: ApplicationContext("OgreTutorialApp"),
	mRoot(0),
	mCamera(0),
	mViewport(0),
	mSceneMgr(0),
	mRenderWindow(0),
	mRobotBox(0),
	mColaboratorBox(0),
	mInputMgr(0),
	mKeyboard(0),
	mMouse(0),
	mShutdown(false),
	mBreakMove(false),
	mCurrentBone(collabBonesEnum_t(0))
{	
	mArrowVector = Vector3::ZERO;
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
	mRobot->updatePosition();

#if ENABLE_UR10_TEXT_BOX == true
	updateRobotTextBox();
#endif
#if ENABLE_COLLABORATOR_TEXT_BOX == true
	updateColaboratorTextBox();
#endif
	updateSafetyBox();
	updateInfoBox();
	mKeyboard->capture(); 
	mMouse->capture();
	updateCircleLight();
	if(!mBreakMove) moveCamera();
	moveCollabBone();
	return true;
}


void AugmentedWindow::updateColaboratorTextBox()
{
#if ENABLE_COLLABORATOR_TEXT_BOX == true
	//Get the position of the camera
	Ogre::Vector3 joint_positions = mCamera->getRealPosition();
	Ogre::UTFString text = "pos user:\t";
	//transform a vector to a printable text
	for (uint8_t i = 0; i < 2; i++)
	{
		text.append(std::to_string((int)joint_positions[i]));
		text.append(", ");
	}
	text.append(std::to_string((int)joint_positions[2]));
	text.append("\nangles:\t\t");
	
	//get the Direction of the camera
	joint_positions = mCamera->getRealDirection();
	//transform a vector to a printable text
	for (uint8_t i = 0; i < 2; i++)
	{
		text.append(std::to_string(joint_positions[i]));
		text.append(", ");
	}
	text.append(std::to_string(joint_positions[2]));
	//Display the resulting values
	mColaboratorBox->setText(text);
#endif
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
	mRobotBox->setText(text);
}

void AugmentedWindow::updateSafetyBox()
{
	Ogre::UTFString text = "Relative distance:";
	text.append(std::to_string(getRelativeDistanceCollaboratorRobot_v(mRobot).length()));
#if USE_SIMULATOR == true
	text.append("\nTime before collision:");
	text.append(std::to_string(timeBeforeCollision(mRobot,100)));
#endif
	mSafetyBox->setText(text);
}
void AugmentedWindow::updateInfoBox()
{
	Ogre::UTFString text = "Selected bone: ";
	text.append(collabSkeleton->getBone(mCurrentBone)->getName());
	//text.append(boneNames[mCurrentBone]); //todo: remove line
	text.append("\nC, R and L move Collab, Right and Left hand");
	mInfoBox->setText(text);
}

void AugmentedWindow::updateCircleLight()
{
	collabLightNode->setPosition(collabEntity->getParentNode()->getPosition());
	collabLightNode->translate(0, HEIGHT_LIGHT, 0);
	Real dist = getRelativeDistanceCollaboratorRobot_v(mRobot).length();
	Real colorValue = CIRCLE_LIGHT_BRIGHTNESS *( 1. + tanh((dist-DIST_COLOR_CHANGE)/ DIST_SPREAD_COLOR))/2.;
	collabLight->setDiffuseColour(CIRCLE_LIGHT_BRIGHTNESS - colorValue, colorValue, 0);
}

void AugmentedWindow::setupBackground()
{
	// register our scene with the RTSS
	RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(mSceneMgr);

	mSceneMgr->setAmbientLight(ColourValue(AMBIENT_BRIGHTNESS, AMBIENT_BRIGHTNESS, AMBIENT_BRIGHTNESS));
	Light* light = mSceneMgr->createLight("MainLight");
	SceneNode* lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	lightNode->attachObject(light);
	lightNode->setPosition(20, 80, 50); //todo change the position. Perhaps have several of them.
	
	//-------set up the collaborator----------------------------
	collabEntity = mSceneMgr->createEntity("collaborator","low_poly_chara_170cm_centered.mesh"); //low_poly_chara_170cm_centered
	SceneNode* collabNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(-150, 3, -50));
	collabNode->rotate(Vector3::UNIT_X, Radian(Degree(0)));
	collabNode->setPosition(500,0, 0);
	collabNode->attachObject(collabEntity);
	collabSkeleton = collabEntity->getSkeleton();
	for (int i = 0; i < collabSkeleton->getNumBones(); i++)
	{
		collabSkeleton->getBone(i)->setManuallyControlled(true);
	}
	//highlight(collabEntity);

	


	//-------set up a floor------------------------------
	// create a floor mesh resource
	MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Plane(Vector3::UNIT_Y, 0), 10000, 10000, 100, 100, true, 1, 100, 100, Vector3::UNIT_Z);

	// create a floor entity, give it a material, and place it at the origin
	Entity* floor = mSceneMgr->createEntity("Floor", "floor");
	floor->setMaterialName("Examples/Rockwall"); //MRAMOR6X6.jpg  Examples/Rockwall
	floor->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->attachObject(floor);



	//------------set a circle around the collaborator----------------------------------
	collabLight = mSceneMgr->createLight("collabLight");
	collabLight->setType(Light::LT_SPOTLIGHT);
	collabLight->setSpotlightRange(Degree(RADIUS_LIGHT-15), Degree(RADIUS_LIGHT)); //max brightness angle, dimming angle
	//SceneNode* collabLightNode = collabNode->createChildSceneNode();
	collabLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	collabLightNode->attachObject(collabLight);
	collabLight->setDirection(- Vector3::UNIT_Y);
	updateCircleLight();
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
	mCameraNode->setPosition(-150, 700, 3500); // mooving the camera
	
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

#if ENABLE_UR10_TEXT_BOX == true
	mRobotBox = mTrayMgr->createTextBox(TL_BOTTOM, "RobotTextBox", "information from the robot", 400, 100);
	updateRobotTextBox();
#endif

#if ENABLE_COLLABORATOR_TEXT_BOX == true
	mColaboratorBox = mTrayMgr->createTextBox(TL_BOTTOM, "ColaboratorTextBox", "information about the observer", 400, 100);
	updateColaboratorTextBox();
#endif

	mSafetyBox = mTrayMgr->createTextBox(TL_TOPLEFT, "SafetyTextBox", "Potential collision with the colaborator", 400, 100);
	updateSafetyBox();

	mInfoBox = mTrayMgr->createTextBox(TL_TOPLEFT, "InfoTextBox", "Diverse information", 400, 100);
	updateInfoBox();
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

	//Creation of an instance of U10.
	mRobot = new UR10(mSceneMgr, mSceneMgr->getRootSceneNode());

	//setting up the background of the window
	printf("setup background\n");
	setupBackground();

	//seting up the camera  which look at that background so that we can see it on the window
	printf("setup camera\n");
	setupCamera();
	
	printf("setup text boxes\n");
	setupTextBoxes();

}

/* Per default, using the arrows move the camera
 * However, if another key is pressed in the meanwhile, the camera is blocked
 * and bone relative to the key that have been pressed move.
*/
bool AugmentedWindow::keyPressed(const OIS::KeyEvent& keyEventRef)
{
	if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		mShutdown = true;	// ask to exit
		return true;
	}
	
	if (mKeyboard->isKeyDown(OIS::KC_SPACE))
		mBreakMove = !mBreakMove;


	//--------ARROWS---------------------------------------------------
	if (mKeyboard->isKeyDown(OIS::KC_UP))
		mArrowVector.z = 1;

	if (mKeyboard->isKeyDown(OIS::KC_DOWN))
		mArrowVector.z = -1;

	if (mKeyboard->isKeyDown(OIS::KC_PGUP)   || mKeyboard->isKeyDown(OIS::KC_END))
		mArrowVector.y = 1;		
	
	if (mKeyboard->isKeyDown(OIS::KC_PGDOWN) || mKeyboard->isKeyDown(OIS::KC_HOME))
		mArrowVector.y = -1;
	
	if (mKeyboard->isKeyDown(OIS::KC_LEFT))	
		mArrowVector.x = -1;
	
	if (mKeyboard->isKeyDown(OIS::KC_RIGHT))
		mArrowVector.x = 1;

	//--------COLLABORATOR---------------------------------------------------
	if (mKeyboard->isKeyDown(OIS::KC_C))
	{
		mBreakMove = true;
		collabEntity->getParentNode()->translate(mArrowVector * arrowToCamera * TRANSLATE_SCALE);
	}
		
	//Collaborator bones
	if (mKeyboard->isKeyDown(OIS::KC_R))
	{
		mBreakMove = true;
		boneToMove = handR;
	}

	if (mKeyboard->isKeyDown(OIS::KC_L))
	{
		mBreakMove = true;
		boneToMove = handL;
	}

	if (mKeyboard->isKeyDown(OIS::KC_S))
	{
		mBreakMove = true;
		boneToMove = spine;
	}

	if (mKeyboard->isKeyDown(OIS::KC_B)) //individual bones
	{
		mBreakMove = true;
		boneToMove = mCurrentBone;
	}

	//Testing the bones
	if (mKeyboard->isKeyDown(OIS::KC_TAB))
	{
		if (mKeyboard->isKeyDown(OIS::KC_LSHIFT))
			mCurrentBone = static_cast<collabBonesEnum_t>(((int)mCurrentBone - 1 + NB_COLLAB_BONES) % NB_COLLAB_BONES);
		else mCurrentBone = static_cast<collabBonesEnum_t>(((int)mCurrentBone + 1) % NB_COLLAB_BONES);
	}

	/*

	//Movement of the whole Collaborator BONES 
	if (mKeyboard->isKeyDown(OIS::KC_Y))
		collabSkeleton->getBone(mCurrentBone)->rotate(Vector3::UNIT_Y, Radian(ROTATE_SCALE));

	if (mKeyboard->isKeyDown(OIS::KC_R))
		collabSkeleton->getBone(mCurrentBone)->rotate(Vector3::NEGATIVE_UNIT_Y, Radian(ROTATE_SCALE));
	
	if (mKeyboard->isKeyDown(OIS::KC_G))
		collabSkeleton->getBone(mCurrentBone)->rotate(Vector3::NEGATIVE_UNIT_X, Radian(ROTATE_SCALE));
	
	//	if (mKeyboard->isKeyDown(OIS::KC_H))
	//		collabSkeleton->getBone(mCurrentBone)->rotate(Vector3::UNIT_Z, Radian(ROTATE_SCALE));
	
	if (mKeyboard->isKeyDown(OIS::KC_T))
		collabSkeleton->getBone(mCurrentBone)->rotate(Vector3::UNIT_X, Radian(ROTATE_SCALE));
	
	if (mKeyboard->isKeyDown(OIS::KC_F))
		collabSkeleton->getBone(mCurrentBone)->rotate(Vector3::NEGATIVE_UNIT_Z, Radian(ROTATE_SCALE));
	*/
	return true;
}

bool AugmentedWindow::keyReleased(const OIS::KeyEvent& keyEventRef)
{
	mArrowVector = Vector3::ZERO;
	mBreakMove = false;
	boneToMove = NoBone;
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
	Vector3 mTranslation = mCameraYawNode->getOrientation() * mCameraPitchNode->getOrientation() * (arrowToCamera * mArrowVector);
	mTranslation = mTranslation * TRANSLATE_SCALE;
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

void AugmentedWindow::moveCollabBone()
{
	if (boneToMove != NoBone)
	{
		collabSkeleton->getBone(boneToMove)->translate(mArrowVector * TRANSLATE_SCALE);
	}
}

Ogre::Vector3 AugmentedWindow::getRelativeSpeedCollaboratorRobot_v(UR10* robot)
{//Relative speed of the collaborator with regards to the Robot
	//we don't have the speed of the collaborator yet
	return robot->getToolSpeed()-0; //0 will be the collaborator speed
}

Ogre::Vector3 AugmentedWindow::getRelativeDistanceCollaboratorRobot_v(UR10* robot)
{
	Vector3 robot_CollabNode_Position = robot->getToolPosition() - collabEntity->getParentNode()->getPosition();
	Vector3 minDist_v = robot_CollabNode_Position + collabSkeleton->getBone(0)->getPosition();
	Real lengthMinDist = minDist_v.squaredLength();
	for (int bone = 1; bone < NB_COLLAB_BONES; bone++)
	{
		if (lengthMinDist > (robot_CollabNode_Position +					//+ because relative coordonates. 
			collabSkeleton->getBone(bone)->getPosition()).squaredLength())	//squaredLength is less expensive for the CPU
		{
			minDist_v = robot_CollabNode_Position + collabSkeleton->getBone(bone)->getPosition();
			lengthMinDist = minDist_v.squaredLength();
		}
	}
	return minDist_v;
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


/*
	add the rim lighting effect to an entity.
*/
void AugmentedWindow::highlight(Ogre::Entity* entity)
{
	unsigned short count = entity->getNumSubEntities();

	const Ogre::String file_name = "rim.dds";
	const Ogre::String rim_material_name = "_rim";

	for (unsigned short i = 0; i < count; ++i)
	{
		Ogre::SubEntity* subentity = entity->getSubEntity(i);

		const Ogre::String& old_material_name = subentity->getMaterialName();
		Ogre::String new_material_name = old_material_name + rim_material_name;

		Ogre::MaterialPtr new_material = MaterialManager::getSingleton().getByName(new_material_name);

		if (new_material.isNull())
		{
			MaterialPtr old_material = MaterialManager::getSingleton().getByName(old_material_name);
			new_material = old_material->clone(new_material_name);

			Pass* pass = new_material->getTechnique(0)->getPass(0);
			Ogre::TextureUnitState* texture = pass->createTextureUnitState();
			texture->setCubicTextureName(&file_name, true);
			texture->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
			texture->setColourOperationEx(Ogre::LBX_ADD, Ogre::LBS_TEXTURE, Ogre::LBS_CURRENT);
			texture->setColourOpMultipassFallback(Ogre::SBF_ONE, Ogre::SBF_ONE);
			texture->setEnvironmentMap(true, Ogre::TextureUnitState::ENV_NORMAL);
		}

		subentity->setMaterial(new_material);
	}
}

/*
	remove the rim lighting effect from an entity.
*/
void AugmentedWindow::unhighlight(Ogre::Entity* entity)
{
	unsigned short count = entity->getNumSubEntities();

	for (unsigned short i = 0; i < count; ++i)
	{
		Ogre::SubEntity* subentity = entity->getSubEntity(i);
		Ogre::SubMesh* submesh = subentity->getSubMesh();

		const Ogre::String& old_material_name = submesh->getMaterialName();
		const Ogre::String& new_material_name = subentity->getMaterialName();

		// if the entity is already using the original material then we're done. 
		if (0 == stricmp(old_material_name.c_str(), new_material_name.c_str()))
			continue;

		// otherwise restore the original material name.
		subentity->setMaterialName(old_material_name);

	}
}