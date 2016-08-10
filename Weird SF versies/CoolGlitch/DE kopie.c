// OS X compilation (something similar for linux I guess, but with other binary paths)
// gcc -Wall -g DE.c -I/usr/local/include/cairo -L/usr/local/lib/ -lcairo -o DE

// To shared library:
// gcc -I/usr/local/include/cairo -L/usr/local/lib/ -lcairo  -Wall -g -fPIC -c DE.c
// gcc -I/usr/local/include/cairo -L/usr/local/lib/ -lcairo -shared -o sf_frame_lib.so DE.o


/* DISPLAY ELEMENTS	 6 Feb. 90 18:00
			definitions */
//#include <dos.h>
//#include <graphics.h>
#include <math.h>
#include <cairo.h>
#include <stdlib.h>
#include <stdio.h>
#include "myconst.h"
#include "myext.h"
#include "time.h"
#include "myvars.h"
#include <string.h> 
//#include "myvars.h"
#define TEXT_HEIGHT 4 // The height of character "h" in pixels in Cairo (with monospace font)
#define TEXT_WIDTH 3 // The height of character "h" in pixels (with monospace font)
#define SF_YELLOW 1.0, 1.0, 0.33
#define SF_GREEN 0.0, 0.66, 0.0
#define SF_BLUE 0.33, 1.0, 1.0
#define SF_ORANGE 1.0, 0.33, 0.33
#define WINDOW_HEIGHT 240
#define WINDOW_WIDTH 320

//#include <boost/python/module.hpp>
//#include <boost/python/def.hpp>
//#include <boost/python/list.hpp>

/*									*/
/*	OPEN GRAPHICS: Initializes the graphics system and reports 	*/
/*	any errors which occured.					*/
/*									*/


void Open_Graphics(void)
{
//	int xasp,yasp;
//
//	// All old and should be replaced. (or probably is unneeded)
//	/* =	-	=	-	=	-	=	-	=	-	=	-	=	-	=	-	=	-	=	-	 */
//	/* Imported from graphics.h */
////	GraphDriver = DETECT;
//	/* Request auto-detection	*/
//	/*GraphMode=EGAHI;*/
////	initgraph( &GraphDriver, &GraphMode, "" );
////	ErrorCode = graphresult();		/* Read result of initialization*/
////	if( ErrorCode != grOk ){		/* Error occured during init	*/
////	exit( 1 );
////	}
//
//	//getpalette( &palette );		/* Read the palette from board	*/
//
//
//	// Lol, max color is probably in the millions now
////	MaxColors = getmaxcolor() + 1;	/* Read maximum number of colors*/
////
////	getaspectratio( &xasp, &yasp );	/* read the hardware aspect	*/
////	AspectRatio = (double)xasp / (double)yasp; /* Get correction factor	*/
////	GraphSqrFact=MaxX*AspectRatio/MaxY;		 /* for EGA cases */
////	setwritemode(XOR_PUT);
//	/* =	-	=	-	=	-	=	-	=	-	=	-	=	-	=	-	=	-	=	-	 */
}

// Returns the cario surface for drawing
void Initialize_Graphics(void)
{
	int Height,OldMaxX;
//	int t,t1; // t is unused
	int t1;
	int x,dx;

	MaxX = WINDOW_WIDTH;
	MaxY = WINDOW_HEIGHT;			/* Originally read the size of the screen	*/

	//MaxX is going to be MaxY eventually (I know this sounds weird but that is how the original
	// code works
//	int side_panel_size=(MaxX - MaxY)/2;

	surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, MaxX, MaxY);
	SF_canvas = cairo_create(surface);
	cairo_set_antialias(SF_canvas, CAIRO_ANTIALIAS_NONE);
//	cairo_set_line_cap(SF_canvas, CAIRO_LINE_CAP_SQUARE);
	// Maybe move down after the final init of MaxX
	
