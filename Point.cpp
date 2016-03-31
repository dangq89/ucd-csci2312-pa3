//
// Created by Quang Dang on 3/28/16.
// Point.cpp

#include "Point.h"
#include "Exceptions.h"

#include <sstream>
#include <cmath>

namespace Clustering {

    unsigned int Point::__idGen = 0;
    const char Point::POINT_VALUE_DELIM = ',';

    Point::Point(unsigned int iDim) {   // Constructor

        if(iDim == 0) throw ZeroDimensionsEx();

        __id = __idGen++;
        __dim = iDim;
        __values = new double[__dim];
        for (int i = 0; i < __dim; ++i) {
            __values[i] = 0;
        };
    }

    void Point::rewindIdGen() {
        __idGen--;
    }

    Point::Point(unsigned int iDim, double *__v) : Point(iDim) {        // doesn't get called
        for (int i = 0; i < iDim; i++)
            __values[i] = __v[i];
    }

    Point::Point(const Point &p) {

        __dim = p.__dim;
        __values = new double[p.__dim];

        for (int i = 0; i < __dim; ++i) {
            __values[i] = p.__values[i];
        }
        __id = p.__id;
    }

    Point &Point::operator=(const Point &rhs) {
        if (__dim != rhs.__dim)
            throw DimensionalityMismatchEx(__dim, rhs.__dim);

        if (this == &rhs)
            return *this;

        std::size_t i;
        for (i = 0; i < __dim; ++i) {
            __values[i] = rhs.__values[i];
        }
        __id = rhs.__id;
            return *this;
    }

    Point::~Point() {
       delete[] __values;
    }

    int Point::getId() const {
        return __id;
    }

    unsigned int Point::getDims() const {
        return __dim;
    }

    void Point::setValue(unsigned int a, double b) {
        if (__dim <= a)
            throw OutOfBoundsEx(a, __dim);

        __values[a] = b;
    }

    double Point::getValue(unsigned int a) const {
        if (__dim <= a)
            throw OutOfBoundsEx(a, __dim);

        return __values[a];
    }

    double Point::distanceTo(const Point &p) const {
        if (__dim != p.__dim)
            throw DimensionalityMismatchEx(__dim, p.__dim);

        double sum_products = 0;

        for (int i = 0; i < __dim; i++) {
            sum_products += pow(__values[i] - p[i], 2);
        }
        return sqrt(sum_products);
    }

    Point &Point::operator*=(double rhs) {
        for (int i = 0; i < __dim; i++)
            __values[i] *= rhs;
        return *this;
    }

    Point &Point::operator/=(double rhs) {
        for (int i = 0; i < __dim; i++)
            __values[i] /= rhs;
        return *this;
    }

    const Point Point::operator*(double d) const {
        Point p(__dim);
        for (int i = 0; i < __dim; i++) {
            p.__values[i] = __values[i] * d;
        }
        return p;
    }

    const Point Point::operator/(double d) const {
        Point p(__dim);
        for (int i = 0; i < __dim; i++) {
            p.__values[i] = __values[i] / d;
        }
        return p;
    }

    double &Point::operator[](unsigned int index) {
        if (__dim <= index)
            throw OutOfBoundsEx(index, __dim);
        return __values[index];
    }

    const double &Point::operator[](unsigned int index) const {
        if (__dim <= index)
            throw OutOfBoundsEx(index, __dim);
        return __values[index];
    }

    // Friends
    Point &operator+=(Point &lhs, const Point &rhs) {
        if (lhs.__dim != rhs.__dim)
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);

        for (int i = 0; i < lhs.__dim; i++)
            lhs.__values[i] += rhs.__values[i];

        return lhs;
    }

    Point &operator-=(Point &lhs, const Point &rhs) {
        if (lhs.__dim != rhs.__dim)
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);

        for (int i = 0; i < lhs.__dim; i++)
            lhs.__values[i] -= rhs.__values[i];

        return lhs;
    }

    const Point operator+(const Point &lhs, const Point &rhs) {
        if (lhs.__dim != rhs.__dim)
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);

        Point p(lhs);
        return p += rhs;
    }

    const Point operator-(const Point &lhs, const Point &rhs) {
        if (lhs.__dim != rhs.__dim)
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);

        Point p(lhs);
        return p -= rhs;
    }

    bool operator==(const Point &lhs, const Point &rhs) {
        if (lhs.__dim != rhs.__dim)
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);

        for (int i = 0; i < lhs.__dim; ++i) {
            if (lhs.__values[i] != rhs.__values[i])
                return false;
        }
        return true;
    }

    bool operator!=(const Point &lhs, const Point &rhs) {
        if (lhs.__dim != rhs.__dim)
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);

        if (lhs == rhs)
            return false;

        return true;
    }


    bool operator<(const Point &lhs, const Point &rhs) {
        if (lhs.__dim != rhs.__dim)
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);

        for (int i = 0; i < lhs.__dim; i++) {
            if (lhs.__values[i] < rhs.__values[i])
                return true;
            else if (lhs.__values[i] > rhs.__values[i])
                return false;
        }
        return false;
    }

    bool operator>(const Point &lhs, const Point &rhs) {
        if (lhs.__dim != rhs.__dim)
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);

        for (int i = 0; i < lhs.__dim; i++) {
            if (lhs.__values[i] > rhs.__values[i])
                return true;
        }
        return false;
    }

    bool operator<=(const Point &lhs, const Point &rhs) {
        if (lhs.__dim != rhs.__dim)
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);

        for (int i = 0; i < lhs.__dim; i++) {
            if (lhs.__values[i] <= rhs.__values[i])
                return true;
        }
        return false;
    }

    bool operator>=(const Point &rhs, const Point &lhs) {
        if (lhs.__dim != rhs.__dim)
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);

        for (int i = 0; i < lhs.__dim; i++) {
            if (lhs.__values[i] >= rhs.__values[i])
                return true;
        }
        return false;
    }

    std::ostream &operator<<(std::ostream &os, const Point &p) {
        for (int i = 0; i < p.__dim; i++) {
            os << p.__values[i];
            if (i != p.__dim - 1) {
                os << ", ";
            }
        }
        return os;
    }

    std::istream &operator>>(std::istream &is, Point &p) {
        std::string line;
        double d;
        int i = 0;

        while (std::getline(is, line, Point::POINT_VALUE_DELIM)) {
            d = stod(line);
            p.__values[i++] = d;
        }
        if (i != p.__dim) {
            Point::rewindIdGen();
            throw DimensionalityMismatchEx(p.__dim, i);
        }
        return is;
    }
}
