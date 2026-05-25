#include <stdio.h>
#include <stdint.h>

#define POOL_ALIGNMENT 16
#define MAX_ALLOC_FROM_POOL 4095

#define align_ptr(p, a)                                                   \
    (char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

#define ALIGNMENT   sizeof(unsigned long) 

typedef struct  pool_s          pool_t;
typedef struct  pool_chain_s    pool_chain_t;
typedef struct  pool_large_s    pool_large_t;
typedef struct  pool_cleanup_s  pool_cleanup_t;
typedef void (*pool_cleanup_pt)(void* data);

typedef struct {
    char* last; // 未使用内存的开始节点地址
    char* end;  // 内存池的结束地址
    pool_t* next;
    int failed;
} pool_data_t;

struct pool_s {
    pool_data_t d;          // 数据区域
    size_t max;             // 可分配的最大内存
    pool_t *current;        // 指向当前内存池指针
    pool_chain_t *chain;    // 缓冲区链表
    pool_large_t *large;    // 存储大数据的链表
    pool_cleanup_t *cleanup;// 清除内存
};

struct pool_large_s {
    pool_large_t* next;
    void* alloc;
};

struct pool_cleanup_s {
    void* data;
    pool_cleanup_t* next;
    pool_cleanup_pt handler;
};

void* alloc(size_t size);
void* calloc(size_t size);
void* create_pool(size_t size);
void* memalign(size_t alignment, size_t size);
void destroy_pool(pool_t* pool);
void reset_pool(pool_t* pool);
void* palloc(pool_t* pool, size_t size);
void* palloc_large(pool_t* pool, size_t size);
void* pnalloc(pool_t* pool, size_t size);