//	cairo_set_line_width(SF_canvas, 10); // Line width equal to one pixel
//	cairo_set_line_width(SF_canvas, (239.1 * 1) / ((double) MaxY * 1));
	cairo_set_line_width(SF_canvas, (239.9 * 1) / ((double) MaxY * 1));
	// Init to empty paths
	cairo_path_t *empty_path = cairo_copy_path(SF_canvas);
	PrevShip = empty_path;
	PrevMissile = empty_path;
	PrevFort = empty_path;
	PrevMine = empty_path;

	// Cairo uses a different coordinate system than graphics.h, so we reflect Cairo's through
	// the x-asis to make it equal to that of graphics.h.
	cairo_matrix_t x_reflection_matrix;	
	// Reflecting it however means that text will also be reflected. We therefore also use a 
	// reflection matrix for drawing fonts to reflect text back. 
	cairo_matrix_t font_reflection_matrix;
	// We need the options to turn off font anti-aliasing
	font_options = cairo_font_options_create();
	
	cairo_matrix_init_identity(&x_reflection_matrix);
//	x_reflection_matrix.yy = -1.0;
	cairo_set_matrix(SF_canvas, &x_reflection_matrix);

	// Mirror font back
	cairo_set_font_size(SF_canvas, 5.5); // 15.4 maximizes visibility while minimizing size
//	cairo_set_font_size(SF_canvas, 5.9);
	cairo_get_font_matrix(SF_canvas, &font_reflection_matrix);
//	font_reflection_matrix.yy = font_reflection_matrix.yy * -1;
//	font_reflection_matrix.x0 += side_panel_size; // see (1)


	cairo_set_font_matrix(SF_canvas, &font_reflection_matrix);
	// Translate negative height down because the reflection draws on top of the drawing surface
	// (i.e. out of frame, directly on top of the frame)

	// (1) Also translate the matrix over the x-axis to emulate the fact that DOS places the SF 
	// square in the middle.
//	cairo_translate(SF_canvas, side_panel_size, -MaxY);
//	cairo_translate(SF_canvas, 0, -MaxY);

	// Turning off anti-alaising
	cairo_get_font_options(SF_canvas, font_options);
	cairo_font_options_set_antialias(font_options, CAIRO_ANTIALIAS_NONE);
	cairo_set_font_options(SF_canvas, font_options);
	cairo_select_font_face(SF_canvas,"Helvetica",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);


	// clears the screen, probably the dos screen, and sets the current graphics write
	// pointer to (0,0) 
	//	cleardevice();

	//	The "textheight" function returns the height of a string in pixels.
//	Height=textheight("H"); /* Get basic text height */
	Height = TEXT_HEIGHT;

 	OldMaxX=MaxX;
  t1=4*Height;

  Panel_Y_End=MaxY;
  Panel_Y_Start=MaxY-t1+2;
  MaxY_Panel=Panel_Y_End-Panel_Y_Start;

  MaxY=MaxY-t1;
	MaxX=MaxY;

	// Any modern graphics library should handle this "if" statements themselves, if needed at
	// all because we don't really need to know anymore wether or not we are on a vga display.
	// This aspect ratio stuff has to do with the fact if your display has square pixels or not.
	// AspectRatio is defined in opengraphics. Pixels are always square nowadays
//	if(AspectRatio==1.0) /* VGA HI */ square pixel ratio
//	MaxX=MaxY;
//	else	/* for all others */
//	{
//		MaxX=MaxX*AspectRatio;	/********* MaxX and MaxY give a square */
//		MaxX=MaxX-t1/AspectRatio;	/******** less two panel lines */
//	}
	Xmargin=OldMaxX/2-MaxX/2;
	cairo_translate(SF_canvas, Xmargin, 0);

	// -- void setviewport(int left, int top, int right, int bottom, int clip);
	// setviewport function is used to restrict drawing to a particular portion on the screen. 	// For example "setviewport(100 , 100, 200, 200, 1)" will restrict our drawing activity 
	// inside the rectangle(100,100, 200, 200).
	//
	// left, top, right, bottom are the coordinates of main diagonal of rectangle in which we wish to restrict our drawing. Also note that the point (left, top) becomes the new origin.
//	setviewport( Xmargin, 0, Xmargin+MaxX, MaxY, 1);

	if(Game_Type==SPACE_FORTRESS)
	{			 /* set data value locations for space fortress */
		dx=MaxX/8;
		Points_X=x=2*TEXT_WIDTH;
		x=x+dx; Control_X=x;
		x=x+dx; Velocity_X=x;
		x=x+dx; Vulner_X=x;
		x=x+dx; IFF_X=x;
		x=x+dx; Interval_X=x;
		x=x+dx; Speed_X=x;
		x=x+dx; Shots_X=x;
	}
	else /* set data value locations for aiming test */
	{
		dx=MaxX/3;
		Mines_X=x=MaxX/6-2*TEXT_WIDTH;
		x=x+dx;	 Speed_X=x;
		x=x+dx-TEXT_WIDTH; Score_X=x;
	}
