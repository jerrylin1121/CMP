CMP:clean
	g++ -o CMP *.cpp
clean:
	rm -f CMP
clean_test:
	rm -f *.rpt makefile result
my:
	~/Archi2017_Project1/single_cycle/simulator/single_cycle
goldon:
	~/Archi2017_Project1/archiTA/simulator/single_cycle
diff_snap:
	diff ./snapshot.rpt ./_snapshot.rpt
diff_error:
	diff ./error_dump.rpt ./_error_dump.rpt
