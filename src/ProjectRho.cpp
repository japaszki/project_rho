#include "ProjectRho.h"

ProjectRho::ProjectRho(void)
	: mRoot(0),
    mCamera(0),
    mSceneMgr(0),
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mCursorWasVisible(false),
    mShutDown(false),
    mInputManager(0),
    mMouse(0),
    mKeyboard(0)
{
}

ProjectRho::~ProjectRho(void)
{
    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}

CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
    switch (buttonID)
    {
    case OIS::MB_Left:
        return CEGUI::LeftButton;
 
    case OIS::MB_Right:
        return CEGUI::RightButton;
 
    case OIS::MB_Middle:
        return CEGUI::MiddleButton;
 
    default:
        return CEGUI::LeftButton;
    }
}

bool ProjectRho::configure(void)
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true, "ProjectRho Render Window");

        return true;
    }
    else
    {
        return false;
    }
}

void ProjectRho::chooseSceneManager(void)
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}

void ProjectRho::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");
    mCamera->setNearClipDistance(5);
	mCamera->setFarClipDistance(1000000);
}

Ogre::SceneManager* ProjectRho::getSceneManager()
{
	return mSceneMgr;
}

OIS::Mouse* ProjectRho::getMouse()
{
	return mMouse;
}
//-------------------------------------------------------------------------------------
void ProjectRho::createScene(void)
{
	//Initialise top-level managers
	mUnitMgr = new UnitManager(this);
	mPlanetMgr = new PlanetManager(this);
	mUIMgr = new UIManager(this, mCamera);


	//debug:
	Ogre::SceneNode* RayNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("RayNode");
	Ogre::ManualObject* RayObject = mSceneMgr->createManualObject("RayObject");
	RayNode->attachObject(RayObject);

	//Enable Compositor
	Ogre::CompositorInstance* testComp = Ogre::CompositorManager::getSingleton().addCompositor(mViewport, "Bloom");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, "Bloom", true);

	//Lights
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.05f, 0.05f, 0.05f));
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);

	Ogre::Light* mainlight = mSceneMgr->createLight("MainLight");
    mainlight->setPosition(0, 0, 0);
	mainlight->setCastShadows(TRUE);
	mainlight->setDiffuseColour(Ogre::ColourValue(0.5f, 0.5f, 0.3f));
	mainlight->setSpecularColour(Ogre::ColourValue(0.8f, 0.8f, 0.5f));

	//Sky box
	//mSceneMgr->setSkyBox(true, "ProjectRhoSkyBox", 5000, true); //3rd parameter is clip distance, may need adjusting

	//Initialise planets
	PlanetData SunData, FirstPlanetData, MoonData;
	SunData.GravityStrength = 100;
	SunData.HUDRadius = 600;
	SunData.OcclusionRadius = 600;

	Planet* Sun = mPlanetMgr->AddPlanet("Sun");
	Sun->IsSun(SunData, "Sun.mesh");

	FirstPlanetData.GravityStrength = 20;
	FirstPlanetData.Inclination = Ogre::Radian(0.5);
	FirstPlanetData.ArgumentOfPeriapsis = Ogre::Radian(Ogre::Math::HALF_PI);
	FirstPlanetData.LongitudeOfAscendingNode = Ogre::Radian(0.0);
	FirstPlanetData.SemiMajorAxis = 8000;
	FirstPlanetData.Eccentricity = 0.5;
	FirstPlanetData.TimeOfPeriapsisPassage = 0;
	FirstPlanetData.OrbitalPeriod = 50000;
	FirstPlanetData.MajorBody = Sun;
	FirstPlanetData.HUDRadius = 200;
	FirstPlanetData.OcclusionRadius = 200;

	Planet* FirstPlanet = mPlanetMgr->AddPlanet("Planet");
	FirstPlanet->setPlanetData(FirstPlanetData, "Uranus.mesh");

	MoonData.GravityStrength = 1;
	MoonData.Inclination = Ogre::Radian(-1);
	MoonData.ArgumentOfPeriapsis = Ogre::Radian(2);
	MoonData.LongitudeOfAscendingNode = Ogre::Radian(1);
	MoonData.SemiMajorAxis = 2000;
	MoonData.Eccentricity = 0;
	MoonData.TimeOfPeriapsisPassage = 0;
	MoonData.OrbitalPeriod = 10000;
	MoonData.MajorBody =  FirstPlanet;
	MoonData.HUDRadius = 80;
	MoonData.OcclusionRadius = 80;

	Planet* Moon = mPlanetMgr->AddPlanet("Moon");
	Moon->setPlanetData(MoonData, "Moon.mesh");

	//Update planets once to allow velocity to be calculated
	mPlanetMgr->UpdateAllPlanets(0.1);

	//Example unit data
	//For Status class:
	UnitParameters TestUnit;
	Engine TEngine;
	Fuel TFuel;
	TurretStatus LaserTurretS = TurretStatus();
	TestUnit.mHull.Mass = 1000000;
	TestUnit.mHull.Inertia = Ogre::Vector3(1000,1000,1000);
	TEngine.EngineType = "ForwardMain";
	TEngine.Direction = Ogre::Quaternion();
	TEngine.ThrustPosition = Ogre::Vector3(0,0,0);
	TEngine.Thrust = 0.0003;
	TEngine.FuelConsumption = 0;
	TEngine.FuelType = "Ammonia";
	TEngine.Throttle = 1.0;
	TFuel.FuelType = "Ammonia";
	TFuel.Capacity = 100;
	TFuel.Position = Ogre::Vector3::ZERO;
	TFuel.Amount = 1.0;
	LaserTurretS.TurretName = "LaserTurret";
	LaserTurretS.MinElevation = Ogre::Radian(0);
	LaserTurretS.MaxElevation = Ogre::Radian(Ogre::Math::HALF_PI);
	LaserTurretS.MaxElevationSpeed = Ogre::Radian(0.0001);
	LaserTurretS.MinAzimuth = Ogre::Radian(0);
	LaserTurretS.MaxAzimuth = Ogre::Radian(0);
	LaserTurretS.MaxAzimuthSpeed = Ogre::Radian(0.01);
	LaserTurretS.setSpeed(Ogre::Radian(0.0001), Ogre::Radian(0.001));

	TestUnit.AddEngine(TEngine);
	TestUnit.AddFuel(TFuel);
	TestUnit.AddTurret(LaserTurretS);

	//For Visuals class:
	NodeElement Frame, Turret, LaserBarrel, Nosecone1, Nosecone2;

	Ogre::Quaternion NoseconeRotation, NoseconePivot, BarrelPivot;
	NoseconeRotation.FromAngleAxis(Ogre::Radian(Ogre::Math::PI), Ogre::Vector3(0,1,0));
	NoseconePivot.FromAngleAxis(Ogre::Radian(0.002), Ogre::Vector3(0,0,1));
	BarrelPivot.FromAngleAxis(Ogre::Radian(Ogre::Math::HALF_PI), Ogre::Vector3(1,0,0));

	Frame.ElementName = "Frame";
	Frame.MeshName = "Scarab_Frame.mesh";
	Frame.AxisOffset = Ogre::Vector3(0,0.7,0);
	Turret.ElementName = "Turret";
	Turret.MeshName = "Scarab_Turret.mesh";
	Turret.AxisOffset = Ogre::Vector3(0,0,0);
	Turret.BasePosition = Ogre::Vector3(0,-2.5,0);
	Turret.BaseOrientation = Ogre::Quaternion::IDENTITY;
	LaserBarrel.ElementName = "LaserBarrel";
	LaserBarrel.MeshName = "Scarab_LaserBarrel.mesh";
	LaserBarrel.AxisOffset = Ogre::Vector3(0,0,0);
	LaserBarrel.BasePosition = Ogre::Vector3(0,-1.3,0);
	LaserBarrel.BaseOrientation = BarrelPivot;
	Nosecone1.ElementName = "Nosecone1";
	Nosecone1.MeshName = "Scarab_Nosecone.mesh";
	Nosecone1.AxisOffset = Ogre::Vector3(0.55,0,0);
	Nosecone1.BasePosition = Ogre::Vector3(0.55,-0.9,0);
	Nosecone1.BaseOrientation = Ogre::Quaternion::IDENTITY;
	Nosecone2.ElementName = "Nosecone2";
	Nosecone2.MeshName = "Scarab_Nosecone.mesh";
	Nosecone2.AxisOffset = Ogre::Vector3(0.55,0,0);
	Nosecone2.BasePosition = Ogre::Vector3(-0.55,-0.9,0);
	Nosecone2.BaseOrientation = NoseconeRotation;
	LaserBarrel.SubNodes.push_back(Nosecone1);
	LaserBarrel.SubNodes.push_back(Nosecone2);
	Turret.SubNodes.push_back(LaserBarrel);
	Frame.SubNodes.push_back(Turret);

	TurretVisuals LaserTurret;
	LaserTurret.AssemblyName = "LaserTurret";
	LaserTurret.TurretName = "Turret";
	LaserTurret.BarrelName = "LaserBarrel";
	LaserTurret.TurretAxis = Ogre::Vector3(0,-1,0);
	LaserTurret.BarrelAxis = Ogre::Vector3(-1,0,0);

	Track Track1, Track2;
	Animation mAnim;
	mAnim.AnimName = "Open";
	mAnim.Duration = 2000;
	Ogre::Animation* TestAnim = mSceneMgr->createAnimation("Open", 2000);
	Ogre::NodeAnimationTrack* TestTrack = TestAnim->createNodeTrack(0);
	TestTrack->setUseShortestRotationPath(true);
	Ogre::TransformKeyFrame* Key = TestTrack->createNodeKeyFrame(0);
	Key->setRotation(Ogre::Quaternion::IDENTITY);
	Key = TestTrack->createNodeKeyFrame(300);
	Key->setRotation(NoseconePivot);
	Key = TestTrack->createNodeKeyFrame(1700);
	Key->setRotation(NoseconePivot);
	Key = TestTrack->createNodeKeyFrame(2000);
	Key->setRotation(Ogre::Quaternion::IDENTITY);
	Track1.ElementName = "Nosecone1";
	Track1.AnimationTrack = TestTrack;
	Track2.ElementName = "Nosecone2";
	Track2.AnimationTrack = TestTrack;
	mAnim.AddTrack(Track1);
	mAnim.AddTrack(Track2);


	//Initialise units
	Unit* NewUnit = mUnitMgr->CreateUnit();
	NewUnit->getPhysics()->setPosition(Ogre::Vector3(2000, 0, 0));
	NewUnit->getPhysics()->setVelocity(Ogre::Vector3(0, 0.01, 0.2));
	NewUnit->getPhysics()->setFacing(Ogre::Vector3::UNIT_Z.getRotationTo(Ogre::Vector3(1,1,0)));
	NewUnit->getPhysics()->setAngularVelocity(Ogre::Vector3::ZERO);
	NewUnit->getStatus()->setUnitParameters(TestUnit);
	NewUnit->getVisuals()->setNodeTree(Frame);
	NewUnit->getVisuals()->AddAnimation(mAnim);
	NewUnit->getVisuals()->AddTurret(LaserTurret);

	NewUnit = mUnitMgr->CreateUnit();
	NewUnit->getPhysics()->setPosition(Ogre::Vector3(1000, 0, 0));
	NewUnit->getPhysics()->setVelocity(Ogre::Vector3(0, 0, 0.3));
	NewUnit->getPhysics()->setFacing(NewUnit->getPhysics()->getAntiNormal());
	NewUnit->getPhysics()->setAngularVelocity(Ogre::Vector3(0, 0, 0));
	NewUnit->getStatus()->setUnitParameters(TestUnit);
	NewUnit->getVisuals()->setNodeTree(Frame);
	NewUnit->getVisuals()->AddAnimation(mAnim);
	NewUnit->getVisuals()->AddTurret(LaserTurret);
}

