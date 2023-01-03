// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "LevelEntryComponent.generated.h"

/**
 * 
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class ULevelEntryComponent : public UArrowComponent
{
  GENERATED_BODY()
  
  public:

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FString> EntryTags;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FString> AcceptedTags;

  public:

  /**
   * Comprueba si un entrada posee tags compatibles con esta.
   */
  bool HasSuitableTags(const ULevelEntryComponent& other);

  /**
   * Comprueba si una entrada esta a menos de una unidad de distancia.
   */
  bool IsNear(const ULevelEntryComponent& other);

  /**
   * Comprueba si dos entries estan en direccion contraria
   */ 
  bool HasOppositeDirection(const ULevelEntryComponent& other, float epsilon);

	
	
};
