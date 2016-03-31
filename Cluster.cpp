//
// Created by Quang Dang on 3/28/16.
// Cluster.cpp

#include "Cluster.h"
#include "Exceptions.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <string>

using Clustering::Point;

namespace Clustering {

    unsigned int Cluster::__idGenerator = 0;
    const char Cluster::POINT_CLUSTER_ID_DELIM = ':';

    LNode::LNode(const Point &p, LNodePtr n) : point(p) {
        next = n;
    }

    //---------------------------(Centroid)----------------------------------

    Cluster::Centroid::Centroid(unsigned int d, const Cluster &c) : __c(c), __p(d), __dimensions(d) {
        __valid = false;
    }

    const Point Cluster::Centroid::get() const {    // Getters/Setters
        return __p;
    }

    void Cluster::Centroid::set(const Point &p) {
        __valid = true;
        __p = p;
    }

    bool Cluster::Centroid::isValid() const {
        return __valid;
    }

    void Cluster::Centroid::setValid(bool valid) {
        __valid = valid;
    }

    void Cluster::Centroid::compute() { // Centroid function

        if (__c.__size > 0) {
            int i = 0;
            Point p(__dimensions);
            LNodePtr hold = __c.__points;

            while (hold != nullptr) {
                p += hold->point / __c.getSize();
                ++i;
                hold = hold->next;
            }
            set(p);
        }
        else if (__c.__size == 0) {
            assert(__c.__points == nullptr);
            __valid = true;
            toInfinity();
        }
    }

    bool Cluster::Centroid::equal(const Point &p) const {
        if (p == __p)
            return true;
        return false;
    }

    void Cluster::Centroid::toInfinity() {
        for (int i = 0; i < __p.getDims(); ++i) {
            __p[i] = std::numeric_limits<double>::max();
        }
    }

    //---------------------------(Cluster)----------------------------------

    Cluster::Cluster(unsigned int d) : centroid(d, *this), __dimensionality(d), __points(nullptr), __size(0) {
        __id = __idGenerator++;
    }

    Cluster::Cluster(const Cluster &c) : __size(0), __dimensionality(c.__dimensionality),
                                         centroid(c.__dimensionality, c) {
        if (c.__size == 0) {
            __points = nullptr;
            __id = c.__id;
        }
        else if (c.__size == 1) {
            __points = new LNode(c.__points->point, nullptr);
            __id = c.__id;
            __size++;

        }
        else if (c.__size > 1) {
            LNodePtr hold = c.__points;
            __cpy(hold);
            __id = c.__id;
        }
        centroid.compute();
        assert(__size == c.__size);
    }

    // Assignement operator
    Cluster &Cluster::operator=(const Cluster &c) {
        if (this == &c) {
            return *this;
        }
        else {
            __del();
            LNodePtr hold = c.__points;
            __size = 0;
            __cpy(hold);
            centroid.compute();
            assert(this->__size == c.__size);
            __id = c.__id;

            return *this;
        }
    }

    // destructor
    Cluster::~Cluster() {
        __del();
    }

    // Getters/setters
    unsigned int Cluster::getSize() const {
        return __size;
    }

    unsigned int Cluster::getDimensionality() const {
        return __dimensionality;
    }

    unsigned int Cluster::getId() const {
        return __id;
    }

    // Set fuctions;
    void Cluster::add(const Point &p)  {
        if (p.getDims() != __dimensionality)
            throw DimensionalityMismatchEx(__dimensionality, p.getDims());

        LNodePtr temp = new LNode(p, nullptr);
        LNodePtr prev = __points;
        LNodePtr curr = __points;

        if (__size == 0) {
            __points = temp;
            __size++;
        }
        else if (__size == 1) {
            if (temp->point < curr->point) {
                LNodePtr foo = __points;
                temp->next = foo;
                __points = temp;
                __size++;
            }
            else {
                __points->next = temp;
                __size++;
            }
        }
        else if (__size > 1) {
            int i = 0;
            while (curr->next != nullptr) {
                if (temp->point < curr->point) {
                    if (curr == __points) {
                        LNodePtr foo = __points;
                        temp->next = foo;
                        __points = temp;
                        break;
                    }
                    else {
                        temp->next = curr;
                        prev->next = temp;
                        break;
                    }
                }

                curr = curr->next;

                if (i > 0) {
                    prev = prev->next;
                }
                i++;
            }
            if (curr->next == nullptr) {
                curr->next = temp;
            }
            __size++;
            centroid.setValid(false);
        }
    }

