1. Your name(s): Allyson Hussey and Amanda Silva
2. Collaboration statement: 

We both watched these youtube videos and read these articles. When reading the articles we would use claud to explain some terminalogy and talk to eachother about parts we struggled on.

 https://www.youtube.com/watch?v=ldJ8WGZVXZk , 
 https://www.youtube.com/watch?v=uA8X5zNOGw8 , 
 https://hpc-tutorials.llnl.gov/posix/passing_args/ , 
 https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/ThreadArgs.html. 

3. About how long did this project take you? 

Over the span of 2-3 days we worked together and in total worked around 4.5 hours including the time of reading and watching videos. 

4. Do you think everything you did is correct?

Yes. It compiles with the given Makefile, handles missing/bad arguments, works for even and odd N, and Both of us verified both results, came out sorted while testing.

5. Timing results:

| N     | One thread (ms) | Two threads (ms) |
|-------|-----------------|------------------|
| 1000  | 0.9             | 0.6              |
| 5000  | 18.6            | 11.7             |
| 10000 | 77.4            | 37.9             |
| 20000 | 310.8           | 142.5            |

6. Comments on two threads vs no threading:

Two threads got faster as N grew, ending around 2x faster at N=20000 (310.8ms vs 142.5ms). Selection sort is O(n^2) so each half is only a quarter of the work, which in theory could give up to 4x on separate cores, but my Codespace only has a couple of cores that the threads share with everything else, so it landed closer to the 2x case, and at N=1000 the speedup was smaller since thread creation overhead is a bigger share of such a short job.

7.

The hardest part for us was passing arguments through a struct and a void pointer, but it clicked after tracing it a few times. 