// File: ClusteringTests.cpp
//
// Created by Ivo Georgiev on 10/29/15.
//

#include <iostream>
#include <cassert>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <map>
#include <regex>

#include "ClusteringTests.h"
#include "Point.h"
#include "Cluster.h"
#include "KMeans.h"
#include "Exceptions.h"

using namespace Clustering;
using namespace Testing;

#define DESC(x) desc(x, __LINE__)  // ugly hack, but saves some time




// - - - - - - - - - - helper functions - - - - - - - - - -
const Point point_in_and_out(const Point p) { return Point(p); }

// - - - - - - - - - - local classes - - - - - - - - - -
class ClusterCounts {
    std::map<unsigned, unsigned> __counts;
    std::string __filename;
    unsigned __total_points;

public:
    ClusterCounts(std::string filename) :
            __filename(filename),
            __total_points(0) {}
    bool parse_and_analyze();
    unsigned long get_num_clusters() { return __counts.size(); }
    unsigned get_num_points(unsigned id) { return __counts[id]; }
    unsigned get_num_points() { return __total_points; }

    // TODO check cluster id-s are consecutive
};

bool ClusterCounts::parse_and_analyze() {
    bool pass = true;

    std::ifstream csv2(__filename);
    if (csv2.is_open()) {

        std::string line;
        while (getline(csv2, line)) {
            std::regex re(":[ ]?[[:d:]]{1,}"); // ECMAScript, by default
            std::smatch m;
            std::regex_search(line, m, re);
            if (m.size() != 1) { // parse problem
                pass = false;
                break;
            }
            std::string s(m[0]);
            std::regex r("[[:d:]]{1,}");
            std::regex_search(s, m, r);
            int id = stoi(m[0]);
            __counts[id] = __counts[id] + 1;
        }

        for (auto it = __counts.begin(); it != __counts.end(); ++it)
            __total_points += it->second;

        csv2.close();
    } else {
        pass = false;
    }

    return pass;
}


// - - - - - - - - - - T E S T S - - - - - - - - - -

// - - - - - - - - - - P O I N T - - - - - - - - - -

// Smoketest: constructor, copy constructor, destructor
void test_point_smoketest(ErrorContext &ec) {
    bool pass;

    ec.DESC("--- Test - Point - Smoketest ---");

    ec.DESC("constructor, dimensionality, destructor");
    pass = true;
    for (int i = 0; i < 10; i ++) {

        // Construct a Point
        // At the end of the block, destructor will be called
        Point p(10);

        pass = (p.getDims() == 10);
        if (!pass) break;
    }
    ec.result(pass);


    ec.DESC("constructor, large size");
    pass = true;
    for (int i = 0; i < 10; i ++) {

        // Construct a Point
        // At the end of the block, destructor will be called
        Point p(1000000);

        pass = (p.getDims() == 1000000);
        if (!pass) break;
    }
    ec.result(pass);


    ec.DESC("copy constructor");
    pass = true;
    for (int i = 0; i < 10; i ++) {

        // Construct a Point
        // At the end of the block, destructor will be called
        Point p1(10);
        Point p2(p1);

        pass = (p1.getDims() == 10 && p2.getDims() == 10);
        if (!pass) break;
    }
    ec.result(pass);

    ec.DESC("zero dimensions");
    pass = true;
    for (int i = 0; i < 10; i ++) {

        // Construct a Point
        // At the end of the block, destructor will be called
        try {
            Point p1(0);
            pass = false;
        } catch (ZeroDimensionsEx &ex) {
            std::cerr << "Caught exception: " << ex << std::endl;
            pass = (ex.getName() == "ZeroDimensionsEx");
        }

        if (!pass) break;
    }
    ec.result(pass);

}

// id
void test_point_id(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Point - Point ID ---");

    for (int run = 0; run < numRuns; run ++)
    {
        ec.DESC("get a point's id");

        {
            Point p(15);

            pass = (p.getId() >= 0);

            ec.result(pass);
        }

        ec.DESC("sequential id-s");

        {
            Point **points = new Point*[100];
            for (int i=0; i<100; i++)
                points[i] = new Point(15);

            pass = true;
            int firstId = points[0]->getId();
            for (int i=0; i<100; i++)
                pass = pass && (points[i]->getId() == (firstId + i));

            for (int i=0; i<100; i++)
                delete points[i];
            delete [] points;

            ec.result(pass);
        }
    }
}

// setValue, getValue, operator[]
void test_point_getsetelem(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Point - Get/set element ---");

    for (int run = 0; run < numRuns; run ++)
    {
        ec.DESC("values default to zero");

        {
            Point p(50);

            // Check zeros
            pass = true;
            for (int i = 0; i < 50; i ++) {
                pass = pass &&
                        (p.getValue(i) == 0.0) &&
                        (p[i] == 0.0);
            }
            ec.result(pass);
        }

        ec.DESC("setValue, getValue (0-indexed)");

        {
            Point p(20);

            // Set values
            for (int i = 0; i < 20; i ++)
                p.setValue(i, 13.43 * i * i + 4.567 * i + 1.234567);

            // Check values
            pass = true;
            for (int i = 0; i < 20; i ++)
                pass = pass &&
                        (p.getValue(i) == (13.43 * i * i + 4.567 * i + 1.234567)) &&
                        (p[i] == (13.43 * i * i + 4.567 * i + 1.234567));
            ec.result(pass);
        }

        ec.DESC("operator[] (0-indexed)");

        {
            Point p(5);

            // Set values
            for (int i = 0; i < 5; i ++)
                p[i] = 1000000.43 * i * i + 400000.567 * i + 10000.234567;

            // Check values
            pass = true;
            for (int i = 0; i < 5; i ++)
                pass = pass &&
                       (p.getValue(i) == (1000000.43 * i * i + 400000.567 * i + 10000.234567)) &&
                       (p[i] == (1000000.43 * i * i + 400000.567 * i + 10000.234567));

            ec.result(pass);
        }

        ec.DESC("setValue/getValue: OutOfBoundsEx");

        {
            Point p(20);

            pass = true;

            // Set values
            try {
                for (int i = 0; i < 21; i++)
                    p.setValue(i, 13.43 * i * i + 4.567 * i + 1.234567);
                pass = false;
            } catch (OutOfBoundsEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "OutOfBoundsEx");
            }

            try {
                for (int i = -1; i < 20; i++)
                    p.setValue(i, 13.43 * i * i + 4.567 * i + 1.234567);
                pass = false;
            } catch (OutOfBoundsEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "OutOfBoundsEx");
            }

            // Check values
            try {
                for (int i = 0; i < 21; i++)
                    pass = pass &&
                           (p.getValue(i) == (13.43 * i * i + 4.567 * i + 1.234567)) &&
                           (p[i] == (13.43 * i * i + 4.567 * i + 1.234567));
                pass = false;
            } catch (OutOfBoundsEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "OutOfBoundsEx");
            }

            try {
                for (int i = -1; i < 20; i++)
                    pass = pass &&
                           (p.getValue(i) == (13.43 * i * i + 4.567 * i + 1.234567)) &&
                           (p[i] == (13.43 * i * i + 4.567 * i + 1.234567));
                pass = false;
            } catch (OutOfBoundsEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "OutOfBoundsEx");
            }

            ec.result(pass);
        }

        ec.DESC("operator[]: OutOfBoundsEx");

        {
            Point p(5);

            pass = true;

            // Set values
            try {
                for (int i = 0; i < 6; i++)
                    p[i] = 1000000.43 * i * i + 400000.567 * i + 10000.234567;
                pass = false;
            } catch (OutOfBoundsEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "OutOfBoundsEx");
            }

            // Check values
            try {
                for (int i = 0; i < 6; i++)
                    pass = pass &&
                           (p.getValue(i) == (1000000.43 * i * i + 400000.567 * i + 10000.234567)) &&
                           (p[i] == (1000000.43 * i * i + 400000.567 * i + 10000.234567));
                pass = false;
            } catch (OutOfBoundsEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "OutOfBoundsEx");
            }

            ec.result(pass);
        }
    }
}

// Copy constructor
void test_point_copying(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Point - Copy ---");

    for (int run = 0; run < numRuns; run ++) {

        ec.DESC("simple copy");

        {
            Point p1(50);

            for (int i = 0; i < 50; i ++)
                p1[i] = 44.56 * i * i + 23.45 * i + 12.34;

            Point p2(p1);

            pass = true;
            for (int i = 0; i < 50; i ++)
                pass = pass && (p1[i] == p2[i]);
            ec.result(pass);
        }

        ec.DESC("pass and return by value");

        {
            Point p1(50);

            for (int i = 0; i < 50; i ++)
                p1[i] = 44.56 * i * i + 23.45 * i + 12.34;

            Point p2 = point_in_and_out(p1);

            pass = true;
            for (int i = 0; i < 50; i ++)
                pass = pass && (p1[i] == p2[i]);
            ec.result(pass);
        }
    }
}

