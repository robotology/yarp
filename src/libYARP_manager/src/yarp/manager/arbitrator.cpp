/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/arbitrator.h>
#include <yarp/manager/binexparser.h>

#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>

#ifdef WITH_YARPMATH
#include <cmath>
#include <yarp/math/Math.h>
//#include <gsl/gsl_version.h>
//#include <gsl/gsl_math.h>
//#include <gsl/gsl_eigen.h>
#endif



using namespace std;
using namespace yarp::sig;
using namespace yarp::manager;


/**
 * Class Arbitrator
 */
void Arbitrator::addRule(const char* con, const char* rule)
{
    if(con && rule)
    {
        rules[con] = rule;
        std::map<std::string, double> w;
        alphas[con] = w;
        biases[con] = 1.0;
    }
}

void Arbitrator::removeRule(const char* con)
{
    rules.erase(rules.find(con));
    alphas.erase(alphas.find(con));
    biases.erase(biases.find(con));

    /*
    // removing weights of the links to this con
    std::map<std::string, std::map<std::string, double> >::iterator itr;
    for(itr=alphas.begin(); itr!=alphas.end(); itr++)
    {
        std::map<std::string, double>& w = itr->second;
        std::map<std::string, double>::iterator jtr;
        for(jtr=w.begin(); jtr!=w.end(); jtr++)
            if(jtr->first == string(con))
                w.erase(jtr);
    }
    */
}


bool Arbitrator::trainWeights(const char* opnd)
{
    __CHECK_NULLPTR(opnd);

    ErrorLogger* logger  = ErrorLogger::Instance();

    string rule = getRule(opnd);
    if(!rule.length())
    {
        std::map<std::string, double> w;
        alphas[opnd] = w;
        biases[opnd] = 1.0;
        return true;
    }

    BinaryExpParser parser;
    std::map<string, string>::iterator itr;
    for(itr=rules.begin(); itr!=rules.end(); itr++)
        parser.addRestrictedOperand((itr->first).c_str());

    // parsing the compact logic
    rule = string(opnd) + " : " + rule;
    if(!parser.parse(rule))
        return false;

    // trining the weights
    LinkTrainer trainer;
    if(trainer.train(parser.getTruthTable()))
    {
        biases[opnd] = trainer.getBias();

        std::vector<double> alf = trainer.getAlphas();
        std::map<std::string, double> w;

        std::map<std::string, bool>::iterator itr;
        std::map<std::string, bool> operands = parser.getOperands();
        int i=0;
        for(itr=operands.begin(); itr!=operands.end(); itr++)
        {
            w[itr->first] = alf[i];
            i++;
        }
        alphas[opnd] = w;
    }
    else
    {
        logger->addError("Maximum number of iterations is reached without finding any solution. Check for the correctness of the expression logic.");
        return false;
    }

    return true;
}

bool Arbitrator::trainWeights()
{
    biases.clear();
    alphas.clear();
    bool bAllOk = true;
    std::map<string, string>::iterator itr;
    for(itr=rules.begin(); itr!=rules.end(); itr++)
        bAllOk &= trainWeights((itr->first).c_str());

    return bAllOk;
}

bool Arbitrator::validate()
{
    ErrorLogger* logger  = ErrorLogger::Instance();

    if(!trainWeights())
        return false;

#ifdef WITH_YARPMATH
//#if (GSL_MAJOR_VERSION >= 2 || (GSL_MAJOR_VERSION >= 1 && GSL_MINOR_VERSION >= 14))
    int n = alphas.size();
    if(n == 0)
        return true;

    yarp::sig::Matrix A(n, n);
    std::map<std::string, std::map<std::string, double> >::iterator itr;    // iterating over rows
    std::map<std::string, std::map<std::string, double> >::iterator jtr;    // iterating over cols

    int row = 0;
    for(itr=alphas.begin(); itr!=alphas.end(); itr++)
    {
        std::map<std::string, double>& w = itr->second;
        int col = 0;
        for(jtr=alphas.begin(); jtr!=alphas.end(); jtr++)
        {
            std::string opnd = jtr->first;
            if(w.find(opnd) != w.end())
                A(row,col) = w[opnd];
            else
                A(row,col) = 0.0;
            col++;
        }
        row++;
    }
    //printf("%s\n\n", A.toString(1).c_str());

    yarp::sig::Vector real;
    yarp::sig::Vector img;
    yarp::math::eigenValues(A, real, img);
    bool bStable = true;
    for(size_t i=0; i<real.size(); i++)
    {
        if((float)fabs(real[i]) >= 1.0)
        {
            bStable = false;
            logger->addError("Inconsistency in logical expressions. This will result an unstable arbitration system!");
            break;
        }
    }
    return bStable;

    /*
    gsl_vector_complex *eval = gsl_vector_complex_alloc(n);
    gsl_matrix_complex *evec = gsl_matrix_complex_alloc(n, n);
    gsl_eigen_nonsymmv_workspace * w = gsl_eigen_nonsymmv_alloc(n);

    gsl_eigen_nonsymmv ((gsl_matrix *)A.getGslMatrix(), eval, evec, w);

    bool bStable = true;
    for(int i=0; i<n; i++)
    {
        gsl_complex eval_i = gsl_vector_complex_get (eval, i);

        if((float)fabs(GSL_REAL(eval_i)) >= 1.0)
        {
            bStable = false;
            logger->addError("Inconsistency in logical expressions. This will result an unstable arbitration system!");
            break;
        }
        //printf ("eigenvalue = %.2f + %.2fi\n", GSL_REAL(eval_i), GSL_IMAG(eval_i));
    }

    gsl_eigen_nonsymmv_free(w);
    gsl_vector_complex_free(eval);
    gsl_matrix_complex_free(evec);
    return bStable;
    */

//#else //GSL_VERSION
//    logger->addWarning("The version of GNU Scientific Library (GSL) used in libYarpMath is insufficient (GSL_VERSION < 1.14). Your compact logical expression might result an unstable arbitration system!");
//    return true;
//#endif //GSL_VERSION

#else //WITH_YARPMATH
    logger->addWarning("Yarpmanager is compiled without libYarpMath. Your compact logical expression might result an unstable arbitration system!");
    return true;
#endif //WITH_YARPMATH

}
