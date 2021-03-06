# Copyright(c) 1986 Association of Universities for Research in Astronomy Inc.

include <math/curfit.h>

include "curfitdef.h"

# CVVECTOR -- Procedure to evaluate  a curve. The CV_NCOEFF(cv) coefficients
# are assumed to be in COEFF.

procedure cvvector (cv, x, yfit, npts)

pointer	cv		# curve descriptor
real	x[npts]		# data x values
real	yfit[npts]	# the fitted y values
int	npts		# number of data points

begin
	switch (CV_TYPE(cv)) {
	case LEGENDRE:
	    call rcv_evleg (COEFF(CV_COEFF(cv)), x, yfit, npts, CV_ORDER(cv), 
		    CV_MAXMIN(cv), CV_RANGE(cv))
	case CHEBYSHEV:
	    call rcv_evcheb (COEFF(CV_COEFF(cv)), x, yfit, npts, CV_ORDER(cv), 
		    CV_MAXMIN(cv), CV_RANGE(cv))
	case SPLINE3:
	    call rcv_evspline3 (COEFF(CV_COEFF(cv)), x, yfit, npts,
	    			CV_NPIECES(cv), -CV_XMIN(cv), CV_SPACING(cv))
	case SPLINE1:
	    call rcv_evspline1 (COEFF(CV_COEFF(cv)), x, yfit, npts,
	    			CV_NPIECES(cv), -CV_XMIN(cv), CV_SPACING(cv))
	case USERFNC:
	    call rcv_evuser (cv, x, yfit, npts)
	}
end
