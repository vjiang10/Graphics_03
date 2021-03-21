#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"


/*======== void parse_file () ==========
Inputs:   char * filename
          struct matrix * transform,
          struct matrix * pm,
          screen s
Returns:

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix -
               takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
         ident: set the transform matrix to the identity matrix -
         scale: create a scale matrix,
                then multiply the transform matrix by the scale matrix -
                takes 3 arguments (sx, sy, sz)
         translate: create a translation matrix,
                    then multiply the transform matrix by the translation matrix -
                    takes 3 arguments (tx, ty, tz)
         rotate: create a rotation matrix,
                 then multiply the transform matrix by the rotation matrix -
                 takes 2 arguments (axis, theta) axis should be x y or z
         apply: apply the current transformation matrix to the edge matrix
         display: clear the screen, then
                  draw the lines of the edge matrix to the screen
                  display the screen
         save: clear the screen, then
               draw the lines of the edge matrix to the screen
               save the screen to a file -
               takes 1 argument (file name)
         quit: end parsing

See the file script for an example of the file format

IMPORTANT MATH NOTE:
the trig functions int math.h use radian measure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename, 
                  struct matrix * transform, 
                  struct matrix * edges,
                  screen s) {

  FILE *f;
  char line[256];
  clear_screen(s);

  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");
  
  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    printf(":%s:\n",line);

    // color: cyan 
    color c; c.red = 100; c.green = 10; c.blue = 255;
    double x0, y0, x1, y1, z0, z1;

    // line
    if (!strcmp(line,"line")) {
      fgets(line, 100, f);
      sscanf(line, "%lf %lf %lf %lf %lf %lf", &x0, &y0, &z0, &x1, &y1, &z1);
      add_edge(edges, x0, y0, z0, x1, y1, z1);
    }

    // ident
    else if (!strcmp(line, "ident")) ident(transform);

    // scale
    else if (!strcmp(line, "scale")) {
      fgets(line, 100, f);
      sscanf(line, "%lf %lf %lf", &x0, &y0, &z0);
      // scale matrix 
      struct matrix * scale = make_scale(x0, y0, z0);
      matrix_mult(scale, transform); 
      free_matrix(scale);
    }

    // translate
    else if (!strcmp(line, "move")) {
      fgets(line, 100, f);
      sscanf(line, "%lf %lf %lf", &x0, &y0, &z0);
      // translation matrix
      struct matrix * translate = make_translate(x0, y0, z0);
      matrix_mult(translate, transform);
      free_matrix(translate);
    }

    // rotation
    else if (!strcmp(line, "rotate")) {
      fgets(line, 100, f);
      char axis;
      double theta;
      sscanf(line, "%c %lf", &axis, &theta);
      // convert to radians
      theta = (theta * M_PI) / 180;
      // rotation matrix
      struct matrix * rotate;
      if (axis == 'x') rotate = make_rotX(theta);
      if (axis == 'y') rotate = make_rotY(theta);
      if (axis == 'z') rotate = make_rotZ(theta);
      matrix_mult(rotate, transform);
      free_matrix(rotate);
    }

    // apply
    else if (!strcmp(line, "apply")) matrix_mult(transform, edges);

    // display
    else if (!strcmp(line, "display")) {
      clear_screen(s);
      draw_lines(edges, s, c);
      display(s);
    }

    // save
    else if (!strcmp(line, "save")) {
      fgets(line, 100, f);
      line[strlen(line)-1]='\0';
      clear_screen(s);
      draw_lines(edges, s, c);
      save_extension(s, line);
    }
    
    // quit
    else if (!strcmp(line, "quit")) break;
  } 
}
  