    const Point &Cluster::remove(const Point &p) {
        LNodePtr hold;
        LNodePtr pre = __points;
        LNodePtr cur = __points;
        int i = 0;

        if (p.getDims() != __dimensionality)
            throw DimensionalityMismatchEx(__dimensionality, p.getDims());

        while (cur != nullptr) {
            if (cur->point == p) {
                if (cur == __points) {
                    __points = __points->next;
                    delete cur;
                    --__size;
                    break;
                }
                hold = cur;
                cur = cur->next;
                pre->next = cur;
                delete hold;
                --__size;
                break;
            }

            cur = cur->next;
            if (i > 0) {
                pre = pre->next;
            }
            ++i;
        }
        if (__size == 0) {
            centroid.setValid(false);
        }
        return p;
    }

    bool Cluster::contains(const Point &p) const {
        if (__dimensionality != p.getDims()) {
            throw DimensionalityMismatchEx(__dimensionality, p.getDims());
        }
        if (__in(p)) {
            return true;
        }
        return false;
    }

    void Cluster::pickCentroids(unsigned int k, Point **pointArray) {


        Point inf(__dimensionality);
        LNodePtr curr = __points;
        for (int j = 0; j < __dimensionality; ++j) {        // fills up infinite threshold for array
            inf[j] = std::numeric_limits<double>::max();
        }

        if (k >= __size) {
            for (int i = 0; i < __size; ++i) {
                *pointArray[i] = curr->point;
                curr = curr->next;
            }
            for (int i = __size; i < k; ++i) {
                *pointArray[i] = inf;
                }

        }
        else if (k < __size) {
            for (int i = 0; i < k; ++i) {
                *pointArray[i] = curr->point;
                curr = curr->next;
            }
        }
    }

    // Members: subscripts
    const Point &Cluster::operator[](unsigned int index) const {
        if (__size <= 0)
            throw EmptyClusterEx();

        if (index >= __size)
            throw OutOfBoundsEx(__size, index);

        int i = 1;
        LNodePtr cur = __points;

        if (index == 0) {
            return __points->point;
        }

        cur = cur->next;

        while (cur->next != nullptr) {
            if (i == index) {
                return cur->point;
            }
            cur = cur->next;
            ++i;
        }
        return cur->point;
    }

    // Operators with Point-s
    // - Members
    Cluster &Cluster::operator+=(const Point &rhs) {
        if (rhs.getDims() != __dimensionality)
        {
            throw DimensionalityMismatchEx(__dimensionality, rhs.getDims());
        }
        this->add(rhs);
        centroid.setValid(false);
        return *this;
    }

    Cluster &Cluster::operator-=(const Point &rhs) {
        if (rhs.getDims() != __dimensionality)
        {
            throw DimensionalityMismatchEx(__dimensionality, rhs.getDims());
        }
        if (this->contains(rhs)) {
            this->remove(rhs);
            centroid.setValid(false);
        }
        return *this;
    }
    // Operators with Cluster-s
    // - Members
    Cluster &Cluster::operator+=(const Cluster &rhs) {
        LNodePtr temp = rhs.__points;

        while (temp != nullptr) {
            if (!this->__in(temp->point)) {
                this->add(temp->point);
            }
                temp = temp->next;
        }
        return *this;
    }

    Cluster &Cluster::operator-=(const Cluster &rhs) {  // asymetric differences
        LNodePtr temp = rhs.__points;

        while (temp != nullptr) {
            if (__in(temp->point)) {
                remove(temp->point);
                centroid.setValid(false);
            }
            temp = temp->next;
        }

        if (__size == 0) {
            centroid.setValid(false);
        }

        return *this;
    }

    // Binary operators for Cluster-s
    // - Friends (Union)
    const Cluster operator+(const Cluster &lhs, const Cluster &rhs) {   // Arithetic for clusters
        if (lhs.__dimensionality != rhs.__dimensionality)
            throw DimensionalityMismatchEx(lhs.__dimensionality, rhs.__dimensionality);

        Cluster c(lhs);
        c += rhs;
        return c;
    }

    const Cluster operator-(const Cluster &lhs, const Cluster &rhs) {
        if (lhs.__dimensionality != rhs.__dimensionality)
            throw DimensionalityMismatchEx(lhs.__dimensionality, rhs.__dimensionality);

        Cluster c(lhs);
        c -= rhs;
        return c;
    }