void ProjectRho::destroyScene(void)
{
}

void ProjectRho::createViewports(void)
{
    // Create one viewport, entire window
    mViewport = mWindow->addViewport(mCamera);
    mViewport->setBackgroundColour(Ogre::ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));
}

void ProjectRho::setupResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}

void ProjectRho::createResourceListener(void)
{
}

void ProjectRho::loadResources(void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

bool ProjectRho::keyPressed(const OIS::KeyEvent &arg )
{
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectKeyDown(arg.key);
	sys.injectChar(arg.text);
	return true;
}

bool ProjectRho::keyReleased(const OIS::KeyEvent &arg)
{
	CEGUI::System::getSingleton().injectKeyUp(arg.key);
	return true;
}

bool ProjectRho::mouseMoved(const OIS::MouseEvent &arg)
{
	//Inject mouse movements to CEGUI
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
	sys.injectMousePosition(arg.state.X.abs, arg.state.Y.abs);
	// Scroll wheel.
	if (arg.state.Z.rel)
    sys.injectMouseWheelChange(arg.state.Z.rel / 120.0f);

	//Inject mouse movements to UI
	mUIMgr->MouseMoved(arg.state);
    return true;
}

bool ProjectRho::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
	mUIMgr->MousePressed(arg.state, id);
	return true;
}

