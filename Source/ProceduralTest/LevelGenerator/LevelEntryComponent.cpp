// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelEntryComponent.h"


bool ULevelEntryComponent::HasSuitableTags(const ULevelEntryComponent& other){
  if (AcceptedTags.Num()==0)
    return true;
  for (auto& tag : AcceptedTags){
    for (auto& otherTag : other.EntryTags){
      if (tag.Equals(otherTag,ESearchCase::IgnoreCase))
	return true;
    }
  }
  return false;
}


bool ULevelEntryComponent::IsNear(const ULevelEntryComponent& other){
  return FVector::Dist(GetComponentLocation(),other.GetComponentLocation()) < 1.0f;
}


bool ULevelEntryComponent::HasOppositeDirection(const ULevelEntryComponent& other, float epsilon){
  return (FVector::DotProduct(GetComponentQuat().GetForwardVector(),other.GetComponentQuat().GetForwardVector()) + 1.0f) < epsilon
    && (FVector::DotProduct(GetComponentQuat().GetUpVector(),other.GetComponentQuat().GetUpVector()) - 1.0f) < epsilon;
}




