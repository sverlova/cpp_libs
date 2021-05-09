#include "geometry.h"

namespace _tools {
    bool equal_double(const double& a, const double& b) {
        return fabs(a - b) < eps;
    }

    double sqr(const double& x) {
        return x * x;
    }
}

// Point

Point::Point(): x(0), y(0) {}

Point::Point(double x, double y): x(x), y(y) {
}

std::ostream& operator<<(std::ostream& out, const Point& p) {
    out << p.x << ' ' << p.y << '\n';
    return out;
}

Point Point::reflexed(const Line& axis) const {
    Vector2D orthodoxal = axis.get_vector().normal();
    Point intersection = line_intersection(axis,
            Line(*this, *this + orthodoxal));
    return intersection * 2 - *this;
}

bool operator==(const Point& A, const Point& B) {
    return _tools::equal_double(A.x, B.x) && _tools::equal_double(A.y, B.y);
}

bool operator!=(const Point& A, const Point& B) {
    return !(A == B);
}

double distance_between_points(const Point& A, const Point& B) {
    return sqrt(_tools::sqr(A.x - B.x) + _tools::sqr(A.y - B.y));
}

// Vector2D

Vector2D::Vector2D(const Point& A): Point(A) {}

Vector2D::Vector2D(double x, double y): Point(x, y) {}

Vector2D& Vector2D::operator+=(const Vector2D& A) {
    x += A.x;
    y += A.y;
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& A) {
    x -= A.x;
    y -= A.y;
    return *this;
}

Vector2D& Vector2D::operator*=(const double& k) {
    x *= k;
    y *= k;
    return *this;
}

Vector2D& Vector2D::operator/=(const double& k) {
    x /= k;
    y /= k;
    return *this;
}

double Vector2D::length() const {
    return distance_between_points(Point(0, 0), *this);
}

Vector2D Vector2D::normal() const {
    return Vector2D(-y, x);
}

Vector2D Vector2D::rotated(double angle) const {
    angle *= PI / 180;
    double cos = ::cos(angle);
    double sin = ::sin(angle);
    return Vector2D(x * cos - y * sin, x * sin + y * cos);
}

Vector2D Vector2D::reflexed(const Line& axis) const {
    Point on_line = Point(0, axis.get_y(0));
    return Vector2D(Point(on_line + *this).reflexed(axis) - on_line);
}


Vector2D operator+(const Vector2D& A, const Vector2D& B) {
    Vector2D C = A;
    C += B;
    return C;
}

Vector2D operator-(const Vector2D& A, const Vector2D& B) {
    Vector2D C = A;
    C -= B;
    return C;
}

Vector2D operator*(const Vector2D& A, const double& k) {
    Vector2D C = A;
    C *= k;
    return C;
}

Vector2D operator/(const Vector2D& A, const double& k) {
    Vector2D C = A;
    C /= k;
    return C;
}

double operator*(const Vector2D& a, const Vector2D& b) {
    return a.x * b.y - a.y * b.x;
}

// Line

Line::Line(const Point& A, const Point& B): A(A.y - B.y),
        B(B.x - A.x), C(A.x * B.y - A.y * B.x) {}

Line::Line(double k, double b): A(k), B(-1), C(b) {}

Line::Line(const Point& P, double k): A(k), B(-1), C(P.y - k * P.x) {}

double Line::get_y(const double x) const {
    return _tools::equal_double(B, 0) ? 0 : -(A * x + C) / B;
}

Vector2D Line::get_vector() const {
    return Vector2D(-B, A);
}

bool operator==(const Line& a, const Line& b) {
    return _tools::equal_double(a.A * b.B, b.A * a.B) &&
            _tools::equal_double(a.C * b.B, b.C * a.B);
}

bool operator!=(const Line& a, const Line& b) {
    return !(a == b);
}

Point line_intersection(const Line& a, const Line& b) {
    return Point((a.B * b.C - a.C * b.B) / (a.A * b.B - a.B * b.A),
        (a.A * b.C - a.C * b.A) / (a.B * b.A - a.A * b.B));
}

// Shape

bool Shape::isCongruentTo(const Shape&) const {
    return false;
}

bool Shape::isSimilarTo(const Shape&) const {
    return false;
}

void Shape::reflex(const Point& center) {
    rotate(center, 180);
}

