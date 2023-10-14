#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  backtrace();
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_sigalarm(void){
    int ticks;
    uint64 handler;
    argint(0,&ticks);
    argaddr(1,&handler);
    struct proc*p=myproc();
    p->ticks=ticks;
    p->handler=handler;
    p->ticks_pass=0;
    return 0;
}

uint64
sys_sigreturn(void){
    struct proc*p=myproc();
    p->trapframe->epc=p->alarm_context.epc;
    p->trapframe->a0=p->alarm_context.a0;
    p->trapframe->a1=p->alarm_context.a1;
    p->trapframe->a2=p->alarm_context.a2;
    p->trapframe->a3=p->alarm_context.a3;
    p->trapframe->a4=p->alarm_context.a4;
    p->trapframe->a5=p->alarm_context.a5;
    p->trapframe->a6=p->alarm_context.a6;
    p->trapframe->a7=p->alarm_context.a7;
    p->trapframe->sp=p->alarm_context.sp;
    p->trapframe->s0=p->alarm_context.s0;
    p->trapframe->s1=p->alarm_context.s1;
    p->trapframe->s2=p->alarm_context.s2;
    p->trapframe->s3=p->alarm_context.s3;
    p->trapframe->s4=p->alarm_context.s4;
    p->trapframe->s5=p->alarm_context.s5;
    p->trapframe->s6=p->alarm_context.s6;
    p->trapframe->s7=p->alarm_context.s7;
    p->trapframe->s8=p->alarm_context.s8;
    p->trapframe->s9=p->alarm_context.s9;
    p->trapframe->s10=p->alarm_context.s10;
    p->trapframe->s11=p->alarm_context.s11;
    p->trapframe->gp=p->alarm_context.gp;
    p->trapframe->ra=p->alarm_context.ra;
    p->trapframe->tp=p->alarm_context.tp;
    p->alarm_status=0;
    return p->alarm_context.a0;
}