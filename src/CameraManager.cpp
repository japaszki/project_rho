
#pragma once
#include "CameraManager.h"

CameraManager::CameraManager(UIManager* UIManagerHandle, Ogre::Camera* PlayerCamera)
{
	Parent = UIManagerHandle;
	PlayerCameraHandle = PlayerCamera;

	CameraYawNode = Parent->getAppHandle()->getSceneManager()->createSceneNode();
	CameraYawNode->setPosition(0,0,0);
	CameraYawNode->setOrientation(Ogre::Quaternion::IDENTITY);

	CameraPitchNode = Parent->getAppHandle()->getSceneManager()->createSceneNode();
	CameraPitchNode->setPosition(0,0,0);
	CameraPitchNode->setOrientation(Ogre::Quaternion::IDENTITY);

	PlayerCameraHandle->setPosition(0,0,-2000);
	PlayerCameraHandle->lookAt(Ogre::Vector3::UNIT_Z);

	CameraPitchNode->attachObject(PlayerCameraHandle);
	CameraYawNode->addChild(CameraPitchNode);

	RotationMode = false;
	TranslateProgress = 0;
	MouseVel = 0;
	InitialPositionDifference = Ogre::Vector3::ZERO;

	mTargetData.Type = TARGETDATATYPE_NONE;
	mTargetData.Position = Ogre::Vector3::ZERO;
	mTargetData.TargetUnit = NULL;
	mTargetData.TargetPlanet = NULL;
}

CameraManager::~CameraManager(void)
{
	Parent->getAppHandle()->getSceneManager()->destroySceneNode(CameraYawNode);
	Parent->getAppHandle()->getSceneManager()->destroySceneNode(CameraPitchNode);
}

void CameraManager::Update(double TimeStep)
{
	const float PitchConstant = 0.9;//0.0015;
	const float YawConstant = -1.2;//-0.002;
	const float ZoomConstant = -0.036;//-0.00006;
	const float MinZoom = -100;
	const float MouseVelDragConstant = 0.95;
	const float ScrollDragConstant = 0.97;
	const float AutoTranslateConstant = 0.995;
	const float ScrollTranslateCoeff = 0.06;//0.0001;

	CameraYawNode->yaw(Ogre::Radian(MouseVel.x * YawConstant * TimeStep));
	//Limit pitch to +- 90deg
	if((CameraPitchNode->getOrientation().getPitch() + Ogre::Radian(MouseVel.y * PitchConstant * TimeStep) < Ogre::Radian(0.5*Ogre::Math::PI)) &&
		(CameraPitchNode->getOrientation().getPitch() + Ogre::Radian(MouseVel.y * PitchConstant * TimeStep) > Ogre::Radian(-0.5*Ogre::Math::PI)))
	{
		CameraPitchNode->pitch(Ogre::Radian(MouseVel.y * PitchConstant * TimeStep));
	}
	
	float ZoomFactor;
	ZoomFactor = MouseVel.z * TimeStep * PlayerCameraHandle->getPosition().z * ZoomConstant;
	//Impose minimum distance limit
	if((ZoomFactor + PlayerCameraHandle->getPosition().z <= MinZoom)||(ZoomFactor< 0))
	{
		PlayerCameraHandle->setPosition(Ogre::Vector3(0,0,ZoomFactor) + PlayerCameraHandle->getPosition());
	}
	//Follow if target is a planet
	if(mTargetData.Type == TARGETDATATYPE_PLANET)
	{
		CameraYawNode->setPosition(mTargetData.TargetPlanet->getPosition());
		//Gradually translate camera to point at target
		CameraPitchNode->setPosition(CameraPitchNode->getPosition() * AutoTranslateConstant);
	}
	//Follow if target is a unit
	else if(mTargetData.Type == TARGETDATATYPE_UNIT)
	{
		CameraYawNode->setPosition(mTargetData.TargetUnit->getPhysics()->getPosition());
		//Gradually translate camera to point at target
		CameraPitchNode->setPosition(CameraPitchNode->getPosition() * AutoTranslateConstant);
	}
	//Translate camera while zooming if target is a position
	else if(mTargetData.Type == TARGETDATATYPE_POSITION)
	{
		Ogre::Vector3 PositionDifference = mTargetData.Position - CameraYawNode->getPosition();
		//CameraYawNode->setPosition(CameraYawNode->getPosition() + std::max(std::min((ZoomFactor * ScrollTranslateCoeff),0.05f),0.0f) * PositionDifference);
		if(MouseVel.z > 0)
		{
			TranslateProgress += MouseVel.z * ScrollTranslateCoeff * TimeStep; //Check if required translation has been made to avoid overshooting
			if(TranslateProgress <= 1)
			{
				CameraYawNode->setPosition(CameraYawNode->getPosition() + MouseVel.z * ScrollTranslateCoeff * TimeStep * InitialPositionDifference);
			}

			//prevent cursor from being stuck at a target position when still zooming in but mousewheel is not being scrolled
			if(mMouseState.Z.rel > 0)
			{
				//set mouse position to target
				Ogre::Vector2 ScreenPosition = ConvertWorldToScreenPosition(mTargetData.Position, PlayerCameraHandle);
				setMousePosition(PlayerCameraHandle->getViewport()->getActualWidth() * ScreenPosition.x, PlayerCameraHandle->getViewport()->getActualHeight() * ScreenPosition.y);
			}
		}
	}
	//Coast to a stop
	MouseVel.x *= MouseVelDragConstant;
	MouseVel.y *= MouseVelDragConstant;
	MouseVel.z *= ScrollDragConstant;
}