    // Binary operators for Cluster-s and Point pointers
    // - Friends
    const Cluster operator+(const Cluster &lhs, const Point &rhs) { // Arithmetic for cluster and point

        if (lhs.__dimensionality != rhs.getDims())
            throw DimensionalityMismatchEx(lhs.__dimensionality, rhs.getDims());

        Cluster c(lhs);
        c.add(rhs);
        return c;
    }

    const Cluster operator-(const Cluster &lhs, const Point &rhs) {
        if (lhs.__dimensionality != rhs.getDims())
            throw DimensionalityMismatchEx(lhs.__dimensionality, rhs.getDims());

        Cluster c(lhs);
        if (c.contains(rhs)) {
            c.remove(rhs);
        }
        return c;
    }

    // IO
    // - Friends
    std::ostream &operator<<(std::ostream &os, const Cluster &c) {
        LNodePtr cur = c.__points;
        if (c.__size == 0) {
            return os;
        }
        else {
            while (cur->next != nullptr) {
                os << cur->point << '  ' << Cluster::POINT_CLUSTER_ID_DELIM << ' ' << c.__id << std::endl;
                cur = cur->next;
            }

            os << cur->point;
            os << ' ' << Cluster::POINT_CLUSTER_ID_DELIM << ' ' << c.__id;

            return os;
        }
    }

    std::istream &operator>>(std::istream &is, Cluster &c) {
        std::string line;
        while (getline(is, line)) {

            Point p(c.__dimensionality);
            std::stringstream stream(line);
            std::string value;
            int i = 0;
            double d = 0.0;

            while (getline(stream, value, Point::POINT_VALUE_DELIM)) {
                try {
                    if (i >= c.__dimensionality) {
                        p.rewindIdGen();
                        throw OutOfBoundsEx(c.__dimensionality, i);
                    }
                    d = stod(value);
                    p.setValue(i, d);
                }
                catch (OutOfBoundsEx &ex) {
                    std::cerr << "Caught Exception: " << ex << std::endl;
                    p.rewindIdGen();
                }
                ++i;
            }
            try {
                if (i != c.__dimensionality) {
                    throw DimensionalityMismatchEx(c.__dimensionality, i);
                }
                c.add(p);
            }
            catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught Exception: " << ex << std::endl;
            }
        }
        return is;
    }

    // - Friends
    bool operator==(const Cluster &lhs, const Cluster &rhs) {

        if (lhs.__dimensionality != rhs.__dimensionality) {
            throw DimensionalityMismatchEx(lhs.__dimensionality, rhs.__dimensionality);
        }
        if (lhs.__size != rhs.__size) {
            return false;
        }

        LNodePtr clust1 = lhs.__points;
        LNodePtr clust2 = rhs.__points;

        while (clust1 != nullptr) {
            if (clust1->point != clust2->point) {
                return false;
            }
            clust1 = clust1->next;
            clust2 = clust2->next;
        }
        return true;
    }

    bool operator!=(const Cluster &lhs, const Cluster &rhs) {
        if (lhs.__dimensionality != rhs.__dimensionality)
            throw DimensionalityMismatchEx(lhs.__dimensionality, rhs.__dimensionality);

        if (lhs == rhs) {
            return false;
        }
        return true;
    }


    Cluster::Move::Move(const Point &p, Cluster &from, Cluster &to) : __p(p), __from(from), __to(to) {
    }

    void Cluster::Move::perform() {
        __to.add(__from.remove(__p));
    }

//--------------------(Cluster del, cpy & in)----------------------

    void Cluster::__del() {
        while (__points != nullptr && __points->next != nullptr) {
            int i = 1;
            LNodePtr curr;
            curr = __points->next;
            delete __points;
            __points = curr;
            ++i;
        }
        if (__points != nullptr && __points->next == nullptr) {
            delete __points;
        }
    }

    void Cluster::__cpy(LNodePtr pts) {
            if (pts == nullptr)
                return;
            this->__points = new LNode(pts->point, nullptr);
            this->__size++;

            pts = pts->next;

            while (pts != nullptr) {
                this->add(pts->point);
                pts = pts->next;
            }
        }

        bool Cluster::__in(const Point &p) const {
            if (__dimensionality != p.getDims()) {
                throw DimensionalityMismatchEx(__dimensionality, p.getDims());
            }

            if (__size == 0) {
                return false;
            }

            LNodePtr curr = __points;

            while (curr->next != nullptr) {
                if (curr->point.getId() == p.getId()) {
                    return true;
                }
                else {
                    curr = curr->next;
                }
            }
            if (curr->point.getId() == p.getId()) {
                return true;
            }
            return false;
        }

}
