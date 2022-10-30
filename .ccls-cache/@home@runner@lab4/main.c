#include <stdio.h>

// Most of what I want is gonna be in multi draw 2.
// I tried putting everything I needed into max poly tools and draw tools.
// Max Poly Tools should have the toolset for drawing up to 10 objects, and
// Max draw tools is my polygon filling algorithm.

//  P.S. I wanna check out
//  https://www.simplilearn.com/tutorials/c-tutorial/enum-in-c

// read, scale, process, render?

#include "FPToolkit.c"
#include "M2d_matrix_toolsS.c"
#include "max_poly_tools.c"
#include <math.h>
#include <stdio.h>

// initialize vars here
#define MAXOBJS 10
#define MAXPTS 59000
#define MAXPOLYS 57500

double centerpoint[MAXOBJS][MAXPOLYS][2];

// total number of points, per object
int numpoints[MAXOBJS];
// every point in an object
double x[MAXOBJS][MAXPTS], y[MAXOBJS][MAXPTS], z[MAXOBJS][MAXPTS];
// the number of polys in each object
int numpolys[MAXOBJS];
// the number of objects
int numobjs;
// the size of each poly
int psize[MAXOBJS][MAXPOLYS];
// the indices of each poly, in order.
int con[MAXOBJS][MAXPOLYS][20];
// the colors of each poly
double poly_rgb[MAXOBJS][MAXPOLYS][3];

void read_object(FILE *f, int obji);
void draw_object(int obji);
void center_and_scale(int obji);
// void stretch_to_fill(int obji);
void rotate(int obji, double t);
void render_h(int obji);

double swidth = 450;
double sheight = 450;

// START # # # # # # # # # # # # # # # # # # # # #
int main(int argc, char **argv) {
  // Initialize graphics

  screen.x = swidth;
  screen.y = sheight;
  G_init_graphics(screen.x, screen.y);

  // Read & Initialize Objects
  for (int i = 1; i < argc; i++) {
    FILE *W;
    W = fopen(argv[i], "r");
    read_object(W, i - 1);

    // stretch_to_fill(i - 1);

    G_rgb(0, 0, 0);
    G_clear();
  }

  render_h(0);

  G_wait_key();

  exit(0);
}

// RENDER H (scale to magnitude 1) # # # # # # # #
void render_h(int obji) {
  double rx[MAXPTS], ry[MAXPTS];
  for (int i = 0; i < numpolys[obji]; i++) {
    for (int j = 0; j < psize[obji][i]; j++) {
      rx[j] = cos(acos(x[obji][con[obji][i][j]] / z[obji][con[obji][i][j]]));
      ry[j] = sin(asin(y[obji][con[obji][i][j]] / z[obji][con[obji][i][j]]));
      printf("point at (%lf, %lf)\n", rx[j], ry[j]);
    }
  }
  printf("beginning draw...\n");
  G_rgb(0, 0, 0.1);
  G_clear();
  // scale virtual camera render to screen
  for (int i = 0; i < numpoints[obji]; i++) {
    rx[i] = (rx[i] * (screen.x / 2.0)) + (screen.x / 2.0);
    ry[i] = (ry[i] * (screen.y / 2.0)) + (screen.y / 2.0);
    printf("point at (%lf, %lf)\n", rx[i], ry[i]);
    G_rgb(1, 0, 0);
    G_fill_circle(rx[i], ry[i], 2);
  }

  /*
for (int i = 0; i < numpolys[obji]; i++) {
// printf("poly %d/%d: \n", i + 1, numpolys[obji]);
double px[20], py[20];
for (int j = 0; j < psize[obji][i]; j++) {
// printf("pt %d/%d ", j + 1, psize[obji][i]);
px[j] = rx[con[obji][i][j]];
py[j] = ry[con[obji][i][j]];
}
// printf("\nfilling...\n");
G_rgb(1,0,0);
G_polygon(px, py, psize[obji][i]);
// my xy_poly_fill produces a segmentation fault after rotation is
// applied xy_poly_fill(px, py, psize[obji][i], poly_rgb[obji][i]);
}
*/
  G_wait_key();
}

// ROTATE  # # # # # # # # # # # # # # # # # # # #
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
  M2d_mat_mult_points(x[obji], y[obji], dmat, x[obji], y[obji],
                      numpoints[obji]);
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
  int unused = fscanf(f, "%d", &numpoints[obji]);

  // Get X and Y coordinates
  for (int i = 0; i < numpoints[obji]; i++) {
    unused = fscanf(f, "%lf %lf %lf", &x[obji][i], &y[obji][i], &z[obji][i]);
  }

  // Get number of polygons
  unused = fscanf(f, "%d", &numpolys[obji]);

  // Get polygon point indices
  for (int i = 0; i < numpolys[obji]; i++) {
    unused = fscanf(f, "%d", &psize[obji][i]);
    for (int j = 0; j < psize[obji][i]; j++) {
      unused = fscanf(f, "%d", &con[obji][i][j]);
    }
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
  double biggest[3] = {0, 0, 0};
  double smallest[3] = {0, 0, 0};

  // Check X and Y coordinates
  biggest[0] = x[obji][0];
  biggest[1] = y[obji][0];
  biggest[2] = z[obji][0];
  smallest[0] = biggest[0];
  smallest[1] = biggest[1];
  smallest[2] = biggest[2];

  for (int i = 1; i < numpoints[obji]; i++) {
    if (x[obji][i] > biggest[0])
      biggest[0] = x[obji][i];
    if (x[obji][i] < smallest[0])
      smallest[0] = x[obji][i];
    if (y[obji][i] > biggest[1])
      biggest[1] = y[obji][i];
    if (y[obji][i] < smallest[1])
      smallest[1] = y[obji][i];
    if (z[obji][i] > biggest[2])
      biggest[2] = z[obji][i];
    if (z[obji][i] < smallest[2])
      smallest[2] = z[obji][i];
  }

  printf("\nShape Dimensions: ( %lf, %lf, %lf ), ( %lf, %lf, %lf )\n",
         smallest[0], smallest[1], smallest[2], biggest[0], biggest[1],
         biggest[2]);
  double xsize, ysize, zsize;
  xsize = biggest[0] - smallest[0];
  ysize = biggest[1] - smallest[1];
  zsize = biggest[2] - smallest[2];

  // I have factored in the 'z' dimension through here. I will not continue
  // right now.

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
// I got rid of stretch to fill