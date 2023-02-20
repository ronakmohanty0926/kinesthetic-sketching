#include "HapticsEventManager.h"


using namespace midl;

  /******************************************************************************
  Factory method for creating an instance of the HapticsEventManager.
  ******************************************************************************/
HapticsEventManagerGeneric *HapticsEventManagerGeneric::Initialize()
{
	return new HapticsEventManager;
}

/******************************************************************************
Factory method for destroying an instance of the HapticsEventManager.
******************************************************************************/
void HapticsEventManagerGeneric::Delete(HapticsEventManagerGeneric *&pInterface)
{
	if (pInterface)
	{
		HapticsEventManager *pImp = static_cast<HapticsEventManager *>(pInterface);
		delete pImp;
		pInterface = 0;
	}
}

/******************************************************************************
HapticsEventManager Constructor.
******************************************************************************/
HapticsEventManager::HapticsEventManager() :
	m_hHD(HD_INVALID_HANDLE),
	m_hUpdateCallback(HD_INVALID_HANDLE),
	m_pHapticDeviceHT(0),
	m_pHapticDeviceGT(0),
	abcSketchManager(0),
	m_nCursorDisplayList(0)
{}

/*******************************************************************************
HapticsEventManager Destructor.
*******************************************************************************/
HapticsEventManager::~HapticsEventManager(){}

/*******************************************************************************
This is the main initialization needed for the haptic glue code.
*******************************************************************************/
void HapticsEventManager::Setup(ABCSketchManager *skManager)
{
	HDErrorInfo error;

	/* Intialize a device configuration. */
	m_hHD = hdInitDevice(HD_DEFAULT_DEVICE);
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		std::cerr << error << std::endl;
		std::cerr << "Failed to initialize haptic device" << std::endl;
		std::cerr << "Press any key to quit." << std::endl;
		getchar();
		exit(-1);
	}

	/* Create the IHapticDevice instances for the haptic and graphic threads
	These interfaces are useful for handling the synchronization of
	state between the two main threads. */
	m_pHapticDeviceHT = IHapticDevice::create(
		IHapticDevice::HAPTIC_THREAD_INTERFACE, m_hHD);
	m_pHapticDeviceGT = IHapticDevice::create(
		IHapticDevice::GRAPHIC_THREAD_INTERFACE, m_hHD);

	/* Setup callbacks so we can be notified about events in the graphics
	thread. */
	m_pHapticDeviceGT->setCallback(
		IHapticDevice::MADE_CONTACT, madeContactCallbackGT, this);
	m_pHapticDeviceGT->setCallback(
		IHapticDevice::LOST_CONTACT, lostContactCallbackGT, this);
	m_pHapticDeviceGT->setCallback(
		IHapticDevice::BUTTON_1_UP, button1UpClickCallbackGT, this);
	m_pHapticDeviceGT->setCallback(
		IHapticDevice::BUTTON_1_DOWN, button1DownClickCallbackGT, this);
	m_pHapticDeviceGT->setCallback(
		IHapticDevice::BUTTON_2_UP, button2UpClickCallbackGT, this);
	m_pHapticDeviceGT->setCallback(
		IHapticDevice::BUTTON_2_DOWN, button2DownClickCallbackGT, this);
	m_pHapticDeviceGT->setCallback(
		IHapticDevice::DEVICE_ERROR, errorCallbackGT, this);

	hdEnable(HD_FORCE_OUTPUT);

	/* Schedule a haptic thread callback for updating the device every
	tick of the servo loop. */
	m_hUpdateCallback = hdScheduleAsynchronous(deviceUpdateCallback, this, HD_MAX_SCHEDULER_PRIORITY);

	/* Start the scheduler to get the haptic loop going. */
	hdStartScheduler();
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		std::cerr << error << std::endl;
		std::cerr << "Failed to start scheduler" << std::endl;
		std::cerr << "Press any key to quit." << std::endl;
		getchar();
		exit(-1);
	}

	// GRAPHICS SETUP
	abcSketchManager =  skManager;
	colorShader.Initialize(".//Shaders//colorShader.vert", ".//Shaders//colorShader.frag");	
	isForceOn = false;
}

