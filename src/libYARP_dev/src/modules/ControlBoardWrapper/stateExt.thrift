
struct jointData
{
  1: list<string> name;
  2: list<double> position;
  3: list<double> velocity;
  4: list<double> effort;
}

struct motorData
{
  1: list<double> position;
  2: list<double> velocity;
  3: list<double> effort;
  4: list<double> current;
}

struct statusExtended
{
  1: string     robotPart;
  2: jointData  joint;
  3: motorData  motor;
  4: string     diagnostic;
}
