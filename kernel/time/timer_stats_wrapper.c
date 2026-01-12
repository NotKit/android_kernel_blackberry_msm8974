
#include <linux/module.h>
/* Force timer.h to see CONFIG_TIMER_STATS enabled so we can implement the extern functions */
#undef CONFIG_TIMER_STATS
#define CONFIG_TIMER_STATS 1
#include <linux/timer.h>
#include <linux/kallsyms.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

void timer_stats_timer_set_start_info(struct timer_list *timer)
{
	if (likely(!timer_stats_active))
		return;

	timer->start_site = __builtin_return_address(0);
	timer->start_pid = current->pid;
	memcpy(timer->start_comm, current->comm, TASK_COMM_LEN);
	timer->start_comm[TASK_COMM_LEN] = 0;
}
EXPORT_SYMBOL_GPL(timer_stats_timer_set_start_info);

void timer_stats_timer_clear_start_info(struct timer_list *timer)
{
	timer->start_site = NULL;
}
EXPORT_SYMBOL_GPL(timer_stats_timer_clear_start_info);
