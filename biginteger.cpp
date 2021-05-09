#include "biginteger.h"

void BigInteger::normalize() {
    while (value.size() > 1 && value.back() == 0) {
        value.pop_back();
    }
    if (value.empty()) value.push_back(0);
    if (value.size() == 1 && value[0] == 0) negative = false;
}

BigInteger::BigInteger(): value(std::vector<unsigned int>(1, 0)), negative(false) {};

BigInteger::BigInteger(int x): value(std::vector<unsigned int>(1, 0)),
        negative(x < 0) {
    if (x < 0) x = -x;
    for (size_t i = 0; x > 0; ++i) {
        if (value.size() <= i) value.push_back(0);
        value[i] = static_cast<unsigned int>(x % MAX_DIGIT);
        x /= MAX_DIGIT;
    }
    normalize();
}

BigInteger& BigInteger::unsigned_add(const BigInteger& x) {
    long long sum = 0;
    for (size_t i = 0; ; ++i) {
        if (i < x.value.size())
            sum += x.value[i];
        if (i < value.size())
            sum += value[i];
        if (sum == 0 && i >= value.size() && i >= x.value.size()) break;
        if (value.size() <= i) value.push_back(0);
        value[i] = static_cast<unsigned int>(sum % MAX_DIGIT);
        sum /= MAX_DIGIT;
    }
    normalize();
    return *this;
}

BigInteger& BigInteger::unsigned_subtract(const BigInteger& x) {
    bool bigger = !unsigned_less_or_equal(x);

    const BigInteger* a = this;
    const BigInteger* b = &x;

    if (!bigger) {
        negative ^= true;
        std::swap(a, b);
    }

    long long sum = 0;
    for (size_t i = 0; i < a->value.size(); ++i) {
        sum += a->value[i];
        if (i < b->value.size()) {
            sum -= b->value[i];
        }
        if (sum == value[i] && a == this && i >= b->value.size()) {
            break;
        }
        long long next_sum = 0;
        if (sum < 0) {
            next_sum = (sum - MAX_DIGIT + 1) / MAX_DIGIT;
            sum += MAX_DIGIT;
        }
        if (value.size() <= i) value.push_back(0);
        value[i] = static_cast<unsigned int>(sum % MAX_DIGIT);
        sum = next_sum;
    }

    normalize();
    return *this;
}

BigInteger BigInteger::operator-() const {
    BigInteger negative_x = *this;
    negative_x.negative ^= true;
    negative_x.normalize();
    return negative_x;
}

BigInteger& BigInteger::operator+=(const BigInteger& x) {
    return (negative == x.negative) ? unsigned_add(x) : unsigned_subtract(x);
}

BigInteger& BigInteger::operator-=(const BigInteger& x) {
    return (negative == x.negative) ? unsigned_subtract(x) : unsigned_add(x);
}

void BigInteger::fourier_transform(std::vector<base>& c, bool invert,
        const std::vector<base>& ws, size_t log = 0) {
    size_t half_n = c.size() / 2;
    if (half_n < 1) return;

    std::vector<base> c1(half_n), c2(half_n);
    for (size_t i = 0; i < half_n; ++i) {
        c1[i] = c[2 * i];
        c2[i] = c[2 * i + 1];
    }

    fourier_transform(c1, invert, ws, log + 1);
    fourier_transform(c2, invert, ws, log + 1);

    base w1 = ws[log];
    if (invert) w1 = base(w1.real(), -w1.imag());
    base wi = 1;

    for (size_t i = 0; i < half_n; ++i) {
        c[i] = c1[i] + wi * c2[i];
        c[i + half_n] = c1[i] - wi * c2[i];

        wi *= w1;
    }
}

