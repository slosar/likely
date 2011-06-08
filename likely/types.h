// Created 20-May-2011 by David Kirkby (University of California, Irvine) <dkirkby@uci.edu>

#ifndef LIKELY_TYPES
#define LIKELY_TYPES

#include "boost/function.hpp"
#include "boost/smart_ptr.hpp"
#include "boost/numeric/ublas/symmetric.hpp"

#include <vector>

namespace likely {

    // Represents a vector of parameter values.
    typedef std::vector<double> Parameters;
    
    // Represents a gradient vector of function partial derivatives.
    typedef std::vector<double> Gradient;

    // Represents a Function covariance matrix near a local minimum.
    // Use column-major storage for compatiblity with LAPACK algorithms.
    typedef boost::numeric::ublas::symmetric_matrix<double,
        boost::numeric::ublas::column_major> Covariance;

    // Encapsulates a minimization objective function.
    typedef boost::function<double (Parameters const &pValues)> Function;

    // Declares a smart pointer to an objective function.
    typedef boost::shared_ptr<Function> FunctionPtr;
    
    // Encapsulates a gradient calculator for a minimization objective function.
    typedef boost::function<void (Parameters const &pValues, Gradient &gValues)>
        GradientCalculator;
        
    // Declares a smart pointer to a gradient calculator.
    typedef boost::shared_ptr<GradientCalculator> GradientCalculatorPtr;

    // Represents a smart pointer to a function minimum object.
    class FunctionMinimum;
    typedef boost::shared_ptr<FunctionMinimum> FunctionMinimumPtr;

} // likely

#endif // LIKELY_TYPES
