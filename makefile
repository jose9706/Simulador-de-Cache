make:
	g++ main.cpp -o branch
bht:
	gunzip -c branch-trace-gcc.trace.gz | head -200 | ./branch -s 3 -bp 0 -gh 4 -ph 3 -o 1
gshare:
	gunzip -c branch-trace-gcc.trace.gz | head -200 | ./branch -s 4 -bp 2 -gh 3 -ph 3 -o 1
pshare:
	gunzip -c branch-trace-gcc.trace.gz | head -200 | ./branch -s 5 -bp 1 -gh 4 -ph 2 -o 1

