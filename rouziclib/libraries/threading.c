#ifdef RL_TINYCTHREAD

int thrd_create_detached(thrd_start_t func, void *arg)
{
	int ret;
	thrd_t thr;

	ret = thrd_create(&thr, func, arg);
	if (ret != thrd_success)
	{
		if (ret == thrd_nomem)
			fprintf_rl(stderr, "No memory could be allocated for the thread requested (in thrd_create_detached(), for thrd_create())");
		if (ret == thrd_error)
			fprintf_rl(stderr, "The request could not be honoured (in thrd_create_detached(), for thrd_create())");

		return ret;
	}

	ret = thrd_detach(thr);
	if (ret != thrd_success)
		fprintf_rl(stderr, "The thread could not be detached (in thrd_create_detached(), for thrd_detach())");

	return ret;
}

mtx_t *mtx_init_alloc(int type)
{
	mtx_t *mtx = calloc(1, sizeof(mtx_t));
	mtx_init(mtx, type);

	return mtx;
}

void mtx_destroy_free(mtx_t **mtx)
{
	if (mtx)
	{
		mtx_destroy(*mtx);
		free(*mtx);
		memset(mtx, 0, sizeof(mtx_t));
	}
}

int thrd_join_and_null(thrd_t *thr, int *res)
{
	int ret;

	ret = thrd_join(*thr, res);

	memset(thr, 0, sizeof(thrd_t));

	return ret;
}

#endif
