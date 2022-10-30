#include "FPToolkit.c"
#include <math.h>
#include <stdio.h>

struct Vector2 {
  double x;
  double y;
};

struct Vector3 {
  double x;
  double y;
  double z;
};

struct Polygon {
  int size;
  double x[59000];
  double y[59000];
  double z[59000];
};

struct Vector2 screen;
struct Vector2 snap_offsets;

double check_line(double c1[2], double c2[2], double scany);

int click_and_save(double x[], double y[]) {
  float done_area_y = 20;
  G_fill_rectangle(0, 0, 400, done_area_y);
  int i = 0;
  double in[2];
  G_wait_click(in);
  while (in[1] > done_area_y) {
    x[i] = in[0];
    y[i] = in[1];
    G_circle(in[0], in[1], 3);
    G_wait_click(in);
    if (in[1] > done_area_y) {
      G_line(in[0], in[1], x[i], y[i]);
    } else {
      G_line(x[i], y[i], x[0], y[0]);
    }
    i++;
  }
  return i;
}

void print_xy(double x[], double y[], int n) {
  printf("Polygon Coordinates:\n");
  for (int i = 0; i < n; i++) {
    printf("%8.3lf, %8.3lf\n", x[i], y[i]);
  }
}
void sort(double a[], int n) {
  double t, r;
  int p;
  // go through array starting at the end
  for (int i = n - 1; i >= 0; i--) {
    r = 0;
    // go through array for largest number
    for (int j = 0; j <= i; j++) {
      if (a[j] > r) {
        p = j;    // store index of largest number
        r = a[j]; // store largest number
      }
    }
    t = a[i];
    a[i] = r;
    a[p] = t;
  }
}

int poly_fill(struct Polygon poly, double rgb[3]) {
  G_rgb(rgb[0], rgb[1], rgb[2]);
  int ymin, ymax;

  // initialize line array
  double lines[1000][2][2];

  ymin = poly.y[0];
  ymax = ymin;

  // store lines
  for (int i = 0; i < poly.size - 1; i++) {
    if (poly.y[i + 1] < ymin)
      ymin = poly.y[i + 1];

    if (poly.y[i + 1] > ymax)
      ymax = poly.y[i + 1];

    lines[i][0][0] = poly.x[i];
    lines[i][0][1] = poly.y[i];

    lines[i][1][0] = poly.x[i + 1];
    lines[i][1][1] = poly.y[i + 1];

    // Draw lines to make shape cleaner
    G_line(lines[i][0][0], lines[i][0][1], lines[i][1][0], lines[i][1][1]);
  }

  lines[poly.size - 1][0][0] = poly.x[poly.size - 1];
  lines[poly.size - 1][0][1] = poly.y[poly.size - 1];
  lines[poly.size - 1][1][0] = poly.x[0];
  lines[poly.size - 1][1][1] = poly.y[0];
  // Draw final Line
  G_line(lines[poly.size - 1][0][0], lines[poly.size - 1][0][1],
         lines[poly.size - 1][1][0], lines[poly.size - 1][1][1]);

  // start scan line loop
  for (int i = ymin; i < ymax; i++) {
    double xhit[1000];

    // index of xhit. Resets on every new scan line
    int pint = 0;

    for (int j = 0; j < poly.size; j++) {
      // check_line() called
      // takes start & end points, a reference to an array of intersection
      // points, and current scan line y value
      double test = check_line(lines[j][0], lines[j][1], (double)i);
      if (test != 0) {
        xhit[pint++] = test;
      }
    }

    if (pint >= 2) {
      // printf("i: %4i\n", pint);
      sort(xhit, pint);
      for (int j = 1; j < pint; j++) {
        for (int k = 1; k < pint; k++) {
          double x0 = xhit[k - 1];
          double x1 = xhit[k];
          if (x0 > x1) {
            xhit[k] = x1;
            xhit[k - 1] = x0;
          }
        }
      }

      for (int j = 0; j < pint; j += 2) {
        G_line(xhit[j], i, xhit[j + 1], i);
      }
    }
  }

  return 0;
}

