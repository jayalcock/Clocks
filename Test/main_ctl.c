#include <string.h>
#include <ctl_api.h>

CTL_TASK_t main_task, new_task;

#define CALLSTACKSIZE 16 // this is only required for AVR builds
#define STACKSIZE 64          
unsigned new_task_stack[1+STACKSIZE+1];

void 
new_task_code(void *p)
{  
  unsigned int v=0;
  while (1)
    {      
      // task logic goes here      
      v++;
    }  
}

void
ctl_handle_error(CTL_ERROR_CODE_t e)
{
  while (1);
}

int main(void)
{
  unsigned int v=0;
  ctl_task_init(&main_task, 255, "main"); // create subsequent tasks whilst running at the highest priority.
  ctl_start_timer(ctl_increment_tick_from_isr); // start the timer 
  memset(new_task_stack, 0xcd, sizeof(new_task_stack));  // write known values into the stack
  new_task_stack[0]=new_task_stack[1+STACKSIZE]=0xfacefeed; // put marker values at the words before/after the stack
  ctl_task_run(&new_task, 1, new_task_code, 0, "new_task", STACKSIZE, new_task_stack+1, CALLSTACKSIZE);
  ctl_task_set_priority(&main_task, 0); // drop to lowest priority to start created tasks running.
  while (1)
    {    
      // power down can go here if supported      
      v++;
    }
  return 0;
}