/* set graphics eraser is done in main */
}

void Close_Graphics()
{
  cairo_destroy(SF_canvas);
	cairo_font_options_destroy(font_options);
  cairo_surface_destroy(surface);
}

float Fcos(int Headings_Degs) /* compute cos of 0 - 359 degrees */
{
		float arc;
		arc=Headings_Degs*ARC_CONV;
		return(cos(arc));
}

float Fsin(int Headings_Degs) /* compute sin of 0 - 359 degrees */
{
	float arc;
	arc=Headings_Degs*ARC_CONV; /* convert degrees to radians */
	return(sin(arc));
}

void snapCoords(cairo_t *canvas, int x, int y)
{
//		printf("Bef: %f %f\n", x, y);
//    cairo_user_to_device(canvas, &x, &y);
//		printf("After: %d %d\n", x, y);
//		&x = round(x) + 0.5;
//		&y = round(y) + 0.5;
//		cairo_device_to_user
//		printf("After [2]: %d %d\n", x, y);
}

void cairo_line(cairo_t *canvas, int x_1, int y_1, int x_2, int y_2)
{
//	snapCoords(canvas, &x1, &y1 );
	
	double x1 = (double) x_1;
	double y1 = (double) y_1;
	double x2 = (double) x_2;
	double y2 = (double) y_2;

	// This code generates straighter and sharper lines, but also drops parts of objects for some 
	// reason
//	printf("Bef: %f %f\n", x1, y1); //	cairo_user_to_device(canvas, &x1, &y1);
//	printf("After: %f %f\n", x1, y1);
//	x1 = round(x1) + 0.5;
//	y1 = round(y1) + 0.5;
//	printf("After [2]: %f %f\n", x1, y1);
//	cairo_device_to_user(canvas, &x1, &y1);
//	printf("After [3]: %f %f\n\n\n", x1, y1);
//
//	printf("Bef: %f %f\n", x2, y2); //	cairo_user_to_device(canvas, &x2, &y2);
//	printf("After: %f %f\n", x2, y2);
//	x2 = round(x2) + 0.5;
//	y2 = round(y2) + 0.5;
//	printf("After [2]: %f %f\n", x2, y2);
//	cairo_device_to_user(canvas, &x2, &y2);
//	printf("After [3]: %f %f\n\n\n", x2, y2);
	cairo_move_to(canvas, x1, y1);
	cairo_line_to(canvas, x2, y2);

}

void cairo_text_at(cairo_t *canvas, int x, int y, const char *string)
{
	cairo_move_to(canvas, x, y); 
	cairo_show_text(canvas, string);
}


// Clip within the bounding box of the current_path()
void clip_path_rect(cairo_t *canvas)
{
	double x1;
	double y1;
	double x2;
	double y2;
	cairo_path_extents(canvas,&x1,&y1,&x2,&y2);
	cairo_path_t *ol_path = cairo_copy_path(canvas);
	cairo_new_path(canvas); 
	// Create the bounding box (somewhat 
	cairo_rectangle(canvas, x1-1, y1-1, (x2-x1)+1, (y2-y1)+1);
//	cairo_set_source_rgb(canvas, 1, 0, 0);
//	cairo_stroke_preserve(canvas);
	cairo_clip(canvas);
	// Restore the old path

	cairo_append_path(canvas,ol_path);
}

	// Clears the pixels on the previous path, i.e. sets them to black
void clear_prev_path(cairo_t *canvas, cairo_path_t *prevPath) 
{
	cairo_set_source_rgb(canvas, 0, 0, 0);
	cairo_new_path(canvas);
	cairo_append_path(canvas, prevPath);
	clip_path_rect(canvas);
	cairo_stroke(canvas);
	cairo_reset_clip(canvas);
}



// Cleans all the previous paths from the context for the objects in need of an update
void clean()
{
//	cairo_save(SF_canvas);
	if (Ship_Should_Update) 
	{
		clear_prev_path(SF_canvas, PrevShip);
	}
	if (Mine_Should_Update) 
	{
		clear_prev_path(SF_canvas, PrevMine);
	}
	if (Fort_Should_Update)
	{
		clear_prev_path(SF_canvas, PrevMine);
	}
	if (Missile_Should_Update)
	{
		// Add code that allows for multiple missiles
		clear_prev_path(SF_canvas, PrevMissile);
	}
//	cairo_restore(SF_canvas);
	cairo_new_path(SF_canvas);
	
}