/*******************************************************************************
Reverse the setup process by shutting down and destructing the services
used by the HapticsEventManager.
*******************************************************************************/
void HapticsEventManager::Cleanup()
{
	hdStopScheduler();

	if (m_hUpdateCallback != HD_INVALID_HANDLE)
	{
		hdUnschedule(m_hUpdateCallback);
		m_hUpdateCallback = HD_INVALID_HANDLE;
	}

	if (m_hHD != HD_INVALID_HANDLE)
	{
		hdDisableDevice(m_hHD);
		m_hHD = HD_INVALID_HANDLE;
	}

	IHapticDevice::destroy(m_pHapticDeviceGT);
	IHapticDevice::destroy(m_pHapticDeviceHT);

	glDeleteLists(m_nCursorDisplayList, 1);
}

/*******************************************************************************
This method will get called every tick of the graphics loop. It is primarily
responsible for synchronizing state with the haptics thread as well as
updating snap state.
*******************************************************************************/
void HapticsEventManager::UpdateState()
{
	/* Capture the latest state from the servoloop. */
	m_pHapticDeviceGT->beginUpdate(m_pHapticDeviceHT);
	m_pHapticDeviceGT->endUpdate(m_pHapticDeviceHT);
}

/*******************************************************************************
Uses the current OpenGL viewing transforms to determine a mapping from device
coordinates to world coordinates.
*******************************************************************************/
void HapticsEventManager::UpdateWorkspace()
{
	GLdouble modelview[16];
	GLdouble projection[16];
	GLint viewport[4];
		
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	/* Compute the transform for going from device coordinates to world
	coordinates based on the current viewing transforms. */
	hduMapWorkspaceModel(modelview, projection, m_workspaceXform);

	/* Compute the scale factor that can be applied to a unit sized object
	in world coordinates that will make it a particular size in pixels. */
	HDdouble screenTworkspace = hduScreenToWorkspaceScale(
		modelview, projection, viewport, m_workspaceXform);
	
	m_cursorScale = CURSOR_SIZE_PIXELS * screenTworkspace;

	hlMatrixMode(HL_TOUCHWORKSPACE);
	hluFitWorkspace(projection);
		
	/* Compute the updated camera position in world coordinates. */
	hduMatrix worldTeye(modelview);
	hduMatrix eyeTworld = worldTeye.getInverse();
	eyeTworld.multVecMatrix(hduVector3Dd(0, 0, 0), m_cameraPosWC);

	hdScheduleSynchronous(setDeviceTransformCallback, this, SYNCHRONIZE_STATE_PRIORITY);
}

/*******************************************************************************
Draws a 3D cursor using the current device transform and the workspace
to world transform.
*******************************************************************************/
void HapticsEventManager::RenderStylus()
{
	IHapticDevice::IHapticDeviceState *pState =
	m_pHapticDeviceGT->getCurrentState();
		
	hduVector3Dd sty = pState->getPosition();

	//sty.normalize();
	
	float curr_stylus[] = {(float)sty[0]*0.005,(float)sty[1]*0.005,(float)sty[2]*0.005};	

	//cout << "valueX:" << sty[0] << endl;
	//cout << "valueY:" << sty[1] << endl;
	//cout << "valueZ:" << sty[2] << endl;

	glDisable(GL_BLEND);
	colorShader.Bind();
	//light.Bind();	
	
	//float toolDir[] = { new_curr_stylus[0],new_curr_stylus[1] + 0.01,new_curr_stylus[2] + 0.5 };
		
	//if (isDrillOn1)
	//{
	//	//DrawCylinder(new_curr_stylus, toolDir, 0.1, Green3f, 1.0, true, 50);
	//	DrawSphere(curr_stylus, 0.03, Green3f, 1.0);
	//}
	//else
	//{
	//	//DrawCylinder(new_curr_stylus, toolDir, 0.1, Red3f, 1.0, true, 50);
	//	DrawSphere(curr_stylus, 0.03, Red3f, 1.0);
	//}

	//glPushMatrix();
	//glMultMatrixd(pState->getParentCumulativeTransform());
	//glMultMatrixd(pState->getTransform());
	////glScaled(m_cursorScale, m_cursorScale, m_cursorScale);

	//if (isDrillOn)
	//{
	//	DrawCylinder(Origin, Zaxis, 0.1, Green3f, 1.0, true, 50);
	//	DrawSphere(Origin, 0.03, Green3f, 1.0);
	//}
	//else
	//{
	//	DrawCylinder(Origin, Zaxis, 0.1, Red3f, 1.0, true, 50);
	//	DrawSphere(Origin, 0.03, Red3f, 1.0);
	//}

	//glPopMatrix();

	//light.Unbind();
	colorShader.Unbind();

	/*glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
	glPushMatrix();

	if (!m_nCursorDisplayList)
	{
		m_nCursorDisplayList = glGenLists(1);
		glNewList(m_nCursorDisplayList, GL_COMPILE);
		GLUquadricObj *qobj = gluNewQuadric();
		gluSphere(qobj, 0.5, 10, 10);
		gluDeleteQuadric(qobj);
		glEndList();
	}

	glMultMatrixd(pState->getParentCumulativeTransform());

	glMultMatrixd(pState->getTransform());

	glScaled(m_cursorScale, m_cursorScale, m_cursorScale);

	glEnable(GL_COLOR_MATERIAL);glColor3f(1.0, 1.0, 1.0);

	glCallList(m_nCursorDisplayList);

	glPopMatrix();
	glPopAttrib();*/
}