BigInteger& BigInteger::operator*=(const BigInteger& x) {
    if (std::min(value.size(), x.value.size()) <= MAX_SMALL_NUMBER_LOG) {
        const BigInteger* a = this;
        const BigInteger* b = &x;

        if (*this < x) std::swap(a, b);
        BigInteger result = 0;

        for (size_t i = 0; i < b->value.size(); ++i) {
            if (b->value[i] == 0) continue;
            BigInteger temp = 0;
            temp.value.resize(i, 0);
            long long sum = 0;
            for (size_t j = 0; ; ++j) {
                if (j < a->value.size()) {
                    sum += 1ll * a->value[j] * b->value[i];
                }
                if (sum == 0 && j >= a->value.size()) break;
                temp.value.push_back(static_cast<unsigned int>(sum % MAX_DIGIT));
                sum /= MAX_DIGIT;
            }
            temp.normalize();
            result += temp;
        }

        result.negative = a->negative ^ b->negative;
        result.normalize();
        *this = result;
        return *this;
    }

    size_t n = 1;
    std::vector<base> ws(1, 1);
    while (n < 2 * value.size() || n < 2 * x.value.size()) {
        n *= 2;
        double angel = 2 * PI / n;
        ws.push_back(base(cos(angel), sin(angel)));
    }
    for (size_t i = 0; i < ws.size() / 2; ++i) {
        std::swap(ws[i], ws[ws.size() - 1 - i]);
    }
    std::vector<base> c1(n, 0), c2(n, 0);
    for (size_t i = 0; i < value.size(); ++i) {
        c1[i] = value[i];
    }
    for (size_t i = 0; i < x.value.size(); ++i) {
        c2[i] = x.value[i];
    }

    fourier_transform(c1, false, ws);
    fourier_transform(c2, false, ws);

    for (size_t i = 0; i < n; ++i) {
        c1[i] *= c2[i];
    }

    fourier_transform(c1, true, ws);

    long long sum = 0;
    value.resize(n, 0);
    for (size_t i = 0; i < n; ++i) {
        c1[i] /= n;
        sum += static_cast<long long>(c1[i].real() + 0.5);
        value[i] = static_cast<unsigned int>(sum % MAX_DIGIT);
        sum /= MAX_DIGIT;
    }

    negative ^= x.negative;
    normalize();
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& x) {
    std::vector<unsigned int> result;
    BigInteger temp = 0;
    for (size_t i = value.size(); i > 0; ) {
        --i;
        temp = temp * MAX_DIGIT + value[i];
        unsigned int left = 0, right = MAX_DIGIT + 1;
        while (right - left > 1) {
            unsigned int middle = (left + right) / 2;
            if ((x * middle).unsigned_less_or_equal(temp)) {
                left = middle;
            } else {
                right = middle;
            }
        }
        result.push_back(left);
        temp.unsigned_subtract(left * x);
    }

    for (size_t i = 0; i < result.size() / 2; ++i) {
        std::swap(result[i], result[result.size() - 1 - i]);
    }
    value = result;
    negative ^= x.negative;
    normalize();
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& x) {
    return *this -= *this / x * x;
}

BigInteger operator+(const BigInteger& a, const BigInteger& b) {
    BigInteger result = a;
    result += b;
    return result;
}

BigInteger operator-(const  BigInteger& a, const BigInteger& b) {
    BigInteger result = a;
    result -= b;
    return result;
}

BigInteger operator*(const BigInteger& a, const BigInteger& b) {
    BigInteger result = a;
    result *= b;
    return result;
}

BigInteger operator/(const BigInteger& a, const BigInteger& b) {
    BigInteger result = a;
    result /= b;
    return result;
}

BigInteger operator%(const BigInteger& a, const BigInteger& b) {
    BigInteger result = a;
    result %= b;
    return result;
}

BigInteger& BigInteger::operator++() {
    return *this += 1;
}

BigInteger BigInteger::operator++(int) {
    BigInteger copy = *this;
    ++*this;
    return copy;
}

BigInteger& BigInteger::operator--() {
    return *this -= 1;
}

BigInteger BigInteger::operator--(int) {
    BigInteger copy = *this;
    --*this;
    return copy;
}