// operator=
void test_point_assignment(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Point - Assign ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("simple assignment");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 44.56 * i * i + 23.45 * i + 12.34;

            Point p2 = p1;

            pass = true;
            for (int i = 0; i < 50; i++)
                pass = pass && (p1[i] == p2[i]);
            ec.result(pass);
        }

        ec.DESC("self assignment");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 44.56 * i * i + 23.45 * i + 12.34;

            Point *ptr = &p1;
            p1 = *ptr;

            pass = (ptr == &p1);
            for (int i = 0; i < 50; i++)
                pass = pass && ((*ptr)[i] == p1[i]);

            ec.result(pass);
        }

        ec.DESC("chained assignment");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 44.56 * i * i + 23.45 * i + 12.34;

            Point p2(50), p3(50), p4(50), p5(50);

            p2 = p3 = p4 = p5 = p1;

            pass = true;
            for (int i = 0; i < 50; i++)
                pass = pass && (p1[i] == p2[i]);
            ec.result(pass);
        }

        ec.DESC("operator=: DimensionalityMismatchEx");

        {
            Point p1(50), p2(51);

            pass = true;

            for (int i = 0; i < 50; i++)
                p1[i] = 44.56 * i * i + 23.45 * i + 12.34;

            try {
                p2 = p1;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }
    }
}

// operator==, operator!=
void test_point_equality(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Point - Equal ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("compare equal");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 44.56 * i * i + 23.45 * i + 12.34;

            Point p2(p1);

            pass = (p2 == p1);
            ec.result(pass);
        }


        ec.DESC("ensure operator== is not a dummy");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 44.56 * i * i + 23.45 * i + 12.34;

            Point p2(p1);
            p2[1] = p2[1] + 1.0;

            pass = !(p2 == p1);
            ec.result(pass);
        }

        ec.DESC("compare not equal");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 44.56 * i * i + 23.45 * i + 12.34;

            Point p2(p1);
            p1[49] = p1[49] + 100.0;

            pass = (p2 != p1);
            ec.result(pass);
        }

        ec.DESC("operator==: DimensionalityMismatchEx");

        {
            Point p1(50), p2(51);

            pass = true;

            for (int i = 0; i < 50; i++)
                p1[i] = 44.56 * i * i + 23.45 * i + 12.34;

            try {
                if (p2 == p1)
                    pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }
    }
}

// operator<, operator<=, operator>, operator>=
// (pseudo-lexicographic comparison)
void test_point_comparison(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Point - Compare ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("compare pseudo-lexicographic order");

        {
            Point p1(50), p2(50), p3(50);

            for (int i = 0; i < 50; i ++) {
                p1[i] = i;
                p2[i] = i + 1.0;
                p3[i] = i + 2.0;
            }

            pass = (p1 < p2) &&
                   (p2 < p3) &&
                   (p1 < p3);
            ec.result(pass);
        }

        ec.DESC("less than, one different value, leading");

        {
            Point p1(50), p2(50), p3(50);

            for (int i = 0; i < 50; i ++) {
                p1[i] = p2[i] = p3[i] = i;
            }
            p2[1] = p1[1] + std::numeric_limits<double>::epsilon();
            p3[1] = p2[1] + std::numeric_limits<double>::epsilon();

            pass = (p1 < p2) &&
                   (p2 < p3) &&
                   (p1 < p3);
            ec.result(pass);
        }

        ec.DESC("less than, one different value, middle");

        {
            Point p1(50), p2(50), p3(50);

            for (int i = 0; i < 50; i ++) {
                p1[i] = p2[i] = p3[i] = i;
            }
            p2[30] = p1[30] + 0.00000001;
            p3[30] = p2[30] + 0.00000001;

            pass = (p1 < p2) &&
                   (p2 < p3) &&
                   (p1 < p3);
            ec.result(pass);
        }

        ec.DESC("less than, one different value, trailing");

        {
            Point p1(50), p2(50), p3(50);

            for (int i = 0; i < 50; i ++) {
                p1[i] = p2[i] = p3[i] = i;
            }
            p2[49] = p1[49] + 0.00000001;
            p3[49] = p2[49] + 0.00000001;

            pass = (p1 < p2) &&
                   (p2 < p3) &&
                   (p1 < p3);
            ec.result(pass);
        }

        ec.DESC("less than or equal, equal values");

        {
            Point p1(50), p2(50), p3(50);

            for (int i = 0; i < 50; i ++) {
                p1[i] = p2[i] = p3[i] = i;
            }

            pass = (p1 <= p2) &&
                   (p2 <= p3) &&
                   (p1 <= p3);
            ec.result(pass);
        }

        ec.DESC("less than or equal, one different value, trailing");

        {
            Point p1(50), p2(50), p3(50);

            for (int i = 0; i < 50; i ++) {
                p1[i] = p2[i] = p3[i] = i;
            }
            p2[49] = p1[49] + 0.00000001;
            p3[49] = p2[49] + 0.00000001;

            pass = (p1 <= p2) &&
                   (p2 <= p3) &&
                   (p1 <= p3);
            ec.result(pass);
        }

        ec.DESC("more than or equal, equal values");

        {
            Point p1(50), p2(50), p3(50);

            for (int i = 0; i < 50; i ++) {
                p1[i] = p2[i] = p3[i] = i;
            }

            pass = (p1 >= p2) &&
                   (p2 >= p3) &&
                   (p1 >= p3);
            ec.result(pass);
        }

        ec.DESC("more than or equal, one different value, middle");

        {
            Point p1(50), p2(50), p3(50);

            for (int i = 0; i < 50; i ++) {
                p1[i] = p2[i] = p3[i] = i;
            }

            p2[30] = p3[30] + 0.00000001;
            p1[30] = p2[30] + 0.00000001;

            pass = (p1 >= p2) &&
                   (p2 >= p3) &&
                   (p1 >= p3);
            ec.result(pass);
        }

        ec.DESC("more than, one different value, middle");

        {
            Point p1(50), p2(50), p3(50);

            for (int i = 0; i < 50; i ++) {
                p1[i] = p2[i] = p3[i] = i;
            }

            p2[30] = p3[30] + 0.00000001;
            p1[30] = p2[30] + 0.00000001;

            pass = (p1 > p2) &&
                   (p2 > p3) &&
                   (p1 > p3);
            ec.result(pass);
        }

        ec.DESC("comparison: DimensionalityMismatchEx");

        {
            Point p1(50), p2(51);
            bool comp;

            pass = true;

            try {
                comp = p1 < p2;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "DimensionalityMismatchEx");
            }

            try {
                comp = p1 > p2;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "DimensionalityMismatchEx");
            }

            try {
                comp = p1 <= p2;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "DimensionalityMismatchEx");
            }

            try {
                comp = p1 >= p2;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }
    }
}

// operator+=, operator-=, operator*=, operator/=
void test_point_CAO(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Point - Compound arithmetic ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("plus equals (two points)");

        {
            Point p1(50), p2(50);

            for (int i = 0; i < 50; i++) {
                p1[i] = i;
                p2[i] = i + 1;
            }

            Point p3(50);
            p3 += p1 += p2;
            pass = true;
            for (int i = 0; i < 50; i++) {
                pass = pass && (p3[i] == 2 * i + 1);
            }
            ec.result(pass);
        }

        ec.DESC("operator+=: DimensionalityMismatchEx");

        {
            Point p1(50), p2(51);

            for (int i = 0; i < 50; i++) {
                p1[i] = i;
                p2[i] = i + 1;
            }

            pass = true;

            try{
                p1 += p2;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }

        ec.DESC("minus equals (two points)");

        {
            Point p1(50), p2(50), p3(50);

            for (int i = 0; i < 50; i++) {
                p1[i] = i;
                p2[i] = i + 1;
                p3[i] = 3 * i + 1;
            }

            p3 -= p2 -= p1;
            pass = true;
            for (int i = 0; i < 50; i++) {
                pass = pass && (p3[i] == 3 * i);
            }
            ec.result(pass);
        }


        ec.DESC("operator-=: DimensionalityMismatchEx");

        {
            Point p1(50), p2(51);

            for (int i = 0; i < 50; i++) {
                p1[i] = i;
                p2[i] = i + 1;
            }

            pass = true;

            try{
                p1 -= p2;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }

        ec.DESC("times equals (point and double)");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++) {
                p1[i] = i;
            }

            p1 *= 3.14;

            pass = true;
            for (int i = 0; i < 50; i++) {
                pass = pass && (p1[i] == 3.14 * i);
            }
            ec.result(pass);
        }

        ec.DESC("divide equals (point and double)");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++) {
                p1[i] = 100.0 * i;
            }

            p1 /= 3.14;

            pass = true;
            for (int i = 0; i < 50; i++) {
                pass = pass && (p1[i] == 100.0 * i / 3.14);
            }
            ec.result(pass);
        }
    }
}

