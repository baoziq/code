#include "memory_pool.h"

static void* palloc_block(pool_t* pool, size_t size);
static void* palloc_large(pool_t* pool, size_t size);

void* alloc(size_t size) {
    void* p = malloc(size);
    if (!p) {
        fprintf(stderr, "alloc failed\n");
        return NULL;
    }
    return p;
}

void* calloc(size_t size) {
    void* p = alloc(size);
    if (p) {
        memset(p, 0, size);
    }
    return p;
}

void* memalign(size_t alignment, size_t size) {
    void* p;
    int err;
    err = posix_memalign(&p, alignment, size);
    if (err) {
        fprintf(stderr, "memalign failed\n");
        p = NULL;
    }
    return p;
}

void* create_pool(size_t size) {
    pool_t* p = memalign(POOL_ALIGNMENT, size);
    if (!p) {
        return NULL;
    }
    p->d.last = (char*)p + sizeof(pool_t);
    p->d.end = (char*)p + size;
    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(pool_t);

    p->max = (size < MAX_ALLOC_FROM_POOL) ? size : MAX_ALLOC_FROM_POOL;
    p->current = p;
    p->chain = NULL;
    p->large = NULL;
    p->cleanup = NULL;

    return p;

}

void destroy_pool(pool_t* pool) {
    pool_t *p, *n;
    pool_large_t *l;
    pool_cleanup_t *c;

    for (c = pool->cleanup; c; c = c->next) {
        if (c->handler) {
            c->handler(c->data);
        }
    }

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            free(l->alloc);
        }
    }

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        free(p);
        if (n == NULL) {
            break;
        }
    }
}

void reset_pool(pool_t* pool) {
    pool_t* p;
    pool_large_t* l;
    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            free(l->alloc);
        }
    }

    pool->large = NULL;

    for (p = pool; p; p = p->d.next) {
        p->d.last = (char*)p + sizeof(pool_t);
        p->d.failed = 0;
    }
}

void* palloc(pool_t* pool, size_t size) {
    char* m;
    pool_t* p;
    if (size <= pool->max) {
        p = pool->current;
        do {
            m = p->d.last;
            if ((size_t)(p->d.end - m) >= size) {
                p->d.last += size;
                return m;
            }
            p = p->d.next;
        } while (p);
        return palloc_block(pool, size);
    }
    return palloc_large(pool, size);
}

void* pnalloc(pool_t* pool, size_t size) {
    char* m;
    pool_t* p;
    
    if (size <= pool->max) {
        p = pool->current;
        do {
            m = p->d.last;
            if ((size_t)(p->d.end - m) >= size) {
                p->d.last += size;
                return m;
            }
            p = p->d.next;
        } while (p);
        return palloc_block(pool, size);
    }
    return palloc_large(pool, size);
}

static void* palloc_block(pool_t* pool, size_t size) {
    char* m;
    int psize;
    pool_t *p, *new, *current;
    psize = (size_t)(pool->d.end - (char*)pool);
    m = memalign(POOL_ALIGNMENT, psize);
    if (!m) {
        return NULL;
    }
    new = (pool_t*)m;
    new->d.end = m + psize;
    new->d.next = NULL;
    new->d.failed = 0;
    new->d.last = m + sizeof(pool_t);
    m += sizeof(pool_data_t);
    m = align_ptr(m, ALIGNMENT);
    new->d.last = m + size;
    current = pool->current;

    for (p = current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            current = p->d.next;
        }
    }
    p->d.next = new;
    pool->current = current ? current : new;
    return m;
}

static void* palloc_large(pool_t* pool, size_t size) {
    void* p;
    int n;
    pool_large_t* large;
    p = alloc(size);
    if (!p) {
        return NULL;
    }
    n = 0;

    
}