/******************************************************************************
 * File mandel_movie.c Assignment: Lab 11 (Multiprocessing)
 * Description: Will fork() and invoke mandel a number of times
 *              based off of the number passed into the command line
 * Author: Chris Martin
 * Last edit Date: 11/19/25
 * Compile with make, run with ./movie -p (# of processes) -t (# of threads)
 * ex: ./movie -p 20 -t 10 & ffmpeg -i mandel%d.jpg mandel.mpg for movie
 * 
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    char c;
    int processes = 1;
    int num_frames = 50; // set to 50 for now, can change to dynamic frame number later
    int num_threads = 1; // min 1, max 20

    /* Terminal input parsing */
    while ((c = getopt(argc, argv, "p:t:")) != -1) {
        switch (c) {
        case 'p':
            processes = atoi(optarg);
            break;
        // case 'n':
        //     num_frames = atoi(optarg);
        //     break;
        case 't':
            num_threads = atoi(optarg);
            break;
        default:
            fprintf(stderr, "========= MOVIE PARSING ERROR, EXITING PROGRAM =========\n");
            exit(1);
            break;
        }
    }

    /* Parsing Error Checking */
    // if (processes < 1 ) {
    //     fprintf(stderr, "Number of Processes must be >= 1\n");
    //     return EXIT_FAILURE;
    // }
    if (processes > 50  || processes < 1) { // default to 25 frames if excedes 
        fprintf(stderr, "Number of Processes must be >= 1\n");
        processes = 25;
    }
    if (num_threads > 20 || num_threads < 1) {
        fprintf(stderr, "Number of Threads must be >= 1 or <= 20\n");
        num_threads = 10;
    }

    /* Parameters for zoom (MAKE SURE TO MATCH MANDEL.C) */
    double x_center = 0.286932;
    double y_center = 0.014287;
    double scale    = 0.5;    // initial scale
    double zoom     = 0.8;   // multiply each frame to zoom in

    int active_children = 0;

    for (int i = 0; i < num_frames ; i++) { // for each frame (i), do following for n frames (50)

        /* If we already have max_procs children, wait for one to finish */
        if (active_children == processes) {
            wait(NULL);
            active_children--;
        }

        pid_t pid = fork();

        if (pid == 0) { /* Child process to parse args and call mandel n times*/
            

            char x_str[32], y_str[32], s_str[32], t_str[32], out_str[64];
            sprintf(x_str, "%f", x_center); 
            sprintf(y_str, "%f", y_center); 
            sprintf(s_str, "%f", scale); 
            sprintf(t_str, "%d", num_threads); 
            sprintf(out_str, "mandel%d.jpg", i); 


            execl("./mandel", "mandel",
                  "-x", x_str,
                  "-y", y_str,
                  "-s", s_str,
                  "-m", "1000",
                  "-W", "1000",
                  "-H", "1000",
                  "-t", t_str,
                  "-o", out_str,
                  (char *) NULL);

            /* If execl returns, there was an error */
            perror("execl");
            _exit(EXIT_FAILURE);

        } else {
            /* Parent process */
            active_children++;
            /* Update scale for next frame */
            scale *= zoom;
        }
    }

    /* Wait for any remaining children */
    while (active_children > 0) {
        wait(NULL);
        active_children--;
    }

    return EXIT_SUCCESS;
}
