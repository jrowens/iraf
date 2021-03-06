# Copyright(c) 1986 Association of Universities for Research in Astronomy Inc.

include <math/curfit.h>

include "curfitdef.h"

# CVREJECT -- Procedure to subtract a single datapoint from the data set.
# The normal equations for the data point are calculated
# and subtracted from MATRIX and VECTOR. After all rejected points
# have been subtracted from the fit CVSOLVE, must be called to generate
# a new set of coefficients.

procedure cvrject (cv, x, y, w)

pointer	cv		# curve fitting image descriptor
real	x		# x value
real	y		# y value
real	w		# weight of the data point

int	left, i, ii, j
pointer	xzptr
pointer mzptr, mzzptr
pointer	vzptr
real   bw

begin

	# increment the number of points
	CV_NPTS(cv) = CV_NPTS(cv) - 1

	# calculate all type non-zero basis functions for a given data point
	switch (CV_TYPE(cv)) {
	case CHEBYSHEV:
	    left = 0
	    call rcv_b1cheb (x, CV_ORDER(cv), CV_MAXMIN(cv), CV_RANGE(cv),
			    XBASIS(CV_XBASIS(cv)))
	case LEGENDRE:
	    left = 0
	    call rcv_b1leg (x, CV_ORDER(cv), CV_MAXMIN(cv), CV_RANGE(cv),
			    XBASIS(CV_XBASIS(cv)))
	case SPLINE3:
	    call rcv_b1spline3 (x, CV_NPIECES(cv), -CV_XMIN(cv),
			       CV_SPACING(cv), XBASIS(CV_XBASIS(cv)), left)
	case SPLINE1:
	    call rcv_b1spline1 (x, CV_NPIECES(cv), -CV_XMIN(cv),
			       CV_SPACING(cv), XBASIS(CV_XBASIS(cv)), left)	
	case USERFNC:
	    left = 0
	    call rcv_b1user (cv, x)
	}

	# index the pointers
	xzptr = CV_XBASIS(cv) - 1
	mzptr = CV_MATRIX(cv) + CV_ORDER(cv) * (left - 1)
	vzptr = CV_VECTOR(cv) + left - 1

	# calculate the normal equations for the data point and subtract
	# them from the fit
	do i = 1, CV_ORDER(cv) {

	    # subtract inner product of basis functions and data ordinate
	    # from the fit
	    bw = XBASIS(xzptr+i) * w
	    VECTOR(vzptr+i) = VECTOR(vzptr+i) - bw * y

	    # subtract inner product of basis functions from the fit
	    ii = 0
	    mzzptr = mzptr + i * CV_ORDER(cv)
	    do j = i, CV_ORDER(cv) {
		MATRIX(mzzptr+ii) = MATRIX(mzzptr+ii) - XBASIS(xzptr+j) * bw
		ii = ii + 1
	    }
	}
end
