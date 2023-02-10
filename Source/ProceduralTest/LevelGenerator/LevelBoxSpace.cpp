// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelBoxSpace.h"
#include "Components/BoxComponent.h"


ALevelBoxSpace::ALevelBoxSpace(){

  SpaceComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
  RootComponent = SpaceComponent;
  
}


