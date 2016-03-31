// File: Exceptions.h
//
// Created by Ivo Georgiev on 3/7/16.
//

#ifndef CLUSTERING_EXCEPTIONS_H
#define CLUSTERING_EXCEPTIONS_H

#include <iostream>

namespace Clustering {

    class OutOfBoundsEx {
        unsigned int __current;
        int __rhs;
        std::string __name;

    public:
        OutOfBoundsEx(unsigned int c, int r);
        unsigned int getCurrent() const;
        int getRhs() const;
        std::string getName() const;

        friend std::ostream &operator<<(std::ostream &os, const OutOfBoundsEx &ex);
    };


    class DimensionalityMismatchEx {
        unsigned int __current, __rhs;
        std::string __name;

    public:
        DimensionalityMismatchEx(unsigned int c, unsigned int r);
        unsigned int getCurrent() const;
        unsigned int getRhs() const;
        std::string getName() const;

        friend std::ostream &operator<<(std::ostream &os, const DimensionalityMismatchEx &ex);
    };

    class ZeroClustersEx {
        std::string __name;

    public:
        ZeroClustersEx();
        std::string getName() const;

        friend std::ostream &operator<<(std::ostream &os, const ZeroClustersEx &ex);
    };


    class DataFileOpenEx {
        std::string __name, __filename;

    public:
        DataFileOpenEx(std::string filename);
        std::string getFilename() const;
        std::string getName() const;

        friend std::ostream &operator<<(std::ostream &os, const DataFileOpenEx &ex);
    };


    class ZeroDimensionsEx {
        std::string __name;

    public:
        ZeroDimensionsEx();
        std::string getName() const;

        friend std::ostream &operator<<(std::ostream &os, const ZeroDimensionsEx &ex);
    };


    class EmptyClusterEx {
        std::string __name;

    public:
        EmptyClusterEx();
        std::string getName() const;

        friend std::ostream &operator<<(std::ostream &os, const EmptyClusterEx &ex);
    };
}


#endif //CLUSTERING_EXCEPTIONS_H