Shape::~Shape() = default;

bool Shape::operator==(const Shape&) const {
    return false;
}

bool Shape::operator!=(const Shape& Another) const {
    return !(*this == Another);
}

// Polygon

Polygon::Polygon(const std::vector<Point>& points): vertices(points) {}
double Polygon::perimeter() const {
    double sum = distance_between_points(vertices[0], vertices.back());
    for (size_t i = 1; i < vertices.size(); ++i) {
        sum += distance_between_points(vertices[i], vertices[i - 1]);
    }
    return sum;
}

double Polygon::area() const {
    double sum = trapeze_area(vertices[0], vertices.back());
    for (size_t i = 1; i < vertices.size(); ++i) {
        sum += trapeze_area(vertices[i], vertices[i - 1]);
    }
    return fabs(sum);
}

bool Polygon::operator==(const Shape& Another) const {
    try {
        const Polygon& A = dynamic_cast<const Polygon&>(Another);
        if (vertices.size() != A.vertices.size()) return false;
        size_t n = vertices.size();
        size_t first = 0;
        while (first < n && A.vertices[first] != vertices[0]) {
            ++first;
        }
        int step = 1;
        if (A.vertices[(first + 1) % n] != vertices[1]) step = -1;
        for (size_t i = 0; i < n; ++i) {
            if (vertices[i] != A.vertices[(first + n + step * i) % n]) {
                return false;
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool Polygon::isCongruentTo(const Shape& Another) const {
    try {
        const Polygon& A = dynamic_cast<const Polygon&>(Another);
        return _tools::equal_double(similarity_coefficient(A), 1);
    } catch (...) {
        return false;
    }
}

bool Polygon::isSimilarTo(const Shape& Another) const {
    try {
        const Polygon& A = dynamic_cast<const Polygon&>(Another);
        return !_tools::equal_double(similarity_coefficient(A), 0);
    } catch (...) {
        return false;
    }
}

bool Polygon::containsPoint(const Point& point) const {
    size_t n = vertices.size();
    size_t first = 0;
    while (first < n && _tools::equal_double(Vector2D(vertices[(first + 1) % n] -
            vertices[first]) * Vector2D(point - vertices[first]), 0)) {
        ++first;
    }
    double etalon = Vector2D(vertices[(first + 1) % n] - vertices[first]) *
            Vector2D(point - vertices[first]);
    for (size_t i = 0; i < n; ++i) {
        double current = Vector2D(vertices[(i + 1) % n] - vertices[i]) *
                Vector2D(point - vertices[i]);
        if (current * etalon < 0) return false;
    }
    return true;
}

size_t Polygon::verticesCount() const {
    return vertices.size();
}

std::vector<Point> Polygon::getVertices() const {
    return vertices;
}

bool Polygon::isConvex() const {
    if (vertices.size() <= 3) return true;
    double etalon = Vector2D(vertices.back() - vertices[0]) *
            Vector2D(vertices[1] - vertices[0]);
    for (size_t i = 1; i < vertices.size(); ++i) {
        double current = Vector2D(vertices[i - 1] - vertices[i]) *
                Vector2D(vertices[(i + 1) % vertices.size()] - vertices[i]);
        if (current * etalon < 0) return false;
    }
    return true;
}

void Polygon::rotate(const Point& center, const double& angle) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertices[i] = center + Vector2D(vertices[i] - center).rotated(angle);
    }
}

void Polygon::reflex(const Point& center) {
    rotate(center, 180);
}

void Polygon::reflex(const Line& axis) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertices[i] = vertices[i].reflexed(axis);
    }
}

void Polygon::scale(const Point& center, const double& coefficient) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertices[i] = center + (vertices[i] - center) * coefficient;
    }
}

double Polygon::trapeze_area(const Point& A, const Point& B) const {
    return (B.x - A.x) * (A.y + B.y) / 2;
}

