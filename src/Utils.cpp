#include "stdafx.h"

Utils::Utils(void)
{
}


Utils::~Utils(void)
{
}

Ogre::Vector2 Utils::ConvertWorldToScreenPosition(const Ogre::Vector3& worldPosition, Ogre::Camera* camera)
{
    Ogre::Vector3 screenPosition = camera->getProjectionMatrix() * camera->getViewMatrix() * worldPosition;
    return Ogre::Vector2(0.5f + 0.5f * screenPosition.x, 0.5f - 0.5f * screenPosition.y);
}
