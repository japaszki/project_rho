#include "UnitManager.h"

UnitManager::UnitManager(ProjectRho* App)
{
	AppHandle = App;
	NextID = 0;
}

UnitManager::~UnitManager()
{
	for(int i = UnitArray.size(); i > 0; i--)
	{
		delete UnitArray.back();
	}
	UnitArray.~vector();
}

Unit* UnitManager::CreateUnit()
{
	Unit* NewUnit;
	NewUnit = new Unit(AppHandle, NextID++);
	UnitArray.push_back(NewUnit);
	return NewUnit;
}

Unit* UnitManager::getUnit(int Index)
{
	return UnitArray[Index];
}

void UnitManager::UpdateAllUnits(double TimeStep)
{
	for(int i = UnitArray.size() - 1; i >= 0; i--)
	{
		UnitArray[i]->UpdateUnit(TimeStep);
		UnitArray[i]->ConditionalPathUpdate();
	}
}

void UnitManager::DeleteUnit(int Index)
{
	UnitArray.erase(UnitArray.begin()+Index);
}

Unit* UnitManager::getUnitByID(int ID)
{
	for(int i = UnitArray.size() - 1; i >= 0; i--)
	{
		if(UnitArray[i]->getUnitID() == ID)
		{
			return UnitArray[i];
		}
	}
	return NULL;
}