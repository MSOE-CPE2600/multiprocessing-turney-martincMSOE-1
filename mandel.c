//  mandel.c
//  Based on example code found here:
//  https://users.cs.fiu.edu/~cpoellab/teaching/cop4610_fall22/project3.html
//
//  Converted to use jpg instead of BMP and other minor changes
//  
///
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "jpegrw.h"
 
typedef struct {
	//pointer for the image
	imgRawImage *img; 
	//set the regions
	double xmin;	
	double xmax;
	double ymin;
	double ymax;
	int max;
	int start;
	int end;
 
}thread_data;
 
// local routines
static int iteration_to_color( int i, int max );
static int iterations_at_point( double x, double y, int max );
static void compute_image( imgRawImage *img, double xmin, double xmax,
									double ymin, double ymax, int max, int threads );
static void* compute_region(void* arg);
static void show_help();
 
 
int main( int argc, char *argv[] )
{
	char c;
 
	// These are the default configuration values used
	// if no command line arguments are given.
	const char *outfile = "mandel.jpg";
	double xcenter = 0;
	double ycenter = 0;
	double xscale = 4;
	double yscale = 0; // calc later
	int    image_width = 1000;
	int    image_height = 1000;
	int    max = 1000;
	//int    children = 10;
	int    threads  = 10;
 
	// For each command line argument given,
	// override the appropriate configuration value.
 
	while((c = getopt(argc,argv,"x:y:s:W:H:m:c:t:o:h"))!=-1) {
		switch(c) 
		{
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				xscale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			// case 'c':
			// 	children = atoi(optarg);
			// 	break;
			case 't':
				threads = atoi(optarg);
				if(threads < 1 || threads > 20) {
					fprintf(stderr, "Number of Threads must be >= 1 or <= 20, Defaulting to 10\n");
					threads = 10;
				}
				break;
			case 'o':
				outfile = optarg;
				break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}

	// Compute yscale based on current xscale and image size
	yscale = xscale / image_width * image_height;

	// Display the configuration of the image.
	printf("mandel: x=%lf y=%lf xscale=%lf yscale=%1f max=%d threads=%d outfile=%s\n",
	       xcenter, ycenter, xscale, yscale, max, threads, outfile);

	// Create a raw image of the appropriate size.
	imgRawImage* img = initRawImage(image_width, image_height);

	// Fill it with black
	setImageCOLOR(img, 0);

	// Compute the Mandelbrot image using threads
	compute_image(img, xcenter - xscale / 2, xcenter + xscale / 2, ycenter - yscale / 2, ycenter + yscale / 2, max, threads);

	// Save the image in the stated file (no extra numbering)
	storeJpegImageFile(img, outfile);

	// free the mallocs
	freeRawImage(img);
	
	return 0;
}
 
 
 
/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/
 
int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;
 
	int iter = 0;
 
	while( (x*x + y*y <= 4) && iter < max ) {
 
		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;
 
		x = xt;
		y = yt;
 
		iter++;
	}
 
	return iter;
}
 
/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/
 
void compute_image(imgRawImage* img, double xmin, double xmax, double ymin, double ymax, int max, int threads)
{
	/* Array for threads and thread data */
	pthread_t thread_arr[threads];
	thread_data t_data[threads];
 
	/* calculate height and width of image */
	int height = img -> height; 		  // (img.height)
	int thread_height = height / threads; // sectioning off "widht" of image into sections for thread calculation
	int extra_thread = height % threads;  // calculating if an off number of threads is present

	int row_start = 0;
 
	/* For every pixel.. */
	for (int i = 0; i < threads; i++) {

		/* Rows variable to increment based on currentr number of rows within thread */
		int rows = thread_height + (i < extra_thread ? 1 : 0);

		
		//assign values to the thread data
		t_data[i].img  = img;
		t_data[i].xmin = xmin;
		t_data[i].xmax = xmax;
		t_data[i].ymin = ymin;
		t_data[i].ymax = ymax;
		t_data[i].max  = max;

		t_data[i].start = row_start;
		t_data[i].end   = row_start + rows;

		// if (i == threads - 1) {
    	// 	t_data[i].end = height;
		// } else {
    	// 	t_data[i].end = (i + 1) * width;
		// }
 
		//create the thread
		pthread_create(&thread_arr[i], NULL, compute_region, &t_data[i]);

		row_start += rows;
	}
 
	/* join all the threads at the end */
	for(int i = 0; i < threads; i++) {
		pthread_join(thread_arr[i], NULL);
	}
 
}
 
void* compute_region(void* input) {
	//assign the data and img
	thread_data* data = (thread_data*)input;
	imgRawImage* img = data -> img;
 
	//calculate the width of region to be computed
	int width = img -> width;

	for (int i = data -> start; i < data -> end; i++) {
    	for (int j = 0; j < width; j++) {
			/* Calculate coordinate for given pixel */
    	    double x = data -> xmin + j*(data -> xmax-data -> xmin)/width;
    	    double y = data -> ymin + i*(data -> ymax-data -> ymin)/img -> height;

			/* Calculate interations at given point */
    	    int iters = iterations_at_point(x,y,data->max);

			/* Set pixel color in bitmap */
    	    setPixelCOLOR(img,i,j,iteration_to_color(iters,data->max));
    	}
	}

	
	return NULL;
}

/*
Convert a iteration number to a color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/
int iteration_to_color( int iters, int max )
{
	int color = 0xFFFFFF*iters/(double)max;
	return color;
}
 
 
/* Show help message */
void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates (X-axis). (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=1000)\n");
	printf("-H <pixels> Height of the image in pixels. (default=1000)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

