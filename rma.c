	/* chronos/rma.c
	*
	* RMA Single-Core Scheduler Module for ChronOS
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

	bool search(struct rt_info *item, struct rt_info *list, int listNo){
		struct rt_info *it = list;
		printk(" Searching %p in: \n", item);
		do {
			printk("%p  ", it);
			it = task_list_entry(it->task_list[listNo].next, listNo);
		} while (it != list);
		list_for_each_entry(it, list->task_list+listNo, task_list[listNo]){
			if(it == item) {
			printk("\n returning true");
				return true;
			}
			printk("%p  with  %lu -", it, it->local_ivd);			
		}
	printk("\n returning false");
	return false;
	/*	it = list;
		do {
			if(it == item) {
			printk("\n returning true");
				return true;
			}
			it = task_list_entry(it->task_list[listNo].next, listNo);
		} while (it != list);
			printk("\n returning false");
		return false;*/
	}
	struct rt_info * phase_insert(struct rt_info *task, struct rt_info *sched_task, int sched, int tentSched){
		struct timespec DL, currentDead;
		struct rt_info *curr = NULL;
		struct rt_info *tentSched_task = sched_task;
		if(sched_task == NULL){
			sched_task = task;
		}
		tentSched_task = sched_task;
	printk(" -- 3 --\n");
		if(search(task, sched_task, sched) == false){
			//tentSched = sched;
			copy_list(task, sched, tentSched);
			//inserting first task
			if(insert_on_list(task, tentSched_task, tentSched, SORT_KEY_DEADLINE, 1))
				tentSched_task = task;
			currentDead = task->temp_deadline;
		
	printk(" -- 4 --\n");
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
			copy_list(task, tentSched, sched);
			return tentSched_task;
		}
		return sched_task;
	}
	struct rt_info* sched_rma(struct list_head *head, int flags)
	{

		struct rt_info *task, *best = local_task(head->next);
		struct rt_info *new_schedule=NULL;
		int cnt = 1;
	/*printk("\n haha %p \n", local_task(head));
		list_for_each_entry(task, best->task_list+LOCAL_LIST, task_list[LOCAL_LIST]){
			printk("%p  with  %lu -", task, task->local_ivd);			
		}
	*/printk("\n >>>>>>>>>>>>> siddharth1 -- %p and next is %p\n", head, head->next);
	printk("\n head prev is %p and head nxt prev is %p\n", head->prev, head->next->prev);
	printk("Local List\n");
		list_for_each_entry(task, head, task_list[LOCAL_LIST]){
			if(check_task_failure(task, flags))
				return task;
			initialize_lists(task);
			initialize_dep(task);
			livd(task, false, flags); //updating value density values for all tasks in queue
			//list_add_before(best, task, SCHED_LIST1);
			if(task->local_ivd < best->local_ivd)
				best = task;
			printk("%p  with %lu -", task, task->local_ivd);
		}
		printk("\n best VD task is %p \n", best);
		copy_list(best, LOCAL_LIST, SCHED_LIST1);
		quicksort(best, SCHED_LIST1, SORT_KEY_LVD, 0);
		printk("\n Sched list 1 (vd sorted)\n");
		list_for_each_entry(task, head+SCHED_LIST1, task_list[SCHED_LIST1]){
			printk("%p  with  %lu -", task, task->local_ivd);			
		}
		task = best;
		do {
			printk(" task  is %p --", task);
			task = task_list_entry(task->task_list[SCHED_LIST1].next, SCHED_LIST1);
		} while(task != best);
		
		printk("\n do while finished \n");
		list_for_each_entry(task, head+SCHED_LIST1, task_list[SCHED_LIST1]){
			printk("%p  with  %lu -", task, task->local_ivd);			
			new_schedule = phase_insert(task, new_schedule, SCHED_LIST2, SCHED_LIST3);
		}
		return (new_schedule == NULL) ? best : new_schedule;
		//best points to task with highest value density
		new_schedule = task = best;

		do {
			//prev_schedule = new_schedule;
			//copy_list(task, SCHED_LIST2, SCHED_LIST3);
			new_schedule = phase_insert(task, new_schedule, SCHED_LIST2, SCHED_LIST3);

		//if(cnt-- == 0)return best;
		printk(" task before calling next is %p--\n", task);
			task = task_list_entry(task->task_list[SCHED_LIST1].next, SCHED_LIST1);
		printk(" task after calling nxt  %p \n", task);
		return best;
		} while(task != best);

		printk(" -- 7 --\n");
		return (new_schedule == NULL) ? best : new_schedule;
	}

	struct rt_sched_local rma = {
		.base.name = "RMA",
		.base.id = SCHED_RT_RMA,
		.flags = 0,
		.schedule = sched_rma,
		.base.sort_key = SORT_KEY_DEADLINE,
		.base.list = LIST_HEAD_INIT(rma.base.list)
	};

	static int __init rma_init(void)
	{
		return add_local_scheduler(&rma);
	}
	module_init(rma_init);

	static void __exit rma_exit(void)
	{
		remove_local_scheduler(&rma);
	}
	module_exit(rma_exit);

	MODULE_DESCRIPTION("RMA Single-Core Scheduling Module for ChronOS");
	MODULE_AUTHOR("Matthew Dellinger <matthew@mdelling.com>");
	MODULE_LICENSE("GPL");