// operator+, operator-, operator*, operator/
void test_point_SAO(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Point - Simple arithmetic ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("plus (two points)");

        {
            Point p1(50), p2(50);

            for (int i = 0; i < 50; i++) {
                p1[i] = i;
                p2[i] = i + 1;
            }

            Point p3 = p1 + p2;
            pass = true;
            for (int i = 0; i < 50; i++) {
                pass = pass && (p3[i] == 2 * i + 1);
            }
            ec.result(pass);
        }

        ec.DESC("operator+: DimensionalityMismatchEx");

        {
            Point p1(50), p2(51);

            for (int i = 0; i < 50; i++) {
                p1[i] = i;
                p2[i] = i + 1;
            }

            pass = true;

            try {
                Point p3 = p1 + p2;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }

        ec.DESC("minus (two points)");

        {
            Point p1(50), p2(50);

            for (int i = 0; i < 50; i++) {
                p1[i] = i + 1;
                p2[i] = 2 * i - 1;
            }

            Point p3 = p2 - p1;
            pass = true;
            for (int i = 0; i < 50; i++) {
                pass = pass && (p3[i] == i - 2);
            }
            ec.result(pass);
        }


        ec.DESC("operator-: DimensionalityMismatchEx");

        {
            Point p1(50), p2(51);

            for (int i = 0; i < 50; i++) {
                p1[i] = i;
                p2[i] = i + 1;
            }

            pass = true;

            try {
                Point p3 = p1 - p2;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }

        ec.DESC("times (point and double)");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++) {
                p1[i] = i;
            }

            Point p2 = p1 * 3.14;

            pass = true;
            for (int i = 0; i < 50; i++) {
                pass = pass && (p2[i] == 3.14 * i);
            }
            ec.result(pass);
        }

        ec.DESC("divide (point and double)");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++) {
                p1[i] = 100.0 * i;
            }

            Point p2 = p1 / 3.14;

            pass = true;
            for (int i = 0; i < 50; i++) {
                pass = pass && (p2[i] == 100.0 * i / 3.14);
            }
            ec.result(pass);
        }
    }
}

// distanceTo
void test_point_distance(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Point - Distance ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("same point");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 2.4 * i * i + 1.3 * i + 6.7;

            Point p2(p1);

            pass = (p1.distanceTo(p2) == 0);
            ec.result(pass);
        }

        ec.DESC("5 units away");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = i;

            Point p2(p1);
            p2[1] += 5;

            pass = (p1.distanceTo(p2) == 5);
            if (!pass) std::cout << p1.distanceTo(p2) << " ";
            ec.result(pass);
        }

        // Integer sequence A180442

        ec.DESC("distance 1612 from origin");

        {
            Point p1(169); // 198 - 29

            unsigned int start = 30;
            for (int i = 0; i < 169; i++) {
                p1[i] = start;
                start++;
            }

            Point origin(169); // relies on initialization to zeros

            pass = (p1.distanceTo(origin) == 1612);
            if (!pass) std::cout << p1.distanceTo(origin) << " ";
            ec.result(pass);
        }

        ec.DESC("distance: DimensionalityMismatchEx");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = i;

            Point p2(60);
            p2[1] += 5;

            pass = true;
            try {
                pass = (p1.distanceTo(p2) == 5);
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }

    }
}

// operator>>, operator<< (incl. exceptions)
void test_point_IO(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Point - Stream IO ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("stream between two points");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 6.12 * i * i + 5.17 * i + 4.19;

            Point p2(50);
            std::stringstream iostr;
            iostr << std::setprecision(20) << p1; // Avoid truncation
            iostr >> p2;

            pass = true;
            for (int i = 0; i < 50; i++)
                pass = pass && (p2[i] == p1[i]);
            if (!pass) {
                std::cout << p1 << std::endl;
                std::cout << p2 << std::endl;
            }

            ec.result(pass);
        }

        ec.DESC("operator>>: DimensionalityMismatchEx");

        {
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 6.12 * i * i + 5.17 * i + 4.19;

            Point p2(49);
            std::stringstream iostr;
            iostr << std::setprecision(20) << p1; // Avoid truncation

            try {
                iostr >> p2;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "DimensionalityMismatchEx");
            }

            Point p3(51);
            std::stringstream iostr2;
            iostr2 << std::setprecision(20) << p1; // Avoid truncation

            try {
                iostr2 >> p3;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }
    }
}


// - - - - - - - - - - C L U S T E R - - - - - - - - - -

// Smoketest: constructor, copy constructor, destructor
void test_cluster_smoketest(ErrorContext &ec) {
    bool pass;

    ec.DESC("--- Test - Cluster - Smoketest ---");

    ec.DESC("constructor, destructor");
    pass = true;
    for (int i = 0; i < 10; i ++) {

        Cluster c(30);
    }
    ec.result(pass);


    ec.DESC("size getter - implement if you haven't");
    pass = true;
    for (int i = 0; i < 10; i ++) {

        // Construct a Point
        // At the end of the block, destructor will be called
        Cluster c(30);

        pass = (c.getSize() == 0);
        if (!pass) break;
    }
    ec.result(pass);


    ec.DESC("copy constructor");
    pass = true;
    for (int i = 0; i < 10; i ++) {

        Cluster c1(30), c2(c1);

        pass = (c1 == c2);
        if (!pass) break;
    }
    ec.result(pass);

    ec.DESC("zero dimensions");
    pass = true;
    for (int i = 0; i < 10; i ++) {

        // Construct a Cluster
        // At the end of the block, destructor will be called
        try {
            Cluster c1(0);
            pass = false;
        } catch (ZeroDimensionsEx &ex) {
            std::cerr << "Caught exception: " << ex << std::endl;
            pass = (ex.getName() == "ZeroDimensionsEx");
        }

        if (!pass) break;
    }
    ec.result(pass);

}

// add, remove, operator[]
void test_cluster_addremove(ErrorContext &ec, unsigned int numRuns) { // TODO add operator[]
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Cluster - Add/remove points ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("add and check with size getter");

        {
            Cluster c1(50);
            c1.add(Point(50));
            c1.add(Point(50));
            c1.add(Point(50));

            pass = (c1.getSize() == 3);

            ec.result(pass);
        } // by default, points will get released here


        ec.DESC("add, remove, and check with size getter");

        {
            Point   p1(10),
                    p2(10),
                    p3(10);
            Cluster c1(10);
            c1.add(p1); c1.add(p2); c1.add(p3);
            c1.remove(p1); c1.remove(p2); c1.remove(p3);

            pass = (c1.getSize() == 0);

            ec.result(pass);
        } // by default, points will get released here

        ec.DESC("add, check with cluster equality, remove");

        {
            Point   p1(10),
                    p2(10),
                    p3(10);

            Cluster c1(10), c2(10);
            c1.add(p1); c1.add(p2); c1.add(p3);
            c2.add(p1); c2.add(p2); c2.add(p3);

            pass = (c1 == c2);

            ec.result(pass);
        }

        ec.DESC("check point after add and remove");

        {
            Point   p1(10);

            for (int i = 0; i < 10; i++)
                p1[i] = 5.4 * i * i + 3.4 * i + 1.6;

            Cluster c1(10);
            c1.add(p1);
            Point p2 = c1.remove(p1);

            pass = (p1 == p2);

            ec.result(pass);
        }
    }
}

// Containment
void test_cluster_contain(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Cluster - Containment ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("cluster with one point");

        {
            Point p(10);
            p[0] = p[2] = p[4] = p[8] = 6.705;
            Cluster c(10);
            c.add(p);

            pass = c.contains(p);

            ec.result(pass);
        }

        ec.DESC("contain: DimensionalityMismatchEx");

        {
            Point p(10);
            p[0] = p[2] = p[4] = p[8] = 6.705;
            Cluster c(10);
            c.add(p);

            Point p1(12);

            pass = true;
            try {
                pass = c.contains(p1);
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }

        ec.DESC("cluster with several points");

        {
            Point p(10);
            p[0] = p[2] = p[4] = p[8] = 6.705;
            Cluster c(10);

            for (int i = 0; i < 10; i ++) {
                Point pp(10);
                for (int j = 0; j < 10; j ++) {
                    pp[i] = 3.4 + i * 2.1 + i * i;
                }
                c.add(pp);
            }
            c.add(p);

            pass = c.contains(p);

            ec.result(pass);
        }
    }
}

