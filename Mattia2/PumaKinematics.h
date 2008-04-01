#ifndef __PumaKinematics__
#define __PumaKinematics__



//#define M_PI 3.14159265358979323846

class PumaKinematics
{


public:
PumaKinematics();

virtual ~PumaKinematics();
 

public:
Vector FwdPuma200Kin(Vector angle);
Matrix Puma200Jac(Vector &angle);
Matrix MatrixInverted(Matrix &A);
static void VisGaussJordanSolveDMatrix(Matrix &A);
Vector CrossProduct(Vector &a, Vector &b);
Matrix Puma200InvJac(Matrix &Puma200Jac);
Vector GetJointSpeed(double *OldPosition);
Vector jacobianINV(double *angles, double *vel3D);
void Puma200UpdateHValue(double *angles);

double det2x2(const double a, const double b, const double c, const double d) ;
double Det(Matrix &A);

	Vector jointVelocity, vel3D;
//	jointVelocity.resize(6);
//	vel3D.resize(6);





	protected:
	void *puma_resources;
/*
private:
PumaKinematics(const PumaKinematics&);
void operator=(const PumaKinematics&);
*/
};



#endif
