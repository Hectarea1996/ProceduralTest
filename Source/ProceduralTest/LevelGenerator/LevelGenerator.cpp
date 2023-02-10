// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGenerator.h"
#include "LevelPlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Containers/UnrealString.h"
#include "EngineUtils.h"
#include <random>
#include <algorithm>
#include <vector>
#include <chrono>
#include <tuple>



// Sets default values
ALevelGenerator::ALevelGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}


void ALevelGenerator::FixInitialRoom(TSubclassOf<ALevelInfo> initialRoomInfoClass) {

  check(initialRoomInfoClass);
  
  // Metemos la sala inicial
  FVector loc = this->GetActorLocation();
  FRotator rot = this->GetActorRotation();
  FActorSpawnParameters spawnParameters;
  ALevelInfo* levelInfo = this->GetWorld()->SpawnActor<ALevelInfo>(initialRoomInfoClass, loc, rot, spawnParameters);
  check(levelInfo);

  // Insertamos la sala en la lista de salas fijadas
  FixedRooms.Push(levelInfo);

  // Insertamos los entries en la lista de open entries
  for (ULevelEntryComponent* entry : levelInfo->GetEntries()){
    OpenEntries.push_back(entry);
  }
    
}


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


static float getProbabilityFactor(const TArray<uint32>& minSpawns, uint32 totalRooms, uint32 roomsLeft){
  uint32 totalMinSpawns = 0;
  for (uint32 minRoomSpawns : minSpawns){
    totalMinSpawns += minRoomSpawns;
  }
  
  if (totalMinSpawns == 0)
    return 1.0f;

  return totalMinSpawns >= roomsLeft ? 0.0f : (1.0f - (totalRooms-roomsLeft)/(totalRooms-totalMinSpawns));
}

static bool IsGreaterPointsLevelInfo(const std::tuple<uint32,uint32,TSubclassOf<ALevelInfo>>& t1, const std::tuple<uint32,uint32,TSubclassOf<ALevelInfo>>& t2){
  return std::get<0>(t1) > std::get<0>(t2);
}

static TArray<std::pair<uint32,TSubclassOf<ALevelInfo>>> reorderRooms(const TArray<TSubclassOf<ALevelInfo>>& levelInfos,
									   const TArray<uint32>& minSpawns, const TArray<int64>& maxSpawns, uint32 totalRooms, uint32 roomsLeft){
  TArray<std::tuple<uint32,uint32,TSubclassOf<ALevelInfo>>> probabilities;
  float probFactor = getProbabilityFactor(minSpawns,totalRooms,roomsLeft);
  int index = 0;
  for (auto& levelInfo : levelInfos){
    if (maxSpawns[index]!=0){
      uint32 levelMaxProbability = levelInfo->GetDefaultObject<ALevelInfo>()->ProbabilityPoints;
      uint32 levelProbability = FMath::RoundHalfFromZero((minSpawns[index]>0 ? 1.0f : probFactor)*FMath::RandRange(1,levelMaxProbability));
      if (levelProbability>0)
	probabilities.Add(std::tuple<uint32,uint32,TSubclassOf<ALevelInfo>>(FMath::RandRange(0,levelProbability-1),index,levelInfo));
      index++;
    }
  }
  probabilities.Sort(IsGreaterPointsLevelInfo);

  TArray<std::pair<uint32,TSubclassOf<ALevelInfo>>> nextLevelRooms;
  for (int i = 0; i < probabilities.Num(); i++){
    nextLevelRooms.Add(std::pair<uint32,TSubclassOf<ALevelInfo>>(std::get<1>(probabilities[i]), std::get<2>(probabilities[i])));
  }

  return nextLevelRooms;
  
}


