// Created 17-Apr-2012 by David Kirkby (University of California, Irvine) <dkirkby@uci.edu>

#ifndef LIKELY_COVARIANCE_MATRIX
#define LIKELY_COVARIANCE_MATRIX

#include <vector>
#include <string>
#include <cstddef>

namespace likely {
    // Represents an abstract interface to a covariance matrix.
	class CovarianceMatrix {
	public:
	    // Creates a new size-by-size covariance matrix with all elements initialized to zero.
	    // Throws a RuntimeError if size <= 0. Note that the matrix created by this constructor
	    // is not valid until sufficient elements have been set to make it positive definite.
		explicit CovarianceMatrix(int size);
		virtual ~CovarianceMatrix();
		// Returns the fixed size of this covariance matrix.
        int getSize() const;
        // Returns the specified (inverse) covariance matrix element or throws a RuntimeError.
        // (row,col) and (col,row) give the same result by construction.
        double getCovariance(int row, int col) const;
        double getInverseCovariance(int row, int col) const;
        // Sets the specified (inverse) covariance matrix element or throws a RuntimeError.
        // Row and column indices should be in the range [0,size-1]. Setting any element with
        // row != col will also set the symmetric element in the matrix.
        void setCovariance(int row, int col, double value);
        void setInverseCovariance(int row, int col, double value);
        // Multiplies the specified vector by the inverse covariance or throws a RuntimeError.
        // The result is stored in the input vector, overwriting its original contents.
        void multiplyByInverseCovariance(std::vector<double> &vector) const;
        // Calculates the chi-square = delta.Cinv.delta for the specified residuals vector delta
        // or throws a RuntimeError.
        double chiSquare(std::vector<double> const &delta) const;
        // Generates the specified number of random residuals vectors by sampling the Gaussian
        // probability density implied by this object, or throws a RuntimeError. The generated
        // vectors are packed into the input vector, which will be resized to nsample*getSize().
        void sample(int nsample, std::vector<double> &residuals) const;
        // Requests that this covariance matrix be compressed to reduce its memory usage,
        // if possible. Returns immediately if we are already compressed. Any compression
        // is lossless. The next call to any method except getSize(), compress(), or
        // isCompressed() will automatically trigger a decompression. Return value indicates
        // if any compression was actually performed.
        bool compress() const;
        // Returns true if this covariance matrix is currently compressed.
        bool isCompressed() const;
        // Returns the memory usage of this object.
        std::size_t getMemoryUsage() const;
        // Returns a string describing this object's internal state in the form
        // 
        // [MICDZV] nnnnnnn
        //
        // where each letter indicates the memory allocation state of an internal
        // vector and nnnnnn is the total number of bytes used by this object, as reported
        // by getMemoryUsage(). The letter codes are: M = _cov, I = _icov, C = _cholesky,
        // D = _diag, Z = _offdiagIndex, V = _offdiagValue. A "-" indidcates that the vector
        // is not allocated. A "." below is a wildcard.
        //
        // [---...] : newly created object with no elements set
        // [M--...] : most recent change was to covariance matrix
        // [-I-...] : most recent change was to inverse covariance matrix
        // [MI-...] : synchronized covariance and inverse covariance both in memory
        // [--C...] : ** this should never happen **
        // [M-C...] : Cholesky decomposition and covariance in memory
        // [-IC...] : Cholesky decomposition and inverse covariance in memory
        // [MIC...] : Cholesky decomposition, covariance and inverse covariance in memory
        // [...D--] : Matrix is diagonal and compressed
        // [...DZV] : Matrix is non-diagonal and compressed
        std::string getMemoryState() const;
    private:
        // Undoes any compression. Returns immediately if we are already uncompressed.
        // There is usually no need to call this method explicitly, since it is called
        // automatically as needed by other methods.
        void _uncompress() const;
	    // Prepares to read elements of _cov or _icov. Returns false if nothing has
	    // been allocated yet, or else returns true. Always uncompresses.
        bool _readsCov() const;
        bool _readsICov() const;
        // Prepares to change at least one element of _cov or _icov.
        void _changesCov();
        void _changesICov();
        // Helper function used by getMemoryState()
        char _tag(char symbol, std::vector<double> const &vector) const;

        // TODO: is a cached value of _ncov = (_size*(_size+1))/2 really necessary?
        int _size, _ncov;
        // Track our compression state. This is not the same as !_diag.empty() since we
        // cache previous compression data until a change to _cov or _icov invalidates it.
        mutable bool _compressed;
        // _cholesky is the Cholesky decomposition of the covariance matrix (_cov, not _icov)
        mutable std::vector<double> _cov, _icov, _cholesky;
        // compression replaces _cov, _icov, _cholesky with the following
        // smaller vectors, that encode the inverse covariance matrix (_icov not _cov).
        mutable std::vector<double> _diag, _offdiagIndex, _offdiagValue;
        // keep a random seed counter used by sample()
        mutable int _nextSeed;
	}; // CovarianceMatrix
	
    inline int CovarianceMatrix::getSize() const { return _size; }
    
    inline bool CovarianceMatrix::isCompressed() const { return _compressed; }

    // Returns the array offset index for the BLAS packed symmetric matrix format
    // described at http://www.netlib.org/lapack/lug/node123.html or throws a
    // RuntimeError for invalid row or col inputs.
    int symmetricMatrixIndex(int row, int col, int size);
    // Returns the size of a symmetric matrix in the BLAS packed format implied by
    // symmetricMatrixIndex, or throws a RuntimeError. The size is related to the
    // number nelem of packed matrix elements by size = (nelem*(nelem+1))/2.
    int symmetricMatrixSize(int nelem);
    // Performs a Cholesky decomposition in place of a symmetric positive definite matrix
    // or throws a RuntimeError if the matrix is not positive definite. The input matrix
    // is assumed to be in the BLAS packed format implied by packedMatrixIndex(row,col).
    // The matrix size will be calculated unless a positive value is provided.
    static void choleskyDecompose(std::vector<double> &matrix, int size = 0);
    // Inverts a symmetric positive definite matrix in place, or throws a RuntimeError.
    // The input matrix should already be Cholesky decomposed and in the BLAS packed format
    // implied by packedMatrixIndex(row,col), e.g. by first calling _choleskyDecompose(matrix).
    // The matrix size will be calculated unless a positive value is provided.
    static void invertCholesky(std::vector<double> &matrix, int size = 0);
    // Multiplies a symmetric matrix by a vector, or throws a RuntimeError. The input matrix
    // is assumed to be in the BLAS packed format implied by packedMatrixIndex(row,col).
    static void symmetricMatrixMultiply(std::vector<double> const &matrix,
        std::vector<double> const &vector, std::vector<double> &result);

} // likely

#endif // LIKELY_COVARIANCE_MATRIX
