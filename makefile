CMP:clean
	g++ -std=c++14 -g -o CMP *.cpp
	./CMP > result
clean:
	rm -f CMP
clean_test:
	rm -f *.rpt makefile result
my:
	~/Archi2017_Project3/CMP/simulator/CMP
goldon:
	~/Archi2017_Project3/archiTA/simulator/CMP
diff:diff_snap diff_report
diff_snap:
	diff ./snapshot.rpt ./_snapshot.rpt
diff_report:
	diff ./report.rpt ./_report.rpt
