#include <stdio.h>
#include "userspace_workqueue.h"

void print(void *s)
{
	printf("%s\n", (char *) s);
}

void main(void)
{
	us_wqueue q1, q2;

	create_userspace_workqueue(&q1);

	INIT_WORK(&q1, print, "hello from thread 1 func 1");
	INIT_WORK(&q1, print, "hello from thread 1 func 2");

	printf("hello from main thread 1\n");

	userspace_queue_work(&q1);

	INIT_WORK(&q1, print, "hello from thread 1 func 3");

	create_userspace_workqueue(&q2);
	INIT_WORK(&q2, print, "hello from thread 2 func 1");
	INIT_WORK(&q2, print, "hello from thread 2 func 2");

	printf("hello from main thread 2\n");

	userspace_queue_work(&q2);
	userspace_queue_work(&q1);

	flush_userspace_workqueue(&q1);
	destroy_userspace_workqueue(&q1);
	flush_userspace_workqueue(&q2);
	destroy_userspace_workqueue(&q2);

	printf("hello from main thread 3\n");
}