// Inner class Move
void test_cluster_move(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Cluster - Move ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("move 1 point across");

        {
            Cluster c1(100), c2(100);
            Point p(100);
            c1.add(p);

            pass = c1.contains(p);

            Cluster::Move move(p, c1, c2);
            move.perform();

            pass = pass && (! c1.contains(p)) && c2.contains(p);

            ec.result(pass);
        }

        ec.DESC("move 1 point back and forth");

        {
            Cluster c1(100), c2(100);
            Point p(100);
            c1.add(p);

            pass = c1.contains(p);

            Cluster::Move move1(p, c1, c2);
            move1.perform();

            pass = pass && (! c1.contains(p)) && c2.contains(p);

            Cluster::Move move2(p, c2, c1);
            move2.perform();

            pass = pass && c1.contains(p) && (! c2.contains(p));

            ec.result(pass);
        }
    }
}

// Copy constructor
void test_cluster_copying(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Cluster - Copy ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("simple copy");

        {
            Point   p1(10),
                    p2(10),
                    p3(10);
            Cluster c1(10);
            c1.add(p1); c1.add(p2); c1.add(p3);
            Cluster c2(c1);

            pass = (c1 == c2);

            ec.result(pass);
        }

        ec.DESC("chained copy");

        {
            Point   p1(10),
                    p2(10),
                    p3(10);
            Cluster c1(10);
            c1.add(p1); c1.add(p2); c1.add(p3);
            Cluster c2(c1), c3(c2), c4(c3);

            pass = (c1 == c4);

            ec.result(pass);
        }
    }
}

// operator=
void test_cluster_assignment(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Cluster - Assign ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("simple assignment");

        {
            Point   p1(10),
                    p2(10),
                    p3(10);

            Cluster c1(10);
            c1.add(p1); c1.add(p2); c1.add(p3);
            Cluster c2 = c1;

            pass = (c1 == c2);

            ec.result(pass);
        }


        ec.DESC("assignment: DimensionalityMismatchEx");

        {
            Point   p1(10),
                    p2(10),
                    p3(10);

            Cluster c1(10);
            c1.add(p1); c1.add(p2); c1.add(p3);

            Cluster c2(15);

            pass = true;
            try {
                c2 = c1;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }


        ec.DESC("assignment causing deletion");

        {
            Point   p1(10),
                    p2(10),
                    p3(10);

            Cluster c1(10);
            c1.add(p1); c1.add(p2); c1.add(p3);
            Cluster c2(10);
            // add some other points
            c2.add(Point(10));
            c2.add(Point(10));
            c2.add(Point(10));

            c2 = c1;

            pass = (c1 == c2);

            ec.result(pass);
        }

        ec.DESC("chained assignment");

        {
            Point   p1(10),
                    p2(10),
                    p3(10);

            Cluster c1(10);
            c1.add(p1); c1.add(p2); c1.add(p3);
            Cluster c2 = c1;
            Cluster c3 = c2;
            Cluster c4 = c3;

            pass = (c1 == c4);

            ec.result(pass);
        }
    }
}

// subscript (operator[])
void test_cluster_subscript(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Cluster - Subscript ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("cluster with one point");

        {
            Cluster c(10);
            Point p(10);
            p[5] = 3.14;

            c.add(p);
            Point p1 = c[0];

            pass = p1[5] == 3.14;

            ec.result(pass);
        }

        ec.DESC("cluster with several points");

        {
            Cluster c(10);
            for (int i = 0; i < 10; i ++) {
                Point p(10);
                p[5] = 3.14;
                c.add(p);
            }

            pass = true;
            for (int i = 0; i < 10; i ++) {
                Point p1 = c[i];
                pass = pass && (p1[5] == 3.14);
            }

            ec.result(pass);
        }

        ec.DESC("const operator[]");

        {
            Cluster c(100);
            for (int i = 0; i < 200; i++)
                c.add(Point(100));

            pass = (c.getSize() == 200);

            for (int i = 0; i < 200; i++) {
                Point tempPt = c[i];
            }

            ec.result(pass);
        }

        ec.DESC("check no non-const operator[] (compile time)");

        {
            Cluster c(100);
            for (int i = 0; i < 200; i++)
                c.add(Point(100));

            pass = true;

            for (int i = 0; i < 200; i++) {
//                c[i] = Point(100); // c[i] read-only and not assignable
            }

            ec.result(pass);
        }

        ec.DESC("operator[]: OutOfBoundsEx");

        {
            Cluster c(100);
            for (int i = 0; i < 200; i++)
                c.add(Point(100));

            pass = true;

            try {
                for (int i = 0; i < 201; i++) {
                    Point p = c[i];
                }
                pass = false;
            } catch (OutOfBoundsEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "OutOfBoundsEx");
            }

            ec.result(pass);
        }

        ec.DESC("operator[]: EmptyClusterEx");

        {
            Cluster c(100);

            pass = true;

            try {
                for (int i = 0; i < 10; i++) {
                    Point p = c[i];
                }
                pass = false;
            } catch (EmptyClusterEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = pass && (ex.getName() == "EmptyClusterEx");
            }

            ec.result(pass);
        }
    }
}

// operator==, operator!=
void test_cluster_equality(ErrorContext &ec, unsigned int numRuns) { // TODO add opeartor!=
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Cluster - Equal ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("check operator== is not a dummy");

        {
            // The requirements don't provide for many other methods that
            // can be used for testing, so operator== is checked first
            Cluster c1(100), c2(100);
            c1.add(Point(100));

            pass = !(c1 == c2);

            ec.result(pass);
        }

        ec.DESC("check inequality");

        {
            // The requirements don't provide for many other methods that
            // can be used for testing, so operator== is checked first
            Cluster c1(100), c2(100);
            c1.add(Point(100));

            pass = (c1 != c2);

            ec.result(pass);
        }

        ec.DESC("operator!=: DimensionalityMismatchEx");

        {
            Cluster c1(100), c2(120);
            c1.add(Point(100));

            pass = true;
            try {
                pass = (c1 != c2);
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }
    }
}

// ascending pseudo-lexicographic order
void test_cluster_order(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Cluster - Order ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("points in a cluster are sorted");

        {
            Point p1(5), p2(5), p3(5), p4(5), p5(5);
            p1[0] = 1;
            p2[1] = 1;
            p3[2] = 1;
            p4[3] = 1;
            p5[4] = 1;

            Cluster c(5);
            c.add(p1);
            c.add(p2);
            c.add(p4);
            c.add(p3);
            c.add(p5);

            pass = (c[0] == p5)
                   && (c[1] == p4)
                   && (c[2] == p3)
                   && (c[3] == p2)
                   && (c[4] == p1);

            if (!pass) {
                std::cout << std::endl;
                std::cout << c << std::endl;
                std::cout << std::endl;
            }

            ec.result(pass);
        }

        ec.DESC("ascending pseudo-lexicographic order");

        {
            Point p1(5), p2(5), p3(5), p4(5), p5(5);
            p1[0] = 1;
            p2[1] = 1;
            p3[2] = -1;
            p4[3] = 1;
            p5[4] = -1;

            Cluster c(5);
            c.add(p1);
            c.add(p2);
            c.add(p4);
            c.add(p3);
            c.add(p5);

            pass = (c[0] == p3)
                   && (c[1] == p5)
                   && (c[2] == p4)
                   && (c[3] == p2)
                   && (c[4] == p1);

            if (!pass) {
                std::cout << std::endl;
                std::cout << c << std::endl;
                std::cout << std::endl;
            }

            ec.result(pass);
        }
    }
}

