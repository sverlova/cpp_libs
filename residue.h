#include <iostream>
#include <stdio.h>
#include <vector>

template<unsigned N>
class Residue {
  public:
    Residue();
    explicit Residue(int x);

    explicit operator int() const;

    Residue& operator+=(const Residue& x);
    Residue& operator-=(const Residue& x);
    Residue& operator*=(const Residue& x);
    Residue& operator/=(const Residue& x);

    Residue pow(unsigned k) const;
    Residue getInverse() const;
    unsigned order() const;
    static Residue getPrimitiveRoot();

  private:
    unsigned value;
    static unsigned gcd(unsigned a, unsigned b);
    static unsigned euler_function();
};

template<unsigned N>
Residue<N> operator+(const Residue<N>& x, const Residue<N>& y);
template<unsigned N>
Residue<N> operator-(const Residue<N>& x, const Residue<N>& y);
template<unsigned N>
Residue<N> operator*(const Residue<N>& x, const Residue<N>& y);
template<unsigned N>
Residue<N> operator/(const Residue<N>& x, const Residue<N>& y);

template<unsigned N>
std::ostream& operator<<(std::ostream& out, const Residue<N>& x);
