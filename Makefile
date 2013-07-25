
all: latex gitadd documentation

latex_mine: tex/method_description.tex
	cd tex ; pdflatex method_description.tex; cd ..;

latex: TAROS13boids/
	cd TAROS13boids ; make ; cd .. ;

documentation:
	doxygen Doxyfile

gitadd:
	git add tex/*
	git add code/*.py
	git add videos/*
	git add papers/*
	git add stats/*
	git add maps/*
	git add impede/*
	git add simdata/*
	git add TAROS13boids/*
	git add Makefile
	git add docs/*
