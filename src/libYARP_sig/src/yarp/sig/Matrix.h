/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_MATRIX_H
#define YARP_SIG_MATRIX_H

#include <cstdlib> //defines size_t
#include <cstring> //memset
#include <yarp/os/Portable.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/ManagedBytes.h>

/**
* \file Matrix.h contains the definition of a Matrix type
*/

namespace yarp {
    namespace sig {
        class Matrix;
    }
}

namespace yarp {
    namespace sig {
        YARP_sig_API bool submatrix(const Matrix &in, Matrix &out, size_t r1, size_t r2, size_t c1, size_t c2);
        YARP_sig_API bool removeCols(const Matrix &in, Matrix &out, size_t first_col, size_t how_many);
        YARP_sig_API bool removeRows(const Matrix &in, Matrix &out, size_t first_row, size_t how_many);
    }
}

/**
* \ingroup sig_class
*
* A class for a Matrix. A Matrix can be sent/read to/from
* a port. Use the [] operator to access the r row of the matrix.
* The function returns a pointer so [][] access the r,c element
* in the matrix.
*/
class YARP_sig_API yarp::sig::Matrix: public yarp::os::Portable
{
private:
    double *storage;
    double **matrix; //double pointer access to elements

    size_t nrows;
    size_t ncols;

    /**
    * Update pointer to data, call this every time you
    * change the size of the object.
    */
    void updatePointers();

public:
    Matrix():
      storage(0),
          matrix(0),
          nrows(0),
          ncols(0)
      {}

      Matrix(size_t r, size_t c);

      /**
      * Copy constructor.
      */
      Matrix(const Matrix &m);

      /**
      * Destructor.
      */
      ~Matrix();

      /**
      * Copy operator.
      */
      const Matrix &operator=(const Matrix &r);

      /**
      * Set all elements of the matrix to a given value.
      * @param v a scalar
      * @return a reference to the object
      */
      const Matrix &operator=(double v);

      /**
      * Return number of rows.
      */
      size_t rows() const
      { return nrows; }

      /**
      * Return number of columns.
      */
      size_t cols() const
      { return ncols; }

      /**
      * Resize the matrix, if matrix is not empty preserve old content.
      * @param r number of rows
      * @param c number of columns
      */
      void resize(size_t r, size_t c);

      /**
      * Single element access, no range check. Returns
      * a pointer to the r-th row.
      * @param r row number.
      * @return a pointer to the first element of the row.
      */
      inline double *operator[](size_t r)
      { return matrix[r]; }

      /**
      * Single element access, no range check (const version).
      * Returns a pointer to the r-th row.
      * @param r row number.
      * @return a (const) pointer to the first element of the row.
      */
      inline const double *operator[](size_t r) const
      { return matrix[r]; }

      /**
      * Single element access, no range check.
      * @param r row number.
      * @param c col number.
      * @return a reference to the element.
      */
      inline const double &operator()(size_t r, size_t c) const
      { return matrix[r][c]; }

      /**
      * Single element access, no range check.
      * @param r row number.
      * @param c col number.
      * @return a (const) reference to the element.
      */
      inline double &operator()(size_t r, size_t c)
      { return matrix[r][c]; }

      /**
      * Zero the matrix. Set all elements of the matrix to zero.
      */
      void zero();

      /**
      * Set a row of the matrix copying the values from a vector: the vector length must be equal to the number of columns of the matrix.
      * @param row the row number
      * @param r vector which contains the desired values for the row
      * @return true if operation succeeds, false if not
      */
      bool setRow(size_t row, const Vector &r);

      /**
      * Set a column of the matrix copying the values from a vector: the vector length must be equal to the number of rows of the matrix.
      * @param col the column number
      * @param c a vector which contains the desired values for the column
      * @return true if operation succeeds, false if not
      */
      bool setCol(size_t col, const Vector &c);

      /**
      * Return the transposed of the matrix.
      * @return the transposed copy of the matrix.
      */
      Matrix transposed() const;

      /**
      * Build an identity matrix, don't resize.
      * @return a reference to the object.
      */
      const Matrix &eye();

      /**
      * Build a diagonal matrix, don't resize.
      * @return a reference to the object.
      */
      const Matrix &diagonal(const Vector &d);

