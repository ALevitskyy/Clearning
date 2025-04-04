#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h> // For INT_MAX

#define MAX_HEAP_SIZE 101 // One extra space for the sentinel

typedef struct {
    int data[MAX_HEAP_SIZE];
    int size;
} Heap;

// Function prototypes
void upheap(Heap *heap, int index);
void downheap(Heap *heap, int index);
bool insert(Heap *heap, int value);
int remove_top(Heap *heap);
int replace(Heap *heap, int value);
void print_heap(Heap *heap);

// Initialize a new heap
void init_heap(Heap *heap) {
    heap->size = 0;
    heap->data[0] = INT_MAX; // Sentinel value
}

// Upheap operation to maintain heap property
void upheap(Heap *heap, int index) {
    int value = heap->data[index];
    while (heap->data[index / 2] < value) { // Compare with parent
        heap->data[index] = heap->data[index / 2];
        index /= 2;
    }
    heap->data[index] = value;
}

// Downheap operation to maintain heap property
void downheap(Heap *heap, int index) {
    int value = heap->data[index];
    while (2 * index <= heap->size) {
        int child = 2 * index;
        if (child < heap->size && heap->data[child] < heap->data[child + 1]) {
            child++; // Select the larger child
        }
        if (value >= heap->data[child]) {
            break;
        }
        heap->data[index] = heap->data[child];
        index = child;
    }
    heap->data[index] = value;
}

// Insert a value into the heap
bool insert(Heap *heap, int value) {
    if (heap->size >= MAX_HEAP_SIZE - 1) {
        return false; // Heap is full
    }
    heap->size++;
    heap->data[heap->size] = value;
    upheap(heap, heap->size);
    return true;
}

// Remove the top (maximum) value from the heap
int remove_top(Heap *heap) {
    if (heap->size == 0) {
        return -1; // Heap is empty
    }
    int top = heap->data[1];
    heap->data[1] = heap->data[heap->size];
    heap->size--;
    downheap(heap, 1);
    return top;
}

// Replace the top value with a new value and maintain heap property
int replace(Heap *heap, int value) {
    if (heap->size == 0) {
        return -1; // Heap is empty
    }
    int top = heap->data[1];
    heap->data[1] = value;
    downheap(heap, 1);
    return top;
}

// Print the heap for debugging
void print_heap(Heap *heap) {
    for (int i = 1; i <= heap->size; i++) { // Start from 1 because of the sentinel
        printf("%d ", heap->data[i]);
    }
    printf("\n");
}

// Main function for testing
int main() {
    Heap heap;
    init_heap(&heap);

    insert(&heap, 10);
    insert(&heap, 20);
    insert(&heap, 5);
    insert(&heap, 30);
    insert(&heap, 15);

    printf("Heap after inserts: ");
    print_heap(&heap);

    printf("Removed top: %d\n", remove_top(&heap));
    printf("Heap after removal: ");
    print_heap(&heap);

    printf("Replaced top with 25: %d\n", replace(&heap, 25));
    printf("Heap after replacement: ");
    print_heap(&heap);

    return 0;
}