void update_drawing()
{
//	cairo_save(cr);
	if (Ship_Should_Update)
	{
//		Ship_X_Pos = 096.000000;
//		Ship_Y_Pos = 50.000000;
//		Ship_Headings = Fort_Headings;
//		printf("Ship_Headings: %d \n", Ship_Headings);
//		printf("Fort_Headings: %d \n", Fort_Headings);
		Draw_Ship(Ship_X_Pos,Ship_Y_Pos,Ship_Headings,SHIP_SIZE_FACTOR*MaxX);
		clip_path_rect(SF_canvas);
		cairo_stroke(SF_canvas);
		cairo_reset_clip(SF_canvas);
	}
	if (Fort_Should_Update)
	{
		Draw_Fort(MaxX/2,MaxY/2,Fort_Headings,FORT_SIZE_FACTOR*MaxX);
		clip_path_rect(SF_canvas);
		cairo_stroke(SF_canvas);
		cairo_reset_clip(SF_canvas);
	}
	if (Missile_Should_Update)
	{
//		Missile_X = 119.000000;
//		Missile_Y = 30.000000;
		Missile_Heading = 51;
		Draw_Missile(Missile_X, Missile_Y, Missile_Heading, MISSILE_SIZE_FACTOR*MaxX);
		clip_path_rect(SF_canvas);
		cairo_stroke(SF_canvas);
		cairo_reset_clip(SF_canvas);
	}
	if (Mine_Should_Update)
	{
//		Mine_X_Pos = 50;
//		Mine_Y_Pos = 50;
		Draw_Mine(Mine_X_Pos,Mine_X_Pos,MINE_SIZE_FACTOR*MaxX);
		clip_path_rect(SF_canvas);
		cairo_stroke(SF_canvas);
		cairo_reset_clip(SF_canvas);
	}
//	cairo_restore(cr);	
}

