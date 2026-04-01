// Used for struct definitions
#define LinkedList(type) type *type##_head; type *type##_tail; int type##_len;

#define AppendToLinkedListPrefix(prefix, type) \
if (prefix##_head == NULL){ \
    prefix##_head = malloc(sizeof(type)); \
    prefix##_tail = prefix##_head; \
    prefix##_head->next = NULL; \
    return prefix##_tail; \
}else if (prefix##_tail != NULL){ \
    prefix##_tail->next = malloc(sizeof(type)); \
    prefix##_tail = prefix##_tail->next; \
    return prefix##_tail; \
} \
    prefix##_len++; \
    assert(0 && "Unreachable code: the tail should only be a null pointer if the head is also a null pointer -- error in linked list"); \
    return NULL;

#define AppendToLinkedList(parent, type) AppendToLinkedListPrefix(parent.type, type)

#define InitLinkedList(parent, type) parent.type##_head = NULL; parent.type##_tail = NULL; parent.type##_len = 0;
