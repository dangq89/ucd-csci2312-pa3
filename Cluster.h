// File: Cluster.h
//
// Created by Ivo Georgiev on 9/14/15.
//

#ifndef CLUSTERING_CLUSTER_H
#define CLUSTERING_CLUSTER_H

#include <assert.h>
#include "Point.h"

namespace Clustering {

    typedef struct LNode *LNodePtr;

    struct LNode {

        Point point;
        LNodePtr next;
        LNode(const Point &p, LNodePtr n);

    };

    class Cluster {
        unsigned int __dimensionality;
        unsigned int __size;
        LNodePtr __points;

        unsigned int __id;

        static unsigned int __idGenerator;

         void __del();
         void __cpy(LNodePtr pts);
         bool __in(const Point &p) const;

        // inner class has private access
        class Centroid {
            unsigned int __dimensions;
            Point __p;
            bool __valid;
            const Cluster &__c;

        public:
            Centroid(unsigned int d, const Cluster &c); // needs ref to cluster

            // no copy or assignment
            Centroid(const Centroid &cent) = delete;

            Centroid &operator[](const Centroid &cent) = delete;

            // getters/setters
            const Point get() const; // doesn't check for validity
            void set(const Point &p); // sets to valid
            bool isValid() const;
            void setValid(bool valid);

            // functions
            void compute();
            bool equal(const Point &) const;
            void toInfinity();
        };

    public:
        static const char POINT_CLUSTER_ID_DELIM;

        Centroid centroid; // the cluster's centroid

        Cluster(unsigned int d);

        // The big three: cpy ctor, overloaded operator=, dtor
        Cluster(const Cluster &cluster);
        Cluster &operator=(const Cluster &);
        ~Cluster();

        // Getters
        unsigned int getSize() const;
        unsigned int getDimensionality() const;
        unsigned int getId() const;

        // Add/remove: They allow calling c1.add(c2.remove(p));
        void add(const Point &);
        const Point &remove(const Point &);
        bool contains(const Point &p) const;

        // Centroid functions
        void pickCentroids(unsigned int k, Point **pointArray); // pick k initial centroids

        // Overloaded operators

        // Element access (aka direct access, array operator)
        const Point &operator[](unsigned int u) const; // const version only

        // IO
        // - Friends
        friend std::ostream &operator<<(std::ostream &, const Cluster &);
        friend std::istream &operator>>(std::istream &, Cluster &);

        // - Friends
        friend bool operator==(const Cluster &lhs, const Cluster &rhs);
        friend bool operator!=(const Cluster &lhs, const Cluster &rhs);

        // Operators with Point-s
        // - Members
        Cluster &operator+=(const Point &rhs); // allocate point
        Cluster &operator-=(const Point &rhs); // delete point(s)

        // Operators with Cluster-s
        // - Members
        Cluster &operator+=(const Cluster &rhs); // union
        Cluster &operator-=(const Cluster &rhs); // (asymmetric) difference

        // Binary operators for Cluster-s
        // - Friends
        friend const Cluster operator+(const Cluster &lhs, const Cluster &rhs);
        friend const Cluster operator-(const Cluster &lhs, const Cluster &rhs);

        // Binary operators for Cluster-s and Point pointers
        // - Friends
        friend const Cluster operator+(const Cluster &lhs, const Point &rhs);
        friend const Cluster operator-(const Cluster &lhs, const Point &rhs);


        class Move {
            const Point &__p;
            Cluster &__from, &__to;

        public:
            Move(const Point &p, Cluster &from, Cluster &to);

            void perform();
        };
    };

}
#endif //CLUSTERING_CLUSTER_H
