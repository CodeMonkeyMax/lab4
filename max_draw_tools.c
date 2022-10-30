#include "FPToolkit.c"
#include "M2d_matrix_toolsS.c"
#include "max_poly_tools.c"
#include <math.h>
#include <stdio.h>

// Multi Draw 2: draws up to 10 .xy objects, given at start

// FIXME: sometimes multi_draw2 gets sad and disfigures an object (just one)

// # is a symbol for compile-time directives
#define MAXOBJS 10
#define MAXPTS 59000
#define MAXPOLYS 57500

int numpoints[MAXOBJS];
double x[MAXOBJS][MAXPTS], y[MAXOBJS][MAXPTS];
int numpolys[MAXOBJS];
int numobjs;
int psize[MAXOBJS][MAXPOLYS];
int con[MAXOBJS][MAXPOLYS][20];
double poly_rgb[MAXOBJS][MAXPOLYS][3];

void read_object(FILE *f, int obji);
void draw_object(int obji);
void center_and_scale(int obji);
void stretch_to_fill(int obji);
void rotate(int obji, double t);

// ROTATE # # # # # # # # # # # # # # # # # # # #
void rotate(int obji, double t) {
	print_xy(x[obji], y[obji], numpoints[obji]);
	double dmat[3][3];
	double ddmat[3][3];

	printf("\nstarting translation...\n");
	M2d_make_identity(dmat);
	M2d_make_identity(ddmat);
	M2d_make_translation(ddmat, -screen.x / 2, -screen.y / 2);
	printf("dmat:\n");
	M2d_print_mat(dmat);
	printf("ddmat:\n");
	M2d_print_mat(ddmat);
	M2d_mat_mult(dmat, ddmat, dmat);
	printf("translation added...\n");
	printf("dmat:\n");
	M2d_print_mat(dmat);
	printf("ddmat:\n");
	M2d_print_mat(ddmat);

	printf("\nstarting rotation...\n");
	M2d_make_identity(ddmat);
	M2d_make_rotation(ddmat, t);
	printf("dmat:\n");
	M2d_print_mat(dmat);
	printf("ddmat:\n");
	M2d_print_mat(ddmat);
	M2d_mat_mult(dmat, ddmat, dmat);
	printf("rotation added...\n");
	printf("dmat:\n");
	M2d_print_mat(dmat);
	printf("ddmat:\n");
	M2d_print_mat(ddmat);

	/*
	M2d_make_translation(
		dmat,
		-cos(t) * screen.x + sin(t) * screen.y,
		-sin(t) * screen.x - cos(t) * screen.y);
  */
	printf("\nstarting translation...\n");
	M2d_make_identity(ddmat);
	M2d_make_translation(ddmat, screen.x / 2, screen.y / 2);
	printf("dmat:\n");
	M2d_print_mat(dmat);
	printf("ddmat:\n");
	M2d_print_mat(ddmat);
	M2d_mat_mult(dmat, ddmat, dmat);
	printf("translation added...\n");
	printf("dmat:\n");
	M2d_print_mat(dmat);
	printf("ddmat:\n");
	M2d_print_mat(ddmat);

	printf("\nstarting mat mult...\n");
	M2d_mat_mult_points(
		x[obji], y[obji], dmat, x[obji], y[obji], numpoints[obji]);
	printf("dmat applied...\n");
	print_xy(x[obji], y[obji], numpoints[obji]);
}

// READ OBJECT # # # # # # # # # # # # # # # # # #
void read_object(FILE *f, int obji) {
	if (f == NULL) {
		printf("bad file\n");
		exit(0);
	}

	// Get number of coordinates
	fscanf(f, "%d", &numpoints[obji]);

	// Get X and Y coordinates
	for (int i = 0; i < numpoints[obji]; i++) {
		fscanf(f, "%lf %lf", &x[obji][i], &y[obji][i]);
	}

	// Get number of polygons
	fscanf(f, "%d", &numpolys[obji]);

	// Get polygon point indices
	for (int i = 0; i < numpolys[obji]; i++) {
		fscanf(f, "%d", &psize[obji][i]);
		for (int j = 0; j < psize[obji][i]; j++) {
			fscanf(f, "%d", &con[obji][i][j]);
		}
	}

	// Get polygon colors
	for (int i = 0; i < numpolys[obji]; i++) {
		fscanf(
			f,
			"%lf %lf %lf",
			&poly_rgb[obji][i][0],
			&poly_rgb[obji][i][1],
			&poly_rgb[obji][i][2]);
	}
}

// DRAW OBJECT # # # # # # # # # # # # # # # # # #
void draw_object(int obji) {
	// Get X,Y values of poly coord indices,
	// and feed to XY_poly_fill()
	printf("beginning draw_object...\n");
	G_rgb(0, 0, 0.1);
	G_clear();
	for (int i = 0; i < numpolys[obji]; i++) {
		// printf("poly %d/%d: \n", i + 1, numpolys[obji]);
		double px[20], py[20];
		for (int j = 0; j < psize[obji][i]; j++) {
			// printf("pt %d/%d ", j + 1, psize[obji][i]);
			px[j] = x[obji][con[obji][i][j]];
			py[j] = y[obji][con[obji][i][j]];
		}
		// printf("\nfilling...\n");
		G_rgb(poly_rgb[obji][i][0], poly_rgb[obji][i][1], poly_rgb[obji][i][2]);
		G_fill_polygon(px, py, psize[obji][i]);
		// my xy_poly_fill produces a segmentation fault after rotation is
		// applied xy_poly_fill(px, py, psize[obji][i], poly_rgb[obji][i]);
	}
}

