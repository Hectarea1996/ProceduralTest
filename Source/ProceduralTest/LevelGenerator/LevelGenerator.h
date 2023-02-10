// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelInfo.h"
#include <list>
#include "LevelEntryComponent.h"
#include "LevelDynamicLoadSphere.h"
#include "LevelGenerator.generated.h"

UCLASS()
class ALevelGenerator : public AActor
{
	GENERATED_BODY()
	
public:

  UPROPERTY(EditDefaultsOnly)
  uint32 MaxRooms = 10;

  
  // Primera sala en spawnear. Deberia contener un Player Start actor
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<TSubclassOf<ALevelInfo>> InitialRooms;

  // Conjunto de niveles que pueden spawnear
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<TSubclassOf<ALevelInfo>> Rooms;

  // Conjunto de niveles que se colocaran al final
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<TSubclassOf<ALevelInfo>> LastRooms;

  // Conjunto de niveles que sirven como limites
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<TSubclassOf<ALevelInfo>> RoomLimits;

  UPROPERTY(EditDefaultsOnly)
  float RoomsSphereRadius = 1000.0f;
  
  UPROPERTY(EditDefaultsOnly)
  float InSphereRadius = 3000.0f;

  UPROPERTY(EditDefaultsOnly)
  float OutSphereRadius = 5000.0f;

private:

  // Conjunto de niveles ya encajados
  UPROPERTY()
  TArray<ALevelInfo*> FixedRooms;

  // Conjunto de entradas libres.
  std::list<ULevelEntryComponent*> OpenEntries;

  UPROPERTY()
  TArray<ULevelDynamicLoadSphere*> LevelInSpheres;

  UPROPERTY()
  TArray<ULevelDynamicLoadSphere*> LevelOutSpheres;

  
public:
  
  /**
   * Genera de manera procedural un nivel entero
   */
  UFUNCTION(BlueprintCallable,CallInEditor,Category="Chaos Project | Level Generator")
  bool GenerateLevel();

  UFUNCTION(BlueprintCallable,CallInEditor,Category="Chaos Project | Level Generator")
  void GenerateLevelText(){
    GenerateLevel();
  }
  
  /**
   * Destruye un nivel entero.
   */
  UFUNCTION(BlueprintCallable,Category="Chaos Project | Level Generator")
  void DestroyLevel();
 
  /**
   * Destruye el nivel actual y genera otro nuevo.
   */
  UFUNCTION(BlueprintCallable,Category="Chaos Project | Level Generator")
  bool RestartLevel();

  /**
   * Get the possible transforms to spawn.
   */
  TArray<FTransform> GetInitialTransforms();

  /**
   * Hide the rooms that are far from player.
   */
  void HideRoomsFarFromPlayer();
  
private:
 
  /**
   * Coloca la primera sala.
   */
  void FixInitialRoom(TSubclassOf<ALevelInfo> levelStartClass);

  /**
   * Intenta colocar una sala junto con las que ya estan colocadas.
   * Devuelve un booleano indicando si se ha podido colocar la sala.
   */
  bool FixRoom(TSubclassOf<ALevelInfo> levelRoomClass);
  
  /**
   *	Indica a cada level info que genere el nivel
   */
  void SpawnRooms();

  UFUNCTION()
  void OnActivateSphereOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			       int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

  UFUNCTION()
  void OnDeactivateSphereOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
  
	
public:	
  // Sets default values for this actor's properties
  ALevelGenerator();
	
	
};