void Draw_Frame()
{
	int Height;
//	int t,t1,svcolor; // All unused

	int x,y,dx;


	// See initialize_graphics for description 
//	Height=textheight("H");		/* Get basic text height */
		Height = TEXT_HEIGHT;

	// removes anything on the screen
//	cleardevice();
 	// FRAME_COLOR is the color of the green border
//	setcolor(FRAME_COLOR);
	cairo_set_source_rgb(SF_canvas, SF_GREEN);

	/* handle panel */
	// See init graphics for description of this function
//	setviewport( Xmargin, Panel_Y_Start, Xmargin+MaxX, Panel_Y_End, 1);
	// Emulate this setviewport call with a matrix translation
	cairo_translate(SF_canvas, 0, Panel_Y_Start);
	
	/* data panel in screen global coordinates */

	// Bottom panel? yes bottom panel
//  void rectangle(int left, int top, int right, int bottom);
//	rectangle(0,0,MaxX,MaxY_Panel);
	cairo_rectangle(SF_canvas, 0, 0, MaxX, MaxY_Panel);
	// The line function is used to draw a line from a point(x1,y1) to point(x2,y2) 
	// void line(int x1, int y1, int x2, int y2);
	
//	line(0,2*Height,MaxX,2*Height);
	// The line on top of the info panel
	cairo_line(SF_canvas, 0, 2*Height, MaxX, 2*Height);
	cairo_stroke(SF_canvas);
	
			/* write panel headers */
	if(Game_Type==SPACE_FORTRESS)
	{
			x=2;
			y=4;
			dx=MaxX/8; /* step between two headers */
			// Called somewhere else
			Data_Line=2*Height+4;
			// I guess gprintf(x_pixel, y_pixel, str);
		
//		gprintf ( &x, &y,"	PNTS");
//		x=x+dx; gprintf ( &x, &y," CNTRL");
//		x=x+dx; gprintf ( &x, &y," VLCTY");
//		x=x+dx; gprintf ( &x, &y," VLNER");
//		x=x+dx; gprintf ( &x, &y,"	IFF ");
//		x=x+dx; gprintf ( &x, &y,"INTRVL");
//		x=x+dx; gprintf ( &x, &y," SPEED");
//		x=x+dx; gprintf ( &x, &y," SHOTS");

			cairo_text_at(SF_canvas, x, y, "	PNTS");
			x=x+dx; cairo_text_at(SF_canvas, x, y, " CNTRL");
			x=x+dx; cairo_text_at(SF_canvas, x, y, " VLCTY");
			x=x+dx; cairo_text_at(SF_canvas, x, y, " VLNER");
			x=x+dx; cairo_text_at(SF_canvas, x, y, "	IFF ");
			x=x+dx; cairo_text_at(SF_canvas, x, y, "INTRVL");
			x=x+dx; cairo_text_at(SF_canvas, x, y, " SPEED");
			x=x+dx; cairo_text_at(SF_canvas, x, y, " SHOTS");
	
			/* draw vertical lines between columns */
	
	//		line(x,0,x,MaxY_Panel);
	//		x=x-dx; line(x,0,x,MaxY_Panel);
	//		x=x-dx; line(x,0,x,MaxY_Panel);
	//		x=x-dx; line(x,0,x,MaxY_Panel);
	//		x=x-dx; line(x,0,x,MaxY_Panel);
	//		x=x-dx; line(x,0,x,MaxY_Panel);
	//		x=x-dx; line(x,0,x,MaxY_Panel);
			cairo_line(SF_canvas,x,0,x,MaxY_Panel);
			x=x-dx; cairo_line(SF_canvas,x,0,x,MaxY_Panel);
			x=x-dx; cairo_line(SF_canvas,x,0,x,MaxY_Panel);
			x=x-dx; cairo_line(SF_canvas,x,0,x,MaxY_Panel);
			x=x-dx; cairo_line(SF_canvas,x,0,x,MaxY_Panel);
			x=x-dx; cairo_line(SF_canvas,x,0,x,MaxY_Panel);
			x=x-dx; cairo_line(SF_canvas,x,0,x,MaxY_Panel);
			cairo_stroke(SF_canvas);

	 }
	 else /* frame for aiming test */
	 {
		x=MaxX/6-32;
		y=4;
		dx=MaxX/3;		 /* step between two headers */
		Data_Line=2*Height+4;
	//		gprintf ( &x, &y,"	MINES");
	//	 x=x+dx; gprintf ( &x, &y," SPEED");
	//	 x=x+dx; gprintf ( &x, &y," SCORE");
	
			/* draw vertical lines between columns */
	//	 x=dx;	 line(x,0,x,MaxY_Panel);
	//	 x=x+dx; line(x,0,x,MaxY_Panel);
	
		cairo_text_at(SF_canvas, x, y, "	MINES");
		x=x+dx; cairo_text_at(SF_canvas, x, y, " SPEED");
		x=x+dx; cairo_text_at(SF_canvas, x, y, " SCORE");
	
			/* draw vertical lines between columns */
		x=dx; cairo_line(SF_canvas,x,0,x,MaxY_Panel);
		x=x+dx; cairo_line(SF_canvas,x,0,x,MaxY_Panel);
		cairo_stroke(SF_canvas);

	 }
	
	// setviewport( Xmargin, 0, Xmargin+MaxX, MaxY, 1); /* in screen global coordinates */
	// translate back from the previous viewport call
	cairo_translate(SF_canvas, 0, -Panel_Y_Start);
	// full panel?
//	rectangle(0,0,MaxX,MaxY); 		/* main frame of the viewport */
	cairo_rectangle(SF_canvas,0,0,MaxX,MaxY); // Maybe drop this or something 
 	cairo_stroke(SF_canvas);



	/* set graphics eraser is done in main */
	// Not needed basically
//	setcolor(svcolor); /* restore previous color */
}

