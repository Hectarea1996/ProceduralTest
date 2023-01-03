
#include "LevelGeneratedTransform.h"


FQuat FLevelGeneratedTransform::getMatchRotation(const FQuat& fix, const FQuat& mov) {

  FQuat halfTurnAround = FQuat(fix.GetUpVector(),PI);
  FQuat localMovInv = mov.Inverse();
  return halfTurnAround * fix * localMovInv;

}

FVector FLevelGeneratedTransform::getMatchTraslation(const FVector& fix, const FVector& mov) {

  return fix - mov;

}


FVector FLevelGeneratedTransform::getMatchScale(const FVector& fix, const FVector& mov){

  return fix/mov;
  
}


FTransform FLevelGeneratedTransform::getMatchTransform(const FTransform& fix, const FTransform& mov) {

  FVector scale = FLevelGeneratedTransform::getMatchScale(fix.GetScale3D(), mov.GetScale3D());
  FQuat rotation = FLevelGeneratedTransform::getMatchRotation(fix.GetRotation(), mov.GetRotation());
  FVector traslation = FLevelGeneratedTransform::getMatchTraslation(fix.GetTranslation(), rotation.RotateVector(scale*mov.GetTranslation()));

  return FTransform(rotation,traslation,scale);
  
}

