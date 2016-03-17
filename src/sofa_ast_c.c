#include "sofa.h"
#include <stdio.h>

void reprd ( char*, double, double );

int main (){
	double phi, elong, hm, phpa, tc, rh, wl, utc1, utc2, tai1, tai2,
	tt1, tt2, xp, yp, dut1, rc, dc, pr, pd, px, rv,
	eo, ri, di, rca, dca, ra, da, aot, zot, hot, dot, rot,
	aob, zob, hob, dob, rob;

	/* Site longitude, latitude (radians) and height above the geoid (m). */
	iauAf2a ( '+', 41, 26, 26.45, &elong );
	iauAf2a ( '+', 43, 39, 12.69, &phi );
	hm = 2070.0;
	/* Ambient pressure (HPa), temperature (C) and rel. humidity (frac). */
	phpa = 952.0;
	tc = 18.5;
	rh = 0.83;
	/* Effective color (microns). */
	wl = 0.55;
	/* UTC date. */
	if ( iauDtf2d ( "UTC", 2016, 3, 16, 11, 42, 0.,
	&utc1, &utc2 ) ) return -1;
	/* TT date. */
	if ( iauUtctai ( utc1, utc2, &tai1, &tai2 ) ) return -1;
	if ( iauTaitt ( tai1, tai2, &tt1, &tt2 ) ) return -1;
	/* EOPs: polar motion in radians, UT1-UTC in seconds. */
	xp = 50.995e-3 * DAS2R;
	yp = 376.723e-3 * DAS2R;
	dut1 = 155.0675e-3;
	/* Star ICRS RA,Dec (radians). */
	if ( iauTf2a ( ' ', 19, 50, 47.6, &rc ) ) return -1;
	if ( iauAf2a ( '+', 8, 52, 12.3, &dc ) ) return -1;
	reprd ( "ICRS, epoch J2000.0:", rc, dc );
	/* Proper motion: RA/Dec derivatives, epoch J2000.0. */
	pr = atan2 ( 536.23e-3 * DAS2R, cos(dc) );
	pd = 385.29e-3 * DAS2R;
	/* Parallax (arcsec) and recession speed (km/s). */
	px = 0.19495;
	rv = -26.1;
	/* ICRS to CIRS (geocentric observer). */
	iauAtci13 ( rc, dc, pr, pd, px, rv, tt1, tt2, &ri, &di, &eo );
	reprd ( "catalog -> CIRS:", ri, di );
	/* CIRS to ICRS (astrometric). */
	iauAtic13 ( ri, di, tt1, tt2, &rca, &dca, &eo );
	reprd ( "CIRS -> astrometric:", rca, dca );
	/* ICRS (astrometric) to CIRS (geocentric observer). */
	iauAtci13 ( rca, dca, 0.0, 0.0, 0.0, 0.0, tt1, tt2, &ri, &di, &eo );
	reprd ( "astrometric -> CIRS:", ri, di );
	/* Apparent place. */
	ra = iauAnp ( ri - eo );
	da = di;
	reprd ( "geocentric apparent:", ra, da );

	/* CIRS to topocentric. */
	if ( iauAtio13 ( ri, di, utc1, utc2, dut1, elong, phi, hm, xp, yp,
	0.0, 0.0, 0.0, 0.0,
	&aot, &zot, &hot, &dot, &rot ) ) return -1;
	reprd ( "CIRS -> topocentric:", rot, dot );
	/* CIRS to observed. */
	if ( iauAtio13 ( ri, di, utc1, utc2, dut1, elong, phi, hm, xp, yp,
	phpa, tc, rh, wl,
	&aob, &zob, &hob, &dob, &rob ) ) return -1;
	reprd ( "CIRS -> observed:", rob, dob );
	/* ICRS to observed. */
	if ( iauAtco13 ( rc, dc, pr,
	 pd, px, rv, utc1, utc2, dut1, elong, phi,
	 hm, xp, yp, phpa, tc, rh, wl, &aob, &zob,
	 &hob, &dob, &rob, &eo ) ) return -1;

	reprd ( "ICRS -> observed:",  rob, dob );

	return 0;
}

void reprd ( char* s, double ra, double dc ){
	char pm;
	int i[4];
	printf ( "%25s", s );
	iauA2tf ( 7, ra, &pm, i );
	printf ( " %2.2d %2.2d %2.2d.%7.7d", i[0],i[1],i[2],i[3] );
	iauA2af ( 6, dc, &pm, i );
	printf ( " %c%2.2d %2.2d %2.2d.%6.6d\n", pm, i[0],i[1],i[2],i[3] );
}
