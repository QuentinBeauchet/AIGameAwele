struct List_i {
    int *tab;
    int length;
};
typedef struct List_i List_i;

List_i *newList_i(int size) {
    List_i *list = malloc(sizeof(List_i));
    list->tab = malloc(sizeof(int) * size);
    list->length = 0;
    return list;
}

void freeList_i(List_i *list) {
    free(list->tab);
    free(list);
}