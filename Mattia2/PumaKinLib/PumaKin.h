
#ifndef __PumaKinematics__
#define __PumaKinematics__
 

class PumaKinematics
{
	private:
    PumaKinematics(const PumaKinematics&);
	void operator=(const PumaKinematics&);

	public:

	/**
	 * Default constructor. 
	 */
    PumaKinematics();

	/**
	 * Destructor.
	 */
	virtual ~PumaKinematics();
 

	//foward kinematics functions
//	Vector FwdPuma200Kin(double *angle, int lastjoint);
//	Vector FwdPuma200Kin(double *angle);
	Matrix FwdPuma200Kin(double *angle, int lastjoint);
	Matrix FwdPuma200Kin(double *angle);
	Matrix FwdPuma200Kin(Vector &angle, int lastjoint);
	Matrix FwdPuma200Kin(Vector &angle);

	//function returning direction of vector Z related to joint i (Rnumber)
	Vector GimmeZi(double *angle, int Rnumber);
	Vector GimmePos(double *angle,int Rnumber);

	//returns the rotation matrix with D-H convention related to the joint specified in Rnumber
	Matrix GimmeR(double *angle, int Rnumber);
	Matrix GimmeR4(double *angle, int Rnumber);



	Matrix PumaJac(double *ANGLE);

	Matrix PumaJac(Vector &angle);
	
	
//	Matrix PumaKinematics::MatrixInverted(Matrix &a, Matrix &b) ;

	protected:
	void *puma_resources;

};

Matrix zeros(int r, int c);
Vector zeros(int s);
Vector operator-(const Vector &a, const Vector &b);
double dot(const yarp::sig::Vector &a, const yarp::sig::Vector &b);


Vector operator*(const yarp::sig::Vector &a, const yarp::sig::Matrix &m);

Vector operator*(const yarp::sig::Matrix &m, const yarp::sig::Vector &a);
Matrix operator-(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b);

Vector operator*(const Vector &a, double k);
Vector operator*(double k, const Vector &b);

Matrix operator*(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b);
Vector operator+(const Vector &a, const Vector &b);
Matrix operator+(const yarp::sig::Matrix &a, const yarp::sig::Matrix &b);
/**
* Perform SVD decomposition on a NxM matrix. Golub Reinsch method.
*/
inline void SVD(const Matrix &in,Matrix &U,Vector &S,Matrix &V);

/**
* Perform the moore-penrose pseudo-inverse on 
* a NxM matrix. 
* 
*/
Vector RotMat2AxisAngle(Matrix RotMat);
Matrix AxisAngle2RotMat(Vector K);
Vector Quat2Euler(Vector Quat);
Vector Quat2Axis(Vector Quat);
Matrix pinv(const Matrix &in);
Matrix pinvCD(const Matrix &in);

	void RadToEnc( double *ang, double *encoders,  MEIMotionControlParameters &_parameters,  double *zeros);
	void EncToRad( double *ang, double *encoders,  MEIMotionControlParameters &_parameters,  double *zeros);

	Vector CrossProd(Vector vector1, Vector vector2);
	double DotProd(Vector vector1, Vector vector2);

	Vector dcm2axis(Matrix R);
	Matrix axis2dcm(Vector v);
	Matrix axis2dcm4x4(Vector v);
	Vector RotMat2Quat(Matrix RotationMatrix);
	Matrix Quat2RotMat(Vector Quat);
	Matrix Quat2RotMat4x4(Vector Quat);


	Vector computeError(Matrix matrix1, Matrix matrix2 );
	static void VisGaussJordanSolveDMatrix(Matrix& A);

	Matrix MatrixInverted(Matrix &A) ;
	void nrerror(char error_text[]);

#endif