/******************************************************************************
Scheduler Callbacks
These callback routines get performed in the haptics thread.
******************************************************************************/

/******************************************************************************
This is the main haptic thread scheduler callback. It handles updating the
currently applied constraint.
******************************************************************************/
HDCallbackCode HDCALLBACK HapticsEventManager::deviceUpdateCallback(
	void *pUserData)
{	
	HapticsEventManager *pThis = static_cast<HapticsEventManager *>(pUserData);

	/* Force the haptic device to update its state. */
	pThis->m_pHapticDeviceHT->beginUpdate(0);

	IHapticDevice::IHapticDeviceState *pCurrentState =
		pThis->m_pHapticDeviceHT->getCurrentState();
	IHapticDevice::IHapticDeviceState *pLastState =
		pThis->m_pHapticDeviceHT->getLastState();

	/* Get the position of the device. */
	hduVector3Dd devicePositionLC = pCurrentState->getPosition();

	float realstylus[] = { (float)devicePositionLC[0] , (float)devicePositionLC[1] , (float)devicePositionLC[2]  };
	
	float forceVector[] = {0.0,0.0,0.0};

	float xRng[] = { -193.21,213.18 }; float yRng[] = { -107.35, 190.732 }; float zRng[] = { -50.0, 94.42 };
	float xLen = xRng[1] - xRng[0]; float yLen = yRng[1] - yRng[0]; float zLen = zRng[1] - zRng[0];
	float stylus[3], displacement[3];

	float newRng[] = { -0.3, 0.3 };
	float Rng = newRng[1] - newRng[0];	

	stylus[0] = Rng*(((float)devicePositionLC[0] - xRng[0]) / xLen) + newRng[0];
	stylus[1] = Rng*(((float)devicePositionLC[1] - yRng[0]) / yLen) + newRng[0];
	stylus[2] = Rng*(((float)devicePositionLC[2] - zRng[0]) / zLen) + newRng[0];

	stylus[0] *= 2.0; stylus[1] *= 2.0; stylus[2] *= 2.0;

	/* Update the voxel model based on the new position data. */
	ABCSketchManager *skAPI = pThis->abcSketchManager;
	Mesh *meshAPI = pThis->meshManager;
	
	skAPI->HapListen(realstylus);

	int startSketch = skAPI->Listen(stylus);
	bool isHapticsActive = skAPI->CheckHapticsForce();
	float prev_stylus[3]; skAPI->SetPivot(prev_stylus);

	displacement[0] = prev_stylus[0] - stylus[0];
	displacement[1] = prev_stylus[1] - stylus[1];
	displacement[2] = prev_stylus[2] - stylus[2];

	//float rotaxis[3];
	//rotaxis[0] = displacement[1];
	//rotaxis[1] = -displacement[0];
	//rotaxis[2] = 0.0;

	//Normalize3(displacement);
	//float rotangle = -0.008*Norm3(displacement);
	//float vertex[3];	

	float planePt[3]; skAPI->SetPlaneCenter(planePt);
	float planeNorm[3] = {0.0,0.0,1.0};
	float planeDist = PointToPlaneDist(stylus, planePt, planeNorm);
	//cout << "planeDistance->" << planeDist << endl;
	hduVector3Dd force;
	if (planeDist < 0.15 && planeDist > -0.08 && startSketch == 0 && stylus[1] > -0.32 && stylus[1] < 0.29 && stylus[0] > -0.5 && stylus[0] < 0.5)
	{
		float stiffness = 5.1;
		//float stiffness = 3.0;
		forceVector[0] = -1*stiffness*0.8*planeDist; forceVector[1] = -1*stiffness*0.8*planeDist; forceVector[2] = -1 * stiffness*0.8*planeDist;
		force.set((HDdouble)forceVector[0], (HDdouble)forceVector[1], (HDdouble)forceVector[2]);
	}
	else if (startSketch == 2 && isHapticsActive )
	{
		//meshAPI->GetAxisAngle(axis, -0.008*Norm3(displacement));
		forceVector[0] = 0.0; forceVector[1] = 0.0; forceVector[2] = 0.0;
		force.set((HDdouble)forceVector[0], (HDdouble)forceVector[1], (HDdouble)forceVector[2]);
	}
	else force.set((HDdouble)0.0, (HDdouble)0.0, (HDdouble)0.0);

	/*if (startSketch)
	force.set((HDdouble)forceVector[0], (HDdouble)forceVector[1], (HDdouble)forceVector[2]);
	else force.set((HDdouble)0.0, (HDdouble)0.0, (HDdouble)0.0);*/

	/*hduVector3Dd force;
	if (startSketch)
		force.set((HDdouble)forceVector[0], (HDdouble)forceVector[1], (HDdouble)forceVector[2]);
	else force.set((HDdouble)0.0, (HDdouble)0.0, (HDdouble)0.0);*/
	hdSetDoublev(HD_CURRENT_FORCE, force);

	//if (pCurrentState->isInContact())
	//{
	//	/* If currently in contact, use the freshest contact data. */
	//	pCurrentState->setContactData(pSnapAPI->getConstraint()->getUserData());
	//}
	//else if (pLastState->isInContact())
	//{
	//	/* If was in contact the last frame, use that contact data, since it
	//	will get reported to the event callbacks. */
	//	pCurrentState->setContactData(pLastState->getContactData());
	//}
	//else
	//{
	//	pCurrentState->setContactData((void *)-1);
	//}

	///* Transform result from world coordinates back to device coordinates. */
	//hduVector3Dd proxyPositionLC = pSnapAPI->getConstrainedProxy();
	//pCurrentState->setProxyPosition(proxyPositionLC);

	//hdGetDoublev(HD_CURRENT_TRANSFORM, pCurrentState->getProxyTransform());
	//pCurrentState->getProxyTransform()[12] = proxyPositionLC[0];
	//pCurrentState->getProxyTransform()[13] = proxyPositionLC[1];
	//pCurrentState->getProxyTransform()[14] = proxyPositionLC[2];

	//double kStiffness;
	//hdGetDoublev(HD_NOMINAL_MAX_STIFFNESS, &kStiffness);
	//kStiffness = hduMin(0.4, kStiffness);

	///* Compute spring force to attract device to constrained proxy. */
	//hduVector3Dd force = kStiffness * (proxyPositionLC - devicePositionLC);
	//hdSetDoublev(HD_CURRENT_FORCE, force);

	pThis->m_pHapticDeviceHT->endUpdate(0);

	return HD_CALLBACK_CONTINUE;
}