// Draws the hexagon around the fortress, maybe drop this? 
void Draw_Hexagone(int X_Center,int Y_Center,int Hex_Outer_Radius)
{
	int Abs_Y;
//	int svcolor;

//	svcolor=getcolor(); /* save present color */
//	setcolor(HEX_COLOR);
	cairo_set_source_rgb(SF_canvas, SF_GREEN);

	Abs_Y=Hex_Outer_Radius*0.866;	/* sin(60)=0.866 */
	// this didn't work for some reason
	cairo_move_to(SF_canvas, X_Center+Hex_Outer_Radius,Y_Center); /* right-hand tip */
	cairo_line_to(SF_canvas, X_Center+Hex_Outer_Radius/2,Y_Center-Abs_Y);
	cairo_line_to(SF_canvas, X_Center-Hex_Outer_Radius/2,Y_Center-Abs_Y);
	cairo_line_to(SF_canvas, X_Center-Hex_Outer_Radius,Y_Center);
	cairo_line_to(SF_canvas, X_Center-Hex_Outer_Radius/2,Y_Center+Abs_Y);
	cairo_line_to(SF_canvas, X_Center+Hex_Outer_Radius/2,Y_Center+Abs_Y);
	cairo_line_to(SF_canvas, X_Center+Hex_Outer_Radius,Y_Center);

//	cairo_line(SF_canvas, X_Center+Hex_Outer_Radius,Y_Center, X_Center+Hex_Outer_Radius/2,Y_Center-Abs_Y);
//	cairo_line(SF_canvas, X_Center-Hex_Outer_Radius/2,Y_Center-Abs_Y, X_Center-Hex_Outer_Radius,Y_Center);
//	cairo_line(SF_canvas, X_Center-Hex_Outer_Radius/2,Y_Center+Abs_Y, X_Center+Hex_Outer_Radius/2,Y_Center+Abs_Y);
//	cairo_line_to(SF_canvas, X_Center+Hex_Outer_Radius,Y_Center);

//	setcolor(svcolor); /* restore previous color */
//	return(0);
}

void Draw_Ship(int x, int y, int Headings, int size)
{
	/* size - is the entire length of the ship */
	int x1,y1;	/* ship's aft location */
	int x2,y2;	/* ship's nose location */
	int xl,yl;	 /* ship's left wing tip location */
	int xr,yr;	 /* ship's right wing tip location */
	int xc,yc;	/* fuselage and wings connecting point */
	int Right_Wing_Headings;
	int Left_Wing_Headings;
//	int svcolor;
//	float tmp;  // Unused

//	svcolor=getcolor(); /* save present color */
//	setcolor(SHIP_COLOR); // yellow
	cairo_set_source_rgb(SF_canvas, SF_YELLOW);

	xc=x;
	yc=y;
	x1=xc-0.5*size*Fsin(Headings);
	y1=yc+0.5*size*Fcos(Headings);
	x2=xc+0.5*size*Fsin(Headings);
	y2=yc-0.5*size*Fcos(Headings);
	cairo_line(SF_canvas,x1,y1,x2,y2);
	Right_Wing_Headings=Headings+135;
	if(Right_Wing_Headings>359) Right_Wing_Headings=Right_Wing_Headings-360;
	Left_Wing_Headings=Headings+225;
	if(Left_Wing_Headings>359) Left_Wing_Headings=Left_Wing_Headings-360;
	xr=xc+0.707*size*Fsin(Right_Wing_Headings);
	yr=yc-0.707*size*Fcos(Right_Wing_Headings);
	cairo_line(SF_canvas,xc,yc,xr,yr);
	xl=xc+0.707*size*Fsin(Left_Wing_Headings);
	yl=yc-0.707*size*Fcos(Left_Wing_Headings);
	cairo_line(SF_canvas,xc,yc,xl,yl);
	PrevShip = cairo_copy_path(SF_canvas);
//	setcolor(svcolor); /* restore previous color */
//	return(0);
}

