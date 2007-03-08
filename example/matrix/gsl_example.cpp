#include <stdio.h>

//#define GSL_DLL

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

//#include <gsl/gsl_math.h>
//#include <gsl/gsl_chebyshev.h>

#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>

using namespace yarp::sig;

int main(int argc, const char **argv)
{
	Matrix m1;
	Matrix m2;
	Matrix m3;
    m1.resize(2,3);
	m2.resize(3,2);
	m1=3;
    m2=3;

	m3.resize(2,2);
    m3.zero();

	gsl_matrix_view A1 = gsl_matrix_view_array(m1.data(), 2, 3);
	gsl_matrix_view A2 = gsl_matrix_view_array(m2.data(), 3, 2);
	gsl_matrix_view A3 = gsl_matrix_view_array(m3.data(), 2, 2);
 
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 
		1.0, &A1.matrix, &A2.matrix,
		0.0, &A3.matrix);

//	m3.print();
    printf("Result: (%s)", m3.toString().c_str());
	
	return 0; 
}

