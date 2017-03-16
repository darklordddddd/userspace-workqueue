#ifndef _US_WQ_H
#define _US_WQ_H

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

// структура: список функций
typedef struct func_list {
	struct func_list *next;
	void (*func)(void *);
	void *param;
} wq_flist;

// структура для рабочей очереди
typedef struct wqueue {
	wq_flist  *list;
	pthread_t thread_id;
	pthread_mutex_t mut;
	sem_t semaphore;
} us_wqueue;

#define userspace_queue_work(head) sem_post(&((head)->semaphore))

int create_userspace_workqueue(us_wqueue *head);
int INIT_WORK(us_wqueue *head, void (*func)(void *), void *param);
void flush_userspace_workqueue(us_wqueue *head);
void destroy_userspace_workqueue(us_wqueue *head);

#endif
