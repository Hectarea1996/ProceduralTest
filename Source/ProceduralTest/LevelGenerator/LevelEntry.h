// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelEntryComponent.h"
#include "LevelEntry.generated.h"

UCLASS()
class ALevelEntry : public AActor
{
  GENERATED_BODY()

  private:

  UPROPERTY(VisibleAnywhere)
  ULevelEntryComponent* EntryComponent = nullptr;

public:
  UPROPERTY(EditAnywhere)
  int hola = 0;
	
  public:	
  // Sets default values for this actor's properties
  ALevelEntry();
	
};
