#include "sbrk.h"
#include "malloc.h"

void free(void *block)
{
	struct header_t *header, *tmp;
	void *programbreak;

	if (!block)
		return;
	pthread_mutex_lock(&global_malloc_lock);
	header = (struct header_t*)block - 1;

	programbreak = sbrk(0);
	if ((char*)block + header->size == programbreak) {
		if (head == tail) {
			head = tail = NULL;
		} else {
			tmp = head;
			while (tmp) {
				if(tmp->next == tail) {
					tmp->next = NULL;
					tail = tmp;
				}
				tmp = tmp->next;
			}
		}
		sbrk(0 - sizeof(struct header_t) - header->size);
//		pthread_mutex_unlock(&global_malloc_lock);
		return;
	}
	header->is_free = 1;
//	pthread_mutex_unlock(&global_malloc_lock);
}