void Draw_Fort (int x, int y, int Headings, int size )
{
	int x1,y1;	 /* fort's aft location */
	int x2,y2;	 /* fort's nose location */
	int xl,yl;	 /* ship's left wing tip location */
	int xr,yr;	 /* ship's right wing tip location */
	int xc,yc;	 /* fuselage and wings connecting point */
	int xrt,yrt;	 /* tip of right wing */
	int xlt,ylt;	 /* tip of left wing */
	int Right_Wing_Headings;
	int Left_Wing_Headings;
	int Right_Wing_Tip_Headings;
	int Left_Wing_Tip_Headings;
//	int svcolor;

//	svcolor=getcolor(); /* save present color */
//	setcolor(FORT_COLOR); // blueee
	cairo_set_source_rgb(SF_canvas, SF_BLUE);
	x1=x;
	y1=y;
	x2=x1+size*Fsin(Headings);
	y2=y1-size*Fcos(Headings);
	cairo_line(SF_canvas,x1,y1,x2,y2);
	xc=x1+(x2-x1)*0.5;
	yc=y1+(y2-y1)*0.5;
	Right_Wing_Headings=Headings+90;
	if(Right_Wing_Headings>359) Right_Wing_Headings=Right_Wing_Headings-360;
	Left_Wing_Headings=Headings+270;
	if(Left_Wing_Headings>359) Left_Wing_Headings=Left_Wing_Headings-360;
	xr=xc+0.4*size*Fsin(Right_Wing_Headings)+0.5;
	yr=yc-0.4*size*Fcos(Right_Wing_Headings)+0.5;
	cairo_line(SF_canvas,xc,yc,xr,yr);
	xl=xc+0.4*size*Fsin(Left_Wing_Headings)+0.5;
	yl=yc-0.4*size*Fcos(Left_Wing_Headings)+0.5;
	cairo_line(SF_canvas,xc,yc,xl,yl);
	Right_Wing_Tip_Headings=Right_Wing_Headings+90;
	if(Right_Wing_Tip_Headings>359) Right_Wing_Tip_Headings=
						 Right_Wing_Tip_Headings-360;
	xrt=xr+0.5*size*Fsin(Right_Wing_Tip_Headings)+0.5;
	yrt=yr-0.5*size*Fcos(Right_Wing_Tip_Headings)+0.5;
	cairo_line(SF_canvas,xr,yr,xrt,yrt);
	Left_Wing_Tip_Headings=Right_Wing_Tip_Headings;
	xlt=xl+0.5*size*Fsin(Left_Wing_Tip_Headings)+0.5;
	ylt=yl-0.5*size*Fcos(Left_Wing_Tip_Headings)+0.5;
	cairo_line(SF_canvas,xl,yl,xlt,ylt);
	PrevFort = cairo_copy_path(SF_canvas);
//	setcolor(svcolor); /* restore previous color */
}

void Draw_Mine (int x, int y, int size)	/* x,y is on screen center location
					size is half of horizontal diagonal */
{
//	int svcolor;

//	svcolor=getcolor(); /* save present color */
//	setcolor(MINE_COLOR); // maybe different than blue for easier recogniztion?
	cairo_set_source_rgb(SF_canvas, SF_BLUE);

	cairo_move_to(SF_canvas,x-size,y);
	cairo_line_to(SF_canvas,x,y-1.18*size);	 /* 1.3/1.1=1.18 */
	cairo_line_to(SF_canvas,x+size,y);
	cairo_line_to(SF_canvas,x,y+1.18*size);
	cairo_line_to(SF_canvas,x-size,y);
	PrevMine = cairo_copy_path(SF_canvas);
//	setcolor(svcolor); /* restore previous color */
}

void Draw_Missile (int x, int y, int Headings, int size)
{
	int x1,y1;	/* ship's aft location */
	int x2,y2;	/* ship's nose location */
	int xl,yl;	 /* ship's left wing tip location */
	int xr,yr;	 /* ship's right wing tip location */
	int xc,yc;	/* fuselage and wings connecting point */
	int Right_Wing_Headings;
	int Left_Wing_Headings;
//	int svcolor;

//	svcolor=getcolor(); /* save present color */
//	setcolor(MISSILE_COLOR);
	cairo_set_source_rgb(SF_canvas, 1, 0, 0);
	x1=x;
	y1=y;
	x2=x1+size*Fsin(Headings);
	y2=y1-size*Fcos(Headings);
	cairo_line(SF_canvas, x1,y1,x2,y2);
	xc=x2;
	yc=y2;
	Right_Wing_Headings=Headings+135;
	if(Right_Wing_Headings>359) Right_Wing_Headings=Right_Wing_Headings-360;
	Left_Wing_Headings=Headings+225;
	if(Left_Wing_Headings>359) Left_Wing_Headings=Left_Wing_Headings-360;
	xr=xc+0.25*size*Fsin(Right_Wing_Headings);
	yr=yc-0.25*size*Fcos(Right_Wing_Headings);
	cairo_line(SF_canvas,xc,yc,xr,yr);
	xl=xc+0.25*size*Fsin(Left_Wing_Headings);
	yl=yc-0.25*size*Fcos(Left_Wing_Headings);
	cairo_line(SF_canvas,xc,yc,xl,yl);
	PrevMissile = cairo_copy_path(SF_canvas);
//	setcolor(svcolor); /* restore previous color */
}

