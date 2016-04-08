/* chronos/dasa.c
 *
 * DASA Single-Core Scheduler Module for ChronOS
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
/*
bool insert_deps_on_list(struct rt_info *task, struct rt_info *sched){
	bool head_changed = false;
	if(task->dep != NULL){
	if(task_check_flag(task, DEADLOCKED))
		abort_deadlock(task);

	if(task_check_flag(task, ABORTED)) {
        	task->local_ivd = -1;
                return -1;
       	}

	curr = task;
	next = curr->dep;
	
	while (next != NULL) {
		if (insert_on_list(task, sched, SCHED_LIST2, SORT_KEY_DEADLINE, 0)){
			sched = task;
			head_changed = true;
		}
		curr = next;
		next = curr->dep;
	}
	return head_changed;
}*/
//struct rt_info *item, struct rt_info *list, int i, int key, int before)
bool search(struct rt_info *item, struct rt_info *list, int listNo){
	struct rt_info *it = list;
	if(list == item)
		return false;
	do {
		if(it == item) {
			return true;
		}
		it = task_list_entry(it->task_list[listNo].next, listNo);
	} while (it != list);
	return false;
}
struct rt_info * phase_insert(struct rt_info *task, struct rt_info *sched_task, int sched, int tentSched){
	struct timespec DL, currentDead;
	struct rt_info *curr = NULL;
	struct rt_info *tentSched_task, *it;

	if(sched_task == NULL){
		sched_task = task;
	}
	tentSched_task = sched_task;

	if(search(task, sched_task, sched) == false){
		//tentSched = sched;
		copy_list(task, sched, tentSched);
	printk("\n Sched is\n");
it = sched_task;
	do {
		printk("%p(%lu) -- ", it, it->local_ivd);			
		it = task_list_entry(it->task_list[sched].next, sched);
	} while(it != sched_task);
	printk("\n tentSched is\n");
it = tentSched_task;
	do {
		printk("%p(%lu) -- ", it, it->local_ivd);			
		it = task_list_entry(it->task_list[sched].next, sched);
	} while(it != tentSched_task);

		if(task != tentSched_task){
		printk("\ninserting task in list");
			insert_on_list(task, tentSched_task, tentSched, SORT_KEY_DEADLINE, 1);
		}
		currentDead = task->temp_deadline;
	
		curr = task->dep;
		while(curr){
			if(search(curr, sched_task, tentSched) == true){
				DL = curr->temp_deadline;//lookup(curr, tentSched);
				if(earlier_deadline(&DL, &currentDead))
					continue;
				else
					list_remove(curr, tentSched);//Remove(curr, tentSched, DL);
			}
			if(earlier_deadline(&curr->deadline, &currentDead))
				currentDead = curr->deadline;
			if(insert_on_list(curr, tentSched_task, tentSched, SORT_KEY_DEADLINE, 1))
				tentSched_task = curr;
			curr = curr->dep;
		}
	}

	if (list_is_feasible(sched_task, sched)){
		printk("\nList is feasible\n");
		copy_list(task, tentSched, sched);
		return tentSched_task;
	} else {
		printk("\nList is not feasible\n");	
	}
	return sched_task;
}
struct rt_info* sched_dasa(struct list_head *head, int flags)
{
	struct rt_info *task, *best = local_task(head->next);
	struct rt_info *new_schedule=NULL;
printk("\n Entered\n");
	list_for_each_entry(task, head, task_list[LOCAL_LIST]){
		if(check_task_failure(task, flags))
			return task;

		initialize_lists(task);
		initialize_dep(task);
		livd(task, false, flags); //updating value density values for all tasks in queue
		list_add_before(best, task, SCHED_LIST1);
		if(task->local_ivd < best->local_ivd)
			best = task;
	}
	//copy_list(task, LOCAL_LIST, SCHED_LIST1);
	quicksort(best, SCHED_LIST1, SORT_KEY_LVD, 0);
	task = best;
	do {
		printk("%p(%lu) \n", task, task->local_ivd);			
		task = task_list_entry(task->task_list[SCHED_LIST1].next, SCHED_LIST1);
	} while(task != best);
	task = best;
	//best points to task with highest value density
	//new_schedule = task = best;
	do {
	//	prev_schedule = new_schedule;
		//copy_list(task, SCHED_LIST2, SCHED_LIST3);
		new_schedule = phase_insert(task, new_schedule, SCHED_LIST2, SCHED_LIST3);
		task = task_list_entry(task->task_list[SCHED_LIST1].next, SCHED_LIST1);
	} while(task != best);
	if(new_schedule)
		printk("new sched is %p(%lu) \n", new_schedule, new_schedule->local_ivd);			
	return (new_schedule == NULL) ? best : new_schedule;
}

struct rt_sched_local dasa = {
	.base.name = "DASA",
	.base.id = SCHED_RT_DASA,
	.flags = 0,
	.schedule = sched_dasa,
	.base.sort_key = SORT_KEY_DEADLINE,
	.base.list = LIST_HEAD_INIT(dasa.base.list)
};

static int __init dasa_init(void)
{
	return add_local_scheduler(&dasa);
}
module_init(dasa_init);

static void __exit dasa_exit(void)
{
	remove_local_scheduler(&dasa);
}
module_exit(dasa_exit);

MODULE_DESCRIPTION("DASA Single-Core Scheduling Module for ChronOS");
MODULE_AUTHOR("Matthew Dellinger <matthew@mdelling.com>");
MODULE_LICENSE("GPL");

