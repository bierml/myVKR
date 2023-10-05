#ifndef LIST_HELPER_H
#define LIST_HELPER_H
// Макрос для добавления элемента (add) в двусвязный список (head)
#define LIST_ADD(head, add) \
do { \
	if (head) { \
	(add)->prev = (head)->prev; \
	(head)->prev->next = (add); \
	(head)->prev = (add); \
	(add)->next = NULL; \
	} else { \
	(head)=(add); \
	(head)->prev = (head); \
	(head)->next = NULL; \
} \
} while (0) \
// Макрос для удаления элемента (del) из двусвязного списка (head)
#define LIST_DEL(head, del) \
do { \
	if ((del)->prev == (del)) { \
	(head)=NULL; \
	} else if ((del)==(head)) { \
	(del)->next->prev = (del)->prev; \
	(head) = (del)->next; \
	} else { \
	(del)->prev->next = (del)->next; \
	if ((del)->next) { \
	(del)->next->prev = (del)->prev; \
	} else { \
	(head)->prev = (del)->prev; \
} \
} \
} while (0)
#endif /* LIST_HELPER_H */