/******************************************************************************
Scheduler callback to set the workspace transform both for use in the graphics
thread and haptics thread.
******************************************************************************/
HDCallbackCode HDCALLBACK HapticsEventManager::setDeviceTransformCallback(
	void *pUserData)
{
	HapticsEventManager *pThis = static_cast<HapticsEventManager *>(pUserData);

	IHapticDevice::IHapticDeviceState *pStateGT =
		pThis->m_pHapticDeviceGT->getCurrentState();
	IHapticDevice::IHapticDeviceState *pStateHT =
		pThis->m_pHapticDeviceHT->getCurrentState();

	pStateGT->setParentCumulativeTransform(pThis->m_workspaceXform);
	pStateHT->setParentCumulativeTransform(pThis->m_workspaceXform);

	return HD_CALLBACK_DONE;
}

/******************************************************************************
Event Callbacks

These are event callbacks that are registered with the IHapticDevice
******************************************************************************/

/******************************************************************************
This handler gets called in the graphics thread whenever the device makes
contact with a constraint. Provide a visual cue (i.e. highlighting) to
accompany the haptic cue of being snapped to the point.
******************************************************************************/
void HapticsEventManager::madeContactCallbackGT(
	IHapticDevice::EventType event,
	const IHapticDevice::IHapticDeviceState * const pState,
	void *pUserData)
{
	HapticsEventManager *pThis = static_cast<HapticsEventManager *>(pUserData);

}