void Draw_Shell(int x, int y, int Headings, int size)
{
	int x1,y1;	/* shell's aft location */
	int x2,y2;	/* shell's nose location */
	int xl,yl;	 /* shell's left tip location */
	int xr,yr;	 /* shell's right tip location */
	int Right_Tip_Headings;
	int Left_Tip_Headings;
//	int svcolor;

//	svcolor=getcolor(); /* save present color */
//	setcolor(SHELL_COLOR);
	cairo_set_source_rgb(SF_canvas, 1, 0, 0);
	x1=x;
	y1=y;
	x2=x1+size*Fsin(Headings);
	y2=y1-size*Fcos(Headings);
	Right_Tip_Headings=Headings+30;
	if(Right_Tip_Headings>359) Right_Tip_Headings=Right_Tip_Headings-360;
	Left_Tip_Headings=Headings+330;
	if(Left_Tip_Headings>359) Left_Tip_Headings=Left_Tip_Headings-360;
	xr=x1+0.4*size*Fsin(Right_Tip_Headings);
	yr=y1-0.4*size*Fcos(Right_Tip_Headings);
	xl=x1+0.4*size*Fsin(Left_Tip_Headings);
	yl=y1-0.4*size*Fcos(Left_Tip_Headings);
	cairo_move_to(SF_canvas,x1,y1);
	cairo_line_to(SF_canvas,xl,yl);
	cairo_line_to(SF_canvas,x2,y2);
	cairo_line_to(SF_canvas,xr,yr);
	cairo_line_to(SF_canvas,x1,y1);
//	setcolor(svcolor); /* restore previous color */
}

int Find_Headings(int x1,int y1,int x2,int y2)
{
//	int quadrant;	// Unused
	double arcsinalfa;
	double b;
	double a;
	arcsinalfa=abs(x1-x2);
	a=pow(x1-x2,2)+pow(y1-y2,2);
	b=sqrt(a);
	arcsinalfa=asin(arcsinalfa/b);
	if (x1<x2)
		if (y1>y2) /* quadrant=1 */ return(arcsinalfa*57.3+0.5);
	else
		/* quadrant=2 */ return(180-arcsinalfa*57.3+0.5);
	 else
	 if (y1>y2) /* quadrant=4 */ return(360-arcsinalfa*57.3+0.5);
	else
		/* quadrant=3 */ return(180+arcsinalfa*57.3+0.5);
}


void start_drawing()
{
	Initialize_Graphics();
	Draw_Frame();
	Ship_Should_Update = 1;
	Mine_Should_Update = 1;
	Fort_Should_Update = 1;
	Missile_Should_Update = 1;
	srand(time(NULL));

	Ship_X_Pos = 096.000000;
	Ship_Y_Pos = 50.000000;
	Missile_X = 119.000000;
	Missile_Y = 30.000000;
	Mine_X_Pos = 50;
	Mine_Y_Pos = 50;
	Ship_Headings = Fort_Headings;
}

void stop_drawing()
{
	Close_Graphics();	
}

int move_update()
{
	return (int) ((rand() % (3)) - 1);
}

unsigned char* update_frame()
//int main(int argc, char *argv[])
{
	if (rand() % 2)
	{
		Ship_X_Pos = (Ship_X_Pos + move_update()) % MaxX;
	}
	if (rand() % 2)
	{
		Ship_Y_Pos = (Ship_Y_Pos + rand() % 2) % MaxY;
	}
	if (rand() % 2)
	{
		Ship_Headings = (Ship_Headings + move_update()) % 360;
	}
	Fort_Headings = (Fort_Headings + 1) % 360;
	if (rand() % 2)
	{
		Mine_X_Pos = (Mine_X_Pos + move_update()) % MaxX;
	}
	if (rand() % 2)
	{
		Mine_Y_Pos = (Mine_Y_Pos + move_update()) % MaxY;
	}
	
	clean();
	Draw_Hexagone(MaxX/2,MaxY/2,SMALL_HEXAGONE_SIZE_FACTOR*MaxX);
	clip_path_rect(SF_canvas);
	cairo_stroke(SF_canvas);
	cairo_reset_clip(SF_canvas);
	update_drawing();
	return cairo_image_surface_get_data(surface);
}

//int main(int argc, char *argv[])
//{
//	start_drawing();
//	update_frame();
//	stop_drawing();
//}




