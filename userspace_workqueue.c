#include "userspace_workqueue.h"

static void *work_thread(void *data) {
	us_wqueue *queue;

	queue = data;
	wq_flist *p, *p1;
	do {
		
		sem_wait(&(queue->semaphore));
		pthread_mutex_lock(&queue->mut);
		//printf("Inside work thread: %lu\n", queue->thread_id);
		p = queue->list;
		//топаем по очереди и выполняем каждую функцию
		while (p) {
			(*(p->func))(p->param);
			p = p->next;
		}

		queue->list = NULL;
		pthread_mutex_unlock(&queue->mut);
		//sem_init(&(queue->semaphore), 0, 0);
		//printf("Outside word thread: %lu\n", queue->thread_id);
	} while (1);
}

int create_userspace_workqueue(us_wqueue *head) {
	head->list = NULL;
	sem_init(&(head->semaphore), 0, 0);
	pthread_mutex_init(&head->mut, NULL);

	return pthread_create(&(head->thread_id), NULL, &work_thread, head);
}

int INIT_WORK(us_wqueue *head, void (*func)(void *), void *param) {
	wq_flist *p1, *p2;

	pthread_mutex_lock(&head->mut);
	//printf("Inside init_work: %lu\n", head->thread_id);
	//выделяет структуру рабочей функции
	p1 = malloc(sizeof(wq_flist));
	if (!p1)
		return -1;

	p1->next = NULL;
	p1->func = func;
	p1->param = param;

	p2 = head->list;
	// если список функций пуст
	if (!p2)
		head->list = p1;
	// иначе идем до конца и добавляем туда
	else {
		while (p2->next)
			p2 = p2->next;

		p2->next = p1;
	}
	pthread_mutex_unlock(&head->mut);
	//printf("Outside init_work: %lu\n", head->thread_id);
	return 0;
}

void flush_userspace_workqueue(us_wqueue *head) {
	int sem_status;
	do {
		pthread_mutex_lock(&head->mut);
		//printf("Inside flush 1: %lu\n", head->thread_id);
		sem_getvalue(&head->semaphore, &sem_status);
		pthread_mutex_unlock(&head->mut);
		//printf("Outside flush 1: %lu\n", head->thread_id);
	} while (sem_status != 0);
	pthread_mutex_lock(&head->mut);
	//printf("Outside cycle: %lu\n", head->thread_id);
	pthread_mutex_unlock(&head->mut);
}

void destroy_userspace_workqueue(us_wqueue *head) {
	wq_flist *p1, *p2;

	pthread_cancel(head->thread_id);
	// ждем на крайний случай
	pthread_join(head->thread_id, NULL);
	p1 = head->list;
	//идем по списку и удаляем все элементы
	if (p1)
		while (p1->next) {
			p2 = p1->next;
			free(p1);
			p1 = p2;
		}
}
