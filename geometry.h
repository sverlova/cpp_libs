#include <iostream>
#include <stdio.h>
#include <vector>
#include <cmath>

const double PI = acos(-1);
const double eps = 1e-6;

class Line;

struct Point {
    double x;
    double y;

    Point();
    Point(double x, double y);
    Point reflexed(const Line& axis) const;
};

bool operator==(const Point& A, const Point& B);
bool operator!=(const Point& A, const Point& B);

std::ostream& operator<<(std::ostream& out, const Point& p);

double distance_between_points(const Point& A, const Point& B);

struct Vector2D: public Point {
    Vector2D() = default;
    Vector2D(const Point& A);
    Vector2D(double x, double y);

    Vector2D& operator+=(const Vector2D& A);
    Vector2D& operator-=(const Vector2D& A);
    Vector2D& operator*=(const double& k);
    Vector2D& operator/=(const double& k);

    double length() const;

    Vector2D normal() const;
    Vector2D rotated(double angle) const;
    Vector2D reflexed(const Line& axis) const;
};

Vector2D operator+(const Vector2D& A, const Vector2D& B);
Vector2D operator-(const Vector2D& A, const Vector2D& B);
Vector2D operator*(const Vector2D& A, const double& k);
Vector2D operator/(const Vector2D& A, const double& k);
double operator*(const Vector2D& a, const Vector2D& b);

class Line {
  public:
    Line(const Point& A, const Point& B);
    Line(double k, double b);
    Line(const Point& P, double k);

    double get_y(const double x) const;
    Vector2D get_vector() const;

  private:
    double A, B, C;
    // Ax + By + C = 0

    friend bool operator==(const Line& a, const Line& b);
    friend Point line_intersection(const Line& a, const Line& b);
};

bool operator==(const Line& a, const Line& b);
bool operator!=(const Line& a, const Line& b);

Point line_intersection(const Line& a, const Line& b);

class Shape {
  public:
    virtual double perimeter() const = 0;
    virtual double area() const = 0;

    virtual bool operator==(const Shape&) const = 0;
    bool operator!=(const Shape& Another) const;

    virtual bool isCongruentTo(const Shape&) const = 0;
    virtual bool isSimilarTo(const Shape&) const = 0;
    virtual bool containsPoint(const Point& point) const = 0;

    virtual void rotate(const Point& center, const double& angle) = 0;
    virtual void reflex(const Point& center);
    virtual void reflex(const Line& axis) = 0;
    virtual void scale(const Point& center, const double& coefficient) = 0;

    virtual ~Shape() = 0;
};

class Polygon: public Shape {
  public:
    Polygon() = default;
    Polygon(const std::vector<Point>& points);

    double perimeter() const override;
    double area() const override;

    bool operator==(const Shape& Another) const override;
    bool isCongruentTo(const Shape& Another) const override;
    bool isSimilarTo(const Shape& Another) const override;
    bool containsPoint(const Point& point) const override;

    size_t verticesCount() const;
    std::vector<Point> getVertices() const;
    bool isConvex() const;

    void rotate(const Point& center, const double& angle) override;
    void reflex(const Point& center) override;
    void reflex(const Line& axis) override;
    void scale(const Point& center, const double& coefficient) override;

    ~Polygon() override = default;

  protected:
    std::vector<Point> vertices;

  private:
    double trapeze_area(const Point& A, const Point& B) const;
    double similarity_coefficient(const Polygon& Another) const;
};

class Ellipse: public Shape {
  public:
    Ellipse(const Point& focus1, const Point& focus2, double sum_distances);

    virtual double perimeter() const override;
    virtual double area() const override;

    bool operator==(const Shape& Another) const override;
    bool isCongruentTo(const Shape& Another) const override;
    bool isSimilarTo(const Shape& Another) const override;
    bool containsPoint(const Point& point) const override;

    std::pair<Point, Point> focuses() const;
    std::pair<Line, Line> directrices() const;
    double eccentricity() const;
    Point center() const;

    void rotate(const Point& center, const double& angle) override;
    void reflex(const Point& center) override;
    void reflex(const Line& axis) override;
    void scale(const Point& center, const double& coefficient) override;

    ~Ellipse() override = default;

  protected:
    double a;
    double b;
    Point _center;
    Vector2D slope;
    // x^2 / a^2 + y^2 / b^2 = 1
    // _center - (0, 0)
    // slope - vector(0, 1)
};

class Circle: public Ellipse {
  public:
    Circle(const Point& A, const double& r);
    double radius() const;
    ~Circle() override = default;
};

class Rectangle: public Polygon {
  public:
    Rectangle(const Point& A, const Point& B, double k);

    virtual double area() const override;
    Point center() const;
    std::pair<Line, Line> diagonalas() const;

    ~Rectangle() override = default;
};

class Square: public Rectangle {
  public:
    Square(const Point& A, const Point& B);

    Circle circumscribedCircle() const;
    Circle inscribedCircle() const;

    ~Square() override = default;
};

class Triangle: public Polygon {
  public:
    Triangle(const Point& A, const Point& B, const Point& C);

    virtual double area() const override;
    Circle circumscribedCircle() const;
    Circle inscribedCircle() const;
    Point centroid() const;
    Point orthocenter() const;
    Line EulerLine() const;
    Circle ninePointsCircle() const;

    ~Triangle() override = default;
};
