#include <iostream>
#include <vector>
#include <string>
#include <complex>

class BigInteger {
  public:
    BigInteger();
    BigInteger(int x);
    BigInteger operator-() const;

    BigInteger& operator+=(const BigInteger& x);
    BigInteger& operator-=(const BigInteger& x);
    BigInteger& operator*=(const BigInteger& x);
    BigInteger& operator/=(const BigInteger& x);
    BigInteger& operator%=(const BigInteger& x);

    BigInteger& operator++();
    BigInteger operator++(int);
    BigInteger& operator--();
    BigInteger operator--(int);

    std::string toString() const;
    explicit operator bool();

  private:
    inline static const size_t DIGIT_LOG = 4;
    inline static const unsigned int MAX_DIGIT = 10'000;

    std::vector<unsigned int> value;
    bool negative;

    using base = std::complex<double>;
    inline static const double PI = acos(-1);

    inline static const size_t MAX_SMALL_NUMBER_LOG = 10;

    BigInteger& unsigned_add(const BigInteger& x);
    BigInteger& unsigned_subtract(const BigInteger& x);
    bool unsigned_less_or_equal(const BigInteger& x) const;
    void normalize();
    void fourier_transform(std::vector<base>& c, bool invert, const std::vector<base>& ws, size_t log);

    friend bool operator==(const BigInteger& a, const BigInteger& b);
    friend bool operator<=(const BigInteger& a, const BigInteger& b);
    friend std::istream& operator>>(std::istream& in, BigInteger& s);
};

BigInteger operator+(const BigInteger& a, const BigInteger& b);
BigInteger operator-(const BigInteger& a, const BigInteger& b);
BigInteger operator*(const BigInteger& a, const BigInteger& b);
BigInteger operator/(const BigInteger& a, const BigInteger& b);
BigInteger operator%(const BigInteger& a, const BigInteger& b);

bool operator==(const BigInteger& a, const BigInteger& b);
bool operator!=(const BigInteger& a, const BigInteger& b);
bool operator<=(const BigInteger& a, const BigInteger& b);
bool operator>=(const BigInteger& a, const BigInteger& b);
bool operator<(const BigInteger& a, const BigInteger& b);
bool operator>(const BigInteger& a, const BigInteger& b);

std::ostream& operator<<(std::ostream& out, const BigInteger& x);
std::istream& operator>>(std::istream& in, BigInteger& x);
