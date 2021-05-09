#include "matrix.h"

template<bool F>
struct compile_error {};

template<>
struct compile_error<false> {
    static const bool happen = false;
};

template<typename Field, bool Const>
class _MatrixHelper {};

template<typename Field>
class _MatrixHelper<Field, true> {
  private:
    const Field* value;
  public:
    _MatrixHelper(const Field* value): value(value) {};
    Field operator[](size_t i) const {
        return value[i];
    }
};

template<typename Field>
class _MatrixHelper<Field, false> {
  private:
    Field* value;
  public:
    _MatrixHelper(Field* value): value(value) {};
    Field& operator[](size_t i) {
        return value[i];
    }
};

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field>::Matrix() {
    value = new Field*[M];
    for (size_t i = 0; i < M; ++i) {
        value[i] = new Field[N];
        for (size_t j = 0; j < N; ++j) {
            value[i][j] = Field(0);
        }
    }
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field>::Matrix(const std::initializer_list<std::initializer_list<Field>>& v) {
    value = new Field*[M];
    auto it = v.begin();
    for (size_t i = 0; i < M; ++i) {
        value[i] = new Field[N];
        auto it2 = it->begin();
        for (size_t j = 0; j < N; ++j) {
            value[i][j] = *it2;
            ++it2;
        }
        ++it;
    }
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field>::Matrix(const Matrix<M, N, Field>& A) {
    value = new Field*[M];
    for (size_t i = 0; i < M; ++i) {
        value[i] = new Field[N];
        for (size_t j = 0; j < N; ++j) {
            value[i][j] = A[i][j];
        }
    }
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field>::~Matrix() {
    for (size_t i = 0; i < M; ++i) {
        delete[] value[i];
    }
    delete[] value;
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator=(const Matrix<M, N, Field>& A) {
    Matrix<M, N, Field> result = A;
    std::swap(result.value, value);
    return *this;
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator+=(const Matrix<M, N, Field>& A) {
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            value[i][j] += A.value[i][j];
        }
    }
    return *this;
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator-=(const Matrix<M, N, Field>& A) {
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            value[i][j] -= A.value[i][j];
        }
    }
    return *this;
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator*=(const Matrix<M, N, Field>& A) {
    if (compile_error<M != N>::happen) {
        return *this;
    }
    return (*this = *this * A);
}

template<unsigned M, unsigned N, typename Field>
Field Matrix<M, N, Field>::det() const {
    if (M != N) return Field(0);
    Field result = Field(1);
    unsigned* was = new unsigned[N];
    for (size_t i = 0; i < N; ++i) {
        was[i] = false;
    }
    unsigned inversions = 0;
    Matrix<M, N, Field> copy = *this;
    for (size_t line = 0; line < N; ++line) {
        size_t boss = 0;
        while (boss < N && copy.value[line][boss] == Field(0)) ++boss;
        if (boss == N) {
            result = Field(0);
            break;
        }
        for (size_t i = boss + 1; i < N; ++i) {
            inversions += was[i];
        }
        was[boss] = true;
        result *= copy.value[line][boss];
        for (size_t i = 0; i < N; ++i) {
            if (i == line) continue;
            Field coef = copy.value[i][boss] / copy.value[line][boss];
            for (size_t j = 0; j < N; ++j) {
                copy.value[i][j] -= copy.value[line][j] * coef;
            }
        }
    }
    if (inversions % 2 == 1) {
        result *= Field(-1);
    }
    return result;
}

template<unsigned M, unsigned N, typename Field>
Matrix<N, M, Field> Matrix<M, N, Field>::transposed() const {
    Matrix<N, M, Field> result;
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            result[j][i] = value[i][j];
        }
    }
    return result;
}

template<unsigned M, unsigned N, typename Field>
unsigned Matrix<M, N, Field>::rank() const {
    Matrix<M, N, Field> copy = *this;
    unsigned result = M;
    for (size_t line = 0; line < M; ++line) {
        size_t boss = 0;
        while (boss < N && copy.value[line][boss] == Field(0)) ++boss;
        if (boss == N) {
            --result;
            continue;
        }
        for (size_t i = 0; i < M; ++i) {
            if (i == line) continue;
            Field coef = copy.value[i][boss] / copy.value[line][boss];
            for (size_t j = 0; j < N; ++j) {
                copy.value[i][j] -= copy.value[line][j] * coef;
            }
        }
    }
    return result;
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::inverted() const {
    if (compile_error<M != N>::happen) {
        return *this;
    }

    Matrix<M, N, Field> copy = *this;
    Matrix<M, N, Field> one;
    for (size_t i = 0; i < N; ++i) {
        one[i][i] = Field(1);
    }

    for (size_t line = 0; line < N; ++line) {
        size_t new_boss = line;
        while (copy.value[new_boss][line] == Field(0)) ++new_boss;
        for (size_t i = 0; i < N; ++i) {
            std::swap(copy.value[new_boss][i], copy.value[line][i]);
        }
        Field val = copy.value[line][line];

        for (size_t j = 0; j < N; ++j) {
            copy.value[line][j] /= val;
            one.value[line][j] /= val;
        }

        for (size_t i = 0; i < N; ++i) {
            if (i == line) continue;
            Field coef = copy.value[i][line];
            for (size_t j = 0; j < N; ++j) {
                copy.value[i][j] -= copy.value[line][j] * coef;
                one.value[i][j] -= one.value[line][j] * coef;
            }

        }
    }

    return one;
}

template<unsigned M, unsigned N, typename Field>
Field Matrix<M, N, Field>::trace() const {
    if (compile_error<M != N>::happen) {
        return Field();
    }

    Field sum(0);
    for (size_t i = 0; i < N; ++i) {
        sum += value[i][i];
    }
    return sum;
}

template<unsigned M, unsigned N, typename Field>
std::vector<Field> Matrix<M, N, Field>::getRow(unsigned number) const {
    std::vector<Field> row(N);
    for (size_t i = 0; i < N; ++i) {
        row[i] = value[number][i];
    }
    return row;
}

template<unsigned M, unsigned N, typename Field>
std::vector<Field> Matrix<M, N, Field>::getColumn(unsigned number) const {
    std::vector<Field> column(M);
    for (size_t i = 0; i < M; ++i) {
        column[i] = value[i][number];
    }
    return column;
}

template<unsigned M, unsigned N, typename Field>
void Matrix<M, N, Field>::invert() {
    if (compile_error<M != N>::happen) {
        return;
    }
    *this = inverted();
}

template<unsigned M, unsigned N, typename Field>
_MatrixHelper<Field, true> Matrix<M, N, Field>::operator[](size_t i) const {
    return _MatrixHelper<Field, true>(value[i]);
}

template<unsigned M, unsigned N, typename Field>
_MatrixHelper<Field, false> Matrix<M, N, Field>::operator[](size_t i) {
    return _MatrixHelper<Field, false>(value[i]);
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> operator+(const Matrix<M, N, Field>& A, const   Matrix<M, N, Field>& B) {
    Matrix<M, N, Field> result = A;
    result += B;
    return result;
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> operator-(const Matrix<M, N, Field>& A, const   Matrix<M, N, Field>& B) {
    Matrix<M, N, Field> result = A;
    result -= B;
    return result;
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> operator*(Matrix<M, N, Field> m, const Field& coef) {
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            m[i][j] *= coef;
        }
    }
    return m;
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> operator*(const Field& coef, Matrix<M, N, Field> m) {
    return m * coef;
}

template<unsigned M, unsigned N, unsigned K, typename Field>
Matrix<M, K, Field> operator*(const Matrix<M, N, Field>& A, const Matrix<N, K, Field>& B) {
    Matrix<M, K, Field> result;
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < K; ++j) {
            for (size_t g = 0; g < N; ++g) {
                result[i][j] += A[i][g] * B[g][j];
            }
        }
    }
    return result;
}

template<unsigned M, unsigned N, typename F1, unsigned K, unsigned L, typename F2>
bool operator==(const Matrix<M, N, F1>& A, const Matrix<K, L, F2>& B) {
    if (M != K || N != L || typeid(F1).name() != typeid(F2).name()) return false;
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            if (A[i][j] != B[i][j]) return false;
        }
    }
    return true;
}

template<unsigned M, unsigned N, typename F1, unsigned K, unsigned L, typename F2>
bool operator!=(const Matrix<M, N, F1>& A, const Matrix<K, L, F2>& B) {
    return !(A == B);
}
