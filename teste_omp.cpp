#include <omp.h>
#include <stdio.h>

int main() {
	#pragma omp parallel
	{
		int id = omp_get_thread_num();
		int total = omp_get_num_threads();
		printf("Ola da thread %d de %d\n", id, total);
	}
	return 0;
}
