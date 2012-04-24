// Created 24-Apr-2012 by David Kirkby (University of California, Irvine) <dkirkby@uci.edu>

#include "likely/BinnedData.h"
#include "likely/RuntimeError.h"
#include "likely/AbsBinning.h"

#include "boost/foreach.hpp"

namespace local = likely;

local::BinnedData::BinnedData(std::vector<AbsBinningCPtr> axes)
: _axisBinning(axes)
{
    if(0 == axes.size()) {
        throw RuntimeError("BinnedData: no axes provided.");
    }
    _initialize();
}

local::BinnedData::BinnedData(AbsBinningCPtr axis1)
{
    if(!axis1) {
        throw RuntimeError("BinnedData: missing axis1.");
    }
    _axisBinning.push_back(axis1);
    _initialize();
}

local::BinnedData::BinnedData(AbsBinningCPtr axis1, AbsBinningCPtr axis2)
{
    if(!axis1 || !axis2) {
        throw RuntimeError("BinnedData: missing axis data.");
    }
    _axisBinning.push_back(axis1);    
    _axisBinning.push_back(axis2);
    _initialize();
}

local::BinnedData::BinnedData(AbsBinningCPtr axis1, AbsBinningCPtr axis2, AbsBinningCPtr axis3)
{
    if(!axis1 || !axis2 || !axis3) {
        throw RuntimeError("BinnedData: missing axis data.");
    }
    _axisBinning.push_back(axis1);
    _axisBinning.push_back(axis2);
    _axisBinning.push_back(axis3);
    _initialize();
}

void local::BinnedData::_initialize() {
    _ndata = 0;
    _nbins = 1;
    BOOST_FOREACH(AbsBinningCPtr binning, _axisBinning) {
        _nbins *= binning->getNBins();
    }
}

local::BinnedData::~BinnedData() { }

int local::BinnedData::getIndex(std::vector<int> const &binIndices) const {
    int index(0), nAxes(getNAxes());
    if(binIndices.size() != nAxes) {
        throw RuntimeError("BinnedData::getIndex: invalid input vector size.");
    }
    for(int axis = nAxes-1; axis >= 0; --axis) {
        int binIndex(binIndices[axis]), nBins(_axisBinning[axis]->getNBins());
        if(binIndex < 0 || binIndex >= nBins) {
            throw RuntimeError("BinnedData::getIndex: invalid bin index.");
        }
        index = binIndices[axis] + index*nBins;
    }
    return index;
}

int local::BinnedData::getIndex(std::vector<double> const &values) const {
    int index(0), nAxes(getNAxes());
    if(values.size() != nAxes) {
        throw RuntimeError("BinnedData::getIndex: invalid input vector size.");
    }
    std::vector<int> binIndices;
    for(int axis = 0; axis < nAxes; ++axis) {
        binIndices.push_back(_axisBinning[axis]->getBinIndex(values[axis]));
    }
    return getIndex(binIndices);
}

void local::BinnedData::getBinIndices(int index, std::vector<int> &binIndices) const {
    binIndices.resize(0);
    binIndices.reserve(getNAxes());
    int partial(index);
    BOOST_FOREACH(AbsBinningCPtr binning, _axisBinning) {
        int nBins(binning->getNBins()), binIndex(partial % nBins);
        binIndices.push_back(binIndex);
        partial = (partial - binIndex)/nBins;
    }
}

void local::BinnedData::getBinCenters(int index, std::vector<double> &binCenters) const {
    binCenters.resize(0);
    binCenters.reserve(getNAxes());
    std::vector<int> binIndices;
    getBinIndices(index,binIndices);
    int nAxes(getNAxes());
    for(int axis = 0; axis < nAxes; ++axis) {
        AbsBinningCPtr binning = _axisBinning[axis];
        binCenters.push_back(binning->getBinCenter(binIndices[axis]));
    }
}

void local::BinnedData::getBinWidths(int index, std::vector<double> &binWidths) const {
    binWidths.resize(0);
    binWidths.reserve(getNAxes());
    std::vector<int> binIndices;
    getBinIndices(index,binIndices);
    int nAxes(getNAxes());
    for(int axis = 0; axis < nAxes; ++axis) {
        AbsBinningCPtr binning = _axisBinning[axis];
        binWidths.push_back(binning->getBinWidth(binIndices[axis]));
    }
}