// TRANSLATE # # # # # # # # # # # # # # # # # # #
void translate(int obji, double dx, double dy) {
	for (int i = 0; i < numpoints[obji]; i++) {
		x[obji][i] += dx;
		y[obji][i] += dy;
	}
}

// CENTER AND SCALE # # # # # # # # # # # # # # #
void center_and_scale(int obji) {
	double biggest[2] = {0, 0};
	double smallest[2] = {0, 0};

	// Check X and Y coordinates
	biggest[0] = x[obji][0];
	biggest[1] = y[obji][0];
	smallest[0] = biggest[0];
	smallest[1] = biggest[1];
	for (int i = 1; i < numpoints[obji]; i++) {
		if (x[obji][i] > biggest[0])
			biggest[0] = x[obji][i];
		if (x[obji][i] < smallest[0])
			smallest[0] = x[obji][i];
		if (y[obji][i] > biggest[1])
			biggest[1] = y[obji][i];
		if (y[obji][i] < smallest[1])
			smallest[1] = y[obji][i];
	}
	printf(
		"\nShape Dimensions: ( %lf, %lf ), ( %lf, %lf )\n",
		smallest[0],
		smallest[1],
		biggest[0],
		biggest[1]);
	double xsize, ysize;
	xsize = biggest[0] - smallest[0];
	ysize = biggest[1] - smallest[1];
	printf("\nShape Size: ( %lf x %lf )\n", xsize, ysize);

	translate(obji, -(smallest[0] + xsize / 2), -(smallest[1] + ysize / 2));

	double xc, yc, c;

	xc = screen.x / xsize;
	yc = screen.y / ysize;

	if (xc < yc) {
		c = xc;
	} else {
		c = yc;
	}

	printf("Scaling by %lfx...\n\n", c);

	for (int i = 0; i < numpoints[obji]; i++) {
		x[obji][i] *= c;
		y[obji][i] *= c;
	}

	translate(obji, screen.x / 2, screen.y / 2);
}

// STRETCH TO FILL # # # # # # # # # # # # # # # #
void stretch_to_fill(int obji) {
	// Check X and Y bounds
	double biggest[2] = {0, 0};
	double smallest[2] = {0, 0};
	biggest[0] = x[obji][0];
	biggest[1] = y[obji][0];
	smallest[0] = biggest[0];
	smallest[1] = biggest[1];
	for (int i = 1; i < numpoints[obji]; i++) {
		if (x[obji][i] > biggest[0])
			biggest[0] = x[obji][i];
		if (x[obji][i] < smallest[0])
			smallest[0] = x[obji][i];
		if (y[obji][i] > biggest[1])
			biggest[1] = y[obji][i];
		if (y[obji][i] < smallest[1])
			smallest[1] = y[obji][i];
	}

	double xsize, ysize;
	xsize = biggest[0] - smallest[0];
	ysize = biggest[1] - smallest[1];

	// Make "delta matrix"
	double dmat[3][3];
	double ddmat[3][3];
	M2d_make_identity(dmat);
	M2d_make_identity(ddmat);

	// Translate
	printf("\nstarting translation...\n");
	M2d_make_identity(dmat);
	M2d_make_identity(ddmat);
	M2d_make_translation(
		ddmat, -(smallest[0] + xsize / 2), -(smallest[1] + ysize / 2));
	printf("dmat:\n");
	M2d_print_mat(dmat);
	printf("ddmat:\n");
	M2d_print_mat(ddmat);
	M2d_mat_mult(dmat, ddmat, dmat);
	printf("translation added...\n");
	printf("dmat:\n");
	M2d_print_mat(dmat);

	// Find Scaling Coefficient, Scale
	double xc, yc, c;
	xc = screen.x / xsize;
	yc = screen.y / ysize;
	if (xc < yc) {
		c = xc;
	} else {
		c = yc;
	}
	printf("\nstarting scaling...\n");
	M2d_make_identity(ddmat);
	M2d_make_scaling(ddmat, c, c);
	printf("dmat:\n");
	M2d_print_mat(dmat);
	printf("ddmat:\n");
	M2d_print_mat(ddmat);
	M2d_mat_mult(dmat, ddmat, dmat);
	printf("scaling added...\n");
	printf("dmat:\n");
	M2d_print_mat(dmat);

	// Translate
	printf("\nstarting translation...\n");
	M2d_make_identity(ddmat);
	M2d_make_translation(ddmat, screen.x / 2, screen.y / 2);
	printf("dmat:\n");
	M2d_print_mat(dmat);
	printf("ddmat:\n");
	M2d_print_mat(ddmat);
	M2d_mat_mult(dmat, ddmat, dmat);
	printf("translation added...\n");
	printf("dmat:\n");
	M2d_print_mat(dmat);

	// Apply Delta Matrix
	M2d_mat_mult_points(
		x[obji], y[obji], dmat, x[obji], y[obji], numpoints[obji]);
	// printf("points multiplied...\n");
}