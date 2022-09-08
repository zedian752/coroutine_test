﻿// ucontext_coroutine.cpp: 定义应用程序的入口点。
//

#include "coroutine.h"

struct args {
	int n;
};

static void
foo(struct schedule* S, void* ud) {
	struct args* arg = (struct args*)ud;
	int start = arg->n;
	int i;
	for (i = 0; i < 5; i++) {
		printf("coroutine %d : %d\n", coroutine_running(S), start + i);
		coroutine_yield(S);
	}
}

static void
test(struct schedule* S) {
	struct args arg1 = { 0 };
	struct args arg2 = { 100 };

	int co1 = coroutine_new(S, foo, &arg1);
	int co2 = coroutine_new(S, foo, &arg2);
	printf("main start\n");
	// 协程1和协程2都未结束，则恢复协程
	while (coroutine_status(S, co1) && coroutine_status(S, co2)) {
		coroutine_resume(S, co1);
		coroutine_resume(S, co2);
	}
	printf("main end\n");
}

int
main() {
	struct schedule* S = coroutine_open();
	test(S);
	coroutine_close(S);

	return 0;
}
