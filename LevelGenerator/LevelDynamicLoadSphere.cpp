// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelDynamicLoadSphere.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"


void ULevelDynamicLoadSphere::AddLevelRoom(ALevelInfo* room){
    LevelRooms.Add(room);
}


void ULevelDynamicLoadSphere::HideWhenFarFromPlayer(){
  bool farFromPlayer = !IsOverlappingActor(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));  
    for (ALevelInfo* room : GetLevelRooms()){
      room->SpawnedLevel->SetActorHiddenInGame(farFromPlayer);
      room->SpawnedLevel->SetActorEnableCollision(!farFromPlayer);
      room->SpawnedLevel->SetActorTickEnabled(!farFromPlayer); //(ver mejor esto)
    }
}
