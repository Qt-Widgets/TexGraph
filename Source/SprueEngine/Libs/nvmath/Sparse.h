// This code is in the public domain -- castanyo@yahoo.es

#pragma once
#ifndef NV_MATH_SPARSE_H
#define NV_MATH_SPARSE_H

#include <stdint.h>
#include <vector>

// Full and sparse vector and matrix classes. BLAS subset.

namespace nv
{
    class FullVector;
    class FullMatrix;
    class SparseMatrix;


    /// Fixed size vector class.
    class FullVector
    {
    public:

        FullVector(uint32_t dim);
        FullVector(const FullVector & v);

        const FullVector & operator=(const FullVector & v);

        uint32_t dimension() const { return m_array.size(); }

        const float & operator[](uint32_t index) const { return m_array[index]; }
        float & operator[] (uint32_t index) { return m_array[index]; }

        void fill(float f);

        void operator+= (const FullVector & v);
        void operator-= (const FullVector & v);
        void operator*= (const FullVector & v);

        void operator+= (float f);
        void operator-= (float f);
        void operator*= (float f);


    private:
        std::vector<float> m_array;
    };

    // Pseudo-BLAS interface.
    void saxpy(float a, const FullVector & x, FullVector & y); // y = a * x + y
    void copy(const FullVector & x, FullVector & y);
    void scal(float a, FullVector & x);
    float dot(const FullVector & x, const FullVector & y);


    enum Transpose
    {
        NoTransposed = 0,
        Transposed = 1
    };

    /// Full matrix class.
    class FullMatrix
    {
    public:

        FullMatrix(uint32_t d);
        FullMatrix(uint32_t w, uint32_t h);
        FullMatrix(const FullMatrix & m);

        const FullMatrix & operator=(const FullMatrix & m);

        uint32_t width() const { return m_width; }
        uint32_t height() const { return m_height; }
        bool isSquare() const { return m_width == m_height; }

        float getCoefficient(uint32_t x, uint32_t y) const;

        void setCoefficient(uint32_t x, uint32_t y, float f);
        void addCoefficient(uint32_t x, uint32_t y, float f);
        void mulCoefficient(uint32_t x, uint32_t y, float f);

        float dotRow(uint32_t y, const FullVector & v) const;
        void madRow(uint32_t y, float alpha, FullVector & v) const;

    protected:

        bool isValid() const {
            return m_array.size() == (m_width * m_height);
        }

    private:
        const uint32_t m_width;
        const uint32_t m_height;
        std::vector<float> m_array;
    };

    void mult(const FullMatrix & M, const FullVector & x, FullVector & y);
    void mult(Transpose TM, const FullMatrix & M, const FullVector & x, FullVector & y);

    // y = alpha*A*x + beta*y
    void sgemv(float alpha, const FullMatrix & A, const FullVector & x, float beta, FullVector & y);
    void sgemv(float alpha, Transpose TA, const FullMatrix & A, const FullVector & x, float beta, FullVector & y);

    void mult(const FullMatrix & A, const FullMatrix & B, FullMatrix & C);
    void mult(Transpose TA, const FullMatrix & A, Transpose TB, const FullMatrix & B, FullMatrix & C);

    // C = alpha*A*B + beta*C
    void sgemm(float alpha, const FullMatrix & A, const FullMatrix & B, float beta, FullMatrix & C);
    void sgemm(float alpha, Transpose TA, const FullMatrix & A, Transpose TB, const FullMatrix & B, float beta, FullMatrix & C);


    /**
    * Sparse matrix class. The matrix is assumed to be sparse and to have
    * very few non-zero elements, for this reason it's stored in indexed 
    * format. To multiply column vectors efficiently, the matrix stores 
    * the elements in indexed-column order, there is a list of indexed 
    * elements for each row of the matrix. As with the FullVector the 
    * dimension of the matrix is constant.
    **/
    class SparseMatrix
    {
        friend class FullMatrix;
    public:

        // An element of the sparse array.
        struct Coefficient {
            uint32_t x;  // column
            float v; // value
        };


    public:

        SparseMatrix(uint32_t d);
        SparseMatrix(uint32_t w, uint32_t h);
        SparseMatrix(const SparseMatrix & m);

        const SparseMatrix & operator=(const SparseMatrix & m);


        uint32_t width() const { return m_width; }
        uint32_t height() const { return m_array.size(); }
        bool isSquare() const { return width() == height(); }

        float getCoefficient(uint32_t x, uint32_t y) const; // x is column, y is row

        void setCoefficient(uint32_t x, uint32_t y, float f);
        void addCoefficient(uint32_t x, uint32_t y, float f);
        void mulCoefficient(uint32_t x, uint32_t y, float f);

        float sumRow(uint32_t y) const;
        float dotRow(uint32_t y, const FullVector & v) const;
        void madRow(uint32_t y, float alpha, FullVector & v) const;

        void clearRow(uint32_t y);
        void scaleRow(uint32_t y, float f);
        void normalizeRow(uint32_t y);

        void clearColumn(uint32_t x);
        void scaleColumn(uint32_t x, float f);

        const std::vector<Coefficient> & getRow(uint32_t y) const;

        bool isSymmetric() const;

    private:

        /// Number of columns.
        const uint32_t m_width;

        /// Array of matrix elements.
        std::vector< std::vector<Coefficient> > m_array;

    };

    void transpose(const SparseMatrix & A, SparseMatrix & B);

    void mult(const SparseMatrix & M, const FullVector & x, FullVector & y);
    void mult(Transpose TM, const SparseMatrix & M, const FullVector & x, FullVector & y);

    // y = alpha*A*x + beta*y
    void sgemv(float alpha, const SparseMatrix & A, const FullVector & x, float beta, FullVector & y);
    void sgemv(float alpha, Transpose TA, const SparseMatrix & A, const FullVector & x, float beta, FullVector & y);

    void mult(const SparseMatrix & A, const SparseMatrix & B, SparseMatrix & C);
    void mult(Transpose TA, const SparseMatrix & A, Transpose TB, const SparseMatrix & B, SparseMatrix & C);

    // C = alpha*A*B + beta*C
    void sgemm(float alpha, const SparseMatrix & A, const SparseMatrix & B, float beta, SparseMatrix & C);
    void sgemm(float alpha, Transpose TA, const SparseMatrix & A, Transpose TB, const SparseMatrix & B, float beta, SparseMatrix & C);

    // C = At * A
    void sqm(const SparseMatrix & A, SparseMatrix & C);

} // nv namespace


#endif // NV_MATH_SPARSE_H
