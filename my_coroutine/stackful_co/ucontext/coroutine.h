#ifndef _COROUTINE_H
#define _COROUTINE_H
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <ucontext.h>


#define DEFAULT_COROUTINE 16
#define STACK_SIZE (1024 * 1024) // 1Mb
// 协程状态
#define COROUTINE_DEAD 0
#define COROUTINE_READY 1
#define COROUTINE_RUNNING 2
#define COROUTINE_SUSPEND 3

typedef void (*coroutine_func)(struct schedule*, void* ud);
/* 协程 */
struct coroutine;
/* 调度器 */
struct schedule {
	char stack[STACK_SIZE];
	ucontext_t main; // 主协程
	int nco;
	int cap;
	int running;
	struct coroutine** co;

};
struct coroutine {
	coroutine_func func;
	void* ud;
	ucontext_t ctx;
	struct schedule* sch;
	ptrdiff_t cap;
	ptrdiff_t size;
	int status;
	char* stack;
};






struct schedule* coroutine_open(void);
void coroutine_close(struct schedule*);

int coroutine_new(struct schedule*, coroutine_func, void* ud);
void coroutine_resume(struct schedule*, int id);
int coroutine_status(struct schedule*, int id);
int coroutine_running(struct schedule*);
void coroutine_yield(struct schedule*);

#endif // !_COROUTINE_H