/******************************************************************************
This handler gets called in the graphics thread whenever the device loses
contact with a constraint. Provide a visual cue (i.e. highlighting) to
accompany the haptic cue of losing contact with the point.
******************************************************************************/
void HapticsEventManager::lostContactCallbackGT(
	IHapticDevice::EventType event,
	const IHapticDevice::IHapticDeviceState * const pState,
	void *pUserData)
{
	HapticsEventManager *pThis = static_cast<HapticsEventManager *>(pUserData);

}

/******************************************************************************
This handler gets called in the graphics thread whenever a button press is
detected. Interpret the click as a drilling on/off.
******************************************************************************/
void HapticsEventManager::button1UpClickCallbackGT(
	IHapticDevice::EventType event,
	const IHapticDevice::IHapticDeviceState * const pState,
	void *pUserData)
{
	HapticsEventManager *pThis = static_cast<HapticsEventManager *>(pUserData);

	ABCSketchManager *skAPI = pThis->abcSketchManager;
	skAPI->ManipulationOff();	
}

void HapticsEventManager::button1DownClickCallbackGT(
	IHapticDevice::EventType event,
	const IHapticDevice::IHapticDeviceState * const pState,
	void *pUserData)
{
	HapticsEventManager *pThis = static_cast<HapticsEventManager *>(pUserData);

	ABCSketchManager *skAPI = pThis->abcSketchManager;
	skAPI->RotationOn();	
}

void HapticsEventManager::button2UpClickCallbackGT(
	IHapticDevice::EventType event,
	const IHapticDevice::IHapticDeviceState * const pState,
	void *pUserData)
{
	HapticsEventManager *pThis = static_cast<HapticsEventManager *>(pUserData);

	ABCSketchManager *skAPI = pThis->abcSketchManager;
	skAPI->ManipulationOff();
}

void HapticsEventManager::button2DownClickCallbackGT(
	IHapticDevice::EventType event,
	const IHapticDevice::IHapticDeviceState * const pState,
	void *pUserData)
{
	HapticsEventManager *pThis = static_cast<HapticsEventManager *>(pUserData);

	ABCSketchManager *skAPI = pThis->abcSketchManager;
	skAPI->TranslationOn();
}


/******************************************************************************
This handler gets called to handle errors
******************************************************************************/
void HapticsEventManager::errorCallbackGT(
	IHapticDevice::EventType event,
	const IHapticDevice::IHapticDeviceState * const pState,
	void *pUserData)
{
	HapticsEventManager *pThis = static_cast<HapticsEventManager *>(pUserData);

	if (hduIsForceError(&pState->getLastError()))
	{
	}
	else
	{
		/* This is likely a more serious error, so just bail. */
		std::cerr << pState->getLastError() << std::endl;
		std::cerr << "Error during haptic rendering" << std::endl;
		std::cerr << "Press any key to quit." << std::endl;
		getchar();
		exit(-1);
	}
}

/******************************************************************************/

