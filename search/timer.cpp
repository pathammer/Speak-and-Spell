#include <stdio.h>
#include <time.h>

class timer {
  clock_t init, final;
public:
  timer() {
	init=clock();
  }
  double getClock() {
	return ((double)clock()-init) / ((double)CLOCKS_PER_SEC);
  }
  void wait (int seconds) {
	clock_t endwait;
	endwait = clock () + seconds * CLOCKS_PER_SEC ;
	while (clock() < endwait) {}
  }
};
