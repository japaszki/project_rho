#pragma once
#include "stdafx.h"
class Utils
{
public:
	Utils(void);
	virtual ~Utils(void);
	Ogre::Vector2 Utils::ConvertWorldToScreenPosition(const Ogre::Vector3& worldPosition, Ogre::Camera* camera);
};

