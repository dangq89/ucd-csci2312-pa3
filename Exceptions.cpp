//
// Created by Quang Dang on 3/28/16.
//

#include "Exceptions.h"
#include "Cluster.h"
#include <sstream>

namespace Clustering {


    OutOfBoundsEx::OutOfBoundsEx(unsigned int c, int r) {
        __name = "OutOfBoundsEx";
        __current = c;
        __rhs = r;
    }

    unsigned int OutOfBoundsEx::getCurrent() const {
        return __current;
    }

    int OutOfBoundsEx::getRhs() const {
        return __rhs;
    }

    std::string OutOfBoundsEx::getName() const {
        return __name;
    }

    std::ostream &operator<<(std::ostream &os, const OutOfBoundsEx &ex) {
        os << ex.getName() << ", size = " << ex.getCurrent() << ", rhs = " << ex.getRhs() << ')';
        return os;
    }

    DimensionalityMismatchEx::DimensionalityMismatchEx(unsigned int c, unsigned int r) {
        __name = "DimensionalityMismatchEx";
        __current = c;
        __rhs = r;
    }

    unsigned int DimensionalityMismatchEx::getCurrent() const {
        return __current;
    }

    unsigned int DimensionalityMismatchEx::getRhs() const {
        return __rhs;
    }

    std::string DimensionalityMismatchEx::getName() const {
        return __name;
    }

    std::ostream &operator<<(std::ostream &os, const DimensionalityMismatchEx &ex) {
        os << ex.getName() << ", Dimensionality = " << ex.getCurrent() << ", rhs = " << ex.getRhs() << ')';
        return os;
    }

    ZeroClustersEx::ZeroClustersEx() {
        __name = "ZeroClustersEx";
    }

    std::string ZeroClustersEx::getName() const {
        return __name;
    }

    std::ostream &operator<<(std::ostream &os, const ZeroClustersEx &ex) {
        os << "zero clusters" << ex.getName();
        return os;
    }

    DataFileOpenEx::DataFileOpenEx(std::string filename) {
        __name = "DataFileOpenEx";
        __filename = filename;
    }

    std::string DataFileOpenEx::getFilename() const {
        return __filename;
    }

    std::string DataFileOpenEx::getName() const {
        return __name;
    }

    std::ostream &operator<<(std::ostream &os, const DataFileOpenEx &ex) {
        os << ex.getName() << ", " << ex.getFilename() << " does not exist";
        return os;
    }

    ZeroDimensionsEx::ZeroDimensionsEx() {
        __name = "ZeroDimensionsEx";
    }

    std::string ZeroDimensionsEx::getName() const {
        return __name;
    }

    std::ostream &operator<<(std::ostream &os, const ZeroDimensionsEx &ex) {
        os << ex.getName() << ", Zero Dimensions";
        return os;
    }

    EmptyClusterEx::EmptyClusterEx() {
        __name = "EmptyClusterEx";
    }

    std::string EmptyClusterEx::getName() const {
        return __name;
    }

    std::ostream &operator<<(std::ostream &os, const EmptyClusterEx &ex) {
        os << "Empty cluster" << ex.getName();
        return os;
    }
}
