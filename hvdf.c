/* chronos/hvdf.c
 *
 * HVDF Single-Core Scheduler Module for ChronOS
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

struct rt_info* sched_hvdf(struct list_head *head, int flags)
{
	struct rt_info *task, *best = local_task(head->next);
	struct timespec t1;
	t1=current_kernel_time();
	list_for_each_entry(task,head,task_list[LOCAL_LIST]){

		if(check_task_aborted(task))
			return task;
		else if(earlier_deadline(&(task->deadline),&t1)==1){
			return task;
		}
		else{
			livd(task,false,flags);
			if(task->local_ivd < best->local_ivd)
				best=task;		
		}
	}

	return best;
}

struct rt_sched_local hvdf = {
	.base.name = "HVDF",
	.base.id = SCHED_RT_HVDF,
	.flags = 0,
	.schedule = sched_hvdf,
	.base.sort_key = SORT_KEY_PERIOD,
	.base.list = LIST_HEAD_INIT(hvdf.base.list)
};

static int __init hvdf_init(void)
{
	return add_local_scheduler(&hvdf);
}
module_init(hvdf_init);

static void __exit hvdf_exit(void)
{
	remove_local_scheduler(&hvdf);
}
module_exit(hvdf_exit);

MODULE_DESCRIPTION("HVDF Single-Core Scheduling Module for ChronOS");
MODULE_AUTHOR("Matthew Dellinger <matthew@mdelling.com>");
MODULE_LICENSE("GPL");

