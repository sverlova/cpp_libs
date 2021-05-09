#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <complex>

template<typename Field, bool Const>
class _MatrixHelper;

template<unsigned M, unsigned N, typename Field=double>
class Matrix {
  public:
    Matrix();
    Matrix(const std::initializer_list<std::initializer_list<Field>>& v);
    Matrix(const Matrix<M, N, Field>& A);

    Matrix<M, N, Field>& operator=(const Matrix<M, N, Field>& A);
    Matrix<M, N, Field>& operator+=(const Matrix<M, N, Field>& A);
    Matrix<M, N, Field>& operator-=(const Matrix<M, N, Field>& A);
    Matrix<M, N, Field>& operator*=(const Matrix<M, N, Field>& A);

    Field det() const;
    Matrix<N, M, Field> transposed() const;
    unsigned rank() const;
    Matrix<M, N, Field> inverted() const;
    Field trace() const;

    std::vector<Field> getRow(unsigned number) const;
    std::vector<Field> getColumn(unsigned number) const;
    _MatrixHelper<Field, true> operator[](size_t i) const;
    _MatrixHelper<Field, false> operator[](size_t i);

    void invert();
    ~Matrix();

  protected:
    Field** value;
};

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> operator+(const Matrix<M, N, Field>& A, const Matrix<M, N, Field>& B);
template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> operator-(const Matrix<M, N, Field>& A, const Matrix<M, N, Field>& B);

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> operator*(Matrix<M, N, Field> m, const Field& coef);
template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> operator*(const Field& coef, Matrix<M, N, Field> m);
template<unsigned M, unsigned N, unsigned K, typename Field>
Matrix<M, K, Field> operator*(const Matrix<M, N, Field>& A, const Matrix<N, K, Field>& B);

template<unsigned M, unsigned N, typename F1, unsigned K, unsigned L, typename F2>
bool operator==(const Matrix<M, N, F1>& A, const Matrix<K, L, F2>& B);
template<unsigned M, unsigned N, typename F1, unsigned K, unsigned L, typename F2>
bool operator!=(const Matrix<M, N, F1>& A, const Matrix<K, L, F2>& B);

template<unsigned N, typename Field=double>
class SquareMatrix: public Matrix<N, N, Field> {
  public:
    SquareMatrix(): Matrix<N, N, Field>() {};
    SquareMatrix(const std::initializer_list<std::initializer_list<Field>>& v): Matrix<N, N, Field>(v) {};
};
