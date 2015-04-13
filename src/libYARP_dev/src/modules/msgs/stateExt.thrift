
struct jointData
{
  1: list<double> jointPosition;
  2: bool jointPosition_isValid;
  3: list<double> jointVelocity;
  4: bool jointVelocity_isValid;
  5: list<double> jointAcceleration;
  6: bool jointAcceleration_isValid;
  7: list<double> motorPosition;
  8: bool motorPosition_isValid;
  9: list<double> motorVelocity;
  10: bool motorVelocity_isValid;
  11: list<double> motorAcceleration;
  12: bool motorAcceleration_isValid;
  13: list<double> torque;
  14: bool torque_isValid;
  15: list<double> pidOutput;
  16: bool pidOutput_isValid;
  17: list<i32>	  controlMode;
  18: bool controlMode_isValid;
  19: list<i32>	  interactionMode;
  20: bool interactionMode_isValid;
}