bool ALevelGenerator::FixRoom(TSubclassOf<ALevelInfo> roomInfoClass) {
    
  // Spawneamos la sala
  FVector loc = this->GetActorLocation();
  FRotator rot = this->GetActorRotation();
  ALevelInfo* levelInfo = this->GetWorld()->SpawnActor<ALevelInfo>(roomInfoClass, loc, rot);
  check(levelInfo);

  // Transformamos la lista de entries abiertos en un array de nodos.
  TArray<std::list<ULevelEntryComponent*>::iterator> fixedEntries;
  for (std::list<ULevelEntryComponent*>::iterator it = OpenEntries.begin(); it != OpenEntries.end(); it = std::next(it)){
    fixedEntries.Add(it);
  }
  
  // Comprobamos si se puede colocar la sala junto con el resto
  FTransform transform;
  TArray<std::list<ULevelEntryComponent*>::iterator> usedFixedEntries;
  TArray<ULevelEntryComponent*> newOpenEntries;
  bool success = false;
  success = levelInfo->getMatchLevelTransform(FixedRooms,fixedEntries,transform,usedFixedEntries,newOpenEntries);

  // En el caso de poder colocar el nivel...
  if (success) {
    
    // Eliminamos los entries usados.
    for (std::list<ULevelEntryComponent*>::iterator usedEntry : usedFixedEntries){
      OpenEntries.erase(usedEntry);
    }
    
    // Insertamos los nuevos open entries
    for (ULevelEntryComponent* newEntry : newOpenEntries){
      OpenEntries.push_back(newEntry);
    }
    
    // Colocamos el nivel
    levelInfo->SetActorTransform(transform, false, nullptr, ETeleportType::ResetPhysics);

    // Insertamos el nivel en la lista de niveles fijados
    this->FixedRooms.Push(levelInfo);
    
  }
  // En caso de fallo...
  else{
    // Destruimos el nivel
    levelInfo->Destroy();
  }  
  
  return success;
	
}


void ALevelGenerator::SpawnRooms() {

  for (ALevelInfo* fixedLevel : this->FixedRooms) {
    fixedLevel->SetActorEnableCollision(false);
    fixedLevel->SpawnLevel();
  }

}