// operator+=, operator-=, different rhs
void test_cluster_CAO(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Cluster - Compound arithmetic ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("plus equals (Cluster and Point) check with non-equality");

        {
            Cluster c1(50), c2(50);
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 6.75 * i * i + 5.45 * i + 1.15;

            c1 += p1;

            pass = !(c1 == c2);

            ec.result(pass);
        }

        ec.DESC("plus equals (Cluster and Point) check with size getter");

        {
            Cluster c1(50);
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 6.75 * i * i + 5.45 * i + 1.15;

            c1 += p1;

            pass = (c1.getSize() == 1);

            ec.result(pass);
        }

        ec.DESC("plus equals point: DimensionalityMismatchEx");

        {
            Cluster c1(50);
            Point p1(60);

            for (int i = 0; i < 60; i++)
                p1[i] = 6.75 * i * i + 5.45 * i + 1.15;

            pass = true;
            try {
                c1 += p1;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }

        ec.DESC("minus equals (Cluster and Point) check with non-equality");

        {
            Cluster c1(50), c2(50);
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 6.75 * i * i + 5.45 * i + 1.15;

            c1 += p1;
            pass = !(c1 == c2);

            c1 -= p1;
            pass = (c1 == c2);

            ec.result(pass);
        }

        ec.DESC("minus equals (Cluster and Point) check with size getter");

        {
            Cluster c1(50);
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 6.75 * i * i + 5.45 * i + 1.15;

            c1 += p1;
            pass = (c1.getSize() == 1);

            c1 -= p1;
            pass = (c1.getSize() == 0);

            ec.result(pass);
        }

        ec.DESC("minus equals point: DimensionalityMismatchEx");

        {
            Cluster c1(50);
            Point p1(60);

            for (int i = 0; i < 60; i++)
                p1[i] = 6.75 * i * i + 5.45 * i + 1.15;

            pass = true;
            try {
                c1 -= p1;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }

        ec.DESC("plus equals (Cluster union) no common points");

        {
            Cluster c1(50), c2(50), c3(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50),
                    p5(50);
            c1.add(p1); c1.add(p2); c1.add(p3);
            c2.add(p4); c2.add(p5);

            // create a union to compare to
            c3.add(p1); c3.add(p2); c3.add(p3);
            c3.add(p4); c3.add(p5);

            c1 += c2;

            pass = (c1 == c3);

            ec.result(pass);
        }

        ec.DESC("plus equals (Cluster union) one common point");

        {
            Cluster c1(50), c2(50), c3(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50),
                    p5(50);
            c1.add(p1); c1.add(p2); c1.add(p3);
            c2.add(p3); c2.add(p4); c2.add(p5);

            // create a union to compare to
            c3.add(p1); c3.add(p2); c3.add(p3);
            c3.add(p4); c3.add(p5);

            c1 += c2;

            pass = (c1 == c3);

            ec.result(pass);
        }

        ec.DESC("plus equals (Cluster union) two equal clusters");

        {
            Cluster c1(50), c2(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50),
                    p5(50);
            c1.add(p1); c1.add(p2); c1.add(p3); c1.add(p4); c1.add(p5);
            c2.add(p1); c2.add(p2); c2.add(p3); c2.add(p4); c2.add(p5);

            c1 += c2;

            pass = (c1 == c2);

            ec.result(pass);
        }

        ec.DESC("plus equals cluster: DimensionalityMismatchEx");

        {
            Cluster c1(50);
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 6.75 * i * i + 5.45 * i + 1.15;
            c1 += p1;

            Cluster c2(25);
            Point p2(25);

            for (int i = 0; i < 25; i++)
                p2[i] = 6.75 * i * i + 5.45 * i + 1.15;
            c2 += p2;

            pass = true;
            try {
                c1 += c2;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }

        ec.DESC("minus equals (asymmetric Cluster difference) no common points");

        {
            Cluster c1(50), c2(50), c3(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50),
                    p5(50);
            c1.add(p1); c1.add(p2); c1.add(p3);
            c2.add(p4); c2.add(p5);

            c3 = c1;
            c1 -= c2;

            pass = (c1 == c3);

            ec.result(pass);
        }

        ec.DESC("minus equals (asymmetric Cluster difference) one common point");

        {
            Cluster c1(50), c2(50), c3(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50),
                    p5(50);
            c1.add(p1); c1.add(p2); c1.add(p3);
            c2.add(p3); c2.add(p4); c2.add(p5);

            // Prepare a difference to compare to
            c3.add(p1); c3.add(p2);
            c1 -= c2;

            pass = (c1 == c3);

            ec.result(pass);
        }

        ec.DESC("minus equals (asymmetric Cluster difference) two equal clusters");

        {
            Cluster c1(50), c2(50), c3(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50),
                    p5(50);
            c1.add(p1); c1.add(p2); c1.add(p3); c1.add(p4); c1.add(p5);
            c2.add(p1); c2.add(p2); c2.add(p3); c2.add(p4); c2.add(p5);

            c1 -= c2;

            pass = (c1 == c3); // c1 should be empty

            ec.result(pass);
        }

        ec.DESC("minus equals cluster: DimensionalityMismatchEx");

        {
            Cluster c1(50);
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 6.75 * i * i + 5.45 * i + 1.15;
            c1 += p1;

            Cluster c2(25);
            Point p2(25);

            for (int i = 0; i < 25; i++)
                p2[i] = 6.75 * i * i + 5.45 * i + 1.15;
            c2 += p2;

            pass = true;
            try {
                c1 -= c2;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }

    }
}

// operator+, operator-, different rhs
void test_cluster_SAO(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Cluster - Simple arithmetic ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("plus (Cluster and Point)");

        {
            Cluster c1(50), c2(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50);
            c1.add(p1); c1.add(p2); c1.add(p3);

            c2 = c1; c2.add(p4);
            Cluster c3 = c1 + p4;

            pass = (c3 == c2);

            ec.result(pass);

        }

        ec.DESC("plus point: DimensionalityMismatchEx");

        {
            Cluster c1(50);
            Point p1(60);

            for (int i = 0; i < 60; i++)
                p1[i] = 6.75 * i * i + 5.45 * i + 1.15;

            pass = true;
            try {
                Cluster c2 = c1 + p1;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }

        ec.DESC("minus (Cluster and Point)");

        {
            Cluster c1(50), c2(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50);
            c1.add(p1); c1.add(p2); c1.add(p3); c1.add(p4);

            c2 = c1; c2.remove(p4);
            Cluster c3 = c1 - p4;

            pass = (c3 == c2);

            ec.result(pass);
        }

        ec.DESC("plus point: DimensionalityMismatchEx");

        {
            Cluster c1(50);
            Point p1(60);

            for (int i = 0; i < 60; i++)
                p1[i] = 6.75 * i * i + 5.45 * i + 1.15;

            pass = true;
            try {
                Cluster c2 = c1 - p1;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }

        ec.DESC("plus (Cluster union)");

        {
            Cluster c1(50), c2(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50),
                    p5(50);
            c1.add(p1); c1.add(p2); c1.add(p3);
            c2.add(p4); c2.add(p5);

            Cluster c3(50);
            c3.add(p1); c3.add(p2); c3.add(p3); c3.add(p4); c3.add(p5);

            Cluster c4 = c1 + c2;

            pass = (c4 == c3);

            ec.result(pass);
        }

        ec.DESC("plus equals cluster: DimensionalityMismatchEx");

        {
            Cluster c1(50);
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 6.75 * i * i + 5.45 * i + 1.15;
            c1 += p1;

            Cluster c2(25);
            Point p2(25);

            for (int i = 0; i < 25; i++)
                p2[i] = 6.75 * i * i + 5.45 * i + 1.15;
            c2 += p2;

            pass = true;
            try {
                Cluster c3 = c1 + c2;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }

        ec.DESC("minus (Cluster difference)");

        {
            Cluster c1(50), c2(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50),
                    p5(50);
            c1.add(p1); c1.add(p2); c1.add(p3);
            c2.add(p3); c2.add(p4); c2.add(p5);

            Cluster c3(50);
            c3.add(p1); c3.add(p2);

            Cluster c4 = c1 - c2;

            pass = (c4 == c3);

            ec.result(pass);
        }

        ec.DESC("plus equals cluster: DimensionalityMismatchEx");

        {
            Cluster c1(50);
            Point p1(50);

            for (int i = 0; i < 50; i++)
                p1[i] = 6.75 * i * i + 5.45 * i + 1.15;
            c1 += p1;

            Cluster c2(25);
            Point p2(25);

            for (int i = 0; i < 25; i++)
                p2[i] = 6.75 * i * i + 5.45 * i + 1.15;
            c2 += p2;

            pass = true;
            try {
                Cluster c3 = c1 - c2;
                pass = false;
            } catch (DimensionalityMismatchEx &ex) {
                std::cerr << "Caught exception: " << ex << std::endl;
                pass = (ex.getName() == "DimensionalityMismatchEx");
            }

            ec.result(pass);
        }
    }
}

// Centroid
void test_cluster_centroid(ErrorContext &ec, unsigned int numRuns) { // TODO implement
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Cluster - Centroid ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("new empty cluster has invalid centroid");

        {
            Cluster c(10);

            pass = !c.centroid.isValid();

            ec.result(pass);
        }

        ec.DESC("centroid set/get");

        {
            // test centroid after set, get
            Cluster c(10);

            Point p(10);
            for (int i = 0; i < 10; i++)
                p[i] = 4.5 * i * i + 3.4 * i + 5.6;

            c.centroid.set(p);

            pass = c.centroid.equal(p) && c.centroid.isValid();

            ec.result(pass);
        }

        ec.DESC("centroid compute (+ invalidation on add/remove)");

        {
            // test centroid compute
            Cluster c(10);

            Point p1(10);
            for (int i = 0; i < 10; i++) p1[i] = 1;
            c.add(p1);
            Point p2(10);
            for (int i = 0; i < 10; i++) p2[i] = 2;
            c.add(p2);
            Point p3(10);
            for (int i = 0; i < 10; i++) p3[i] = 3;
            c.add(p3);

            pass = (!c.centroid.isValid()); // test invalid here

            c.centroid.compute();

            pass = pass && c.centroid.isValid(); // test valid here

            Point cent = c.centroid.get();

            for (int i = 0; i < 10; i++)
                pass = pass && (cent[i] == 2); // test compute

            c.remove(p1);
            c.remove(p2);
            c.remove(p3);

            pass = pass && (!c.centroid.isValid()); // test invalid here

            ec.result(pass);
        }

        ec.DESC("operator+=(const Point &rhs), operator-=(const Point &rhs)");

        {
            Cluster c(10); // empty cluster
            c.centroid.compute(); // this will be infinity, but valid
            Point inf = c.centroid.get();

            pass = c.centroid.isValid();

            Point p(10);
            for (int i = 0; i < 10; i++) p[i] = 1.5;

            c += p;

            pass = pass && (! c.centroid.isValid());

            c.centroid.compute();

            pass = pass && c.centroid.isValid();

            pass = pass && c.centroid.equal(p);

            c -= p;

            pass = pass && (! c.centroid.isValid());

            c.centroid.compute();

            pass = pass && c.centroid.equal(inf);

            ec.result(pass);
        }

        ec.DESC("operator+= (Cluster union) no common points");

        {
            Cluster c1(50), c2(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50),
                    p5(50);
            c1.add(p1); c1.add(p2); c1.add(p3);
            c2.add(p4); c2.add(p5);

            pass = (! c1.centroid.isValid()) && (! c2.centroid.isValid());

            c1.centroid.compute(); c2.centroid.compute();

            c1 += c2;

            // only the lhs centroid should be invalidated
            pass = pass && (! c1.centroid.isValid()) && c2.centroid.isValid();

            c1.centroid.compute();

            pass = pass && c1.centroid.isValid() && c2.centroid.isValid();

            ec.result(pass);
        }

        ec.DESC("operator+= (Cluster union) all common points");

        {
            Cluster c1(50), c2(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50),
                    p5(50);
            c1.add(p1); c1.add(p2); c1.add(p3); c1.add(p4); c1.add(p5);
            c2.add(p1); c2.add(p2); c2.add(p3); c2.add(p4); c2.add(p5);

            pass = (! c1.centroid.isValid()) && (! c2.centroid.isValid());

            c1.centroid.compute(); c2.centroid.compute();

            c1 += c2;

            // neither centroid should be invalidated - no points added
            pass = pass && c1.centroid.isValid() && c2.centroid.isValid();

            ec.result(pass);
        }

        ec.DESC("operator-= (Cluster difference) no common points");
        {
            Cluster c1(50), c2(50), c3(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50),
                    p5(50);
            c1.add(p1); c1.add(p2); c1.add(p3);
            c2.add(p4); c2.add(p5);

            c1.centroid.compute(); c2.centroid.compute();

            c3 = c1;

            c3.centroid.compute();

            c1 -= c2;

            pass = c1.centroid.isValid() && c2.centroid.isValid() &&
                    (c1.centroid.equal(c3.centroid.get()));

            ec.result(pass);
        }

        ec.DESC("operator-= (asymmetric Cluster difference) one common point");

        {
            Cluster c1(50), c2(50), c3(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50),
                    p5(50);
            c1.add(p1); c1.add(p2); c1.add(p3);
            c2.add(p3); c2.add(p4); c2.add(p5);

            c1.centroid.compute(); c2.centroid.compute();

            // Prepare a difference to compare to
            c3.add(p1); c3.add(p2);

            c3.centroid.compute();

            c1 -= c2;

            pass = (! c1.centroid.isValid()) && c2.centroid.isValid();

            c1.centroid.compute();

            pass = pass && (c1.centroid.equal(c3.centroid.get()));

            ec.result(pass);
        }

        ec.DESC("operator-= (asymmetric Cluster difference) two equal clusters");

        {
            Cluster c1(50), c2(50), c3(50);
            Point   p1(50),
                    p2(50),
                    p3(50),
                    p4(50),
                    p5(50);

            for (int i = 0; i < 50; i++) p1[i] = 2.5;
            for (int i = 0; i < 50; i++) p2[i] = 50.4 + i * 2.3;
            for (int i = 0; i < 50; i++) p3[i] = 6.4 * i * i + i * 2.3;
            for (int i = 0; i < 50; i++) p4[i] = i * 2.003 + 56.98;
            for (int i = 0; i < 50; i++) p5[i] = i + 12.3;

            c1.add(p1); c1.add(p2); c1.add(p3); c1.add(p4); c1.add(p5);
            c2.add(p1); c2.add(p2); c2.add(p3); c2.add(p4); c2.add(p5);

            c1.centroid.compute(); c2.centroid.compute();

            pass = (c1.centroid.equal(c2.centroid.get()));

            c1 -= c2;

            pass = pass && (! c1.centroid.isValid()) && c2.centroid.isValid();

            Point inf(50);
            for (int i = 0; i < 50; i++) inf[i] = std::numeric_limits<double>::max();

            c1.centroid.compute();

            pass = pass && (c1.centroid.equal(inf));

            ec.result(pass);
        }
    }
}

// Id
void test_cluster_id(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Cluster - Id ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("sequential id-s");

        {
            Cluster *c[10];

            for (int i=0; i<10; i++) c[i] = new Cluster(100);

            pass = true;
            for (int i=0; i<10; i++)
                pass = pass && ((c[i]->getId() - c[0]->getId()) == i);

            // cleanup
            for (int i=0; i<10; i++) delete c[i];

            ec.result(pass);
        }

        ec.DESC("no id generation on copy and assignment");

        {
            Cluster c1(10), c2(c1), c3 = c1;

            pass = (c1.getId() == c2.getId()) && (c1.getId() == c3.getId());

            Cluster c4(10);

            c4 = c3;

            pass = pass && (c4.getId() == c3.getId());

            ec.result(pass);
        }
    }
}

// Init element selection ("pickCentroids")
void test_cluster_initselection(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Cluster - Select initial centroids ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("k == size of Cluster");

        {
            unsigned int k = 10;
            Cluster c(20);
            for (int i = 0; i < k; i++) {
                Point p(20);
                for (int j = 0; j < 20; j++)
                    p[j] = 2.3 * i * i + 1.45 * j + 5.67;
                c.add(p);
            }
            Point **pointArray = new Point*[k];
            for (int i=0; i<k; i++) pointArray[i] = new Point(20);
            c.pickCentroids(k, pointArray);

            pass = true;

            // all Cluster points should be returned as centroids
            for (int i = 0; i < k; i++) {
                pass = pass && (*pointArray[i] == c[i]);
            }

            // clean up
            for (int i=0; i<k; i++) delete pointArray[i];
            delete [] pointArray;

            ec.result(pass);
        }

        ec.DESC("k > size of Cluster");

        {
            unsigned int k = 10;
            Cluster c(20);
            for (int i = 0; i < k/2; i++) {
                Point p(20);
                for (int j = 0; j < 20; j++)
                    p[j] = 2.3 * i * i + 1.45 * j + 5.67;
                c.add(p);
            }

            Point **pointArray = new Point*[k];
            for (int i=0; i<k; i++) pointArray[i] = new Point(20);
            c.pickCentroids(k, pointArray);

            pass = true;

            // all Cluster points should be returned as centroids
            int i = 0;
            for ( ; i < k/2; i++) {
                pass = pass && (*pointArray[i] == c[i]);
                delete pointArray[i];
            }

            Point inf(20);
            for (int j = 0; j < 20; j++) inf[j] = std::numeric_limits<double>::max();
            for ( ; i < k; i++) {
                for (int ii=0; ii<20; ii++) { // note: id's won't match so compare value-by-value
                    Point p = *pointArray[i];
                    pass = pass && (p[ii] == inf[ii]);
                }
                delete pointArray[i];
            }

            // clean up
            delete [] pointArray;

            ec.result(pass);
        }

        ec.DESC("k < size of Cluster");

        {
            unsigned int k = 3;
            Cluster c(20);
            for (int i = 0; i < 50; i++) {
                Point p(20);
                for (int j = 0; j < 20; j++)
                    p[j] = 2.3 * i * i + 1.45 * j + 5.67;
                c.add(p);
            }
            Point **pointArray = new Point*[k];
            for (int i=0; i<k; i++) pointArray[i] = new Point(20);
            c.pickCentroids(k, pointArray);

            pass = true;

            // test if pointArray was assigned with points from the Cluster
            Point empty(20);
            for (int i = 0; i < k; i++) {
                pass = pass && (*pointArray[i] != empty) && c.contains(*pointArray[i]);
                delete pointArray[i];
            }

            // clean up
            delete [] pointArray;

            ec.result(pass);
        }

//        ec.DESC("k=13 < size=15000 of Cluster - COMMENTED OUT");
//        pass = true;
        ec.DESC("k=13 << Cluster size=15000");

        {
            unsigned int k = 13;
            Cluster c(20);
            for (int i = 0; i < 15000; i++) {
                Point p(20);
                for (int j = 0; j < 20; j++)
                    p[j] = 2.3 * i * i + 1.45 * j + 5.67;
                c.add(p);
            }
            Point **pointArray = new Point*[k];
            for (int i=0; i<k; i++) pointArray[i] = new Point(20);
            c.pickCentroids(k, pointArray);

            pass = true;

            // test if pointArray was assigned with pointers from the Cluster
            for (int i = 0; i < k; i++) {
                pass = pass && (pointArray[i] != nullptr) && c.contains(*pointArray[i]);
                delete pointArray[i];
            }

            // clean up
            delete [] pointArray;

            ec.result(pass);
        }
//        ec.result(pass);
    }
}

// operator>>, operator<<
void test_cluster_IO(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - Cluster - Stream IO ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("read from a file");

        {
            std::ifstream csv("points4.csv");
            Cluster c(5);
            if (csv.is_open()) {
                csv >> c;
                csv.close();
            }
            pass = (c.getSize() == 4);

            ec.result(pass);
        }

        ec.DESC("read, write, and read again");

        {
            std::ifstream csv("points4.csv");
            Cluster c(5);
            if (csv.is_open()) {
                csv >> c;
                csv.close();
            }
            pass = (c.getSize() == 4);

            // add a point
            c.add(Point(5));

            std::ofstream csv1("points4_1.csv", std::ofstream::out);
            csv1 << c;
            csv1.close();

            std::ifstream csv2("points4_1.csv");
            int size = 0;
            std::string line;
            if (csv2.is_open()) {
                while (getline(csv2, line)) size ++;
                csv2.close();
            }
            pass = pass && (size == 5);
            if (! pass) std::cout << "size = " << size;

            ec.result(pass);
        }

        ec.DESC("check element order");

        {
            std::ifstream csv("points4.csv");
            Cluster c(5);
            if (csv.is_open()) {
                csv >> c;
                csv.close();
            }

            pass = (c[0] > c[1] && c[1] > c[2] && c[2] > c[3]) ||
                   (c[0] < c[1] && c[1] < c[2] && c[2] < c[3]);

            ec.result(pass);
        }
        ec.DESC("read from a file: 2 caught exceptions");

        {
            std::ifstream csv("points4_ex.csv");
            Cluster c(5);

            if (csv.is_open()) {
                csv >> c;
                csv.close();

                pass = (c.getSize() == 2) &&
                       (c[0].getId() + 1 == c[1].getId()); // two points didn't load, but no id-s wasted

                if (! pass)
                    std::cout << c[0].getId() << ' ' << c[1].getId() << std::endl;
            } else {
                pass = false;
            }

            ec.result(pass);
        }
    }
}


//
// - - - - - - - - - - K M E A N S - - - - - - - - - -

// Smoketest: constructor, destructor, loading points
void test_kmeans_smoketest(ErrorContext &ec) {
    bool pass;

    ec.DESC("--- Test - KMeans - Smoketest ---");

    ec.DESC("constructor, destructor");

    pass = true;
    for (int i = 0; i < 10; i ++) {

        KMeans kmeans(5, 3, "points4.csv", 20);

    }
    ec.result(pass);

    ec.DESC("empty filename");

    pass = true;
    for (int i = 0; i < 10; i ++) {

        try {
            KMeans kmeans(5, 3, "", 20);
            pass = false;
        } catch (DataFileOpenEx &ex) {
            std::cerr << "Caught exception: " << ex << std::endl;
            pass = (ex.getName() == "DataFileOpenEx");
        }

    }
    ec.result(pass);

    ec.DESC("non-existent filename");

    pass = true;
    for (int i = 0; i < 10; i ++) {

        try {
            KMeans kmeans(5, 3, "no_such_file.csv", 20);
            pass = false;
        } catch (DataFileOpenEx &ex) {
            std::cerr << "Caught exception: " << ex << std::endl;
            pass = (ex.getName() == "DataFileOpenEx");
        }

    }
    ec.result(pass);

    ec.DESC("k = 0");

    pass = true;
    for (int i = 0; i < 10; i ++) {

        try {
            KMeans kmeans(5, 0, "points4.csv", 20);
            pass = false;
        } catch (ZeroClustersEx &ex) {
            std::cerr << "Caught exception: " << ex << std::endl;
            pass = (ex.getName() == "ZeroClustersEx");
        }

    }
    ec.result(pass);


    ec.DESC("zero dimensions");
    pass = true;
    for (int i = 0; i < 10; i ++) {

        // Construct a Point
        // At the end of the block, destructor will be called
        try {
            KMeans kmeans(0, 3, "points4.csv", 20);
            pass = false;
        } catch (ZeroDimensionsEx &ex) {
            std::cerr << "Caught exception: " << ex << std::endl;
            pass = (ex.getName() == "ZeroDimensionsEx");
        }

        if (!pass) break;
    }
    ec.result(pass);

    ec.DESC("loading small data file");

    pass = true;
    for (int i = 0; i < 10; i ++) {

        KMeans kmeans(5, 3, "points4.csv", 20);

        pass = pass && (kmeans[0].getSize() == 4);
        if (! pass) break;

    }
    ec.result(pass);

    ec.DESC("no copy, no assignment");

    pass = true;
    for (int i = 0; i < 10; i ++) {

        KMeans kmeans(5, 3, "points4.csv", 20);

//        KMeans kmeans1(kmeans); // deleted copy constructor
//        KMeans kmeans1 = kmeans; // deleted copy constructor

        KMeans kmeans1(5, 5, "points4.csv", 20);

//        kmeans1 = kmeans; // deleted operator '='
    }
    ec.result(pass);

    ec.DESC("loading medium data file");

    pass = true;
    for (int i = 0; i < 10; i ++) {

        KMeans kmeans(3, 3, "points2499.csv", 20);

        pass = pass && (kmeans[0].getSize() == 2499);
        if (! pass) break;

    }
    ec.result(pass);

//    ec.DESC("loading large data file - COMMENTED OUT");
//    pass = true;
    ec.DESC("loading large data file (RUNS LONG)");

    pass = true;
    for (int i = 0; i < 10; i ++) {

        KMeans kmeans(50, 3, "points25000.csv", 20);

        pass = pass && (kmeans[0].getSize() == 25000);
        if (! pass) break;

    }
    ec.result(pass);
}

// running
void test_kmeans_run(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - KMeans - Run ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("4 points, 4 clusters");

        {
            KMeans kmeans(5, 4, "points4.csv", 20);

            kmeans.run();

            // The points should end up each in its own cluster
            pass =  (kmeans.getNumNonemptyClusters() == 4) &&
                    (kmeans[0].getSize() == 1) &&
                    (kmeans[1].getSize() == 1) &&
                    (kmeans[2].getSize() == 1) &&
                    (kmeans[3].getSize() == 1);

            ec.result(pass);
        }

        ec.DESC("4 points, 4 clusters (iterations & moves)");

        {
            unsigned int iter = 20, prevIter = 0;

            pass = true;
            for (unsigned int i=1; i<=iter; ++i) {
                KMeans kmeans(5, 4, "points4.csv", i);

                kmeans.run();

                if (kmeans.getNumMovesLastIter() > 0) {
                    pass = pass && (i > prevIter);
                    prevIter = i;
                    pass = pass &&
                           !((kmeans.getNumNonemptyClusters() == 4) &&
                             (kmeans[0].getSize() == 1) &&
                             (kmeans[1].getSize() == 1) &&
                             (kmeans[2].getSize() == 1) &&
                             (kmeans[3].getSize() == 1));
                } else {

                    pass = (kmeans.getNumNonemptyClusters() == 4) &&
                           (kmeans[0].getSize() == 1) &&
                           (kmeans[1].getSize() == 1) &&
                           (kmeans[2].getSize() == 1) &&
                           (kmeans[3].getSize() == 1);
                    break;
                }
            }

            ec.result(pass);
        }

        ec.DESC("2499 points, k=1..15");

        {
            pass = true;

            for (unsigned int k=1; k<16; ++k) {
                std::cout << "k = " << k << std::endl;

                KMeans kmeans(3, k, "points2499.csv", 20);
                kmeans.run();
                pass = pass && (kmeans.getNumNonemptyClusters() == k);
            }

            ec.result(pass);
        }
    }
}