double Polygon::similarity_coefficient(const Polygon& Another) const {
    if (vertices.size() != Another.vertices.size()) return false;
    size_t n = vertices.size();
    for (size_t i = 0; i < n; ++i) {
        for (int step = -1; step <= 1; step += 2) {
            for (int sign = -1; sign <= 1; sign += 2) {
                bool ok = true;
                double coefficient = distance_between_points(vertices[0],
                        vertices[1]) / distance_between_points(Another.vertices[i],
                        Another.vertices[(i + n + step) % n]);
                for (size_t j = 2; j < n && ok; ++j) {
                    double this_distance = distance_between_points(vertices[j],
                            vertices[j - 1]);
                    double another_distance = distance_between_points(
                            Another.vertices[(i + n + j * step) % n],
                            Another.vertices[(i + n + (j - 1) * step) % n]);
                    double this_area = Vector2D(vertices[j - 1] - vertices[0]) *
                            Vector2D(vertices[j] - vertices[0]);
                    double another_area = Vector2D(Another.vertices[(i + n +
                            (j - 1) * step) % n] - Another.vertices[i]) *
                            Vector2D(Another.vertices[(i + n + j * step) % n] -
                            Another.vertices[i]);
                    if (!_tools::equal_double(this_distance / another_distance, coefficient) ||
                            !_tools::equal_double(sign * this_area / another_area,
                            _tools::sqr(coefficient))) {
                        ok = false;
                    }
                }
                if (ok) return coefficient;
            }
        }
    }
    return 0;
}

// Ellipse

Ellipse::Ellipse(const Point& focus1, const Point& focus2, double sum_distances):
        a(sum_distances  / 2), b(sqrt(_tools::sqr(sum_distances ) -
        _tools::sqr(distance_between_points(focus1, focus2))) / 2),
        _center((focus1 + focus2) / 2), slope(focus1 - _center) {
    double length = slope.length();
    if (!_tools::equal_double(length, 0)) slope /= length;
    if (slope.x < 0) slope *= -1;
}

double Ellipse::perimeter() const {
    return PI * (3 * (a + b) - sqrt((3 * a + b) * (a + 3 * b)));
}

double Ellipse::area() const {
    return PI * a * b;
}

bool Ellipse::operator==(const Shape& Another) const {
    try {
        const Ellipse& A = dynamic_cast<const Ellipse&>(Another);
        return _tools::equal_double(a, A.a) && _tools::equal_double(b, A.b) &&
                _center == A._center && slope == A.slope;
    } catch (...) {
        return false;
    }
}

bool Ellipse::isCongruentTo(const Shape& Another) const {
    try {
        const Ellipse& A = dynamic_cast<const Ellipse&>(Another);
        return _tools::equal_double(a, A.a) && _tools::equal_double(b, A.b);
    } catch (...) {
        return false;
    }
}

bool Ellipse::isSimilarTo(const Shape& Another) const {
    try {
        const Ellipse& A = dynamic_cast<const Ellipse&>(Another);
        return _tools::equal_double(a * A.b, b * A.a);
    } catch (...) {
        return false;
    }
}

bool Ellipse::containsPoint(const Point& point) const {
    std::pair<Point, Point> focuses = this->focuses();
    return distance_between_points(focuses.first, point) +
            distance_between_points(focuses.second, point) <= 2 * a;
}

std::pair<Point, Point> Ellipse::focuses() const {
    double c = sqrt(_tools::sqr(a) - _tools::sqr(b));
    return {_center + slope * c, _center - slope * c};
}

std::pair<Line, Line> Ellipse::directrices() const {
    Vector2D direction = slope.normal();
    double shift_x = a / eccentricity();
    Vector2D shift = slope * shift_x;
    return {Line(_center + shift, _center + shift + direction),
            Line(_center - shift, _center - shift + direction)};
}

double Ellipse::eccentricity() const {
    return sqrt(1 - _tools::sqr(b / a));
}

Point Ellipse::center() const {
    return _center;
}

void Ellipse::rotate(const Point& center, const double& angle) {
    slope = slope.rotated(angle);
    _center = center + Vector2D(_center - center).rotated(angle);
}

void Ellipse::reflex(const Point& center) {
    rotate(center, 180);
}

void Ellipse::reflex(const Line& axis) {
    _center = _center.reflexed(axis);
    slope = slope.reflexed(axis);
}

void Ellipse::scale(const Point& center, const double& coefficient) {
    std::pair<Point, Point> focuses = this->focuses();
    focuses.first = center + (focuses.first - center) * coefficient;
    focuses.second = center + (focuses.second - center) * coefficient;
    Point A = _center + slope * a;
    Point B = _center - slope * a;
    A = center + (A - center) * coefficient;
    B = center + (B - center) * coefficient;
    Ellipse new_ellipse = Ellipse(focuses.first, focuses.second,
            distance_between_points(A, B));
    a = new_ellipse.a;
    b = new_ellipse.b;
    _center = new_ellipse._center;
    slope = new_ellipse.slope;
}

