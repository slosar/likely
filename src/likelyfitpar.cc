// Created 05-Jun-2012 by David Kirkby (University of California, Irvine) <dkirkby@uci.edu>
// Demonstates the FitParameter class.

#include "likely/likely.h"

#include <iostream>
#include <cassert>

namespace lk = likely;

class TestModel : public lk::FitModel {
public:
    TestModel() : lk::FitModel("Test Model") {
        defineParameter("p1",1);
        defineParameter("p2",2);
        defineParameter("p3",3);
    }
    virtual ~TestModel() { }
    void printChangedAndReset() {
        for(int index = 0; index < getNParameters(); ++index) {
            std::cout << "index " << index << " value " << getParameterValue(index)
                << " changed? " << isParameterValueChanged(index) << std::endl;
        }
        resetParameterValuesChanged();
    }
    bool update(lk::Parameters const &values) {
        return updateParameterValues(values);
    }
};

int main(int argc, char *argv[]) {
    lk::FitParameters params;
    params.push_back(lk::FitParameter("param1",1,0.1));
    params.push_back(lk::FitParameter("param2",2,0.2));
    params.push_back(lk::FitParameter("param3",3,0.3));
    params.push_back(lk::FitParameter("(1-beta)*bias",1.23));
    lk::printFitParametersToStream(params,std::cout);    

    lk::modifyFitParameters(params," fix [param2] = -2; fix [param1]");
    lk::printFitParametersToStream(params,std::cout);    

    lk::modifyFitParameters(params,"release [par*] ;");
    lk::printFitParametersToStream(params,std::cout);    

    lk::modifyFitParameters(params,"value[param3*]=-123; error[param1]=1e-2");
    lk::printFitParametersToStream(params,std::cout);

    lk::modifyFitParameters(params,"error [(1-beta)*bias] = 0.5");
    lk::printFitParametersToStream(params,std::cout);

    try {
        lk::modifyFitParameters(params,"value [param3]=0;error [param3] = -123");
    }
    catch(lk::RuntimeError const &e) {
        // We expect this since error < 0 is not allowed.
    }
    // Check that the parameters were not actually modified.
    lk::printFitParametersToStream(params,std::cout);
    
    try {
        lk::FitParameter badName("a,b",123);
    }
    catch(lk::RuntimeError const &e) {
        // We expect this since commas are not allowed in names.
        std::cout << "Got expected RuntimeError" << std::endl;
    }    
    try {
        lk::modifyFitParameters(params,"release [aram*]");
    }
    catch(lk::RuntimeError const &e) {
        // We expect this since the pattern does not match any names.
        std::cout << "Got expected RuntimeError" << std::endl;
    }
    
    TestModel model;
    model.printToStream(std::cout);
    model.printChangedAndReset();
    model.printChangedAndReset();
    model.setParameterValue("p2",22);
    model.printChangedAndReset();
    lk::Parameters pvalues(model.getNParameters());
    pvalues[0] = -1;
    pvalues[1] = model.getParameterValue("p2");
    pvalues[2] = model.getParameterValue("p3");
    std::cout << "update any changes? " << model.update(pvalues) << std::endl;
    model.printChangedAndReset();
    pvalues[1] = -2;
    pvalues[2] = -3;
    std::cout << "update any changes? " << model.update(pvalues) << std::endl;
    model.printChangedAndReset();
    std::cout << "update any changes? " << model.update(pvalues) << std::endl;
    model.printChangedAndReset();
}