std::string BigInteger::toString() const {
    std::string number = "";
    for (size_t i = 0; i < value.size(); ++i) {
        unsigned int digit = value[i];
        for (size_t j = 0; j < DIGIT_LOG; ++j) {
            number += static_cast<char>(digit % 10 + 48);
            digit /= 10;
            if (i + 1 == value.size() && digit == 0) break;
        }
    }
    if (negative) number += '-';
    for (size_t i = 0; i < number.size() / 2; ++i) {
        std::swap(number[i], number[number.size() - i - 1]);
    }
    return number;
}

bool BigInteger::unsigned_less_or_equal(const BigInteger& x) const {
    if (value.size() != x.value.size()) {
        return value.size() < x.value.size();
    }
    for (size_t i = value.size(); i > 0; ) {
        --i;
        if (value[i] < x.value[i]) return true;
        if (value[i] > x.value[i]) return false;
    }
    return true;
}

bool operator==(const BigInteger& a, const BigInteger& b) {
    if (a.value.size() != b.value.size() || a.negative != b.negative)
        return false;
    for (size_t i = 0; i < a.value.size(); ++i) {
        if (a.value[i] != b.value[i]) return false;
    }
    return true;
}

bool operator!=(const BigInteger& a, const BigInteger& b) {
    return !(a == b);
}

bool operator<=(const BigInteger& a, const BigInteger& b) {
    if (a.negative != b.negative) return a.negative;
    return a.negative ? b.unsigned_less_or_equal(a) : a.unsigned_less_or_equal(b);
}

bool operator>=(const BigInteger& a, const BigInteger& b) {
    return b <= a;
}

bool operator<(const BigInteger& a, const BigInteger& b) {
    return a <= b && a != b;
}

bool operator>(const BigInteger& a, const BigInteger& b) {
    return b < a;
}

BigInteger::operator bool() {
    return *this != BigInteger(0);
}

std::ostream& operator<<(std::ostream& out, const BigInteger& x) {
    out << x.toString();
    return out;
}

std::istream& operator>>(std::istream& in, BigInteger& x) {
    std::string s;
    in >> s;
    x = 0;
    x.negative = s.size() > 0 && s[0] == '-';
    for (size_t i = 0; i < s.size() / 2; ++i) {
        std::swap(s[i], s[s.size() - 1 - i]);
    }
    while (s.size() > 1 && s.back() == '0') {
        s.pop_back();
    }
    x.value = std::vector<unsigned int>(0);
    unsigned int power = 1;
    for (size_t i = 0; i < s.size() && s[i] != '-'; ++i) {
        if (i % BigInteger::DIGIT_LOG == 0) {
            power = 1;
            x.value.push_back(0);
        } else {
            power *= 10;
        }
        x.value.back() += power * (static_cast<int>(s[i]) - 48);
    }
    x.normalize();
    return in;
}

class Rational {
  public:
    Rational();
    Rational(int x);
    Rational(const BigInteger& x);

    Rational operator-() const;
    Rational& operator+=(const Rational& x);
    Rational& operator-=(const Rational& x);
    Rational& operator*=(const Rational& x);
    Rational& operator/=(const Rational& x);

    std::string toString() const;
    std::string asDecimal(size_t precision) const;
    explicit operator double();

  private:
    BigInteger numerator;
    BigInteger denominator;

    void normalize();

    friend bool operator==(const Rational& a, const Rational& b);
    friend bool operator<=(const Rational& a, const Rational& b);
};

Rational operator+(const Rational& a, const Rational& b);
Rational operator-(const Rational& a, const Rational& b);
Rational operator*(const Rational& a, const Rational& b);
Rational operator/(const Rational& a, const Rational& b);

bool operator==(const Rational& a, const Rational& b);
bool operator!=(const Rational& a, const Rational& b);
bool operator<=(const Rational& a, const Rational& b);
bool operator>=(const Rational& a, const Rational& b);
bool operator<(const Rational& a, const Rational& b);
bool operator>(const Rational& a, const Rational& b);

std::ostream& operator<<(std::ostream& out, const Rational& x);

