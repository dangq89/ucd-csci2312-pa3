//
// Created by Quang Dang on 3/28/16.
//KMeans.cpp

#include "KMeans.h"
#include "Exceptions.h"

namespace Clustering {

    KMeans::KMeans(unsigned int dim, unsigned int k, std::string filename, unsigned int maxIter) :
            __dimensionality(dim),
            __k(k),
            __iFileName(filename),
            __maxIter(maxIter),
            __numIter(0),
            __numNonempty(0) {

        if (k == 0) {
            throw ZeroClustersEx();
        }

        __initCentroids = new Point*[k];

        std::ifstream dataIn(__iFileName);
        if (__iFileName != "") {
            __clusters = new Cluster*[__k];
            for (int i = 0; i < __k; ++i) {
                __clusters[i] = new Cluster(__dimensionality);
            }
            if (!dataIn.is_open()) {
                for (int i = 0; i < k; ++i) {
                    delete __clusters[i];
                }
                delete [] __clusters;
                throw DataFileOpenEx(__iFileName);
            }
            else {
                dataIn >> *__clusters[0];
                __numNonempty = 1;
                dataIn.close();
            }
        }
        else {
            throw DataFileOpenEx(__iFileName);
        }

        for (int i = 0; i < __k; ++i) {
            __initCentroids[i] = new Point(__dimensionality);
        }

        __clusters[0]->pickCentroids(k, __initCentroids);

        for (int i = 0; i < __k; ++i) {
            __clusters[i]->centroid.set(*__initCentroids[i]);
        }
    }

    KMeans::~KMeans() {
        for (unsigned int i = 0; i < __k; ++i) {
            delete __initCentroids[i];
        }
        delete[] __initCentroids;

        for (unsigned int i = 0; i < __k; ++i) {
            delete __clusters[i];
        }
        delete[] __clusters;
    }

    unsigned int KMeans::getMaxIter() {
        return __maxIter;
    }

    unsigned int KMeans::getNumIters() {
        return __numIter;
    }

    unsigned int KMeans::getNumNonemptyClusters() {
        return __numNonempty;
    }

    unsigned int KMeans::getNumMovesLastIter() {
        return __numMovesLastIter;
    }

    Cluster &KMeans::operator[](unsigned int u) {
        if (__k == 0) {
            throw ZeroClustersEx();
        }
        if (u >= __k) {
            throw DimensionalityMismatchEx(__k, u);
        }
        return *__clusters[u];
    }

    const Cluster &KMeans::operator[](unsigned int u) const {
        if (__k == 0) {
            throw ZeroClustersEx();
        }
        if (u >= __k) {
            throw DimensionalityMismatchEx(__k, u);
        }
        return *__clusters[u];
    }

    std::ostream &operator<<(std::ostream &os, const KMeans &kmeans) {
        Point inf(kmeans.__dimensionality);
        for (int i = 0; i < kmeans.__dimensionality; ++i) {
            inf[i] = std::numeric_limits<double>::max();
        }
        for (int i = 0; i < kmeans.__k; ++i) {
            if (!kmeans.__clusters[i]->centroid.equal(inf)) {
                os << *kmeans.__clusters[i] << std::endl;
            }
        }
        return os;
    }

    void KMeans::run() {
        unsigned int moves = 100;
        unsigned int iter = 0;
        int nonempty = 0;

        while (moves > 0 && iter < __maxIter) {
            moves = 0;

            for (int i = 0; i < __k; ++i) {
                for (int j = 0; j < __clusters[i]->getSize(); ++j) {
                    Cluster &c = *(__clusters[i]);

                    Point current_point(__dimensionality);
                    current_point = c[j];
                    int short_dist_index = 0;

                    double short_dist = current_point.distanceTo(*__initCentroids[0]);
                    for (int e = 0; e < __k; e++) {
                        if (current_point.distanceTo(*__initCentroids[e]) < short_dist) {
                            short_dist = current_point.distanceTo(*__initCentroids[e]);
                            short_dist_index = e;
                        }
                    }
                    Cluster::Move change_clusters(current_point, *__clusters[i], *__clusters[short_dist_index]);
                    change_clusters.perform();

                    for (int c = 0; c < __k; ++c) {

                        __clusters[c]->centroid.compute();
                    }
                    if (*__clusters[i] != *__clusters[short_dist_index]) {
                        moves++;
                    }
                }
            }
            ++iter;
        }

        Point inf(__dimensionality);
        for (int i = 0; i < __dimensionality; ++i) {
            inf[i] = std::numeric_limits<double>::max();
        }

        for (int i = 0; i < __k; ++i) {
            if (__clusters[i]->centroid.get() != inf) {
                ++nonempty;
            }
        }
        __numIter = iter;
        __numMovesLastIter = moves;
        __numNonempty = nonempty;
    }
}

