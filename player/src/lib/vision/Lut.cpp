#include "Lut.h"

Lut::Lut(HSVClrParams orange, HSVClrParams blue, HSVClrParams yellow, HSVClrParams green, HSVClrParams white, HSVClrParams cyan, HSVClrParams magenta)
{
	BITS = log2(UCHAR_RANGE / ENTRIES_CH);
	create(orange, blue, yellow, green, white, cyan, magenta);
}


Lut::~Lut()
{

}


void
Lut::create(HSVClrParams orange, HSVClrParams blue, HSVClrParams yellow, HSVClrParams green, HSVClrParams white, HSVClrParams cyan, HSVClrParams magenta)
{
	float H=0.0, S=0.0, V=0.0;
	unsigned char value, r, g, b;
	bool ish, iss, isv;
	int pos_y, pos_u, pos_v;

	for (int y = 0; y < UCHAR_MAX + 1; y += STEP)
		for (int u = 0; u < UCHAR_MAX + 1; u += STEP)
			for (int v = 0; v < UCHAR_MAX + 1; v += STEP)
			{
				ColorModelConversions::yuv2rgb(y, u, v, r, g, b);
				ColorModelConversions::rgb2hsv(r, g, b, H, S, V);

				//H = 360.0 * H / 255.0;
				H = fromDegrees(H);

				value = 0;

				/* Orange */
				if (orange.hmin <= orange.hmax)
					ish = (H >= orange.hmin && H <= orange.hmax);
				else
					ish = (H >= orange.hmin || H <= orange.hmax);

				iss = (S >= orange.smin && S <= orange.smax);
				isv = (V >= orange.vmin && V <= orange.vmax);

				if (ish && iss && isv) {
					value += ORANGE_MASK;
				}

				/* Blue */
				if (blue.hmin <= blue.hmax)
					ish = (H >= blue.hmin && H <= blue.hmax);
				else
					ish = (H >= blue.hmin || H <= blue.hmax);

				iss = (S >= blue.smin && S <= blue.smax);
				isv = (V >= blue.vmin && V <= blue.vmax);

				if (ish && iss && isv) {
					value += BLUE_MASK;
				}

				/* Yellow */
				if (yellow.hmin <= yellow.hmax)
					ish = (H >= yellow.hmin && H <= yellow.hmax);
				else
					ish = (H >= yellow.hmin || H <= yellow.hmax);

				iss = (S >= yellow.smin && S <= yellow.smax);
				isv = (V >= yellow.vmin && V <= yellow.vmax);

				if (ish && iss && isv) {
					value += YELLOW_MASK;
				}

				/* Green */
				if (green.hmin <= green.hmax)
					ish = (H >= green.hmin && H <= green.hmax);
				else
					ish = (H >= green.hmin || H <= green.hmax);

				iss = (S >= green.smin && S <= green.smax);
				isv = (V >= green.vmin && V <= green.vmax);

				if (ish && iss && isv) {
					value += GREEN_MASK;
				}

				/* White */
				if (white.hmin <= white.hmax)
					ish = (H >= white.hmin && H <= white.hmax);
				else
					ish = (H >= white.hmin || H <= white.hmax);

				iss = (S >= white.smin && S <= white.smax);
				isv = (V >= white.vmin && V <= white.vmax);

				if (ish && iss && isv) {
					value += WHITE_MASK;
				}

				/* Cyan */
				if (cyan.hmin <= cyan.hmax)
					ish = (H >= cyan.hmin && H <= cyan.hmax);
				else
					ish = (H >= cyan.hmin || H <= cyan.hmax);

				iss = (S >= cyan.smin && S <= cyan.smax);
				isv = (V >= cyan.vmin && V <= cyan.vmax);

				if (ish && iss && isv) {
					value += CYAN_MASK;
				}

				/* Magenta */
				if (magenta.hmin <= magenta.hmax)
					ish = (H >= magenta.hmin && H <= magenta.hmax);
				else
					ish = (H >= magenta.hmin || H <= magenta.hmax);

				iss = (S >= magenta.smin && S <= magenta.smax);
				isv = (V >= magenta.vmin && V <= magenta.vmax);

				if (ish && iss && isv) {
					value += MAGENTA_MASK;
				}

				pos_y = (int)y / STEP;
				pos_u = (int)u / STEP;
				pos_v = (int)v / STEP;

				table[pos_y][pos_u][pos_v] = value;
			}
}
