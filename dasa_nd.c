/* chronos/dasa_nd.c
 *
 * DASA_ND Single-Core Scheduler Module for ChronOS
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

struct rt_info* sched_dasa_nd(struct list_head *head, int flags)
{
	struct rt_info *task, *best = local_task(head->next);
	struct rt_info *new_schedule=NULL, *prev_schedule = NULL;
	list_for_each_entry(task, head, task_list[LOCAL_LIST]){
		if(check_task_failure(task, flags))
			return task;

		initialize_lists(task);
		livd(task, false, flags); //updating value density values for all tasks in queue
		if(task->local_ivd < best->local_ivd)
			best = task;
		}
	copy_list(task, LOCAL_LIST, SCHED_LIST1);
	quicksort(best, SCHED_LIST1, SORT_KEY_LVD, 0);
	//best points to task with highest value density
	new_schedule = task = best;
	while(1) {
		prev_schedule = new_schedule;
		if (insert_on_list(task, new_schedule, SCHED_LIST2, SORT_KEY_DEADLINE, 0))
			new_schedule = task;

		if (!list_is_feasible(new_schedule, SCHED_LIST2)){
			list_remove(task, SCHED_LIST2);
			new_schedule = prev_schedule;
			}
		//moving to next task
		task = task_list_entry(task->task_list[SCHED_LIST1].next, SCHED_LIST1);
		if(task == best) //list iteration completed
			break;
		}

	return (new_schedule == NULL) ? best : new_schedule;
}

struct rt_sched_local dasa_nd = {
	.base.name = "DASA_ND",
	.base.id = SCHED_RT_DASA_ND,
	.flags = 0,
	.schedule = sched_dasa_nd,
	.base.sort_key = SORT_KEY_DEADLINE,
	.base.list = LIST_HEAD_INIT(dasa_nd.base.list)
};
/*
struct rt_info* sched_dasa_nd(struct list_head *head, int flags)
{
	struct rt_info *task, *temp=NULL, *save_temp, *best = local_task(head->next);
	list_for_each_entry(task, head, task_list[LOCAL_LIST]){
		if(check_task_failure(task, flags))
			return task;

		initialize_lists(task);
		livd(task, false, flags);
		list_add_before(best, task, SCHED_LIST1);
		if(task->local_ivd < best->local_ivd)
			best = task;
		}
	quicksort(best, SCHED_LIST1, SORT_KEY_LVD, 0);
	task = best;
	while(1) {
		save_temp = temp;
		if(temp == NULL)
			temp = task;
		else if (insert_on_list(task, temp, SCHED_LIST2, SORT_KEY_DEADLINE, 0))
			temp = task;

		if (!list_is_feasible(temp, SCHED_LIST2)){
			list_remove(task, SCHED_LIST2);
			temp = save_temp;
			}

		task = task_list_entry(task->task_list[SCHED_LIST1].next, SCHED_LIST1);
		if(task == best)
			break;
		}
	if(temp)
		return temp;
	else
		return best;
}

struct rt_sched_local dasa_nd = {
	.base.name = "DASA_ND",
	.base.id = SCHED_RT_DASA_ND,
	.flags = 0,
	.schedule = sched_dasa_nd,
	.base.sort_key = SORT_KEY_DEADLINE,
	.base.list = LIST_HEAD_INIT(dasa_nd.base.list)
};
*/
static int __init dasa_nd_init(void)
{
	return add_local_scheduler(&dasa_nd);
}
module_init(dasa_nd_init);

static void __exit dasa_nd_exit(void)
{
	remove_local_scheduler(&dasa_nd);
}
module_exit(dasa_nd_exit);

MODULE_DESCRIPTION("DASA_ND Single-Core Scheduling Module for ChronOS");
MODULE_AUTHOR("Matthew Dellinger <matthew@mdelling.com>");
MODULE_LICENSE("GPL");