      /**
      * Extract a submatrix from (r1,c1) to (r2,c2) (extremes included), as in
      * Matlab B=A(r1:r2, c1:c2).
      *
      * @param r1 start row
      * @param c1 start column
      * @param r2 end row
      * @param c2 end column
      * @return the sumbatrix
      */
      Matrix submatrix(size_t r1, size_t r2, size_t c1, size_t c2) const
      {
          Matrix ret;
          ret.resize(r2-r1+1, c2-c1+1);

          yarp::sig::submatrix((*this), ret, r1, r2, c1, c2);
          return ret;
      }

      /**
      * Set a portion of this matrix with the values of the specified matrix m.
      * The portion to be set is from row r to row r+m.rows()-1 and from column c
      * to column c+m.cols()-1.
      *
      * @param m matrix containing the values to set
      * @param r start row
      * @param c start column
      * @return true if the operation succeeded, false otherwise
      */
      bool setSubmatrix(const Matrix &m, size_t r, size_t c);

      /**
      * Set a portion of a row of this matrix with the values of the specified vector v.
      * The portion to be set is from column c to column c+v.size()-1.
      *
      * @param v vector containing the values to set
      * @param r index of the row to set
      * @param c start column
      * @return true if the operation succeeded, false otherwise
      */
      bool setSubrow(const Vector &v, size_t r, size_t c);

      /**
      * Set a portion of a column of this matrix with the values of the specified vector v.
      * The portion to be set is from row r to row r+v.size()-1.
      *
      * @param v vector containing the values to set
      * @param r start row index
      * @param c index of the column to set
      * @return true if the operation succeeded, false otherwise
      */
      bool setSubcol(const Vector &v, size_t r, size_t c);

      /**
      * Get a row of the matrix as a vector.
      * @param r the row number
      * @return a vector which contains the requested row
      */
      Vector getRow(size_t r) const;

      /**
      * Get a columns of the matrix as a vector.
      * @param c the column number
      * @return a vector which contains the requested row
      */
      Vector getCol(size_t c) const;

      /**
      * Modifies the matrix, removing one or more columns from it.
      * @param first_col the number of the first column to remove
      * @param how_many the number of columns to remove
      * @return the matrix with the specified columns removed
      */
      Matrix removeCols(size_t first_col, size_t how_many);

      /**
      * Modifies the matrix, removing one or more rows from it.
      * @param first_row the number of the first row to remove
      * @param how_many the number of rows to remove
      * @return the matrix with the specified rows removed
      */
      Matrix removeRows(size_t first_row, size_t how_many);

      /**
      * Get a subrow of the matrix as a vector.
      * @param r the row number
      * @param c the first column number
      * @param size the size of the subrow
      * @return a vector which contains the requested subrow
      */
      Vector subrow(size_t r, size_t c, size_t size) const;

      /**
      * Get a subcolumn of the matrix as a vector.
      * @param r the first row number
      * @param c the column number
      * @param size the size of the subcolumn
      * @return a vector which contains the requested subcolumn
      */
      Vector subcol(size_t r, size_t c, size_t size) const;

      /**
      * Print matrix to a string. Useful for display, writing to text.
      * To get a nice format the optional parameters precision and width may be used (same meaning as in printf and cout).
      * @param precision the number of digits to be printed after the decimal point.
      * @param width minimum number of characters to be printed. If the value to be printed is shorter than this number, the result is padded with blank spaces. The value is never truncated.
      * @param endRowStr string appended at the end of every row, except for the last one.
      * If width is specified the inter-value separator is a blank space, otherwise it is a tab.
      * Warning: the string format might change in the future. This method
      * is here to ease debugging.
      * @return a const string which contain the matrix in text form.
      */
      std::string toString(int precision=-1, int width=-1, const char* endRowStr="\n") const;

      /**
      * Return a pointer to the first element
      * @return the pointer to the first element (or NULL if either dimension of the matrix is 0)
      */
      inline double *data()
      {return (nrows>0&&ncols>0)?storage:0/*NULL*/;}

      /**
      * Return a pointer to the first element (const version).
      * @return the (const) pointer to the first element (or NULL if either dimension of the matrix is 0)
      */
      inline const double *data() const
      {return (nrows>0&&ncols>0)?storage:0/*NULL*/;}

      /**
      * True iff all elements of a match all element of b.
      */
      bool operator==(const yarp::sig::Matrix &r) const;

      ///////// Serialization methods
      /*
      * Read vector from a connection.
      * return true iff a vector was read correctly
      */
      bool read(yarp::os::ConnectionReader& connection) override;

      /**
      * Write vector to a connection.
      * return true iff a vector was written correctly
      */
      bool write(yarp::os::ConnectionWriter& connection) const override;

};

#endif // YARP_SIG_MATRIX_H