// operator<<
void test_kmeans_IO(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - KMeans - IO ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("write out to a file, 4 points, 4 clusters");

        {
            KMeans kmeans(5, 4, "points4.csv", 20);

            kmeans.run();

            // The points should end up each in its own cluster
            pass =  (kmeans.getNumNonemptyClusters() == 4) &&
                    (kmeans[0].getSize() == 1) &&
                    (kmeans[1].getSize() == 1) &&
                    (kmeans[2].getSize() == 1) &&
                    (kmeans[3].getSize() == 1);

            std::ofstream csv1("points4_2.csv", std::ofstream::out);
            csv1 << kmeans;
            csv1.close();

            ClusterCounts cc("points4_2.csv");
            pass = pass && cc.parse_and_analyze() &&
                   (cc.get_num_clusters() == 4) &&
                   (cc.get_num_points() == 4);

            ec.result(pass);
        }

//        ec.DESC("write out to a file, 2499 points, 6 clusters - COMMENTED OUT");
//        pass = true;
        ec.DESC("write out to a file, 2499 points, 6 clusters (RUNS LONG)");

        {
            KMeans kmeans(3, 6, "points2499.csv", 20);

            kmeans.run();

            pass = (kmeans.getNumNonemptyClusters() == 6);

            std::ofstream csv1("points2499_2.csv", std::ofstream::out);
            csv1 << kmeans;
            csv1.close();

            ClusterCounts cc("points2499_2.csv");
            pass = pass && cc.parse_and_analyze() &&
                   (cc.get_num_clusters() == 6) &&
                   (cc.get_num_points() == 2499);

            ec.result(pass);
        }
//        ec.result(pass);
    }
}