// Circle

Circle::Circle(const Point& A, const double& r): Ellipse(A, A, 2 * r) {}
double Circle::radius() const {
    return a;
}

// Rectangle

Rectangle::Rectangle(const Point& A, const Point& B, double k) {
    if (k < 1) k = 1 / k;
    double diagonal = distance_between_points(A, B);
    double shorter = sqrt(_tools::sqr(diagonal) / (1 + _tools::sqr(k)));
    Point H = A + (B - A) * _tools::sqr(shorter / diagonal);
    Point C = H + Vector2D((B - A) * k * _tools::sqr(shorter / diagonal)).normal();
    H = B - (B - A) * _tools::sqr(shorter / diagonal);
    Point D = H + Vector2D((B - A) * k * _tools::sqr(shorter / diagonal)).normal() * -1;
    vertices = {A, C, B, D};
}

double Rectangle::area() const {
    return distance_between_points(vertices[0], vertices[1]) *
            distance_between_points(vertices[1], vertices[2]);
}

Point Rectangle::center() const {
    return (vertices[0] + vertices[2]) / 2;
}

std::pair<Line, Line> Rectangle::diagonalas() const {
    return {Line(vertices[0], vertices[2]),
            Line(vertices[1], vertices[3])};
}

// Square

Square::Square(const Point& A, const Point& B): Rectangle(A, B, 1) {}
Circle Square::circumscribedCircle() const {
    return Circle(center(), distance_between_points(vertices[0],
            vertices[2]) / 2);
}

Circle Square::inscribedCircle() const {
    return Circle(center(), distance_between_points(vertices[0],
            vertices[1]) / 2);
}

// Triangle

Triangle::Triangle(const Point& A, const Point& B, const Point& C):
        Polygon(std::vector<Point>{A, B, C}) {}

double Triangle::area() const {
    return abs(Vector2D(vertices[1] - vertices[0]) *
            Vector2D(vertices[2] - vertices[0])) / 2;
}

Circle Triangle::circumscribedCircle() const {
    Point middle01 = (vertices[0] + vertices[1]) / 2;
    Line a(middle01, middle01 + Vector2D(vertices[0] - vertices[1]).normal());
    Point middle02 = (vertices[0] + vertices[2]) / 2;
    Line b(middle02, middle02 + Vector2D(vertices[0] - vertices[2]).normal());
    Point center = line_intersection(a, b);
    return Circle(center, distance_between_points(center, vertices[0]));
}

Circle Triangle::inscribedCircle() const {
    double length01 = distance_between_points(vertices[0], vertices[1]);
    double length02 = distance_between_points(vertices[0], vertices[2]);
    double length12 = distance_between_points(vertices[1], vertices[2]);
    Line a(vertices[0], vertices[0] * (1 - length01 - length02) +
            vertices[2] * length01  + vertices[1] * length02);
    Line b(vertices[1], vertices[1] * (1 - length01 - length12) +
            vertices[2] * length01  + vertices[0] * length12);
    Point center = line_intersection(a, b);
    double p = (length01 + length02 + length12) / 2;
    double radius = sqrt((p - length01) * (p - length02) * (p - length12) / p);
    return Circle(center, radius);
}

Point Triangle::centroid() const {
    return (vertices[0] + vertices[1] + vertices[2]) / 3;
}

Point Triangle::orthocenter() const {
    Line a(vertices[0], vertices[0] + Vector2D(vertices[2] -
            vertices[1]).normal());
    Line b(vertices[1], vertices[1] + Vector2D(vertices[2] -
            vertices[0]).normal());
    return line_intersection(a, b);
}

Line Triangle::EulerLine() const {
    Point A = centroid();
    Point B = orthocenter();
    if (A == B) B = circumscribedCircle().center();
    return Line(A, B);
}

Circle Triangle::ninePointsCircle() const {
    return Triangle((vertices[0] + vertices[1]) / 2, (vertices[1] +vertices[2]) / 2,
            (vertices[2] + vertices[0]) / 2).circumscribedCircle();
}
