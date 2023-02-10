// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSphereSpace.h"
#include "Components/SphereComponent.h"



ALevelSphereSpace::ALevelSphereSpace(){

  SpaceComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
  RootComponent = SpaceComponent;
  
}