bool ALevelGenerator::GenerateLevel(){

  // Obtenemos informacion sobre cada sala
  TArray<uint32> minSpawns;
  TArray<int64> maxSpawns;
  if (Rooms.Num()==0)
    UE_LOG(LogTemp,Warning,TEXT("Rooms are empty."));
  for (auto& levelInfo : Rooms){
    int64 minRoomSpawns = levelInfo->GetDefaultObject<ALevelInfo>()->MinSpawns;
    minSpawns.Add( minRoomSpawns );
    int64 maxRoomSpawns = levelInfo->GetDefaultObject<ALevelInfo>()->MaxSpawns;
    maxSpawns.Add( maxRoomSpawns > 0 ? maxRoomSpawns : -1 );
  }

  // Metemos una sala inicial.
  check(InitialRooms.Num()>0);
  int startIndex = FMath::RandRange(0, InitialRooms.Num() - 1);
  this->FixInitialRoom(InitialRooms[startIndex]);

  // Metemos MaxRooms salas o hasta no poder meter mas.
  bool unexpectedStop = false;
  for (uint32 i = 0; (i < MaxRooms) && !unexpectedStop; i++) {
    
    // Reordenamos al azar las salas.
    TArray<std::pair<uint32,TSubclassOf<ALevelInfo>>> reorderedRooms = reorderRooms(Rooms,minSpawns,maxSpawns,MaxRooms,MaxRooms-i);

    // Metemos una sala.
    bool levelFixed = false;
    for (int j = 0; j < reorderedRooms.Num() && !levelFixed; j++){
      levelFixed = this->FixRoom(reorderedRooms[j].second);

      // Si la sala se ha metido, disminuimos el minimo y maximo de spawns.
      if (levelFixed){
	uint32 index = reorderedRooms[j].first;
	if (minSpawns[index]>0)
	  minSpawns[index]--;
	if (maxSpawns[index]>0)
	  maxSpawns[index]--;
      }
      
    }

    // Comprobamos si no podemos seguir.
    unexpectedStop = !levelFixed;
    
  }

  // Si no se han colocado los niveles minimos, la generacion ha fallado.
  uint32 totalMinSpawns = 0;
  for (uint32 minSpawn : minSpawns){
    totalMinSpawns += minSpawn;
  }
  if (totalMinSpawns > 0){
    return false;
  }

  // Metemos las ultimas salas (son obligatorias)
  TArray<TSubclassOf<ALevelInfo>> reorderedLastRooms = getShuffledArray(LastRooms);
  bool lastRoomsPlaced = true;
  for (int j = 0; j < reorderedLastRooms.Num() && lastRoomsPlaced; j++){
    lastRoomsPlaced = FixRoom(reorderedLastRooms[j]);
  }

  // Si no se han colocado las ultimas salas obligatorias, la generacion ha fallado.
  if (!lastRoomsPlaced){
    return false;
  }

  // Metemos salas limites hasta no poder meter mas.
  bool fullOfLimits = false;
  while (!OpenEntries.empty() && !fullOfLimits) {
    
    // Reordenamos al azar los limites.
    TArray<TSubclassOf<ALevelInfo>> reorderedRoomLimits = getShuffledArray(RoomLimits);

    // Metemos un limite.
    bool limitFixed = false;
    for (int i = 0; i < reorderedRoomLimits.Num() && !limitFixed; i++){
      limitFixed = this->FixRoom(reorderedRoomLimits[i]);
    }

    // Comprobamos si hemos terminado.
    fullOfLimits = !limitFixed;
    
  }

  // Creamos las esferas para la carga dinamica de niveles
  int sphereId = 1;
  for (ALevelInfo* levelRoom : FixedRooms){
    bool inSomeSphere = false;
    for (int i=0; i < LevelInSpheres.Num(); i++){
      ULevelDynamicLoadSphere* inSphere = LevelInSpheres[i];
      ULevelDynamicLoadSphere* outSphere = LevelOutSpheres[i];
      if (FVector::Dist(inSphere->GetComponentLocation(),levelRoom->GetActorLocation())<RoomsSphereRadius){
	UE_LOG(LogTemp,Warning,TEXT("Radius: %f"),inSphere->GetScaledSphereRadius());
	inSphere->AddLevelRoom(levelRoom);
	outSphere->AddLevelRoom(levelRoom);
	inSomeSphere = true;
      }
      if (inSomeSphere)
	break;
    }
    if (!inSomeSphere){
      ULevelDynamicLoadSphere* newInSphere = NewObject<ULevelDynamicLoadSphere>(this,ULevelDynamicLoadSphere::StaticClass(),FName(FString::Printf(TEXT("NewInSphere%d"),sphereId)));
      ULevelDynamicLoadSphere* newOutSphere = NewObject<ULevelDynamicLoadSphere>(this,ULevelDynamicLoadSphere::StaticClass(),FName(FString::Printf(TEXT("NewOutSphere%d"),sphereId)));
      sphereId++;
      newInSphere->RegisterComponent();
      newOutSphere->RegisterComponent();
      //newInSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
      newInSphere->SetWorldTransform(levelRoom->GetTransform());
      newInSphere->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepWorldTransform);
      newInSphere->AddLevelRoom(levelRoom);
      newInSphere->SetSphereRadius(InSphereRadius);
      UE_LOG(LogTemp,Warning,TEXT("Nombre: %f"),InSphereRadius);
      newInSphere->OnComponentBeginOverlap.AddDynamic(this,&ALevelGenerator::OnActivateSphereOverlapBegin);
      newOutSphere->SetWorldTransform(levelRoom->GetTransform());
      newOutSphere->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepWorldTransform);
      newOutSphere->AddLevelRoom(levelRoom);
      newOutSphere->SetSphereRadius(OutSphereRadius);
      newOutSphere->OnComponentEndOverlap.AddDynamic(this,&ALevelGenerator::OnDeactivateSphereOverlapEnd);
      LevelInSpheres.Add(newInSphere);
      LevelOutSpheres.Add(newOutSphere);
    }
  }

  // UE_LOG(LogTemp,Warning,TEXT("InSpheres: %d"),LevelInSpheres[0]->GetRooms().Num());
  // UE_LOG(LogTemp,Warning,TEXT("OutSpheres: %d"),LevelOutSpheres.Num());

  // Movemos al jugador a algun punto de partida
  TArray<FTransform> startTransforms = GetInitialTransforms();
  if (startTransforms.Num()>0){
    int transformIndex = FMath::RandRange(0,startTransforms.Num()-1);
    UGameplayStatics::GetPlayerCharacter(GetWorld(),0)->SetActorTransform(startTransforms[transformIndex]);
  }
  else{
    UE_LOG(LogTemp, Warning, TEXT("No start position found!"));
  }
  
  // ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
  // check(player);
  // TArray<FTransform> startTransforms;
  // for (TActorIterator<ALevelPlayerStart> StartIt(GetWorld()); StartIt; ++StartIt){
  //   startTransforms.Add(StartIt->GetActorTransform());
  // }
  // if (startTransforms.Num()>0){
  //   int transformIndex = FMath::RandRange(0,startTransforms.Num()-1);
  //   player->SetActorTransform(startTransforms[transformIndex]);
  // }
  // else{
  //   UE_LOG(LogTemp, Warning, TEXT("No start position found!"));
  // }

  // Spawneamos las salas.
  this->SpawnRooms();

  // Ocultamos las salas que esten lejos del jugador.
  HideRoomsFarFromPlayer();

  return true;
  
}


