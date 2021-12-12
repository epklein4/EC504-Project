# EC504-Project
## Instructions to install and run program:
- First clone the repository by pasting the following in to your console:

`git clone git@github.com:epklein4/EC504-Project.git`
- Next run the following commands

`cd EC504-Project/`
`make`
- Finally, to run the program enter the following and follow the instructions given by the console

`./Segment` 

## Instructions on running performance test:
- Follow step 1 above
- Run the following command

`cd EC504-Project/`
- Go to main.cpp and change the code on line 20 from `“#define PERFORMANCE_TEST 1”` to `#define PERFORMANCE_TEST 0”`
- Run the following commands

`make`
`./Segment`
`Gnuplot graphScript.p`
- After everything has run, you should be able to find a performance.txt and performance.png of your results.