int xy_poly_fill(double x[], double y[], int size, double rgb[3]) {
  int ymin = y[0], ymax = y[0];

  G_rgb(rgb[0], rgb[1], rgb[2]);

  // initialize line array
  double lines[1000][2][2];

  // store lines
  for (int i = 0; i < size - 1; i++) {
    if (y[i + 1] < ymin)
      ymin = y[i + 1];
    if (y[i + 1] > ymax)
      ymax = y[i + 1];

    lines[i][0][0] = x[i];
    lines[i][0][1] = y[i];

    lines[i][1][0] = x[i + 1];
    lines[i][1][1] = y[i + 1];
  }
  lines[size - 1][0][0] = x[size - 1];
  lines[size - 1][0][1] = y[size - 1];
  lines[size - 1][1][0] = x[0];
  lines[size - 1][1][1] = y[0];

  // start scan line loop
  for (int i = 0; i < screen.y; i++) {
    // if (i % 10 == 0)
    //	G_wait_key();
    double xhit[1000];
    // index of xhit. Resets on every new scan line
    int pint = 0;

    for (int j = 0; j < size; j++) {
      // check_line() called
      // takes start & end points, a reference to an array of intersection
      // points, and current scan line y value
      double test = check_line(lines[j][0], lines[j][1], (double)i);
      if (test != 0) {
        xhit[pint++] = test;
      }
    }

    if (pint >= 2) {
      // printf("i: %4i\n", pint);
      sort(xhit, pint);
      for (int j = 1; j < pint; j++) {
        for (int k = 1; k < pint; k++) {
          double x0 = xhit[k - 1];
          double x1 = xhit[k];
          if (x0 > x1) {
            xhit[k] = x1;
            xhit[k - 1] = x0;
          }
        }
      }

      for (int j = 0; j < pint; j += 2) {
        // printf("draw ( %3.0lf, %3.0lf )\n", xhit[j], xhit[j + 1]);
        G_line(xhit[j], i, xhit[j + 1], i);
      }
    }
  }

  return 0;
}

int sub_xy_poly_fill(double x[], double y[], int lo, int hi, double rgb[3]) {
  G_rgb(rgb[0], rgb[1], rgb[2]);
  // initialize line array
  double lines[1000][2][2];
  int size = hi - lo;
  printf("Polygon Size: %i\n", size);
  // store lines
  for (int i = lo; i < hi - 1; i++) {
    lines[i][0][0] = x[i];
    lines[i][0][1] = y[i];

    lines[i][1][0] = x[i + 1];
    lines[i][1][1] = y[i + 1];
  }
  lines[size - 1][0][0] = x[hi - 1];
  lines[size - 1][0][1] = y[hi - 1];
  lines[size - 1][1][0] = x[lo];
  lines[size - 1][1][1] = y[lo];

  // start scan line loop
  for (int i = 0; i < screen.y; i++) {
    // if (i % 10 == 0)
    //	G_wait_key();
    double xhit[1000];
    // index of xhit. Resets on every new scan line
    int pint = 0;

    for (int j = 0; j < size; j++) {
      // check_line() called
      // takes start & end points, a reference to an array of intersection
      // points, and current scan line y value
      double test = check_line(lines[j][0], lines[j][1], (double)i);
      if (test != 0) {
        xhit[pint++] = test;
      }
    }

    if (pint >= 2) {
      // printf("i: %4i\n", pint);
      sort(xhit, pint);
      for (int j = 1; j < pint; j++) {
        for (int k = 1; k < pint; k++) {
          double x0 = xhit[k - 1];
          double x1 = xhit[k];
          if (x0 > x1) {
            xhit[k] = x1;
            xhit[k - 1] = x0;
          }
        }
      }

      for (int j = 0; j < pint; j += 2) {
        // printf("draw ( %3.0lf, %3.0lf )\n", xhit[j], xhit[j + 1]);
        G_line(xhit[j], i, xhit[j + 1], i);
      }
    }
  }

  return 0;
}

double check_line(double c1[2], double c2[2], double scany) {
  struct Vector2 smaller;

  double ybounds[2];

  if (c1[1] > c2[1]) {
    ybounds[0] = c2[1];
    ybounds[1] = c1[1];
  } else {
    ybounds[0] = c1[1];
    ybounds[1] = c2[1];
  }

  if (c1[0] > c2[0]) {
    smaller.x = c2[0];
    smaller.y = c2[1];
  } else {
    smaller.x = c1[0];
    smaller.y = c1[1];
  }

  if (scany >= ybounds[0] && scany < ybounds[1]) {
    double c, ydiff, x;

    c = fabs(c2[0] - c1[0]) / fabs(c2[1] - c1[1]);
    ydiff = fabs(smaller.y - scany);

    x = (ydiff * c) + smaller.x;

    return x;
  }
  return 0;
}
