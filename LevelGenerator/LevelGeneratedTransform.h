
#include "Math/Quat.h"
#include "Math/Vector.h"
#include "Math/TransformNonVectorized.h"



namespace FLevelGeneratedTransform{

  /**
   * Retorna la rotacion que debe de hacer una sala para ajustarse a la rotacion de
   * otra sala.
   */
  static FQuat getMatchRotation(const FQuat& fix, const FQuat& mov);

  /**
   * Retorna el escalado que debe hacer una sala para ajustarse al escalado de otra sala.
   */
  static FVector getMatchScale(const FVector& fix, const FVector& mov);
  
  
  /**
   * Retorna la traslacion que debe de hacer una sala para ajustarse a la posicion
   * de otra sala.
   */
  static FVector getMatchTraslation(const FVector& fix, const FVector& mov);

  
  /**
   * Retorna la transformacion que debe de hacer una sala para ajustarse a la transformacion
   * de otra sala.
   */
  FTransform getMatchTransform(const FTransform& fix, const FTransform& mov);
  
}
