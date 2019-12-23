# defines the needed .c files in alphabetic order
sourceFies = cluster.c index.c list.c main.c merge_sort.c record.c

# gather all .c files and sort (alphabetically)
files = $(sort $(wildcard *.c))

#checks if you have every file you need
ifneq ($(files),$(sourceFies))
$(error You're missing some (if not all) the necessary source files for this project, try getting them at 'https://github.com/EdPirro/USP-SCC0215' then try again)
endif

# simple compile all .c files with gcc
all exe exe.exe:
	gcc merge_sort.c cluster.c list.c index.c record.c main.c -o exe 

# defines run and clean if windows is being used
ifeq ($(OS), Windows_NT)

# run checks whether the case variable was set to a valid one, if it was
# runs with the entered test case, if it isn't it runs withou any test case
# and waits for the inputs to be given manually
run: exe.exe
	powershell \
	"if ( Test-Path Test/Cases/$(case).in ) \
		{ echo "Output:";	 Get-Content Test/Cases/$(case).in | ./exe } \
	else \
		{ echo "Running..."; ./exe }"

# simply remove the exe and every .bin file created
clean: exe.exe
	powershell "rm exe.exe; rm * -Include *.bin"

# defines run and clean for non-windows system (Tested on Ubuntu18.04LTS only) 
else 

# run checks wheater the case variable was set to a valid one, if it was
# runs with the entered test case, if it isn't it runs withou any test case
# and waits for the inputs to be given manually
run: exe
	if [ -f Test/Cases/$(case).in ]; then \
		echo "Output:"; \
		./exe < Test/Cases/$(case).in;\
	else \
		echo "Running..."; \
		./exe; \
	fi

# simply remove the exe and every .bin file created
clean: exe
	rm exe; rm *.bin
endif
