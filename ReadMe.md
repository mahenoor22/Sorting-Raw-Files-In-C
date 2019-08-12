## 2 Source - 1 Destination (2S1D)

The most basic where there are 2 source files are used. 

`Mark and Distribute Run` - We mark runs in one file(*f0*) and distribute them to two different temp files. 

`Merge Runs` - These two files(*f1 and f2*) are open and we merge a sorted run from both files in destination file(*f0*) creating a sorted run from two sorted runs of source files. 

We keep doing this until all the runs from both the files are merged and written on the third file. 

## 2 Source 2 Destination (2S2D)

Here, we use two alternating destination files which makes the distribution of runs after every iteration redundant as we are distributing them to two different destination in the merging process itself. 

## Polyphase Sort
A polyphase merge sort is a variation that sorts a list using an initial uneven distribution of sub-lists (runs), primarily used for external sorting, and is more efficient than 2S-1D and 2S-2D. 
