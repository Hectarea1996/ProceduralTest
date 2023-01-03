// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "LevelInfo.h"
#include "LevelDynamicLoadSphere.generated.h"

/**
 * 
 */
UCLASS()
class ULevelDynamicLoadSphere : public USphereComponent
{
  GENERATED_BODY()
	
  private:

  UPROPERTY()
  TArray<ALevelInfo*> LevelRooms;
  
  
public:

  void AddLevelRoom(ALevelInfo* room);

  inline TArray<ALevelInfo*> GetLevelRooms(){ return LevelRooms; }

  void HideWhenFarFromPlayer();
	
};