// K larger than number of points
void test_kmeans_toofewpoints(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - KMeans - Too few points ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("4 points with k=8");

        {
            KMeans kmeans(5, 8, "points4.csv", 20);

            kmeans.run();

            // The points should end up each in its own cluster
            pass =  (kmeans.getNumNonemptyClusters() == 4) &&
                    (kmeans[0].getSize() == 1) &&
                    (kmeans[1].getSize() == 1) &&
                    (kmeans[2].getSize() == 1) &&
                    (kmeans[3].getSize() == 1);

            std::ofstream csv1("points4_2.csv", std::ofstream::out);
            csv1 << kmeans;
            csv1.close();

            ClusterCounts cc("points4_2.csv");
            pass = pass && cc.parse_and_analyze() &&
                   (cc.get_num_clusters() == 4) &&
                   (cc.get_num_points() == 4);

            ec.result(pass);
        }

//        ec.DESC("2499 points, k=2520 - COMMENTED OUT");
//        pass = true;
        ec.DESC("2499 points, k=2520 (RUNS LONG)");

        {
            KMeans kmeans(3, 2520, "points2499.csv", 20);

            kmeans.run();

            pass = (kmeans.getNumNonemptyClusters() > 0) &&
                   (kmeans.getNumNonemptyClusters() <= 2499);

            std::ofstream csv1("points2499_2.csv", std::ofstream::out);
            csv1 << kmeans;
            csv1.close();

            ClusterCounts cc("points2499_2.csv");
            bool parse_success;
            pass = pass && (parse_success = cc.parse_and_analyze()) &&
                   (cc.get_num_clusters() < 2520) &&    // this is the only meaningful test for the number of clusters
                                                        // the exact number will vary with implementation
                   (cc.get_num_points() == 2499);

            ec.result(pass);
        }
//        ec.result(pass);
    }
}


