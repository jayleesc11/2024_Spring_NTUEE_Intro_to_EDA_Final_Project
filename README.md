# Intro to EDA Final Project - 2024 ICCAD Contest Problem D: Chip Level Global Router
This program supports a solution to the ICCAD 2024 CAD Contest Problem D: Chip Level Global Router. 
Default setting corresponds to the stage 5 of Experiment Result in our report.

Author
- B11901029 Jhih-Jie Lee
- B10901192 Chun Che Lin
- B09901137 Tsz Wun Fok

## How to compile
    make

## How to run

    ulimit -s unlimited
    ./CGR XX(tracks/um) inputs/caseOO/caseOO_def inputs/caseOO/caseOO_cfg.json inputs/caseOO/caseOO.json

For example

    ulimit -s unlimited
	./CGR 20 inputs/case4/case4_def inputs/case4/case4_cfg.json inputs/case4/case4.json

The program occasionally reports a segmentation fault on WSL after activating -O3 optimization. A solution has not yet been identified. If this issue occurs, please try running the program again. It generally runs successfully within three attempts.