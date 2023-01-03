// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelEntryComponent.h"
#include "LevelPlayerStart.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Components/SphereComponent.h"
#include <list>
#include "LevelInfo.generated.h"

UCLASS()
class ALevelInfo : public AActor{
  GENERATED_BODY()

  public:

  // Los componentes que definen el espacio de colision de un nivel
  /*UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<UShapeComponent*> ColliderComponents;*/

  UPROPERTY(EditDefaultsOnly, meta=(ClampMin="1",UIMin="1"))
  uint32 ProbabilityPoints = 100;

  UPROPERTY(EditDefaultsOnly)
  uint32 MinSpawns = 0;

  UPROPERTY(EditDefaultsOnly)
  uint32 MaxSpawns = 0;
  
  // Los componentes que definen las entradas a un nivel.
  UPROPERTY()
  TArray<ULevelEntryComponent*> Entries;	
  
  // UPROPERTY()
  //TArray<ALevelPlayerStart*> Starts;
  
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<TSubclassOf<AActor>> SpawnableLevels;

  UPROPERTY()
  AActor* SpawnedLevel = nullptr;

  // UPROPERTY() (Esferas por cada nivel? No gracias)
  // USphereComponent* ToActivateRoomSphere = nullptr;

  // UPROPERTY(EditDefaultsOnly)
  // float ActivateSphereRadius = 100.0f;

  // UPROPERTY()
  // USphereComponent* ToDeactivateRoomSphere = nullptr;

  // UPROPERTY(EditDefaultsOnly)
  // float DeactivateSphereRadius = 150.0f;
  
private:

  // UFUNCTION()
  // void OnActivateSphereOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
  //  			       int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

  // UFUNCTION()
  // void OnDeactivateSphereOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
  
public:

  /**
   * Return the entries of this level info.
   */
  TArray<ULevelEntryComponent*> GetEntries(){
    return Entries;
  };

  /**
   * Comprueba si existe una transformacion que pueda realizar una sala para ajustarse al nivel 
   * que contiene las salas fijas.
   * En caso firmativo, devuelve la transformacion y todas las nuevas entradas que ahora estaran
   * pegadas.
   */
  bool getMatchLevelTransform(const TArray<ALevelInfo*>& fixedLevels, const TArray<std::list<ULevelEntryComponent*>::iterator>& fixedEntries, FTransform& result,
			      TArray<std::list<ULevelEntryComponent*>::iterator>& usedEntries, TArray<ULevelEntryComponent*>& newOpenEntries);

  /**
   * Spawnea el nivel en la posicion y rotacion del levelGenerationInfo.
   */
  void SpawnLevel();

  /**
   * Destruye el nivel si se spawneo con anterioridad.
   */
  void DestroyLevel();

  /**
   * Devuelve los transforms donde el jugador puede aparecer.
   */
  // TArray<FTransform> GetStartTransforms();

  /*
   *  Sets default values for this actor's properties
   */
  ALevelInfo();

  
 


protected:

// #if WITH_EDITOR
//   virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
// #endif
  
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;
	
};
