// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelCapsuleSpace.h"
#include "Components/CapsuleComponent.h"



ALevelCapsuleSpace::ALevelCapsuleSpace(){

  SpaceComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
  RootComponent = SpaceComponent;
  
}




