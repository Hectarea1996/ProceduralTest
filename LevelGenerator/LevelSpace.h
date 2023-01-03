// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelSpace.generated.h"


UCLASS(Abstract)
class ALevelSpace : public AActor{
  GENERATED_BODY()

  protected:

  UPROPERTY()
  UShapeComponent* SpaceComponent = nullptr;
  
  public:	

  // Sets default values for this actor's properties
  ALevelSpace();
  
	
	
};
