# System Programming Lab 11 Multiprocessing
![alt text](image.png)

## It seems that the time exponentially decreases with the number of processes used, capping out around 12 seconds with a minimum of 20 processes


# System Programming Lab 12 Multithreading
![alt text](image-1.png)

## It seems that there was definately an increase in performance with the implementation on multithreading, but personally I believe that the largest performance increase was the initial utilization of multiprocessing. Odly enough the time to generate each frame seemed to decrease when the number of threads increased. There may be an error within the program to blame (or the face I have about 20 tabs open constantly), but intuitively you ould think the time would drasticlly decrease with 20 acrive threads compared to 1, but the time varied greatly depending on the combination. The fastest time seemed to belong to the combination of 20 processes and 5 threads. Looking at the graph, there seems to be no correlation (visually) in the number of threads used and the total time to generate all 50 frames. Hopefully there is some systematic error that I was unable to account for during the process of this experiment, because ti would be very unfortunate if multithreading was unpredictable in actual applications
## 