bool ProjectRho::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
	mUIMgr->MouseReleased(arg.state, id);
	return true;
}

bool ProjectRho::quit(const CEGUI::EventArgs &e)
{
	mShutDown = true;
	return true;
}

void ProjectRho::createFrameListener(void)
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem( pl );

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    //Set initial mouse clipping size
    windowResized(mWindow);

	// set initial mouse position
	OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(mMouse->getMouseState());
	mutableMouseState.X.abs = mCamera->getViewport()->getActualWidth() / 2;
	mutableMouseState.Y.abs = mCamera->getViewport()->getActualHeight() / 2;

    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

    mRoot->addFrameListener(this);
}

bool ProjectRho::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(mWindow->isClosed())
		return false;
 
    if(mShutDown)
		return false;
 
    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();
 
    //Need to inject timestamps to CEGUI System.
    CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);
	
	//Update game engine
	mUnitMgr->UpdateAllUnits(1000 * evt.timeSinceLastFrame); //1000x speedup
	mPlanetMgr->UpdateAllPlanets(1000 * evt.timeSinceLastFrame);
	mUIMgr->UpdateUI(evt.timeSinceLastFrame); //Needs to be called after planets and units are updated to prevent camera shaking when following

	return true;
}

bool ProjectRho::setup(void)
{
    mRoot = new Ogre::Root(mPluginsCfg);

    setupResources();

    bool carryOn = configure();
    if (!carryOn) return false;

    chooseSceneManager();
    createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();

    // Create the scene
    createScene();

    createFrameListener();

    return true;
}

void ProjectRho::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void ProjectRho::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}

void ProjectRho::go(void)
{
#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

    if (!setup())
        return;

    mRoot->startRendering();

    // clean up
    destroyScene();
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        ProjectRho app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
