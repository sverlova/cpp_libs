#include "residue.h"

template<unsigned N, unsigned D, bool F>
struct is_prime_helper {
    static const bool value = N % D == 0 ? false : is_prime_helper<N, D + 1, (N / (D + 1) >= (D + 1))>::value;
};

template<unsigned N, unsigned D>
struct is_prime_helper<N, D, false> {
    static const bool value = true;
};

template<unsigned N>
struct is_prime {
    static const bool value = N == 2 || (N != 1 &&
            is_prime_helper<N, 2, true>::value);
};

template<unsigned N>
const bool is_prime_v = is_prime<N>::value;

// *****

template<unsigned N, unsigned D, unsigned R>
struct check_pow {
    static const bool value = false;
};

template<unsigned N, unsigned D>
struct check_pow<N, D, 0> {
    static const bool value = check_pow<N / D, D, N % D>::value;
};

template<unsigned D>
struct check_pow<1, D, 0> {
    static const bool value = true;
};

// *****

template<unsigned N, unsigned D, unsigned M, bool F>
struct prime_pow_helper {};

template<unsigned N, unsigned D, unsigned M>
struct prime_pow_helper<N, D, M, true> {
    static const bool value = prime_pow_helper<N, D + 2,
            N % (D + 2), D + 2 <= N / (D + 2)>::value;
};

template<unsigned N, unsigned D, unsigned M>
struct prime_pow_helper<N, D, M, false> {
    static const bool value = true;
};

template<unsigned N, unsigned D>
struct prime_pow_helper<N, D, 0, true> {
    static const bool value = check_pow<N, D, 0>::value;
};

// *****

template<unsigned N>
struct prime_pow {
    static const bool value = prime_pow_helper<N, 1, 1, true>::value;
};

template<unsigned N>
struct has_primitive_root {
    static const bool value = N == 2 || N == 4 ||
            (N % 2 != 0 && prime_pow<N>::value) ||
            (N % 2 == 0 && N / 2 % 2 != 0 &&
            prime_pow<N / 2>::value);
};

template<unsigned N>
const bool has_primitive_root_v = has_primitive_root<N>::value;

template<bool F>
struct compile_error {};

template<>
struct compile_error<false> {
    static const bool happen = false;
};

// *****

template<unsigned N>
Residue<N>::Residue(): value(0) {}

template<unsigned N>
Residue<N>::Residue(int x): value((static_cast<long long>(x) % N + N) % N) {}

template<unsigned N>
Residue<N>::operator int() const {
    return value;
}

template<unsigned N>
Residue<N>& Residue<N>::operator+=(const Residue<N>& x) {
    value = (static_cast<unsigned long long>(value) + x.value) % N;
    return *this;
}

template<unsigned N>
Residue<N>& Residue<N>::operator-=(const Residue<N>& x) {
    value = (static_cast<unsigned long long>(value) + N - x.value) % N;
    return *this;
}

template<unsigned N>
Residue<N>& Residue<N>::operator*=(const Residue<N>& x) {
    value = (static_cast<unsigned long long>(value) * x.value) % N;
    return *this;
}

template<unsigned N>
Residue<N>& Residue<N>::operator/=(const Residue<N>& x) {
    return *this *= x.getInverse();
}

template<unsigned N>
Residue<N> operator+(const Residue<N>& x, const Residue<N>& y) {
    Residue<N> result = x;
    result += y;
    return result;
}

template<unsigned N>
Residue<N> operator-(const Residue<N>& x, const Residue<N>& y) {
    Residue<N> result = x;
    result -= y;
    return result;
}

template<unsigned N>
Residue<N> operator*(const Residue<N>& x, const Residue<N>& y) {
    Residue<N> result = x;
    result *= y;
    return result;
}

template<unsigned N>
Residue<N> operator/(const Residue<N>& x, const Residue<N>& y) {
    Residue<N> result = x;
    result /= y;
    return result;
}

template<unsigned N>
Residue<N> Residue<N>::pow(unsigned k) const {
    Residue<N> result(1);
    Residue<N> power = *this;
    while (k > 0) {
        if (k % 2 == 1) {
            result *= power;
        }
        power *= power;
        k /= 2;
    }
    return result;
}

template<unsigned N>
Residue<N> Residue<N>::getInverse() const {
    if (!compile_error<!is_prime_v<N>>::happen) {
        return pow(N - 2);
    }
}

template<unsigned N>
unsigned Residue<N>::order() const {
    if (gcd(N, value) != 1) return 0;
    unsigned phi = euler_function();
    unsigned answer = phi;
    for (unsigned divider = 1; divider * divider <= phi; ++divider) {
        if (phi % divider != 0) continue;
        if (int(pow(divider)) == 1) {
            return divider;
        }
        if (int(pow(phi / divider)) == 1) {
            answer = phi / divider;
        }
    }
    return answer;
}

template<unsigned N>
Residue<N> Residue<N>::getPrimitiveRoot() {
    if (compile_error<!has_primitive_root_v<N>>::happen) {
        return Residue<N>(0);
    }
    std::vector<unsigned> factorization;
    unsigned phi = euler_function();
    unsigned phi_copy = phi;
    for (unsigned divider = 2; divider * divider <= phi_copy; ++divider) {
        if (phi_copy % divider == 0) {
            factorization.push_back(divider);
            while (phi_copy % divider == 0) {
                phi_copy /= divider;
            }
        }
    }
    if (phi_copy > 1) factorization.push_back(phi_copy);
    for (unsigned result = 2; result <= N; ++result) {
        if (gcd(result, N) != 1) continue;
        bool ok = true;
        Residue<N> residue_result(result);
        for (size_t i = 0; i < factorization.size(); ++i) {
            if (int(residue_result.pow(phi / factorization[i])) == 1) {
                ok = false;
                break;
            }
        }
        if (ok) return residue_result;
    }
    return Residue<N>(0);
}

template<unsigned N>
unsigned Residue<N>::gcd(unsigned a, unsigned b) {
    while (b != 0) {
        a %= b;
        std::swap(a, b);
    }
    return a;
}

template<unsigned N>
unsigned Residue<N>::euler_function() {
    unsigned result = N;
    unsigned n = N;
    for (unsigned divider = 2; divider * divider <= n; ++divider) {
        if (n % divider == 0) {
            while (n % divider == 0) {
                n /= divider;
            }
            result -= result / divider;
        }
    }
    if (n > 1) result -= result / n;
    return result;
}

template<unsigned N>
std::ostream& operator<<(std::ostream& out, const Residue<N>& x) {
    out << int(x);
    return out;
}