// Check if scoring works with large points
void test_kmeans_largepoints(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - KMeans - Large points ---");

    for (int run = 0; run < numRuns; run++) {

        ec.DESC("4 large points, k=1");

        {
            KMeans kmeans(4, 1, "points4_large.csv", 20);

            kmeans.run();

            pass = kmeans.getNumNonemptyClusters() == 1;

            std::ofstream csv1("points4_large_2.csv", std::ofstream::out);
            csv1 << kmeans;
            csv1.close();

            ClusterCounts cc("points4_large_2.csv");
            bool parse_success;
            pass = pass &&
                   (parse_success = cc.parse_and_analyze()) &&
                   (cc.get_num_clusters() == 1) &&
                   (cc.get_num_points() == 4);

            ec.result(pass);
        }

        ec.DESC("4 large points, k=2");

        {
            KMeans kmeans(4, 2, "points4_large.csv", 20);

            kmeans.run();

            pass = kmeans.getNumNonemptyClusters() == 2;

            std::ofstream csv1("points4_large_2.csv", std::ofstream::out);
            csv1 << kmeans;
            csv1.close();

            ClusterCounts cc("points4_large_2.csv");
            bool parse_success;
            pass = pass &&
                   (parse_success = cc.parse_and_analyze()) &&
                   (cc.get_num_clusters() == 2) &&
                   (cc.get_num_points() == 4);

            ec.result(pass);
        }

        ec.DESC("4 large points, k=3");

        {
            KMeans kmeans(4, 3, "points4_large.csv", 20);

            kmeans.run();

            pass = kmeans.getNumNonemptyClusters() == 3;

            std::ofstream csv1("points4_large_2.csv", std::ofstream::out);
            csv1 << kmeans;
            csv1.close();

            ClusterCounts cc("points4_large_2.csv");
            bool parse_success;
            pass = pass &&
                   (parse_success = cc.parse_and_analyze()) &&
                   (cc.get_num_clusters() ==3 ) &&
                   (cc.get_num_points() == 4);

            ec.result(pass);
        }

        ec.DESC("4 large points, k=4");

        {
            KMeans kmeans(4, 4, "points4_large.csv", 20);

            kmeans.run();

            pass = kmeans.getNumNonemptyClusters() == 4;

            std::ofstream csv1("points4_large_2.csv", std::ofstream::out);
            csv1 << kmeans;
            csv1.close();

            ClusterCounts cc("points4_large_2.csv");
            bool parse_success;
            pass = pass &&
                   (parse_success = cc.parse_and_analyze()) &&
                   (cc.get_num_clusters() == 4) &&
                   (cc.get_num_points() == 4);

            ec.result(pass);
        }
    }
}

// Large k, less than number of points
void test_kmeans_toomanyclusters(ErrorContext &ec, unsigned int numRuns) {
    bool pass;

    // Run at least once!!
    assert(numRuns > 0);

    ec.DESC("--- Test - KMeans - Too many clusters ---");

    for (int run = 0; run < numRuns; run++) {

//        ec.DESC("2499 points, k=250 - COMMENTED OUT");
//        pass = true;
        ec.DESC("2499 points, k=250 (RUNS LONG)");

        {
            KMeans kmeans(3, 250, "points2499.csv", 20);

            kmeans.run();

            pass = kmeans.getNumNonemptyClusters() == 250;

            std::ofstream csv1("points2499_2.csv", std::ofstream::out);
            csv1 << kmeans;
            csv1.close();

            ClusterCounts cc("points2499_2.csv");
            bool parse_success;
            pass = pass && (parse_success = cc.parse_and_analyze()) &&
                   (cc.get_num_clusters() <= 250) &&
                   (cc.get_num_points() == 2499);

            ec.result(pass);
        }
//        ec.result(pass);

//        ec.DESC("2499 points, k=520 - COMMENTED OUT");
//        pass = true;
        ec.DESC("2499 points, k=520 (RUNS LONG)");

        {
            KMeans kmeans(3, 520, "points2499.csv", 20);

            kmeans.run();

            pass = kmeans.getNumNonemptyClusters() == 520;

            std::ofstream csv1("points2499_2.csv", std::ofstream::out);
            csv1 << kmeans;
            csv1.close();

            ClusterCounts cc("points2499_2.csv");
            bool parse_success;
            pass = pass && (parse_success = cc.parse_and_analyze()) &&
                   (cc.get_num_clusters() == 520) &&
                   (cc.get_num_points() == 2499);

            ec.result(pass);
        }
//        ec.result(pass);
    }
}