Rational::Rational(): numerator(0), denominator(1) {};
Rational::Rational(int x): numerator(x), denominator(1) {};
Rational::Rational(const BigInteger& x): numerator(x), denominator(1) {};

void Rational::normalize() {
    BigInteger a = numerator;
    BigInteger b = denominator;
    if (a < 0) a *= -1;
    if (b < 0) b *= -1;
    while (b) {
        a %= b;
        std::swap(a, b);
    }
    if (denominator < 0) a *= -1;
    numerator /= a;
    denominator /= a;
}

Rational Rational::operator-() const {
    Rational x = *this;
    x.numerator *= -1;
    x.normalize();
    return x;
}

Rational& Rational::operator+=(const Rational& x) {
    numerator = numerator * x.denominator + x.numerator * denominator;
    denominator *= x.denominator;
    normalize();
    return *this;
}

Rational& Rational::operator-=(const Rational& x) {
    numerator = numerator * x.denominator - x.numerator * denominator;
    denominator *= x.denominator;
    normalize();
    return *this;
}

Rational& Rational::operator*=(const Rational& x) {
    numerator *= x.numerator;
    denominator *= x.denominator;
    normalize();
    return *this;
}

Rational& Rational::operator/=(const Rational& x) {
    numerator *= x.denominator;
    denominator *= x.numerator;
    normalize();
    return *this;
}

Rational operator+(const Rational& a, const Rational& b) {
    Rational result = a;
    result += b;
    return result;
}

Rational operator-(const Rational& a, const Rational& b) {
    Rational result = a;
    result -= b;
    return result;
}

Rational operator*(const Rational& a, const Rational& b) {
    Rational result = a;
    result *= b;
    return result;
}

Rational operator/(const Rational& a, const Rational& b) {
    Rational result = a;
    result /= b;
    return result;
}

bool operator==(const Rational& a, const Rational& b) {
    return a.numerator * b.denominator == b.numerator * a.denominator;
}

bool operator!=(const Rational& a, const Rational& b) {
    return !(a == b);
}

bool operator<=(const Rational& a, const Rational& b) {
    return a.numerator * b.denominator <= b.numerator * a.denominator;
}

bool operator>=(const Rational& a, const Rational& b) {
    return b <= a;
}

bool operator<(const Rational& a, const Rational& b) {
    return a <= b && a != b;
}

bool operator>(const Rational& a, const Rational& b) {
    return b < a;
}

std::string Rational::toString() const {
    std::string result = numerator.toString();
    if (denominator != 1) result += '/' + denominator.toString();
    return result;
}

std::string Rational::asDecimal(size_t precision = 0) const {
    BigInteger power = 1;
    for (size_t i = 0; i < precision; ++i) {
        power *= 10;
    }
    bool negative = numerator < 0;
    if (negative) power *= -1;
    std::string division = (numerator * power / denominator).toString();
    std::string result = "";
    for (size_t i = 0; i < precision + 1 || i < division.size(); ++i) {
        result += i < division.size() ? division[division.size() - 1 - i] : '0';
        if (i + 1 == precision) result += '.';
    }
    if (negative) result += '-';
    for (size_t i = 0; i < result.size() / 2; ++i) {
        std::swap(result[i], result[result.size() - 1 - i]);
    }
    return result;
}

Rational::operator double() {
    const size_t precision = 300;
    std::string number = asDecimal(precision);
    double result = 0;
    double power = 1;
    for (size_t i = 0; i + precision + 1 < number.size(); ++i) {
        result += power * (static_cast<unsigned int>(number[number.size() - 2 - precision - i]) - 48);
        power *= 10;
    }
    power = 1;
    for (size_t i = 0; i < precision; ++i) {
        power /= 10;
        result += power * (static_cast<unsigned int>(number[number.size() - precision + i]) - 48);
    }
    return result;
}

std::ostream& operator<<(std::ostream& out, const Rational& x) {
    out << x.toString();
    return out;
}
