/*
farfield.c

far field E = (-jk)/sqrt(8*pi*Z)*F -> (square/Pfeed) = gain [1]
*/

#include "omm.h"
#include "complex.h"

static void farcomp(
	const d_complex_t c, const double pos[3], const double tan[3], const double lng, const double kwave,
	const double r1[3], const double t1[3], const double p1[3],
	d_complex_t *cth, d_complex_t *cph, d_complex_t *cexp)
{
/*
	d_complex_t clt[3], clp[3];
	for (int k = 0; k < 3; k++) {
		clt[k].r = lng * tan[k] * t1[k] * c.r;
		clt[k].i = lng * tan[k] * t1[k] * c.i;
		clp[k].r = lng * tan[k] * p1[k] * c.r;
		clp[k].i = lng * tan[k] * p1[k] * c.i;
	}
	cth->r = clt[0].r + clt[1].r + clt[2].r;
	cth->i = clt[0].i + clt[1].i + clt[2].i;
	cph->r = clp[0].r + clp[1].r + clp[2].r;
	cph->i = clp[0].i + clp[1].i + clp[2].i;
*/
	*cth = d_rmul(((tan[0] * t1[0]) + (tan[1] * t1[1]) + (tan[2] * t1[2])) * lng, c);
	*cph = d_rmul(((tan[0] * p1[0]) + (tan[1] * p1[1]) + (tan[2] * p1[2])) * lng, c);
/*
	const double phs = (r1[0] * pos[0]) + (r1[1] * pos[1]) + (r1[2] * pos[2]);
	cexp->r = cos(kwave * phs);
	cexp->i = sin(kwave * phs);
*/
	*cexp = d_exp(kwave * ((pos[0] * r1[0]) + (pos[1] * r1[1]) + (pos[2] * r1[2])));
}

void farfield(int ifreq, double theta, double phi, double fctr, d_complex_t *etheta, d_complex_t *ephi)
{
	double r1[3], t1[3], p1[3], pos[3], tan[3];
	d_complex_t cth, cph, cexp;

	const double kwave = (2 * PI * Frequency[ifreq]) / C;

	const double sint = sin(theta * DTOR);
	const double cost = cos(theta * DTOR);
	const double sinp = sin(phi   * DTOR);
	const double cosp = cos(phi   * DTOR);

	// unit vectors in r, theta and phi direction
	r1[0] = +sint * cosp;
	r1[1] = +sint * sinp;
	r1[2] = +cost;
	t1[0] = +cost * cosp;
	t1[1] = +cost * sinp;
	t1[2] = -sint;
	p1[0] =        -sinp;
	p1[1] =        +cosp;
	p1[2] = 0;

	// check under ground
	if (IGround && (r1[IGround - 1] < -EPS)) {
		*etheta = *ephi = d_complex(0, 0);
		return;
	}

	// sum
	double sumt_r = 0, sumt_i = 0;
	double sump_r = 0, sump_i = 0;
	for (int ie = 0; ie < NElement; ie++) {
		const d_complex_t c = Cv[(ifreq * NElement) + ie];
		const double lng = Element[ie].lng[0];

		for (int k = 0; k < 3; k++) {
			pos[k] = Element[ie].pos[0][k];
			tan[k] = Element[ie].tan[k];
		}

		farcomp(c, pos, tan, lng, kwave, r1, t1, p1, &cth, &cph, &cexp);
		sumt_r += (cth.r * cexp.r) - (cth.i * cexp.i);
		sumt_i += (cth.r * cexp.i) + (cth.i * cexp.r);
		sump_r += (cph.r * cexp.r) - (cph.i * cexp.i);
		sump_i += (cph.r * cexp.i) + (cph.i * cexp.r);

		// substract ground image
		if (IGround) {
			pos[IGround - 1] *= -1;
			tan[IGround - 1] *= -1;

			farcomp(c, pos, tan, lng, kwave, r1, t1, p1, &cth, &cph, &cexp);
			sumt_r -= (cth.r * cexp.r) - (cth.i * cexp.i);
			sumt_i -= (cth.r * cexp.i) + (cth.i * cexp.r);
			sump_r -= (cph.r * cexp.r) - (cph.i * cexp.i);
			sump_i -= (cph.r * cexp.i) + (cph.i * cexp.r);
		}
	}

	// factor
	d_complex_t cfctr = d_complex(0, -kwave * sqrt(ETA0 / (8 * PI * fctr)));
	*etheta = d_mul(cfctr, d_complex(sumt_r, sumt_i));
	*ephi   = d_mul(cfctr, d_complex(sump_r, sump_i));
	//printf("%f %f\n", cfctr.r, cfctr.i);
	//printf("%f %f %f %f\n", etheta->r, etheta->i, ephi->r, ephi->i);
}


// far field components
void farComponent(d_complex_t etheta, d_complex_t ephi, double e[])
{
	// abs
	e[0] = sqrt(d_norm(etheta) + d_norm(ephi));

	// theta/phi
	e[1] = d_abs(etheta);
	e[2] = d_abs(ephi);

	// major/minor
	double tmp = d_abs(d_add(d_mul(etheta, etheta), d_mul(ephi, ephi)));
	e[3] = sqrt((d_norm(etheta) + d_norm(ephi) + tmp) / 2);
	e[4] = sqrt((d_norm(etheta) + d_norm(ephi) - tmp) / 2);

	// RHCP/LHCP
	e[5] = d_abs(d_add(etheta, d_mul(d_complex(0, 1), ephi))) / sqrt(2);
	e[6] = d_abs(d_sub(etheta, d_mul(d_complex(0, 1), ephi))) / sqrt(2);
}

// far field factor
double farfactor(int ifreq)
{
	double ret = 0;

	if (NFeed) {
		int ifeed = 0;
		for (int ie = 0; ie < NElement; ie++) {
			if (Element[ie].ifeed) {
				const d_complex_t zin = Zin[(ifeed * NFrequency) + ifreq];
				const double gin = d_inv(zin).r;
				const double amp = Element[ie].feed[0];
				double pow = 0.5 * gin * (amp * amp);
				if (MatchingLoss) {
					const double rin = zin.r;
					const double xin = zin.i;
					const double denom = 1
						 - ((rin - Z0) * (rin - Z0) + (xin * xin))
						 / ((rin + Z0) * (rin + Z0) + (xin * xin));
					pow /= denom;
				}
				ret += pow;
				ifeed++;
			}
		}
	}
	else if (IPlanewave) {
		ret = 1 / (2 * ETA0);
	}

	return ret;
}
