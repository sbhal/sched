/* chronos/lbesa.c
 *
 * LBESA Single-Core Scheduler Module for ChronOS
 *
 * Author(s)
 *	- Matthew Dellinger, mdelling@vt.edu
 *
 * Copyright (C) 2009-2012 Virginia Tech Real Time Systems Lab
 */

#include <linux/module.h>
#include <linux/chronos_types.h>
#include <linux/chronos_sched.h>
#include <linux/list.h>

struct rt_info* sched_lbesa(struct list_head *head, int flags)
{
	struct rt_info *lvd_task, *hvd_task = NULL;
	struct rt_info* task = local_task(head->next);
	lvd_task = hvd_task = task;
	
	list_for_each_entry(task, head, task_list[LOCAL_LIST]) {
		if(check_task_failure(task, flags))
			return task;

		livd(task, false, flags);
		initialize_lists(task);

		if(task->local_ivd < hvd_task->local_ivd)
			hvd_task = task;
	}

	copy_list(task, LOCAL_LIST, SCHED_LIST1);
	copy_list(task, LOCAL_LIST, SCHED_LIST2);
	quicksort(task, SCHED_LIST2, SORT_KEY_LVD, 0);
	lvd_task = task_list_entry(hvd_task->task_list[SCHED_LIST2].prev, SCHED_LIST2);
	//printk("%ld and %ld \n", lvd_task->local_ivd, hvd_task->local_ivd);
	// Remove the lowest Value density task if schedule is in feasible
	do {
		if(list_is_feasible(task, SCHED_LIST1))
			break;

		if(lvd_task == task)
			task = task_list_entry(task->task_list[SCHED_LIST1].next, SCHED_LIST1);

		list_remove(lvd_task, SCHED_LIST1);
		lvd_task = task_list_entry(lvd_task->task_list[SCHED_LIST2].prev, SCHED_LIST2);
	} while (lvd_task != hvd_task);

	return task;
}

struct rt_sched_local lbesa = {
	.base.name = "LBESA",
	.base.id = SCHED_RT_LBESA,
	.flags = 0,
	.schedule = sched_lbesa,
	.base.sort_key = SORT_KEY_DEADLINE,
	.base.list = LIST_HEAD_INIT(lbesa.base.list)
};
/*
struct rt_info* sched_lbesa(struct list_head *head, int flags)
{
	struct rt_info *looper, *ivd_list;
	struct rt_info *last_ivd, *edf_list;

	ivd_list = local_task(head->next);
	edf_list = ivd_list;
	initialize_lists(ivd_list);

	list_for_each_entry(looper, head, task_list[LOCAL_LIST]) {
		if(check_task_failure(looper, flags))
			return looper;

		livd(looper, false, flags);
		list_add_before(ivd_list, looper, SCHED_LIST1);
		list_add_before(edf_list, looper, SCHED_LIST2); //List 2 i.e is for EDF and is by default EDF sorted

		if(looper->local_ivd < ivd_list->local_ivd)
			ivd_list = looper;
	}

	quicksort(ivd_list, SCHED_LIST1, SORT_KEY_LVD, 0);
	last_ivd = task_list_entry(ivd_list->task_list[SCHED_LIST1].prev, SCHED_LIST1);

	// Remove the lowest Value density task if schedule is in feasible 
	do {
		if(list_is_feasible(edf_list, SCHED_LIST2))
			break;

		if(last_ivd == edf_list)
			edf_list = task_list_entry(edf_list->task_list[SCHED_LIST2].next, SCHED_LIST2);

		list_remove(last_ivd, SCHED_LIST2);
		last_ivd = task_list_entry(last_ivd->task_list[SCHED_LIST1].prev, SCHED_LIST1);
	} while (last_ivd != ivd_list);

	return edf_list;
}

struct rt_sched_local lbesa = {
	.base.name = "LBESA",
	.base.id = SCHED_RT_LBESA,
	.flags = 0,
	.schedule = sched_lbesa,
	.base.sort_key = SORT_KEY_DEADLINE,
	.base.list = LIST_HEAD_INIT(lbesa.base.list)
};

struct rt_info* sched_lbesa(struct list_head *head, int flags)
{
	struct rt_info *best = local_task(head->next);

	if(flags & SCHED_FLAG_PI)
		best = get_pi_task(best, head, flags);

	return best;
}


struct rt_sched_local lbesa = {
	.base.name = "LBESA",
	.base.id = SCHED_RT_LBESA,
	.flags = 0,
	.schedule = sched_lbesa,
	.base.sort_key = SORT_KEY_PERIOD,
	.base.list = LIST_HEAD_INIT(lbesa.base.list)
};
*/

static int __init lbesa_init(void)
{
	return add_local_scheduler(&lbesa);
}
module_init(lbesa_init);

static void __exit lbesa_exit(void)
{
	remove_local_scheduler(&lbesa);
}
module_exit(lbesa_exit);

MODULE_DESCRIPTION("LBESA Single-Core Scheduling Module for ChronOS");
MODULE_AUTHOR("Matthew Dellinger <matthew@mdelling.com>");
MODULE_LICENSE("GPL");

