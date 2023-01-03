// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelInfo.h"
#include "Kismet/KismetMathLibrary.h"
#include "LevelGeneratedTransform.h"
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>


// Sets default values
ALevelInfo::ALevelInfo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

}



// Called when the game starts or when spawned
void ALevelInfo::BeginPlay()
{
  Super::BeginPlay();

  // Asignamos el radio de accion de las esferas
  //ToActivateRoomSphere->SetSphereRadius(ActivateSphereRadius);
  //ToDeactivateRoomSphere->SetSphereRadius(DeactivateSphereRadius);

  // Desactivamos la colision de las esferas para el generador.
  //ToActivateRoomSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  //ToDeactivateRoomSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  
  // Obtenemos los componentes que describen los puntos de union.
  GetComponents<ULevelEntryComponent>(this->Entries, false);

  // Obtenemos los componentes que describen los puntos de spawn.
  //GetComponents<ULevelStart>(this->Starts, false);
  
}



// void ALevelInfo::OnActivateSphereOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
// 							int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){

//   ACharacter* player = Cast<ACharacter>(OtherActor);

//   // Si el actor es el jugador...
//   if (player && SpawnedLevel){
//     // Activamos la sala
//     SpawnedLevel->SetActorHiddenInGame(false);
//     SpawnedLevel->SetActorEnableCollision(true);
//     SpawnedLevel->SetActorTickEnabled(true); //(ver mejor esto)
//   }
  
// }


// void ALevelInfo::OnDeactivateSphereOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){

//   ACharacter* player = Cast<ACharacter>(OtherActor);
  
//   // Si el actor es el jugador...
//   if (player && SpawnedLevel){
//     // Desactivamos la sala
//     SpawnedLevel->SetActorHiddenInGame(true);
//     SpawnedLevel->SetActorEnableCollision(false);
//     SpawnedLevel->SetActorTickEnabled(false); //(ver mejor esto)
//   }
  
// }


template<typename T>
static TArray<T> getShuffledArray(const TArray<T>& srcArray){

  std::vector<size_t> shuffledIndices;
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  shuffledIndices.resize(srcArray.Num());
  for (size_t i = 0; i < shuffledIndices.size(); i++)
    shuffledIndices[i]=i;
  std::shuffle(shuffledIndices.begin(),shuffledIndices.end(),std::default_random_engine(seed));

  TArray<T> shuffledArray;
  for (size_t i = 0; i < shuffledIndices.size(); i++){
    shuffledArray.Add(srcArray[shuffledIndices[i]]);
  }

  return shuffledArray;
  
}

bool ALevelInfo::getMatchLevelTransform(const TArray<ALevelInfo*>& fixedLevels, const TArray<std::list<ULevelEntryComponent*>::iterator>& fixedEntries, FTransform& result,
						  TArray<std::list<ULevelEntryComponent*>::iterator>& usedEntries, TArray<ULevelEntryComponent*>& newOpenEntries) {

  for (ULevelEntryComponent* movArrow : getShuffledArray<ULevelEntryComponent*>(Entries)) {
    for (std::list<ULevelEntryComponent*>::iterator nodeFixArrow : getShuffledArray<std::list<ULevelEntryComponent*>::iterator>(fixedEntries)) {

      ULevelEntryComponent* fixArrow = *nodeFixArrow;

      if (fixArrow->HasSuitableTags(*movArrow)){
	
	FTransform levelTransform = FLevelGeneratedTransform::getMatchTransform(fixArrow->GetComponentTransform(), movArrow->GetRelativeTransform());

	this->SetActorTransform(levelTransform, false, nullptr, ETeleportType::ResetPhysics);

	bool valid = true;
	for (ALevelInfo* fixedLevel2 : fixedLevels) {
	  if (this->IsOverlappingActor(fixedLevel2)) {
	    valid = false;
	    break;
	  }
	}

	TArray<std::list<ULevelEntryComponent*>::iterator> possibleUsedEntries;
	TArray<ULevelEntryComponent*> possibleOpenEntries;
	for (size_t i = 0; i < Entries.Num() && valid; i++){
	  ULevelEntryComponent* movEntry = Entries[i];
	  bool isMovEntryOpen = true;
	  for (size_t j = 0; j < fixedEntries.Num() && valid; j++){
	    ULevelEntryComponent* fixEntry = *fixedEntries[j];
	    if (fixEntry->IsNear(*movEntry)){
	      if (fixEntry->HasSuitableTags(*movEntry) && fixEntry->HasOppositeDirection(*movEntry,0.1f)){
		possibleUsedEntries.Add(fixedEntries[j]);
		isMovEntryOpen = false;
		break;
	      }
	      else{
		valid = false;
	      }
	    }
	  }
	  if (isMovEntryOpen){
	    possibleOpenEntries.Add(movEntry);
	  }
	}
	if (valid){
	  usedEntries = std::move(possibleUsedEntries);
	  newOpenEntries = std::move(possibleOpenEntries);
	  result = levelTransform;
	  return true;
	}
      }
    }		
  }

  return false;

}



void ALevelInfo::SpawnLevel() {

  check(SpawnableLevels.Num()>0)
  // if (SpawnableLevels.Num() == 0){
  //   UE_LOG(LogTemp, Warning, TEXT("No levels assigned to this level info!"));
  //   return;
  // }

  if (!SpawnedLevel){

    // Spawneamos el nivel
    int32 randIndex = FMath::RandRange(0, SpawnableLevels.Num()-1);
    SpawnedLevel = GetWorld()->SpawnActor<AActor>(SpawnableLevels[randIndex].Get());
    FTransform transform = GetActorTransform();
    SpawnedLevel->SetActorTransform(transform);

    // Lo pegamos a este actor (no entiendo la diferencia de los parametros)
    // FAttachmentTransformRules attachmentRules(EAttachmentRule::KeepWorld,true);
    // this->LevelSpawned->AttachToActor(this,attachmentRules);

    // Desactivamos los overlap de todos
    TArray<UPrimitiveComponent*> primitiveComponents;
    GetComponents<UPrimitiveComponent>(primitiveComponents,false);
    for (UPrimitiveComponent* primComponent : primitiveComponents){
      primComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    
    // Activamos los overlaps de las esferas
    // ToActivateRoomSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    // ToDeactivateRoomSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    // ToActivateRoomSphere->OnComponentBeginOverlap.AddDynamic(this,&ALevelInfo::OnActivateSphereOverlapBegin);
    // ToDeactivateRoomSphere->OnComponentEndOverlap.AddDynamic(this,&ALevelInfo::OnDeactivateSphereOverlapEnd);
   
  }

}


void ALevelInfo::DestroyLevel() {

  if (SpawnedLevel) {
    SpawnedLevel->Destroy();
    SpawnedLevel = nullptr;
  }

}