void CameraManager::setTargetUnit(Unit* Target)
{
	mTargetData.Type = TARGETDATATYPE_UNIT;
	mTargetData.TargetUnit = Target;
	Ogre::Vector3 PositionDifference;
	PositionDifference = mTargetData.TargetUnit->getPhysics()->getPosition() - CameraYawNode->getPosition();
	CameraYawNode->translate(PositionDifference, Ogre::Node::TS_WORLD);
	CameraPitchNode->translate(-PositionDifference, Ogre::Node::TS_WORLD);
}

void CameraManager::setTargetPlanet(Planet* Target)
{
	mTargetData.Type = TARGETDATATYPE_PLANET;
	mTargetData.TargetPlanet = Target;
	Ogre::Vector3 PositionDifference;
	PositionDifference = mTargetData.TargetPlanet->getPosition() - CameraYawNode->getPosition();
	CameraYawNode->translate(PositionDifference, Ogre::Node::TS_WORLD);
	CameraPitchNode->translate(-PositionDifference, Ogre::Node::TS_WORLD);
}

void CameraManager::setTargetPosition(Ogre::Vector3 TargetPosition)
{
	mTargetData.Type = TARGETDATATYPE_POSITION;
	mTargetData.Position = TargetPosition;
	InitialPositionDifference = TargetPosition - CameraYawNode->_getDerivedPosition();
	TranslateProgress = 0;

	//Reset any yaw axis offset
	Ogre::Vector3 PositionDifference;
	PositionDifference = CameraPitchNode->_getDerivedPosition() - CameraYawNode->_getDerivedPosition();
	CameraYawNode->translate(PositionDifference, Ogre::Node::TS_WORLD);
	CameraPitchNode->translate(-PositionDifference, Ogre::Node::TS_WORLD);
}

void CameraManager::RemoveTarget()
{
	mTargetData.Type = TARGETDATATYPE_NONE;
}

void CameraManager::RemoveTargetUnit(Unit* Target)
{
	if((mTargetData.Type == TARGETDATATYPE_UNIT) && (mTargetData.TargetUnit == Target))
	{
		RemoveTarget();
	}
}

void CameraManager::RemoveTargetPlanet(Planet* Target)
{
	if((mTargetData.Type == TARGETDATATYPE_PLANET) && (mTargetData.TargetPlanet == Target))
	{
		RemoveTarget();
	}
}

void CameraManager::MouseMoved(OIS::MouseState State)
{
	//save mouse state
	mMouseState = State;

	//Rotate camera only in rotation mode
	if(RotationMode)
	{
		if(abs(State.X.rel) > abs(MouseVel.x))// Allow smooth stop 
		{
			MouseVel.x = State.X.rel;
		}
		if(abs(State.Y.rel) > abs(MouseVel.y))
		{
			MouseVel.y = State.Y.rel;
		}
	}
	//Scroll wheel always works
	if(abs(State.Z.rel) > abs(MouseVel.z))
	{
		MouseVel.z = State.Z.rel;
	}
}

void CameraManager::MousePressed(OIS::MouseState State, OIS::MouseButtonID id)
{
	switch(id)
	{
	case OIS::MB_Right: 
		RotationMode = true;
		break;
	default:
		RotationMode = false;
		break;
	}
}

void CameraManager::MouseReleased(OIS::MouseState State, OIS::MouseButtonID id)
{
	RotationMode = false;
}

Ogre::Real CameraManager::getTargetUnitDistance()
{
	if(mTargetData.Type == TARGETDATATYPE_UNIT)
	{
		return PlayerCameraHandle->getDerivedPosition().distance(mTargetData.TargetUnit->getPhysics()->getPosition());
	}
	else
	{
		return Ogre::Math::POS_INFINITY;
	}
}

void CameraManager::setMousePosition(int x, int y)
{
	OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(Parent->getAppHandle()->getMouse()->getMouseState());
	mutableMouseState.X.abs = x;
	mutableMouseState.Y.abs = y;
}