void ALevelGenerator::DestroyLevel(){
  if (FixedRooms.Num()>0){
    for (ALevelInfo* fixedRoom : FixedRooms){
      fixedRoom->DestroyLevel();
      fixedRoom->Destroy();
    }
    FixedRooms.Empty();
  }
  if (LevelInSpheres.Num()>0){
    for (ULevelDynamicLoadSphere* inSphere : LevelInSpheres){
      inSphere->OnComponentBeginOverlap.RemoveAll(this);
      inSphere->DestroyComponent();
    }
    LevelInSpheres.Empty();
  }
  if (LevelOutSpheres.Num()>0){
    for (ULevelDynamicLoadSphere* outSphere : LevelOutSpheres){
      outSphere->OnComponentBeginOverlap.RemoveAll(this);
      outSphere->DestroyComponent();
    }
    LevelOutSpheres.Empty();
  }
}


bool ALevelGenerator::RestartLevel(){
  DestroyLevel();
  return GenerateLevel();
}


TArray<FTransform> ALevelGenerator::GetInitialTransforms(){
  TArray<FTransform> transforms;
  for (ALevelInfo* levelInfo : FixedRooms){
    TArray<FTransform> levelStarts = levelInfo->GetInitialTransforms();
    transforms.Append(levelStarts);
  }
  return transforms;
}


void ALevelGenerator::HideRoomsFarFromPlayer(){
  for (ULevelDynamicLoadSphere* inSphere : LevelInSpheres){
    inSphere->HideWhenFarFromPlayer();
  }
}


void ALevelGenerator::OnActivateSphereOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
							int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){

  ACharacter* player = Cast<ACharacter>(OtherActor);
  ULevelDynamicLoadSphere* dynamicSphere = Cast<ULevelDynamicLoadSphere>(OverlappedComp);

  // Si el actor es el jugador...
  if (player && dynamicSphere){
    UE_LOG(LogTemp,Warning,TEXT("Entra"));
    for (ALevelInfo* room : dynamicSphere->GetLevelRooms()){
      // Activamos la sala
      if (room->SpawnedLevel){
	room->SpawnedLevel->SetActorHiddenInGame(false);
	room->SpawnedLevel->SetActorEnableCollision(true);
	room->SpawnedLevel->SetActorTickEnabled(true); //(ver mejor esto)	
      }
    }
  }
  
}


void ALevelGenerator::OnDeactivateSphereOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){

  ACharacter* player = Cast<ACharacter>(OtherActor);
  ULevelDynamicLoadSphere* dynamicSphere = Cast<ULevelDynamicLoadSphere>(OverlappedComp);
  
  // Si el actor es el jugador...
  if (player && dynamicSphere){
    UE_LOG(LogTemp,Warning,TEXT("Sale"));
    UE_LOG(LogTemp,Warning,TEXT("Salas que salen: %d"),dynamicSphere->GetLevelRooms().Num());
    for (ALevelInfo* room : dynamicSphere->GetLevelRooms()){
      // Activamos la sala
      if (room->SpawnedLevel){
	room->SpawnedLevel->SetActorHiddenInGame(true);
	room->SpawnedLevel->SetActorEnableCollision(false);
	room->SpawnedLevel->SetActorTickEnabled(false); //(ver mejor esto)
      }
    }
  }
  
}





