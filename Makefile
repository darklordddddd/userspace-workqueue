all: userspace_workqueue
	gcc -o us_wq userspace_workqueue.o test.c -pthread

userspace_workqueue:
	gcc -pthread -c userspace_workqueue.c

clean:
	rm *.o
	rm *.out
