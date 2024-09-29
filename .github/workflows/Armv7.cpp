# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: #
# Assorted ARMv7M macros
#

echo Loading ARMv7M GDB macros.  Use 'help armv7m' for more information.\n

define armv7m
        echo Use 'help armv7m' for more information.\n
end

document armv7m
. Various macros for working with the ARMv7M family of processors.
.
.    vecstate
.        Print information about the current exception handling state.
.
. Use 'help <macro>' for more specific help.
end


define vecstate
        set $icsr  = *(unsigned *)0xe000ed04
        set $vect  = $icsr & 0x1ff
        set $pend  = ($icsr & 0x1ff000) >> 12
        set $shcsr = *(unsigned *)0xe000ed24
        set $cfsr  = *(unsigned *)0xe000ed28
        set $mmfsr = $cfsr & 0xff
        set $bfsr  = ($cfsr >> 8) & 0xff
        set $ufsr  = ($cfsr >> 16) & 0xffff
        set $hfsr  = *(unsigned *)0xe000ed2c
        set $bfar  = *(unsigned *)0xe000ed38
        set $mmfar = *(unsigned *)0xe000ed34

        if $vect < 15

                if $hfsr != 0
                        printf "HardFault:"
                        if $hfsr & (1<<1)
                                printf " due to vector table read fault\n"
                        end
                        if $hfsr & (1<<30)
                                printf " forced due to escalated or disabled configurable fault (see below)\n"
                        end
                        if $hfsr & (1<<31)
                                printf " due to an unexpected debug event\n"
                        end
                end
                if $mmfsr != 0
                        printf "MemManage:"
                        if $mmfsr & (1<<5)
                                printf " during lazy FP state save"
                        end
                        if $mmfsr & (1<<4)
                                printf " during exception entry"
                        end
                        if $mmfsr & (1<<3)
                                printf " during exception return"
                        end
                        if $mmfsr & (1<<1)
                                printf " during data access"
                        end
                        if $mmfsr & (1<<0)
                                printf " during instruction prefetch"
                        end
                        if $mmfsr & (1<<7)
                                printf " accessing 0x%08x", $mmfar
                        end
                        printf "\n"
                end
                if $bfsr != 0
                        printf "BusFault:"
                        if $bfsr & (1<<2)
                                printf " (imprecise)"
                        end
                        if $bfsr & (1<<1)
                                printf " (precise)"
                        end
                        if $bfsr & (1<<5)
                                printf " during lazy FP state save"
                        end
                        if $bfsr & (1<<4)
                                printf " during exception entry"
                        end
                        if $bfsr & (1<<3)
                                printf " during exception return"
                        end
                        if $bfsr & (1<<0)
                                printf " during instruction prefetch"
                        end
                        if $bfsr & (1<<7)
                                printf " accessing 0x%08x", $bfar
                        end
                        printf "\n"
                end
                if $ufsr != 0
                        printf "UsageFault"
                        if $ufsr & (1<<9)
                                printf " due to divide-by-zero"
                        end
                        if $ufsr & (1<<8)
                                printf " due to unaligned memory access"
                        end
                        if $ufsr & (1<<3)
                                printf " due to access to disabled/absent coprocessor"
                        end
                        if $ufsr & (1<<2)
                                printf " due to a bad EXC_RETURN value"
                        end
                        if $ufsr & (1<<1)
                                printf " due to bad T or IT bits in EPSR"
                        end
                        if $ufsr & (1<<0)
                                printf " due to executing an undefined instruction"
                        end
                        printf "\n"
                end
        else
                if $vect >= 15
                        printf "Handling vector %u\n", $vect
                end
        end
        if ((unsigned)$lr & 0xf0000000) == 0xf0000000
                if ($lr & 1)
                        printf "exception frame is on MSP\n"
                        #set $frame_ptr = (unsigned *)$msp
                        set $frame_ptr = (unsigned *)$sp
                else
                        printf "exception frame is on PSP, backtrace may not be possible\n"
                        #set $frame_ptr = (unsigned *)$psp
                        set $frame_ptr = (unsigned *)$sp
                end
                if $lr & 0x10
                        set $fault_sp = $frame_ptr + (8 * 4)
                else
                        set $fault_sp = $frame_ptr + (26 * 4)
                end


                printf "  r0: %08x  r1: %08x  r2: %08x  r3: %08x\n", $frame_ptr[0], $frame_ptr[1], $frame_ptr[2], $frame_ptr[3]
                printf "  r4: %08x  r5: %08x  r6: %08x  r7: %08x\n", $r4, $r5, $r6, $r7
                printf "  r8: %08x  r9: %08x r10: %08x r11: %08x\n", $r8, $r9, $r10, $r11
                printf " r12: %08x\n", $frame_ptr[4]
                printf "  sp: %08x  lr: %08x  pc: %08x PSR: %08x\n", $fault_sp, $frame_ptr[5], $frame_ptr[6], $frame_ptr[7]

                # Swap to the context of the faulting code and try to print a backtrace
                set $saved_sp = $sp
                set $sp = $fault_sp
                set $saved_lr = $lr
                set $lr = $frame_ptr[5]
                set $saved_pc = $pc
                set $pc = $frame_ptr[6]
                bt
                set $sp = $saved_sp
                set $lr = $saved_lr
                set $pc = $saved_pc
        else
                printf "(not currently in exception handler)\n"
        end
end

document vecstate
.    vecstate
.        Print information about the current exception handling state.
end
  push: #
# Generic GDB macros for working with NuttX
#

echo Loading NuttX GDB macros.  Use 'help nuttx' for more information.\n

define nuttx
        echo Use 'help nuttx' for more information.\n
end

document nuttx
. Various macros for working with NuttX.
.
.    showheap
.        Prints the contents of the malloc heap(s).
.    showtasks
.        Prints a list of all tasks.
.    showtask <address>
.        Prints information about the task at <address>
.
. Use 'help <macro>' for more specific help.
end

################################################################################
# Heap display
################################################################################

define _showheap
        set $index = $arg0
        set $used = 0
        set $free = 0
        if (sizeof(struct mm_allocnode_s) == 4)
                set $MM_ALLOC_BIT = 0x8000
        else
                set $MM_ALLOC_BIT = 0x80000000
        end
        printf "HEAP %d  %p - %p\n", $index, g_mmheap.mm_heapstart[$index], g_mmheap.mm_heapend[$index]
        printf "ptr      size\n"
        set $node = (char *)g_mmheap.mm_heapstart[$index] + sizeof(struct mm_allocnode_s)
        while $node < g_mmheap.mm_heapend[$index]
                printf "  %p", $node
                set $nodestruct = (struct mm_allocnode_s *)$node
                printf "  %u", $nodestruct->size
                if !($nodestruct->preceding & $MM_ALLOC_BIT)
                        printf " FREE"
                        set $free = $free + $nodestruct->size
                else
                        set $used = $used + $nodestruct->size
                end
                if ($nodestruct->size > g_mmheap.mm_heapsize) || (($node + $nodestruct->size) > g_mmheap.mm_heapend[$index])
                        printf "  (BAD SIZE)"
                end
                printf "\n"
                set $node = $node + $nodestruct->size
        end
        printf "  ----------\n"
        printf "  Used:       %u\n", $used
        printf "  Free:       %u\n\n", $free
end

define showheap
        set $nheaps = sizeof(g_mmheap.mm_heapstart) / sizeof(g_mmheap.mm_heapstart[0])
        printf "Printing %d heaps\n", $nheaps
        set $heapindex = (int)0
        while $heapindex < $nheaps
                _showheap $heapindex
                set $heapindex = $heapindex + 1
        end
end

document showheap
.    showheap
.        Prints the contents of the malloc heap(s).
end

################################################################################
# Task file listing 
################################################################################

define showfiles
        set $task = (struct tcb_s *)$arg0
        set $nfiles = sizeof((*(struct filelist*)0).fl_files) / sizeof(struct file)
        printf "%d files\n", $nfiles
        set $index = 0
        while $index < $nfiles
                set $file = &($task->filelist->fl_files[$index])
                printf "%d: inode %p f_priv %p\n", $index, $file->f_inode, $file->f_priv
                if $file->f_inode != 0
                        printf "    i_name %s  i_private %p\n", &$file->f_inode->i_name[0], $file->f_inode->i_private
                end
                set $index = $index + 1
        end
end

document showfiles
.    showfiles <TCB pointer>
.        Prints the files opened by a task.
end

################################################################################
# Task display
################################################################################

define _showtask_oneline
        set $task = (struct tcb_s *)$arg0
        printf "    %p  %.2d %.3d %s\n", $task, $task->pid, $task->sched_priority, $task->name
end

define _showtasklist
        set $queue = (dq_queue_t *)$arg0
        set $cursor = (dq_entry_t *)$queue->head

        if $cursor != 0
                printf "    TCB        PID PRI\n"
        else
                printf "    <none>\n"
        end

        while $cursor != 0

                _showtask_oneline $cursor

                if $cursor == $queue->tail
                        set $cursor = 0
                else
                        set $next = $cursor->flink

                        if $next->blink != $cursor
                                printf "task linkage corrupt\n"
                                set $cursor = 0
                        else
                                set $cursor = $next
                        end
                end
        end
end

#
# Print task registers for a NuttX v7em target with FPU enabled.
#
define _showtaskregs_v7em
        set $task = (struct tcb_s *)$arg0
        set $regs = (uint32_t *)&($task->xcp.regs[0])

        printf "    r0: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", $regs[27], $regs[28], $regs[29], $regs[30], $regs[2], $regs[3], $regs[4], $regs[5]
        printf "    r8: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", $regs[6], $regs[7], $regs[8], $regs[9], $regs[31], $regs[0], $regs[32], $regs[33]
        printf "    XPSR 0x%08x EXC_RETURN 0x%08x PRIMASK 0x%08x\n", $regs[34], $regs[10], $regs[1]
end

#
# Print current registers for a NuttX v7em target with FPU enabled.
#
define _showcurrentregs_v7em
        printf "    r0: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", $r0, $r1, $r2, $r3, $r4, $r5, $r6, $r7
        printf "    r8: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", $r8, $r9, $r10, $r11, $r12, $r13, $r14, $r15
        printf "    XPSR 0x%08x\n", $xpsr
end

#
# Print details of a semaphore
#
define _showsemaphore
        printf "count %d ", $arg0->semcount
        if $arg0->holder.htcb != 0
                set $_task = (struct tcb_s *)$arg0->holder.htcb
                printf "held by %s", $_task->name
        end
        printf "\n"
end

#
# Print information about a task's stack usage
#
define showtaskstack
        set $task = (struct tcb_s *)$arg0

        if $task == &g_idletcb
                printf "can't measure idle stack\n"
        else
                set $stack_free = 0
                while ($stack_free < $task->adj_stack_size) && ((uint32_t *)($task->stack_alloc_ptr))[$stack_free] == 0xffffffff
                        set $stack_free = $stack_free + 1
                end
                set $stack_free = $stack_free * 4
                printf"    stack 0x%08x-0x%08x (%d) %d free\n", $task->stack_alloc_ptr, $task->adj_stack_ptr, $task->adj_stack_size, $stack_free
        end
end

#
# Print details of a task
#
define showtask
        set $task = (struct tcb_s *)$arg0

        printf "%p %.2d ", $task, $task->pid
        _showtaskstate $task
        printf " %s\n", $task->name
        showtaskstack $task

        if $task->task_state == TSTATE_WAIT_SEM
                printf "    waiting on %p ", $task->waitsem
                _showsemaphore $task->waitsem
        end

        if $task->task_state != TSTATE_TASK_RUNNING
                _showtaskregs_v7em $task
        else
                _showtaskregs_v7em $task
        end

        # XXX print registers here
end

document showtask
.    showtask <TCB pointer>
.        Print details of a task.
end

define _showtaskstate
        if $arg0->task_state == TSTATE_TASK_INVALID
                printf "INVALID"
        end
        if $arg0->task_state == TSTATE_TASK_PENDING
                printf "PENDING"
        end
        if $arg0->task_state == TSTATE_TASK_READYTORUN
                printf "READYTORUN"
        end
        if $arg0->task_state == TSTATE_TASK_RUNNING
                printf "RUNNING"
        end
        if $arg0->task_state == TSTATE_TASK_INACTIVE
                printf "INACTIVE"
        end
        if $arg0->task_state == TSTATE_WAIT_SEM
                printf "WAIT_SEM"
        end
        if $arg0->task_state == TSTATE_WAIT_SIG
                printf "WAIT_SIG"
        end
        if $arg0->task_state > TSTATE_WAIT_SIG
                printf "%d", $arg0->task_state
        end
end

define showtasks
        printf "PENDING\n"
        _showtasklist &g_pendingtasks
        printf "RUNNABLE\n"
        _showtasklist &g_readytorun
        printf "WAITING for Semaphore\n"
        _showtasklist &g_waitingforsemaphore
        printf "WAITING for Signal\n"
        _showtasklist &g_waitingforsignal
        printf "INACTIVE\n"
        _showtasklist &g_inactivetasks
end

document showtasks
.    showtasks
.        Print a list of all tasks in the system, separated into their respective queues.
end

define my_mem

    set $start = $arg0
    set $end = $arg1
    set $cursor = $start

    if $start < $end
      while $cursor != $end
        set *$cursor = 0x0000
        set $cursor = $cursor + 4
        printf "0x%x of 0x%x\n",$cursor,$end 
      end
    else
      while $cursor != $end
        set *$cursor = 0x0000
        set $cursor = $cursor - 4
      end
    end
end
    branches: [ "main" ]
  pull_request: #
# Setup macros for the BlackMagic debug probe and NuttX.
#

mon swdp_scan
attach 1
    branches: [ "main" ]
  schedule: # GDB/Python functions for dealing with NuttX

from __future__ import print_function
import gdb, gdb.types

parse_int = lambda x: int(str(x), 0)

class NX_register_set(object):
        """Copy of the registers for a given context"""

        v7_regmap = {
                'R13':                0,
                'SP':                0,
                'PRIORITY':        1,
                'R4':                2,
                'R5':                3,
                'R6':                4,
                'R7':                5,
                'R8':                6,
                'R9':                7,
                'R10':                8,
                'R11':                9,
                'EXC_RETURN':        10,
                'R0':                11,
                'R1':                12,
                'R2':                13,
                'R3':                14,
                'R12':                15,
                'R14':                16,
                'LR':                16,
                'R15':                17,
                'PC':                17,
                'XPSR':                18,
        }

        v7em_regmap = {
                'R13':                0,
                'SP':                0,
                'PRIORITY':        1,
                'R4':                2,
                'R5':                3,
                'R6':                4,
                'R7':                5,
                'R8':                6,
                'R9':                7,
                'R10':                8,
                'R11':                9,
                'EXC_RETURN':        10,
                'R0':                27,
                'R1':                28,
                'R2':                29,
                'R3':                30,
                'R12':                31,
                'R14':                32,
                'LR':                32,
                'R15':                33,
                'PC':                33,
                'XPSR':                34,
        }

        regs = dict()

        def __init__(self, xcpt_regs):
                if xcpt_regs is None:
                        self.regs['R0']         = self.mon_reg_call('r0')
                        self.regs['R1']         = self.mon_reg_call('r1')
                        self.regs['R2']         = self.mon_reg_call('r2')
                        self.regs['R3']         = self.mon_reg_call('r3')
                        self.regs['R4']         = self.mon_reg_call('r4')
                        self.regs['R5']         = self.mon_reg_call('r5')
                        self.regs['R6']         = self.mon_reg_call('r6')
                        self.regs['R7']         = self.mon_reg_call('r7')
                        self.regs['R8']         = self.mon_reg_call('r8')
                        self.regs['R9']         = self.mon_reg_call('r9')
                        self.regs['R10']        = self.mon_reg_call('r10')
                        self.regs['R11']        = self.mon_reg_call('r11')
                        self.regs['R12']        = self.mon_reg_call('r12')
                        self.regs['R13']        = self.mon_reg_call('r13')
                        self.regs['SP']         = self.mon_reg_call('sp')
                        self.regs['R14']        = self.mon_reg_call('r14')
                        self.regs['LR']         = self.mon_reg_call('lr')
                        self.regs['R15']        = self.mon_reg_call('r15')
                        self.regs['PC']         = self.mon_reg_call('pc')
                        self.regs['XPSR']       = self.mon_reg_call('xPSR')
                else:
                        for key in self.v7em_regmap.keys():
                                self.regs[key] = int(xcpt_regs[self.v7em_regmap[key]])

        def mon_reg_call(self,register):
                """
                register is the register as a string e.g. 'pc'
                return integer containing the value of the register
                """
                str_to_eval = "mon reg "+register
                resp = gdb.execute(str_to_eval,to_string = True)
                content = resp.split()[-1];
                try:
                        return int(content,16)
                except:
                        return 0

        @classmethod
        def with_xcpt_regs(cls, xcpt_regs):
                return cls(xcpt_regs)

        @classmethod
        def for_current(cls):
                return cls(None)

        def __format__(self, format_spec):
                return format_spec.format(
                        registers         = self.registers
                        )

        @property
        def registers(self):
                return self.regs


class NX_task(object):
        """Reference to a NuttX task and methods for introspecting it"""

        def __init__(self, tcb_ptr):
                self._tcb = tcb_ptr.dereference()
                self._group = self._tcb['group'].dereference()
                self.pid = tcb_ptr['pid']

        @classmethod
        def for_tcb(cls, tcb):
                """return a task with the given TCB pointer"""
                pidhash_sym = gdb.lookup_global_symbol('g_pidhash')
                pidhash_value = pidhash_sym.value()
                pidhash_type = pidhash_sym.type
                for i in range(pidhash_type.range()[0],pidhash_type.range()[1]):
                        pidhash_entry = pidhash_value[i]
                        if pidhash_entry['tcb'] == tcb:
                                return cls(pidhash_entry['tcb'])
                return None

        @classmethod
        def for_pid(cls, pid):
                """return a task for the given PID"""
                pidhash_sym = gdb.lookup_global_symbol('g_pidhash')
                pidhash_value = pidhash_sym.value()
                pidhash_type = pidhash_sym.type
                for i in range(pidhash_type.range()[0],pidhash_type.range()[1]):
                        pidhash_entry = pidhash_value[i]
                        if pidhash_entry['pid'] == pid:
                                return cls(pidhash_entry['tcb'])
                return None

        @staticmethod
        def pids():
                """return a list of all PIDs"""
                pidhash_sym = gdb.lookup_global_symbol('g_pidhash')
                pidhash_value = pidhash_sym.value()
                pidhash_type = pidhash_sym.type
                result = []
                for i in range(pidhash_type.range()[0],pidhash_type.range()[1]):
                        entry = pidhash_value[i]
                        pid = parse_int(entry['pid'])
                        if pid is not -1:
                                result.append(pid)
                return result

        @staticmethod
        def tasks():
                """return a list of all tasks"""
                tasks = []
                for pid in NX_task.pids():
                        tasks.append(NX_task.for_pid(pid))
                return tasks

        def _state_is(self, state):
                """tests the current state of the task against the passed-in state name"""
                statenames = gdb.types.make_enum_dict(gdb.lookup_type('enum tstate_e'))
                if self._tcb['task_state'] == statenames[state]:
                        return True
                return False

        @property
        def stack_used(self):
                """calculate the stack used by the thread"""
                if 'stack_used' not in self.__dict__:
                        stack_base = self._tcb['stack_alloc_ptr'].cast(gdb.lookup_type('unsigned char').pointer())
                        if stack_base == 0:
                                self.__dict__['stack_used'] = 0
                        else:
                                stack_limit = self._tcb['adj_stack_size']
                                for offset in range(0, parse_int(stack_limit)):
                                        if stack_base[offset] != 0xff:
                                                break
                                self.__dict__['stack_used'] = stack_limit - offset
                return self.__dict__['stack_used']

        @property
        def name(self):
                """return the task's name"""
                return self._tcb['name'].string()

        @property
        def state(self):
                """return the name of the task's current state"""
                statenames = gdb.types.make_enum_dict(gdb.lookup_type('enum tstate_e'))
                for name,value in statenames.items():
                        if value == self._tcb['task_state']:
                                return name
                return 'UNKNOWN'

        @property
        def waiting_for(self):
                """return a description of what the task is waiting for, if it is waiting"""
                if self._state_is('TSTATE_WAIT_SEM'):
                        try: 
                                waitsem = self._tcb['waitsem'].dereference()
                                waitsem_holder = waitsem['holder']
                                holder = NX_task.for_tcb(waitsem_holder['htcb'])
                                if holder is not None:
                                        return '{}({})'.format(waitsem.address, holder.name)
                                else:
                                        return '{}(<bad holder>)'.format(waitsem.address)
                        except:
                                return 'EXCEPTION'
                if self._state_is('TSTATE_WAIT_SIG'):
                        return 'signal'
                return ""

        @property
        def is_waiting(self):
                """tests whether the task is waiting for something"""
                if self._state_is('TSTATE_WAIT_SEM') or self._state_is('TSTATE_WAIT_SIG'):
                        return True

        @property
        def is_runnable(self):
                """tests whether the task is runnable"""
                if (self._state_is('TSTATE_TASK_PENDING') or 
                        self._state_is('TSTATE_TASK_READYTORUN') or 
                        self._state_is('TSTATE_TASK_RUNNING')):
                        return True
                return False

        @property
        def file_descriptors(self):
                """return a dictionary of file descriptors and inode pointers"""
                filelist = self._group['tg_filelist']
                filearray = filelist['fl_files']
                result = dict()
                for i in range(filearray.type.range()[0],filearray.type.range()[1]):
                        inode = parse_int(filearray[i]['f_inode'])
                        if inode != 0:
                                result[i] = inode
                return result

        @property
        def registers(self):
                if 'registers' not in self.__dict__:
                        registers = dict()
                        if self._state_is('TSTATE_TASK_RUNNING'):
                                registers = NX_register_set.for_current().registers
                        else:
                                context = self._tcb['xcp']
                                regs = context['regs']
                                registers = NX_register_set.with_xcpt_regs(regs).registers

                        self.__dict__['registers'] = registers
                return self.__dict__['registers']

        def __repr__(self):
                return "<NX_task {}>".format(self.pid)

        def __str__(self):
                return "{}:{}".format(self.pid, self.name)

        def showoff(self):
                print("-------")
                print(self.pid,end = ", ")
                print(self.name,end = ", ")
                print(self.state,end = ", ")
                print(self.waiting_for,end = ", ")
                print(self.stack_used,end = ", ")
                print(self._tcb['adj_stack_size'],end = ", ")
                print(self.file_descriptors)
                print(self.registers)

        def __format__(self, format_spec):
                return format_spec.format(
                        pid              = self.pid,
                        name             = self.name,
                        state            = self.state,
                        waiting_for      = self.waiting_for,
                        stack_used       = self.stack_used,
                        stack_limit      = self._tcb['adj_stack_size'],
                        file_descriptors = self.file_descriptors,
                        registers         = self.registers
                        )

class NX_show_task (gdb.Command):
        """(NuttX) prints information about a task"""

        def __init__(self):
                super(NX_show_task, self).__init__("show task", gdb.COMMAND_USER)

        def invoke(self, arg, from_tty):
                t = NX_task.for_pid(parse_int(arg))
                if t is not None:
                        my_fmt = 'PID:{pid}  name:{name}  state:{state}\n'
                        my_fmt += '  stack used {stack_used} of {stack_limit}\n'
                        if t.is_waiting:
                                my_fmt += '  waiting for {waiting_for}\n'
                        my_fmt += '  open files: {file_descriptors}\n'
                        my_fmt += '  R0  {registers[R0]:#010x} {registers[R1]:#010x} {registers[R2]:#010x} {registers[R3]:#010x}\n'
                        my_fmt += '  R4  {registers[R4]:#010x} {registers[R5]:#010x} {registers[R6]:#010x} {registers[R7]:#010x}\n'
                        my_fmt += '  R8  {registers[R8]:#010x} {registers[R9]:#010x} {registers[R10]:#010x} {registers[R11]:#010x}\n'
                        my_fmt += '  R12 {registers[PC]:#010x}\n'
                        my_fmt += '  SP  {registers[SP]:#010x} LR {registers[LR]:#010x} PC {registers[PC]:#010x} XPSR {registers[XPSR]:#010x}\n'
                        print(format(t, my_fmt))

class NX_show_tasks (gdb.Command):
        """(NuttX) prints a list of tasks"""

        def __init__(self):
                super(NX_show_tasks, self).__init__('show tasks', gdb.COMMAND_USER)

        def invoke(self, args, from_tty):
                tasks = NX_task.tasks()
                print ('Number of tasks: ' + str(len(tasks)))
                for t in tasks:
                        #t.showoff()
                        print(format(t, 'Task: {pid} {name} {state} {stack_used}/{stack_limit}'))

NX_show_task()
NX_show_tasks()

class NX_show_heap (gdb.Command):
        """(NuttX) prints the heap"""

        def __init__(self):
                super(NX_show_heap, self).__init__('show heap', gdb.COMMAND_USER)
                struct_mm_allocnode_s = gdb.lookup_type('struct mm_allocnode_s')
                preceding_size = struct_mm_allocnode_s['preceding'].type.sizeof
                if preceding_size == 2:
                        self._allocflag = 0x8000
                elif preceding_size == 4:
                        self._allocflag = 0x80000000
                else:
                        raise gdb.GdbError('invalid mm_allocnode_s.preceding size %u' % preceding_size)
                        self._allocnodesize = struct_mm_allocnode_s.sizeof

        def _node_allocated(self, allocnode):
                if allocnode['preceding'] & self._allocflag:
                        return True
                return False

        def _node_size(self, allocnode):
                return allocnode['size'] & ~self._allocflag

        def _print_allocations(self, region_start, region_end):
                if region_start >= region_end:
                        raise gdb.GdbError('heap region {} corrupt'.format(hex(region_start)))
                nodecount = region_end - region_start
                print ('heap {} - {}'.format(region_start, region_end))
                cursor = 1
                while cursor < nodecount:
                        allocnode = region_start[cursor]
                        if self._node_allocated(allocnode):
                                state = ''
                        else:
                                state = '(free)'
                        print( '  {} {} {}'.format(allocnode.address + self._allocnodesize,
                                                  self._node_size(allocnode), state))
                        cursor += self._node_size(allocnode) / self._allocnodesize

        def invoke(self, args, from_tty):
                heap = gdb.lookup_global_symbol('g_mmheap').value()
                nregions = heap['mm_nregions']
                region_starts = heap['mm_heapstart']
                region_ends = heap['mm_heapend']
                print( '{} heap(s)'.format(nregions))
                # walk the heaps
                for i in range(0, nregions):
                        self._print_allocations(region_starts[i], region_ends[i])

NX_show_heap()

class NX_show_interrupted_thread (gdb.Command):
        """(NuttX) prints the register state of an interrupted thread when in interrupt/exception context"""

        def __init__(self):
                super(NX_show_interrupted_thread, self).__init__('show interrupted-thread', gdb.COMMAND_USER)

        def invoke(self, args, from_tty):
                regs = gdb.lookup_global_symbol('current_regs').value()
                if regs is 0:
                        raise gdb.GdbError('not in interrupt context')
                else:
                        registers = NX_register_set.with_xcpt_regs(regs)
                        my_fmt = ''
                        my_fmt += '  R0  {registers[R0]:#010x} {registers[R1]:#010x} {registers[R2]:#010x} {registers[R3]:#010x}\n'
                        my_fmt += '  R4  {registers[R4]:#010x} {registers[R5]:#010x} {registers[R6]:#010x} {registers[R7]:#010x}\n'
                        my_fmt += '  R8  {registers[R8]:#010x} {registers[R9]:#010x} {registers[R10]:#010x} {registers[R11]:#010x}\n'
                        my_fmt += '  R12 {registers[PC]:#010x}\n'
                        my_fmt += '  SP  {registers[SP]:#010x} LR {registers[LR]:#010x} PC {registers[PC]:#010x} XPSR {registers[XPSR]:#010x}\n'
                        print (format(registers, my_fmt))

NX_show_interrupted_thread()

class NX_check_tcb(gdb.Command):
        """ check the tcb of a task from a address """
        def __init__(self):
                super(NX_check_tcb,self).__init__('show tcb', gdb.COMMAND_USER)

        def invoke(self,args,sth):
                tasks = NX_task.tasks()
                print("tcb int: ",int(args))
                print(tasks[int(args)]._tcb)
                a =tasks[int(args)]._tcb['xcp']['regs']
                print("relevant registers:")
                for reg in regmap:
                        hex_addr= hex(int(a[regmap[reg]]))
                        eval_string = 'info line *'+str(hex_addr)
                        print(reg,": ",hex_addr,)
NX_check_tcb()

class NX_tcb(object):
        def __init__(self):
                pass

        def is_in(self,arg,list):
                for i in list:
                        if arg == i:
                                return True;
                return False

        def find_tcb_list(self,dq_entry_t):
                tcb_list = []
                tcb_ptr = dq_entry_t.cast(gdb.lookup_type('struct tcb_s').pointer())
                first_tcb = tcb_ptr.dereference()
                tcb_list.append(first_tcb);
                next_tcb = first_tcb['flink'].dereference()
                while not self.is_in(parse_int(next_tcb['pid']),[parse_int(t['pid']) for t in tcb_list]):
                        tcb_list.append(next_tcb); 
                        old_tcb = next_tcb;
                        next_tcb = old_tcb['flink'].dereference()

                return [t for t in tcb_list if parse_int(t['pid'])<2000]

        def getTCB(self):
                list_of_listsnames = ['g_pendingtasks','g_readytorun','g_waitingforsemaphore','g_waitingforsignal','g_inactivetasks']
                tcb_list = [];
                for l in list_of_listsnames:
                        li = gdb.lookup_global_symbol(l)
                        print(li)
                        cursor = li.value()['head']
                        tcb_list = tcb_list + self.find_tcb_list(cursor)

class NX_check_stack_order(gdb.Command):
        """ Check the Stack order corresponding to the tasks """

        def __init__(self):
                super(NX_check_stack_order,self).__init__('show check_stack', gdb.COMMAND_USER)

        def is_in(self,arg,list):
                for i in list:
                        if arg == i:
                                return True;
                return False

        def find_tcb_list(self,dq_entry_t):
                tcb_list = []
                tcb_ptr = dq_entry_t.cast(gdb.lookup_type('struct tcb_s').pointer())
                first_tcb = tcb_ptr.dereference()
                tcb_list.append(first_tcb);
                next_tcb = first_tcb['flink'].dereference()
                while not self.is_in(parse_int(next_tcb['pid']),[parse_int(t['pid']) for t in tcb_list]):
                        tcb_list.append(next_tcb); 
                        old_tcb = next_tcb;
                        next_tcb = old_tcb['flink'].dereference()

                return [t for t in tcb_list if parse_int(t['pid'])<2000]

        def getTCB(self):
                list_of_listsnames = ['g_pendingtasks','g_readytorun','g_waitingforsemaphore','g_waitingforsignal','g_inactivetasks']
                tcb_list = [];
                for l in list_of_listsnames:
                        li = gdb.lookup_global_symbol(l)
                        cursor = li.value()['head']
                        tcb_list = tcb_list + self.find_tcb_list(cursor)
                return tcb_list

        def getSPfromTask(self,tcb):
                regmap = NX_register_set.v7em_regmap
                a =tcb['xcp']['regs']
                return         parse_int(a[regmap['SP']])

        def find_closest(self,list,val):
                tmp_list = [abs(i-val) for i in list]
                tmp_min = min(tmp_list)
                idx = tmp_list.index(tmp_min)
                return idx,list[idx]

        def find_next_stack(self,address,_dict_in):
                add_list = []
                name_list = []
                for key in _dict_in.keys():
                        for i in range(3):
                                if _dict_in[key][i] < address:
                                        add_list.append(_dict_in[key][i])
                                        if i == 2: # the last one is the processes stack pointer
                                                name_list.append(self.check_name(key)+"_SP")
                                        else:
                                                name_list.append(self.check_name(key))

                idx,new_address = self.find_closest(add_list,address)
                return new_address,name_list[idx]

        def check_name(self,name):
                if isinstance(name,(list)):
                        name = name[0];
                idx = name.find("\\")
                newname = name[:idx]

                return newname

        def invoke(self,args,sth):
                tcb = self.getTCB();
                stackadresses={};
                for t in tcb:
                        p = [];
                        #print(t.name,t._tcb['stack_alloc_ptr'])
                        p.append(parse_int(t['stack_alloc_ptr']))
                        p.append(parse_int(t['adj_stack_ptr']))
                        p.append(self.getSPfromTask(t))
                        stackadresses[str(t['name'])] = p;
                address = int("0x30000000",0)
                print("stack address  :  process")
                for i in range(len(stackadresses)*3):
                          address,name = self.find_next_stack(address,stackadresses)
                          print(hex(address),": ",name)

NX_check_stack_order()

class NX_run_debug_util(gdb.Command):
        """ show the registers of a task corresponding to a tcb address"""
        def __init__(self):
                super(NX_run_debug_util,self).__init__('show regs', gdb.COMMAND_USER)

        def printRegisters(self,task):
                regmap = NX_register_set.v7em_regmap
                a =task._tcb['xcp']['regs']
                print("relevant registers in ",task.name,":")
                for reg in regmap:
                        hex_addr= hex(int(a[regmap[reg]]))
                        eval_string = 'info line *'+str(hex_addr)
                        print(reg,": ",hex_addr,)

        def getPCfromTask(self,task):
                regmap = NX_register_set.v7em_regmap
                a =task._tcb['xcp']['regs']
                return         hex(int(a[regmap['PC']]))

        def invoke(self,args,sth):
                tasks = NX_task.tasks() 
                if args == '':
                        for t in tasks:
                                self.printRegisters(t)
                                eval_str = "list *"+str(self.getPCfromTask(t))
                                print("this is the location in code where the current threads $pc is:")
                                gdb.execute(eval_str)
                else:
                        tcb_nr = int(args);
                        print("tcb_nr = ",tcb_nr)
                        t = tasks[tcb_nr]
                        self.printRegisters(t)
                        eval_str = "list *"+str(self.getPCfromTask(t))
                        print("this is the location in code where the current threads $pc is:")
                        gdb.execute(eval_str)

NX_run_debug_util()


class NX_search_tcb(gdb.Command):
        """ shot PID's of all running tasks """

        def __init__(self):
                super(NX_search_tcb,self).__init__('show alltcb', gdb.COMMAND_USER)

        def is_in(self,arg,list):
                for i in list:
                        if arg == i:
                                return True;
                return False

        def find_tcb_list(self,dq_entry_t):
                tcb_list = []
                tcb_ptr = dq_entry_t.cast(gdb.lookup_type('struct tcb_s').pointer())
                first_tcb = tcb_ptr.dereference()
                tcb_list.append(first_tcb);
                next_tcb = first_tcb['flink'].dereference()
                while not self.is_in(parse_int(next_tcb['pid']),[parse_int(t['pid']) for t in tcb_list]):
                        tcb_list.append(next_tcb); 
                        old_tcb = next_tcb;
                        next_tcb = old_tcb['flink'].dereference()

                return [t for t in tcb_list if parse_int(t['pid'])<2000]

        def invoke(self,args,sth):
                list_of_listsnames = ['g_pendingtasks','g_readytorun','g_waitingforsemaphore','g_waitingforsignal','g_inactivetasks']
                tasks = [];
                for l in list_of_listsnames:
                        li = gdb.lookup_global_symbol(l)
                        cursor = li.value()['head']
                        tasks = tasks + self.find_tcb_list(cursor)

                # filter for tasks that are listed twice
                tasks_filt = {}
                for t in tasks:
                        pid = parse_int(t['pid']);
                        if not pid in tasks_filt.keys():
                                tasks_filt[pid] = t['name']; 
                print('{num_t} Tasks found:'.format(num_t = len(tasks_filt)))
                for pid in tasks_filt.keys():
                        print("PID: ",pid," ",tasks_filt[pid])

NX_search_tcb()


class NX_my_bt(gdb.Command):
        """ 'fake' backtrace: backtrace the stack of a process and check every suspicious address for the list 
        arg: tcb_address$
        (can easily be found by typing 'showtask').
        """

        def __init__(self):
                super(NX_my_bt,self).__init__('show mybt', gdb.COMMAND_USER)

        def readmem(self,addr):
                '''
                read memory at addr and return nr
                '''        
                str_to_eval = "x/x "+hex(addr)
                resp = gdb.execute(str_to_eval,to_string = True)
                idx = resp.find('\t')
                return int(resp[idx:],16)

        def is_in_bounds(self,val):
                lower_bound = int("08004000",16)
                upper_bound = int("080ae0c0",16);
                #print(lower_bound," ",val," ",upper_bound)
                if val>lower_bound and val<upper_bound:
                        return True;
                else:
                        return False;
        def get_tcb_from_address(self,addr):
                addr_value = gdb.Value(addr)
                tcb_ptr = addr_value.cast(gdb.lookup_type('struct tcb_s').pointer())
                return tcb_ptr.dereference()

        def resolve_file_line_func(self,addr,stack_percentage):
                gdb.write(str(round(stack_percentage,2))+":")
                str_to_eval = "info line *"+hex(addr)
                #gdb.execute(str_to_eval)
                res = gdb.execute(str_to_eval,to_string = True)
                # get information from results string:
                words = res.split()
                if words[0] != 'No':
                        line = int(words[1])
                        block = gdb.block_for_pc(addr)
                        func = block.function
                        if str(func) == "None":
                                func = block.superblock.function
                        return words[3].strip('"'), line, func

        def invoke(self,args,sth):
                try:
                        addr_dec = parse_int(args)   # Trying to interpret the input as TCB address
                except ValueError:
                        for task in NX_task.tasks(): # Interpreting as a task name
                                if task.name == args:
                                        _tcb = task._tcb
                                        break
                else:
                        _tcb = self.get_tcb_from_address(addr_dec)

                print("found task with PID: ",_tcb["pid"])
                up_stack = parse_int(_tcb['adj_stack_ptr'])
                curr_sp = parse_int(_tcb['xcp']['regs'][0]) #curr stack pointer
                other_sp = parse_int(_tcb['xcp']['regs'][8]) # other stack pointer
                stacksize = parse_int(_tcb['adj_stack_size']) # other stack pointer

                print("tasks current SP = ",hex(curr_sp),"stack max ptr is at ",hex(up_stack))

                item = 0
                for sp in range(other_sp if curr_sp == up_stack else curr_sp, up_stack, 4):
                        mem = self.readmem(sp)
                        #print(hex(sp)," : ",hex(mem))
                        if self.is_in_bounds(mem):
                                # this is a potential instruction ptr
                                stack_percentage = (up_stack-sp)/stacksize
                                filename,line,func = self.resolve_file_line_func(mem, stack_percentage)
                                print('#%-2d ' % item, '0x%08x in ' % mem, func, ' at ', filename, ':', line, sep='')
                                item += 1

NX_my_bt()
    - cron: #
# Various PX4-specific macros
#
source Debug/NuttX
source Debug/ARMv7M

echo Loading PX4 GDB macros.  Use 'help px4' for more information.\n

define px4
        echo Use 'help px4' for more information.\n
end

document px4
. Various macros for working with the PX4 firmware.
.
.    perf
.        Prints the state of all performance counters.
.
. Use 'help <macro>' for more specific help.
end


define _perf_print
        set $hdr = (struct perf_ctr_header *)$arg0
        #printf "%p\n", $hdr
        printf "%s: ", $hdr->name
        # PC_COUNT
        if $hdr->type == 0
                set $count = (struct perf_ctr_count *)$hdr
                printf "%llu events\n", $count->event_count
        end
        # PC_ELPASED
        if $hdr->type == 1
                set $elapsed = (struct perf_ctr_elapsed *)$hdr
                printf "%llu events, %lluus elapsed, min %lluus, max %lluus\n", $elapsed->event_count, $elapsed->time_total, $elapsed->time_least, $elapsed->time_most
        end
        # PC_INTERVAL
        if $hdr->type == 2
                set $interval = (struct perf_ctr_interval *)$hdr
                printf "%llu events, %llu avg, min %lluus max %lluus\n", $interval->event_count, ($interval->time_last - $interval->time_first) / $interval->event_count, $interval->time_least, $interval->time_most
        end
end

define perf
        set $ctr = (sq_entry_t *)(perf_counters.head)
        while $ctr != 0
                _perf_print $ctr
                set $ctr = $ctr->flink
        end
end

document perf
.    perf
.        Prints performance counters.
end

env: # copy the file to .gdbinit in your Firmware tree, and adjust the path
# below to match your system
# For example:
# target extended /dev/serial/by-id/usb-Black_Sphere_Technologies_Black_Magic_Probe_DDE5A1C4-if00
# target extended /dev/ttyACM4


monitor swdp_scan
attach 1
monitor vector_catch disable hard
set mem inaccessible-by-default off
set print pretty
source Debug/PX4
  # Path to the CMake build directory.
  build: define f4_memdump
        shell mkdir -p /tmp/dump
        printf "Dumping CCSRAM to /tmp/dump/ccsram\n"
        dump memory /tmp/dump/ccsram 0x10000000 0x10010000
        printf "Dumping SRAM to /tmp/dump/sram\n"
        dump memory /tmp/dump/sram 0x20000000 0x20020000
end

document f4_memdump
Dumps the STM32F4 memory to files in /tmp/dump.
end

permissions: # program a bootable device load on a mavstation
# To run type openocd -f mavprogram.cfg

source [find interface/olimex-arm-usb-ocd-h.cfg]
source [find px4fmu-v1-board.cfg]

init
halt

# Find the flash inside this CPU
flash probe 0

# erase it (128 pages) then program and exit

#flash erase_sector 0 0 127
# stm32f1x mass_erase 0 

# It seems that Pat's image has a start address offset of 0x1000 but the vectors need to be at zero, so fixbin.sh moves things around
#flash write_bank 0 fixed.bin 0
#flash write_image firmware.elf
#shutdown

  contents: target remote :3333

# Don't let GDB get confused while stepping
define hook-step
  mon cortex_m maskisr on
end
define hookpost-step
  mon cortex_m maskisr off
end

mon init
mon stm32_init
# mon reset halt
mon poll
mon cortex_m maskisr auto
set mem inaccessible-by-default off
set print pretty
source Debug/PX4

echo PX4 resumed, press ctrl-c to interrupt\n
continue

jobs: #!/bin/bash
#
# Poor man's sampling profiler for NuttX.
#
# Usage: Install flamegraph.pl in your PATH, configure your .gdbinit, run the script with proper arguments and go
#        have a coffee. When you're back, you'll see the flamegraph. Note that frequent calls to GDB significantly
#        interfere with normal operation of the target, which means that you can't profile real-time tasks with it.
#
# Requirements: ARM GDB with Python support
#

set -e
root=$(dirname $0)/..

function die()
{
    echo "$@"
    exit 1
}

function usage()
{
    echo "Invalid usage. Supported options:"
    cat $0 | sed -n 's/^\s*--\([^)\*]*\).*/\1/p' # Don't try this at home.
    exit 1
}

which flamegraph.pl > /dev/null || die "Install flamegraph.pl first"

#
# Parsing the arguments. Read this section for usage info.
#
nsamples=0
sleeptime=0.1    # Doctors recommend 7-8 hours a day
taskname=
elf=$root/Build/px4fmu-v2_default.build/firmware.elf
append=0
fgfontsize=10
fgwidth=1900

for i in "$@"
do
    case $i in
        --nsamples=*)
            nsamples="${i#*=}"
            ;;
        --sleeptime=*)
            sleeptime="${i#*=}"
            ;;
        --taskname=*)
            taskname="${i#*=}"
            ;;
        --elf=*)
            elf="${i#*=}"
            ;;
        --append)
            append=1
            ;;
        --fgfontsize=*)
            fgfontsize="${i#*=}"
            ;;
        --fgwidth=*)
            fgwidth="${i#*=}"
            ;;
        *)
            usage
            ;;
    esac
    shift
done

#
# Temporary files
#
stacksfile=/tmp/pmpn-stacks.log
foldfile=/tmp/pmpn-folded.txt
graphfile=/tmp/pmpn-flamegraph.svg
gdberrfile=/tmp/pmpn-gdberr.log

#
# Sampling if requested. Note that if $append is true, the stack file will not be rewritten.
#
cd $root

if [[ $nsamples > 0 ]]
then
    [[ $append = 0 ]] && (rm -f $stacksfile; echo "Old stacks removed")

    echo "Sampling the task '$taskname'..."

    for x in $(seq 1 $nsamples)
    do
        if [[ "$taskname" = "" ]]
        then
            arm-none-eabi-gdb $elf --batch -ex "set print asm-demangle on" -ex bt \
                2> $gdberrfile \
                | sed -n 's/\(#.*\)/\1/p' \
                >> $stacksfile
        else
            arm-none-eabi-gdb $elf --batch -ex "set print asm-demangle on" \
                                           -ex "source $root/Debug/Nuttx.py" \
                                           -ex "show mybt $taskname" \
                2> $gdberrfile \
                | sed -n 's/0\.0:\(#.*\)/\1/p' \
                >> $stacksfile
        fi
        echo -e '\n\n' >> $stacksfile
        echo -ne "\r$x/$nsamples"
        sleep $sleeptime
    done

    echo
    echo "Stacks saved to $stacksfile"
else
    echo "Sampling skipped - set 'nsamples' to re-sample."
fi

#
# Folding the stacks.
#
[ -f $stacksfile ] || die "Where are the stack samples?"

cat << 'EOF' > /tmp/pmpn-folder.py
#
# This stack folder correctly handles C++ types.
#
from __future__ import print_function, division
import fileinput, collections, os, sys

def enforce(x, msg='Invalid input'):
    if not x:
        raise Exception(msg)

def split_first_part_with_parens(line):
    LBRACES = {'(':'()', '<':'<>', '[':'[]', '{':'{}'}
    RBRACES = {')':'()', '>':'<>', ']':'[]', '}':'{}'}
    QUOTES = set(['"', "'"])
    quotes = collections.defaultdict(bool)
    braces = collections.defaultdict(int)
    out = ''
    for ch in line:
        out += ch
        # escape character cancels further processing
        if ch == '\\':
            continue
        # special cases
        if out.endswith('operator>') or out.endswith('operator>>') or out.endswith('operator->'):  # gotta love c++
            braces['<>'] += 1
        if out.endswith('operator<') or out.endswith('operator<<'):
            braces['<>'] -= 1
        # switching quotes
        if ch in QUOTES:
            quotes[ch] = not quotes[ch]
        # counting parens only when outside quotes
        if sum(quotes.values()) == 0:
            if ch in LBRACES.keys():
                braces[LBRACES[ch]] += 1
            if ch in RBRACES.keys():
                braces[RBRACES[ch]] -= 1
        # sanity check
        for v in braces.values():
            enforce(v >= 0, 'Unaligned braces: ' + str(dict(braces)))
        # termination condition
        if ch == ' ' and sum(braces.values()) == 0:
            break
    out = out.strip()
    return out, line[len(out):]

def parse(line):
    def take_path(line, output):
        line = line.strip()
        if line.startswith('at '):
            line = line[3:].strip()
        if line:
            output['file_full_path'] = line.rsplit(':', 1)[0].strip()
            output['file_base_name'] = os.path.basename(output['file_full_path'])
            output['line'] = int(line.rsplit(':', 1)[1])
        return output

    def take_args(line, output):
        line = line.lstrip()
        if line[0] == '(':
            output['args'], line = split_first_part_with_parens(line)
        return take_path(line.lstrip(), output)

    def take_function(line, output):
        output['function'], line = split_first_part_with_parens(line.lstrip())
        return take_args(line.lstrip(), output)

    def take_mem_loc(line, output):
        line = line.lstrip()
        if line.startswith('0x'):
            end = line.find(' ')
            num = line[:end]
            output['memloc'] = int(num, 16)
            line = line[end:].lstrip()
            end = line.find(' ')
            enforce(line[:end] == 'in')
            line = line[end:].lstrip()
        return take_function(line, output)

    def take_frame_num(line, output):
        line = line.lstrip()
        enforce(line[0] == '#')
        end = line.find(' ')
        num = line[1:end]
        output['frame_num'] = int(num)
        return take_mem_loc(line[end:], output)

    return take_frame_num(line, {})

stacks = collections.defaultdict(int)
current = ''

stack_tops = collections.defaultdict(int)
num_stack_frames = 0

for idx,line in enumerate(fileinput.input()):
    try:
        line = line.strip()
        if line:
            inf = parse(line)
            fun = inf['function']
            current = (fun + ';' + current) if current else fun

            if inf['frame_num'] == 0:
                num_stack_frames += 1
                stack_tops[fun] += 1
        elif current:
            stacks[current] += 1
            current = ''
    except Exception, ex:
        print('ERROR (line %d):' % (idx + 1), ex, file=sys.stderr)

for s, f in sorted(stacks.items(), key=lambda (s, f): s):
    print(s, f)

print('Total stack frames:', num_stack_frames, file=sys.stderr)
print('Top consumers (distribution of the stack tops):', file=sys.stderr)
for name,num in sorted(stack_tops.items(), key=lambda (name, num): num, reverse=True)[:10]:
    print('% 5.1f%%   ' % (100 * num / num_stack_frames), name, file=sys.stderr)
EOF

cat $stacksfile | python /tmp/pmpn-folder.py > $foldfile

echo "Folded stacks saved to $foldfile"

#
# Graphing.
#
cat $foldfile | flamegraph.pl --fontsize=$fgfontsize --width=$fgwidth > $graphfile
echo "FlameGraph saved to $graphfile"

# On KDE, xdg-open prefers Gwenview by default, which doesn't handle interactive SVGs, so we need a browser.
# The current implementation is hackish and stupid. Somebody, please do something about it.
opener=xdg-open
which firefox       > /dev/null && opener=firefox
which google-chrome > /dev/null && opener=google-chrome

$opener $graphfile
  analyze: # The latest defaults in OpenOCD 0.7.0 are actually prettymuch correct for the px4fmu

# increase working area to 32KB for faster flash programming
set WORKAREASIZE 0x8000

source [find target/stm32f4x.cfg]

# needed for px4
reset_config trst_only

proc stm32_reset {} {
  reset halt
# FIXME - needed to init periphs on reset
# 0x40023800 RCC base
# 0x24 RCC_APB2 0x75933
# RCC_APB2 0
}

# perform init that is required on each connection to the target
proc stm32_init {} {

  # force jtag to not shutdown during sleep
  #uint32_t cr = getreg32(STM32_DBGMCU_CR);
  #cr |= DBGMCU_CR_STANDBY | DBGMCU_CR_STOP | DBGMCU_CR_SLEEP;
  #putreg32(cr, STM32_DBGMCU_CR);
  mww 0xe0042004 00000007
}

# if srst is not fitted use SYSRESETREQ to
# perform a soft reset
cortex_m reset_config sysresetreq

# Let GDB directly program elf binaries
gdb_memory_map enable

# doesn't work yet
gdb_flash_program disable

    permissions: #!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

openocd -f interface/olimex-arm-usb-ocd-h.cfg -f $DIR/px4fmu-v1-board.cfg
      contents: cp vendor/twbs/bootstrap/dist/css/bootstrap.min.css ../css/bootstrap.min.css;
cp vendor/twbs/bootstrap/dist/js/bootstrap.min.js ../js/bootstrap.min.js;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.eot ../fonts/glyphicons-halflings-regular.eot;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.svg ../fonts/glyphicons-halflings-regular.svg;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.ttf ../fonts/glyphicons-halflings-regular.ttf;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff ../fonts/glyphicons-halflings-regular.woff;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff2 ../fonts/glyphicons-halflings-regular.woff2;

cp vendor/select2/select2/select2.js ../js/select2.js;
cp vendor/select2/select2/select2_locale_ru.js ../js/select2_locale_ru.js;
cp vendor/select2/select2/select2.css ../css/select2.css;
cp vendor/select2/select2/select2.png ../css/select2.png;
cp vendor/select2/select2/select2-spinner.gif ../css/select2-spinner.gif;

cp vendor/silverfire/select2-bootstrap3-css/select2-bootstrap.min.css ../css/select2-bootstrap.min.css;
      security-events: <?php

class FakerCommand extends CConsoleCommand
{
    const PER_INSERT = 100; // how many rows should be inserted in one query
    const PER_TRANSACTION = 100; // how many queries should contain one transaction
    const PASSWORD = 'demo';
    const PUBLISHED = 'published';

    private $fakerData = array();
    private $builder;
    private $password;

    public function init()
    {
        $this->fakerData = new FakerData(self::PER_INSERT);
        $this->builder = Yii::app()->db->getSchema()->getCommandBuilder();
        $this->password = CPasswordHelper::hashPassword(self::PASSWORD);
    }

    public function actionUser($cnt = 1000)
    {
        $table = 'user'; 
        $insertCount = intval(floor($cnt/self::PER_INSERT));
        $txn = Yii::app()->db->beginTransaction();
        for ($i=0; $i < $insertCount; $i++) {
            $this->progressMsg($i * self::PER_INSERT, $cnt);
            $data = $this->collectUserData();
            $this->multipleInsert($table, $data);
            if ($i % self::PER_TRANSACTION == 0 and $i != 0) {
                $txn->commit();
                $txn = Yii::app()->db->beginTransaction();
            }
        }
        $remainder = $cnt % self::PER_INSERT;
        if ($remainder) {
            $data = $this->collectUserData($remainder);
            $this->multipleInsert($table, $data);
        }
        $txn->commit();
    }

    public function actionAd($cnt=1000, $eav=false, $photos=false)
    {
        echo date('H:i:s') . "\n";
        $insertCount = intval(floor($cnt/self::PER_INSERT));
        $txn = Yii::app()->db->beginTransaction();
        for ($i=0; $i < $insertCount; $i++) {
            $this->progressMsg($i * self::PER_INSERT, $cnt);
            $this->insertAd($eav, $photos);
            if ($i % self::PER_TRANSACTION == 0 and $i != 0) {
                $txn->commit();
                $txn = Yii::app()->db->beginTransaction();
            }
        }
        $remainder = $cnt % self::PER_INSERT;
        if ($remainder) {
            $this->insertAd($eav, $photos, $remainder);
        }
        $txn->commit();
        echo date('H:i:s') . "\n";
    }

    private function multipleInsert($table, $data)
    {
        $command = $this->builder->createMultipleInsertCommand($table, $data);
        $command->execute();
    }

    private function eavMultipleInsert($table, $data)
    {
        $sql = "INSERT INTO {$table} (eav_attribute_id, entity_id, entity, value) VALUES ";
        $i = 0;
        foreach ($data as $row) {
            if ($i == 0) {
                $sql .= "(:a{$i},:e{$i},'ad',:v{$i})";
            } else {
                $sql .= ",(:a{$i},:e{$i},'ad',:v{$i})";
            }
            $params[":e{$i}"] = $row['entity_id'];
            $params[":a{$i}"] = $row['eav_attribute_id'];
            $params[":v{$i}"] = $row['value'];
            $i++;
        }
        Yii::app()->db->createCommand($sql)->execute($params);
    }

    private function insertAd($eav, $photos, $cnt = self::PER_INSERT)
    {
        $table = 'ad';
        $data = $this->collectAdData($cnt);
        $this->multipleInsert($table, $data);
        $this->attachEav($eav, $cnt);
        $this->attachPhoto($photos, $cnt);
    }

    private function collectUserData($cnt = self::PER_INSERT)
    {
        $data = array();
        for ($i=0; $i < $cnt; $i++) {
            $data[] = array(
                'email' => $this->fakerData->getEmail(),
                // use 'demo' to login
                'password' => $this->password,
                'name' => $this->fakerData->getUserName(),
                'phone' => $this->fakerData->getPhoneNumber(),
            );
        }
        return $data;
    }

    private function collectAdData($cnt = self::PER_INSERT)
    {
        $data = array();
        for ($i=0; $i < $cnt; $i++) {
            $category = $this->fakerData->getCategory();
            $data[] = array(
                'title' => $this->fakerData->getTitle(),
                'description' => $this->fakerData->getDescription(),
                'author_id' => $this->fakerData->getAuthor(),
                'city_id' => $this->fakerData->getCity(),
                'category_id' => $category['id'],
                'eav_set_id' => $category['set_id'],
                'status' => self::PUBLISHED,
            );
        }
        return $data;
    }

    private function attachEav($eav, $cnt = self::PER_INSERT)
    {
        if (!$eav) return;
        $data = $this->collectEavData($cnt);
        if (!empty($data['int'])) {
            $this->eavMultipleInsert('eav_attribute_int', $data['int']);
        }
        if (!empty($data['varchar'])) {
            $this->eavMultipleInsert('eav_attribute_varchar', $data['varchar']);
        }
    }

    private function collectEavData($cnt)
    {
        $ads = $this->getAdsCommand($cnt)->queryAll();
        foreach ($ads as $ad) {
            $set = $this->fakerData->getSet($ad['eav_set_id']);
            foreach ($set as $attr) {
                if ($attr['data_type'] == 'IntDataType') {
                    $value = $this->getEavIntValue($attr);
                    $data['int'][] = array(
                        'eav_attribute_id' => $attr['eav_attribute_id'],
                        'entity_id' => $ad['id'],
                        'entity' => 'ad',
                        'value' => $value,
                    );
                } elseif ($attr['data_type'] == 'VarcharDataType') {
                    $value = $this->getEavVarcharValue($attr);
                    $data['varchar'][] = array(
                        'eav_attribute_id' => $attr['eav_attribute_id'],
                        'entity_id' => $ad['id'],
                        'entity' => 'ad',
                        'value' => $value,
                    );
                }
            }
        }
        return $data;
    }

    private function attachPhoto($photos, $cnt = self::PER_INSERT)
    {
        if (!$photos) return;
        $data = $this->collectPhotoData($cnt);
        $this->multipleInsert('photo', $data);
    }

    private function collectPhotoData($cnt)
    {
        $data = array();
        $ads = $this->getAdsCommand($cnt)->queryAll();
        foreach ($ads as $ad) {
            $photoCount = rand(0, 5);
            for ($i=0; $i < $photoCount; $i++) {
                $data[] = array(
                    'name' => $this->fakerData->getPhotoName(),
                    'ad_id' => $ad['id'],
                );
            }
        }
        return $data;
    }

    private function getEavIntValue($attr)
    {
        $rawData = unserialize($attr['data']);
        $min = (isset($rawData['rules']['numerical']['min']))
                ? intval($rawData['rules']['numerical']['min'])
                : 0;
        $max = (isset($rawData['rules']['numerical']['max']))
                ? intval($rawData['rules']['numerical']['max'])
                : 100000000;
        if ($attr['name'] == 'price') {
            $value = rand($min, intval($max/100));
        } elseif ($attr['name'] == 'modelYear') {
            $max = intval(date('Y'));
            $value = rand($min, $max);
        } else {
            $value = rand($min, $max);
        }
        return $value;
    }

    private function getEavVarcharValue($attr)
    {
        $rawData = unserialize($attr['data']);
        return $rawData['values'][array_rand($rawData['values'])];
    }

    private function getAdsCommand($cnt)
    {
        $sql = "SELECT id, eav_set_id FROM ad ORDER BY id DESC LIMIT $cnt";
        return Yii::app()->db->createCommand($sql);
    }

    private function progressMsg($cur, $cnt)
    {
        echo round($cur * 100 / $cnt, 2) . "%\n";
    }
}
      actions: <?php
/**
 * Controller is the customized base controller class.
 * All controller classes for this application should extend from this base class.
 */
class Controller extends CController
{
        /**
         * @var string the default layout for the controller view. Defaults to '//layouts/column1',
         * meaning using a single column layout. See 'protected/views/layouts/column1.php'.
         */
        public $layout=false;
        /**
         * @var array context menu items. This property will be assigned to {@link CMenu::items}.
         */
        public $menu=array();
        /**
         * @var array the breadcrumbs of the current page. The value of this property will
         * be assigned to {@link CBreadcrumbs::links}. Please refer to {@link CBreadcrumbs::links}
         * for more details on how to specify this property.
         */
        public $breadcrumbs=array();

        /**
         * Search via sphinx (mysql client)
         */
        protected function sphinxSearch($phrase)
        {
                $connection = new CDbConnection(
                        Yii::app()->params['sphinx']['dsn'],
                        Yii::app()->params['sphinx']['user'],
                        Yii::app()->params['sphinx']['pass']
                );
                $connection->active=true;
                $words = mb_split('[^\w]+', $phrase);
                $words = array_filter($words); // unset empty elements
                $search = implode('|', $words);
                $sphinxIndexes = SphinxService::implodeIndexes();
                $sql = "SELECT * FROM $sphinxIndexes WHERE MATCH('$search') LIMIT 10000";
                $command = $connection->createCommand($sql);
                return $command->queryColumn();
        }
}
    name: <?php

class EavActiveDataProvider extends CActiveDataProvider
{
    private $_pagination=null;

    protected function fetchData()
    {
        $criteria=clone $this->getCriteria();

        if(($pagination=$this->getPagination())!==false)
        {
            $pagination->setItemCount($this->getTotalItemCount());
            $pagination->applyLimit($criteria);
        }

        $baseCriteria=$this->model->getDbCriteria(false);

        if(($sort=$this->getSort())!==false)
        {
            // set model criteria so that CSort can use its table alias setting
            if($baseCriteria!==null)
            {
                $c=clone $baseCriteria;
                $c->mergeWith($criteria);
                $this->model->setDbCriteria($c);
            }
            else
                $this->model->setDbCriteria($criteria);
            $sort->applyOrder($criteria);
        }

        $this->model->setDbCriteria($baseCriteria!==null ? clone $baseCriteria : null);
        $data=$this->model->withEavAttributes()->findAll($criteria);
        $this->model->setDbCriteria($baseCriteria);  // restore original criteria
        return $data;
    }

    public function getPagination($className='CPagination')
    {
        if($this->_pagination===null)
        {
            $this->_pagination=new $className;
            if(($id=$this->getId())!='')
                $this->_pagination->pageVar='page';
        }
        return $this->_pagination;
    }
}
    runs-on: <?php

class FakerData
{
    private $faker;

    private $userNames;
    private $userPhones;
    private $userEmails;

    private $adTitles;
    private $adDescriptions;
    private $adAuthors;
    private $adCities;
    private $adCategories;
    private $adSets;

    private $photoNames;

    public function __construct($cnt)
    {
        $this->faker = Faker\Factory::create('ru_RU');

        for ($i=0; $i < $cnt; $i++) {
            $this->userNames[] = $this->faker->name;
            $this->userPhones[] = $this->faker->phoneNumber;
            $this->userEmails[] = $this->faker->safeEmail;
            $this->adTitles[] = $this->faker->sentence;
            $this->adDescriptions[] = $this->faker->paragraph;
            $this->photoNames[] = $this->faker->word;
        }

        $this->setAuthors();
        $this->setCities();
        $this->setCategories();
        $this->setAdSets();
    }

    public function getSet($id)
    {
        return $this->adSets[$id];
    }

    public function getTitle()
    {
        return $this->adTitles[array_rand($this->adTitles)];
    }

    public function getDescription()
    {
        return $this->adDescriptions[array_rand($this->adDescriptions)];
    }

    public function getAuthor()
    {
        return $this->adAuthors[array_rand($this->adAuthors)];
    }

    public function getCity()
    {
        return $this->adCities[array_rand($this->adCities)];
    }

    public function getCategory()
    {
        return $this->adCategories[array_rand($this->adCategories)];
    }

    public function getPhotoName()
    {
        return $this->photoNames[array_rand($this->photoNames)];
    }

    public function getUserName()
    {
        return $this->userNames[array_rand($this->userNames)];
    }

    public function getPhoneNumber()
    {
        return $this->userPhones[array_rand($this->userPhones)];
    }

    public function getEmail()
    {
        return 'u' . microtime(true) . rand(1000, 9999) . $this->userEmails[array_rand($this->userEmails)];
    }

    private function setAuthors()
    {
        $sql = "SELECT id FROM user LIMIT 100";
        $command = Yii::app()->db->createCommand($sql);
        $this->adAuthors = $command->queryColumn();
    }

    private function setCities()
    {
        $sql = "SELECT city_id FROM city WHERE country_id = 3159";
        $command = Yii::app()->db->createCommand($sql);
        $this->adCities = $command->queryColumn();
    }

    private function setCategories()
    {
        $sql = "SELECT id, set_id FROM category WHERE lft = rgt - 1";
        $command = Yii::app()->db->createCommand($sql);
        $this->adCategories = $command->queryAll();
    }

    private function setAdSets()
    {
        $sql = "SELECT id FROM eav_set";
        $command = Yii::app()->db->createCommand($sql);
        $setsIds = $command->queryColumn();
        $sql = "SELECT sa.eav_attribute_id, at.name, at.data_type, at.data
                FROM eav_attribute_set sa
                JOIN eav_attribute at ON sa.eav_attribute_id=at.id
                WHERE sa.eav_set_id = :set_id";
        $command = Yii::app()->db->createCommand($sql);
        foreach ($setsIds as $set_id) {
            $this->adSets[$set_id] = $command->queryAll(true, array(':set_id'=>$set_id));
        }
    }
}

    steps: <?php

class SphinxService
{
    public static function saveAdToRt($adID)
    {
        $ad = Ad::model()->findByPk($adID);
        $connection = new CDbConnection(
            Yii::app()->params['sphinx']['dsn'],
            Yii::app()->params['sphinx']['user'],
            Yii::app()->params['sphinx']['pass']
        );
        $connection->active=true;

        $sphinxIndexes = Yii::app()->params['sphinx']['indexes'];
        $rt = $sphinxIndexes['rt'][0];
        $sql = "INSERT INTO $rt (id, title, description, added)
                VALUES (:id, :title, :description, :added)";
        $command = $connection->createCommand($sql);
        $command->execute(
            array(
                ':id' => $ad->id,
                ':title' => $ad->title,
                ':description' => $ad->description,
                ':added' => time(),
        ));
    }

    public static function implodeIndexes()
    {
        $sphinxIndexes = Yii::app()->params['sphinx']['indexes'];
        return implode(',', $sphinxIndexes['rt']) . ','
        . implode(',', $sphinxIndexes['disc']);
    }
}
      - name: <?php

/**
 * UserIdentity represents the data needed to identity a user.
 * It contains the authentication method that checks if the provided
 * data can identity the user.
 */
class UserIdentity extends CUserIdentity
{
        /**
         * Authenticates a user.
         * The example implementation makes sure if the username and password
         * are both 'demo'.
         * In practical applications, this should be changed to authenticate
         * against some persistent user identity storage (e.g. database).
         * @return boolean whether authentication succeeds.
         */

        private $_id;

        public function authenticate()
        {
                $record = User::model()->findByAttributes(array('email'=>$this->username));
        if($record===null)
            $this->errorCode=self::ERROR_USERNAME_INVALID;
        else if(!CPasswordHelper::verifyPassword($this->password, $record->password))
            $this->errorCode=self::ERROR_PASSWORD_INVALID;
        else
        {
            $this->_id=$record->id;
            $this->setState('id', $record->id);
            $this->setState('name', $record->name);
            $this->errorCode=self::ERROR_NONE;
        }
        return !$this->errorCode;
        }
}
        uses: <?php

// This is the configuration for yiic console application.
// Any writable CConsoleApplication properties can be configured here.
return array(
        'basePath'=>dirname(__FILE__).DIRECTORY_SEPARATOR.'..',
        'name'=>'My Console Application',

        // preloading 'log' component
        'preload'=>array('log'),

        'import'=>array(
                'application.models.*',
                'application.components.*',
                'application.vendor.iachilles.eavactiverecord.*',
                'application.vendor.iachilles.eavactiverecord.datatypes.*',
                'application.vendor.iachilles.eavactiverecord.helpers.*',
        ),

        // application components
        'components'=>array(

                // database settings are configured in database.php
                'db'=>require(dirname(__FILE__).'/database.php'),

                'log'=>array(
                        'class'=>'CLogRouter',
                        'routes'=>array(
                                array(
                                        'class'=>'CFileLogRoute',
                                        'levels'=>'error, warning',
                                ),
                        ),
                ),

        ),
);

      - name: <?php

// This is the database connection configuration.
return array(

        'connectionString' => 'mysql:host=localhost;dbname=classifieds',
        'emulatePrepare' => false,
        'username' => 'root',
        'password' => '',
        'charset' => 'utf8',
    'tablePrefix' => '',
    'schemaCachingDuration' => 3600,

);
        run: <?php

// uncomment the following to define a path alias
// Yii::setPathOfAlias('local','path/to/local-folder');

// This is the main Web application configuration. Any writable
// CWebApplication properties can be configured here.
return array(
    'basePath'=>dirname(dirname(__FILE__)),
        'name'=>'Доска объявлений',
        'language'=>'ru',

        // preloading 'log' component
        'preload'=>array('log'),

        // autoloading model and component classes
        'import'=>array(
                'application.models.*',
                'application.components.*',
                'application.vendor.iachilles.eavactiverecord.*',
                'application.vendor.iachilles.eavactiverecord.datatypes.*',
                'application.vendor.iachilles.eavactiverecord.helpers.*',
        ),

        'modules'=>array(
                // uncomment the following to enable the Gii tool
                /*
                'gii'=>array(
                        'class'=>'system.gii.GiiModule',
                        'password'=>'Enter Your Password Here',
                        // If removed, Gii defaults to localhost only. Edit carefully to taste.
                        'ipFilters'=>array('127.0.0.1','::1'),
                ),
                */
        ),

        // application components
        'components'=>array(

                'user'=>array(
                        // enable cookie-based authentication
                        'allowAutoLogin'=>true,
                ),

                'viewRenderer' => array(
                        'class' => 'application.vendor.yiiext.twig-renderer.ETwigViewRenderer',
                        'twigPathAlias' => 'application.vendor.twig.twig.lib.Twig',
                        'fileExtension' => '.twig',
                ),

                'eavCache' => array(
                        'class' => 'system.caching.CRedisCache'
                ),

                'cache' => array(
                        'class' => 'system.caching.CRedisCache',
                        'hostname' => '127.0.0.1',
                        'port' => 6379,
                        'database' => 0,
                ),

                // uncomment the following to enable URLs in path-format

                'urlManager'=>array(
                        'urlFormat'=>'path',
                        'showScriptName'=>false,
                        'rules'=>array(
                                '<controller:\w+>/<id:\d+>'=>'<controller>/view',
                                '<controller:\w+>/<action:\w+>/<id:\d+>'=>'<controller>/<action>',
                                '<controller:\w+>/<action:\w+>'=>'<controller>/<action>',
                        ),
                ),


                // database settings are configured in database.php
                'db'=>require(dirname(__FILE__).'/database.php'),

                'authManager'=>array(
            'class'=>'CDbAuthManager',
            'connectionID'=>'db',
        ),

                'errorHandler'=>array(
                        // use 'site/error' action to display errors
                        'errorAction'=>YII_DEBUG ? null : 'site/error',
                ),

                'log'=>array(
                        'class'=>'CLogRouter',
                        'routes'=>array(
                                array(
                                        'class'=>'CFileLogRoute',
                                        'levels'=>'error, warning',
                                ),
                                // uncomment the following to show log messages on web pages

                                /*array(
                                        'class'=>'CWebLogRoute',
                                ),*/

                        ),
                ),

        ),

        // application-level parameters that can be accessed
        // using Yii::app()->params['paramName']
        'params'=>array(
                // this is used in contact page
                'adminEmail'=>'webmaster@example.com',
                // configure to match your sphinx configuration,
                // or comment to disable sphinxsearch
                'sphinx'=>array(
                        'dsn'=>'mysql:host=127.0.0.1;port=9306',
                        'user'=>'root',
                        'pass'=>'',
                        'indexes'=>array('rt'=>['rt_ad'], 'disc'=>['ix_ad']),
                ),
        ),
);

      # Build is not required unless generated source files are used
      # - name: <?php

return CMap::mergeArray(
        require(dirname(__FILE__).'/main.php'),
        array(
                'components'=>array(
                        'fixture'=>array(
                                'class'=>'system.test.CDbFixtureManager',
                        ),
                        /* uncomment the following to provide test database connection
                        'db'=>array(
                                'connectionString'=>'DSN for test database',
                        ),
                        */
                ),
        )
);
      #   run: <?php

class AdController extends Controller
{
        /**
         * @return array action filters
         */
        public function filters()
        {
                return array(
                        'accessControl', // perform access control for CRUD operations
                        'postOnly + delete', // we only allow deletion via POST request
                );
        }

        /**
         * Specifies the access control rules.
         * This method is used by the 'accessControl' filter.
         * @return array access control rules
         */
        public function accessRules()
        {
                return array(
                        array('allow',  // allow all users to perform 'index' and 'view' actions
                                'actions'=>array('index','view'),
                                'users'=>array('*'),
                        ),
                        array('allow', // allow authenticated user to perform 'create' and 'update' actions
                                'actions'=>array('update','new','getcategories','create'),
                                'users'=>array('@'),
                        ),
                        array('allow', // allow admin user to perform 'admin' and 'delete' actions
                                'actions'=>array('admin','delete'),
                                'users'=>array('admin'),
                        ),
                        array('deny',  // deny all users
                                'users'=>array('*'),
                        ),
                );
        }

        /**
         * Displays a particular model.
         * @param integer $id the ID of the model to be displayed
         */
        public function actionView($id)
        {
                $this->render('view',array(
                        'model'=>$this->loadModel($id),
                ));
        }

        /**
         * Displays the page with category selection. When user choose category, redirect
         * to the 'create' page.
         */
        public function actionNew()
        {
                $criteria = new CDbCriteria;
                $criteria->condition = 'level=:level';
                $criteria->params = array('level'=>1);
                $models = Category::model()->findAll($criteria);
                $this->render('new', array('models'=>$models));
        }

        /*
         * Responds to ajax request from ad/new page
         */
        public function actionGetcategories()
        {
                if (!isset($_POST['id'])) {
                        echo json_encode(array());
                        Yii::app()->end();
                }
                $id = intval($_POST['id']);
                $parent_cat = Category::model()->findByPk($id);
                $children = $parent_cat->children()->findAll();
                if (!$children) {
                        echo json_encode(array());
                        Yii::app()->end();
                }
                foreach ($children as $child) {
                        $res[$child->id] = $child->title;
                }
                echo json_encode($res);
        }

        /**
         * Creates a new model.
         * If creation is successful, the browser will be redirected to the 'view' page.
         */
        public function actionCreate($id)
        {
                $regions = Region::getRegionList();
                $model = new Ad;
                $model->attachEavSet(Category::model()->findByPk($id)->set_id);
                $model->category_id = $id;

                $photo = new Photo;
                if (isset($_POST['Ad'])) {
                        $model->attributes = $_POST['Ad'];
                        $model->author_id = Yii::app()->user->id;
                        $transaction = Yii::app()->db->beginTransaction();
                        if ($model->saveWithEavAttributes()) {
                                $images = CUploadedFile::getInstancesByName('images');
                                if ($images) {
                                        $wrongImage = Photo::validateMultiple($images, $model->id);
                                        if (!$wrongImage) {
                                                foreach ($images as $image) {
                                                        $photo = new Photo;
                                                        $photo->image = $image;
                                                        $photo->name = $photo->image->getName();
                                                        $photo->ad_id = $model->id;
                                                        $photo->save(false);
                                                }
                                                $transaction->commit();
                                                $this->redirect(array('view','id'=>$model->id));
                                        } else {
                                                $photo = $wrongImage;
                                                $transaction->rollback();
                                        }
                                } else {
                                        $transaction->commit();
                                        SphinxService::saveAdToRt($model->id);
                                        $this->redirect(array('view','id'=>$model->id));
                                }
                        }
                }

                $this->render('create', array(
                        'model'=>$model,
                        'photo'=>$photo,
                        'regions'=>$regions,
                ));
        }



        /**
         * Updates a particular model.
         * If update is successful, the browser will be redirected to the 'view' page.
         * @param integer $id the ID of the model to be updated
         */
        /*public function actionUpdate($id)
        {
                $model=$this->loadModel($id);

                // Uncomment the following line if AJAX validation is needed
                // $this->performAjaxValidation($model);

                if(isset($_POST['Ad']))
                {
                        $model->attributes=$_POST['Ad'];
                        if($model->save())
                                $this->redirect(array('view','id'=>$model->id));
                }

                $this->render('update',array(
                        'model'=>$model,
                ));
        }*/

        /**
         * Deletes a particular model.
         * If deletion is successful, the browser will be redirected to the 'admin' page.
         * @param integer $id the ID of the model to be deleted
         */
        /*public function actionDelete($id)
        {
                $this->loadModel($id)->delete();

                // if AJAX request (triggered by deletion via admin grid view), we should not redirect the browser
                if(!isset($_GET['ajax']))
                        $this->redirect(isset($_POST['returnUrl']) ? $_POST['returnUrl'] : array('admin'));
        }*/

        /**
         * Lists all models.
         */
        /*public function actionIndex()
        {
                $dataProvider=new CActiveDataProvider('Ad');
                $this->render('index',array(
                        'dataProvider'=>$dataProvider,
                ));
        }*/

        /**
         * Manages all models.
         */
        /*public function actionAdmin()
        {
                $model=new Ad('search');
                $model->unsetAttributes();  // clear any default values
                if(isset($_GET['Ad']))
                        $model->attributes=$_GET['Ad'];

                $this->render('admin',array(
                        'model'=>$model,
                ));
        }*/

        /**
         * Returns the data model based on the primary key given in the GET variable.
         * If the data model is not found, an HTTP exception will be raised.
         * @param integer $id the ID of the model to be loaded
         * @return Ad the loaded model
         * @throws CHttpException
         */
        public function loadModel($id)
        {
                $model = Ad::model()->withEavAttributes()->with(
                                'author', 'category', 'city', 'photos'
                        )->findByPk($id);
                if($model===null)
                        throw new CHttpException(404,'The requested page does not exist.');
                return $model;
        }

        /**
         * Performs the AJAX validation.
         * @param Ad $model the model to be validated
         */
        protected function performAjaxValidation($model)
        {
                if(isset($_POST['ajax']) && $_POST['ajax']==='ad-form')
                {
                        echo CActiveForm::validate($model);
                        Yii::app()->end();
                }
        }
}

      - name: <?php

class SiteController extends Controller
{
    /**
     * Declares class-based actions.
     */
    public function actions()
    {
        return array(
            // captcha action renders the CAPTCHA image displayed on the contact page
            /*'captcha'=>array(
                'class'=>'CCaptchaAction',
                'backColor'=>0xFFFFFF,
            ),*/
            // page action renders "static" pages stored under 'protected/views/site/pages'
            // They can be accessed via: index.php?r=site/page&view=FileName
            /*'page'=>array(
                'class'=>'CViewAction',
            ),*/
        );
    }

    public function filters()
    {
        return array('accessControl');
    }

    public function accessRules()
    {
        return array(
            array('allow',
                'actions'=>array('index','view','search', 'error', 'contact',
                    'login', 'logout', 'cityData', 'getcities'),
                'users'=>array('*'),
            ),
            array('allow',
                'actions'=>array('logout'),
                'users'=>array('@'),
            ),
            array('deny',
                'actions'=>array('admin'),
                'users'=>array('*'),
            ),
            array('deny',  // deny all users
                'users'=>array('*'),
            ),
        );
    }

    /**
     * This is the default 'index' action that is invoked
     * when an action is not explicitly requested by users.
     */
    public function actionIndex()
    {
        $criteria = new CDbCriteria;
        $criteria->addInCondition('level', array(1,2));
        $criteria->order = 'root, lft';
        $categories = Category::model()->findAll($criteria);
        $form = new SearchForm;
        $criteria = new CDbCriteria(array(
            'condition' => 'status="published"',
            'order' => 'added DESC',
            'with' => array('photos'),
            'limit' => 20,
        ));
        $models = Ad::model()->withEavAttributes()->findAll($criteria);
        $dp = new CActiveDataProvider('Ad', array(
            'data' => $models,
            'pagination' => false
        ));
        $regionList = Region::model()->getRegionList();

        $this->render(
            'index',
            array(
                'categories' => $categories,
                'form' => $form,
                'dataProvider' => $dp,
                'regionList' => $regionList,
            )
        );
    }

    /**
     * Action to search ads by key words
     */
    public function actionSearch($id=null,$word=null,$city_id=null,$page=null)
    {
        $criteria = new CDbCriteria;
        $criteria->condition = "status='published'";
        $criteria->order = 'added DESC';

        $form = new EavSearchForm();
        if ($id) {
            $category = Category::model()->findByPk($id);
            $childrenIds = ($category) ? $category->getDescendantIds() : null;
            if ($childrenIds) {
                $criteria->addInCondition('category_id', $childrenIds);
            } else {
                $criteria->addCondition('category_id=:category_id');
                $criteria->params[':category_id'] = intval($id);
            }
            $form->model->attachEavSet($category->set_id);
            $form->eav = $form->model->getEavAttributes();
            if (isset($_GET['search'])) {
                $form->fill();
                $this->buildEavCriteria($criteria);
            }
        }
        if ($word) {
            try {
                $ids = $this->sphinxSearch($word);
                $criteria->addInCondition('t.id', $ids);
            } catch(Exception $e) {
                $criteria->addCondition('title LIKE :word1 OR description LIKE :word2');
                $criteria->params[':word1'] = "%{$word}%";
                $criteria->params[':word2'] = "%{$word}%";
            }
        }
        if ($city_id) {
            $criteria->addCondition('city_id=:city_id');
            $criteria->params[':city_id'] = intval($city_id);
        }
        $regions = Region::model()->getRegionList();

        $dp = new EavActiveDataProvider('Ad', array(
            'criteria'=>$criteria,
            'countCriteria'=>array(
                'condition'=>$criteria->condition,
                'params'=>$criteria->params),
            'pagination'=>array('pageSize'=>10),
            ));

        $this->render(
            'search',
            array(
                'dataProvider'=>$dp,
                'form'=>$form,
                'regions'=>$regions,
            )
        );
    }

    /**
     * This is the action to handle external exceptions.
     */
    public function actionError()
    {
        if($error=Yii::app()->errorHandler->error)
        {
            if(Yii::app()->request->isAjaxRequest)
                echo $error['message'];
            else
                $this->render('error', $error);
        }
    }

    /**
     * Displays the login page
     */
    public function actionLogin()
    {
        $model=new LoginForm;

        // if it is ajax validation request
        if(isset($_POST['ajax']) && $_POST['ajax']==='login-form')
        {
            echo CActiveForm::validate($model);
            Yii::app()->end();
        }

        // collect user input data
        if(isset($_POST['LoginForm']))
        {
            $model->attributes=$_POST['LoginForm'];
            // validate user input and redirect to the previous page if valid
            if($model->validate() && $model->login())
                $this->redirect(Yii::app()->user->returnUrl);
        }
        // display the login form
        $this->render('login',array('model'=>$model));
    }

    /**
     * Logs out the current user and redirect to homepage.
     */
    public function actionLogout()
    {
        Yii::app()->user->logout();
        $this->redirect(Yii::app()->homeUrl);
    }

    public function actionGetcities()
    {
        if (!isset($_POST['id']) or empty($_POST['id'])) {
            echo json_encode(false);
            Yii::app()->end();
        }
        $regionId = intval($_POST['id']);
        $cities = City::model()->findAllByAttributes(array('region_id'=>$regionId));
        foreach ($cities as $city) {
            $res[$city->city_id] = $city->name;
        }
        echo json_encode($res);
    }

    protected function buildEavCriteria(CDbCriteria $criteria, $getParam = 'search')
    {
        $attributes = Ad::getEavList();
        foreach ($_GET[$getParam] as $key=>$value) {
            if (!in_array($key, $attributes)) continue;
            if (is_array($value)) {
                if (isset($value['min']) and !empty($value['min'])) {
                    $criteria->addCondition("::{$key} >= :min_{$key}");
                    $criteria->params[":min_{$key}"] = $value['min'];
                }
                if (isset($value['max']) and !empty($value['max'])) {
                    $criteria->addCondition("::{$key} <= :max_{$key}");
                    $criteria->params[":max_{$key}"] = $value['max'];
                }
            } else {
                if (!$value) continue;
                $criteria->addCondition("::{$key} = :{$key}");
                $criteria->params[":{$key}"] = $value;
            }
        }
    }
}
        uses: <?php

class UserController extends Controller
{
        /**
         * @return array action filters
         */
        public function filters()
        {
                return array(
                        'accessControl', // perform access control for CRUD operations
                        'postOnly + delete', // we only allow deletion via POST request
                );
        }

        /**
         * Specifies the access control rules.
         * This method is used by the 'accessControl' filter.
         * @return array access control rules
         */
        public function accessRules()
        {
                return array(
                        array('allow',  // allow all users to perform 'index' and 'view' actions
                                'actions'=>array('register'),
                                'users'=>array('*'),
                        ),
                        array('allow', // allow authenticated user to perform 'create' and 'update' actions
                                'actions'=>array('update','view'),
                                'users'=>array('@'),
                        ),
                        array('allow', // allow admin user to perform 'admin' and 'delete' actions
                                'actions'=>array('admin','delete', 'index'),
                                'users'=>array('admin'),
                        ),
                        array('deny',  // deny all users
                                'users'=>array('*'),
                        ),
                );
        }

        /**
         * Displays a particular model.
         * @param integer $id the ID of the model to be displayed
         */
        public function actionView($id)
        {
                $this->render('view',array(
                        'model'=>$this->loadModel($id),
                ));
        }

        /**
         * Creates a new model.
         * If creation is successful, the browser will be redirected to the 'view' page.
         */
        public function actionRegister()
        {
                $model = new User;
                $this->performAjaxValidation($model);

                if(isset($_POST['User']))
                {
                        $model->attributes = $_POST['User'];
                        $model->password = CPasswordHelper::hashPassword($model->password);
                        if($model->save())
                                $this->redirect(array('site/login'));
                }

                $this->render('register',array(
                        'model'=>$model,
                ));
        }

        /**
         * Updates a particular model.
         * If update is successful, the browser will be redirected to the 'view' page.
         * @param integer $id the ID of the model to be updated
         */
        /*public function actionUpdate($id)
        {
                $model=$this->loadModel($id);

                // Uncomment the following line if AJAX validation is needed
                // $this->performAjaxValidation($model);

                if(isset($_POST['User']))
                {
                        $model->attributes=$_POST['User'];
                        if($model->save())
                                $this->redirect(array('view','id'=>$model->id));
                }

                $this->render('update',array(
                        'model'=>$model,
                ));
        }*/

        /**
         * Deletes a particular model.
         * If deletion is successful, the browser will be redirected to the 'admin' page.
         * @param integer $id the ID of the model to be deleted
         */
        /*public function actionDelete($id)
        {
                $this->loadModel($id)->delete();

                // if AJAX request (triggered by deletion via admin grid view), we should not redirect the browser
                if(!isset($_GET['ajax']))
                        $this->redirect(isset($_POST['returnUrl']) ? $_POST['returnUrl'] : array('admin'));
        }*/

        /**
         * Lists all models.
         */
        /*public function actionIndex()
        {
                $dataProvider=new CActiveDataProvider('User');
                $this->render('index',array(
                        'dataProvider'=>$dataProvider,
                ));
        }*/

        /**
         * Manages all models.
         */
        /*public function actionAdmin()
        {
                $model=new User('search');
                $model->unsetAttributes();  // clear any default values
                if(isset($_GET['User']))
                        $model->attributes=$_GET['User'];

                $this->render('admin',array(
                        'model'=>$model,
                ));
        }*/

        /**
         * Returns the data model based on the primary key given in the GET variable.
         * If the data model is not found, an HTTP exception will be raised.
         * @param integer $id the ID of the model to be loaded
         * @return User the loaded model
         * @throws CHttpException
         */
        public function loadModel($id)
        {
                $model=User::model()->findByPk($id);
                if($model===null)
                        throw new CHttpException(404,'The requested page does not exist.');
                return $model;
        }

        /**
         * Performs the AJAX validation.
         * @param User $model the model to be validated
         */
        protected function performAjaxValidation($model)
        {
                if(isset($_POST['ajax']) && $_POST['ajax']==='user-form')
                {
                        echo CActiveForm::validate($model);
                        Yii::app()->end();
                }
        }
}
        # Provide a unique ID to access the sarif output path
        id: -- MySQL dump 10.13  Distrib 5.5.44, for debian-linux-gnu (i686)
--
-- Host: localhost    Database: classifieds
-- ------------------------------------------------------
-- Server version        5.5.44-0ubuntu0.14.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `ad`
--

DROP TABLE IF EXISTS `ad`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ad` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(255) NOT NULL,
  `description` text NOT NULL,
  `added` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `author_id` int(10) unsigned NOT NULL,
  `city_id` int(10) unsigned NOT NULL,
  `category_id` int(10) unsigned NOT NULL,
  `visit_counter` int(10) unsigned NOT NULL DEFAULT '0',
  `status` enum('unpublished','published','trash') NOT NULL DEFAULT 'unpublished',
  `importance` enum('usual','top','highlighted') NOT NULL DEFAULT 'usual',
  `eav_set_id` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `author_id` (`author_id`),
  KEY `city_id` (`city_id`),
  KEY `category_id` (`category_id`),
  KEY `eav_set_id` (`eav_set_id`),
  CONSTRAINT `ad_ibfk_1` FOREIGN KEY (`author_id`) REFERENCES `user` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `ad_ibfk_2` FOREIGN KEY (`city_id`) REFERENCES `city` (`city_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `ad_ibfk_3` FOREIGN KEY (`category_id`) REFERENCES `category` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `ad_ibfk_4` FOREIGN KEY (`eav_set_id`) REFERENCES `eav_set` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ad`
--

LOCK TABLES `ad` WRITE;
/*!40000 ALTER TABLE `ad` DISABLE KEYS */;
/*!40000 ALTER TABLE `ad` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `category`
--

DROP TABLE IF EXISTS `category`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `category` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(255) NOT NULL,
  `root` int(10) unsigned DEFAULT NULL,
  `lft` int(10) unsigned NOT NULL,
  `rgt` int(10) unsigned NOT NULL,
  `level` smallint(5) unsigned NOT NULL,
  `set_id` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `root` (`root`),
  KEY `lft` (`lft`),
  KEY `rgt` (`rgt`),
  KEY `level` (`level`),
  KEY `ix_set_id` (`set_id`),
  CONSTRAINT `category_ibfk_1` FOREIGN KEY (`set_id`) REFERENCES `eav_set` (`id`) ON DELETE SET NULL ON UPDATE CASCADE,
  CONSTRAINT `ix_set_id` FOREIGN KEY (`set_id`) REFERENCES `eav_set` (`id`) ON DELETE CASCADE ON UPDATE SET NULL
) ENGINE=InnoDB AUTO_INCREMENT=3235 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `category`
--

LOCK TABLES `category` WRITE;
/*!40000 ALTER TABLE `category` DISABLE KEYS */;
INSERT INTO `category` VALUES (1,'Детский мир',1,1,28,1,12),(2,'Недвижимость',2,1,100,1,12),(3,'Транспорт',3,1,3088,1,12),(4,'Работа',4,1,42,1,12),(5,'Животные',5,1,234,1,12),(6,'Дом и сад',6,1,120,1,12),(7,'Электроника',7,1,2130,1,12),(8,'Бизнес и услуги',8,1,102,1,12),(9,'Мода и стиль',9,1,256,1,12),(10,'Хобби, отдых и спорт',10,1,368,1,12),(11,'Детская одежда',1,2,9,2,4),(12,'Детская обувь',1,10,11,2,5),(13,'Детские коляски',1,12,13,2,9),(14,'Детские автокресла',1,14,15,2,9),(15,'Детская мебель',1,16,17,2,9),(16,'Игрушки',1,18,19,2,9),(17,'Детский транспорт',1,20,21,2,9),(18,'Товары для кормления',1,22,23,2,9),(19,'Товары для школьников',1,24,25,2,9),(20,'Прочие детские товары',1,26,27,2,9),(21,'Одежда для мальчиков',1,3,4,3,4),(22,'Одежда для девочек',1,5,6,3,4),(23,'Одежда для новорожденных',1,7,8,3,4),(24,'Аренда квартир',2,2,9,2,7),(25,'Аренда комнат',2,10,17,2,7),(26,'Аренда домов',2,18,23,2,10),(27,'Аренда земли',2,24,25,2,11),(28,'Аренда гаражей / стоянок',2,26,27,2,11),(29,'Ищу компаньона',2,28,29,2,12),(30,'Продажа квартир',2,30,35,2,6),(31,'Продажа комнат',2,36,37,2,7),(32,'Продажа домов',2,38,45,2,10),(33,'Продажа земли',2,46,55,2,10),(34,'Продажа гаражей / стоянок',2,56,57,2,11),(35,'Аренда помещений',2,58,77,2,10),(36,'Продажа помещений',2,78,95,2,10),(37,'Обмен недвижимости',2,96,97,2,12),(38,'Прочая недвижимость',2,98,99,2,11),(39,'Квартиры посуточно',2,3,4,3,7),(40,'Квартиры с почасовой оплатой',2,5,6,3,7),(41,'Долгосрочная аренда квартир',2,7,8,3,7),(42,'Койко-места',2,11,12,3,7),(43,'Комнаты посуточно',2,13,14,3,7),(44,'Долгосрочная аренда комнат',2,15,16,3,7),(45,'Долгосрочная аренда домов',2,19,20,3,10),(46,'Дома посуточно / почасово',2,21,22,3,10),(47,'Вторичный рынок',2,31,32,3,6),(48,'Новостройки',2,33,34,3,6),(49,'Продажа домов в городе',2,39,40,3,10),(50,'Продажа домов за городом',2,41,42,3,10),(51,'Продажа дач',2,43,44,3,10),(52,'Земля под индивидуальное строительство',2,47,48,3,10),(53,'Земля под сад / огород',2,49,50,3,10),(54,'Земля сельскохозяйственного назначения',2,51,52,3,10),(55,'Земля промышленного назначения',2,53,54,3,10),(56,'Аренда магазинов / салонов',2,59,60,3,10),(57,'Аренда ресторанов / баров',2,61,62,3,10),(58,'Аренда офисов',2,63,64,3,10),(59,'Аренда складов',2,65,66,3,10),(60,'Аренда отдельно стоящих зданий',2,67,68,3,10),(61,'Аренда баз отдыха',2,69,70,3,10),(62,'Аренда помещений промышленного назначения',2,71,72,3,10),(63,'Аренда помещений свободного назначения',2,73,74,3,10),(64,'Прочее',2,75,76,3,10),(65,'Продажа магазинов / салонов',2,79,80,3,10),(66,'Продажа ресторанов / баров',2,81,82,3,10),(67,'Продажа офисов',2,83,84,3,10),(68,'Продажа складов',2,85,86,3,10),(69,'Продажа отдельно стоящих зданий',2,87,88,3,10),(70,'Продажа баз отдыха',2,89,90,3,10),(71,'Продажа помещений промышленного назначения',2,91,92,3,10),(72,'Продажа помещений свободного назначения',2,93,94,3,10),(73,'Легковые автомобили',3,2,2537,2,1),(74,'Мото',3,2538,2745,2,3),(75,'Автобусы',3,2746,2807,2,11),(76,'Спецтехника',3,2808,2825,2,11),(77,'Грузовые автомобили',3,2826,2899,2,2),(78,'Сельхозтехника',3,2900,2901,2,11),(79,'Водный транспорт',3,2902,2903,2,11),(80,'Воздушный транспорт',3,2904,2905,2,11),(81,'Запчасти / аксессуары',3,2906,3083,2,9),(82,'Прицепы',3,3084,3085,2,11),(83,'Другой транспорт',3,3086,3087,2,11),(84,'Acura',3,3,22,3,1),(85,'Alfa Romeo',3,23,66,3,1),(86,'Audi',3,67,134,3,1),(87,'BMW',3,135,180,3,1),(88,'Brilliance',3,181,188,3,1),(89,'Chery',3,189,224,3,1),(90,'Chevrolet',3,225,318,3,1),(91,'Chrysler',3,319,366,3,1),(92,'Citroen',3,367,430,3,1),(93,'Dacia',3,431,454,3,1),(94,'Daewoo',3,455,504,3,1),(95,'Dodge',3,505,546,3,1),(96,'Fiat',3,547,630,3,1),(97,'Ford',3,631,738,3,1),(98,'Geely',3,739,756,3,1),(99,'Great Wall',3,757,776,3,1),(100,'Honda',3,777,838,3,1),(101,'Hyundai',3,839,918,3,1),(102,'Jeep',3,919,934,3,1),(103,'JMC',3,935,938,3,1),(104,'Kia',3,939,994,3,1),(105,'Land Rover',3,995,1020,3,1),(106,'Lexus',3,1021,1042,3,1),(107,'Mazda',3,1043,1118,3,1),(108,'Mercedes',3,1119,1204,3,1),(109,'Mitsubishi',3,1205,1288,3,1),(110,'Nissan',3,1289,1418,3,1),(111,'Oldsmobile',3,1419,1426,3,1),(112,'Opel',3,1427,1494,3,1),(113,'Peugeot',3,1495,1562,3,1),(114,'Porsche',3,1563,1590,3,1),(115,'Renault',3,1591,1684,3,1),(116,'Roewe',3,1685,1688,3,1),(117,'Seat',3,1689,1720,3,1),(118,'Skoda',3,1721,1752,3,1),(119,'Smart',3,1753,1764,3,1),(120,'Ssang Yong',3,1765,1782,3,1),(121,'Subaru',3,1783,1818,3,1),(122,'Suzuki',3,1819,1860,3,1),(123,'Toyota',3,1861,2030,3,1),(124,'Volkswagen',3,2031,2108,3,1),(125,'Volvo',3,2109,2166,3,1),(126,'ВАЗ',3,2167,2326,3,1),(127,'ГАЗ',3,2327,2388,3,1),(128,'ЗАЗ',3,2389,2420,3,1),(129,'ИЖ',3,2421,2440,3,1),(130,'Москвич',3,2441,2482,3,1),(131,'Таврия',3,2483,2484,3,1),(132,'УАЗ',3,2485,2534,3,1),(133,'Другие',3,2535,2536,3,1),(134,'CL',3,4,5,4,1),(135,'Legend',3,6,7,4,1),(136,'MDX',3,8,9,4,1),(137,'RDX',3,10,11,4,1),(138,'RL',3,12,13,4,1),(139,'TL',3,14,15,4,1),(140,'TSX',3,16,17,4,1),(141,'ZDX',3,18,19,4,1),(142,'Другая',3,20,21,4,1),(143,'145',3,24,25,4,1),(144,'146',3,26,27,4,1),(145,'147',3,28,29,4,1),(146,'155',3,30,31,4,1),(147,'156',3,32,33,4,1),(148,'159',3,34,35,4,1),(149,'164',3,36,37,4,1),(150,'166',3,38,39,4,1),(151,'33',3,40,41,4,1),(152,'75',3,42,43,4,1),(153,'90',3,44,45,4,1),(154,'Alfasud',3,46,47,4,1),(155,'Alfetta',3,48,49,4,1),(156,'Arna',3,50,51,4,1),(157,'Brera',3,52,53,4,1),(158,'Giulietta',3,54,55,4,1),(159,'GT',3,56,57,4,1),(160,'GTV',3,58,59,4,1),(161,'MiTo',3,60,61,4,1),(162,'Spider',3,62,63,4,1),(163,'Другая',3,64,65,4,1),(164,'50',3,68,69,4,1),(165,'80',3,70,71,4,1),(166,'90',3,72,73,4,1),(167,'100',3,74,75,4,1),(168,'200',3,76,77,4,1),(169,'5000',3,78,79,4,1),(170,'A1',3,80,81,4,1),(171,'A2',3,82,83,4,1),(172,'A3',3,84,85,4,1),(173,'A4',3,86,87,4,1),(174,'A5',3,88,89,4,1),(175,'A6',3,90,91,4,1),(176,'A6 Allroad',3,92,93,4,1),(177,'A7',3,94,95,4,1),(178,'A8',3,96,97,4,1),(179,'Cabriolet',3,98,99,4,1),(180,'NSU RO 80',3,100,101,4,1),(181,'Q3',3,102,103,4,1),(182,'Q5',3,104,105,4,1),(183,'Q7',3,106,107,4,1),(184,'Quattro',3,108,109,4,1),(185,'R8',3,110,111,4,1),(186,'RS3',3,112,113,4,1),(187,'RS6',3,114,115,4,1),(188,'S3',3,116,117,4,1),(189,'S4',3,118,119,4,1),(190,'S5',3,120,121,4,1),(191,'S6',3,122,123,4,1),(192,'S7',3,124,125,4,1),(193,'S8',3,126,127,4,1),(194,'TT',3,128,129,4,1),(195,'V8',3,130,131,4,1),(196,'Другая',3,132,133,4,1),(197,'02 (E10)',3,136,137,4,1),(198,'1 серия',3,138,139,4,1),(199,'3 серия',3,140,141,4,1),(200,'5 серия',3,142,143,4,1),(201,'6 серия',3,144,145,4,1),(202,'7 серия',3,146,147,4,1),(203,'8 серия',3,148,149,4,1),(204,'Gran Turismo',3,150,151,4,1),(205,'M1',3,152,153,4,1),(206,'M3',3,154,155,4,1),(207,'M5',3,156,157,4,1),(208,'M6',3,158,159,4,1),(209,'X1',3,160,161,4,1),(210,'X3',3,162,163,4,1),(211,'X5',3,164,165,4,1),(212,'X5 M',3,166,167,4,1),(213,'X6',3,168,169,4,1),(214,'X6 M',3,170,171,4,1),(215,'Z3',3,172,173,4,1),(216,'Z4',3,174,175,4,1),(217,'Z4 M',3,176,177,4,1),(218,'Другая',3,178,179,4,1),(219,'BS6',3,182,183,4,1),(220,'M1 (Zhonghua)',3,184,185,4,1),(221,'M2 (JunJie)',3,186,187,4,1),(222,'Bonus (A13)',3,190,191,4,1),(223,'CrossEastar',3,192,193,4,1),(224,'Eastar',3,194,195,4,1),(225,'Elara',3,196,197,4,1),(226,'Fora',3,198,199,4,1),(227,'IndiS (S18D)',3,200,201,4,1),(228,'Karry',3,202,203,4,1),(229,'Kimo (A1)',3,204,205,4,1),(230,'M11',3,206,207,4,1),(231,'Oriental Son',3,208,209,4,1),(232,'QQ6 (S21)',3,210,211,4,1),(233,'Sweet (QQ)',3,212,213,4,1),(234,'Tiggo',3,214,215,4,1),(235,'Другая',3,216,217,4,1),(236,'A21',3,218,219,4,1),(237,'Amulet',3,220,221,4,1),(238,'Beat',3,222,223,4,1),(239,'Alero',3,226,227,4,1),(240,'Astra',3,228,229,4,1),(241,'Astro',3,230,231,4,1),(242,'Avalanche',3,232,233,4,1),(243,'Aveo',3,234,235,4,1),(244,'Beretta',3,236,237,4,1),(245,'Blazer',3,238,239,4,1),(246,'Camaro',3,240,241,4,1),(247,'Caprice',3,242,243,4,1),(248,'Captiva',3,244,245,4,1),(249,'Cavalier',3,246,247,4,1),(250,'Chevette',3,248,249,4,1),(251,'Cobalt',3,250,251,4,1),(252,'Colorado',3,252,253,4,1),(253,'Corsica',3,254,255,4,1),(254,'Corvette',3,256,257,4,1),(255,'Cruze',3,258,259,4,1),(256,'Epica',3,260,261,4,1),(257,'Equinox',3,262,263,4,1),(258,'Evanda',3,264,265,4,1),(259,'Express',3,266,267,4,1),(260,'Geo Storm',3,268,269,4,1),(261,'Geo Tracker',3,270,271,4,1),(262,'HHR',3,272,273,4,1),(263,'Impala',3,274,275,4,1),(264,'Lacetti',3,276,277,4,1),(265,'Lanos',3,278,279,4,1),(266,'Lumina',3,280,281,4,1),(267,'Malibu',3,282,283,4,1),(268,'Metro',3,284,285,4,1),(269,'Monte Carlo',3,286,287,4,1),(270,'Niva',3,288,289,4,1),(271,'Nova',3,290,291,4,1),(272,'Nubira',3,292,293,4,1),(273,'Orlando',3,294,295,4,1),(274,'S-10 Pickup',3,296,297,4,1),(275,'Spark',3,298,299,4,1),(276,'Suburban',3,300,301,4,1),(277,'Tacuma',3,302,303,4,1),(278,'Tahoe',3,304,305,4,1),(279,'Tracker',3,306,307,4,1),(280,'Van',3,308,309,4,1),(281,'Venture',3,310,311,4,1),(282,'Viva',3,312,313,4,1),(283,'Volt',3,314,315,4,1),(284,'Другая',3,316,317,4,1),(285,'Другая',3,320,321,4,1),(286,'300C',3,322,323,4,1),(287,'300M',3,324,325,4,1),(288,'Cirrus',3,326,327,4,1),(289,'Concorde',3,328,329,4,1),(290,'Crossfire',3,330,331,4,1),(291,'Daytona Shelby',3,332,333,4,1),(292,'Dynasty',3,334,335,4,1),(293,'Grand Voyager',3,336,337,4,1),(294,'Intrepid',3,338,339,4,1),(295,'LE Baron',3,340,341,4,1),(296,'LHS',3,342,343,4,1),(297,'Laser',3,344,345,4,1),(298,'NEW Yorker',3,346,347,4,1),(299,'Neon',3,348,349,4,1),(300,'PT Cruiser',3,350,351,4,1),(301,'Pacifica',3,352,353,4,1),(302,'Saratoga',3,354,355,4,1),(303,'Sebring',3,356,357,4,1),(304,'Stratus',3,358,359,4,1),(305,'Town & Country',3,360,361,4,1),(306,'Vision',3,362,363,4,1),(307,'Voyager',3,364,365,4,1),(308,'2 CV',3,368,369,4,1),(309,'Ax',3,370,371,4,1),(310,'BX',3,372,373,4,1),(311,'Berlingo',3,374,375,4,1),(312,'C1',3,376,377,4,1),(313,'C15',3,378,379,4,1),(314,'C2',3,380,381,4,1),(315,'C25',3,382,383,4,1),(316,'C3',3,384,385,4,1),(317,'C4',3,386,387,4,1),(318,'C4 Aircross',3,388,389,4,1),(319,'C4 Picasso',3,390,391,4,1),(320,'C5',3,392,393,4,1),(321,'C6',3,394,395,4,1),(322,'C8',3,396,397,4,1),(323,'CX',3,398,399,4,1),(324,'DS',3,400,401,4,1),(325,'DS3',3,402,403,4,1),(326,'DS4',3,404,405,4,1),(327,'DS5',3,406,407,4,1),(328,'Evasion',3,408,409,4,1),(329,'Jumper',3,410,411,4,1),(330,'Jumpy',3,412,413,4,1),(331,'Nemo',3,414,415,4,1),(332,'Visa',3,416,417,4,1),(333,'XM',3,418,419,4,1),(334,'Xantia',3,420,421,4,1),(335,'Xsara',3,422,423,4,1),(336,'Xsara Picasso',3,424,425,4,1),(337,'ZX',3,426,427,4,1),(338,'Другая',3,428,429,4,1),(339,'1300',3,432,433,4,1),(340,'1304',3,434,435,4,1),(341,'1310',3,436,437,4,1),(342,'1410',3,438,439,4,1),(343,'Duster',3,440,441,4,1),(344,'Logan',3,442,443,4,1),(345,'Nova',3,444,445,4,1),(346,'Sandero',3,446,447,4,1),(347,'Solenza',3,448,449,4,1),(348,'SuperNova',3,450,451,4,1),(349,'Другая',3,452,453,4,1),(350,'Brougham',3,456,457,4,1),(351,'Damas',3,458,459,4,1),(352,'Espero',3,460,461,4,1),(353,'Evanda',3,462,463,4,1),(354,'Gentra',3,464,465,4,1),(355,'Kalos',3,466,467,4,1),(356,'LE Mans',3,468,469,4,1),(357,'Lacetti',3,470,471,4,1),(358,'Lanos (Sens)',3,472,473,4,1),(359,'Leganza',3,474,475,4,1),(360,'Lublin',3,476,477,4,1),(361,'Magnus',3,478,479,4,1),(362,'Matiz',3,480,481,4,1),(363,'Musso',3,482,483,4,1),(364,'Nexia',3,484,485,4,1),(365,'Nubira',3,486,487,4,1),(366,'Polonez',3,488,489,4,1),(367,'Prince',3,490,491,4,1),(368,'Racer',3,492,493,4,1),(369,'Sens',3,494,495,4,1),(370,'Statesman',3,496,497,4,1),(371,'Super Salon',3,498,499,4,1),(372,'Tico',3,500,501,4,1),(373,'Другая',3,502,503,4,1),(374,'Aries',3,506,507,4,1),(375,'Avenger',3,508,509,4,1),(376,'Caliber',3,510,511,4,1),(377,'Caravan',3,512,513,4,1),(378,'Challenger',3,514,515,4,1),(379,'Charger',3,516,517,4,1),(380,'Dakota',3,518,519,4,1),(381,'Daytona',3,520,521,4,1),(382,'Durango',3,522,523,4,1),(383,'Grand Caravan',3,524,525,4,1),(384,'Intrepid',3,526,527,4,1),(385,'Magnum',3,528,529,4,1),(386,'Monaco',3,530,531,4,1),(387,'Neon',3,532,533,4,1),(388,'Nitro',3,534,535,4,1),(389,'RAM',3,536,537,4,1),(390,'Shadow',3,538,539,4,1),(391,'Spirit',3,540,541,4,1),(392,'Stratus',3,542,543,4,1),(393,'Другая',3,544,545,4,1),(394,'2300',3,548,549,4,1),(395,'900   T/E',3,550,551,4,1),(396,'Albea',3,552,553,4,1),(397,'Argenta',3,554,555,4,1),(398,'Barchetta',3,556,557,4,1),(399,'Brava',3,558,559,4,1),(400,'Bravo',3,560,561,4,1),(401,'Cinquecento',3,562,563,4,1),(402,'Coupe',3,564,565,4,1),(403,'Croma',3,566,567,4,1),(404,'Doblo',3,568,569,4,1),(405,'Ducato',3,570,571,4,1),(406,'Duna',3,572,573,4,1),(407,'Fiorino',3,574,575,4,1),(408,'Grande Punto',3,576,577,4,1),(409,'Idea',3,578,579,4,1),(410,'Linea',3,580,581,4,1),(411,'Marea',3,582,583,4,1),(412,'Multipla',3,584,585,4,1),(413,'New 500',3,586,587,4,1),(414,'Palio',3,588,589,4,1),(415,'Panda',3,590,591,4,1),(416,'Punto',3,592,593,4,1),(417,'Qubo',3,594,595,4,1),(418,'Regata',3,596,597,4,1),(419,'Ritmo',3,598,599,4,1),(420,'Scudo',3,600,601,4,1),(421,'Seicento',3,602,603,4,1),(422,'Siena',3,604,605,4,1),(423,'Stilo',3,606,607,4,1),(424,'Tempra',3,608,609,4,1),(425,'Tipo',3,610,611,4,1),(426,'UNO',3,612,613,4,1),(427,'Ulysse',3,614,615,4,1),(428,'Другая',3,616,617,4,1),(429,'124',3,618,619,4,1),(430,'126',3,620,621,4,1),(431,'127',3,622,623,4,1),(432,'131',3,624,625,4,1),(433,'132',3,626,627,4,1),(434,'500',3,628,629,4,1),(435,'Aerostar',3,632,633,4,1),(436,'Aspire',3,634,635,4,1),(437,'Bronco',3,636,637,4,1),(438,'C-MAX',3,638,639,4,1),(439,'Capri',3,640,641,4,1),(440,'Contour',3,642,643,4,1),(441,'Cougar',3,644,645,4,1),(442,'Courier',3,646,647,4,1),(443,'Crown Victoria',3,648,649,4,1),(444,'Econoline',3,650,651,4,1),(445,'Econovan',3,652,653,4,1),(446,'Edge',3,654,655,4,1),(447,'Escape',3,656,657,4,1),(448,'Escort',3,658,659,4,1),(449,'Excursion',3,660,661,4,1),(450,'Expedition',3,662,663,4,1),(451,'Explorer',3,664,665,4,1),(452,'F-series',3,666,667,4,1),(453,'Festiva',3,668,669,4,1),(454,'Fiesta',3,670,671,4,1),(455,'Fiesta ST',3,672,673,4,1),(456,'Flex',3,674,675,4,1),(457,'Focus',3,676,677,4,1),(458,'Focus RS',3,678,679,4,1),(459,'Focus ST',3,680,681,4,1),(460,'Fusion',3,682,683,4,1),(461,'Fusion (USA)',3,684,685,4,1),(462,'GT',3,686,687,4,1),(463,'Galaxy',3,688,689,4,1),(464,'Granada',3,690,691,4,1),(465,'KA',3,692,693,4,1),(466,'Kuga',3,694,695,4,1),(467,'Laser',3,696,697,4,1),(468,'Maverick',3,698,699,4,1),(469,'Mondeo',3,700,701,4,1),(470,'Mustang',3,702,703,4,1),(471,'Orion',3,704,705,4,1),(472,'Probe',3,706,707,4,1),(473,'Puma',3,708,709,4,1),(474,'Ranger',3,710,711,4,1),(475,'S-MAX',3,712,713,4,1),(476,'Scorpio',3,714,715,4,1),(477,'Shelby',3,716,717,4,1),(478,'Sierra',3,718,719,4,1),(479,'Taunus',3,720,721,4,1),(480,'Taurus',3,722,723,4,1),(481,'Taurus X',3,724,725,4,1),(482,'Tempo',3,726,727,4,1),(483,'Thunderbird',3,728,729,4,1),(484,'Tourneo Connect',3,730,731,4,1),(485,'Transit',3,732,733,4,1),(486,'Windstar',3,734,735,4,1),(487,'Другая',3,736,737,4,1),(488,'CK',3,740,741,4,1),(489,'Emgrand',3,742,743,4,1),(490,'FC',3,744,745,4,1),(491,'MK',3,746,747,4,1),(492,'MK Cross',3,748,749,4,1),(493,'Maple',3,750,751,4,1),(494,'Uliou',3,752,753,4,1),(495,'Другая',3,754,755,4,1),(496,'Wingle',3,758,759,4,1),(497,'Другая',3,760,761,4,1),(498,'Cool Bear',3,762,763,4,1),(499,'Deer',3,764,765,4,1),(500,'Florid',3,766,767,4,1),(501,'Hover',3,768,769,4,1),(502,'Pegasus',3,770,771,4,1),(503,'SUV',3,772,773,4,1),(504,'Safe',3,774,775,4,1),(505,'Accord',3,778,779,4,1),(506,'Airwave',3,780,781,4,1),(507,'CR-V',3,782,783,4,1),(508,'CR-Z',3,784,785,4,1),(509,'CRX',3,786,787,4,1),(510,'City',3,788,789,4,1),(511,'Civic',3,790,791,4,1),(512,'Civic Hybrid',3,792,793,4,1),(513,'Civic Shuttle',3,794,795,4,1),(514,'Concerto',3,796,797,4,1),(515,'Crosstour',3,798,799,4,1),(516,'Element',3,800,801,4,1),(517,'FR-V',3,802,803,4,1),(518,'Fit',3,804,805,4,1),(519,'HR-V',3,806,807,4,1),(520,'Insight',3,808,809,4,1),(521,'Inspire',3,810,811,4,1),(522,'Integra',3,812,813,4,1),(523,'Jazz',3,814,815,4,1),(524,'Legend',3,816,817,4,1),(525,'Logo',3,818,819,4,1),(526,'Odyssey',3,820,821,4,1),(527,'Pilot',3,822,823,4,1),(528,'Prelude',3,824,825,4,1),(529,'Quintet',3,826,827,4,1),(530,'S2000',3,828,829,4,1),(531,'Shuttle',3,830,831,4,1),(532,'Stream',3,832,833,4,1),(533,'Vigor',3,834,835,4,1),(534,'Другая',3,836,837,4,1),(535,'Accent',3,840,841,4,1),(536,'Atos',3,842,843,4,1),(537,'Avante',3,844,845,4,1),(538,'Azera',3,846,847,4,1),(539,'Coupe',3,848,849,4,1),(540,'Elantra',3,850,851,4,1),(541,'Equus',3,852,853,4,1),(542,'Excel',3,854,855,4,1),(543,'Galloper',3,856,857,4,1),(544,'Genesis',3,858,859,4,1),(545,'Getz',3,860,861,4,1),(546,'Grandeur',3,862,863,4,1),(547,'H1 (Starex)',3,864,865,4,1),(548,'H100',3,866,867,4,1),(549,'H200',3,868,869,4,1),(550,'HD',3,870,871,4,1),(551,'Lantra',3,872,873,4,1),(552,'Matrix',3,874,875,4,1),(553,'Pony',3,876,877,4,1),(554,'S-Coupe',3,878,879,4,1),(555,'Santa FE',3,880,881,4,1),(556,'Solaris',3,882,883,4,1),(557,'Sonata',3,884,885,4,1),(558,'Starex',3,886,887,4,1),(559,'Terracan',3,888,889,4,1),(560,'Tiburon',3,890,891,4,1),(561,'Trajet',3,892,893,4,1),(562,'Tucson',3,894,895,4,1),(563,'Veloster',3,896,897,4,1),(564,'Veracruz (ix55)',3,898,899,4,1),(565,'Verna',3,900,901,4,1),(566,'XG',3,902,903,4,1),(567,'i10',3,904,905,4,1),(568,'i20',3,906,907,4,1),(569,'i30',3,908,909,4,1),(570,'i40',3,910,911,4,1),(571,'ix35',3,912,913,4,1),(572,'ix55',3,914,915,4,1),(573,'Другая',3,916,917,4,1),(574,'Grand Cherokee',3,920,921,4,1),(575,'Liberty',3,922,923,4,1),(576,'Patriot',3,924,925,4,1),(577,'Wrangler',3,926,927,4,1),(578,'Другая',3,928,929,4,1),(579,'Cherokee',3,930,931,4,1),(580,'Compass',3,932,933,4,1),(581,'Baodian',3,936,937,4,1),(582,'Avella',3,940,941,4,1),(583,'Besta',3,942,943,4,1),(584,'Cadenza (K7)',3,944,945,4,1),(585,'Capital',3,946,947,4,1),(586,'Carens',3,948,949,4,1),(587,'Carnival',3,950,951,4,1),(588,'Cee\'d',3,952,953,4,1),(589,'Cerato (Forte)',3,954,955,4,1),(590,'Clarus',3,956,957,4,1),(591,'Joice',3,958,959,4,1),(592,'K2700',3,960,961,4,1),(593,'Magentis',3,962,963,4,1),(594,'Mohave (Borrego)',3,964,965,4,1),(595,'Opirus',3,966,967,4,1),(596,'Optima',3,968,969,4,1),(597,'Picanto',3,970,971,4,1),(598,'Pregio',3,972,973,4,1),(599,'Pride',3,974,975,4,1),(600,'Rio (Pride)',3,976,977,4,1),(601,'Sephia',3,978,979,4,1),(602,'Shuma',3,980,981,4,1),(603,'Sorento',3,982,983,4,1),(604,'Soul',3,984,985,4,1),(605,'Spectra',3,986,987,4,1),(606,'Sportage',3,988,989,4,1),(607,'Venga',3,990,991,4,1),(608,'Другая',3,992,993,4,1),(609,'90/110',3,996,997,4,1),(610,'Defender',3,998,999,4,1),(611,'Discovery',3,1000,1001,4,1),(612,'Evoque',3,1002,1003,4,1),(613,'Freelander',3,1004,1005,4,1),(614,'Hardtop',3,1006,1007,4,1),(615,'Land Rover',3,1008,1009,4,1),(616,'Range Rover',3,1010,1011,4,1),(617,'Range Rover Evoque',3,1012,1013,4,1),(618,'Range Rover Sport',3,1014,1015,4,1),(619,'Series I',3,1016,1017,4,1),(620,'Другая',3,1018,1019,4,1),(621,'CT',3,1022,1023,4,1),(622,'ES серия',3,1024,1025,4,1),(623,'GS серия',3,1026,1027,4,1),(624,'GX серия',3,1028,1029,4,1),(625,'IS серия',3,1030,1031,4,1),(626,'LS серия',3,1032,1033,4,1),(627,'LX серия',3,1034,1035,4,1),(628,'RX серия',3,1036,1037,4,1),(629,'SC серия',3,1038,1039,4,1),(630,'Другая',3,1040,1041,4,1),(631,'Mx-6',3,1044,1045,4,1),(632,'Navajo',3,1046,1047,4,1),(633,'Persona',3,1048,1049,4,1),(634,'Premacy',3,1050,1051,4,1),(635,'Protege',3,1052,1053,4,1),(636,'RX 7',3,1054,1055,4,1),(637,'Rx-8',3,1056,1057,4,1),(638,'Tribute',3,1058,1059,4,1),(639,'Xedos 6',3,1060,1061,4,1),(640,'Xedos 9',3,1062,1063,4,1),(641,'Другая',3,1064,1065,4,1),(642,'2',3,1066,1067,4,1),(643,'3',3,1068,1069,4,1),(644,'5',3,1070,1071,4,1),(645,'6',3,1072,1073,4,1),(646,'121',3,1074,1075,4,1),(647,'323',3,1076,1077,4,1),(648,'6 MPS',3,1078,1079,4,1),(649,'626',3,1080,1081,4,1),(650,'929',3,1082,1083,4,1),(651,'Axela',3,1084,1085,4,1),(652,'B-series',3,1086,1087,4,1),(653,'BT-50',3,1088,1089,4,1),(654,'Bongo',3,1090,1091,4,1),(655,'CX-5',3,1092,1093,4,1),(656,'CX-7',3,1094,1095,4,1),(657,'CX-9',3,1096,1097,4,1),(658,'Capella',3,1098,1099,4,1),(659,'Demio',3,1100,1101,4,1),(660,'E 2000,2200 Bus',3,1102,1103,4,1),(661,'E 2000,2200 Kasten',3,1104,1105,4,1),(662,'Eunos 800',3,1106,1107,4,1),(663,'Familia',3,1108,1109,4,1),(664,'MPV',3,1110,1111,4,1),(665,'Millenia',3,1112,1113,4,1),(666,'Mx-3',3,1114,1115,4,1),(667,'Mx-5',3,1116,1117,4,1),(668,'8',3,1120,1121,4,1),(669,'190',3,1122,1123,4,1),(670,'200',3,1124,1125,4,1),(671,'220',3,1126,1127,4,1),(672,'230',3,1128,1129,4,1),(673,'240',3,1130,1131,4,1),(674,'250',3,1132,1133,4,1),(675,'260',3,1134,1135,4,1),(676,'280',3,1136,1137,4,1),(677,'300',3,1138,1139,4,1),(678,'320',3,1140,1141,4,1),(679,'500',3,1142,1143,4,1),(680,'A серия',3,1144,1145,4,1),(681,'B серия',3,1146,1147,4,1),(682,'C серия',3,1148,1149,4,1),(683,'CL серия',3,1150,1151,4,1),(684,'CLC серия',3,1152,1153,4,1),(685,'CLK серия',3,1154,1155,4,1),(686,'CLS серия',3,1156,1157,4,1),(687,'Cabrio',3,1158,1159,4,1),(688,'Coupe',3,1160,1161,4,1),(689,'E серия',3,1162,1163,4,1),(690,'G серия',3,1164,1165,4,1),(691,'GL серия',3,1166,1167,4,1),(692,'GLK серия',3,1168,1169,4,1),(693,'ML серия',3,1170,1171,4,1),(694,'Pullmann',3,1172,1173,4,1),(695,'R серия',3,1174,1175,4,1),(696,'S серия',3,1176,1177,4,1),(697,'SL серия',3,1178,1179,4,1),(698,'SLK серия',3,1180,1181,4,1),(699,'SLS',3,1182,1183,4,1),(700,'Sprinter',3,1184,1185,4,1),(701,'T-mod.',3,1186,1187,4,1),(702,'V серия',3,1188,1189,4,1),(703,'Vaneo',3,1190,1191,4,1),(704,'Vario',3,1192,1193,4,1),(705,'Viano',3,1194,1195,4,1),(706,'Vito',3,1196,1197,4,1),(707,'W123',3,1198,1199,4,1),(708,'W124',3,1200,1201,4,1),(709,'Другая',3,1202,1203,4,1),(710,'Space Runner',3,1206,1207,4,1),(711,'Space Star',3,1208,1209,4,1),(712,'Space Wagon',3,1210,1211,4,1),(713,'Toppo',3,1212,1213,4,1),(714,'Tredia',3,1214,1215,4,1),(715,'i',3,1216,1217,4,1),(716,'Другая',3,1218,1219,4,1),(717,'3000 GT',3,1220,1221,4,1),(718,'500',3,1222,1223,4,1),(719,'ASX',3,1224,1225,4,1),(720,'Airtrek',3,1226,1227,4,1),(721,'Carisma',3,1228,1229,4,1),(722,'Celeste',3,1230,1231,4,1),(723,'Colt',3,1232,1233,4,1),(724,'Cordia',3,1234,1235,4,1),(725,'Delica',3,1236,1237,4,1),(726,'Diamante',3,1238,1239,4,1),(727,'EK Wagon',3,1240,1241,4,1),(728,'Eclipse',3,1242,1243,4,1),(729,'Endeavor',3,1244,1245,4,1),(730,'Eterna',3,1246,1247,4,1),(731,'FTO',3,1248,1249,4,1),(732,'Galant',3,1250,1251,4,1),(733,'Grandis',3,1252,1253,4,1),(734,'Jeep',3,1254,1255,4,1),(735,'L 200',3,1256,1257,4,1),(736,'Lancer',3,1258,1259,4,1),(737,'Lancer Cedia',3,1260,1261,4,1),(738,'Lancer Evolution',3,1262,1263,4,1),(739,'Lancer Sportback',3,1264,1265,4,1),(740,'Mirage',3,1266,1267,4,1),(741,'Montero',3,1268,1269,4,1),(742,'Montero Sport',3,1270,1271,4,1),(743,'Nativa',3,1272,1273,4,1),(744,'Outlander',3,1274,1275,4,1),(745,'Pajero',3,1276,1277,4,1),(746,'Pajero Sport',3,1278,1279,4,1),(747,'Proton',3,1280,1281,4,1),(748,'Sapporo',3,1282,1283,4,1),(749,'Sigma',3,1284,1285,4,1),(750,'Space Gear',3,1286,1287,4,1),(751,'Stanza',3,1290,1291,4,1),(752,'Sunny',3,1292,1293,4,1),(753,'Teana',3,1294,1295,4,1),(754,'Terrano',3,1296,1297,4,1),(755,'Tiida',3,1298,1299,4,1),(756,'Tino',3,1300,1301,4,1),(757,'Urvan',3,1302,1303,4,1),(758,'Vanette',3,1304,1305,4,1),(759,'Wingroad',3,1306,1307,4,1),(760,'X-Terra',3,1308,1309,4,1),(761,'X-Trail',3,1310,1311,4,1),(762,'Другая',3,1312,1313,4,1),(763,'100 NX',3,1314,1315,4,1),(764,'200 SX',3,1316,1317,4,1),(765,'350Z',3,1318,1319,4,1),(766,'Almera',3,1320,1321,4,1),(767,'Almera Classic',3,1322,1323,4,1),(768,'Almera Tino',3,1324,1325,4,1),(769,'Altima',3,1326,1327,4,1),(770,'Armada',3,1328,1329,4,1),(771,'Bluebird',3,1330,1331,4,1),(772,'Cedric',3,1332,1333,4,1),(773,'Cefiro',3,1334,1335,4,1),(774,'Cherry',3,1336,1337,4,1),(775,'Cima',3,1338,1339,4,1),(776,'Cube',3,1340,1341,4,1),(777,'Datsun',3,1342,1343,4,1),(778,'Expert',3,1344,1345,4,1),(779,'Frontier',3,1346,1347,4,1),(780,'GT-R',3,1348,1349,4,1),(781,'Gloria',3,1350,1351,4,1),(782,'Juke',3,1352,1353,4,1),(783,'King Cab',3,1354,1355,4,1),(784,'Kubistar',3,1356,1357,4,1),(785,'Langley',3,1358,1359,4,1),(786,'Largo',3,1360,1361,4,1),(787,'Laurel',3,1362,1363,4,1),(788,'Leaf',3,1364,1365,4,1),(789,'Liberta Villa',3,1366,1367,4,1),(790,'Liberty',3,1368,1369,4,1),(791,'March',3,1370,1371,4,1),(792,'Maxima',3,1372,1373,4,1),(793,'Micra',3,1374,1375,4,1),(794,'Murano',3,1376,1377,4,1),(795,'NP 300 Pick up',3,1378,1379,4,1),(796,'NV200',3,1380,1381,4,1),(797,'Navara',3,1382,1383,4,1),(798,'Note',3,1384,1385,4,1),(799,'Pathfinder',3,1386,1387,4,1),(800,'Patrol',3,1388,1389,4,1),(801,'Pick UP',3,1390,1391,4,1),(802,'Prairie',3,1392,1393,4,1),(803,'Primastar',3,1394,1395,4,1),(804,'Primera',3,1396,1397,4,1),(805,'Pulsar',3,1398,1399,4,1),(806,'Qashqai',3,1400,1401,4,1),(807,'Quest',3,1402,1403,4,1),(808,'R Nessa',3,1404,1405,4,1),(809,'Rogue',3,1406,1407,4,1),(810,'S-Cargo',3,1408,1409,4,1),(811,'Sentra',3,1410,1411,4,1),(812,'Serena',3,1412,1413,4,1),(813,'Silvia',3,1414,1415,4,1),(814,'Skyline',3,1416,1417,4,1),(815,'Cutlass',3,1420,1421,4,1),(816,'Ninety-Eight',3,1422,1423,4,1),(817,'Другая',3,1424,1425,4,1),(818,'Admiral',3,1428,1429,4,1),(819,'Agila',3,1430,1431,4,1),(820,'Antara',3,1432,1433,4,1),(821,'Ascona',3,1434,1435,4,1),(822,'Astra',3,1436,1437,4,1),(823,'Astra OPC',3,1438,1439,4,1),(824,'Calibra',3,1440,1441,4,1),(825,'Campo',3,1442,1443,4,1),(826,'Combo',3,1444,1445,4,1),(827,'Commodore',3,1446,1447,4,1),(828,'Corsa',3,1448,1449,4,1),(829,'Frontera',3,1450,1451,4,1),(830,'GT',3,1452,1453,4,1),(831,'Insignia',3,1454,1455,4,1),(832,'Kadett',3,1456,1457,4,1),(833,'Manta',3,1458,1459,4,1),(834,'Meriva',3,1460,1461,4,1),(835,'Mokka',3,1462,1463,4,1),(836,'Monterey',3,1464,1465,4,1),(837,'Monza',3,1466,1467,4,1),(838,'Movano',3,1468,1469,4,1),(839,'OPC',3,1470,1471,4,1),(840,'Olimpia',3,1472,1473,4,1),(841,'Omega',3,1474,1475,4,1),(842,'Rekord',3,1476,1477,4,1),(843,'Senator',3,1478,1479,4,1),(844,'Sintra',3,1480,1481,4,1),(845,'Tigra',3,1482,1483,4,1),(846,'Vectra',3,1484,1485,4,1),(847,'Vita',3,1486,1487,4,1),(848,'Vivaro',3,1488,1489,4,1),(849,'Zafira',3,1490,1491,4,1),(850,'Другая',3,1492,1493,4,1),(851,'106',3,1496,1497,4,1),(852,'107',3,1498,1499,4,1),(853,'205',3,1500,1501,4,1),(854,'206',3,1502,1503,4,1),(855,'207',3,1504,1505,4,1),(856,'304',3,1506,1507,4,1),(857,'305',3,1508,1509,4,1),(858,'306',3,1510,1511,4,1),(859,'307',3,1512,1513,4,1),(860,'308',3,1514,1515,4,1),(861,'309',3,1516,1517,4,1),(862,'405',3,1518,1519,4,1),(863,'406',3,1520,1521,4,1),(864,'407',3,1522,1523,4,1),(865,'408',3,1524,1525,4,1),(866,'505',3,1526,1527,4,1),(867,'508',3,1528,1529,4,1),(868,'604',3,1530,1531,4,1),(869,'605',3,1532,1533,4,1),(870,'607',3,1534,1535,4,1),(871,'806',3,1536,1537,4,1),(872,'807',3,1538,1539,4,1),(873,'1007',3,1540,1541,4,1),(874,'3008',3,1542,1543,4,1),(875,'4007',3,1544,1545,4,1),(876,'4008',3,1546,1547,4,1),(877,'5008',3,1548,1549,4,1),(878,'Bipper',3,1550,1551,4,1),(879,'Boxer',3,1552,1553,4,1),(880,'Expert',3,1554,1555,4,1),(881,'Partner',3,1556,1557,4,1),(882,'RCZ',3,1558,1559,4,1),(883,'Другая',3,1560,1561,4,1),(884,'911',3,1564,1565,4,1),(885,'924',3,1566,1567,4,1),(886,'928',3,1568,1569,4,1),(887,'944',3,1570,1571,4,1),(888,'Boxster',3,1572,1573,4,1),(889,'Cayenne',3,1574,1575,4,1),(890,'Cayenne GTS',3,1576,1577,4,1),(891,'Cayenne S',3,1578,1579,4,1),(892,'Cayenne Turbo',3,1580,1581,4,1),(893,'Cayenne Turbo S',3,1582,1583,4,1),(894,'Cayman',3,1584,1585,4,1),(895,'Panamera',3,1586,1587,4,1),(896,'Другая',3,1588,1589,4,1),(897,'4',3,1592,1593,4,1),(898,'5',3,1594,1595,4,1),(899,'6',3,1596,1597,4,1),(900,'8',3,1598,1599,4,1),(901,'9',3,1600,1601,4,1),(902,'10',3,1602,1603,4,1),(903,'11',3,1604,1605,4,1),(904,'12',3,1606,1607,4,1),(905,'14',3,1608,1609,4,1),(906,'15',3,1610,1611,4,1),(907,'16',3,1612,1613,4,1),(908,'18',3,1614,1615,4,1),(909,'19',3,1616,1617,4,1),(910,'21',3,1618,1619,4,1),(911,'25',3,1620,1621,4,1),(912,'Avantime',3,1622,1623,4,1),(913,'Clio',3,1624,1625,4,1),(914,'Clio Symbol',3,1626,1627,4,1),(915,'Duster',3,1628,1629,4,1),(916,'Espace',3,1630,1631,4,1),(917,'Express',3,1632,1633,4,1),(918,'Fluence',3,1634,1635,4,1),(919,'Fuego',3,1636,1637,4,1),(920,'Grand Scenic',3,1638,1639,4,1),(921,'Kangoo Express',3,1640,1641,4,1),(922,'Kangoo Passenger',3,1642,1643,4,1),(923,'Koleos',3,1644,1645,4,1),(924,'Laguna',3,1646,1647,4,1),(925,'Latitude',3,1648,1649,4,1),(926,'Logan',3,1650,1651,4,1),(927,'Master',3,1652,1653,4,1),(928,'Megane',3,1654,1655,4,1),(929,'Modus',3,1656,1657,4,1),(930,'Nevada',3,1658,1659,4,1),(931,'Rapid',3,1660,1661,4,1),(932,'Safrane',3,1662,1663,4,1),(933,'Sandero',3,1664,1665,4,1),(934,'Scenic',3,1666,1667,4,1),(935,'Scenic II',3,1668,1669,4,1),(936,'Scenic III',3,1670,1671,4,1),(937,'Scenic RX',3,1672,1673,4,1),(938,'Symbol (Thalia)',3,1674,1675,4,1),(939,'Trafic',3,1676,1677,4,1),(940,'Twingo',3,1678,1679,4,1),(941,'Twizy',3,1680,1681,4,1),(942,'Другая',3,1682,1683,4,1),(943,'750',3,1686,1687,4,1),(944,'Alhambra',3,1690,1691,4,1),(945,'Altea',3,1692,1693,4,1),(946,'Altea XL',3,1694,1695,4,1),(947,'Arosa',3,1696,1697,4,1),(948,'Cordoba',3,1698,1699,4,1),(949,'Freetrack',3,1700,1701,4,1),(950,'Ibiza',3,1702,1703,4,1),(951,'Inca',3,1704,1705,4,1),(952,'Leon',3,1706,1707,4,1),(953,'Malaga',3,1708,1709,4,1),(954,'Marbella',3,1710,1711,4,1),(955,'Ronda',3,1712,1713,4,1),(956,'Terra',3,1714,1715,4,1),(957,'Toledo',3,1716,1717,4,1),(958,'Другая',3,1718,1719,4,1),(959,'100',3,1722,1723,4,1),(960,'105,120',3,1724,1725,4,1),(961,'Fabia',3,1726,1727,4,1),(962,'Favorit',3,1728,1729,4,1),(963,'Felicia',3,1730,1731,4,1),(964,'Forman',3,1732,1733,4,1),(965,'Octavia',3,1734,1735,4,1),(966,'Octavia Scout',3,1736,1737,4,1),(967,'Praktik',3,1738,1739,4,1),(968,'RS',3,1740,1741,4,1),(969,'Rapid',3,1742,1743,4,1),(970,'Roomster',3,1744,1745,4,1),(971,'Superb',3,1746,1747,4,1),(972,'Yeti',3,1748,1749,4,1),(973,'Другая',3,1750,1751,4,1),(974,'City',3,1754,1755,4,1),(975,'Forfour',3,1756,1757,4,1),(976,'Fortwo',3,1758,1759,4,1),(977,'Roadster',3,1760,1761,4,1),(978,'Другая',3,1762,1763,4,1),(979,'Actyon',3,1766,1767,4,1),(980,'Chairman',3,1768,1769,4,1),(981,'Korando',3,1770,1771,4,1),(982,'Kyron',3,1772,1773,4,1),(983,'Musso',3,1774,1775,4,1),(984,'Rexton',3,1776,1777,4,1),(985,'Rodius',3,1778,1779,4,1),(986,'Другая',3,1780,1781,4,1),(987,'Impreza WRX',3,1784,1785,4,1),(988,'Impreza WRX STI',3,1786,1787,4,1),(989,'Impreza XV',3,1788,1789,4,1),(990,'Justy',3,1790,1791,4,1),(991,'Legacy',3,1792,1793,4,1),(992,'Leone',3,1794,1795,4,1),(993,'Libero',3,1796,1797,4,1),(994,'Outback',3,1798,1799,4,1),(995,'SVX',3,1800,1801,4,1),(996,'Tribeca',3,1802,1803,4,1),(997,'Vivio',3,1804,1805,4,1),(998,'XT',3,1806,1807,4,1),(999,'XV',3,1808,1809,4,1),(1000,'Другая',3,1810,1811,4,1),(1001,'BRZ',3,1812,1813,4,1),(1002,'Forester',3,1814,1815,4,1),(1003,'Impreza',3,1816,1817,4,1),(1004,'Aerio',3,1820,1821,4,1),(1005,'Alto',3,1822,1823,4,1),(1006,'Baleno',3,1824,1825,4,1),(1007,'Cervo',3,1826,1827,4,1),(1008,'Cultus Wagon',3,1828,1829,4,1),(1009,'Grand Vitara',3,1830,1831,4,1),(1010,'Ignis',3,1832,1833,4,1),(1011,'Jimny',3,1834,1835,4,1),(1012,'Kizashi',3,1836,1837,4,1),(1013,'Liana',3,1838,1839,4,1),(1014,'MR Wagon',3,1840,1841,4,1),(1015,'SX4',3,1842,1843,4,1),(1016,'Samurai',3,1844,1845,4,1),(1017,'Splash',3,1846,1847,4,1),(1018,'Swift',3,1848,1849,4,1),(1019,'Vitara',3,1850,1851,4,1),(1020,'Wagon R',3,1852,1853,4,1),(1021,'Wagon R Plus',3,1854,1855,4,1),(1022,'XL7',3,1856,1857,4,1),(1023,'Другая',3,1858,1859,4,1),(1024,'Cynos',3,1862,1863,4,1),(1025,'Deliboy',3,1864,1865,4,1),(1026,'Duet',3,1866,1867,4,1),(1027,'Echo',3,1868,1869,4,1),(1028,'FJ Cruiser',3,1870,1871,4,1),(1029,'Fortuner',3,1872,1873,4,1),(1030,'Granvia',3,1874,1875,4,1),(1031,'Hiace',3,1876,1877,4,1),(1032,'Highlander',3,1878,1879,4,1),(1033,'Hilux Pick Up',3,1880,1881,4,1),(1034,'Hilux Surf',3,1882,1883,4,1),(1035,'Ist',3,1884,1885,4,1),(1036,'Kluger',3,1886,1887,4,1),(1037,'Land Cruiser 100',3,1888,1889,4,1),(1038,'Land Cruiser 105',3,1890,1891,4,1),(1039,'Land Cruiser 200',3,1892,1893,4,1),(1040,'Land Cruiser 40',3,1894,1895,4,1),(1041,'Land Cruiser 60',3,1896,1897,4,1),(1042,'Land Cruiser 70',3,1898,1899,4,1),(1043,'Land Cruiser 80',3,1900,1901,4,1),(1044,'Land Cruiser Prado 120',3,1902,1903,4,1),(1045,'Land Cruiser Prado 150',3,1904,1905,4,1),(1046,'Land Cruiser Prado 90',3,1906,1907,4,1),(1047,'Lite Ace',3,1908,1909,4,1),(1048,'MR 2',3,1910,1911,4,1),(1049,'Mark II',3,1912,1913,4,1),(1050,'Matrix',3,1914,1915,4,1),(1051,'Paseo',3,1916,1917,4,1),(1052,'Picnic',3,1918,1919,4,1),(1053,'Prado 70',3,1920,1921,4,1),(1054,'Previa',3,1922,1923,4,1),(1055,'Prius',3,1924,1925,4,1),(1056,'RAV 4',3,1926,1927,4,1),(1057,'Scion',3,1928,1929,4,1),(1058,'Sequoia',3,1930,1931,4,1),(1059,'Sienna',3,1932,1933,4,1),(1060,'Soarer',3,1934,1935,4,1),(1061,'Solara',3,1936,1937,4,1),(1062,'4runner',3,1938,1939,4,1),(1063,'Allion',3,1940,1941,4,1),(1064,'Sports',3,1942,1943,4,1),(1065,'Alphard',3,1944,1945,4,1),(1066,'Sprinter',3,1946,1947,4,1),(1067,'Starlet',3,1948,1949,4,1),(1068,'Aristo',3,1950,1951,4,1),(1069,'Aurion',3,1952,1953,4,1),(1070,'Auris',3,1954,1955,4,1),(1071,'Supra',3,1956,1957,4,1),(1072,'Avalon',3,1958,1959,4,1),(1073,'Tacoma',3,1960,1961,4,1),(1074,'Avensis',3,1962,1963,4,1),(1075,'Tercel',3,1964,1965,4,1),(1076,'Avensis Verso',3,1966,1967,4,1),(1077,'Town Ace',3,1968,1969,4,1),(1078,'Aygo',3,1970,1971,4,1),(1079,'Tundra',3,1972,1973,4,1),(1080,'Urban Cruiser',3,1974,1975,4,1),(1081,'Venza',3,1976,1977,4,1),(1082,'Verso',3,1978,1979,4,1),(1083,'Caldina',3,1980,1981,4,1),(1084,'Vista',3,1982,1983,4,1),(1085,'Camry',3,1984,1985,4,1),(1086,'Camry Solara',3,1986,1987,4,1),(1087,'Carib',3,1988,1989,4,1),(1088,'Carina',3,1990,1991,4,1),(1089,'Carina E',3,1992,1993,4,1),(1090,'Windom',3,1994,1995,4,1),(1091,'Carina ED',3,1996,1997,4,1),(1092,'Yaris',3,1998,1999,4,1),(1093,'Celica',3,2000,2001,4,1),(1094,'Yaris Verso',3,2002,2003,4,1),(1095,'iQ',3,2004,2005,4,1),(1096,'Другая',3,2006,2007,4,1),(1097,'Chaser',3,2008,2009,4,1),(1098,'Corolla',3,2010,2011,4,1),(1099,'Corolla FX',3,2012,2013,4,1),(1100,'Corolla Levin',3,2014,2015,4,1),(1101,'Corolla Verso',3,2016,2017,4,1),(1102,'Corona',3,2018,2019,4,1),(1103,'Corsa',3,2020,2021,4,1),(1104,'Cressida',3,2022,2023,4,1),(1105,'Cresta',3,2024,2025,4,1),(1106,'Crown',3,2026,2027,4,1),(1107,'Curren',3,2028,2029,4,1),(1108,'Golf Plus',3,2032,2033,4,1),(1109,'Iltis',3,2034,2035,4,1),(1110,'Jetta',3,2036,2037,4,1),(1111,'Kaefer',3,2038,2039,4,1),(1112,'LT',3,2040,2041,4,1),(1113,'Lupo',3,2042,2043,4,1),(1114,'Multivan',3,2044,2045,4,1),(1115,'Parati',3,2046,2047,4,1),(1116,'Passat CC',3,2048,2049,4,1),(1117,'Passat Hatchback',3,2050,2051,4,1),(1118,'Passat Sedan',3,2052,2053,4,1),(1119,'Passat Variant',3,2054,2055,4,1),(1120,'Phaeton',3,2056,2057,4,1),(1121,'Pointer',3,2058,2059,4,1),(1122,'Polo',3,2060,2061,4,1),(1123,'Santana',3,2062,2063,4,1),(1124,'Scirocco',3,2064,2065,4,1),(1125,'Sharan',3,2066,2067,4,1),(1126,'Taro',3,2068,2069,4,1),(1127,'Tiguan',3,2070,2071,4,1),(1128,'Touareg',3,2072,2073,4,1),(1129,'Touran',3,2074,2075,4,1),(1130,'Transporter',3,2076,2077,4,1),(1131,'Vento',3,2078,2079,4,1),(1132,'Другая',3,2080,2081,4,1),(1133,'1600',3,2082,2083,4,1),(1134,'Amarok',3,2084,2085,4,1),(1135,'Beetle',3,2086,2087,4,1),(1136,'Bora',3,2088,2089,4,1),(1137,'Caddy',3,2090,2091,4,1),(1138,'California',3,2092,2093,4,1),(1139,'Caravelle',3,2094,2095,4,1),(1140,'Corrado',3,2096,2097,4,1),(1141,'Crafter',3,2098,2099,4,1),(1142,'Derby',3,2100,2101,4,1),(1143,'Eos',3,2102,2103,4,1),(1144,'Fox',3,2104,2105,4,1),(1145,'Golf',3,2106,2107,4,1),(1146,'140',3,2110,2111,4,1),(1147,'164',3,2112,2113,4,1),(1148,'240',3,2114,2115,4,1),(1149,'340-360',3,2116,2117,4,1),(1150,'440 K',3,2118,2119,4,1),(1151,'460',3,2120,2121,4,1),(1152,'480 E',3,2122,2123,4,1),(1153,'740',3,2124,2125,4,1),(1154,'744',3,2126,2127,4,1),(1155,'760',3,2128,2129,4,1),(1156,'850',3,2130,2131,4,1),(1157,'940',3,2132,2133,4,1),(1158,'960',3,2134,2135,4,1),(1159,'C30',3,2136,2137,4,1),(1160,'C70',3,2138,2139,4,1),(1161,'S40',3,2140,2141,4,1),(1162,'S60',3,2142,2143,4,1),(1163,'S70',3,2144,2145,4,1),(1164,'S80',3,2146,2147,4,1),(1165,'S90',3,2148,2149,4,1),(1166,'V40 Kombi',3,2150,2151,4,1),(1167,'V50',3,2152,2153,4,1),(1168,'V60',3,2154,2155,4,1),(1169,'V70',3,2156,2157,4,1),(1170,'XC60',3,2158,2159,4,1),(1171,'XC70',3,2160,2161,4,1),(1172,'XC90',3,2162,2163,4,1),(1173,'Другая',3,2164,2165,4,1),(1174,'21108',3,2168,2169,4,1),(1175,'2111 Богдан',3,2170,2171,4,1),(1176,'21111',3,2172,2173,4,1),(1177,'21112',3,2174,2175,4,1),(1178,'21113',3,2176,2177,4,1),(1179,'21114',3,2178,2179,4,1),(1180,'21121',3,2180,2181,4,1),(1181,'21122',3,2182,2183,4,1),(1182,'21123',3,2184,2185,4,1),(1183,'21124',3,2186,2187,4,1),(1184,'2115i',3,2188,2189,4,1),(1185,'2120 Надежда',3,2190,2191,4,1),(1186,'21213',3,2192,2193,4,1),(1187,'21214',3,2194,2195,4,1),(1188,'21217',3,2196,2197,4,1),(1189,'21218',3,2198,2199,4,1),(1190,'2170 Priora Седан',3,2200,2201,4,1),(1191,'2171 Priora Универсал',3,2202,2203,4,1),(1192,'2172 Priora Хэтчбек',3,2204,2205,4,1),(1193,'Granta',3,2206,2207,4,1),(1194,'Largus',3,2208,2209,4,1),(1195,'Другая',3,2210,2211,4,1),(1196,'1111 Ока',3,2212,2213,4,1),(1197,'11113 Ока',3,2214,2215,4,1),(1198,'1117 Kalina Универсал',3,2216,2217,4,1),(1199,'1118 Kalina Седан',3,2218,2219,4,1),(1200,'1119 Kalina Sport',3,2220,2221,4,1),(1201,'1119 Kalina Хэтчбек',3,2222,2223,4,1),(1202,'2101',3,2224,2225,4,1),(1203,'2102',3,2226,2227,4,1),(1204,'2103',3,2228,2229,4,1),(1205,'2104',3,2230,2231,4,1),(1206,'2105',3,2232,2233,4,1),(1207,'2106',3,2234,2235,4,1),(1208,'2107',3,2236,2237,4,1),(1209,'2108',3,2238,2239,4,1),(1210,'2109',3,2240,2241,4,1),(1211,'2110',3,2242,2243,4,1),(1212,'2111',3,2244,2245,4,1),(1213,'2112',3,2246,2247,4,1),(1214,'2113',3,2248,2249,4,1),(1215,'2114',3,2250,2251,4,1),(1216,'2115',3,2252,2253,4,1),(1217,'2121',3,2254,2255,4,1),(1218,'2123',3,2256,2257,4,1),(1219,'2131',3,2258,2259,4,1),(1220,'2328',3,2260,2261,4,1),(1221,'2345',3,2262,2263,4,1),(1222,'2723',3,2264,2265,4,1),(1223,'21043',3,2266,2267,4,1),(1224,'21045',3,2268,2269,4,1),(1225,'21047',3,2270,2271,4,1),(1226,'21053',3,2272,2273,4,1),(1227,'21055',3,2274,2275,4,1),(1228,'21059',3,2276,2277,4,1),(1229,'21061',3,2278,2279,4,1),(1230,'21063',3,2280,2281,4,1),(1231,'21065',3,2282,2283,4,1),(1232,'21071',3,2284,2285,4,1),(1233,'21072',3,2286,2287,4,1),(1234,'21073',3,2288,2289,4,1),(1235,'21074',3,2290,2291,4,1),(1236,'21079',3,2292,2293,4,1),(1237,'21081',3,2294,2295,4,1),(1238,'21083',3,2296,2297,4,1),(1239,'21086',3,2298,2299,4,1),(1240,'2108i',3,2300,2301,4,1),(1241,'21091',3,2302,2303,4,1),(1242,'21093',3,2304,2305,4,1),(1243,'21096',3,2306,2307,4,1),(1244,'21099',3,2308,2309,4,1),(1245,'21099i',3,2310,2311,4,1),(1246,'2109i',3,2312,2313,4,1),(1247,'2110 Богдан',3,2314,2315,4,1),(1248,'21101',3,2316,2317,4,1),(1249,'21102',3,2318,2319,4,1),(1250,'21103',3,2320,2321,4,1),(1251,'21104',3,2322,2323,4,1),(1252,'21106',3,2324,2325,4,1),(1253,'12 ЗИМ',3,2328,2329,4,1),(1254,'13',3,2330,2331,4,1),(1255,'20',3,2332,2333,4,1),(1256,'21',3,2334,2335,4,1),(1257,'22',3,2336,2337,4,1),(1258,'2217 Соболь/Баргузин',3,2338,2339,4,1),(1259,'2308 Атаман',3,2340,2341,4,1),(1260,'2310 Соболь',3,2342,2343,4,1),(1261,'24',3,2344,2345,4,1),(1262,'24-10 Волга',3,2346,2347,4,1),(1263,'2752 Соболь',3,2348,2349,4,1),(1264,'2818 Газель',3,2350,2351,4,1),(1265,'67',3,2352,2353,4,1),(1266,'69',3,2354,2355,4,1),(1267,'2790',3,2356,2357,4,1),(1268,'3102',3,2358,2359,4,1),(1269,'3105',3,2360,2361,4,1),(1270,'3110',3,2362,2363,4,1),(1271,'3111',3,2364,2365,4,1),(1272,'3310',3,2366,2367,4,1),(1273,'31022',3,2368,2369,4,1),(1274,'31029',3,2370,2371,4,1),(1275,'31105',3,2372,2373,4,1),(1276,'310221',3,2374,2375,4,1),(1277,'3102i',3,2376,2377,4,1),(1278,'3110i',3,2378,2379,4,1),(1279,'3221 Газель',3,2380,2381,4,1),(1280,'3302 Газель',3,2382,2383,4,1),(1281,'M-20 Победа',3,2384,2385,4,1),(1282,'Другая',3,2386,2387,4,1),(1283,'965',3,2390,2391,4,1),(1284,'966',3,2392,2393,4,1),(1285,'968',3,2394,2395,4,1),(1286,'1102',3,2396,2397,4,1),(1287,'1103 Slavuta',3,2398,2399,4,1),(1288,'1105',3,2400,2401,4,1),(1289,'1125',3,2402,2403,4,1),(1290,'1140',3,2404,2405,4,1),(1291,'Chance',3,2406,2407,4,1),(1292,'Forza',3,2408,2409,4,1),(1293,'Lanos',3,2410,2411,4,1),(1294,'Nova',3,2412,2413,4,1),(1295,'Sens',3,2414,2415,4,1),(1296,'Vida',3,2416,2417,4,1),(1297,'Другая',3,2418,2419,4,1),(1298,'412',3,2422,2423,4,1),(1299,'2117',3,2424,2425,4,1),(1300,'2125',3,2426,2427,4,1),(1301,'2126',3,2428,2429,4,1),(1302,'2715',3,2430,2431,4,1),(1303,'2717',3,2432,2433,4,1),(1304,'21261',3,2434,2435,4,1),(1305,'27171',3,2436,2437,4,1),(1306,'Другая',3,2438,2439,4,1),(1307,'2141-02 Святогор',3,2442,2443,4,1),(1308,'2142R5 Князь Владимир',3,2444,2445,4,1),(1309,'400',3,2446,2447,4,1),(1310,'401',3,2448,2449,4,1),(1311,'402',3,2450,2451,4,1),(1312,'403',3,2452,2453,4,1),(1313,'407',3,2454,2455,4,1),(1314,'408',3,2456,2457,4,1),(1315,'410',3,2458,2459,4,1),(1316,'412',3,2460,2461,4,1),(1317,'423',3,2462,2463,4,1),(1318,'426',3,2464,2465,4,1),(1319,'427',3,2466,2467,4,1),(1320,'2125',3,2468,2469,4,1),(1321,'2135',3,2470,2471,4,1),(1322,'2137',3,2472,2473,4,1),(1323,'2138',3,2474,2475,4,1),(1324,'2140',3,2476,2477,4,1),(1325,'2141',3,2478,2479,4,1),(1326,'2901',3,2480,2481,4,1),(1327,'31512',3,2486,2487,4,1),(1328,'31514',3,2488,2489,4,1),(1329,'31517',3,2490,2491,4,1),(1330,'31519',3,2492,2493,4,1),(1331,'315195 Hunter',3,2494,2495,4,1),(1332,'3163 Patriot',3,2496,2497,4,1),(1333,'Другая',3,2498,2499,4,1),(1334,'2206',3,2500,2501,4,1),(1335,'2317',3,2502,2503,4,1),(1336,'2360',3,2504,2505,4,1),(1337,'2363 Pickup',3,2506,2507,4,1),(1338,'3151',3,2508,2509,4,1),(1339,'315108 Hunter',3,2510,2511,4,1),(1340,'459',3,2512,2513,4,1),(1341,'469',3,2514,2515,4,1),(1342,'3153',3,2516,2517,4,1),(1343,'3159',3,2518,2519,4,1),(1344,'3160',3,2520,2521,4,1),(1345,'3162',3,2522,2523,4,1),(1346,'3303',3,2524,2525,4,1),(1347,'3309',3,2526,2527,4,1),(1348,'3741',3,2528,2529,4,1),(1349,'3909',3,2530,2531,4,1),(1350,'3962',3,2532,2533,4,1),(1351,'Мопеды / скутеры',3,2539,2622,3,3),(1352,'Квадроциклы',3,2623,2678,3,3),(1353,'Мотоциклы',3,2679,2742,3,3),(1354,'Мото - прочее',3,2743,2744,3,3),(1355,'ABM',3,2540,2541,4,3),(1356,'Aprilia',3,2542,2543,4,3),(1357,'ASA',3,2544,2545,4,3),(1358,'Atlant',3,2546,2547,4,3),(1359,'Baotian Scooters',3,2548,2549,4,3),(1360,'Benelli',3,2550,2551,4,3),(1361,'BM',3,2552,2553,4,3),(1362,'Corsa',3,2554,2555,4,3),(1363,'CPI',3,2556,2557,4,3),(1364,'Daelim',3,2558,2559,4,3),(1365,'Derbi',3,2560,2561,4,3),(1366,'Forsage',3,2562,2563,4,3),(1367,'Geely',3,2564,2565,4,3),(1368,'Gilera',3,2566,2567,4,3),(1369,'GX moto',3,2568,2569,4,3),(1370,'Honda',3,2570,2571,4,3),(1371,'Honling',3,2572,2573,4,3),(1372,'IRBIS',3,2574,2575,4,3),(1373,'Italjet',3,2576,2577,4,3),(1374,'Jialing',3,2578,2579,4,3),(1375,'Keeway',3,2580,2581,4,3),(1376,'Kinlon',3,2582,2583,4,3),(1377,'Kymco',3,2584,2585,4,3),(1378,'Lifan',3,2586,2587,4,3),(1379,'Malaguti',3,2588,2589,4,3),(1380,'Nexus',3,2590,2591,4,3),(1381,'Peugeot',3,2592,2593,4,3),(1382,'Piaggio',3,2594,2595,4,3),(1383,'Sagitta',3,2596,2597,4,3),(1384,'Stels',3,2598,2599,4,3),(1385,'Suzuki',3,2600,2601,4,3),(1386,'Sym',3,2602,2603,4,3),(1387,'Tornado',3,2604,2605,4,3),(1388,'Vento',3,2606,2607,4,3),(1389,'Vespa',3,2608,2609,4,3),(1390,'Yamaha',3,2610,2611,4,3),(1391,'Вятка',3,2612,2613,4,3),(1392,'ЗиД',3,2614,2615,4,3),(1393,'Карпаты',3,2616,2617,4,3),(1394,'Рига',3,2618,2619,4,3),(1395,'Другая марка',3,2620,2621,4,3),(1396,'ABM',3,2624,2625,4,3),(1397,'Arctic Cat',3,2626,2627,4,3),(1398,'Armada',3,2628,2629,4,3),(1399,'BASHAN',3,2630,2631,4,3),(1400,'BM',3,2632,2633,4,3),(1401,'BRP',3,2634,2635,4,3),(1402,'Dinli',3,2636,2637,4,3),(1403,'Forsage',3,2638,2639,4,3),(1404,'Godzilla',3,2640,2641,4,3),(1405,'Honda',3,2642,2643,4,3),(1406,'HSUN',3,2644,2645,4,3),(1407,'IRBIS',3,2646,2647,4,3),(1408,'Jianshe-Yamaha',3,2648,2649,4,3),(1409,'Kawasaki',3,2650,2651,4,3),(1410,'Kazuma',3,2652,2653,4,3),(1411,'Kymco',3,2654,2655,4,3),(1412,'Nissamaran',3,2656,2657,4,3),(1413,'Polaris',3,2658,2659,4,3),(1414,'Reggy',3,2660,2661,4,3),(1415,'Stels',3,2662,2663,4,3),(1416,'Suzuki',3,2664,2665,4,3),(1417,'Tramp',3,2666,2667,4,3),(1418,'Yamaha',3,2668,2669,4,3),(1419,'ЗиД',3,2670,2671,4,3),(1420,'Русская Механика',3,2672,2673,4,3),(1421,'ТМЗ',3,2674,2675,4,3),(1422,'Другая марка',3,2676,2677,4,3),(1423,'Regal-Raptor',3,2680,2681,4,3),(1424,'Suzuki',3,2682,2683,4,3),(1425,'Triumph',3,2684,2685,4,3),(1426,'Yamaha',3,2686,2687,4,3),(1427,'Днепр',3,2688,2689,4,3),(1428,'ЗИД',3,2690,2691,4,3),(1429,'Иж',3,2692,2693,4,3),(1430,'Минск',3,2694,2695,4,3),(1431,'Тула',3,2696,2697,4,3),(1432,'Урал',3,2698,2699,4,3),(1433,'Другая марка',3,2700,2701,4,3),(1434,'Aprilia',3,2702,2703,4,3),(1435,'Blata',3,2704,2705,4,3),(1436,'BM',3,2706,2707,4,3),(1437,'BMW',3,2708,2709,4,3),(1438,'Buell',3,2710,2711,4,3),(1439,'Cagiva',3,2712,2713,4,3),(1440,'CPI',3,2714,2715,4,3),(1441,'CZ',3,2716,2717,4,3),(1442,'Ducati',3,2718,2719,4,3),(1443,'Harley-Davidson',3,2720,2721,4,3),(1444,'Honda',3,2722,2723,4,3),(1445,'Husqvarna',3,2724,2725,4,3),(1446,'Hyosung',3,2726,2727,4,3),(1447,'Jawa',3,2728,2729,4,3),(1448,'Kawasaki',3,2730,2731,4,3),(1449,'KTM',3,2732,2733,4,3),(1450,'Kymco',3,2734,2735,4,3),(1451,'Moto Guzzi',3,2736,2737,4,3),(1452,'MV Agusta',3,2738,2739,4,3),(1453,'Pannonia',3,2740,2741,4,3),(1454,'ПАЗ',3,2747,2748,3,11),(1455,'РАФ',3,2749,2750,3,11),(1456,'Другая марка',3,2751,2752,3,11),(1457,'Bova',3,2753,2754,3,11),(1458,'Daewoo',3,2755,2756,3,11),(1459,'Ford',3,2757,2758,3,11),(1460,'Golden Dragon',3,2759,2760,3,11),(1461,'Higer',3,2761,2762,3,11),(1462,'Hyundai',3,2763,2764,3,11),(1463,'Ikarus',3,2765,2766,3,11),(1464,'IVECO',3,2767,2768,3,11),(1465,'Kia',3,2769,2770,3,11),(1466,'King Long',3,2771,2772,3,11),(1467,'MAN',3,2773,2774,3,11),(1468,'Mercedes-Benz',3,2775,2776,3,11),(1469,'Neoplan',3,2777,2778,3,11),(1470,'Nissan',3,2779,2780,3,11),(1471,'Scania',3,2781,2782,3,11),(1472,'Setra',3,2783,2784,3,11),(1473,'Volvo',3,2785,2786,3,11),(1474,'Yutong',3,2787,2788,3,11),(1475,'Богдан',3,2789,2790,3,11),(1476,'Волжанин',3,2791,2792,3,11),(1477,'ГАЗ',3,2793,2794,3,11),(1478,'ЗИЛ',3,2795,2796,3,11),(1479,'КАвЗ',3,2797,2798,3,11),(1480,'ЛАЗ',3,2799,2800,3,11),(1481,'ЛиАЗ',3,2801,2802,3,11),(1482,'МАЗ',3,2803,2804,3,11),(1483,'МАРЗ',3,2805,2806,3,11),(1484,'Бульдозеры / тракторы',3,2809,2810,3,11),(1485,'Коммунальная техника',3,2811,2812,3,11),(1486,'Погрузчики',3,2813,2814,3,11),(1487,'Самосвалы',3,2815,2816,3,11),(1488,'Строительная техника',3,2817,2818,3,11),(1489,'Экскаваторы',3,2819,2820,3,11),(1490,'Оборудование для спецтехники',3,2821,2822,3,11),(1491,'Прочая спецтехника',3,2823,2824,3,11),(1492,'Avia',3,2827,2828,3,2),(1493,'BAW',3,2829,2830,3,2),(1494,'CAMC',3,2831,2832,3,2),(1495,'Daewoo',3,2833,2834,3,2),(1496,'DAF',3,2835,2836,3,2),(1497,'DongFeng',3,2837,2838,3,2),(1498,'FAW',3,2839,2840,3,2),(1499,'Ford',3,2841,2842,3,2),(1500,'Foton',3,2843,2844,3,2),(1501,'Freightliner',3,2845,2846,3,2),(1502,'HOWO',3,2847,2848,3,2),(1503,'Hyundai',3,2849,2850,3,2),(1504,'IFA',3,2851,2852,3,2),(1505,'International',3,2853,2854,3,2),(1506,'Isuzu',3,2855,2856,3,2),(1507,'IVECO',3,2857,2858,3,2),(1508,'Kia',3,2859,2860,3,2),(1509,'MAN',3,2861,2862,3,2),(1510,'Mazda',3,2863,2864,3,2),(1511,'Mercedes',3,2865,2866,3,2),(1512,'Mitsubishi',3,2867,2868,3,2),(1513,'Nissan',3,2869,2870,3,2),(1514,'Renault',3,2871,2872,3,2),(1515,'Scania',3,2873,2874,3,2),(1516,'Shaanxi-MAN',3,2875,2876,3,2),(1517,'Tata',3,2877,2878,3,2),(1518,'Tatra',3,2879,2880,3,2),(1519,'YueJin',3,2881,2882,3,2),(1520,'Volvo',3,2883,2884,3,2),(1521,'ГАЗ',3,2885,2886,3,2),(1522,'ЗИЛ',3,2887,2888,3,2),(1523,'КАМАЗ',3,2889,2890,3,2),(1524,'КРАЗ',3,2891,2892,3,2),(1525,'МАЗ',3,2893,2894,3,2),(1526,'УРАЛ',3,2895,2896,3,2),(1527,'Другая марка',3,2897,2898,3,2),(1528,'Автозапчасти',3,2907,2908,3,9),(1529,'Аксессуары для авто',3,2909,2910,3,9),(1530,'Мото аксессуары',3,2911,2974,3,9),(1531,'Мотозапчасти',3,2975,2976,3,9),(1532,'Шины / диски',3,2977,2978,3,9),(1533,'Автозвук',3,2979,2988,3,9),(1534,'GPS-навигаторы / авторегистраторы',3,2989,3076,3,9),(1535,'Запчасти для спец / с.х. техники',3,3077,3078,3,9),(1536,'Транспорт на запчасти',3,3079,3080,3,9),(1537,'Прочие запчасти',3,3081,3082,3,9),(1538,'Regal-Raptor',3,2912,2913,4,9),(1539,'Suzuki',3,2914,2915,4,9),(1540,'Triumph',3,2916,2917,4,9),(1541,'Yamaha',3,2918,2919,4,9),(1542,'Днепр',3,2920,2921,4,9),(1543,'ЗИД',3,2922,2923,4,9),(1544,'Иж',3,2924,2925,4,9),(1545,'Минск',3,2926,2927,4,9),(1546,'Тула',3,2928,2929,4,9),(1547,'Урал',3,2930,2931,4,9),(1548,'Другая марка',3,2932,2933,4,9),(1549,'Aprilia',3,2934,2935,4,9),(1550,'Blata',3,2936,2937,4,9),(1551,'BM',3,2938,2939,4,9),(1552,'BMW',3,2940,2941,4,9),(1553,'Buell',3,2942,2943,4,9),(1554,'Cagiva',3,2944,2945,4,9),(1555,'CPI',3,2946,2947,4,9),(1556,'CZ',3,2948,2949,4,9),(1557,'Ducati',3,2950,2951,4,9),(1558,'Harley-Davidson',3,2952,2953,4,9),(1559,'Honda',3,2954,2955,4,9),(1560,'Husqvarna',3,2956,2957,4,9),(1561,'Hyosung',3,2958,2959,4,9),(1562,'Jawa',3,2960,2961,4,9),(1563,'Kawasaki',3,2962,2963,4,9),(1564,'KTM',3,2964,2965,4,9),(1565,'Kymco',3,2966,2967,4,9),(1566,'Moto Guzzi',3,2968,2969,4,9),(1567,'MV Agusta',3,2970,2971,4,9),(1568,'Pannonia',3,2972,2973,4,9),(1569,'Магнитолы',3,2980,2981,4,9),(1570,'Колонки / сабвуферы',3,2982,2983,4,9),(1571,'Усилители',3,2984,2985,4,9),(1572,'Прочий автозвук',3,2986,2987,4,9),(1573,'Другая марка',3,2990,2991,4,9),(1574,'Asus',3,2992,2993,4,9),(1575,'Altina',3,2994,2995,4,9),(1576,'Atomy',3,2996,2997,4,9),(1577,'AutoNavi',3,2998,2999,4,9),(1578,'Becker',3,3000,3001,4,9),(1579,'Carman i',3,3002,3003,4,9),(1580,'Clarion',3,3004,3005,4,9),(1581,'EasyGo',3,3006,3007,4,9),(1582,'Element',3,3008,3009,4,9),(1583,'Explay',3,3010,3011,4,9),(1584,'Garmin',3,3012,3013,4,9),(1585,'Global Navigation',3,3014,3015,4,9),(1586,'Globalsat',3,3016,3017,4,9),(1587,'GlobusGPS',3,3018,3019,4,9),(1588,'Globway',3,3020,3021,4,9),(1589,'Goclever',3,3022,3023,4,9),(1590,'Holux',3,3024,3025,4,9),(1591,'iSUN',3,3026,3027,4,9),(1592,'JJ-Connect',3,3028,3029,4,9),(1593,'Lexand',3,3030,3031,4,9),(1594,'Magellan',3,3032,3033,4,9),(1595,'Mio',3,3034,3035,4,9),(1596,'Mystery',3,3036,3037,4,9),(1597,'Navigon',3,3038,3039,4,9),(1598,'Navistar',3,3040,3041,4,9),(1599,'Navitel',3,3042,3043,4,9),(1600,'NaviTop',3,3044,3045,4,9),(1601,'NEC',3,3046,3047,4,9),(1602,'Neoline',3,3048,3049,4,9),(1603,'Nexx',3,3050,3051,4,9),(1604,'OODO',3,3052,3053,4,9),(1605,'Orion',3,3054,3055,4,9),(1606,'Packard Bell',3,3056,3057,4,9),(1607,'Pioneer',3,3058,3059,4,9),(1608,'Prestigio',3,3060,3061,4,9),(1609,'Qstarz',3,3062,3063,4,9),(1610,'Tenex',3,3064,3065,4,9),(1611,'TeXet',3,3066,3067,4,9),(1612,'TomTom',3,3068,3069,4,9),(1613,'Treelogic',3,3070,3071,4,9),(1614,'Unistar',3,3072,3073,4,9),(1615,'xDevice',3,3074,3075,4,9),(1616,'Розничная торговля / Продажи',4,2,3,2,8),(1617,'Транспорт / логистика',4,4,5,2,8),(1618,'Строительство',4,6,7,2,8),(1619,'Бары / рестораны',4,8,9,2,8),(1620,'Юриспруденция и бухгалтерия',4,10,11,2,8),(1621,'Охрана / безопасность',4,12,13,2,8),(1622,'Домашний персонал',4,14,15,2,8),(1623,'Красота / фитнес / спорт',4,16,17,2,8),(1624,'Туризм / отдых / развлечения',4,18,19,2,8),(1625,'Образование',4,20,21,2,8),(1626,'Культура / искусство',4,22,23,2,8),(1627,'Медицина / фармация',4,24,25,2,8),(1628,'ИТ / телеком / компьютеры',4,26,27,2,8),(1629,'Недвижимость',4,28,29,2,8),(1630,'Маркетинг / реклама / дизайн',4,30,31,2,8),(1631,'Производство / энергетика',4,32,33,2,8),(1632,'Cекретариат / АХО',4,34,35,2,8),(1633,'Начало карьеры / Студенты',4,36,37,2,8),(1634,'Сервис и быт',4,38,39,2,8),(1635,'Другие сферы занятий',4,40,41,2,8),(1636,'Собаки',5,2,153,2,11),(1637,'Кошки',5,154,215,2,11),(1638,'Аквариумистика',5,216,217,2,11),(1639,'Птицы',5,218,219,2,11),(1640,'Грызуны',5,220,221,2,11),(1641,'Рептилии',5,222,223,2,11),(1642,'Сельхоз животные',5,224,225,2,11),(1643,'Зоотовары',5,226,227,2,11),(1644,'Вязка',5,228,229,2,11),(1645,'Бюро находок',5,230,231,2,11),(1646,'Другие  животные',5,232,233,2,11),(1647,'Эрдельтерьер',5,3,4,3,11),(1648,'Японский хин',5,5,6,3,11),(1649,'Другая',5,7,8,3,11),(1650,'Акита',5,9,10,3,11),(1651,'Аляскинский маламут',5,11,12,3,11),(1652,'Американский бульдог',5,13,14,3,11),(1653,'Английский бульдог',5,15,16,3,11),(1654,'Бассет',5,17,18,3,11),(1655,'Бельгийская овчарка',5,19,20,3,11),(1656,'Бернский зенненхунд',5,21,22,3,11),(1657,'Бигль',5,23,24,3,11),(1658,'Бишон фризе',5,25,26,3,11),(1659,'Боксер',5,27,28,3,11),(1660,'Болонка',5,29,30,3,11),(1661,'Бордоский дог',5,31,32,3,11),(1662,'Бульмастиф',5,33,34,3,11),(1663,'Бультерьер',5,35,36,3,11),(1664,'Бурбуль',5,37,38,3,11),(1665,'Вест хайленд уайт терьер',5,39,40,3,11),(1666,'Восточно-европейская овчарка',5,41,42,3,11),(1667,'Гриффон',5,43,44,3,11),(1668,'Далматин',5,45,46,3,11),(1669,'Джек Рассел',5,47,48,3,11),(1670,'Доберман',5,49,50,3,11),(1671,'Золотистый ретривер',5,51,52,3,11),(1672,'Ирландский сеттер',5,53,54,3,11),(1673,'Ирландский терьер',5,55,56,3,11),(1674,'Йоркширский терьер',5,57,58,3,11),(1675,'Кавказская овчарка',5,59,60,3,11),(1676,'Кане корсо',5,61,62,3,11),(1677,'Карликовый пинчер',5,63,64,3,11),(1678,'Керри-блю терьер',5,65,66,3,11),(1679,'Китайская хохлатая',5,67,68,3,11),(1680,'Колли',5,69,70,3,11),(1681,'Лабрадор ретривер',5,71,72,3,11),(1682,'Лайка',5,73,74,3,11),(1683,'Мальтийская болонка',5,75,76,3,11),(1684,'Мастиф',5,77,78,3,11),(1685,'Мексиканская голая собака',5,79,80,3,11),(1686,'Миттельшнауцер',5,81,82,3,11),(1687,'Мопс',5,83,84,3,11),(1688,'Московская сторожевая',5,85,86,3,11),(1689,'Немецкая овчарка',5,87,88,3,11),(1690,'Немецкий дог',5,89,90,3,11),(1691,'Папийон',5,91,92,3,11),(1692,'Пекинес',5,93,94,3,11),(1693,'Пит-бультерьер',5,95,96,3,11),(1694,'Померанский шпиц',5,97,98,3,11),(1695,'Пудель',5,99,100,3,11),(1696,'Ризеншнауцер',5,101,102,3,11),(1697,'Родезийский риджбек',5,103,104,3,11),(1698,'Ротвейлер',5,105,106,3,11),(1699,'Русская борзая',5,107,108,3,11),(1700,'Русский черный терьер',5,109,110,3,11),(1701,'Самоедская собака',5,111,112,3,11),(1702,'Сенбернар',5,113,114,3,11),(1703,'Сибирский хаски',5,115,116,3,11),(1704,'Скай-терьер',5,117,118,3,11),(1705,'Скотч-терьер',5,119,120,3,11),(1706,'Спаниель',5,121,122,3,11),(1707,'Среднеазиатская овчарка',5,123,124,3,11),(1708,'Стаффордширский бультерьер',5,125,126,3,11),(1709,'Стаффордширский терьер',5,127,128,3,11),(1710,'Такса',5,129,130,3,11),(1711,'Той-терьер',5,131,132,3,11),(1712,'Фокстерьер',5,133,134,3,11),(1713,'Французский бульдог',5,135,136,3,11),(1714,'Цвергпинчер',5,137,138,3,11),(1715,'Цвергшнауцер',5,139,140,3,11),(1716,'Чау-чау',5,141,142,3,11),(1717,'Чихуахуа',5,143,144,3,11),(1718,'Шарпей',5,145,146,3,11),(1719,'Шелти',5,147,148,3,11),(1720,'Ши-тцу',5,149,150,3,11),(1721,'Шпиц',5,151,152,3,11),(1722,'Абиссинская',5,155,156,3,11),(1723,'Американский керл',5,157,158,3,11),(1724,'Бенгальская',5,159,160,3,11),(1725,'Британская длинношерстная',5,161,162,3,11),(1726,'Британская короткошерстная',5,163,164,3,11),(1727,'Девон-рекс',5,165,166,3,11),(1728,'Донской сфинкс',5,167,168,3,11),(1729,'Европейская короткошерстная',5,169,170,3,11),(1730,'Канадский сфинкс',5,171,172,3,11),(1731,'Корниш-рекс',5,173,174,3,11),(1732,'Курильский бобтейл',5,175,176,3,11),(1733,'Манчкин',5,177,178,3,11),(1734,'Мейн-кун',5,179,180,3,11),(1735,'Меконгский бобтейл',5,181,182,3,11),(1736,'Невская маскарадная',5,183,184,3,11),(1737,'Норвежская лесная',5,185,186,3,11),(1738,'Ориентальная',5,187,188,3,11),(1739,'Персидская',5,189,190,3,11),(1740,'Петерболд',5,191,192,3,11),(1741,'Русская голубая',5,193,194,3,11),(1742,'Селкирк-рекс',5,195,196,3,11),(1743,'Сиамская',5,197,198,3,11),(1744,'Сибирская',5,199,200,3,11),(1745,'Скоттиш страйт',5,201,202,3,11),(1746,'Скоттиш фолд',5,203,204,3,11),(1747,'Сомалийская',5,205,206,3,11),(1748,'Турецкая ангора',5,207,208,3,11),(1749,'Экзотическая короткошерстная',5,209,210,3,11),(1750,'Японский бобтейл',5,211,212,3,11),(1751,'Другая',5,213,214,3,11),(1752,'Канцтовары / расходные материалы',6,2,3,2,9),(1753,'Мебель',6,4,47,2,9),(1754,'Продукты питания / напитки',6,48,49,2,11),(1755,'Сад / огород',6,50,51,2,11),(1756,'Предметы интерьера',6,52,71,2,9),(1757,'Строительство / ремонт',6,72,97,2,9),(1758,'Инструменты',6,98,109,2,9),(1759,'Комнатные растения',6,110,111,2,11),(1760,'Посуда / кухонная утварь',6,112,113,2,9),(1761,'Садовый инвентарь',6,114,115,2,9),(1762,'Хозяйственный инвентарь / бытовая химия',6,116,117,2,9),(1763,'Прочие товары для дома',6,118,119,2,9),(1764,'Мебель для гостиной',6,5,14,3,9),(1765,'Мебель для спальни',6,15,28,3,9),(1766,'Мебель для прихожей',6,29,30,3,9),(1767,'Кухонная мебель',6,31,32,3,9),(1768,'Мебель для ванной комнаты',6,33,34,3,9),(1769,'Офисная мебель',6,35,44,3,9),(1770,'Мебель на заказ',6,45,46,3,9),(1771,'Мягкая мебель',6,6,7,4,9),(1772,'Шкафы / стенки',6,8,9,4,9),(1773,'Столы / стулья',6,10,11,4,9),(1774,'Прочая мебель для гостиной',6,12,13,4,9),(1775,'Гарнитуры',6,16,17,4,9),(1776,'Кровати',6,18,19,4,9),(1777,'Матрасы',6,20,21,4,9),(1778,'Комоды / тумбы',6,22,23,4,9),(1779,'Шкафы',6,24,25,4,9),(1780,'Прочая мебель для спальни',6,26,27,4,9),(1781,'Столы',6,36,37,4,9),(1782,'Кресла / стулья',6,38,39,4,9),(1783,'Шкафы / полки',6,40,41,4,9),(1784,'Прочая офисная мебель',6,42,43,4,9),(1785,'Светильники',6,53,54,3,9),(1786,'Текстиль',6,55,62,3,9),(1787,'Декор окон',6,63,70,3,9),(1788,'Подушки и одеяла',6,56,57,4,9),(1789,'Постельное белье',6,58,59,4,9),(1790,'Полотенца',6,60,61,4,9),(1791,'Шторы',6,64,65,4,9),(1792,'Жалюзи',6,66,67,4,9),(1793,'Карнизы',6,68,69,4,9),(1794,'Сантехника',6,73,74,3,9),(1795,'Вентиляция',6,75,76,3,9),(1796,'Отопление',6,77,78,3,9),(1797,'Электрика',6,79,80,3,9),(1798,'Пиломатериалы',6,81,82,3,9),(1799,'Отделочные и облицовочные материалы',6,83,84,3,9),(1800,'Окна / двери / стеклo / зеркала',6,85,86,3,9),(1801,'Лакокрасочные материалы',6,87,88,3,9),(1802,'Металлопрокат / арматура',6,89,90,3,9),(1803,'Элементы крепежа',6,91,92,3,9),(1804,'Кирпич / бетон / пеноблоки',6,93,94,3,9),(1805,'Прочие стройматериалы',6,95,96,3,9),(1806,'Ручной инструмент',6,99,100,3,9),(1807,'Бензоинструмент',6,101,102,3,9),(1808,'Электроинструмент',6,103,104,3,9),(1809,'Пневмоинструмент',6,105,106,3,9),(1810,'Прочий инструмент',6,107,108,3,9),(1811,'Телефоны',7,2,135,2,9),(1812,'Компьютеры',7,136,325,2,9),(1813,'Фото / видео',7,326,487,2,9),(1814,'Тв / видеотехника',7,488,711,2,9),(1815,'Аудиотехника',7,712,1059,2,9),(1816,'Игры и игровые приставки',7,1060,1097,2,9),(1817,'Техника для дома',7,1098,1481,2,9),(1818,'Техника для кухни',7,1482,2113,2,9),(1819,'Климатическое оборудование',7,2114,2115,2,9),(1820,'Индивидуальный уход',7,2116,2125,2,9),(1821,'Аксессуары и комплектующие',7,2126,2127,2,9),(1822,'Прочая электроника',7,2128,2129,2,9),(1823,'Аксессуары / запчасти',7,3,18,3,9),(1824,'Мобильные телефоны',7,19,122,3,9),(1825,'Сим-карты / тарифы / номера',7,123,124,3,9),(1826,'Стационарные телефоны',7,125,130,3,9),(1827,'Ремонт / прошивка',7,131,132,3,9),(1828,'Прочие телефоны',7,133,134,3,9),(1829,'Аккумуляторы',7,4,5,4,9),(1830,'Зарядные устройства',7,6,7,4,9),(1831,'Гарнитуры',7,8,9,4,9),(1832,'Корпуса / панели',7,10,11,4,9),(1833,'Чехлы',7,12,13,4,9),(1834,'Data-кабели',7,14,15,4,9),(1835,'Прочие аксессуары для телефонов',7,16,17,4,9),(1836,'HTC',7,20,21,4,9),(1837,'Huawei',7,22,23,4,9),(1838,'i-mate',7,24,25,4,9),(1839,'Lenovo',7,26,27,4,9),(1840,'LG',7,28,29,4,9),(1841,'Meizu',7,30,31,4,9),(1842,'Mio',7,32,33,4,9),(1843,'Mobiado',7,34,35,4,9),(1844,'Motorola',7,36,37,4,9),(1845,'NEC',7,38,39,4,9),(1846,'Nokia',7,40,41,4,9),(1847,'ORSiO',7,42,43,4,9),(1848,'Palm',7,44,45,4,9),(1849,'Panasonic',7,46,47,4,9),(1850,'Pantech',7,48,49,4,9),(1851,'Philips',7,50,51,4,9),(1852,'RIM',7,52,53,4,9),(1853,'Rover',7,54,55,4,9),(1854,'Sagem',7,56,57,4,9),(1855,'Samsung',7,58,59,4,9),(1856,'Sharp',7,60,61,4,9),(1857,'Siemens',7,62,63,4,9),(1858,'Sony',7,64,65,4,9),(1859,'Sony Ericsson',7,66,67,4,9),(1860,'Toshiba',7,68,69,4,9),(1861,'Anycool',7,70,71,4,9),(1862,'Ubiquam',7,72,73,4,9),(1863,'Vertu',7,74,75,4,9),(1864,'Voxtel',7,76,77,4,9),(1865,'xDevice',7,78,79,4,9),(1866,'Другая марка',7,80,81,4,9),(1867,'Acer',7,82,83,4,9),(1868,'Alcatel-Lucent',7,84,85,4,9),(1869,'Anydata',7,86,87,4,9),(1870,'Apple',7,88,89,4,9),(1871,'ASUS',7,90,91,4,9),(1872,'BBK',7,92,93,4,9),(1873,'BenQ-Siemens',7,94,95,4,9),(1874,'BlackBerry',7,96,97,4,9),(1875,'Daewoo',7,98,99,4,9),(1876,'E-TEN',7,100,101,4,9),(1877,'Fly',7,102,103,4,9),(1878,'Fujitsu Siemens',7,104,105,4,9),(1879,'GIGABYTE',7,106,107,4,9),(1880,'Google',7,108,109,4,9),(1881,'Gresso',7,110,111,4,9),(1882,'Haier',7,112,113,4,9),(1883,'Helio',7,114,115,4,9),(1884,'Highscreen',7,116,117,4,9),(1885,'Hisense',7,118,119,4,9),(1886,'HP',7,120,121,4,9),(1887,'Проводные телефоны',7,126,127,4,9),(1888,'Радиотелефоны',7,128,129,4,9),(1889,'Настольные',7,137,138,3,9),(1890,'Ноутбуки',7,139,190,3,9),(1891,'Планшетные компьютеры',7,191,192,3,9),(1892,'Серверы',7,193,194,3,9),(1893,'Аксессуары',7,195,206,3,9),(1894,'Комплектующие',7,207,230,3,9),(1895,'Периферийные устройства',7,231,250,3,9),(1896,'Мониторы',7,251,306,3,9),(1897,'Внешние накопители',7,307,314,3,9),(1898,'Расходные материалы',7,315,322,3,9),(1899,'Другое',7,323,324,3,9),(1900,'Acer',7,140,141,4,9),(1901,'Apple',7,142,143,4,9),(1902,'Asus',7,144,145,4,9),(1903,'BenQ',7,146,147,4,9),(1904,'Bliss',7,148,149,4,9),(1905,'Compaq',7,150,151,4,9),(1906,'Dell',7,152,153,4,9),(1907,'eMachines',7,154,155,4,9),(1908,'Fujitsu-Siemens',7,156,157,4,9),(1909,'GigaByte',7,158,159,4,9),(1910,'HP',7,160,161,4,9),(1911,'HTC',7,162,163,4,9),(1912,'IBM/ThinkPad',7,164,165,4,9),(1913,'iRU',7,166,167,4,9),(1914,'Lenovo',7,168,169,4,9),(1915,'LG',7,170,171,4,9),(1916,'MSI',7,172,173,4,9),(1917,'NEC',7,174,175,4,9),(1918,'Packard Bell',7,176,177,4,9),(1919,'Panasonic',7,178,179,4,9),(1920,'RoverBook',7,180,181,4,9),(1921,'Samsung',7,182,183,4,9),(1922,'Sony',7,184,185,4,9),(1923,'Toshiba',7,186,187,4,9),(1924,'Другая марка',7,188,189,4,9),(1925,'Для настольных компьютеров',7,196,197,4,9),(1926,'Для ноутбуков',7,198,199,4,9),(1927,'Для КПК / планшетов',7,200,201,4,9),(1928,'Для серверов',7,202,203,4,9),(1929,'Прочие комплектующие',7,208,209,4,9),(1930,'Модули памяти',7,210,211,4,9),(1931,'Видеокарты',7,212,213,4,9),(1932,'Звуковые карты',7,214,215,4,9),(1933,'Материнские платы',7,216,217,4,9),(1934,'Оптические приводы',7,218,219,4,9),(1935,'Жесткие диски',7,220,221,4,9),(1936,'Корпуса',7,222,223,4,9),(1937,'Процессоры',7,224,225,4,9),(1938,'ТВ-тюнеры',7,226,227,4,9),(1939,'Блоки питания',7,228,229,4,9),(1940,'Копиры',7,232,233,4,9),(1941,'Сетевое оборудование',7,234,235,4,9),(1942,'Прочие периферийные устройства',7,236,237,4,9),(1943,'Клавиатуры / мыши / манипуляторы',7,238,239,4,9),(1944,'Вебкамеры',7,240,241,4,9),(1945,'Компьютерная акустика',7,242,243,4,9),(1946,'Принтеры',7,244,245,4,9),(1947,'Мфу',7,246,247,4,9),(1948,'Сканеры',7,248,249,4,9),(1949,'AOC',7,252,253,4,9),(1950,'ASUS',7,254,255,4,9),(1951,'Acer',7,256,257,4,9),(1952,'Apple',7,258,259,4,9),(1953,'BenQ',7,260,261,4,9),(1954,'DELL',7,262,263,4,9),(1955,'Eizo',7,264,265,4,9),(1956,'Envision',7,266,267,4,9),(1957,'Fujitsu-Siemens',7,268,269,4,9),(1958,'HP',7,270,271,4,9),(1959,'Hanns.G',7,272,273,4,9),(1960,'Hyundai',7,274,275,4,9),(1961,'Iiyama',7,276,277,4,9),(1962,'LG',7,278,279,4,9),(1963,'Lenovo',7,280,281,4,9),(1964,'MAG',7,282,283,4,9),(1965,'NEC',7,284,285,4,9),(1966,'Neovo',7,286,287,4,9),(1967,'Philips',7,288,289,4,9),(1968,'Prestigio',7,290,291,4,9),(1969,'Proview',7,292,293,4,9),(1970,'Samsung',7,294,295,4,9),(1971,'Sony',7,296,297,4,9),(1972,'Topview',7,298,299,4,9),(1973,'Toshiba',7,300,301,4,9),(1974,'Viewsonic',7,302,303,4,9),(1975,'Другая марка',7,304,305,4,9),(1976,'Карты памяти',7,308,309,4,9),(1977,'Usb flash',7,310,311,4,9),(1978,'Внешние жесткие диски',7,312,313,4,9),(1979,'Картриджи / тонеры',7,316,317,4,9),(1980,'Дискеты / диски / кассеты',7,318,319,4,9),(1981,'Прочие расходные материалы',7,320,321,4,9),(1982,'Пленочные фотоаппараты',7,327,328,3,9),(1983,'Цифровые фотоаппараты',7,329,394,3,9),(1984,'Видеокамеры',7,395,436,3,9),(1985,'Объективы',7,437,470,3,9),(1986,'Штативы / моноподы',7,471,472,3,9),(1987,'Фотовспышки',7,473,474,3,9),(1988,'Аксессуары для фото / видеокамер',7,475,484,3,9),(1989,'Телескопы / бинокли',7,485,486,3,9),(1990,'Sanyo',7,330,331,4,9),(1991,'Sealife',7,332,333,4,9),(1992,'Sigma',7,334,335,4,9),(1993,'Sony',7,336,337,4,9),(1994,'UFO',7,338,339,4,9),(1995,'Другая марка',7,340,341,4,9),(1996,'Agfaphoto',7,342,343,4,9),(1997,'BBK',7,344,345,4,9),(1998,'BenQ',7,346,347,4,9),(1999,'Canon',7,348,349,4,9),(2000,'Casio',7,350,351,4,9),(2001,'Ergo',7,352,353,4,9),(2002,'Fujifilm',7,354,355,4,9),(2003,'General Electric',7,356,357,4,9),(2004,'Genius',7,358,359,4,9),(2005,'Hasselblad',7,360,361,4,9),(2006,'HP',7,362,363,4,9),(2007,'Kodak',7,364,365,4,9),(2008,'Leica',7,366,367,4,9),(2009,'Mamiya',7,368,369,4,9),(2010,'Nikon',7,370,371,4,9),(2011,'Olympus',7,372,373,4,9),(2012,'Panasonic',7,374,375,4,9),(2013,'Pentax',7,376,377,4,9),(2014,'Polaroid',7,378,379,4,9),(2015,'Praktica',7,380,381,4,9),(2016,'Premier',7,382,383,4,9),(2017,'Rekam',7,384,385,4,9),(2018,'Ricoh',7,386,387,4,9),(2019,'Rollei',7,388,389,4,9),(2020,'Rovershot',7,390,391,4,9),(2021,'Samsung',7,392,393,4,9),(2022,'Aiptek',7,396,397,4,9),(2023,'Canon',7,398,399,4,9),(2024,'DXG',7,400,401,4,9),(2025,'Direc',7,402,403,4,9),(2026,'Elmo',7,404,405,4,9),(2027,'Espada',7,406,407,4,9),(2028,'Explay',7,408,409,4,9),(2029,'Flip Video',7,410,411,4,9),(2030,'Genius',7,412,413,4,9),(2031,'Hitachi',7,414,415,4,9),(2032,'JVC',7,416,417,4,9),(2033,'Kodak',7,418,419,4,9),(2034,'Panasonic',7,420,421,4,9),(2035,'Praktica',7,422,423,4,9),(2036,'Rovershot',7,424,425,4,9),(2037,'Samsung',7,426,427,4,9),(2038,'Sanyo',7,428,429,4,9),(2039,'Sony',7,430,431,4,9),(2040,'Toshiba',7,432,433,4,9),(2041,'Другая марка',7,434,435,4,9),(2042,'Canon',7,438,439,4,9),(2043,'Elicar',7,440,441,4,9),(2044,'Leica',7,442,443,4,9),(2045,'Lensbaby',7,444,445,4,9),(2046,'Mamiya',7,446,447,4,9),(2047,'Nikon',7,448,449,4,9),(2048,'Olympus',7,450,451,4,9),(2049,'Panasonic',7,452,453,4,9),(2050,'Pentax',7,454,455,4,9),(2051,'Sigma',7,456,457,4,9),(2052,'Sony',7,458,459,4,9),(2053,'Tamron',7,460,461,4,9),(2054,'Tokina',7,462,463,4,9),(2055,'Zeiss',7,464,465,4,9),(2056,'Зенит',7,466,467,4,9),(2057,'Другая марка',7,468,469,4,9),(2058,'Сумки',7,476,477,4,9),(2059,'Светофильтры',7,478,479,4,9),(2060,'Зарядные устройства / аккумуляторы',7,480,481,4,9),(2061,'Прочие фото / видеоаксессуары',7,482,483,4,9),(2062,'Медиа проигрыватели',7,489,576,3,9),(2063,'Телевизоры',7,577,704,3,9),(2064,'Аксессуары для ТВ / видео',7,705,706,3,9),(2065,'Спутниковое ТВ',7,707,708,3,9),(2066,'Прочая ТВ / видеотехника',7,709,710,3,9),(2067,'Akai',7,490,491,4,9),(2068,'Akira',7,492,493,4,9),(2069,'BBK',7,494,495,4,9),(2070,'Cambridge Audio',7,496,497,4,9),(2071,'Daewoo',7,498,499,4,9),(2072,'Denon',7,500,501,4,9),(2073,'Dex',7,502,503,4,9),(2074,'Digital',7,504,505,4,9),(2075,'Eplutus',7,506,507,4,9),(2076,'Ergo',7,508,509,4,9),(2077,'Erisson',7,510,511,4,9),(2078,'Gemix',7,512,513,4,9),(2079,'General',7,514,515,4,9),(2080,'Harman/Kardon',7,516,517,4,9),(2081,'Hyundai',7,518,519,4,9),(2082,'JVC',7,520,521,4,9),(2083,'Kenwood',7,522,523,4,9),(2084,'LG',7,524,525,4,9),(2085,'Marantz',7,526,527,4,9),(2086,'Meridian',7,528,529,4,9),(2087,'Mustek',7,530,531,4,9),(2088,'Mystery',7,532,533,4,9),(2089,'Odeon',7,534,535,4,9),(2090,'Panasonic',7,536,537,4,9),(2091,'Phantom',7,538,539,4,9),(2092,'Philips',7,540,541,4,9),(2093,'Pioneer',7,542,543,4,9),(2094,'Rolsen',7,544,545,4,9),(2095,'Samsung',7,546,547,4,9),(2096,'Sharp',7,548,549,4,9),(2097,'Sherwood',7,550,551,4,9),(2098,'Sony',7,552,553,4,9),(2099,'Subini',7,554,555,4,9),(2100,'Supra',7,556,557,4,9),(2101,'Sven',7,558,559,4,9),(2102,'T+A',7,560,561,4,9),(2103,'Toshiba',7,562,563,4,9),(2104,'Vitek',7,564,565,4,9),(2105,'WOKSTER',7,566,567,4,9),(2106,'Xoro',7,568,569,4,9),(2107,'Yamaha',7,570,571,4,9),(2108,'Витязь',7,572,573,4,9),(2109,'Другая марка',7,574,575,4,9),(2110,'Acer',7,578,579,4,9),(2111,'Akai',7,580,581,4,9),(2112,'Akira',7,582,583,4,9),(2113,'Aquavision',7,584,585,4,9),(2114,'BBK',7,586,587,4,9),(2115,'BEKO',7,588,589,4,9),(2116,'BRAVIS',7,590,591,4,9),(2117,'Bang & Olufsen',7,592,593,4,9),(2118,'BenQ',7,594,595,4,9),(2119,'Cameron',7,596,597,4,9),(2120,'Conrac',7,598,599,4,9),(2121,'Daewoo',7,600,601,4,9),(2122,'Dex',7,602,603,4,9),(2123,'Digital',7,604,605,4,9),(2124,'Elenberg',7,606,607,4,9),(2125,'Erisson',7,608,609,4,9),(2126,'Fujitsu',7,610,611,4,9),(2127,'Grundig',7,612,613,4,9),(2128,'Hanns.G',7,614,615,4,9),(2129,'Hantarex',7,616,617,4,9),(2130,'Hitachi',7,618,619,4,9),(2131,'Hyundai',7,620,621,4,9),(2132,'JVC',7,622,623,4,9),(2133,'LG',7,624,625,4,9),(2134,'Liberton',7,626,627,4,9),(2135,'Loewe',7,628,629,4,9),(2136,'Metz',7,630,631,4,9),(2137,'Mitsubishi Electric',7,632,633,4,9),(2138,'Mystery',7,634,635,4,9),(2139,'NEC',7,636,637,4,9),(2140,'Novex',7,638,639,4,9),(2141,'Orion',7,640,641,4,9),(2142,'Panasonic',7,642,643,4,9),(2143,'Patriot',7,644,645,4,9),(2144,'Philips',7,646,647,4,9),(2145,'Pioneer',7,648,649,4,9),(2146,'Prestigio',7,650,651,4,9),(2147,'Prima',7,652,653,4,9),(2148,'Rainford',7,654,655,4,9),(2149,'Rolsen',7,656,657,4,9),(2150,'Runco',7,658,659,4,9),(2151,'SAGA',7,660,661,4,9),(2152,'Samsung',7,662,663,4,9),(2153,'Sansui',7,664,665,4,9),(2154,'Sanyo',7,666,667,4,9),(2155,'Saturn',7,668,669,4,9),(2156,'Sharp',7,670,671,4,9),(2157,'Shivaki',7,672,673,4,9),(2158,'Sony',7,674,675,4,9),(2159,'Supra',7,676,677,4,9),(2160,'TCL',7,678,679,4,9),(2161,'Techno',7,680,681,4,9),(2162,'Thomson',7,682,683,4,9),(2163,'Toshiba',7,684,685,4,9),(2164,'VR',7,686,687,4,9),(2165,'Vestel',7,688,689,4,9),(2166,'WEST',7,690,691,4,9),(2167,'Xoro',7,692,693,4,9),(2168,'Витязь',7,694,695,4,9),(2169,'Горизонт',7,696,697,4,9),(2170,'Рубин',7,698,699,4,9),(2171,'Электрон',7,700,701,4,9),(2172,'Другая марка',7,702,703,4,9),(2173,'Mp3 плееры',7,713,796,3,9),(2174,'Магнитолы',7,797,798,3,9),(2175,'Музыкальные центры',7,799,840,3,9),(2176,'Акустические системы',7,841,842,3,9),(2177,'Наушники',7,843,844,3,9),(2178,'Радиоприемники',7,845,846,3,9),(2179,'Портативная акустика',7,847,848,3,9),(2180,'Усилители / ресиверы',7,849,960,3,9),(2181,'Cd / md / виниловые проигрыватели',7,961,1056,3,9),(2182,'Прочая аудиотехника',7,1057,1058,3,9),(2183,'ACME',7,714,715,4,9),(2184,'Akai',7,716,717,4,9),(2185,'Apacer',7,718,719,4,9),(2186,'Archos',7,720,721,4,9),(2187,'Assistant',7,722,723,4,9),(2188,'BBK',7,724,725,4,9),(2189,'Canyon',7,726,727,4,9),(2190,'Cowon',7,728,729,4,9),(2191,'Creative',7,730,731,4,9),(2192,'Daewoo',7,732,733,4,9),(2193,'DITECH',7,734,735,4,9),(2194,'Ergo',7,736,737,4,9),(2195,'Erisson',7,738,739,4,9),(2196,'Explay',7,740,741,4,9),(2197,'FunTwist',7,742,743,4,9),(2198,'Genius',7,744,745,4,9),(2199,'GETHAP',7,746,747,4,9),(2200,'GRAND',7,748,749,4,9),(2201,'Intenso',7,750,751,4,9),(2202,'iPod',7,752,753,4,9),(2203,'iRiver',7,754,755,4,9),(2204,'Le-Mon',7,756,757,4,9),(2205,'MobiBlu',7,758,759,4,9),(2206,'Packard Bell',7,760,761,4,9),(2207,'Philips',7,762,763,4,9),(2208,'Qumo',7,764,765,4,9),(2209,'Ritmix',7,766,767,4,9),(2210,'R-TOUCH',7,768,769,4,9),(2211,'Samsung',7,770,771,4,9),(2212,'Sandisk',7,772,773,4,9),(2213,'Seekwood',7,774,775,4,9),(2214,'Sony',7,776,777,4,9),(2215,'Sweex',7,778,779,4,9),(2216,'TakeMS',7,780,781,4,9),(2217,'TEAC',7,782,783,4,9),(2218,'TeXet',7,784,785,4,9),(2219,'Transcend',7,786,787,4,9),(2220,'WOKSTER',7,788,789,4,9),(2221,'Zen',7,790,791,4,9),(2222,'Zoom',7,792,793,4,9),(2223,'Другая марка',7,794,795,4,9),(2224,'Bang & Olufsen',7,800,801,4,9),(2225,'Daewoo',7,802,803,4,9),(2226,'Denon',7,804,805,4,9),(2227,'Grundig',7,806,807,4,9),(2228,'Hyundai',7,808,809,4,9),(2229,'JVC',7,810,811,4,9),(2230,'Kenwood',7,812,813,4,9),(2231,'LG',7,814,815,4,9),(2232,'Mystery',7,816,817,4,9),(2233,'Nakamichi',7,818,819,4,9),(2234,'Onkyo',7,820,821,4,9),(2235,'Panasonic',7,822,823,4,9),(2236,'Philips',7,824,825,4,9),(2237,'Samsung',7,826,827,4,9),(2238,'Sanyo',7,828,829,4,9),(2239,'Sharp',7,830,831,4,9),(2240,'Sony',7,832,833,4,9),(2241,'TEAC',7,834,835,4,9),(2242,'Yamaha',7,836,837,4,9),(2243,'Другая марка',7,838,839,4,9),(2244,'Accuphase',7,850,851,4,9),(2245,'Adcom',7,852,853,4,9),(2246,'AMC',7,854,855,4,9),(2247,'Anthem',7,856,857,4,9),(2248,'Arcam',7,858,859,4,9),(2249,'ATI',7,860,861,4,9),(2250,'Atoll',7,862,863,4,9),(2251,'Audio Analogue',7,864,865,4,9),(2252,'Audio Note',7,866,867,4,9),(2253,'Audio Space',7,868,869,4,9),(2254,'Audionet',7,870,871,4,9),(2255,'Ayon Audio',7,872,873,4,9),(2256,'Bryston',7,874,875,4,9),(2257,'Cambridge Audio',7,876,877,4,9),(2258,'Cayin',7,878,879,4,9),(2259,'Classe Audio',7,880,881,4,9),(2260,'Creek',7,882,883,4,9),(2261,'Cyrus',7,884,885,4,9),(2262,'Denon',7,886,887,4,9),(2263,'Densen',7,888,889,4,9),(2264,'EAR',7,890,891,4,9),(2265,'Electrocompaniet',7,892,893,4,9),(2266,'Enlightened Audio',7,894,895,4,9),(2267,'Exposure',7,896,897,4,9),(2268,'Harman/Kardon',7,898,899,4,9),(2269,'Lexicon',7,900,901,4,9),(2270,'Luxman',7,902,903,4,9),(2271,'Marantz',7,904,905,4,9),(2272,'Mark Levinson',7,906,907,4,9),(2273,'McIntosh',7,908,909,4,9),(2274,'Melody',7,910,911,4,9),(2275,'Micromega',7,912,913,4,9),(2276,'Musical Fidelity',7,914,915,4,9),(2277,'Myryad',7,916,917,4,9),(2278,'NAD',7,918,919,4,9),(2279,'Naim Audio',7,920,921,4,9),(2280,'NuForce',7,922,923,4,9),(2281,'Onkyo',7,924,925,4,9),(2282,'Parasound',7,926,927,4,9),(2283,'Pioneer',7,928,929,4,9),(2284,'Plinius',7,930,931,4,9),(2285,'Primare',7,932,933,4,9),(2286,'Pro-Ject',7,934,935,4,9),(2287,'PS Audio',7,936,937,4,9),(2288,'Rega',7,938,939,4,9),(2289,'Roksan',7,940,941,4,9),(2290,'Rotel',7,942,943,4,9),(2291,'Sherwood',7,944,945,4,9),(2292,'Sony',7,946,947,4,9),(2293,'T+A',7,948,949,4,9),(2294,'TEAC',7,950,951,4,9),(2295,'Vincent',7,952,953,4,9),(2296,'Yamaha',7,954,955,4,9),(2297,'YBA',7,956,957,4,9),(2298,'Другая марка',7,958,959,4,9),(2299,'Unison Research',7,962,963,4,9),(2300,'Vincent',7,964,965,4,9),(2301,'Wadia',7,966,967,4,9),(2302,'YBA',7,968,969,4,9),(2303,'Yamaha',7,970,971,4,9),(2304,'Другая марка',7,972,973,4,9),(2305,'Atoll Electronique',7,974,975,4,9),(2306,'AVM',7,976,977,4,9),(2307,'Accuphase',7,978,979,4,9),(2308,'Advance Acoustic',7,980,981,4,9),(2309,'April Music',7,982,983,4,9),(2310,'Arcam',7,984,985,4,9),(2311,'Audio Analogue',7,986,987,4,9),(2312,'Audio Research',7,988,989,4,9),(2313,'BlueNote',7,990,991,4,9),(2314,'Burmester',7,992,993,4,9),(2315,'C.E.C.',7,994,995,4,9),(2316,'Cambridge Audio',7,996,997,4,9),(2317,'Cary Audio',7,998,999,4,9),(2318,'Creek',7,1000,1001,4,9),(2319,'Cyrus',7,1002,1003,4,9),(2320,'Denon',7,1004,1005,4,9),(2321,'Exposure',7,1006,1007,4,9),(2322,'Gemini',7,1008,1009,4,9),(2323,'Harman/Kardon',7,1010,1011,4,9),(2324,'Marantz',7,1012,1013,4,9),(2325,'McIntosh',7,1014,1015,4,9),(2326,'Meridian',7,1016,1017,4,9),(2327,'Monrio',7,1018,1019,4,9),(2328,'Musical Fidelity',7,1020,1021,4,9),(2329,'NAD',7,1022,1023,4,9),(2330,'Numark',7,1024,1025,4,9),(2331,'Onkyo',7,1026,1027,4,9),(2332,'Original Electronics',7,1028,1029,4,9),(2333,'Perreaux',7,1030,1031,4,9),(2334,'Pioneer',7,1032,1033,4,9),(2335,'Primare',7,1034,1035,4,9),(2336,'Rega',7,1036,1037,4,9),(2337,'Rotel',7,1038,1039,4,9),(2338,'Sherwood',7,1040,1041,4,9),(2339,'Sim Audio',7,1042,1043,4,9),(2340,'Sony',7,1044,1045,4,9),(2341,'Stanton',7,1046,1047,4,9),(2342,'T+A',7,1048,1049,4,9),(2343,'TEAC',7,1050,1051,4,9),(2344,'Tangent',7,1052,1053,4,9),(2345,'Tascam',7,1054,1055,4,9),(2346,'Игры для приставок',7,1061,1074,3,9),(2347,'Приставки',7,1075,1088,3,9),(2348,'Игры для PC',7,1089,1090,3,9),(2349,'Аксессуары',7,1091,1092,3,9),(2350,'Герои игр',7,1093,1094,3,9),(2351,'Ремонт приставок',7,1095,1096,3,9),(2352,'Nintendo DS',7,1062,1063,4,9),(2353,'Nintendo Wii',7,1064,1065,4,9),(2354,'Sony PlayStation',7,1066,1067,4,9),(2355,'Sony PSP',7,1068,1069,4,9),(2356,'XBOX',7,1070,1071,4,9),(2357,'Другая',7,1072,1073,4,9),(2358,'Nintendo DS',7,1076,1077,4,9),(2359,'Nintendo Wii',7,1078,1079,4,9),(2360,'Sony PlayStation',7,1080,1081,4,9),(2361,'Sony PSP',7,1082,1083,4,9),(2362,'XBOX',7,1084,1085,4,9),(2363,'Другая',7,1086,1087,4,9),(2364,'Пылесосы',7,1099,1238,3,9),(2365,'Утюги',7,1239,1334,3,9),(2366,'Стиральные машины',7,1335,1430,3,9),(2367,'Швейные машины и оверлоки',7,1431,1474,3,9),(2368,'Вязальные машины',7,1475,1476,3,9),(2369,'Фильтры для воды',7,1477,1478,3,9),(2370,'Прочая техника для дома',7,1479,1480,3,9),(2371,'Hansa',7,1100,1101,4,9),(2372,'Heyner',7,1102,1103,4,9),(2373,'Hoover',7,1104,1105,4,9),(2374,'Hyundai',7,1106,1107,4,9),(2375,'Irit',7,1108,1109,4,9),(2376,'iRobot',7,1110,1111,4,9),(2377,'Kirby',7,1112,1113,4,9),(2378,'Karcher',7,1114,1115,4,9),(2379,'LG',7,1116,1117,4,9),(2380,'Liberton',7,1118,1119,4,9),(2381,'Lumme',7,1120,1121,4,9),(2382,'Makita',7,1122,1123,4,9),(2383,'Marta',7,1124,1125,4,9),(2384,'Miele',7,1126,1127,4,9),(2385,'Orion',7,1128,1129,4,9),(2386,'Panasonic',7,1130,1131,4,9),(2387,'Phantom',7,1132,1133,4,9),(2388,'Philips',7,1134,1135,4,9),(2389,'Piece',7,1136,1137,4,9),(2390,'Redber',7,1138,1139,4,9),(2391,'Redmond',7,1140,1141,4,9),(2392,'Rolsen',7,1142,1143,4,9),(2393,'Rotex',7,1144,1145,4,9),(2394,'Rowenta',7,1146,1147,4,9),(2395,'Russell Hobbs',7,1148,1149,4,9),(2396,'Samsung',7,1150,1151,4,9),(2397,'Saturn',7,1152,1153,4,9),(2398,'Scarlett',7,1154,1155,4,9),(2399,'Severin',7,1156,1157,4,9),(2400,'Siemens',7,1158,1159,4,9),(2401,'Soteco',7,1160,1161,4,9),(2402,'Supra',7,1162,1163,4,9),(2403,'Synco',7,1164,1165,4,9),(2404,'Thomas',7,1166,1167,4,9),(2405,'Trisa',7,1168,1169,4,9),(2406,'VES',7,1170,1171,4,9),(2407,'VR',7,1172,1173,4,9),(2408,'Vax',7,1174,1175,4,9),(2409,'Vitek',7,1176,1177,4,9),(2410,'Vitesse',7,1178,1179,4,9),(2411,'Zanussi',7,1180,1181,4,9),(2412,'Zelmer',7,1182,1183,4,9),(2413,'Аксион',7,1184,1185,4,9),(2414,'Горизонт',7,1186,1187,4,9),(2415,'Рубин',7,1188,1189,4,9),(2416,'Фея',7,1190,1191,4,9),(2417,'Другая марка',7,1192,1193,4,9),(2418,'Alpari',7,1194,1195,4,9),(2419,'Akai',7,1196,1197,4,9),(2420,'Alpina',7,1198,1199,4,9),(2421,'Ariete',7,1200,1201,4,9),(2422,'Artlina',7,1202,1203,4,9),(2423,'BEKO',7,1204,1205,4,9),(2424,'Black&Decker',7,1206,1207,4,9),(2425,'Bork',7,1208,1209,4,9),(2426,'Bosch',7,1210,1211,4,9),(2427,'Clatronic',7,1212,1213,4,9),(2428,'Daewoo',7,1214,1215,4,9),(2429,'Delonghi',7,1216,1217,4,9),(2430,'Delvir',7,1218,1219,4,9),(2431,'Dirt',7,1220,1221,4,9),(2432,'Dyson',7,1222,1223,4,9),(2433,'EIO',7,1224,1225,4,9),(2434,'Elect',7,1226,1227,4,9),(2435,'Electrolux',7,1228,1229,4,9),(2436,'Elekta',7,1230,1231,4,9),(2437,'Elenberg',7,1232,1233,4,9),(2438,'Energy',7,1234,1235,4,9),(2439,'Gorenje',7,1236,1237,4,9),(2440,'Alpina',7,1240,1241,4,9),(2441,'Ariete',7,1242,1243,4,9),(2442,'Atlanta',7,1244,1245,4,9),(2443,'Aurora',7,1246,1247,4,9),(2444,'Binatone',7,1248,1249,4,9),(2445,'Bork',7,1250,1251,4,9),(2446,'Bosch',7,1252,1253,4,9),(2447,'Braun',7,1254,1255,4,9),(2448,'Clatronic',7,1256,1257,4,9),(2449,'Daewoo',7,1258,1259,4,9),(2450,'Delfa',7,1260,1261,4,9),(2451,'Delonghi',7,1262,1263,4,9),(2452,'Elect',7,1264,1265,4,9),(2453,'Elbee',7,1266,1267,4,9),(2454,'Electrolux',7,1268,1269,4,9),(2455,'Energy',7,1270,1271,4,9),(2456,'First',7,1272,1273,4,9),(2457,'Gorenje',7,1274,1275,4,9),(2458,'Home-Element',7,1276,1277,4,9),(2459,'Kenwood',7,1278,1279,4,9),(2460,'Lelit',7,1280,1281,4,9),(2461,'Magnit',7,1282,1283,4,9),(2462,'Marta',7,1284,1285,4,9),(2463,'Maxima',7,1286,1287,4,9),(2464,'Maxwell',7,1288,1289,4,9),(2465,'Morphy Richards',7,1290,1291,4,9),(2466,'Moulinex',7,1292,1293,4,9),(2467,'Orion',7,1294,1295,4,9),(2468,'Panasonic',7,1296,1297,4,9),(2469,'Philips',7,1298,1299,4,9),(2470,'Polaris',7,1300,1301,4,9),(2471,'Redmond',7,1302,1303,4,9),(2472,'Rowenta',7,1304,1305,4,9),(2473,'Saturn',7,1306,1307,4,9),(2474,'Scarlett',7,1308,1309,4,9),(2475,'Severin',7,1310,1311,4,9),(2476,'Siemens',7,1312,1313,4,9),(2477,'Supra',7,1314,1315,4,9),(2478,'Tefal',7,1316,1317,4,9),(2479,'Viconte',7,1318,1319,4,9),(2480,'Vitek',7,1320,1321,4,9),(2481,'WEST',7,1322,1323,4,9),(2482,'Wellton',7,1324,1325,4,9),(2483,'Zelmer',7,1326,1327,4,9),(2484,'Рубин',7,1328,1329,4,9),(2485,'Фея',7,1330,1331,4,9),(2486,'Другая марка',7,1332,1333,4,9),(2487,'Indesit',7,1336,1337,4,9),(2488,'Kaiser',7,1338,1339,4,9),(2489,'Kuppersbusch',7,1340,1341,4,9),(2490,'LG',7,1342,1343,4,9),(2491,'Maytag',7,1344,1345,4,9),(2492,'Miele',7,1346,1347,4,9),(2493,'Neff',7,1348,1349,4,9),(2494,'Philco',7,1350,1351,4,9),(2495,'Polar',7,1352,1353,4,9),(2496,'Rolsen',7,1354,1355,4,9),(2497,'Samsung',7,1356,1357,4,9),(2498,'SCHULTHESS',7,1358,1359,4,9),(2499,'Siemens',7,1360,1361,4,9),(2500,'Siltal',7,1362,1363,4,9),(2501,'Skiff',7,1364,1365,4,9),(2502,'Smeg',7,1366,1367,4,9),(2503,'Teka',7,1368,1369,4,9),(2504,'Vestel',7,1370,1371,4,9),(2505,'Whirlpool',7,1372,1373,4,9),(2506,'Zanussi',7,1374,1375,4,9),(2507,'Zerowatt',7,1376,1377,4,9),(2508,'Атлант',7,1378,1379,4,9),(2509,'Вятка',7,1380,1381,4,9),(2510,'Ока',7,1382,1383,4,9),(2511,'Фея',7,1384,1385,4,9),(2512,'Другая марка',7,1386,1387,4,9),(2513,'AEG',7,1388,1389,4,9),(2514,'Akai',7,1390,1391,4,9),(2515,'Ardo',7,1392,1393,4,9),(2516,'Ariston',7,1394,1395,4,9),(2517,'Asko',7,1396,1397,4,9),(2518,'Bauknecht',7,1398,1399,4,9),(2519,'BEKO',7,1400,1401,4,9),(2520,'Blomberg',7,1402,1403,4,9),(2521,'Bosch',7,1404,1405,4,9),(2522,'Brandt',7,1406,1407,4,9),(2523,'Candy',7,1408,1409,4,9),(2524,'Daewoo',7,1410,1411,4,9),(2525,'Desany',7,1412,1413,4,9),(2526,'Electrolux',7,1414,1415,4,9),(2527,'Euronova',7,1416,1417,4,9),(2528,'Fagor',7,1418,1419,4,9),(2529,'Frigidaire',7,1420,1421,4,9),(2530,'General Electric',7,1422,1423,4,9),(2531,'Gorenje',7,1424,1425,4,9),(2532,'Hansa',7,1426,1427,4,9),(2533,'Hoover',7,1428,1429,4,9),(2534,'AstraLux',7,1432,1433,4,9),(2535,'Aurora',7,1434,1435,4,9),(2536,'Boutique',7,1436,1437,4,9),(2537,'Bernina',7,1438,1439,4,9),(2538,'Brother',7,1440,1441,4,9),(2539,'Dragon Fly',7,1442,1443,4,9),(2540,'Elna',7,1444,1445,4,9),(2541,'Family',7,1446,1447,4,9),(2542,'Husqvarna',7,1448,1449,4,9),(2543,'Jaguar',7,1450,1451,4,9),(2544,'Janome',7,1452,1453,4,9),(2545,'Juki',7,1454,1455,4,9),(2546,'Leader',7,1456,1457,4,9),(2547,'New Home',7,1458,1459,4,9),(2548,'Pfaff',7,1460,1461,4,9),(2549,'Profi',7,1462,1463,4,9),(2550,'Singer',7,1464,1465,4,9),(2551,'Toyota',7,1466,1467,4,9),(2552,'Veritas',7,1468,1469,4,9),(2553,'Yamata',7,1470,1471,4,9),(2554,'Другая марка',7,1472,1473,4,9),(2555,'Микроволновые печи',7,1483,1530,3,9),(2556,'Холодильники',7,1531,1618,3,9),(2557,'Плиты / печи',7,1619,1714,3,9),(2558,'Электрочайники',7,1715,1716,3,9),(2559,'Кофеварки / кофемолки',7,1717,1804,3,9),(2560,'Кухонные комбайны / измельчители',7,1805,1894,3,9),(2561,'Пароварки / мультиварки',7,1895,1986,3,9),(2562,'Хлебопечки',7,1987,2052,3,9),(2563,'Посудомоечные машины',7,2053,2108,3,9),(2564,'Вытяжки',7,2109,2110,3,9),(2565,'Прочая техника для кухни',7,2111,2112,3,9),(2566,'AEG',7,1484,1485,4,9),(2567,'ALPARI',7,1486,1487,4,9),(2568,'BEKO',7,1488,1489,4,9),(2569,'Bork',7,1490,1491,4,9),(2570,'Bosch',7,1492,1493,4,9),(2571,'Daewoo',7,1494,1495,4,9),(2572,'Delonghi',7,1496,1497,4,9),(2573,'Electrolux',7,1498,1499,4,9),(2574,'Gorenje',7,1500,1501,4,9),(2575,'Hyundai',7,1502,1503,4,9),(2576,'Kuppersbusch',7,1504,1505,4,9),(2577,'LG',7,1506,1507,4,9),(2578,'Miele',7,1508,1509,4,9),(2579,'Moulinex',7,1510,1511,4,9),(2580,'Panasonic',7,1512,1513,4,9),(2581,'Samsung',7,1514,1515,4,9),(2582,'Scarlett',7,1516,1517,4,9),(2583,'Sharp',7,1518,1519,4,9),(2584,'Siemens',7,1520,1521,4,9),(2585,'Supra',7,1522,1523,4,9),(2586,'Vitek',7,1524,1525,4,9),(2587,'Whirlpool',7,1526,1527,4,9),(2588,'Другая марка',7,1528,1529,4,9),(2589,'AEG',7,1532,1533,4,9),(2590,'Amana',7,1534,1535,4,9),(2591,'Ardo',7,1536,1537,4,9),(2592,'Ariston',7,1538,1539,4,9),(2593,'BEKO',7,1540,1541,4,9),(2594,'Blomberg',7,1542,1543,4,9),(2595,'Bosch',7,1544,1545,4,9),(2596,'Candy',7,1546,1547,4,9),(2597,'Chambrer',7,1548,1549,4,9),(2598,'Climadiff',7,1550,1551,4,9),(2599,'Daewoo',7,1552,1553,4,9),(2600,'Electrolux',7,1554,1555,4,9),(2601,'Frigidaire',7,1556,1557,4,9),(2602,'Gaggenau',7,1558,1559,4,9),(2603,'Gorenje',7,1560,1561,4,9),(2604,'Haier',7,1562,1563,4,9),(2605,'Hansa',7,1564,1565,4,9),(2606,'Indesit',7,1566,1567,4,9),(2607,'Kuppersbusch',7,1568,1569,4,9),(2608,'LG',7,1570,1571,4,9),(2609,'Liebherr',7,1572,1573,4,9),(2610,'Miele',7,1574,1575,4,9),(2611,'Nardi',7,1576,1577,4,9),(2612,'Neff',7,1578,1579,4,9),(2613,'NORD',7,1580,1581,4,9),(2614,'OAK',7,1582,1583,4,9),(2615,'Panasonic',7,1584,1585,4,9),(2616,'Pozis',7,1586,1587,4,9),(2617,'Samsung',7,1588,1589,4,9),(2618,'Sharp',7,1590,1591,4,9),(2619,'Siemens',7,1592,1593,4,9),(2620,'Smeg',7,1594,1595,4,9),(2621,'Snaige',7,1596,1597,4,9),(2622,'Stinol',7,1598,1599,4,9),(2623,'TEKA',7,1600,1601,4,9),(2624,'Vestfrost',7,1602,1603,4,9),(2625,'Vinosafe',7,1604,1605,4,9),(2626,'Whirlpool',7,1606,1607,4,9),(2627,'Атлант',7,1608,1609,4,9),(2628,'Бирюса',7,1610,1611,4,9),(2629,'Саратов',7,1612,1613,4,9),(2630,'Смоленск',7,1614,1615,4,9),(2631,'Другая марка',7,1616,1617,4,9),(2632,'AEG',7,1620,1621,4,9),(2633,'Ardo',7,1622,1623,4,9),(2634,'Ariston',7,1624,1625,4,9),(2635,'BEKO',7,1626,1627,4,9),(2636,'Blomberg',7,1628,1629,4,9),(2637,'Bompani',7,1630,1631,4,9),(2638,'Bosch',7,1632,1633,4,9),(2639,'Candy',7,1634,1635,4,9),(2640,'Dako (Mabe)',7,1636,1637,4,9),(2641,'Delonghi',7,1638,1639,4,9),(2642,'Deluxe',7,1640,1641,4,9),(2643,'Desany',7,1642,1643,4,9),(2644,'Electrolux',7,1644,1645,4,9),(2645,'Fratelli Onofri',7,1646,1647,4,9),(2646,'Fulgor',7,1648,1649,4,9),(2647,'Gorenje',7,1650,1651,4,9),(2648,'GRETA',7,1652,1653,4,9),(2649,'Hansa',7,1654,1655,4,9),(2650,'ILVE',7,1656,1657,4,9),(2651,'Indesit',7,1658,1659,4,9),(2652,'Kaiser',7,1660,1661,4,9),(2653,'Kuppersbusch',7,1662,1663,4,9),(2654,'LUXELL',7,1664,1665,4,9),(2655,'Miele',7,1666,1667,4,9),(2656,'Mora',7,1668,1669,4,9),(2657,'Neff',7,1670,1671,4,9),(2658,'NORD',7,1672,1673,4,9),(2659,'Rainford',7,1674,1675,4,9),(2660,'Restart',7,1676,1677,4,9),(2661,'Severin',7,1678,1679,4,9),(2662,'Siemens',7,1680,1681,4,9),(2663,'Smeg',7,1682,1683,4,9),(2664,'Vestel',7,1684,1685,4,9),(2665,'Whirlpool',7,1686,1687,4,9),(2666,'Zanussi',7,1688,1689,4,9),(2667,'Zigmund & Shtain',7,1690,1691,4,9),(2668,'Веста',7,1692,1693,4,9),(2669,'Газмаш',7,1694,1695,4,9),(2670,'Гефест',7,1696,1697,4,9),(2671,'ЗВИ',7,1698,1699,4,9),(2672,'Идель',7,1700,1701,4,9),(2673,'Крона',7,1702,1703,4,9),(2674,'Лада',7,1704,1705,4,9),(2675,'Мечта',7,1706,1707,4,9),(2676,'Нововятка',7,1708,1709,4,9),(2677,'Омичка',7,1710,1711,4,9),(2678,'Другая марка',7,1712,1713,4,9),(2679,'AEG',7,1718,1719,4,9),(2680,'Ariete',7,1720,1721,4,9),(2681,'Ascaso',7,1722,1723,4,9),(2682,'Bezzera',7,1724,1725,4,9),(2683,'Bork',7,1726,1727,4,9),(2684,'Bosch',7,1728,1729,4,9),(2685,'Brasilia',7,1730,1731,4,9),(2686,'Braun',7,1732,1733,4,9),(2687,'Briel',7,1734,1735,4,9),(2688,'Delonghi',7,1736,1737,4,9),(2689,'Electrolux',7,1738,1739,4,9),(2690,'Expobar',7,1740,1741,4,9),(2691,'Faema',7,1742,1743,4,9),(2692,'First',7,1744,1745,4,9),(2693,'Futurmat',7,1746,1747,4,9),(2694,'Gaggia',7,1748,1749,4,9),(2695,'Irit',7,1750,1751,4,9),(2696,'Jura',7,1752,1753,4,9),(2697,'Kenwood',7,1754,1755,4,9),(2698,'Krups',7,1756,1757,4,9),(2699,'La Cimbali',7,1758,1759,4,9),(2700,'La Marzocco',7,1760,1761,4,9),(2701,'La Pavoni',7,1762,1763,4,9),(2702,'Lavazza',7,1764,1765,4,9),(2703,'Markus',7,1766,1767,4,9),(2704,'Melitta',7,1768,1769,4,9),(2705,'Miele',7,1770,1771,4,9),(2706,'Nespresso',7,1772,1773,4,9),(2707,'Philips',7,1774,1775,4,9),(2708,'Rancilio',7,1776,1777,4,9),(2709,'Rowenta',7,1778,1779,4,9),(2710,'SPINEL',7,1780,1781,4,9),(2711,'Saeco',7,1782,1783,4,9),(2712,'Scarlett',7,1784,1785,4,9),(2713,'Schaerer',7,1786,1787,4,9),(2714,'Severin',7,1788,1789,4,9),(2715,'Siemens',7,1790,1791,4,9),(2716,'Tefal',7,1792,1793,4,9),(2717,'UNIT',7,1794,1795,4,9),(2718,'Vitek',7,1796,1797,4,9),(2719,'Waeco',7,1798,1799,4,9),(2720,'Zelmer',7,1800,1801,4,9),(2721,'Другая марка',7,1802,1803,4,9),(2722,'Saturn',7,1806,1807,4,9),(2723,'Scarlett',7,1808,1809,4,9),(2724,'Severin',7,1810,1811,4,9),(2725,'Siemens',7,1812,1813,4,9),(2726,'Supra',7,1814,1815,4,9),(2727,'Tefal',7,1816,1817,4,9),(2728,'VES',7,1818,1819,4,9),(2729,'Viconte',7,1820,1821,4,9),(2730,'Vitek',7,1822,1823,4,9),(2731,'WEST',7,1824,1825,4,9),(2732,'Zauber',7,1826,1827,4,9),(2733,'Zelmer',7,1828,1829,4,9),(2734,'Ладомир',7,1830,1831,4,9),(2735,'Другая марка',7,1832,1833,4,9),(2736,'Alpina',7,1834,1835,4,9),(2737,'Ariete',7,1836,1837,4,9),(2738,'Atlanta',7,1838,1839,4,9),(2739,'Aurora',7,1840,1841,4,9),(2740,'BEKO',7,1842,1843,4,9),(2741,'Binatone',7,1844,1845,4,9),(2742,'Bork',7,1846,1847,4,9),(2743,'Bosch',7,1848,1849,4,9),(2744,'Braun',7,1850,1851,4,9),(2745,'Clatronic',7,1852,1853,4,9),(2746,'Daewoo',7,1854,1855,4,9),(2747,'Elekta',7,1856,1857,4,9),(2748,'Elenberg',7,1858,1859,4,9),(2749,'Irit',7,1860,1861,4,9),(2750,'Kenwood',7,1862,1863,4,9),(2751,'Kia',7,1864,1865,4,9),(2752,'Krups',7,1866,1867,4,9),(2753,'Lamarque',7,1868,1869,4,9),(2754,'Magimix',7,1870,1871,4,9),(2755,'Marta',7,1872,1873,4,9),(2756,'Morphy Richards',7,1874,1875,4,9),(2757,'Moulinex',7,1876,1877,4,9),(2758,'Mystery',7,1878,1879,4,9),(2759,'Orion',7,1880,1881,4,9),(2760,'Philips',7,1882,1883,4,9),(2761,'Polaris',7,1884,1885,4,9),(2762,'Rainford',7,1886,1887,4,9),(2763,'Redmond',7,1888,1889,4,9),(2764,'Rolsen',7,1890,1891,4,9),(2765,'Rotel',7,1892,1893,4,9),(2766,'AEG',7,1896,1897,4,9),(2767,'Alpina',7,1898,1899,4,9),(2768,'Ariete',7,1900,1901,4,9),(2769,'Atlanta',7,1902,1903,4,9),(2770,'Binatone',7,1904,1905,4,9),(2771,'Bomann',7,1906,1907,4,9),(2772,'Bork',7,1908,1909,4,9),(2773,'Bosch',7,1910,1911,4,9),(2774,'Braun',7,1912,1913,4,9),(2775,'Cameron',7,1914,1915,4,9),(2776,'Clatronic',7,1916,1917,4,9),(2777,'Daewoo',7,1918,1919,4,9),(2778,'Elbee',7,1920,1921,4,9),(2779,'Elenberg',7,1922,1923,4,9),(2780,'First',7,1924,1925,4,9),(2781,'Gaggenau',7,1926,1927,4,9),(2782,'Gorenje',7,1928,1929,4,9),(2783,'Irit',7,1930,1931,4,9),(2784,'Kenwood',7,1932,1933,4,9),(2785,'Lumme',7,1934,1935,4,9),(2786,'Marta',7,1936,1937,4,9),(2787,'Maxima',7,1938,1939,4,9),(2788,'Morphy Richards',7,1940,1941,4,9),(2789,'Moulinex',7,1942,1943,4,9),(2790,'Mystery',7,1944,1945,4,9),(2791,'Orion',7,1946,1947,4,9),(2792,'Philips',7,1948,1949,4,9),(2793,'Polaris',7,1950,1951,4,9),(2794,'Redmond',7,1952,1953,4,9),(2795,'Saturn',7,1954,1955,4,9),(2796,'Scarlett',7,1956,1957,4,9),(2797,'Severin',7,1958,1959,4,9),(2798,'Siemens',7,1960,1961,4,9),(2799,'Smile',7,1962,1963,4,9),(2800,'Supra',7,1964,1965,4,9),(2801,'Tefal',7,1966,1967,4,9),(2802,'Trisa',7,1968,1969,4,9),(2803,'Unit',7,1970,1971,4,9),(2804,'VES',7,1972,1973,4,9),(2805,'VR',7,1974,1975,4,9),(2806,'Vitek',7,1976,1977,4,9),(2807,'Vitesse',7,1978,1979,4,9),(2808,'Zauber',7,1980,1981,4,9),(2809,'Zelmer',7,1982,1983,4,9),(2810,'Другая марка',7,1984,1985,4,9),(2811,'Alpari',7,1988,1989,4,9),(2812,'Ariete',7,1990,1991,4,9),(2813,'Belson',7,1992,1993,4,9),(2814,'Benten',7,1994,1995,4,9),(2815,'Billeo',7,1996,1997,4,9),(2816,'Binatone',7,1998,1999,4,9),(2817,'Bomann',7,2000,2001,4,9),(2818,'Bork',7,2002,2003,4,9),(2819,'Clatronic',7,2004,2005,4,9),(2820,'Delfa',7,2006,2007,4,9),(2821,'Delonghi',7,2008,2009,4,9),(2822,'Dex',7,2010,2011,4,9),(2823,'Elbee',7,2012,2013,4,9),(2824,'Electrolux',7,2014,2015,4,9),(2825,'Elenberg',7,2016,2017,4,9),(2826,'First',7,2018,2019,4,9),(2827,'Kenwood',7,2020,2021,4,9),(2828,'LG',7,2022,2023,4,9),(2829,'Liberton',7,2024,2025,4,9),(2830,'Moulinex',7,2026,2027,4,9),(2831,'Orion',7,2028,2029,4,9),(2832,'Panasonic',7,2030,2031,4,9),(2833,'Philips',7,2032,2033,4,9),(2834,'Rotex',7,2034,2035,4,9),(2835,'Saturn',7,2036,2037,4,9),(2836,'Severin',7,2038,2039,4,9),(2837,'Supra',7,2040,2041,4,9),(2838,'Tefal',7,2042,2043,4,9),(2839,'Vimar',7,2044,2045,4,9),(2840,'WEST',7,2046,2047,4,9),(2841,'Zelmer',7,2048,2049,4,9),(2842,'Другая марка',7,2050,2051,4,9),(2843,'AEG',7,2054,2055,4,9),(2844,'Ardo',7,2056,2057,4,9),(2845,'Asko',7,2058,2059,4,9),(2846,'BEKO',7,2060,2061,4,9),(2847,'Blomberg',7,2062,2063,4,9),(2848,'Bosch',7,2064,2065,4,9),(2849,'Candy',7,2066,2067,4,9),(2850,'Electrolux',7,2068,2069,4,9),(2851,'Fagor',7,2070,2071,4,9),(2852,'Gaggenau',7,2072,2073,4,9),(2853,'Gorenje',7,2074,2075,4,9),(2854,'Haier',7,2076,2077,4,9),(2855,'Hansa',7,2078,2079,4,9),(2856,'Hotpoint-Ariston',7,2080,2081,4,9),(2857,'Indesit',7,2082,2083,4,9),(2858,'Kaiser',7,2084,2085,4,9),(2859,'Kuppersbusch',7,2086,2087,4,9),(2860,'LG',7,2088,2089,4,9),(2861,'Miele',7,2090,2091,4,9),(2862,'Neff',7,2092,2093,4,9),(2863,'Samsung',7,2094,2095,4,9),(2864,'Siemens',7,2096,2097,4,9),(2865,'Smeg',7,2098,2099,4,9),(2866,'TEKA',7,2100,2101,4,9),(2867,'Whirlpool',7,2102,2103,4,9),(2868,'Zanussi',7,2104,2105,4,9),(2869,'Другая марка',7,2106,2107,4,9),(2870,'Бритвы, эпиляторы, машинки для стрижки',7,2117,2118,3,9),(2871,'Фены, укладка волос',7,2119,2120,3,9),(2872,'Весы',7,2121,2122,3,9),(2873,'Прочая техника для индивидуального ухода',7,2123,2124,3,9),(2874,'Строительство / ремонт / уборка',8,2,37,2,12),(2875,'Финансовые услуги / партнерство',8,38,39,2,12),(2876,'Перевозки / аренда транспорта',8,40,41,2,12),(2877,'Реклама / полиграфия / маркетинг / интернет',8,42,43,2,12),(2878,'Няни / сиделки',8,44,45,2,12),(2879,'Сырьё / материалы',8,46,47,2,12),(2880,'Красота / здоровье',8,48,75,2,12),(2881,'Оборудование',8,76,77,2,11),(2882,'Образование / Спорт',8,78,79,2,12),(2883,'Услуги для животных',8,80,81,2,12),(2884,'Продажа бизнеса',8,82,83,2,11),(2885,'Развлечения / Искусство / Фото / Видео',8,84,85,2,12),(2886,'Туризм / иммиграция',8,86,87,2,12),(2887,'Услуги переводчиков / набор текста',8,88,89,2,12),(2888,'Авто / мото услуги',8,90,91,2,12),(2889,'Обслуживание, ремонт техники',8,92,93,2,12),(2890,'Сетевой маркетинг',8,94,95,2,12),(2891,'Юридические услуги',8,96,97,2,12),(2892,'Прокат товаров',8,98,99,2,12),(2893,'Прочие услуги',8,100,101,2,12),(2894,'Cтроительные услуги',8,3,4,3,12),(2895,'Дизайн / архитектура',8,5,6,3,12),(2896,'Отделка / ремонт',8,7,8,3,12),(2897,'Окна / двери / балконы',8,9,10,3,12),(2898,'Сантехника / коммуникации',8,11,12,3,12),(2899,'Бытовой ремонт / уборка',8,13,30,3,12),(2900,'Вентиляция / кондиционирование',8,31,32,3,12),(2901,'Электрика',8,33,34,3,12),(2902,'Готовые конструкции',8,35,36,3,12),(2903,'Ремонт / сборка мебели / хозтоваров',8,14,15,4,12),(2904,'Пошив / ремонт одежды / обуви',8,16,17,4,12),(2905,'Пошив штор / чехлов',8,18,19,4,12),(2906,'Уборка помещений',8,20,21,4,12),(2907,'Химчистка / прачечная',8,22,23,4,12),(2908,'Вывоз мусора / снега',8,24,25,4,12),(2909,'Дезинфекция',8,26,27,4,12),(2910,'Прочие уборка / чистка',8,28,29,4,12),(2911,'Стрижки / наращивание волос',8,49,50,3,12),(2912,'Маникюр / наращивание ногтей',8,51,52,3,12),(2913,'Макияж / косметология / наращивание ресниц',8,53,54,3,12),(2914,'Медицина',8,55,56,3,12),(2915,'Массаж',8,57,58,3,12),(2916,'Красота / здоровье - прочее',8,59,60,3,12),(2917,'Услуги психолога',8,61,62,3,12),(2918,'Одежда/обувь',9,2,19,2,4),(2919,'Для свадьбы',9,20,25,2,9),(2920,'Наручные часы',9,26,227,2,9),(2921,'Аксессуары',9,228,249,2,9),(2922,'Подарки',9,250,251,2,9),(2923,'Красота / здоровье',9,252,253,2,9),(2924,'Мода разное',9,254,255,2,9),(2925,'Женская одежда',9,3,4,3,4),(2926,'Женское белье, купальники',9,5,6,3,4),(2927,'Женская обувь',9,7,8,3,4),(2928,'Одежда для беременных',9,9,10,3,4),(2929,'Мужская одежда',9,11,12,3,4),(2930,'Мужское белье',9,13,14,3,4),(2931,'Мужская обувь',9,15,16,3,4),(2932,'Головные уборы',9,17,18,3,4),(2933,'Свадебные аксессуары',9,21,22,3,9),(2934,'Свадебные платья/костюмы',9,23,24,3,9),(2935,'Nooka',9,27,28,3,9),(2936,'ORIS',9,29,30,3,9),(2937,'Omega',9,31,32,3,9),(2938,'Adriatica',9,33,34,3,9),(2939,'Orient',9,35,36,3,9),(2940,'Aigner',9,37,38,3,9),(2941,'Alfex',9,39,40,3,9),(2942,'AndyWatch',9,41,42,3,9),(2943,'Anne Klein',9,43,44,3,9),(2944,'Passion',9,45,46,3,9),(2945,'Appella',9,47,48,3,9),(2946,'Aristo',9,49,50,3,9),(2947,'Philip Laurence',9,51,52,3,9),(2948,'Philip Watch',9,53,54,3,9),(2949,'Pierre Cardin',9,55,56,3,9),(2950,'Armani',9,57,58,3,9),(2951,'Atlantic',9,59,60,3,9),(2952,'Polar',9,61,62,3,9),(2953,'Balmain',9,63,64,3,9),(2954,'Baume',9,65,66,3,9),(2955,'Q&Q',9,67,68,3,9),(2956,'Rado',9,69,70,3,9),(2957,'Bentley',9,71,72,3,9),(2958,'RaymondWeil',9,73,74,3,9),(2959,'Roberto Cavalli',9,75,76,3,9),(2960,'Breguet',9,77,78,3,9),(2961,'Breitling',9,79,80,3,9),(2962,'Romanson',9,81,82,3,9),(2963,'Bulova',9,83,84,3,9),(2964,'Seconda',9,85,86,3,9),(2965,'Sector',9,87,88,3,9),(2966,'Seiko',9,89,90,3,9),(2967,'Calvin Klein',9,91,92,3,9),(2968,'Candino',9,93,94,3,9),(2969,'Skagen',9,95,96,3,9),(2970,'Suunto',9,97,98,3,9),(2971,'Carrera y Carrera',9,99,100,3,9),(2972,'Swatch',9,101,102,3,9),(2973,'Cartier',9,103,104,3,9),(2974,'Swiss Military',9,105,106,3,9),(2975,'Casio',9,107,108,3,9),(2976,'TW Steel',9,109,110,3,9),(2977,'Tag Heuer',9,111,112,3,9),(2978,'Cerruti',9,113,114,3,9),(2979,'Certina',9,115,116,3,9),(2980,'Timex',9,117,118,3,9),(2981,'Tissot',9,119,120,3,9),(2982,'Tommy Hilfiger',9,121,122,3,9),(2983,'Ulysse Nardin',9,123,124,3,9),(2984,'Vacheron Constantin',9,125,126,3,9),(2985,'Citizen',9,127,128,3,9),(2986,'Versace',9,129,130,3,9),(2987,'Continental',9,131,132,3,9),(2988,'Victorinox',9,133,134,3,9),(2989,'Wenger',9,135,136,3,9),(2990,'DKNY',9,137,138,3,9),(2991,'Zenith',9,139,140,3,9),(2992,'Восток',9,141,142,3,9),(2993,'Diesel',9,143,144,3,9),(2994,'Заря',9,145,146,3,9),(2995,'Dolce & Gabbana',9,147,148,3,9),(2996,'Edox',9,149,150,3,9),(2997,'Полет',9,151,152,3,9),(2998,'Elite',9,153,154,3,9),(2999,'Рекорд',9,155,156,3,9),(3000,'Epos',9,157,158,3,9),(3001,'Слава',9,159,160,3,9),(3002,'Esprit',9,161,162,3,9),(3003,'Спецназ',9,163,164,3,9),(3004,'Спутник',9,165,166,3,9),(3005,'Тик-Так',9,167,168,3,9),(3006,'Другая марка',9,169,170,3,9),(3007,'Festina',9,171,172,3,9),(3008,'Fossil',9,173,174,3,9),(3009,'Frederique Constant',9,175,176,3,9),(3010,'Freelook',9,177,178,3,9),(3011,'Gc',9,179,180,3,9),(3012,'Givenchy',9,181,182,3,9),(3013,'Grovana',9,183,184,3,9),(3014,'Gucci',9,185,186,3,9),(3015,'Guess',9,187,188,3,9),(3016,'Hugo Boss',9,189,190,3,9),(3017,'IceLink',9,191,192,3,9),(3018,'Invicta',9,193,194,3,9),(3019,'Jacques Lemans',9,195,196,3,9),(3020,'Le Chic',9,197,198,3,9),(3021,'Longines',9,199,200,3,9),(3022,'Lotus',9,201,202,3,9),(3023,'Luminox',9,203,204,3,9),(3024,'Mango',9,205,206,3,9),(3025,'Marc Ecko',9,207,208,3,9),(3026,'Maurice Lacroix',9,209,210,3,9),(3027,'Michael Kors',9,211,212,3,9),(3028,'Mido',9,213,214,3,9),(3029,'Montblanc',9,215,216,3,9),(3030,'Moog',9,217,218,3,9),(3031,'Moschino',9,219,220,3,9),(3032,'Movado',9,221,222,3,9),(3033,'Nautica',9,223,224,3,9),(3034,'Nina Ricci',9,225,226,3,9),(3035,'Ювелирные изделия',9,229,244,3,9),(3036,'Сумки',9,245,246,3,9),(3037,'Бижутерия',9,247,248,3,9),(3038,'Другие аксессуары',7,204,205,4,9),(3039,'Кольца',9,230,231,4,9),(3040,'Серьги',9,232,233,4,9),(3041,'Браслеты',9,234,235,4,9),(3042,'Кулоны / подвески',9,236,237,4,9),(3043,'Броши',9,238,239,4,9),(3044,'Цепочки',9,240,241,4,9),(3045,'Прочие ювелирные изделия',9,242,243,4,9),(3046,'Косметика',8,63,64,3,12),(3047,'Парфюмерия',8,65,66,3,12),(3048,'Средства по уходу',8,67,68,3,12),(3049,'Оборудование парикмахерских / салонов красоты',8,69,70,3,12),(3050,'Товары для инвалидов',8,71,72,3,12),(3051,'Прочие товары для красоты и здоровья',8,73,74,3,12),(3052,'Антиквариат / коллекции',10,2,27,2,11),(3053,'Музыкальные инструменты',10,28,249,2,9),(3054,'Спорт / отдых',10,250,355,2,9),(3055,'Книги / журналы',10,356,357,2,9),(3056,'CD / DVD / пластинки / кассеты',10,358,359,2,9),(3057,'Билеты',10,360,361,2,11),(3058,'Поиск попутчиков',10,362,363,2,12),(3059,'Поиск групп / музыкантов',10,364,365,2,12),(3060,'Другое',10,366,367,2,11),(3061,'Антикварная мебель',10,3,4,3,11),(3062,'Букинистика',10,5,6,3,11),(3063,'Живопись',10,7,8,3,11),(3064,'Предметы искусства',10,9,10,3,11),(3065,'Коллекционирование',10,11,24,3,11),(3066,'Поделки / рукоделие',10,25,26,3,11),(3067,'Нумизматика',10,12,13,4,11),(3068,'Филателия',10,14,15,4,11),(3069,'Коллекционное оружие',10,16,17,4,11),(3070,'Спортивное коллекционирование',10,18,19,4,11),(3071,'Прочее коллекционирование',10,20,21,4,11),(3072,'Милитария',10,22,23,4,11),(3073,'Прочее',10,29,30,3,9),(3074,'Акустические гитары',10,31,114,3,9),(3075,'Бас-гитары',10,115,116,3,9),(3076,'Пианино / фортепиано / рояли',10,117,118,3,9),(3077,'Электрогитары',10,119,178,3,9),(3078,'Духовые инструменты',10,179,180,3,9),(3079,'Комбоусилители',10,181,182,3,9),(3080,'Синтезаторы',10,183,226,3,9),(3081,'Ударные инструменты',10,227,228,3,9),(3082,'Студийное оборудование',10,229,246,3,9),(3083,'Аксессуары для музыкальных инструментов',10,247,248,3,9),(3084,'A&L',10,32,33,4,9),(3085,'Adamas',10,34,35,4,9),(3086,'Admira',10,36,37,4,9),(3087,'Almires',10,38,39,4,9),(3088,'Antonio Aparicio',10,40,41,4,9),(3089,'Augusto',10,42,43,4,9),(3090,'Camps',10,44,45,4,9),(3091,'Colombo',10,46,47,4,9),(3092,'Crafter',10,48,49,4,9),(3093,'Epiphone',10,50,51,4,9),(3094,'Euphony',10,52,53,4,9),(3095,'Fender',10,54,55,4,9),(3096,'Flight',10,56,57,4,9),(3097,'Fusion',10,58,59,4,9),(3098,'Gibson',10,60,61,4,9),(3099,'Greg Bennett',10,62,63,4,9),(3100,'Hohner',10,64,65,4,9),(3101,'Ibanez',10,66,67,4,9),(3102,'Jose Ramirez',10,68,69,4,9),(3103,'Julia',10,70,71,4,9),(3104,'Kremona',10,72,73,4,9),(3105,'Manuel Contreras',10,74,75,4,9),(3106,'Manuel Rodriguez',10,76,77,4,9),(3107,'Martinez',10,78,79,4,9),(3108,'Moreno',10,80,81,4,9),(3109,'Ovation',10,82,83,4,9),(3110,'Paulino Bernabe',10,84,85,4,9),(3111,'Peavey',10,86,87,4,9),(3112,'Phil',10,88,89,4,9),(3113,'Raimundo',10,90,91,4,9),(3114,'Stagg',10,92,93,4,9),(3115,'Strunal',10,94,95,4,9),(3116,'SX',10,96,97,4,9),(3117,'Takamine',10,98,99,4,9),(3118,'Valencia',10,100,101,4,9),(3119,'Vision',10,102,103,4,9),(3120,'Walden',10,104,105,4,9),(3121,'Washburn',10,106,107,4,9),(3122,'Western OscarSchmidt',10,108,109,4,9),(3123,'Yamaha',10,110,111,4,9),(3124,'Другая марка',10,112,113,4,9),(3125,'Ltd',10,120,121,4,9),(3126,'Marshall',10,122,123,4,9),(3127,'Olp',10,124,125,4,9),(3128,'Orange',10,126,127,4,9),(3129,'Peavey',10,128,129,4,9),(3130,'Prs',10,130,131,4,9),(3131,'Schecter',10,132,133,4,9),(3132,'Squier',10,134,135,4,9),(3133,'Ural',10,136,137,4,9),(3134,'Vox',10,138,139,4,9),(3135,'Washburn',10,140,141,4,9),(3136,'Yamaha',10,142,143,4,9),(3137,'Форманта',10,144,145,4,9),(3138,'Другая марка',10,146,147,4,9),(3139,'B.C.Rich',10,148,149,4,9),(3140,'Behringer',10,150,151,4,9),(3141,'Burny',10,152,153,4,9),(3142,'Dean',10,154,155,4,9),(3143,'Epiphone',10,156,157,4,9),(3144,'Esp',10,158,159,4,9),(3145,'Fender',10,160,161,4,9),(3146,'Fernandes',10,162,163,4,9),(3147,'Gibson',10,164,165,4,9),(3148,'Godin',10,166,167,4,9),(3149,'Gretsch',10,168,169,4,9),(3150,'Ibanez',10,170,171,4,9),(3151,'Jackson',10,172,173,4,9),(3152,'Lag',10,174,175,4,9),(3153,'Line6',10,176,177,4,9),(3154,'SeqCircuits',10,184,185,4,9),(3155,'Studio_Elec',10,186,187,4,9),(3156,'Technics',10,188,189,4,9),(3157,'Yamaha',10,190,191,4,9),(3158,'Другая марка',10,192,193,4,9),(3159,'Access',10,194,195,4,9),(3160,'Akai',10,196,197,4,9),(3161,'Alesis',10,198,199,4,9),(3162,'Casio',10,200,201,4,9),(3163,'Clavia',10,202,203,4,9),(3164,'Crumar',10,204,205,4,9),(3165,'E-Mu',10,206,207,4,9),(3166,'Ensoniq',10,208,209,4,9),(3167,'GEM',10,210,211,4,9),(3168,'Kawai',10,212,213,4,9),(3169,'Ketron',10,214,215,4,9),(3170,'Korg',10,216,217,4,9),(3171,'Kurzweil',10,218,219,4,9),(3172,'M-audio',10,220,221,4,9),(3173,'Novation',10,222,223,4,9),(3174,'Roland/Boss',10,224,225,4,9),(3175,'Микшерные пульты',10,230,231,4,9),(3176,'Dj оборудование',10,232,233,4,9),(3177,'Студийные мониторы',10,234,235,4,9),(3178,'Усилители / предусилители',10,236,237,4,9),(3179,'Наушники',10,238,239,4,9),(3180,'Микрофоны',10,240,241,4,9),(3181,'Проигрыватели и рекордеры',10,242,243,4,9),(3182,'Прочее студийное оборудование',10,244,245,4,9),(3183,'Вело',10,251,258,3,9),(3184,'Лыжи / сноуборды',10,259,268,3,9),(3185,'Коньки',10,269,314,3,9),(3186,'Роликовые коньки',10,315,316,3,9),(3187,'Атлетика / фитнес',10,317,326,3,9),(3188,'Туризм',10,327,338,3,9),(3189,'Охота / рыбалка',10,339,340,3,9),(3190,'Игры с ракеткой',10,341,342,3,9),(3191,'Водные виды спорта',10,343,344,3,9),(3192,'Футбол',10,345,346,3,9),(3193,'Хоккей',10,347,348,3,9),(3194,'Единоборства / бокс',10,349,350,3,9),(3195,'Настольные игры',10,351,352,3,9),(3196,'Прочие виды спорта',10,353,354,3,9),(3197,'Велосипеды',10,252,253,4,9),(3198,'Велозапчасти',10,254,255,4,9),(3199,'Велоаксессуары',10,256,257,4,9),(3200,'Лыжи',10,260,261,4,9),(3201,'Сноуборды',10,262,263,4,9),(3202,'Ботинки',10,264,265,4,9),(3203,'Прочие товары для лыж / сноубордов',10,266,267,4,9),(3204,'Botas',10,270,271,4,9),(3205,'CCM',10,272,273,4,9),(3206,'Easton',10,274,275,4,9),(3207,'Fila',10,276,277,4,9),(3208,'Fischer',10,278,279,4,9),(3209,'GRAF',10,280,281,4,9),(3210,'Hespeler',10,282,283,4,9),(3211,'Jackson',10,284,285,4,9),(3212,'K2',10,286,287,4,9),(3213,'Libera',10,288,289,4,9),(3214,'Nike Bauer',10,290,291,4,9),(3215,'Nordway',10,292,293,4,9),(3216,'PowerSlide',10,294,295,4,9),(3217,'RBK',10,296,297,4,9),(3218,'Reebok',10,298,299,4,9),(3219,'Roces',10,300,301,4,9),(3220,'Team Canada',10,302,303,4,9),(3221,'WIFA',10,304,305,4,9),(3222,'Барс',10,306,307,4,9),(3223,'Динамо',10,308,309,4,9),(3224,'ТТ',10,310,311,4,9),(3225,'Другая марка',10,312,313,4,9),(3226,'Тренажеры',10,318,319,4,9),(3227,'Тяжелая атлетика',10,320,321,4,9),(3228,'Фитнес',10,322,323,4,9),(3229,'Спортивное питание',10,324,325,4,9),(3230,'Gps навигаторы',10,328,329,4,9),(3231,'Палатки',10,330,331,4,9),(3232,'Спальные мешки',10,332,333,4,9),(3233,'Рюкзаки',10,334,335,4,9),(3234,'Прочие туристические товары',10,336,337,4,9);
/*!40000 ALTER TABLE `category` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `city`
--

DROP TABLE IF EXISTS `city`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `city` (
  `city_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `country_id` int(11) unsigned NOT NULL DEFAULT '0',
  `region_id` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(128) NOT NULL DEFAULT '',
  PRIMARY KEY (`city_id`),
  KEY `country_id` (`country_id`),
  KEY `region_id` (`region_id`),
  CONSTRAINT `city_ibfk_1` FOREIGN KEY (`country_id`) REFERENCES `country` (`country_id`),
  CONSTRAINT `city_ibfk_2` FOREIGN KEY (`region_id`) REFERENCES `region` (`region_id`)
) ENGINE=InnoDB AUTO_INCREMENT=15789521 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `city`
--

LOCK TABLES `city` WRITE;
/*!40000 ALTER TABLE `city` DISABLE KEYS */;
INSERT INTO `city` VALUES (6,4,5,'Балларат'),(7,4,5,'Бендиго'),(8,4,5,'Варрнамбул'),(9,4,5,'Водонга'),(10,4,5,'Гилонг'),(11,4,5,'Мелтон'),(12,4,5,'Мельбурн'),(13,4,5,'Милдура'),(14,4,5,'Траралгон'),(15,4,5,'Шеппартон'),(17,4,16,'Бунбури'),(18,4,16,'Гералдтон'),(19,4,16,'Калгурли'),(20,4,16,'Мандурах'),(21,4,16,'Олбани'),(22,4,16,'Перт'),(24,4,23,'Брисбен'),(25,4,23,'Бундаберг'),(26,4,23,'Гладстон'),(27,4,23,'Каирнс'),(28,4,23,'Калундра'),(29,4,23,'Маккей'),(30,4,23,'Мариборо'),(31,4,23,'Маунт-Иса'),(32,4,23,'Рокхамптон'),(33,4,23,'Таунсвилл'),(34,4,23,'Тувумба'),(36,4,35,'Албури'),(37,4,35,'Армидейл'),(38,4,35,'Батурст'),(39,4,35,'Брокен-Хилл'),(40,4,35,'Вагга-Вагга'),(41,4,35,'Воллонгонг'),(42,4,35,'Гоулбурн'),(43,4,35,'Дуббо-Дуббо'),(44,4,35,'Коффс-Харбор'),(45,4,35,'Куэнбиан'),(46,4,35,'Лисмор'),(47,4,35,'Ньюкастл'),(48,4,35,'Оранж'),(49,4,35,'Сидней'),(50,4,35,'Тамворт'),(52,4,51,'Алис Спрингс'),(53,4,51,'Дарвин'),(55,4,54,'Девонпорт'),(56,4,54,'Лаункестон'),(57,4,54,'Хобарт'),(59,4,58,'Канберра'),(61,4,60,'Аделаида'),(62,4,60,'Маунт-Гамбир'),(65,63,64,'Вена'),(67,63,66,'Велс'),(68,63,66,'Линц'),(69,63,66,'Стир'),(71,63,70,'Зальцбург'),(73,63,72,'Виллач'),(74,63,72,'Клагенфурт'),(76,63,75,'Венер-Нойштадт'),(78,63,77,'Грац'),(79,63,77,'Иннсбрук'),(80,63,77,'Леобен'),(83,81,82,'Агдам'),(84,81,82,'Агдаш'),(85,81,82,'Агджабеди'),(86,81,82,'Аджикенд'),(87,81,82,'Акстафа'),(88,81,82,'Али-Байрамлы'),(89,81,82,'Алунитаг'),(90,81,82,'Артем-Остров'),(91,81,82,'Астара'),(92,81,82,'Ахсу'),(93,81,82,'Баку'),(94,81,82,'Балаханы'),(95,81,82,'Банк'),(96,81,82,'Барда'),(97,81,82,'Белоканы'),(98,81,82,'Бинагади'),(99,81,82,'Бирмай'),(100,81,82,'Бузовна'),(101,81,82,'Варташен'),(102,81,82,'Гадрут'),(103,81,82,'Геокчай'),(104,81,82,'Гобустан'),(105,81,82,'Горадиз'),(106,81,82,'Гэтргян'),(107,81,82,'Дальмамедли'),(108,81,82,'Дашкесан'),(109,81,82,'Джалилабад'),(110,81,82,'Джебраил'),(111,81,82,'Дивичи'),(112,81,82,'Евлах'),(113,81,82,'Ждановск'),(114,81,82,'Закаталы'),(115,81,82,'Зангелан'),(116,81,82,'Зардоб'),(117,81,82,'Имишли'),(118,81,82,'Исмаиллы'),(119,81,82,'Истису'),(120,81,82,'Казанбулак'),(121,81,82,'Казах'),(122,81,82,'Кази-Магомед'),(123,81,82,'Карачала'),(124,81,82,'Касум-Исмаилов'),(125,81,82,'Кахи'),(126,81,82,'Кедабек'),(127,81,82,'Кельбаджар'),(128,81,82,'Кергез'),(129,81,82,'Кировобад'),(130,81,82,'Кировск'),(131,81,82,'Кировский'),(132,81,82,'Куба'),(133,81,82,'Кубатлы'),(134,81,82,'Куткашен'),(135,81,82,'Кюрдамир'),(136,81,82,'Ленкорань'),(137,81,82,'Лерик'),(138,81,82,'Маргуни'),(139,81,82,'Мардакерт'),(140,81,82,'Масаллы'),(141,81,82,'Маштага'),(142,81,82,'Мингечаур'),(143,81,82,'Мир-Башир'),(144,81,82,'Нафталан'),(145,81,82,'Нефтечала'),(146,81,82,'Пушкино'),(147,81,82,'Саатлы'),(148,81,82,'Сабирабад'),(149,81,82,'Сабуичи'),(150,81,82,'Сальяны'),(151,81,82,'Сиазань'),(152,81,82,'Сумгаит'),(153,81,82,'Тауз'),(154,81,82,'Уджары'),(155,81,82,'Физули'),(156,81,82,'Ханлар'),(157,81,82,'Хачмас'),(158,81,82,'Худат'),(159,81,82,'Шамхор'),(160,81,82,'Шаумяновск'),(161,81,82,'Шеки'),(162,81,82,'Шемаха'),(163,81,82,'Ярдымлы'),(165,81,164,'Степанокерт'),(166,81,164,'Шуша'),(168,81,167,'Джульфа'),(169,81,167,'Ильичевск'),(170,81,167,'Нахичевань'),(171,81,167,'Ордубад'),(172,81,167,'Шахбуз'),(175,173,174,'Ангуилья'),(176,173,174,'Сомбреро'),(179,177,178,'Азул'),(180,177,178,'Байя-Бланка'),(181,177,178,'Буэнос-Айрес'),(182,177,178,'Кампана'),(183,177,178,'Ла-Плата'),(184,177,178,'Мар-дель-Плата'),(185,177,178,'Мерседес'),(186,177,178,'Некочеа'),(187,177,178,'Олаварриа'),(188,177,178,'Пергамино'),(189,177,178,'Пунта-Альта'),(190,177,178,'Сан-Николас'),(191,177,178,'Тандил'),(192,177,178,'Трес-Арройос'),(193,177,178,'Чивилкой'),(195,177,194,'Хухуй'),(197,177,196,'Катамарка'),(199,177,198,'Альта-Грасия'),(200,177,198,'Вилла-Мариа'),(201,177,198,'Кордова'),(202,177,198,'Рио-Куарто'),(203,177,198,'Сан-Франсиско'),(205,177,204,'Гойя'),(206,177,204,'Корриентес'),(208,177,207,'Женераль-Рока'),(209,177,207,'Санта-Роза'),(211,177,210,'Ла-Риойя'),(213,177,212,'Мендоза'),(214,177,212,'Сан-Рафаель'),(216,177,215,'Посадас'),(218,177,217,'Ньюкуэн'),(220,177,219,'Салта'),(222,177,221,'Сан-Луис'),(224,177,223,'Сан-Хуан'),(226,177,225,'Рио-Галльегос'),(228,177,227,'Росарио'),(229,177,227,'Санта-Фе'),(231,177,230,'Сантьяго-дел-Эстеро'),(233,177,232,'Тукуман'),(235,177,234,'Формоза'),(237,177,236,'Пресиденсиа-Рокуэ-Сенз'),(238,177,236,'Ресистенсиа'),(240,177,239,'Комодоро-Ривадавия'),(242,177,241,'Конкордиа'),(243,177,241,'Консепсион-дель-Уругвай'),(244,177,241,'Парана'),(247,245,246,'Ереван'),(250,248,249,'Антополь'),(251,248,249,'Барановичи'),(252,248,249,'Белоозерск'),(253,248,249,'Береза'),(254,248,249,'Береза Картуска'),(255,248,249,'Брест'),(256,248,249,'Высокое'),(257,248,249,'Ганцевичи'),(258,248,249,'Городище'),(259,248,249,'Давид-Городок'),(260,248,249,'Домачево'),(261,248,249,'Дрогичин'),(262,248,249,'Жабинка'),(263,248,249,'Ивацевичи'),(264,248,249,'Каменец'),(265,248,249,'Кобрин'),(266,248,249,'Лунинец'),(267,248,249,'Ляховичи'),(268,248,249,'Малорита'),(269,248,349,'Минск'),(270,248,249,'Пружаны'),(271,248,249,'Столин'),(273,248,272,'Барань'),(274,248,272,'Бегомль'),(275,248,272,'Бешенковичи'),(276,248,272,'Богушевск'),(277,248,272,'Браслав'),(278,248,272,'Верхнедвинск'),(279,248,272,'Ветрино'),(280,248,272,'Видзы'),(281,248,272,'Витебск'),(282,248,272,'Воропаево'),(283,248,272,'Глубокое'),(284,248,272,'Городок'),(285,248,272,'Дисна'),(286,248,272,'Докшицы'),(287,248,272,'Друя'),(288,248,272,'Дубровно'),(289,248,272,'Езерище'),(290,248,272,'Лепель'),(291,248,272,'Лиозно'),(292,248,272,'Миоры'),(293,248,272,'Новополоцк'),(294,248,272,'Орша'),(295,248,272,'Полоцк'),(296,248,272,'Поставы'),(297,248,272,'Россоны'),(298,248,272,'Сенно'),(299,248,272,'Толочин'),(300,248,272,'Ушачи'),(301,248,272,'Чашники'),(302,248,272,'Шарковщина'),(303,248,272,'Шумилино'),(305,248,304,'Белицк'),(306,248,304,'Большевик'),(307,248,304,'Брагин'),(308,248,304,'Буда-Кошелево'),(309,248,304,'Василевичи'),(310,248,304,'Васильевка'),(311,248,304,'Ветка'),(312,248,304,'Гомель'),(313,248,304,'Добруш'),(314,248,304,'Ельск'),(315,248,304,'Житковичи'),(316,248,304,'Жлобин'),(317,248,304,'Калинковичи'),(318,248,304,'Корма'),(319,248,304,'Лельчицы'),(320,248,304,'Лоев'),(321,248,304,'Мозырь'),(322,248,304,'Наровля'),(323,248,304,'Октябрьский'),(324,248,304,'Петриков'),(325,248,304,'Речица'),(326,248,304,'Рогачев'),(327,248,304,'Светлогорск'),(328,248,304,'Хойники'),(329,248,304,'Чечерск'),(331,248,330,'Большая Берестовица'),(332,248,330,'Волковыск'),(333,248,330,'Вороново'),(334,248,330,'Гродно'),(335,248,330,'Желудок'),(336,248,330,'Зельва'),(337,248,330,'Ивье'),(338,248,330,'Козловщина'),(339,248,330,'Кореличи'),(340,248,330,'Лида'),(341,248,330,'Мосты'),(342,248,330,'Новогрудок'),(343,248,330,'Островец'),(344,248,330,'Ошмяны'),(345,248,330,'Свислочь'),(346,248,330,'Слоним'),(347,248,330,'Сморгонь'),(348,248,330,'Щучин'),(350,248,349,'Березино'),(351,248,349,'Бобр'),(352,248,349,'Борисов'),(353,248,349,'Вилейка'),(354,248,349,'Воложин'),(355,248,349,'Городея'),(356,248,349,'Дзержинск'),(357,248,349,'Жодино'),(358,248,349,'Заславль'),(359,248,349,'Зеленый Бор'),(360,248,349,'Ивенец'),(361,248,349,'Клецк'),(362,248,349,'Копыль'),(363,248,349,'Крупки'),(364,248,349,'Логойск'),(365,248,349,'Марьина Горка'),(366,248,349,'Пинск'),(367,248,349,'Молодечно'),(368,248,349,'Мядель'),(369,248,349,'Несвиж'),(370,248,349,'Слуцк'),(371,248,349,'Смолевичи'),(372,248,349,'Солигорск'),(373,248,349,'Старые Дороги'),(374,248,349,'Столбцы'),(375,248,349,'Узда'),(376,248,349,'Червень'),(378,248,377,'Белыничи'),(379,248,377,'Бобруйск'),(380,248,377,'Быхов'),(381,248,377,'Глуск'),(382,248,377,'Глуша'),(383,248,377,'Горки'),(384,248,377,'Гродзянка'),(385,248,377,'Елизово'),(386,248,377,'Кировск'),(387,248,377,'Климовичи'),(388,248,377,'Кличев'),(389,248,377,'Костюковичи'),(390,248,377,'Краснополье'),(391,248,377,'Кричев'),(392,248,377,'Круглое'),(393,248,377,'Могилев'),(394,248,377,'Мстиславль'),(395,248,377,'Осиповичи'),(396,248,377,'Славгород'),(397,248,377,'Хотимск'),(398,248,377,'Чаусы'),(399,248,377,'Чериков'),(400,248,377,'Шклов'),(403,401,402,'Белиз-Сити'),(406,404,405,'Антверпен'),(407,404,405,'Мешлен'),(409,404,408,'Брюссель'),(411,404,410,'Алост'),(412,404,410,'Сант-Никлас'),(414,404,413,'Бруже'),(415,404,413,'Куртра'),(416,404,413,'Роселар'),(418,404,417,'Генк'),(420,404,419,'Льеж'),(421,404,419,'Сераинг'),(423,404,422,'Ла-Лювьер'),(424,404,422,'Шарлеруа'),(427,425,426,'Гамильтон'),(430,428,429,'Бургас'),(432,428,431,'Варна'),(434,428,433,'Велико Тарново'),(435,428,433,'Горна Джумая'),(437,428,436,'Видин'),(439,428,438,'Габрово'),(441,428,440,'Пазарджик'),(443,428,442,'Перник'),(445,428,444,'Плевен'),(447,428,446,'Асеновград'),(448,428,446,'Пловдив'),(450,428,449,'Рус'),(452,428,451,'Силистра'),(454,428,453,'Сливен'),(456,428,455,'София'),(458,428,457,'Казанлак'),(459,428,457,'Стара Загора'),(461,428,460,'Димитровград'),(462,428,460,'Кхасково'),(464,428,463,'Шумен'),(466,428,465,'Ямбол'),(469,467,468,'Рио-Бранко'),(471,467,470,'Арапирака'),(472,467,470,'Макейо'),(474,467,473,'Манаус'),(476,467,475,'Макапа'),(478,467,477,'Алагойнас'),(479,467,477,'Байя'),(480,467,477,'Виториа-да-Конкиста'),(481,467,477,'Витория'),(482,467,477,'Жекуи'),(483,467,477,'Жуазейро'),(484,467,477,'Илхеус'),(485,467,477,'Итабуна'),(486,467,477,'Итапетинга'),(487,467,477,'Сальвадор'),(488,467,477,'Санта-Мария'),(489,467,477,'Фейра-де-Сантана'),(491,467,490,'Анаполис'),(492,467,490,'Гояния'),(494,467,493,'Бакабаль'),(495,467,493,'Кахиас'),(496,467,493,'Сан-Луис'),(498,467,497,'Куйяба'),(500,467,499,'Кампо-Гранде'),(501,467,499,'Корумба'),(502,467,499,'Трес-Лагоас'),(504,467,503,'Арха'),(505,467,503,'Барбасена'),(506,467,503,'Белу-Оризонти'),(507,467,503,'Варгина'),(508,467,503,'Говернадор-Валадарес'),(509,467,503,'Дивинополис'),(510,467,503,'Жуис-де-Фора'),(511,467,503,'Итажуба'),(512,467,503,'Каратинга'),(513,467,503,'Катагуасес'),(514,467,503,'Монтес-Кларос'),(515,467,503,'Пассос'),(516,467,503,'Покос-де-Кальдас'),(517,467,503,'Сан-Жоау-дель-Рей'),(518,467,503,'Теофилу-Отони'),(519,467,503,'Убераба'),(520,467,503,'Уберландиа'),(522,467,521,'Белен'),(523,467,521,'Сантарем'),(525,467,524,'Жоао-Пессоа'),(526,467,524,'Кажазейрас'),(528,467,527,'Кампина-Гранде'),(529,467,527,'Куритиба'),(530,467,527,'Лондрина'),(531,467,527,'Паранагуа'),(532,467,527,'ПонтахГросса'),(534,467,533,'Гаранхунс'),(535,467,533,'Каруару'),(536,467,533,'Олинда'),(537,467,533,'Петролина'),(538,467,533,'Ресифе'),(540,467,539,'Парнаиба'),(541,467,539,'Тересина'),(543,467,542,'Вольта-Редонда'),(544,467,542,'Кампос'),(545,467,542,'Масау'),(546,467,542,'Нитерои'),(547,467,542,'Параиба-ду-Сул'),(548,467,542,'Петрополис'),(549,467,542,'Рио-де-Жанейро'),(551,467,550,'Кайку'),(552,467,550,'Моссору'),(553,467,550,'Наталь'),(555,467,554,'Алегрете'),(556,467,554,'Баге'),(557,467,554,'Кахиас-до-Сул'),(558,467,554,'Качоэйра-до-Сул'),(559,467,554,'Круз-Альта'),(560,467,554,'Пассо-Фундо'),(561,467,554,'Пелотас'),(562,467,554,'Порту-Алегри'),(563,467,554,'Рио-Гранде'),(564,467,554,'Сан-Леопольдо'),(565,467,554,'Сантана-до-Ливраменто'),(566,467,554,'Санто-Ангело'),(567,467,554,'Уругвайяна'),(569,467,568,'Порту-Велью'),(571,467,570,'Аракатуба'),(572,467,570,'Арараквира'),(573,467,570,'Барретос'),(574,467,570,'Бауру'),(575,467,570,'Бебедоуро'),(576,467,570,'Ботукату'),(577,467,570,'Жау'),(578,467,570,'Жундиаи'),(579,467,570,'Кампинас'),(580,467,570,'Катандува'),(581,467,570,'Лимейра'),(582,467,570,'Линс'),(583,467,570,'Марилия'),(584,467,570,'Пиракикаба'),(585,467,570,'Пресиденте-Пруденте'),(586,467,570,'Рибейрао-Прето'),(587,467,570,'Сан-Бернардо-ду-Кампу'),(588,467,570,'Сан-Жоау-да-Боа-Виста'),(589,467,570,'Сан-Паулу'),(590,467,570,'Сан-Хосе-до-Рио-Прето'),(591,467,570,'Сантос'),(592,467,570,'Сорокаба'),(593,467,570,'Таубати'),(594,467,570,'Франка'),(596,467,595,'Арагуари'),(597,467,595,'Блуменау'),(598,467,595,'Жоинвиле'),(599,467,595,'Итажаи'),(600,467,595,'Лахес'),(601,467,595,'Тубарао'),(602,467,595,'Флорианополис'),(604,467,603,'Жуазейру-ду-Норте'),(605,467,603,'Игуату'),(606,467,603,'Крато'),(607,467,603,'Собраль'),(608,467,603,'Форталеза'),(610,467,609,'Аракажу'),(612,467,611,'Бразилия'),(614,467,613,'Качоэйру-де-Итапемирим'),(615,467,613,'Колатина'),(618,616,617,'Абингдон'),(619,616,617,'Айлесбюри'),(620,616,617,'Аккрингтон'),(621,616,617,'Алдершот'),(622,616,617,'Алтринчам'),(623,616,617,'Андовер'),(624,616,617,'Аштон-андер-Лин'),(625,616,617,'Аштон-ин-Макерфилд'),(626,616,617,'Байдфорд'),(627,616,617,'Банбери'),(628,616,617,'Банстед'),(629,616,617,'Барнли'),(630,616,617,'Барнсли'),(631,616,617,'Барнстапл'),(632,616,617,'Барроу-ин-Фарнесс'),(633,616,617,'Бартон-он-Трент'),(634,616,617,'Бас'),(635,616,617,'Басингсток'),(636,616,617,'Батли'),(637,616,617,'Беверли'),(638,616,617,'Бедворт'),(639,616,617,'Бедфорд'),(640,616,617,'Бексли'),(641,616,617,'Бервик-он-Твид'),(642,616,617,'Биркенхед'),(643,616,617,'Бирмингем'),(644,616,617,'Бистон'),(645,616,617,'Бишоп-Окленд'),(646,616,617,'Блэкберн'),(647,616,617,'Блэкпул'),(648,616,617,'Болтон'),(649,616,617,'Боримут'),(650,616,617,'Бостон'),(651,616,617,'Брадфорд'),(652,616,617,'Брайтон'),(653,616,617,'Брентвуд'),(654,616,617,'Бриджуотер'),(655,616,617,'Бридлингтон'),(656,616,617,'Бристоль'),(657,616,617,'Бромсгров'),(658,616,617,'Бури'),(659,616,617,'Бури-Сант-Эдмондс'),(660,616,617,'Бутл'),(661,616,617,'Валласи'),(662,616,617,'Валлсенд'),(663,616,617,'Валсалл'),(664,616,617,'Валтон'),(665,616,617,'Варвик'),(666,616,617,'Варрингтон'),(667,616,617,'Ватерлоо'),(668,616,617,'Ватфорд'),(669,616,617,'Вейкфилд'),(670,616,617,'Велвин-Гарден-Сити'),(671,616,617,'Вест-Бромвич'),(672,616,617,'Вестон-супер-Мар'),(673,616,617,'Виган'),(674,616,617,'Виднес'),(675,616,617,'Вимоут'),(676,616,617,'Виндзор'),(677,616,617,'Винчестер'),(678,616,617,'Висбеч'),(679,616,617,'Вокинг'),(680,616,617,'Вокингем'),(681,616,617,'Ворчестер'),(682,616,617,'Вулвергемптон'),(683,616,617,'Гейтшид'),(684,616,617,'Глоссоп'),(685,616,617,'Глочестер'),(686,616,617,'Годалминг'),(687,616,617,'Голборн'),(688,616,617,'Госпорт'),(689,616,617,'Госфорт'),(690,616,617,'Грантхем'),(691,616,617,'Гримсби'),(692,616,617,'Грисби'),(693,616,617,'Грэйт-Ярмут'),(694,616,617,'Гуилдфорд'),(695,616,617,'Дадли'),(696,616,617,'Дарлингтон'),(697,616,617,'Дартфорд'),(698,616,617,'Дерби'),(699,616,617,'Джерроу'),(700,616,617,'Донкастер'),(701,616,617,'Дувр'),(702,616,617,'Дурхем'),(703,616,617,'Дьюсбури'),(704,616,617,'Ексетер'),(705,616,617,'Ипсвич'),(706,616,617,'Ист-Гринстед'),(707,616,617,'Истлейг'),(708,616,617,'Йорк'),(709,616,617,'Каннок'),(710,616,617,'Карлисл'),(711,616,617,'Карлтон'),(712,616,617,'Кастлфорд'),(713,616,617,'Кейгли'),(714,616,617,'Кембридж'),(715,616,617,'Кендал'),(716,616,617,'Кенилворт'),(717,616,617,'Кентербери'),(718,616,617,'Кеттеринг'),(719,616,617,'Киддерминстер'),(720,616,617,'Кингс-Линн'),(721,616,617,'Киркби'),(722,616,617,'Клактон-Он-Си'),(723,616,617,'Ковентри'),(724,616,617,'Колчестер'),(725,616,617,'Корби'),(726,616,617,'Кристчерч'),(727,616,617,'Кросби'),(728,616,617,'Кроули'),(729,616,617,'Крю'),(730,616,617,'Ланкастер'),(731,616,617,'Лаустофт'),(732,616,617,'Лейг'),(733,616,617,'Ливерпуль'),(734,616,617,'Лидс'),(735,616,617,'Линкольн'),(736,616,617,'Литам-Сант-Аннес'),(737,616,617,'Литерхед'),(738,616,617,'Литлгемптон'),(739,616,617,'Личфилд'),(740,616,617,'Лондон'),(741,616,617,'Лугборо'),(742,616,617,'Лутон'),(743,616,617,'Майденхед'),(744,616,617,'Майдстоун'),(745,616,617,'Макклесфилд'),(746,616,617,'Мансфилд'),(747,616,617,'Манчестер'),(748,616,617,'Маргейт'),(749,616,617,'Миддлсбро'),(750,616,617,'Миддлтаун'),(751,616,617,'Морли'),(752,616,617,'Наилси'),(753,616,617,'Норвич'),(754,616,617,'Нортвич'),(755,616,617,'Нортгемптон'),(756,616,617,'Нортфлит'),(757,616,617,'Ноттингем'),(758,616,617,'Нунитон'),(759,616,617,'Ньюарк'),(760,616,617,'Ньюкасл-он-Тайн'),(761,616,617,'Ньюкастле-андер-Лим'),(762,616,617,'Ньюпорт'),(763,616,617,'Минск'),(764,616,617,'Олдхам'),(765,616,617,'Ормскирк'),(766,616,617,'Оссетт'),(767,616,617,'Падси'),(768,616,617,'Пайнтон'),(769,616,617,'Пензанк'),(770,616,617,'Петерборо'),(771,616,617,'Петерли'),(772,616,617,'Плимут'),(773,616,617,'Портсмут'),(774,616,617,'Прескот'),(775,616,617,'Престон'),(776,616,617,'Пул'),(777,616,617,'Рагби'),(778,616,617,'Радклифф'),(779,616,617,'Райд'),(780,616,617,'Рамсгейт'),(781,616,617,'Ранкорн'),(782,616,617,'Реддитч'),(783,616,617,'Редкар'),(784,616,617,'Рейгейт'),(785,616,617,'Рейли'),(786,616,617,'Ридинг'),(787,616,617,'Роиал-Тунбридж-Уэллс'),(788,616,617,'Ротерхам'),(789,616,617,'Рочестер'),(790,616,617,'Рошдейл'),(791,616,617,'Салфорд'),(792,616,617,'Сандерленд'),(793,616,617,'Сант-Албанс'),(794,616,617,'Сант-Аустель'),(795,616,617,'Сант-Хеленс'),(796,616,617,'Саттон-ин-Ашфилд'),(797,616,617,'Саут-Шилдс'),(798,616,617,'Саутгэмптон'),(799,616,617,'Саутенд-он-Си'),(800,616,617,'Саутпорт'),(801,616,617,'Сванли'),(802,616,617,'Свиндон'),(803,616,617,'Свинтон'),(804,616,617,'Севеноакс'),(805,616,617,'Сейл'),(806,616,617,'Сейлсбури'),(807,616,617,'Ситтингборн'),(808,616,617,'Сихам'),(809,616,617,'Сканторп'),(810,616,617,'Скарборо'),(811,616,617,'Слоуг'),(812,616,617,'Солихалл'),(813,616,617,'Стайнс'),(814,616,617,'Стаффорд'),(815,616,617,'Стенли'),(816,616,617,'Стивенейдж'),(817,616,617,'Стокпорт'),(818,616,617,'Стоктон'),(819,616,617,'Стоктон-он-Тис'),(820,616,617,'Стоурбридж'),(821,616,617,'Стратфорд-он-Эйвон'),(822,616,617,'Стретфорд'),(823,616,617,'Строуд'),(824,616,617,'Татчхем'),(825,616,617,'Таунтон'),(826,616,617,'Тонбридж'),(827,616,617,'Торкуэй'),(828,616,617,'Торнаби-он-Тис'),(829,616,617,'Траубридж'),(830,616,617,'Уайтхейен'),(831,616,617,'Урмстон'),(832,616,617,'Фалмаут'),(833,616,617,'Фарнборо'),(834,616,617,'Фарнворт'),(835,616,617,'Фарнхем'),(836,616,617,'Фейрхам'),(837,616,617,'Феликстов'),(838,616,617,'Флитвуд'),(839,616,617,'Фолькстон'),(840,616,617,'Формби'),(841,616,617,'Хавант'),(842,616,617,'Хаддерсфилд'),(843,616,617,'Хазел-Гров'),(844,616,617,'Хай-Викомб'),(845,616,617,'Халифакс'),(846,616,617,'Харлоу'),(847,616,617,'Харпенден'),(848,616,617,'Харрогейт'),(849,616,617,'Хартлепул'),(850,616,617,'Хастингс'),(851,616,617,'Хатфилд'),(852,616,617,'Хейвардс-Хит'),(853,616,617,'Хейлсовен'),(854,616,617,'Хемел-Хемпстед'),(855,616,617,'Херефорд'),(856,616,617,'Хертфорд'),(857,616,617,'Хивуд'),(858,616,617,'Хид'),(859,616,617,'Хиндли'),(860,616,617,'Хинкли'),(861,616,617,'Хойлайк'),(862,616,617,'Хорсхем'),(863,616,617,'Хоув'),(864,616,617,'Хьютон-вит-Роби'),(865,616,617,'Чапелтаун'),(866,616,617,'Чатхем'),(867,616,617,'Челмсфорд'),(868,616,617,'Челтенхам'),(869,616,617,'Честер'),(870,616,617,'Честерфилд'),(871,616,617,'Чешант'),(872,616,617,'Чичестер'),(873,616,617,'Чорли'),(874,616,617,'Шеффилд'),(875,616,617,'Шипли'),(876,616,617,'Шревсбури'),(877,616,617,'Эллесмер-Порт'),(878,616,617,'Эпсом'),(879,616,617,'Эшер'),(880,616,617,'Эшфорд'),(882,616,881,'Баллимена'),(883,616,881,'Бангор'),(884,616,881,'Белфаст'),(885,616,881,'Колерайн'),(886,616,881,'Лондондерри'),(887,616,881,'Ньюри'),(888,616,881,'Ньютаунабби'),(890,616,889,'Абердар'),(891,616,889,'Барри'),(892,616,889,'Врексхам'),(893,616,889,'Кардифф'),(894,616,889,'Керфилли'),(895,616,889,'Мертир-Тидвил'),(896,616,889,'Порт Талбот'),(897,616,889,'Рондда'),(898,616,889,'Сванси'),(900,616,899,'Абердин'),(901,616,899,'Айрдри'),(902,616,899,'Арброат'),(903,616,899,'Глазго'),(904,616,899,'Гринок'),(905,616,899,'Данди'),(906,616,899,'Данфермлин'),(907,616,899,'Думбартон'),(908,616,899,'Думфрис'),(909,616,899,'Инвернесс'),(910,616,899,'Ирвин'),(911,616,899,'Ист-Килбрайд'),(912,616,899,'Камберналд'),(913,616,899,'Килмарнок'),(914,616,899,'Кирккалди'),(915,616,899,'Коатбридж'),(916,616,899,'Мотервелл'),(917,616,899,'Паисли'),(918,616,899,'Перт'),(919,616,899,'Ренфро'),(920,616,899,'Стирлинг'),(921,616,899,'Фалкирк'),(922,616,899,'Хамилтон'),(923,616,899,'Эдинбург'),(926,924,925,'Сольнок'),(927,924,925,'Яшберены'),(929,924,928,'Печ'),(931,924,930,'Байя'),(932,924,930,'Кечкемет'),(933,924,930,'Кискунфелегихаза'),(935,924,934,'Бекешсаба'),(937,924,936,'Мишкольц'),(938,924,936,'Озд'),(940,924,939,'Будапешт'),(942,924,941,'Жомбатели'),(944,924,943,'Вешпрем'),(946,924,945,'Сопрон'),(948,924,947,'Залегержег'),(949,924,947,'Нагиканиша'),(951,924,950,'Татабаня'),(953,924,952,'Салготарян'),(955,924,954,'Сеглед'),(957,924,956,'Ныйрегихаза'),(959,924,958,'Дунауйварош'),(960,924,958,'Шекесфехервар'),(962,924,961,'Дебрецен'),(963,924,961,'Хайдубошормены'),(965,924,964,'Гионгиос'),(966,924,964,'Эгер'),(968,924,967,'Сегед'),(969,924,967,'Сентес'),(970,924,967,'Ходмезовасархели'),(973,971,972,'Кан-То'),(974,971,972,'Рач-Гиа'),(976,971,975,'Нам-Динх'),(977,971,975,'Хайфон'),(978,971,975,'Ханой'),(980,971,979,'Вунг-Тау'),(981,971,979,'Сайгон'),(983,971,982,'Дананг'),(984,971,982,'Нячанг'),(985,971,982,'Пхан-Тхит'),(986,971,982,'Туи-Хоа'),(988,971,987,'Винь'),(989,971,987,'Хуэ'),(991,971,990,'Хон-Гаи'),(993,971,992,'Плейку'),(996,994,995,'Гонаивес'),(998,994,997,'Джереми'),(1000,994,999,'Порт-де-Пакс'),(1002,994,1001,'Порт-о-Пренс'),(1004,994,1003,'Лес-Кайес'),(1006,994,1005,'Джакмель'),(1009,1007,1008,'Пойнт-а-Питр'),(1011,1007,1010,'Бассе-Терре'),(1014,1012,1013,'Берлин'),(1016,1012,1015,'Бранденбург'),(1017,1012,1015,'Котбус'),(1018,1012,1015,'Потсдам'),(1019,1012,1015,'Франкфурт-на-Одере'),(1021,1012,1020,'Висмар'),(1022,1012,1020,'Грейфсвальд'),(1023,1012,1020,'Гюстров'),(1024,1012,1020,'Нойебранденбург'),(1025,1012,1020,'Росток'),(1026,1012,1020,'Шверин'),(1027,1012,1020,'Штральзунд'),(1029,1012,1028,'Баутцен'),(1030,1012,1028,'Герлиц'),(1031,1012,1028,'Дрезден'),(1032,1012,1028,'Лейпциг'),(1033,1012,1028,'Мейссен'),(1034,1012,1028,'Пирна'),(1035,1012,1028,'Плауэн'),(1036,1012,1028,'Радебюль'),(1037,1012,1028,'Риса'),(1038,1012,1028,'Фрейтал'),(1039,1012,1028,'Хемниц'),(1040,1012,1028,'Хойерсверда'),(1041,1012,1028,'Цвикау'),(1042,1012,1028,'Циттау'),(1044,1012,1043,'Бернбург'),(1045,1012,1043,'Вейссенфельс'),(1046,1012,1043,'Виттенберг'),(1047,1012,1043,'Дессау'),(1048,1012,1043,'Зейтз'),(1049,1012,1043,'Магдебург'),(1050,1012,1043,'Мерсебург'),(1051,1012,1043,'Халберштадт'),(1052,1012,1043,'Шонебек'),(1054,1012,1053,'Альтенбург'),(1055,1012,1053,'Веймар'),(1056,1012,1053,'Вернигероде'),(1057,1012,1053,'Гера'),(1058,1012,1053,'Гота'),(1059,1012,1053,'Йена'),(1060,1012,1053,'Мёлхаусен'),(1061,1012,1053,'Нордхаузен'),(1062,1012,1053,'Эизенах'),(1063,1012,1053,'Эрфурт'),(1065,1012,1064,'Амберг'),(1066,1012,1064,'Аугсбург'),(1067,1012,1064,'Ашхаффенбург'),(1068,1012,1064,'Бамберг'),(1069,1012,1064,'Бэйреут'),(1070,1012,1064,'Вейден'),(1071,1012,1064,'Вюрцбург'),(1072,1012,1064,'Ерланген'),(1073,1012,1064,'Ингольштадт'),(1074,1012,1064,'Кемптен'),(1075,1012,1064,'Кобург'),(1076,1012,1064,'Ландсхут'),(1077,1012,1064,'Нюрнберг'),(1078,1012,1064,'Пасау'),(1079,1012,1064,'Регенсбург'),(1080,1012,1064,'Фурт'),(1081,1012,1064,'Хоф'),(1082,1012,1064,'Швейнфурт'),(1084,1012,1083,'Баден-Баден'),(1085,1012,1083,'Гоппинген'),(1086,1012,1083,'Зинделфинген'),(1087,1012,1083,'Карлсруэ'),(1088,1012,1083,'Констанц'),(1089,1012,1083,'Людвигсбург'),(1090,1012,1083,'Мангейм'),(1091,1012,1083,'Пфорзхейм'),(1092,1012,1083,'Рютлинген'),(1093,1012,1083,'Тюбинген'),(1094,1012,1083,'Ульм'),(1095,1012,1083,'Фрейберг'),(1096,1012,1083,'Фридрихсхафен'),(1097,1012,1083,'Хейдельберг'),(1098,1012,1083,'Хейденхейм-ан-дер-Бренц'),(1099,1012,1083,'Хейлбронн'),(1100,1012,1083,'Швабиш-Гмунд'),(1101,1012,1083,'Штутгарт'),(1102,1012,1083,'Эсслинген'),(1104,1012,1103,'Бремен'),(1105,1012,1103,'Бремерхавен'),(1107,1012,1106,'Гамбург'),(1109,1012,1108,'Бад-Хомбург-вор-дер-Хох'),(1110,1012,1108,'Висбаден'),(1111,1012,1108,'Гиссен'),(1112,1012,1108,'Дармштадт'),(1113,1012,1108,'Кассель'),(1114,1012,1108,'Марбург-ан-дер-Лан'),(1115,1012,1108,'Оффенбах'),(1116,1012,1108,'Руссельшейм'),(1117,1012,1108,'Франкфурт-на-Майне'),(1118,1012,1108,'Фульда'),(1119,1012,1108,'Ханау'),(1121,1012,1120,'Вильгельмсхавен'),(1122,1012,1120,'Волфенбуттель'),(1123,1012,1120,'Вольфсбург'),(1124,1012,1120,'Ганновер'),(1125,1012,1120,'Гослар'),(1126,1012,1120,'Готтинген'),(1127,1012,1120,'Дельменхорст'),(1128,1012,1120,'Куксхавен'),(1129,1012,1120,'Лунебург'),(1130,1012,1120,'Нордхорн'),(1131,1012,1120,'Олденбург'),(1132,1012,1120,'Оснабрюк'),(1133,1012,1120,'Селл'),(1134,1012,1120,'Хамельн'),(1135,1012,1120,'Хильдешейм'),(1136,1012,1120,'Эмден'),(1138,1012,1137,'Бад-Крейцнах'),(1139,1012,1137,'Вормс'),(1140,1012,1137,'Кайзерслаутерн'),(1141,1012,1137,'Кобленц'),(1142,1012,1137,'Людвигшафен'),(1143,1012,1137,'Майнц'),(1144,1012,1137,'Ньюштадт-ан-дер-Вейнштрассе'),(1145,1012,1137,'Пирмасенс'),(1146,1012,1137,'Спейер'),(1147,1012,1137,'Трир'),(1148,1012,1137,'Франкенталь'),(1150,1012,1149,'Ньюнкирхен'),(1151,1012,1149,'Саарбрюкен'),(1153,1012,1152,'Аахен'),(1154,1012,1152,'Айзерлон'),(1155,1012,1152,'Ален'),(1156,1012,1152,'Бад-Зальцуфлен'),(1157,1012,1152,'Бергиш-Гладбах'),(1158,1012,1152,'Билефельд'),(1159,1012,1152,'Бонн'),(1160,1012,1152,'Бохольт'),(1161,1012,1152,'Бохум'),(1162,1012,1152,'Брул'),(1163,1012,1152,'Весел'),(1164,1012,1152,'Вирсен'),(1165,1012,1152,'Виттен'),(1166,1012,1152,'Вупперталь'),(1167,1012,1152,'Гельзенкирхен'),(1168,1012,1152,'Гуммерсбах'),(1169,1012,1152,'Гутерсло'),(1170,1012,1152,'Детмольд'),(1171,1012,1152,'Дойсбург'),(1172,1012,1152,'Дурен'),(1173,1012,1152,'Дюссельдорф'),(1174,1012,1152,'Золинген'),(1175,1012,1152,'Кёльн'),(1176,1012,1152,'Крефельд'),(1177,1012,1152,'Леверкузен'),(1178,1012,1152,'Липпштадт'),(1179,1012,1152,'Лунен'),(1180,1012,1152,'Люденсхейд'),(1181,1012,1152,'Малхейм-ан-дер-Рур'),(1182,1012,1152,'Марл'),(1183,1012,1152,'Минден'),(1184,1012,1152,'Монхенгладбах'),(1185,1012,1152,'Мюнстер'),(1186,1012,1152,'Ньюсс'),(1187,1012,1152,'Оберхаузен'),(1188,1012,1152,'Падерборн'),(1189,1012,1152,'Ратинген'),(1190,1012,1152,'Рейн'),(1191,1012,1152,'Ремшейд'),(1192,1012,1152,'Сест'),(1193,1012,1152,'Зиген'),(1194,1012,1152,'Стендаль'),(1195,1012,1152,'Хаген'),(1196,1012,1152,'Харт'),(1197,1012,1152,'Херн'),(1198,1012,1152,'Херфорд'),(1199,1012,1152,'Эскирхен'),(1200,1012,1152,'Эссен'),(1202,1012,1201,'Киль'),(1203,1012,1201,'Любек'),(1204,1012,1201,'Фленсбург'),(1205,1012,1201,'Эльмсхорн'),(1208,1206,1207,'Апельдоорн'),(1209,1206,1207,'Арнхем'),(1210,1206,1207,'Нижмеген'),(1211,1206,1207,'Реден'),(1212,1206,1207,'Эд'),(1214,1206,1213,'Гронинген'),(1216,1206,1215,'Эммен'),(1218,1206,1217,'Венло'),(1219,1206,1217,'Керкрад'),(1220,1206,1217,'Маастрихт'),(1221,1206,1217,'Хеерлен'),(1223,1206,1222,'Альмело'),(1224,1206,1222,'Девентер'),(1225,1206,1222,'Хенгело'),(1226,1206,1222,'Эншед'),(1228,1206,1227,'Алькмаар'),(1229,1206,1227,'Амстельвеен'),(1230,1206,1227,'Амстердам'),(1231,1206,1227,'Велсен'),(1232,1206,1227,'Ден-Хельдер'),(1233,1206,1227,'Хаарлем'),(1234,1206,1227,'Хилверсум'),(1236,1206,1235,'Бреда'),(1237,1206,1235,'Тилбург'),(1238,1206,1235,'Хелмонд'),(1239,1206,1235,'Эйндховен'),(1241,1206,1240,'Амерсфоорт'),(1242,1206,1240,'Зейст'),(1243,1206,1240,'Утрехт'),(1245,1206,1244,'Лееварден'),(1247,1206,1246,'Влаардинген'),(1248,1206,1246,'Гуда'),(1249,1206,1246,'Дельфт'),(1250,1206,1246,'Дордрехт'),(1251,1206,1246,'Заандам'),(1252,1206,1246,'Зволь'),(1253,1206,1246,'Лейден'),(1254,1206,1246,'Роттердам'),(1255,1206,1246,'Хаге'),(1256,1206,1246,'Шевенинген'),(1257,1206,1246,'Шидам'),(1260,1258,1259,'Патры'),(1262,1258,1261,'Афины'),(1263,1258,1261,'Пирей'),(1265,1258,1264,'Драма'),(1267,1258,1266,'Александруполис'),(1269,1258,1268,'Хания'),(1271,1258,1270,'Иоаннина'),(1273,1258,1272,'Ираклион'),(1275,1258,1274,'Кавалла'),(1277,1258,1276,'Волос'),(1279,1258,1278,'Фессалоники'),(1282,1280,1281,'Авадхара'),(1283,1280,1281,'Гагра'),(1284,1280,1281,'Гали'),(1285,1280,1281,'Гудаута'),(1286,1280,1281,'Гульрипш'),(1287,1280,1281,'Новый Афон'),(1288,1280,1281,'Очамчиров'),(1289,1280,1281,'Пицунда'),(1290,1280,1281,'Сухуми'),(1291,1280,1281,'Ткварчели'),(1293,1280,1292,'Батуми'),(1294,1280,1292,'Кобулети'),(1295,1280,1292,'Хуло'),(1297,1280,1296,'Абастумани'),(1298,1280,1296,'Абаша'),(1299,1280,1296,'Агара'),(1300,1280,1296,'Адигени'),(1301,1280,1296,'Амбролаури'),(1302,1280,1296,'Аспиндза'),(1303,1280,1296,'Ахалкалаки'),(1304,1280,1296,'Ахалцихе'),(1305,1280,1296,'Ахмета'),(1306,1280,1296,'Бакуриани'),(1307,1280,1296,'Богдановка'),(1308,1280,1296,'Болниси'),(1309,1280,1296,'Боржоми'),(1310,1280,1296,'Вани'),(1311,1280,1296,'Гардабани'),(1312,1280,1296,'Гегечкори'),(1313,1280,1296,'Гори'),(1314,1280,1296,'Гурджаани'),(1315,1280,1296,'Джвари'),(1316,1280,1296,'Дманиси'),(1317,1280,1296,'Душети'),(1318,1280,1296,'Зестафони'),(1319,1280,1296,'Зугдиди'),(1320,1280,1296,'Казбеги'),(1321,1280,1296,'Карели'),(1322,1280,1296,'Каспи'),(1323,1280,1296,'Кваиси'),(1324,1280,1296,'Кварели'),(1325,1280,1296,'Коджори'),(1326,1280,1296,'Кутаиси'),(1327,1280,1296,'Лагодехи'),(1328,1280,1296,'Ланчхути'),(1329,1280,1296,'Ленингори'),(1330,1280,1296,'Лентехи'),(1331,1280,1296,'Марнеули'),(1332,1280,1296,'Махарадзе'),(1333,1280,1296,'Маяковский'),(1334,1280,1296,'Местиа'),(1335,1280,1296,'Они'),(1336,1280,1296,'Орджоникидзе'),(1337,1280,1296,'Пасанаури'),(1338,1280,1296,'Поти'),(1339,1280,1296,'Рустави'),(1340,1280,1296,'Сагареджо'),(1341,1280,1296,'Самтредиа'),(1342,1280,1296,'Сачхере'),(1343,1280,1296,'Сигнахи'),(1344,1280,1296,'Тбилиси'),(1345,1280,1296,'Телави'),(1346,1280,1296,'Тержола'),(1347,1280,1296,'Тетри-Цкаро'),(1348,1280,1296,'Тианети'),(1349,1280,1296,'Ткибули'),(1350,1280,1296,'Хашури'),(1351,1280,1296,'Хоби'),(1352,1280,1296,'Цагери'),(1353,1280,1296,'Цаленджиха'),(1354,1280,1296,'Цалка'),(1355,1280,1296,'Цители-Цкаро'),(1356,1280,1296,'Цнори'),(1357,1280,1296,'Цулукидзе'),(1358,1280,1296,'Цхакая'),(1359,1280,1296,'Цхалтубо'),(1360,1280,1296,'Чиатура'),(1361,1280,1296,'Чохатаури'),(1362,1280,1296,'Чхороцку'),(1364,1280,1363,'Джава'),(1365,1280,1363,'Цхинвали'),(1368,1366,1367,'Колдинг'),(1370,1366,1369,'Альборг'),(1372,1366,1371,'Орхус'),(1373,1366,1371,'Рандерс'),(1375,1366,1374,'Роскилде'),(1377,1366,1376,'Эсбйерг'),(1379,1366,1378,'Оденс'),(1382,1380,1381,'Каир'),(1384,1380,1383,'Асуан'),(1386,1380,1385,'Асыут'),(1388,1380,1387,'Бени Суэф'),(1390,1380,1389,'Танта'),(1392,1380,1391,'Думият'),(1395,1393,1394,'Арад'),(1396,1393,1394,'Ашдод'),(1397,1393,1394,'Ашкелон'),(1398,1393,1394,'Бээр-Шева'),(1399,1393,1394,'Димона'),(1400,1393,1394,'Кирьят-Гат'),(1401,1393,1394,'Кирьят-Малахи'),(1402,1393,1394,'Эйлат'),(1404,1393,1403,'Бнэй-Брак'),(1405,1393,1403,'Гэдера'),(1406,1393,1403,'Лод'),(1407,1393,1403,'Натания'),(1408,1393,1403,'Нэс-Циона'),(1409,1393,1403,'Пэтах-Тиква'),(1410,1393,1403,'Раанана'),(1411,1393,1403,'Рамла'),(1412,1393,1403,'Ришон-ЛеЦион'),(1413,1393,1403,'Рош-ха-Аин'),(1414,1393,1403,'Рэховот'),(1415,1393,1403,'Явне'),(1417,1393,1416,'Акко'),(1418,1393,1416,'Акко (порт)'),(1419,1393,1416,'Афула'),(1420,1393,1416,'Кармиэль'),(1421,1393,1416,'Кирьят-Тивон'),(1422,1393,1416,'Кирьят-Шмона'),(1423,1393,1416,'Нагария'),(1424,1393,1416,'Нацэрэт'),(1425,1393,1416,'Сахнин'),(1426,1393,1416,'Тверия'),(1427,1393,1416,'Цфат'),(1428,1393,1416,'Хайфа'),(1430,1393,1429,'Далият-Эль-Кармэль'),(1431,1393,1429,'Зихрон-Яаков'),(1432,1393,1429,'Кирьят-Бялик'),(1433,1393,1429,'Кирьят-Моцкин'),(1434,1393,1429,'Кирьят-Ям'),(1435,1393,1429,'Нэшэр'),(1436,1393,1429,'Ор-Акива'),(1437,1393,1429,'Пардэс-Ханна'),(1438,1393,1429,'Тират-Кармэль'),(1439,1393,1429,'Хэдэра'),(1441,1393,1440,'Бат-Ям'),(1442,1393,1440,'Герцелия'),(1443,1393,1440,'Гиватаим'),(1444,1393,1440,'Кирьят-Оно'),(1445,1393,1440,'Рамат-Ган'),(1446,1393,1440,'Рамат-Хашарон'),(1447,1393,1440,'Тель-Авив'),(1448,1393,1440,'Холон'),(1450,1393,1449,'Иерусалим'),(1453,1451,1452,'Асансол'),(1454,1451,1452,'Байдьябати'),(1455,1451,1452,'Балли'),(1456,1451,1452,'Банкура'),(1457,1451,1452,'Барасат'),(1458,1451,1452,'Бхатпара'),(1459,1451,1452,'Даржилинг'),(1460,1451,1452,'Дум-Дум'),(1461,1451,1452,'Дургапур'),(1462,1451,1452,'Камархати'),(1463,1451,1452,'Кришнанагар'),(1464,1451,1452,'Кхарагпур'),(1465,1451,1452,'Наихати'),(1466,1451,1452,'Панихати'),(1467,1451,1452,'Титагарх'),(1469,1451,1468,'Биласпур'),(1470,1451,1468,'Бхилаи'),(1471,1451,1468,'Дург'),(1472,1451,1468,'Раипур'),(1474,1451,1473,'Багалкот'),(1475,1451,1473,'Белгаум'),(1476,1451,1473,'Беллари'),(1477,1451,1473,'Бияпур'),(1478,1451,1473,'Гадаг'),(1479,1451,1473,'Давангер'),(1480,1451,1473,'Колар Голд Филдс'),(1481,1451,1473,'Мангалор'),(1482,1451,1473,'Раичур'),(1483,1451,1473,'Сагар'),(1484,1451,1473,'Тумкур'),(1485,1451,1473,'Хоспет'),(1486,1451,1473,'Хубли'),(1487,1451,1473,'Шимога'),(1489,1451,1488,'Дехра Дун'),(1490,1451,1488,'Сринагар'),(1492,1451,1491,'Адони'),(1493,1451,1491,'Анакапал'),(1494,1451,1491,'Анантапур'),(1495,1451,1491,'Визианагарам'),(1496,1451,1491,'Вияиавада'),(1497,1451,1491,'Гунтакал'),(1498,1451,1491,'Гунтур'),(1499,1451,1491,'Какинада'),(1500,1451,1491,'Куддапах'),(1501,1451,1491,'Мачилипатнам'),(1502,1451,1491,'Нандиал'),(1503,1451,1491,'Низамабад'),(1504,1451,1491,'Проддатур'),(1505,1451,1491,'Тенали'),(1506,1451,1491,'Чирала'),(1507,1451,1491,'Читтур'),(1508,1451,1491,'Элуру'),(1510,1451,1509,'Дибругарх'),(1511,1451,1509,'Силчар'),(1513,1451,1512,'Беттиах'),(1514,1451,1512,'Бихар'),(1515,1451,1512,'Бхагалпур'),(1516,1451,1512,'Гэйа'),(1517,1451,1512,'Дарбханга'),(1518,1451,1512,'Катихар'),(1519,1451,1512,'Музаффарпур'),(1520,1451,1512,'Патна'),(1522,1451,1521,'Ахмадабад'),(1523,1451,1521,'Бхуй'),(1524,1451,1521,'Веравал'),(1525,1451,1521,'Годхра'),(1526,1451,1521,'Гондал'),(1527,1451,1521,'Дхорайи'),(1528,1451,1521,'Йодхпур'),(1529,1451,1521,'Морви'),(1530,1451,1521,'Навсари'),(1531,1451,1521,'Надиад'),(1532,1451,1521,'Патан'),(1533,1451,1521,'Порбандар'),(1534,1451,1521,'Райкот'),(1535,1451,1521,'Сурат'),(1536,1451,1521,'Сурендранагар'),(1537,1451,1521,'Юнагадх'),(1538,1451,1521,'Ямнагар'),(1540,1451,1539,'Дханбад'),(1541,1451,1539,'Ранчи'),(1542,1451,1539,'Сринагар'),(1543,1451,1539,'Ямму'),(1544,1451,1539,'Ямшедпур'),(1546,1451,1545,'Кожикод'),(1547,1451,1545,'Коттэйам'),(1549,1451,1548,'Барейлли'),(1550,1451,1548,'Бурханпур'),(1551,1451,1548,'Бхопал'),(1552,1451,1548,'Гвалиор'),(1553,1451,1548,'Дамох'),(1554,1451,1548,'Индаур'),(1555,1451,1548,'Кхандва'),(1556,1451,1548,'Мау'),(1557,1451,1548,'Мурвара'),(1558,1451,1548,'Ратлам'),(1559,1451,1548,'Рева'),(1560,1451,1548,'Уйяин'),(1562,1451,1561,'Имфал'),(1564,1451,1563,'Акола'),(1565,1451,1563,'Амальнер'),(1566,1451,1563,'Ахалпур'),(1567,1451,1563,'Ахмаднагар'),(1568,1451,1563,'Барси'),(1569,1451,1563,'Бхиванди'),(1570,1451,1563,'Бхусавал'),(1571,1451,1563,'Вардха'),(1572,1451,1563,'Дхулиа'),(1573,1451,1563,'Калиан'),(1574,1451,1563,'Колхапур'),(1575,1451,1563,'Кхамгаон'),(1576,1451,1563,'Латур'),(1577,1451,1563,'Малегаон'),(1578,1451,1563,'Нагпур'),(1579,1451,1563,'Нандурбар'),(1580,1451,1563,'Пандхарпур'),(1581,1451,1563,'Сангли'),(1582,1451,1563,'Сатара'),(1583,1451,1563,'Тана'),(1584,1451,1563,'Улхаснагар'),(1585,1451,1563,'Шолапур'),(1587,1451,1586,'Шиллонг'),(1589,1451,1588,'Пури'),(1590,1451,1588,'Рауркела'),(1592,1451,1591,'Амритсар'),(1593,1451,1591,'Батала'),(1594,1451,1591,'Лудхиана'),(1595,1451,1591,'Патиала'),(1597,1451,1596,'Пондичерри'),(1599,1451,1598,'Аймер'),(1600,1451,1598,'Альвар'),(1601,1451,1598,'Бивар'),(1602,1451,1598,'Биканер'),(1603,1451,1598,'Бхаратпур'),(1604,1451,1598,'Бхилвара'),(1605,1451,1598,'Ганганагар'),(1606,1451,1598,'Кота'),(1607,1451,1598,'Сикар'),(1608,1451,1598,'Тонк'),(1609,1451,1598,'Удаипур'),(1610,1451,1598,'Фатехгарх'),(1611,1451,1598,'Чуру'),(1613,1451,1612,'Аруппокоттаи'),(1614,1451,1612,'Бодинэйакканур'),(1615,1451,1612,'Ванииамбади'),(1616,1451,1612,'Виллупурам'),(1617,1451,1612,'Диндигул'),(1618,1451,1612,'Ерод'),(1619,1451,1612,'Карур'),(1620,1451,1612,'Кумбаконам'),(1621,1451,1612,'Мадурай'),(1622,1451,1612,'Нагеркоил'),(1623,1451,1612,'Пудуккоттаи'),(1624,1451,1612,'Раяпалаииам'),(1625,1451,1612,'Тируваннамалаи'),(1626,1451,1612,'Тирунелвели'),(1627,1451,1612,'Тируччираппалли'),(1628,1451,1612,'Чидамбарам'),(1630,1451,1629,'Агартала'),(1632,1451,1631,'Агра'),(1633,1451,1631,'Алигар'),(1634,1451,1631,'Аллахабад'),(1635,1451,1631,'Бахраич'),(1636,1451,1631,'Будаун'),(1637,1451,1631,'Варанаси'),(1638,1451,1631,'Гонда'),(1639,1451,1631,'Горакхпур'),(1640,1451,1631,'Гхазиабад'),(1641,1451,1631,'Етавах'),(1642,1451,1631,'Йханси'),(1643,1451,1631,'Канпур'),(1644,1451,1631,'Матура'),(1645,1451,1631,'Мирзапур'),(1646,1451,1631,'Морадабад'),(1647,1451,1631,'Музаффарнагар'),(1648,1451,1631,'Пилибхит'),(1649,1451,1631,'Рампур'),(1650,1451,1631,'Самбхал'),(1651,1451,1631,'Ситапур'),(1652,1451,1631,'Фаизабад'),(1653,1451,1631,'Фирозабад'),(1654,1451,1631,'Хатрас'),(1655,1451,1631,'Шахяханпур'),(1657,1451,1656,'Амбала'),(1658,1451,1656,'Бхивани'),(1659,1451,1656,'Карнал'),(1660,1451,1656,'Панипат'),(1662,1451,1661,'Чандигарх'),(1665,1663,1664,'Марагех'),(1666,1663,1664,'Тебриз'),(1668,1663,1667,'Кашан'),(1670,1663,1669,'Хамадан'),(1672,1663,1671,'Санандай'),(1674,1663,1673,'Арак'),(1676,1663,1675,'Захедан'),(1678,1663,1677,'Иезд'),(1680,1663,1679,'Керман'),(1682,1663,1681,'Керманшах'),(1684,1663,1683,'Бабол'),(1685,1663,1683,'Сари'),(1687,1663,1686,'Тегеран'),(1689,1663,1688,'Абадан'),(1690,1663,1688,'Шираз'),(1692,1663,1691,'Бендер-Аббас'),(1694,1663,1693,'Ахваз'),(1695,1663,1693,'Хоррамшахр'),(1698,1696,1697,'Корк'),(1700,1696,1699,'Дан-Логер'),(1701,1696,1699,'Дублин'),(1703,1696,1702,'Лимерик'),(1705,1696,1704,'Луимнич'),(1706,1696,1704,'Уотерфорд'),(1709,1707,1708,'Сантандер'),(1711,1707,1710,'Сеута'),(1713,1707,1712,'Гранада'),(1715,1707,1714,'Логроно'),(1717,1707,1716,'Саламанка'),(1719,1707,1718,'Витория'),(1721,1707,1720,'Аликанте'),(1722,1707,1720,'Алкантара'),(1723,1707,1720,'Алкой'),(1725,1707,1724,'Альбасете'),(1727,1707,1726,'Алмерия'),(1729,1707,1728,'Гийон'),(1730,1707,1728,'Овьедо'),(1732,1707,1731,'Баладона'),(1733,1707,1731,'Барселона'),(1734,1707,1731,'Манреса'),(1735,1707,1731,'Сабадель'),(1736,1707,1731,'Тарраса'),(1737,1707,1731,'Хоспиталет'),(1739,1707,1738,'Бильбао'),(1741,1707,1740,'Бургос'),(1743,1707,1742,'Валенсия'),(1745,1707,1744,'Валладолид'),(1747,1707,1746,'Алжекирас'),(1748,1707,1746,'Кадис'),(1750,1707,1749,'Кацерес'),(1751,1707,1749,'Ла-Линея'),(1752,1707,1749,'Херес-де-ла-Фронтера'),(1754,1707,1753,'Кордова'),(1756,1707,1755,'Лас-Пальмас-де-Гран-Канар'),(1758,1707,1757,'Леон'),(1760,1707,1759,'Лерида'),(1762,1707,1761,'Луго'),(1764,1707,1763,'Мадрид'),(1766,1707,1765,'Малага'),(1768,1707,1767,'Мурсия'),(1770,1707,1769,'Палма-де-Мальорка'),(1772,1707,1771,'Виго'),(1773,1707,1771,'Паленсия'),(1775,1707,1774,'Замора'),(1777,1707,1776,'Санта-Круз-де-Тенерифе'),(1779,1707,1778,'Сарагоса'),(1781,1707,1780,'Севилья'),(1783,1707,1782,'Пуэртоллано'),(1785,1707,1784,'Линарес'),(1788,1786,1787,'Пескара'),(1790,1786,1789,'Альтамура'),(1791,1786,1789,'Андрия'),(1792,1786,1789,'Бари'),(1793,1786,1789,'Бискегли'),(1794,1786,1789,'Бриндизи'),(1795,1786,1789,'Корато'),(1796,1786,1789,'Лечче'),(1797,1786,1789,'Мольфетта'),(1798,1786,1789,'Сан-Северо'),(1799,1786,1789,'Таранто'),(1800,1786,1789,'Трани'),(1801,1786,1789,'Фоджия'),(1803,1786,1802,'Верона'),(1804,1786,1802,'Виченца'),(1805,1786,1802,'Тревисо'),(1807,1786,1806,'Катанцаро'),(1808,1786,1806,'Косенца'),(1809,1786,1806,'Реджио-ди-Калабриа'),(1811,1786,1810,'Аверса'),(1812,1786,1810,'Беневенто'),(1813,1786,1810,'Кастелламмаре-ди-Стабия'),(1814,1786,1810,'Ночера-Инфериоре'),(1815,1786,1810,'Поццуоли'),(1816,1786,1810,'Салерно'),(1817,1786,1810,'Торре-Аннунциата'),(1818,1786,1810,'Торре-дель-Греко'),(1820,1786,1819,'Рим'),(1822,1786,1821,'Генуя'),(1823,1786,1821,'Ла-Специя'),(1824,1786,1821,'Савона'),(1825,1786,1821,'Сан-Ремо'),(1827,1786,1826,'Бергамо'),(1828,1786,1826,'Брескиа'),(1829,1786,1826,'Бусто-Арсизио'),(1830,1786,1826,'Варезе'),(1831,1786,1826,'Комо'),(1832,1786,1826,'Кремона'),(1833,1786,1826,'Леччо'),(1834,1786,1826,'Мантуя'),(1835,1786,1826,'Милан'),(1836,1786,1826,'Монца'),(1837,1786,1826,'Павия'),(1839,1786,1838,'Анкона'),(1840,1786,1838,'Песаро'),(1842,1786,1841,'Каглиари'),(1843,1786,1841,'Сассари'),(1845,1786,1844,'Алькамо'),(1846,1786,1844,'Витториа'),(1847,1786,1844,'Гела'),(1848,1786,1844,'Калтагирон'),(1849,1786,1844,'Калтаниссетта'),(1850,1786,1844,'Катания'),(1851,1786,1844,'Ликата'),(1852,1786,1844,'Мессина'),(1853,1786,1844,'Палермо'),(1854,1786,1844,'Патерно'),(1855,1786,1844,'Рагуса'),(1856,1786,1844,'Сиракузы'),(1857,1786,1844,'Трапани'),(1859,1786,1858,'Асти'),(1860,1786,1858,'Биелла'),(1861,1786,1858,'Верцелли'),(1862,1786,1858,'Новара'),(1863,1786,1858,'Турин'),(1865,1786,1864,'Ареццо'),(1866,1786,1864,'Виареджио'),(1867,1786,1864,'Гроссето'),(1868,1786,1864,'Каррара'),(1869,1786,1864,'Лючча'),(1870,1786,1864,'Масса'),(1871,1786,1864,'Пиза'),(1872,1786,1864,'Пистойя'),(1873,1786,1864,'Прато'),(1874,1786,1864,'Сьена'),(1875,1786,1864,'Флоренция'),(1877,1786,1876,'Больцано'),(1878,1786,1876,'Тренто'),(1880,1786,1879,'Перуджиа'),(1881,1786,1879,'Терни'),(1883,1786,1882,'Триест'),(1884,1786,1882,'Удин'),(1886,1786,1885,'Болонья'),(1887,1786,1885,'Модена'),(1888,1786,1885,'Парма'),(1889,1786,1885,'Пиаченца'),(1890,1786,1885,'Равенна'),(1891,1786,1885,'Фенца'),(1892,1786,1885,'Феррара'),(1893,1786,1885,'Форли'),(1896,1894,1895,'Актюбинск'),(1897,1894,1895,'Акшам'),(1898,1894,1895,'Алга'),(1899,1894,1895,'Байганин'),(1900,1894,1895,'Батамшинский'),(1901,1894,1895,'Иргиз'),(1902,1894,1895,'Карабутак'),(1903,1894,1895,'Мартук'),(1904,1894,1895,'Новоалексеевка'),(1905,1894,1895,'Октябрьск'),(1906,1894,1895,'Уил'),(1907,1894,1895,'Хромтау'),(1908,1894,1895,'Челкар'),(1909,1894,1895,'Шубаркудук'),(1910,1894,1895,'Эмба'),(1912,1894,1911,'Алма-Ата'),(1913,1894,1911,'Алматы'),(1914,1894,1911,'Баканас'),(1915,1894,1911,'Бурундай'),(1916,1894,1911,'Иссык'),(1917,1894,1911,'Капчагай'),(1918,1894,1911,'Каскелен'),(1919,1894,1911,'Нарынкол'),(1920,1894,1911,'Талгар'),(1921,1894,1911,'Узунагач'),(1922,1894,1911,'Чилик'),(1923,1894,1911,'Чунджа'),(1925,1894,1924,'Акжар'),(1926,1894,1924,'Алексеевка'),(1927,1894,1924,'Асубулак'),(1928,1894,1924,'Белогорский'),(1929,1894,1924,'Белоусовка'),(1930,1894,1924,'Верхнеберезовский'),(1931,1894,1924,'Глубокое'),(1932,1894,1924,'Зайсан'),(1933,1894,1924,'Зыряновск'),(1934,1894,1924,'Карагужиха'),(1935,1894,1924,'Катон-Карагай'),(1936,1894,1924,'Курчум'),(1937,1894,1924,'Лениногорск'),(1938,1894,1924,'Самарское'),(1939,1894,1924,'Серебрянск'),(1940,1894,1924,'Усть-Каменогорск'),(1941,1894,1924,'Шемонаиха'),(1943,1894,1942,'Байчунас'),(1944,1894,1942,'Балыкши'),(1945,1894,1942,'Ганюшкино'),(1946,1894,1942,'Атырау(Гурьев)'),(1947,1894,1942,'Доссор'),(1948,1894,1942,'Индерборский'),(1949,1894,1942,'Искининский'),(1950,1894,1942,'Каратон'),(1951,1894,1942,'Кульсары'),(1952,1894,1942,'Макат'),(1953,1894,1942,'Миялы'),(1955,1894,1954,'Акколь'),(1956,1894,1954,'Байкадам'),(1957,1894,1954,'Брлик'),(1958,1894,1954,'Бурное'),(1959,1894,1954,'Георгиевка'),(1960,1894,1954,'Гранитогорск'),(1961,1894,1954,'Джамбул'),(1962,1894,1954,'Жанатас'),(1963,1894,1954,'Каратау'),(1964,1894,1954,'Коктал'),(1965,1894,1954,'Луговое'),(1966,1894,1954,'Мерке'),(1967,1894,1954,'Михайловка'),(1968,1894,1954,'Новотроицкое'),(1969,1894,1954,'Ойтал'),(1970,1894,1954,'Отар'),(1971,1894,1954,'Фурмановка'),(1972,1894,1954,'Чиганак'),(1973,1894,1954,'Чу'),(1975,1894,1974,'Агадырь'),(1976,1894,1974,'Акжал'),(1977,1894,1974,'Актас'),(1978,1894,1974,'Актогай'),(1979,1894,1974,'Атасу'),(1980,1894,1974,'Балхаш'),(1981,1894,1974,'Восточно-Коунрадский'),(1982,1894,1974,'Гульшад'),(1983,1894,1974,'Дарьинский'),(1984,1894,1974,'Джамбул'),(1985,1894,1974,'Джезды'),(1986,1894,1974,'Джезказган'),(1987,1894,1974,'Жарык'),(1988,1894,1974,'Кайракты'),(1989,1894,1974,'Каражал'),(1990,1894,1974,'Никольский'),(1991,1894,1974,'Сарышаган'),(1992,1894,1974,'Тараз'),(1993,1894,1974,'Улытау'),(1995,1894,1994,'Абай'),(1996,1894,1994,'Актас'),(1997,1894,1994,'Актау'),(1998,1894,1994,'Егиндыбулак'),(1999,1894,1994,'Карагайлы'),(2000,1894,1994,'Караганда'),(2001,1894,1994,'Каркаралинск'),(2002,1894,1994,'Киевка'),(2003,1894,1994,'Осакаровка'),(2004,1894,1994,'Сарань'),(2005,1894,1994,'Темиртау'),(2006,1894,1994,'Токаревка'),(2007,1894,1994,'Топар'),(2008,1894,1994,'Ульяновский'),(2009,1894,1994,'Шахтинск'),(2011,1894,2010,'Аралсульфат'),(2012,1894,2010,'Аральск'),(2013,1894,2010,'Джалагаш'),(2014,1894,2010,'Джусалы'),(2015,1894,2010,'Казалинск'),(2016,1894,2010,'Кзыл-Орда'),(2017,1894,2010,'Новоказалинск'),(2018,1894,2010,'Тасбугет'),(2019,1894,2010,'Чиили'),(2020,1894,2010,'Яныкурган'),(2022,1894,2021,'Айдабул'),(2023,1894,2021,'Алексеевка'),(2024,1894,2021,'Боровое'),(2025,1894,2021,'Володарское'),(2026,1894,2021,'Зеренда'),(2027,1894,2021,'Келлеровка'),(2028,1894,2021,'Кзылту'),(2029,1894,2021,'Кокчетав'),(2030,1894,2021,'Красноармейск'),(2031,1894,2021,'Красный Яр'),(2032,1894,2021,'Куйбышевский'),(2033,1894,2021,'Ленинградское'),(2034,1894,2021,'Рузаевка'),(2035,1894,2021,'Степняк'),(2036,1894,2021,'Талшик'),(2037,1894,2021,'Чистополье'),(2038,1894,2021,'Чкалово'),(2039,1894,2021,'Щучинск'),(2041,1894,2040,'Боровской'),(2042,1894,2040,'Джетыгара'),(2043,1894,2040,'Затобольск'),(2044,1894,2040,'Камышное'),(2045,1894,2040,'Комсомолец'),(2046,1894,2040,'Кустанай'),(2047,1894,2040,'Кушмурун'),(2048,1894,2040,'Ленинское'),(2049,1894,2040,'Лисаковск'),(2050,1894,2040,'Орджоникидзе'),(2051,1894,2040,'Рудный'),(2052,1894,2040,'Семиозерное'),(2053,1894,2040,'Тобол'),(2054,1894,2040,'Урицкий'),(2056,1894,2055,'Баутино'),(2057,1894,2055,'Бейнеу'),(2058,1894,2055,'Новый Узень'),(2059,1894,2055,'Форт-Шевченко'),(2060,1894,2055,'Шевченко'),(2062,1894,2061,'Баянаул'),(2063,1894,2061,'Ермак'),(2064,1894,2061,'Железинка'),(2065,1894,2061,'Иртышск'),(2066,1894,2061,'Калкаман'),(2067,1894,2061,'Краснокутск'),(2068,1894,2061,'Лебяжье'),(2069,1894,2061,'Майкаин'),(2070,1894,2061,'Павлодар'),(2071,1894,2061,'Успенка'),(2072,1894,2061,'Щербакты'),(2073,1894,2061,'Экибастуз'),(2075,1894,2074,'Благовещенка'),(2076,1894,2074,'Булаево'),(2077,1894,2074,'Корнеевка'),(2078,1894,2074,'Мамлютка'),(2079,1894,2074,'Петропавловск'),(2080,1894,2074,'Пресновка'),(2081,1894,2074,'Сергеевка'),(2082,1894,2074,'Соколовка'),(2083,1894,2074,'Явленка'),(2085,1894,2084,'Акжал'),(2086,1894,2084,'Аксуат'),(2087,1894,2084,'Ауэзов'),(2088,1894,2084,'Аягуз'),(2089,1894,2084,'Баршатас'),(2090,1894,2084,'Бельагаш'),(2091,1894,2084,'Боко'),(2092,1894,2084,'Большая Владимировка'),(2093,1894,2084,'Бородулиха'),(2094,1894,2084,'Георгиевка'),(2095,1894,2084,'Жангизтобе'),(2096,1894,2084,'Жарма'),(2097,1894,2084,'Кайнар'),(2098,1894,2084,'Кокпекты'),(2099,1894,2084,'Маканчи'),(2100,1894,2084,'Новая Шульба'),(2101,1894,2084,'Семипалатинск'),(2102,1894,2084,'Таскескен'),(2103,1894,2084,'Урджар'),(2104,1894,2084,'Чарск'),(2106,1894,2105,'Актогай'),(2107,1894,2105,'Андреевка'),(2108,1894,2105,'Джансугуров'),(2109,1894,2105,'Капал'),(2110,1894,2105,'Карабулак'),(2111,1894,2105,'Кировский'),(2112,1894,2105,'Кугалы'),(2113,1894,2105,'Панфилов'),(2114,1894,2105,'Сарканд'),(2115,1894,2105,'Сарыозек'),(2116,1894,2105,'Талды-Курган'),(2117,1894,2105,'Текели'),(2118,1894,2105,'Учарал'),(2119,1894,2105,'Уштобе'),(2121,1894,2120,'Амангельды'),(2122,1894,2120,'Аркалык'),(2123,1894,2120,'Державинск'),(2124,1894,2120,'Есиль'),(2125,1894,2120,'Жаксы'),(2126,1894,2120,'Октябрьское'),(2127,1894,2120,'Тургай'),(2129,1894,2128,'Акбеит'),(2130,1894,2128,'Акмолинск'),(2131,1894,2128,'Аксу'),(2132,1894,2128,'Алексеевка'),(2133,1894,2128,'Астана'),(2134,1894,2128,'Астраханка'),(2135,1894,2128,'Атабасар'),(2136,1894,2128,'Балкащино'),(2137,1894,2128,'Бестобе'),(2138,1894,2128,'Вишневка'),(2139,1894,2128,'Ерментау'),(2140,1894,2128,'Жалтыр'),(2141,1894,2128,'Жолымбет'),(2142,1894,2128,'Кургальджинский'),(2143,1894,2128,'Макинск'),(2144,1894,2128,'Целиноград'),(2145,1894,2128,'Шортанды'),(2147,1894,2146,'Арысь'),(2148,1894,2146,'Ачисай'),(2149,1894,2146,'Байжансай'),(2150,1894,2146,'Белые Воды'),(2151,1894,2146,'Ванновка'),(2152,1894,2146,'Джетысай'),(2153,1894,2146,'Кентау'),(2154,1894,2146,'Ленгер'),(2155,1894,2146,'Сарыагач'),(2156,1894,2146,'Темирлановка'),(2157,1894,2146,'Туркестан'),(2158,1894,2146,'Чардара'),(2159,1894,2146,'Чаян'),(2160,1894,2146,'Чимкент'),(2161,1894,2146,'Чулаккурган'),(2162,1894,2146,'Шаульдер'),(2165,2163,2164,'Дуала'),(2167,2163,2166,'Виктория'),(2169,2163,2168,'Гаруа'),(2171,2163,2170,'Яунде'),(2174,2172,2173,'Гранд-Праири'),(2175,2172,2173,'Калгари'),(2176,2172,2173,'Летбридж'),(2177,2172,2173,'Медикин-Хат'),(2178,2172,2173,'Ред-Дир'),(2179,2172,2173,'Сант-Альберт'),(2180,2172,2173,'Эдмонтон'),(2182,2172,2181,'Бурнаби'),(2183,2172,2181,'Ванкувер'),(2184,2172,2181,'Вернон'),(2185,2172,2181,'Вест-Ванкувер'),(2186,2172,2181,'Дельта'),(2187,2172,2181,'Камлупс'),(2188,2172,2181,'Кампбелл-Ривер'),(2189,2172,2181,'Коквитлам'),(2190,2172,2181,'Мапл-Ридж'),(2191,2172,2181,'Миссион-Сити'),(2192,2172,2181,'Нанаимо'),(2193,2172,2181,'Норт-Ванкувер'),(2194,2172,2181,'Нью-Вестминстер'),(2195,2172,2181,'Порт-Коквитлам'),(2196,2172,2181,'Порт-Муди'),(2197,2172,2181,'Принц-Джордж'),(2198,2172,2181,'Ричмонд'),(2199,2172,2181,'Сарри'),(2200,2172,2181,'Чилливак'),(2202,2172,2201,'Аутремонт'),(2203,2172,2201,'Боучервилл'),(2204,2172,2201,'Броссард'),(2205,2172,2201,'Бьюпорт'),(2206,2172,2201,'Вердан'),(2207,2172,2201,'Вестмаунт'),(2208,2172,2201,'Викториавилл'),(2209,2172,2201,'Джонкуир'),(2210,2172,2201,'Доллард-дес-Ормо'),(2211,2172,2201,'Драммондвилл'),(2212,2172,2201,'Квебек'),(2213,2172,2201,'Лаваль'),(2214,2172,2201,'Левис'),(2215,2172,2201,'Монреаль'),(2216,2172,2201,'Пиррифондс'),(2217,2172,2201,'Репентигни'),(2218,2172,2201,'Римауски'),(2219,2172,2201,'Роуин'),(2220,2172,2201,'Сант-Хуберт'),(2221,2172,2201,'Сорел'),(2222,2172,2201,'Труа-Ривьер'),(2223,2172,2201,'Халл'),(2224,2172,2201,'Чарльсбург'),(2225,2172,2201,'Чатогуэй'),(2226,2172,2201,'Чикоутими'),(2227,2172,2201,'Шербрук'),(2228,2172,2201,'Элма'),(2230,2172,2229,'Брандон'),(2231,2172,2229,'Виннипег'),(2233,2172,2232,'Галифакс'),(2234,2172,2232,'Дартмут'),(2236,2172,2235,'Монктон'),(2237,2172,2235,'Сент-Джон'),(2238,2172,2235,'Фредериктон'),(2240,2172,2239,'Корнер-Брук'),(2241,2172,2239,'Сент-Джонс'),(2243,2172,2242,'Аврора'),(2244,2172,2242,'Анкастер'),(2245,2172,2242,'Аякс'),(2246,2172,2242,'Барлингтон'),(2247,2172,2242,'Барри'),(2248,2172,2242,'Беллвилл'),(2249,2172,2242,'Брамптон'),(2250,2172,2242,'Брантфорд'),(2251,2172,2242,'Броквилл'),(2252,2172,2242,'Ватерлоо'),(2253,2172,2242,'Вауган'),(2254,2172,2242,'Велланд'),(2255,2172,2242,'Виндзор'),(2256,2172,2242,'Витби'),(2257,2172,2242,'Вудсток'),(2258,2172,2242,'Гримсби'),(2259,2172,2242,'Гуэлф'),(2260,2172,2242,'Каледон'),(2261,2172,2242,'Кингстон'),(2262,2172,2242,'Китченер'),(2263,2172,2242,'Корнуолл'),(2264,2172,2242,'Ла-Саль'),(2265,2172,2242,'Линкольн'),(2266,2172,2242,'Лондон'),(2267,2172,2242,'Маркхам'),(2268,2172,2242,'Миссиссуга'),(2269,2172,2242,'Ниагара-Фоллс'),(2270,2172,2242,'Норт-Бэй'),(2271,2172,2242,'Оаквилл'),(2272,2172,2242,'Овен-Саунд'),(2273,2172,2242,'Ориллиа'),(2274,2172,2242,'Оттава'),(2275,2172,2242,'Ошава'),(2276,2172,2242,'Петерборо'),(2277,2172,2242,'Пикеринг'),(2278,2172,2242,'Ричмонд-Хилл'),(2279,2172,2242,'Садбури'),(2280,2172,2242,'Сант-Катаринс'),(2281,2172,2242,'Сант-Томас'),(2282,2172,2242,'Сарниа'),(2283,2172,2242,'Солт-Сант-Мари'),(2284,2172,2242,'Стони-Крик'),(2285,2172,2242,'Стратфорд'),(2286,2172,2242,'Тимминс'),(2287,2172,2242,'Торонто'),(2288,2172,2242,'Тундер Бэй'),(2289,2172,2242,'Форт-Эри'),(2290,2172,2242,'Хамилтон'),(2292,2172,2291,'Шарлоттаун'),(2294,2172,2293,'Мус-Яв'),(2295,2172,2293,'Реджайна'),(2296,2172,2293,'Саскатун'),(2299,2297,2298,'Лимассол'),(2301,2297,2300,'Никосия'),(2302,2297,2300,'Фамагуста'),(2305,2303,2304,'Ак-Шыйрак'),(2306,2303,2304,'Ананьево'),(2307,2303,2304,'Боконбаевское'),(2308,2303,2304,'Бостери'),(2309,2303,2304,'Каджи-Сай'),(2310,2303,2304,'Кызыл Туу'),(2311,2303,2304,'Покровка'),(2312,2303,2304,'Пржевальск'),(2313,2303,2304,'Рыбачье'),(2314,2303,2304,'Тюп'),(2315,2303,2304,'Чолпон-Ата'),(2317,2303,2316,'Бишкек'),(2318,2303,2316,'Бордунский'),(2319,2303,2316,'Быстровка'),(2320,2303,2316,'Ивановка'),(2321,2303,2316,'Каинды'),(2322,2303,2316,'Кант'),(2323,2303,2316,'Кара-Балта'),(2324,2303,2316,'Соколук'),(2325,2303,2316,'Токмак'),(2326,2303,2316,'Фрунзе'),(2328,2303,2327,'Ат-Баши'),(2329,2303,2327,'Дюрбельджин'),(2330,2303,2327,'Казарман'),(2331,2303,2327,'Кочкорка'),(2332,2303,2327,'Кош-Дебе'),(2333,2303,2327,'Мин-Куш'),(2334,2303,2327,'Нарын'),(2335,2303,2327,'Сары-Булак'),(2336,2303,2327,'Суусамыр'),(2337,2303,2327,'Угют'),(2338,2303,2327,'Чаек'),(2339,2303,2327,'Чолпон'),(2341,2303,2340,'Ак-Там'),(2342,2303,2340,'Ала-Бука'),(2343,2303,2340,'Араван'),(2344,2303,2340,'Арсланбоб'),(2345,2303,2340,'Базар-Курган'),(2346,2303,2340,'Баткен'),(2347,2303,2340,'Гульча'),(2348,2303,2340,'Джалал-Абад'),(2349,2303,2340,'Исфана'),(2350,2303,2340,'Кара-Кульджа'),(2351,2303,2340,'Кара-Суу'),(2352,2303,2340,'Караван'),(2353,2303,2340,'Карамык'),(2354,2303,2340,'Кок-Янгак'),(2355,2303,2340,'Кызыл-Кия'),(2356,2303,2340,'Майли-Сай'),(2357,2303,2340,'Ош'),(2358,2303,2340,'Сары-Таш'),(2359,2303,2340,'Сопу-Коргон'),(2360,2303,2340,'Сулюкта'),(2361,2303,2340,'Таш-Кумыр'),(2362,2303,2340,'Узген'),(2363,2303,2340,'Фрунзе'),(2364,2303,2340,'Хайдаркен'),(2366,2303,2365,'Аманбаево'),(2367,2303,2365,'Кара-Куль'),(2368,2303,2365,'Кировское'),(2369,2303,2365,'Ленинполь'),(2370,2303,2365,'Покровка'),(2371,2303,2365,'Талас'),(2372,2303,2365,'Токтогул'),(2373,2303,2365,'Толук'),(2376,2374,2375,'Аксу'),(2377,2374,2375,'Урумчи'),(2378,2374,2375,'Хами'),(2380,2374,2379,'Баотоу'),(2381,2374,2379,'Хайлар'),(2383,2374,2382,'Ланьчжоу'),(2384,2374,2382,'Пинглианг'),(2385,2374,2382,'Юмень'),(2387,2374,2386,'Наннинг'),(2389,2374,2388,'Аншань'),(2390,2374,2388,'Далянь'),(2391,2374,2388,'Ляоян'),(2392,2374,2388,'Шэньян'),(2394,2374,2393,'Иаан'),(2395,2374,2393,'Суининг'),(2397,2374,2396,'Нанпинг'),(2399,2374,2398,'Ухань'),(2400,2374,2398,'Шаси'),(2402,2374,2401,'Иянг'),(2403,2374,2401,'Чангша'),(2405,2374,2404,'Шихайкванг'),(2407,2374,2406,'Харбин'),(2409,2374,2408,'Кайфенг'),(2410,2374,2408,'Нанянь'),(2412,2374,2411,'Лиаоиуан'),(2413,2374,2411,'Чанчунь'),(2415,2374,2414,'Нанчанг'),(2417,2374,2416,'Иангчау'),(2418,2374,2416,'Нанкин'),(2420,2374,2419,'Венчоу'),(2421,2374,2419,'Нингпо'),(2422,2374,2419,'Ханчоу'),(2423,2374,2419,'Шаохинь'),(2425,2374,2424,'Шанхай'),(2427,2374,2426,'Вейфанг'),(2429,2374,2428,'Тайюань'),(2432,2430,2431,'Алагуела'),(2434,2430,2433,'Либериа'),(2436,2430,2435,'Хередиа'),(2438,2430,2437,'Лимон'),(2440,2430,2439,'Пунтаренас'),(2442,2430,2441,'Сан-Хосе'),(2445,2443,2444,'Аль-Яхра'),(2447,2443,2446,'Эль-Кувейт'),(2450,2448,2449,'Айзпуте'),(2451,2448,2449,'Айнажи'),(2452,2448,2449,'Алоя'),(2453,2448,2449,'Алсунга'),(2454,2448,2449,'Алуксне'),(2455,2448,2449,'Апе'),(2456,2448,2449,'Ауце'),(2457,2448,2449,'Балви'),(2458,2448,2449,'Балдоне'),(2459,2448,2449,'Баложи'),(2460,2448,2449,'Бауска'),(2461,2448,2449,'Бене'),(2462,2448,2449,'Броцены'),(2463,2448,2449,'Валдемарпилс'),(2464,2448,2449,'Валка'),(2465,2448,2449,'Валмиера'),(2466,2448,2449,'Варакляны'),(2467,2448,2449,'Вентспилс'),(2468,2448,2449,'Виесите'),(2469,2448,2449,'Виляка'),(2470,2448,2449,'Виляны'),(2471,2448,2449,'Гробиня'),(2472,2448,2449,'Гулбитис'),(2473,2448,2449,'Гульбене'),(2474,2448,2449,'Даугавпилс'),(2475,2448,2449,'Добеле'),(2476,2448,2449,'Дундага'),(2477,2448,2449,'Дурбе'),(2478,2448,2449,'Елгава'),(2479,2448,2449,'Зилупе'),(2480,2448,2449,'Иецава'),(2481,2448,2449,'Илуксте'),(2482,2448,2449,'Калнциемс'),(2483,2448,2449,'Кандава'),(2484,2448,2449,'Карсава'),(2485,2448,2449,'Кегумс'),(2486,2448,2449,'Кокнесе'),(2487,2448,2449,'Колка'),(2488,2448,2449,'Краслава'),(2489,2448,2449,'Кулдига'),(2490,2448,2449,'Ливаны'),(2491,2448,2449,'Лиепая'),(2492,2448,2449,'Лимбажи'),(2493,2448,2449,'Лудза'),(2494,2448,2449,'Мадона'),(2495,2448,2449,'Огре'),(2496,2448,2449,'Плявиняс'),(2497,2448,2449,'Прейли'),(2498,2448,2449,'Резекне'),(2499,2448,2449,'Рига'),(2500,2448,2449,'Салдус'),(2501,2448,2449,'Сигулда'),(2502,2448,2449,'Стучка'),(2503,2448,2449,'Талси'),(2504,2448,2449,'Тукумс'),(2505,2448,2449,'Цесис'),(2506,2448,2449,'Экабпилс'),(2507,2448,2449,'Элея'),(2508,2448,2449,'Юрмала'),(2511,2509,2510,'Триполи'),(2513,2509,2512,'Бенгази'),(2516,2514,2515,'Адутишкис'),(2517,2514,2515,'Алитус'),(2518,2514,2515,'Аникщчяй'),(2519,2514,2515,'Ариогала'),(2520,2514,2515,'Балтойи-Воке'),(2521,2514,2515,'Бальберишкис'),(2522,2514,2515,'Биржай'),(2523,2514,2515,'Бирштонас'),(2524,2514,2515,'Вабальнинкас'),(2525,2514,2515,'Ванджиогала'),(2526,2514,2515,'Варена'),(2527,2514,2515,'Варняй'),(2528,2514,2515,'Вейсеяй'),(2529,2514,2515,'Виевис'),(2530,2514,2515,'Вилкавишкис'),(2531,2514,2515,'Вилькия'),(2532,2514,2515,'Вильно'),(2533,2514,2515,'Вильнюс'),(2534,2514,2515,'Вирбалис'),(2535,2514,2515,'Гаргждай'),(2536,2514,2515,'Гарлява'),(2537,2514,2515,'Гелгаудишкис'),(2538,2514,2515,'Гиедрайчяй'),(2539,2514,2515,'Даугай'),(2540,2514,2515,'Дотнува'),(2541,2514,2515,'Друскининкай'),(2542,2514,2515,'Дукштас'),(2543,2514,2515,'Дусетос'),(2544,2514,2515,'Езнас'),(2545,2514,2515,'Ейшишес'),(2546,2514,2515,'Жагаре'),(2547,2514,2515,'Жямайчю Наумиестис'),(2548,2514,2515,'Зарасай'),(2549,2514,2515,'Игналина'),(2550,2514,2515,'Ионишкис'),(2551,2514,2515,'Йодкранте'),(2552,2514,2515,'Йонава'),(2553,2514,2515,'Казлу-Руда'),(2554,2514,2515,'Кайшядорис'),(2555,2514,2515,'Калвария'),(2556,2514,2515,'Капсукас'),(2557,2514,2515,'Каунас'),(2558,2514,2515,'Качергине'),(2559,2514,2515,'Кедайняй'),(2560,2514,2515,'Кельме'),(2561,2514,2515,'Кибартай'),(2562,2514,2515,'Клайпеда'),(2563,2514,2515,'Ковно'),(2564,2514,2515,'Кретинга'),(2565,2514,2515,'Кудиркос Наумиестис'),(2566,2514,2515,'Кулаутува'),(2567,2514,2515,'куодас'),(2568,2514,2515,'Купишкис'),(2569,2514,2515,'Куршенай'),(2570,2514,2515,'Лаздияй'),(2571,2514,2515,'Лейпалингис'),(2572,2514,2515,'Лентварис'),(2573,2514,2515,'Мажейкяй'),(2574,2514,2515,'Майшягала'),(2575,2514,2515,'Меркине'),(2576,2514,2515,'Молетай'),(2577,2514,2515,'Науйойи-Акмяне'),(2578,2514,2515,'Неринга'),(2579,2514,2515,'Пабраде'),(2580,2514,2515,'Пагегяй'),(2581,2514,2515,'Пакруоис'),(2582,2514,2515,'Паланга'),(2583,2514,2515,'Паневежис'),(2584,2514,2515,'Пасвалис'),(2585,2514,2515,'Плунге'),(2586,2514,2515,'Прейла'),(2587,2514,2515,'Приекуле'),(2588,2514,2515,'Приенай'),(2589,2514,2515,'Рагува'),(2590,2514,2515,'Радвилишкис'),(2591,2514,2515,'Расейняй'),(2592,2514,2515,'Риетавас'),(2593,2514,2515,'Рокишкис'),(2594,2514,2515,'Русне'),(2595,2514,2515,'Салантай'),(2596,2514,2515,'Симнас'),(2597,2514,2515,'Скаудвиле'),(2598,2514,2515,'Таураге'),(2599,2514,2515,'Тельшяй'),(2600,2514,2515,'Тракай'),(2601,2514,2515,'Укмерге'),(2602,2514,2515,'Утена'),(2603,2514,2515,'Шакяй'),(2604,2514,2515,'Шальчининкай'),(2605,2514,2515,'Швенченеляй'),(2606,2514,2515,'Швенчионис'),(2607,2514,2515,'Шета'),(2608,2514,2515,'Шилале'),(2609,2514,2515,'Шилуте'),(2610,2514,2515,'Ширвинтос'),(2611,2514,2515,'Шяуляй'),(2612,2514,2515,'Эжярелис'),(2613,2514,2515,'Юрбаркас'),(2616,2614,2615,'Люксембург'),(2619,2617,2618,'Агуаскалинтес'),(2621,2617,2620,'Акаюкан'),(2622,2617,2620,'Алтотонга'),(2623,2617,2620,'Альварадо'),(2624,2617,2620,'Веракрус'),(2625,2617,2620,'Коатепек'),(2626,2617,2620,'Коатцакоалькос'),(2627,2617,2620,'Кордоба'),(2628,2617,2620,'Косамалоапан (де Карпио)'),(2629,2617,2620,'Мартинес-де-ла-Торре'),(2630,2617,2620,'Минатитлан'),(2631,2617,2620,'Оризаба'),(2632,2617,2620,'Пануко'),(2633,2617,2620,'Папантла (де Оларте)'),(2634,2617,2620,'Поза-Рика-де-Хидальго'),(2635,2617,2620,'Сан-Андрес-Тукстла'),(2636,2617,2620,'Тихуатлан'),(2637,2617,2620,'Тукспан-де-Родригес-Кано'),(2638,2617,2620,'Тьерра-Бланка'),(2640,2617,2639,'Акапулько'),(2641,2617,2639,'Игуала'),(2642,2617,2639,'Такско-де-Аларкон'),(2643,2617,2639,'Текпан-де-Галина'),(2644,2617,2639,'Телолоапан'),(2645,2617,2639,'Чилпанкинго'),(2647,2617,2646,'Акамбаро'),(2648,2617,2646,'Валле-де-Сантъяго'),(2649,2617,2646,'Гуанахуато'),(2650,2617,2646,'Ирапуато'),(2651,2617,2646,'Леон (де лос Альдамас)'),(2652,2617,2646,'Пенхамо'),(2653,2617,2646,'Саламанка'),(2654,2617,2646,'Селая'),(2655,2617,2646,'Силао'),(2657,2617,2656,'Гомес-Палацио'),(2658,2617,2656,'Дуранго'),(2659,2617,2656,'Канатлан'),(2660,2617,2656,'Сьюдад-Лердо'),(2662,2617,2661,'Гуэхутла-де-Рейес'),(2663,2617,2661,'Иксмикуилпан'),(2664,2617,2661,'Пачука (де Сото)'),(2665,2617,2661,'Туланкинго'),(2667,2617,2666,'Кампеч'),(2669,2617,2668,'Куэретаро'),(2671,2617,2670,'Четумал'),(2673,2617,2672,'Матаморос'),(2674,2617,2672,'Монклова'),(2675,2617,2672,'Пьедрас-Неграс'),(2676,2617,2672,'Салтилло'),(2677,2617,2672,'Торреон'),(2678,2617,2672,'Фронтера'),(2680,2617,2679,'Колима'),(2681,2617,2679,'Манзанилло'),(2682,2617,2679,'Текоман'),(2684,2617,2683,'Куаутитлан'),(2685,2617,2683,'Наукалпан'),(2686,2617,2683,'Текскоко (де Мора)'),(2687,2617,2683,'Толука (де Лердо)'),(2688,2617,2683,'Хилотепек-де-Абасоло'),(2689,2617,2683,'Чалко'),(2691,2617,2690,'Апачинган'),(2692,2617,2690,'Закапу'),(2693,2617,2690,'Замора-де-Хидальго'),(2694,2617,2690,'Зитакуаро'),(2695,2617,2690,'Ла-Пидад (Кавадас)'),(2696,2617,2690,'Морелиа'),(2697,2617,2690,'Пацкуаро'),(2698,2617,2690,'Пуруандиро'),(2699,2617,2690,'Сахуэйо'),(2701,2617,2700,'Куаутла-Морелос'),(2702,2617,2700,'Куэрнавака'),(2703,2617,2700,'Хохутла'),(2705,2617,2704,'Акапонета'),(2706,2617,2704,'Компостела'),(2707,2617,2704,'Текуала'),(2708,2617,2704,'Тепик'),(2710,2617,2709,'Мехикали'),(2711,2617,2709,'Тиюана'),(2712,2617,2709,'Энсенада'),(2714,2617,2713,'Кадерита-Хименес'),(2715,2617,2713,'Линарес'),(2716,2617,2713,'Монтеррей'),(2718,2617,2717,'Оаксака (де Хуарес)'),(2719,2617,2717,'Почутла'),(2720,2617,2717,'Техуантепек'),(2721,2617,2717,'Тлаколула (де Матаморос)'),(2722,2617,2717,'Тукстепек'),(2723,2617,2717,'Хуахуапан-де-Леон'),(2724,2617,2717,'Хучитан (де Сарагоса)'),(2726,2617,2725,'Ицукар-де-Матаморос'),(2727,2617,2725,'Пуэбла (де Зарагоза)'),(2728,2617,2725,'Техуакан'),(2729,2617,2725,'Хуаучинанго'),(2731,2617,2730,'Закатекас'),(2732,2617,2730,'Сан-Мигель'),(2733,2617,2730,'Сомбререт'),(2734,2617,2730,'Фреснилло'),(2736,2617,2735,'Матехуала'),(2737,2617,2735,'Риоверде'),(2738,2617,2735,'Сан-Луис-Потоси'),(2739,2617,2735,'Сбюдад-де-Валлес'),(2740,2617,2735,'Херез-де-Гарсиа-Салинас'),(2742,2617,2741,'Гуасейв'),(2743,2617,2741,'Кулиакан'),(2744,2617,2741,'Мазатлан'),(2745,2617,2741,'Эскуинапа (де Хидальго)'),(2747,2617,2746,'Гуэймас'),(2748,2617,2746,'Емпалм'),(2749,2617,2746,'Навохоа'),(2750,2617,2746,'Сан-Луис-Рио-Колорадо'),(2751,2617,2746,'Сьюдад-Обрегон'),(2752,2617,2746,'Хермосилло'),(2753,2617,2746,'Хероика-Ногалес'),(2754,2617,2746,'Хуатабампо'),(2756,2617,2755,'Виллахермоса'),(2757,2617,2755,'Макуспана'),(2758,2617,2755,'Хуимангуилло'),(2760,2617,2759,'Валле-Хермосо'),(2761,2617,2759,'Нуэво-Ларедо'),(2762,2617,2759,'Риноса'),(2763,2617,2759,'Сьюдад-Мадеро'),(2764,2617,2759,'Тампико'),(2765,2617,2759,'Чиудад-Викториа'),(2767,2617,2766,'Мехико'),(2769,2617,2768,'Амека'),(2770,2617,2768,'Арандас'),(2771,2617,2768,'Атотонилко'),(2772,2617,2768,'Аутлан-де-Наварро'),(2773,2617,2768,'Гвадалахара'),(2774,2617,2768,'Ла-Барка'),(2775,2617,2768,'Лагос-де-Морено'),(2776,2617,2768,'Сьюдад-Гузман'),(2777,2617,2768,'Тепатитлан (де Морелос)'),(2779,2617,2778,'Чиуауа'),(2781,2617,2780,'Комитан (де Домингес)'),(2782,2617,2780,'Тапачула'),(2783,2617,2780,'Тонала'),(2784,2617,2780,'Тукстла-Гутьеррес'),(2785,2617,2780,'Чинталапа-де-Фигуэра'),(2787,2617,2786,'Мерида'),(2790,2788,2789,'Атаки'),(2791,2788,2789,'Бельцы'),(2792,2788,2789,'Бендеры'),(2793,2788,2789,'Бессарабка'),(2794,2788,2789,'Бричаны'),(2795,2788,2789,'Быковец'),(2796,2788,2789,'Вишневка'),(2797,2788,2789,'Вулканешты'),(2798,2788,2789,'Глодяны'),(2799,2788,2789,'Григориополь'),(2800,2788,2789,'Дондюшаны'),(2801,2788,2789,'Дрокия'),(2802,2788,2789,'Дубоссары'),(2803,2788,2789,'Единцы'),(2804,2788,2789,'Кагул'),(2805,2788,2789,'Калараш'),(2806,2788,2789,'Каменка'),(2807,2788,2789,'Каушаны'),(2808,2788,2789,'Кишинев'),(2809,2788,2789,'Комрат'),(2810,2788,2789,'Корнешты'),(2811,2788,2789,'Котовск'),(2812,2788,2789,'Криуляны'),(2813,2788,2789,'Леово'),(2814,2788,2789,'Ниспорены'),(2815,2788,2789,'Новые Анены'),(2816,2788,2789,'Окница'),(2817,2788,2789,'Оргеев'),(2818,2788,2789,'Резина'),(2819,2788,2789,'Рыбница'),(2820,2788,2789,'Рышканы'),(2821,2788,2789,'Слободзея'),(2822,2788,2789,'Сороки'),(2823,2788,2789,'Страшены'),(2824,2788,2789,'Тараклия'),(2825,2788,2789,'Теленешты'),(2826,2788,2789,'Тирасполь'),(2827,2788,2789,'Унгены'),(2828,2788,2789,'Фалешты'),(2829,2788,2789,'Флорешты'),(2830,2788,2789,'Чадыр-Лунга'),(2831,2788,2789,'Чимишлия'),(2832,2788,2789,'Чишмикиой'),(2835,2833,2834,'Монако'),(2836,2833,2834,'Монте-Карло'),(2839,2837,2838,'Манукау'),(2840,2837,2838,'Окленд'),(2842,2837,2841,'Роторуа'),(2843,2837,2841,'Тауранга'),(2845,2837,2844,'Ашбуртон'),(2846,2837,2844,'Тимару'),(2848,2837,2847,'Гисборн'),(2850,2837,2849,'Напир'),(2851,2837,2849,'Хастингс'),(2853,2837,2852,'Вангануи'),(2854,2837,2852,'Палмерстон-Норт'),(2856,2837,2855,'Бленхейм'),(2858,2837,2857,'Нельсон'),(2860,2837,2859,'Вангарей'),(2862,2837,2861,'Дунедин'),(2864,2837,2863,'Гор'),(2865,2837,2863,'Инверкаргилл'),(2867,2837,2866,'Нью-Плимут'),(2868,2837,2866,'Хавера'),(2870,2837,2869,'Ричмонд'),(2872,2837,2871,'Гамильтон'),(2873,2837,2871,'Таупо'),(2875,2837,2874,'Веллингтон'),(2876,2837,2874,'Ловер-Хатт'),(2877,2837,2874,'Мастертон'),(2879,2837,2878,'Греймоут'),(2882,2880,2881,'Драммен'),(2884,2880,2883,'Кристиансанд'),(2886,2880,2885,'Нарвик'),(2888,2880,2887,'Осло'),(2890,2880,2889,'Ставангер'),(2892,2880,2891,'Тронхейм'),(2894,2880,2893,'Скин'),(2896,2880,2895,'Берген'),(2899,2897,2898,'Билава'),(2900,2897,2898,'Богатыня'),(2901,2897,2898,'Болеславец'),(2902,2897,2898,'Валбржич'),(2903,2897,2898,'Вроцлав'),(2904,2897,2898,'Вроцлав ОА'),(2905,2897,2898,'Глогов'),(2906,2897,2898,'Дзирзонев'),(2907,2897,2898,'Желеня-Гора'),(2908,2897,2898,'Згорзелец'),(2909,2897,2898,'Клодзко'),(2910,2897,2898,'Легница'),(2911,2897,2898,'Любан'),(2912,2897,2898,'Нова-Руда'),(2913,2897,2898,'Олава'),(2914,2897,2898,'Олесница'),(2915,2897,2898,'Полковице'),(2916,2897,2898,'Свибоджице'),(2917,2897,2898,'Свидница'),(2918,2897,2898,'Явор'),(2920,2897,2919,'Бродница'),(2921,2897,2919,'Быдгощ'),(2922,2897,2919,'Влоцлавек'),(2923,2897,2919,'Грудзядзь'),(2924,2897,2919,'Иновроцлав'),(2925,2897,2919,'Накло-над-Нотеча'),(2926,2897,2919,'Свечье'),(2927,2897,2919,'Торун'),(2928,2897,2919,'Челмно'),(2930,2897,2929,'Горзов-Виелкопольски'),(2931,2897,2929,'Заган'),(2932,2897,2929,'Зары'),(2933,2897,2929,'Зелона-Гора'),(2934,2897,2929,'Меджиржеч'),(2935,2897,2929,'Нова-Сол'),(2936,2897,2929,'Свебоджин'),(2938,2897,2937,'Андрыхов'),(2939,2897,2937,'Бочния'),(2940,2897,2937,'Горлице'),(2941,2897,2937,'Закопане'),(2942,2897,2937,'Краков'),(2943,2897,2937,'Краков (обс. ул. Коперника)'),(2944,2897,2937,'Краков (обс. Форт Скала)'),(2945,2897,2937,'Краков (ш. им. Еромского)'),(2946,2897,2937,'Краков (ш. им. Нарутауича)'),(2947,2897,2937,'Краков (ш. ул. Вроклавска)'),(2948,2897,2937,'Краков (ш. ул. Галла)'),(2949,2897,2937,'Краков (ш. ул. Коперника)'),(2950,2897,2937,'Краков (ш. ул. Симирадзка)'),(2951,2897,2937,'Науи Сач'),(2952,2897,2937,'Новы-Тарг'),(2953,2897,2937,'Олкуш'),(2954,2897,2937,'Освецим'),(2955,2897,2937,'Скавина'),(2956,2897,2937,'Тарнов'),(2957,2897,2937,'Хрзанов'),(2959,2897,2958,'Варшава'),(2960,2897,2958,'Варшава ОА ПВ'),(2961,2897,2958,'Варшава ОА УВ'),(2962,2897,2958,'Воломин'),(2963,2897,2958,'Вышков'),(2964,2897,2958,'Гостынин'),(2965,2897,2958,'Гроджиск-Мазовецки'),(2966,2897,2958,'Жирардов'),(2967,2897,2958,'Козенице'),(2968,2897,2958,'Легионово'),(2969,2897,2958,'Минск-Мазовецки'),(2970,2897,2958,'Млава'),(2971,2897,2958,'Новы-Двор-Мазовецки'),(2972,2897,2958,'Остров-Мазовецки'),(2973,2897,2958,'Остролека'),(2974,2897,2958,'Отвок'),(2975,2897,2958,'Пионки'),(2976,2897,2958,'Плонск'),(2977,2897,2958,'Плоцк'),(2978,2897,2958,'Прушков'),(2979,2897,2958,'Пьястов'),(2980,2897,2958,'Радом'),(2981,2897,2958,'Седльце'),(2982,2897,2958,'Сохачев'),(2984,2897,2983,'Дебица'),(2985,2897,2983,'Кросно'),(2986,2897,2983,'Мелец'),(2987,2897,2983,'Пржемысл'),(2988,2897,2983,'Рхешов'),(2989,2897,2983,'Санок'),(2990,2897,2983,'Сталова-Вола'),(2991,2897,2983,'Тарнобржег'),(2992,2897,2983,'Ярослав'),(2993,2897,2983,'Ясло'),(2995,2897,2994,'Аугустов'),(2996,2897,2994,'Белосток'),(2997,2897,2994,'Бельск Подласки'),(2998,2897,2994,'Граево'),(2999,2897,2994,'Замбров'),(3000,2897,2994,'Ломжа'),(3001,2897,2994,'Сувалки'),(3002,2897,2994,'Хайнавка'),(3004,2897,3003,'Вейхерово'),(3005,2897,3003,'Гданьск'),(3006,2897,3003,'Гдыня'),(3007,2897,3003,'Квиджин'),(3008,2897,3003,'Косцержина'),(3009,2897,3003,'Леборк'),(3010,2897,3003,'Мальборк'),(3011,2897,3003,'Прущ-Гданьски'),(3012,2897,3003,'Румия'),(3013,2897,3003,'Слупск'),(3014,2897,3003,'Сопот'),(3015,2897,3003,'Старогард-Гданьски'),(3016,2897,3003,'Тчев'),(3017,2897,3003,'Чойнице'),(3019,2897,3018,'Беджин'),(3020,2897,3018,'Белско-Бяла'),(3021,2897,3018,'Берун-Новы'),(3022,2897,3018,'Бытом'),(3023,2897,3018,'Водзислав-Сласки'),(3024,2897,3018,'Гливице'),(3025,2897,3018,'Даброваа-Горница'),(3026,2897,3018,'Живец'),(3027,2897,3018,'Забрже'),(3028,2897,3018,'Заверцие'),(3029,2897,3018,'Зоры'),(3030,2897,3018,'Катовице'),(3031,2897,3018,'Люблинец'),(3032,2897,3018,'Миколов'),(3033,2897,3018,'Мысловице'),(3034,2897,3018,'Мышков'),(3035,2897,3018,'Пекары-Слаские'),(3036,2897,3018,'Пшчина'),(3037,2897,3018,'Пысковице'),(3038,2897,3018,'Рачиборз'),(3039,2897,3018,'Руда-Сласка'),(3040,2897,3018,'Рыбник'),(3041,2897,3018,'Сосновец'),(3042,2897,3018,'Тарновские-Горы'),(3043,2897,3018,'Тыхи'),(3044,2897,3018,'Цеховице-Дзедзице'),(3045,2897,3018,'Честохова'),(3046,2897,3018,'Чешин'),(3047,2897,3018,'Чорзов'),(3048,2897,3018,'Яворзно'),(3050,2897,3049,'Кельце'),(3051,2897,3049,'Конские'),(3052,2897,3049,'Островец-Свитокржиски'),(3053,2897,3049,'Сандомерж'),(3054,2897,3049,'Скаржиско-Каменна'),(3055,2897,3049,'Стараховице'),(3057,2897,3056,'Бартошице'),(3058,2897,3056,'Гижичко'),(3059,2897,3056,'Дзялдово'),(3060,2897,3056,'Илава'),(3061,2897,3056,'Кетржин'),(3062,2897,3056,'Мрагово'),(3063,2897,3056,'Ольштын'),(3064,2897,3056,'Острода'),(3065,2897,3056,'Шхов'),(3066,2897,3056,'Элблаг'),(3067,2897,3056,'Элк'),(3069,2897,3068,'Вагровец'),(3070,2897,3068,'Вржесня'),(3071,2897,3068,'Гнезно'),(3072,2897,3068,'Гостын'),(3073,2897,3068,'Калиш'),(3074,2897,3068,'Коло'),(3075,2897,3068,'Конин'),(3076,2897,3068,'Косциян'),(3077,2897,3068,'Кротошин'),(3078,2897,3068,'Лешно'),(3079,2897,3068,'Любон'),(3080,2897,3068,'Остров-Велкопольски'),(3081,2897,3068,'Пила'),(3082,2897,3068,'Познань'),(3083,2897,3068,'Равич'),(3084,2897,3068,'Сваржедж'),(3085,2897,3068,'Срем'),(3086,2897,3068,'Срода-Велкопольска'),(3087,2897,3068,'Турек'),(3088,2897,3068,'Чодзиеж'),(3089,2897,3068,'Яроцин'),(3091,2897,3090,'Валч'),(3092,2897,3090,'Голенёв'),(3093,2897,3090,'Грифино'),(3094,2897,3090,'Колобржег'),(3095,2897,3090,'Кошалин'),(3096,2897,3090,'Поличе'),(3097,2897,3090,'Свинойски'),(3098,2897,3090,'Старгард-Щецинский'),(3099,2897,3090,'Щецин'),(3100,2897,3090,'Щецинек'),(3102,2897,3101,'Александров-Ёдзжи'),(3103,2897,3101,'Вилун'),(3104,2897,3101,'Згерз'),(3105,2897,3101,'Здунска-Вола'),(3106,2897,3101,'Кутно'),(3107,2897,3101,'Ласк'),(3108,2897,3101,'Ловиц'),(3109,2897,3101,'Лодзь'),(3110,2897,3101,'Озорков'),(3111,2897,3101,'Опочно'),(3112,2897,3101,'Пабьянице'),(3113,2897,3101,'Пиотрков-Трыбунальски'),(3114,2897,3101,'Радомско'),(3115,2897,3101,'Серадзь'),(3116,2897,3101,'Скерневице'),(3117,2897,3101,'Томашов-Мазовецки'),(3119,2897,3118,'Биала Подласка'),(3120,2897,3118,'Билгорай'),(3121,2897,3118,'Замосц'),(3122,2897,3118,'Красник'),(3123,2897,3118,'Красныстав'),(3124,2897,3118,'Лешна'),(3125,2897,3118,'Лубартов'),(3126,2897,3118,'Луков'),(3127,2897,3118,'Люблин'),(3128,2897,3118,'Пулавы'),(3129,2897,3118,'Свидник'),(3130,2897,3118,'Томашов Любельски'),(3131,2897,3118,'Хрубешов'),(3132,2897,3118,'Челм'),(3134,2897,3133,'Бржег'),(3135,2897,3133,'Кедзержин-Козле'),(3136,2897,3133,'Ключборк'),(3137,2897,3133,'Ныса'),(3138,2897,3133,'Ополе'),(3139,2897,3133,'Прудник'),(3140,2897,3133,'Стржельце-Опольские'),(3143,3141,3142,'Коимбра'),(3145,3141,3144,'Фунчал'),(3147,3141,3146,'Амадора'),(3148,3141,3146,'Лиссабон'),(3149,3141,3146,'Сетубаль'),(3151,3141,3150,'Брага'),(3152,3141,3150,'Вила-Нова-де-Гайя'),(3153,3141,3150,'Матосинхос'),(3154,3141,3150,'Опорто'),(3155,3141,3150,'Порту'),(3158,3156,3157,'Сант-Денис'),(3161,3159,3160,'Акташ'),(3162,3159,3160,'Акутиха'),(3163,3159,3160,'Алейск'),(3164,3159,3160,'Алтайский'),(3165,3159,3160,'Баево'),(3166,3159,3160,'Барнаул'),(3167,3159,3160,'Белово'),(3168,3159,3160,'Белокуриха'),(3169,3159,3160,'Белоярск'),(3170,3159,3160,'Бийск'),(3171,3159,3160,'Благовещенск'),(3172,3159,3160,'Боровлянка'),(3173,3159,3160,'Бурла'),(3174,3159,3160,'Бурсоль'),(3175,3159,3160,'Волчиха'),(3176,3159,3160,'Горно-Алтайск'),(3177,3159,3160,'Горняк'),(3178,3159,3160,'Ельцовка'),(3179,3159,3160,'Залесово'),(3180,3159,3160,'Заринск'),(3181,3159,3160,'Заток'),(3182,3159,3160,'Змеиногорск'),(3183,3159,3160,'Камень-на-Оби'),(3184,3159,3160,'Ключи'),(3185,3159,3160,'Кош-Агач'),(3186,3159,3160,'Красногорское'),(3187,3159,3160,'Краснощеково'),(3188,3159,3160,'Кулунда'),(3189,3159,3160,'Кытманово'),(3190,3159,3160,'Мамонтово'),(3191,3159,3160,'Новичиха'),(3192,3159,3160,'Новоалтайск'),(3193,3159,3160,'Онгудай'),(3194,3159,3160,'Павловск'),(3195,3159,3160,'Петропавловское'),(3196,3159,3160,'Поспелиха'),(3197,3159,3160,'Ребриха'),(3198,3159,3160,'Родино'),(3199,3159,3160,'Рубцовск'),(3200,3159,3160,'Славгород'),(3201,3159,3160,'Смоленское'),(3202,3159,3160,'Солонешное'),(3203,3159,3160,'Солтон'),(3204,3159,3160,'Староаллейское'),(3205,3159,3160,'Табуны'),(3206,3159,3160,'Тальменка'),(3207,3159,3160,'Топчиха'),(3208,3159,3160,'Троицкое'),(3209,3159,3160,'Турочак'),(3210,3159,3160,'Тюменцево'),(3211,3159,3160,'Угловское'),(3212,3159,3160,'Усть-Калманка'),(3213,3159,3160,'Усть-Кан'),(3214,3159,3160,'Усть-Кокса'),(3215,3159,3160,'Усть-Улаган'),(3216,3159,3160,'Усть-Чарышская Пристань'),(3217,3159,3160,'Хабары'),(3218,3159,3160,'Целинное'),(3219,3159,3160,'Чарышское'),(3220,3159,3160,'Шебалино'),(3221,3159,3160,'Шелаболиха'),(3222,3159,3160,'Шипуново'),(3224,3159,3223,'Айгунь'),(3225,3159,3223,'Архара'),(3226,3159,3223,'Белогорск'),(3227,3159,3223,'Благовещенск (Амурская обл.)'),(3228,3159,3223,'Бурея'),(3229,3159,3223,'Возжаевка'),(3230,3159,3223,'Екатеринославка'),(3231,3159,3223,'Ерофей Павлович'),(3232,3159,3223,'Завитинск'),(3233,3159,3223,'Зея'),(3234,3159,3223,'Златоустовск'),(3235,3159,3223,'Ивановка'),(3236,3159,3223,'Коболдо'),(3237,3159,3223,'Магдагачи'),(3238,3159,3223,'Новобурейский'),(3239,3159,3223,'Поярково'),(3240,3159,3223,'Райчихинск'),(3241,3159,3223,'Ромны'),(3242,3159,3223,'Свободный'),(3243,3159,3223,'Серышево'),(3244,3159,3223,'Сковородино'),(3245,3159,3223,'Стойба'),(3246,3159,3223,'Тамбовка'),(3247,3159,3223,'Тында'),(3248,3159,3223,'Шимановск'),(3249,3159,3223,'Экимчан'),(3250,3159,3223,'Ядрино'),(3252,3159,3251,'Амдерма'),(3253,3159,3251,'Архангельск'),(3254,3159,3251,'Березник'),(3255,3159,3251,'Вельск'),(3256,3159,3251,'Верхняя Тойма'),(3257,3159,3251,'Волошка'),(3258,3159,3251,'Вычегодский'),(3259,3159,3251,'Емца'),(3260,3159,3251,'Илеза'),(3261,3159,3251,'Ильинско-Подомское'),(3262,3159,3251,'Каргополь'),(3263,3159,3251,'Карпогоры'),(3264,3159,3251,'Кодино'),(3265,3159,3251,'Коноша'),(3266,3159,3251,'Коряжма'),(3267,3159,3251,'Котлас'),(3268,3159,3251,'Красноборск'),(3269,3159,3251,'Лешуконское'),(3270,3159,3251,'Мезень'),(3271,3159,3251,'Нарьян-Мар'),(3272,3159,3251,'Новодвинск'),(3273,3159,3251,'Няндома'),(3274,3159,3251,'Онега'),(3275,3159,3251,'Пинега'),(3276,3159,3251,'Плесецк'),(3277,3159,3251,'Северодвинск'),(3278,3159,3251,'Сольвычегодск'),(3279,3159,3251,'Холмогоры'),(3280,3159,3251,'Шенкурск'),(3281,3159,3251,'Яренск'),(3283,3159,3282,'Астрахань'),(3284,3159,3282,'Ахтубинск'),(3285,3159,3282,'Верхний Баскунчак'),(3286,3159,3282,'Володарский'),(3287,3159,3282,'Енотаевка'),(3288,3159,3282,'Икряное'),(3289,3159,3282,'Камызяк'),(3290,3159,3282,'Капустин Яр'),(3291,3159,3282,'Красный Яр'),(3292,3159,3282,'Лиман'),(3293,3159,3282,'Началово'),(3294,3159,3282,'Харабали'),(3295,3159,3282,'Черный Яр'),(3297,3159,3296,'Аксаково'),(3298,3159,3296,'Амзя'),(3299,3159,3296,'Аскино'),(3300,3159,3296,'Баймак'),(3301,3159,3296,'Бакалы'),(3302,3159,3296,'Белебей'),(3303,3159,3296,'Белорецк'),(3304,3159,3296,'Бижбуляк'),(3305,3159,3296,'Бирск'),(3306,3159,3296,'Благовещенск'),(3307,3159,3296,'Большеустьикинское'),(3308,3159,3296,'Верхнеяркеево'),(3309,3159,3296,'Верхние Киги'),(3310,3159,3296,'Верхние Татышлы'),(3311,3159,3296,'Верхний Авзян'),(3312,3159,3296,'Давлеканово'),(3313,3159,3296,'Дуван'),(3314,3159,3296,'Дюртюли'),(3315,3159,3296,'Ермекеево'),(3316,3159,3296,'Ермолаево'),(3317,3159,3296,'Зилаир'),(3318,3159,3296,'Зирган'),(3319,3159,3296,'Иглино'),(3320,3159,3296,'Инзер'),(3321,3159,3296,'Исянгулово'),(3322,3159,3296,'Ишимбай'),(3323,3159,3296,'Кананикольское'),(3324,3159,3296,'Кандры'),(3325,3159,3296,'Караидель'),(3326,3159,3296,'Караидельский'),(3327,3159,3296,'Киргиз-Мияки'),(3328,3159,3296,'Красноусольский'),(3329,3159,3296,'Кумертау'),(3330,3159,3296,'Кушнаренково'),(3331,3159,3296,'Малояз'),(3332,3159,3296,'Мелеуз'),(3333,3159,3296,'Месягутово'),(3334,3159,3296,'Мраково'),(3335,3159,3296,'Нефтекамск'),(3336,3159,3296,'Октябрьский'),(3337,3159,3296,'Раевский'),(3338,3159,3296,'Салават'),(3339,3159,3296,'Сибай'),(3340,3159,3296,'Старобалтачево'),(3341,3159,3296,'Старосубхангулово'),(3342,3159,3296,'Стерлибашево'),(3343,3159,3296,'Стерлитамак'),(3344,3159,3296,'Туймазы'),(3345,3159,3296,'Уфа'),(3346,3159,3296,'Учалы'),(3347,3159,3296,'Федоровка'),(3348,3159,3296,'Чекмагуш'),(3349,3159,3296,'Чишмы'),(3350,3159,3296,'Шаран'),(3351,3159,3296,'Янаул'),(3353,3159,3352,'Алексеевка'),(3354,3159,3352,'Белгород'),(3355,3159,3352,'Борисовка'),(3356,3159,3352,'Валуйки'),(3357,3159,3352,'Вейделевка'),(3358,3159,3352,'Волоконовка'),(3359,3159,3352,'Грайворон'),(3360,3159,3352,'Губкин'),(3361,3159,3352,'Ивня'),(3362,3159,3352,'Короча'),(3363,3159,3352,'Красногвардейское'),(3364,3159,3352,'Новый Оскол'),(3365,3159,3352,'Ракитное'),(3366,3159,3352,'Ровеньки'),(3367,3159,3352,'Старый Оскол'),(3368,3159,3352,'Строитель'),(3369,3159,3352,'Чернянка'),(3370,3159,3352,'Шебекино'),(3372,3159,3371,'Алтухово'),(3373,3159,3371,'Белая Березка'),(3374,3159,3371,'Белые Берега'),(3375,3159,3371,'Большое Полпино'),(3376,3159,3371,'Брянск'),(3377,3159,3371,'Бытошь'),(3378,3159,3371,'Выгоничи'),(3379,3159,3371,'Вышков'),(3380,3159,3371,'Гордеевка'),(3381,3159,3371,'Дубровка'),(3382,3159,3371,'Жирятино'),(3383,3159,3371,'Жуковка'),(3384,3159,3371,'Злынка'),(3385,3159,3371,'Ивот'),(3386,3159,3371,'Карачев'),(3387,3159,3371,'Клетня'),(3388,3159,3371,'Климово'),(3389,3159,3371,'Клинцы'),(3390,3159,3371,'Кокаревка'),(3391,3159,3371,'Комаричи'),(3392,3159,3371,'Красная Гора'),(3393,3159,3371,'Локоть'),(3394,3159,3371,'Мглин'),(3395,3159,3371,'Навля'),(3396,3159,3371,'Новозыбков'),(3397,3159,3371,'Погар'),(3398,3159,3371,'Почеп'),(3399,3159,3371,'Ржаница'),(3400,3159,3371,'Рогнедино'),(3401,3159,3371,'Севск'),(3402,3159,3371,'Стародуб'),(3403,3159,3371,'Суземка'),(3404,3159,3371,'Сураж'),(3405,3159,3371,'Трубчевск'),(3406,3159,3371,'Унеча'),(3408,3159,3407,'Бабушкин'),(3409,3159,3407,'Багдарин'),(3410,3159,3407,'Баргузин'),(3411,3159,3407,'Баянгол'),(3412,3159,3407,'Бичура'),(3413,3159,3407,'Выдрино'),(3414,3159,3407,'Гусиное Озеро'),(3415,3159,3407,'Гусиноозерск'),(3416,3159,3407,'Заиграево'),(3417,3159,3407,'Закаменск'),(3418,3159,3407,'Иволгинск'),(3419,3159,3407,'Илька'),(3420,3159,3407,'Кабанск'),(3421,3159,3407,'Каменск'),(3422,3159,3407,'Кижинга'),(3423,3159,3407,'Курумкан'),(3424,3159,3407,'Кырен'),(3425,3159,3407,'Кяхта'),(3426,3159,3407,'Монды'),(3427,3159,3407,'Мухоршибирь'),(3428,3159,3407,'Нижнеангарск'),(3429,3159,3407,'Орлик'),(3430,3159,3407,'Петропавловка'),(3431,3159,3407,'Романовка'),(3432,3159,3407,'Селенгинск'),(3433,3159,3407,'Сосново-Озерское'),(3434,3159,3407,'Турунтаево'),(3435,3159,3407,'Улан-Удэ'),(3436,3159,3407,'Хоринск'),(3438,3159,3437,'Александров'),(3439,3159,3437,'Андреево'),(3440,3159,3437,'Анопино'),(3441,3159,3437,'Бавлены'),(3442,3159,3437,'Балакирево'),(3443,3159,3437,'Боголюбово'),(3444,3159,3437,'Великодворский'),(3445,3159,3437,'Вербовский'),(3446,3159,3437,'Владимир'),(3447,3159,3437,'Вязники'),(3448,3159,3437,'Городищи'),(3449,3159,3437,'Гороховец'),(3450,3159,3437,'Гусевский'),(3451,3159,3437,'Гусь Хрустальный'),(3452,3159,3437,'Золотково'),(3453,3159,3437,'Иванищи'),(3454,3159,3437,'Камешково'),(3455,3159,3437,'Карабаново'),(3456,3159,3437,'Киржач'),(3457,3159,3437,'Ковров'),(3458,3159,3437,'Кольчугино'),(3459,3159,3437,'Красная Горбатка'),(3460,3159,3437,'Меленки'),(3461,3159,3437,'Муром'),(3462,3159,3437,'Петушки'),(3463,3159,3437,'Покров'),(3464,3159,3437,'Собинка'),(3465,3159,3437,'Судогда'),(3466,3159,3437,'Суздаль'),(3467,3159,3437,'Юрьев-Польский'),(3469,3159,3468,'Алексеевская'),(3470,3159,3468,'Алущевск'),(3471,3159,3468,'Быково'),(3472,3159,3468,'Волгоград'),(3473,3159,3468,'Волжский'),(3474,3159,3468,'Городище'),(3475,3159,3468,'Дубовка'),(3476,3159,3468,'Елань'),(3477,3159,3468,'Жирновск'),(3478,3159,3468,'Иловля'),(3479,3159,3468,'Калач-на-Дону'),(3480,3159,3468,'Камышин'),(3481,3159,3468,'Клетский'),(3482,3159,3468,'Котельниково'),(3483,3159,3468,'Котово'),(3484,3159,3468,'Кумылженская'),(3485,3159,3468,'Ленинск'),(3486,3159,3468,'Михайловка'),(3487,3159,3468,'Нехаевский'),(3488,3159,3468,'Николаевск'),(3489,3159,3468,'Новоаннинский'),(3490,3159,3468,'Новониколаевский'),(3491,3159,3468,'Ольховка'),(3492,3159,3468,'Палласовка'),(3493,3159,3468,'Рудня'),(3494,3159,3468,'Светлый Яр'),(3495,3159,3468,'Серафимович'),(3496,3159,3468,'Средняя Ахтуба'),(3497,3159,3468,'Сталинград'),(3498,3159,3468,'Старая Полтавка'),(3499,3159,3468,'Суровикино'),(3500,3159,3468,'Урюпинск'),(3501,3159,3468,'Фролово'),(3502,3159,3468,'Чернышковский'),(3504,3159,3503,'Бабаево'),(3505,3159,3503,'Белозерск'),(3506,3159,3503,'Великий Устюг'),(3507,3159,3503,'Верховажье'),(3508,3159,3503,'Вожега'),(3509,3159,3503,'Вологда'),(3510,3159,3503,'Вохтога'),(3511,3159,3503,'Вытегра'),(3512,3159,3503,'Грязовец'),(3513,3159,3503,'Кадников'),(3514,3159,3503,'Кадуй'),(3515,3159,3503,'Кичменгский Городок'),(3516,3159,3503,'Липин Бор'),(3517,3159,3503,'Никольск'),(3518,3159,3503,'Нюксеница'),(3519,3159,3503,'Сокол'),(3520,3159,3503,'Сямжа'),(3521,3159,3503,'Тарногский Городок'),(3522,3159,3503,'Тотьма'),(3523,3159,3503,'Устюжна'),(3524,3159,3503,'Харовск'),(3525,3159,3503,'Чагода'),(3526,3159,3503,'Череповец'),(3527,3159,3503,'Шексна'),(3528,3159,3503,'Шуйское'),(3530,3159,3529,'Анна'),(3531,3159,3529,'Бобров'),(3532,3159,3529,'Богучар'),(3533,3159,3529,'Борисоглебск'),(3534,3159,3529,'Бутурлиновка'),(3535,3159,3529,'Верхний Мамон'),(3536,3159,3529,'Верхняя Хава'),(3537,3159,3529,'Воробьевка'),(3538,3159,3529,'Воронеж'),(3539,3159,3529,'Лиски'),(3540,3159,3529,'Грибановский'),(3541,3159,3529,'Давыдовка'),(3542,3159,3529,'Елань-Коленовский'),(3543,3159,3529,'Калач'),(3544,3159,3529,'Кантемировка'),(3545,3159,3529,'Нижнедевицк'),(3546,3159,3529,'Новая Усмань'),(3547,3159,3529,'Новохоперск'),(3548,3159,3529,'Ольховатка'),(3549,3159,3529,'Острогожск'),(3550,3159,3529,'Павловск'),(3551,3159,3529,'Панино'),(3552,3159,3529,'Петропавловка'),(3553,3159,3529,'Поворино'),(3554,3159,3529,'Подгоренский'),(3555,3159,3529,'Рамонь'),(3556,3159,3529,'Репьевка'),(3557,3159,3529,'Россошь'),(3558,3159,3529,'Семилуки'),(3559,3159,3529,'Таловая'),(3560,3159,3529,'Терновка'),(3561,3159,3529,'Хохольский'),(3562,3159,3529,'Эртиль'),(3564,3159,3563,'Ардатов'),(3565,3159,3563,'Арзамас'),(3566,3159,3563,'Арья'),(3567,3159,3563,'Балахна'),(3568,3159,3563,'Богородск'),(3569,3159,3563,'Большереченск'),(3570,3159,3563,'Большое Болдино'),(3571,3159,3563,'Большое Козино'),(3572,3159,3563,'Большое Мурашкино'),(3573,3159,3563,'Большое Пикино'),(3574,3159,3563,'Бор'),(3575,3159,3563,'Бутурлино'),(3576,3159,3563,'Вад'),(3577,3159,3563,'Варнавино'),(3578,3159,3563,'Васильсурск'),(3579,3159,3563,'Вахтан'),(3580,3159,3563,'Вача'),(3581,3159,3563,'Велетьма'),(3582,3159,3563,'Ветлуга'),(3583,3159,3563,'Виля'),(3584,3159,3563,'Вознесенское'),(3585,3159,3563,'Володарск'),(3586,3159,3563,'Воротынец'),(3587,3159,3563,'Ворсма'),(3588,3159,3563,'Воскресенское'),(3589,3159,3563,'Выездное'),(3590,3159,3563,'Выкса'),(3591,3159,3563,'Гагино'),(3592,3159,3563,'Гидроторф'),(3593,3159,3563,'Горбатов'),(3594,3159,3563,'Горбатовка'),(3595,3159,3563,'Городец'),(3596,3159,3563,'Горький'),(3597,3159,3563,'Дальнее Константиново'),(3598,3159,3563,'Дзержинск'),(3599,3159,3563,'Дивеево'),(3600,3159,3563,'Досчатое'),(3601,3159,3563,'Заволжье'),(3602,3159,3563,'Катунки'),(3603,3159,3563,'Керженец'),(3604,3159,3563,'Княгинино'),(3605,3159,3563,'Ковернино'),(3606,3159,3563,'Красные Баки'),(3607,3159,3563,'Кстово'),(3608,3159,3563,'Кулебаки'),(3609,3159,3563,'Лукоянов'),(3610,3159,3563,'Лысково'),(3611,3159,3563,'Навашино'),(3612,3159,3563,'Нижний Новгород'),(3613,3159,3563,'Павлово'),(3614,3159,3563,'Первомайск'),(3615,3159,3563,'Перевоз'),(3616,3159,3563,'Пильна'),(3617,3159,3563,'Починки'),(3618,3159,3563,'Сергач'),(3619,3159,3563,'Сеченово'),(3620,3159,3563,'Сосновское'),(3621,3159,3563,'Спасское'),(3622,3159,3563,'Тонкино'),(3623,3159,3563,'Тоншаево'),(3624,3159,3563,'Уразовка'),(3625,3159,3563,'Урень'),(3626,3159,3563,'Чкаловск'),(3627,3159,3563,'Шаранга'),(3628,3159,3563,'Шатки'),(3629,3159,3563,'Шахунья'),(3631,3159,3630,'Агвали'),(3632,3159,3630,'Акуша'),(3633,3159,3630,'Ахты'),(3634,3159,3630,'Ачису'),(3635,3159,3630,'Бабаюрт'),(3636,3159,3630,'Бежта'),(3637,3159,3630,'Ботлих'),(3638,3159,3630,'Буйнакск'),(3639,3159,3630,'Вачи'),(3640,3159,3630,'Гергебиль'),(3641,3159,3630,'Гуниб'),(3642,3159,3630,'Дагестанские Огни'),(3643,3159,3630,'Дербент'),(3644,3159,3630,'Дылым'),(3645,3159,3630,'Ершовка'),(3646,3159,3630,'Избербаш'),(3647,3159,3630,'Карабудахкент'),(3648,3159,3630,'Карата'),(3649,3159,3630,'Каспийск'),(3650,3159,3630,'Касумкент'),(3651,3159,3630,'Кизилюрт'),(3652,3159,3630,'Кизляр'),(3653,3159,3630,'Кочубей'),(3654,3159,3630,'Кумух'),(3655,3159,3630,'Курах'),(3656,3159,3630,'Магарамкент'),(3657,3159,3630,'Маджалис'),(3658,3159,3630,'Махачкала'),(3659,3159,3630,'Мехельта'),(3660,3159,3630,'Новолакское'),(3661,3159,3630,'Рутул'),(3662,3159,3630,'Советское'),(3663,3159,3630,'Тарумовка'),(3664,3159,3630,'Терекли-Мектеб'),(3665,3159,3630,'Тлярата'),(3666,3159,3630,'Тпиг'),(3667,3159,3630,'Уркарах'),(3668,3159,3630,'Хасавюрт'),(3669,3159,3630,'Хив'),(3670,3159,3630,'Хунзах'),(3671,3159,3630,'Цуриб'),(3672,3159,3630,'Южно-Сухокумск'),(3674,3159,3673,'Биробиджан'),(3676,3159,3675,'Архиповка'),(3677,3159,3675,'Верхний Ландех'),(3678,3159,3675,'Вичуга'),(3679,3159,3675,'Гаврилов Посад'),(3680,3159,3675,'Долматовский'),(3681,3159,3675,'Дуляпино'),(3682,3159,3675,'Заволжск'),(3683,3159,3675,'Заречный'),(3684,3159,3675,'Иваново'),(3685,3159,3675,'Иваньковский'),(3686,3159,3675,'Ильинское-Хованское'),(3687,3159,3675,'Каминский'),(3688,3159,3675,'Кинешма'),(3689,3159,3675,'Комсомольск'),(3690,3159,3675,'Лух'),(3691,3159,3675,'Палех'),(3692,3159,3675,'Пестяки'),(3693,3159,3675,'Приволжск'),(3694,3159,3675,'Пучеж'),(3695,3159,3675,'Родники'),(3696,3159,3675,'Савино'),(3697,3159,3675,'Сокольское'),(3698,3159,3675,'Тейково'),(3699,3159,3675,'Фурманов'),(3700,3159,3675,'Шуя'),(3701,3159,3675,'Южа'),(3702,3159,3675,'Юрьевец'),(3704,3159,3703,'Алексеевск'),(3705,3159,3703,'Алзамай'),(3706,3159,3703,'Алыгжер'),(3707,3159,3703,'Ангарск'),(3708,3159,3703,'Артемовский'),(3709,3159,3703,'Атагай'),(3710,3159,3703,'Байкал'),(3711,3159,3703,'Байкальск'),(3712,3159,3703,'Балаганск'),(3713,3159,3703,'Баяндай'),(3714,3159,3703,'Бирюсинск'),(3715,3159,3703,'Бодайбо'),(3716,3159,3703,'Большая Речка'),(3717,3159,3703,'Большой Луг'),(3718,3159,3703,'Бохан'),(3719,3159,3703,'Братск'),(3720,3159,3703,'Видим'),(3721,3159,3703,'Витимский'),(3722,3159,3703,'Вихоревка'),(3723,3159,3703,'Еланцы'),(3724,3159,3703,'Ербогачен'),(3725,3159,3703,'Железногорск-Илимский'),(3726,3159,3703,'Жигалово'),(3727,3159,3703,'Забитуй'),(3728,3159,3703,'Залари'),(3729,3159,3703,'Звездный'),(3730,3159,3703,'Зима'),(3731,3159,3703,'Иркутск'),(3732,3159,3703,'Казачинское'),(3733,3159,3703,'Качуг'),(3734,3159,3703,'Квиток'),(3735,3159,3703,'Киренск'),(3736,3159,3703,'Куйтун'),(3737,3159,3703,'Кутулик'),(3738,3159,3703,'Мама'),(3739,3159,3703,'Нижнеудинск'),(3740,3159,3703,'Оса'),(3741,3159,3703,'Слюдянка'),(3742,3159,3703,'Тайшет'),(3743,3159,3703,'Тулун'),(3744,3159,3703,'Усолье-Сибирское'),(3745,3159,3703,'Усть-Илимск'),(3746,3159,3703,'Усть-Кут'),(3747,3159,3703,'Усть-Ордынский'),(3748,3159,3703,'Усть-Уда'),(3749,3159,3703,'Черемхово'),(3750,3159,3703,'Шелехов'),(3752,3159,3751,'Баксан'),(3753,3159,3751,'Майский'),(3754,3159,3751,'Нальчик'),(3755,3159,3751,'Нарткала'),(3756,3159,3751,'Прохладный'),(3757,3159,3751,'Советское'),(3758,3159,3751,'Терек'),(3759,3159,3751,'Тырныауз'),(3760,3159,3751,'Чегем-Первый'),(3762,3159,3761,'Багратионовск'),(3763,3159,3761,'Балтийск'),(3764,3159,3761,'Гвардейск'),(3765,3159,3761,'Гурьевск'),(3766,3159,3761,'Гусев'),(3767,3159,3761,'Железнодорожный'),(3768,3159,3761,'Зеленоградск'),(3769,3159,3761,'Знаменск'),(3770,3159,3761,'Калининград'),(3771,3159,3761,'Кенисберг'),(3772,3159,3761,'Краснознаменск'),(3773,3159,3761,'Мамоново'),(3774,3159,3761,'Неман'),(3775,3159,3761,'Нестеров'),(3776,3159,3761,'Озерск'),(3777,3159,3761,'Полесск'),(3778,3159,3761,'Правдинск'),(3779,3159,3761,'Светлогорск'),(3780,3159,3761,'Светлый'),(3781,3159,3761,'Славск'),(3782,3159,3761,'Советск'),(3783,3159,3761,'Черняховск'),(3785,3159,3784,'Андреаполь'),(3786,3159,3784,'Бежецк'),(3787,3159,3784,'Белый'),(3788,3159,3784,'Белый Городок'),(3789,3159,3784,'Березайка'),(3790,3159,3784,'Бологое'),(3791,3159,3784,'Васильевский Мох'),(3792,3159,3784,'Выползово'),(3793,3159,3784,'Вышний Волочек'),(3794,3159,3784,'Жарковский'),(3795,3159,3784,'Западная Двина'),(3796,3159,3784,'Заречье'),(3797,3159,3784,'Зубцов'),(3798,3159,3784,'Изоплит'),(3799,3159,3784,'Калашниково'),(3800,3159,3784,'Калинин'),(3801,3159,3784,'Калязин'),(3802,3159,3784,'Кашин'),(3803,3159,3784,'Кесова Гора'),(3804,3159,3784,'Кимры'),(3805,3159,3784,'Конаково'),(3806,3159,3784,'Красный Холм'),(3807,3159,3784,'Кувшиново'),(3808,3159,3784,'Лесное'),(3809,3159,3784,'Лихославль'),(3810,3159,3784,'Максатиха'),(3811,3159,3784,'Молоково'),(3812,3159,3784,'Нелидово'),(3813,3159,3784,'Оленино'),(3814,3159,3784,'Осташков'),(3815,3159,3784,'Пено'),(3816,3159,3784,'Рамешки'),(3817,3159,3784,'Ржев'),(3818,3159,3784,'Сандово'),(3819,3159,3784,'Селижарово'),(3820,3159,3784,'Сонково'),(3821,3159,3784,'Спирово'),(3822,3159,3784,'Старица'),(3823,3159,3784,'Тверь'),(3824,3159,3784,'Торжок'),(3825,3159,3784,'Торопец'),(3826,3159,3784,'Фирово'),(3828,3159,3827,'Аршань'),(3829,3159,3827,'Каспийский'),(3830,3159,3827,'Комсомольский'),(3831,3159,3827,'Малые Дербеты'),(3832,3159,3827,'Приютное'),(3833,3159,3827,'Советское'),(3834,3159,3827,'Троицкое'),(3835,3159,3827,'Утта'),(3836,3159,3827,'Цаган-Аман'),(3837,3159,3827,'Элиста'),(3838,3159,3827,'Юста'),(3839,3159,3827,'Яшалта'),(3840,3159,3827,'Яшкуль'),(3842,3159,3841,'Бабынино'),(3843,3159,3841,'Балабаново'),(3844,3159,3841,'Барятино'),(3845,3159,3841,'Белоусово'),(3846,3159,3841,'Бетлица'),(3847,3159,3841,'Боровск'),(3848,3159,3841,'Дугна'),(3849,3159,3841,'Дудоровский'),(3850,3159,3841,'Думиничи'),(3851,3159,3841,'Еленский'),(3852,3159,3841,'Жиздра'),(3853,3159,3841,'Износки'),(3854,3159,3841,'Калуга'),(3855,3159,3841,'Киров'),(3856,3159,3841,'Козельск'),(3857,3159,3841,'Кондрово'),(3858,3159,3841,'Людиново'),(3859,3159,3841,'Малоярославец'),(3860,3159,3841,'Медынь'),(3861,3159,3841,'Мещовск'),(3862,3159,3841,'Мосальск'),(3863,3159,3841,'Обнинск'),(3864,3159,3841,'Перемышль'),(3865,3159,3841,'Спас-Деменск'),(3866,3159,3841,'Сухиничи'),(3867,3159,3841,'Таруса'),(3868,3159,3841,'Ульяново'),(3869,3159,3841,'Ферзиково'),(3870,3159,3841,'Хвастовичи'),(3871,3159,3841,'Юхнов'),(3873,3159,3872,'Атласово'),(3874,3159,3872,'Аянка'),(3875,3159,3872,'Большерецк'),(3876,3159,3872,'Ильпырский'),(3877,3159,3872,'Каменское'),(3878,3159,3872,'Кировский'),(3879,3159,3872,'Ключи'),(3880,3159,3872,'Крапивная'),(3881,3159,3872,'Мильково'),(3882,3159,3872,'Никольское'),(3883,3159,3872,'Оссора'),(3884,3159,3872,'Палана'),(3885,3159,3872,'Парень'),(3886,3159,3872,'Пахачи'),(3887,3159,3872,'Петропавловск-Камчатский'),(3888,3159,3872,'Тигиль'),(3889,3159,3872,'Тиличики'),(3890,3159,3872,'Усть-Большерецк'),(3891,3159,3872,'Усть-Камчатск'),(3893,3159,3892,'Амбарный'),(3894,3159,3892,'Беломорск'),(3895,3159,3892,'Валаам'),(3896,3159,3892,'Вирандозеро'),(3897,3159,3892,'Гирвас'),(3898,3159,3892,'Деревянка'),(3899,3159,3892,'Идель'),(3900,3159,3892,'Ильинский'),(3901,3159,3892,'Импалахти'),(3902,3159,3892,'Калевала'),(3903,3159,3892,'Кемь'),(3904,3159,3892,'Кестеньга'),(3905,3159,3892,'Кондопога'),(3906,3159,3892,'Костомукша'),(3907,3159,3892,'Лахденпохья'),(3908,3159,3892,'Лоухи'),(3909,3159,3892,'Медвежьегорск'),(3910,3159,3892,'Муезерский'),(3911,3159,3892,'Олонец'),(3912,3159,3892,'Петрозаводск'),(3913,3159,3892,'Питкяранта'),(3914,3159,3892,'Повенец'),(3915,3159,3892,'Пряжа'),(3916,3159,3892,'Пудож'),(3917,3159,3892,'Сегежа'),(3918,3159,3892,'Сортавала'),(3919,3159,3892,'Софпорог'),(3920,3159,3892,'Суоярви'),(3922,3159,3921,'Анжеро-Судженск'),(3923,3159,3921,'Барзас'),(3924,3159,3921,'Белово'),(3925,3159,3921,'Белогорск'),(3926,3159,3921,'Березовский'),(3927,3159,3921,'Грамотеино'),(3928,3159,3921,'Гурьевск'),(3929,3159,3921,'Ижморский'),(3930,3159,3921,'Итатский'),(3931,3159,3921,'Калтан'),(3932,3159,3921,'Кедровка'),(3933,3159,3921,'Кемерово'),(3934,3159,3921,'Киселевск'),(3935,3159,3921,'Крапивинский'),(3936,3159,3921,'Ленинск-Кузнецкий'),(3937,3159,3921,'Мариинск'),(3938,3159,3921,'Междуреченск'),(3939,3159,3921,'Мыски'),(3940,3159,3921,'Новокузнецк'),(3941,3159,3921,'Осинники'),(3942,3159,3921,'Прокопьевск'),(3943,3159,3921,'Промышленная'),(3944,3159,3921,'Тайга'),(3945,3159,3921,'Таштагол'),(3946,3159,3921,'Тисуль'),(3947,3159,3921,'Топки'),(3948,3159,3921,'Тяжинский'),(3949,3159,3921,'Юрга'),(3950,3159,3921,'Яшкино'),(3951,3159,3921,'Яя'),(3953,3159,3952,'Арбаж'),(3954,3159,3952,'Аркуль'),(3955,3159,3952,'Белая Холуница'),(3956,3159,3952,'Богородское'),(3957,3159,3952,'Боровой'),(3958,3159,3952,'Верхошижемье'),(3959,3159,3952,'Зуевка'),(3960,3159,3952,'Каринторф'),(3961,3159,3952,'Кикнур'),(3962,3159,3952,'Кильмезь'),(3963,3159,3952,'Киров'),(3964,3159,3952,'Кирово-Чепецк'),(3965,3159,3952,'Кирс'),(3966,3159,3952,'Кобра'),(3967,3159,3952,'Котельнич'),(3968,3159,3952,'Кумены'),(3969,3159,3952,'Ленинское'),(3970,3159,3952,'Луза'),(3971,3159,3952,'Малмыж'),(3972,3159,3952,'Мураши'),(3973,3159,3952,'Нагорск'),(3974,3159,3952,'Нема'),(3975,3159,3952,'Нововятск'),(3976,3159,3952,'Нолинск'),(3977,3159,3952,'Омутнинск'),(3978,3159,3952,'Опарино'),(3979,3159,3952,'Оричи'),(3980,3159,3952,'Пижанка'),(3981,3159,3952,'Подосиновец'),(3982,3159,3952,'Санчурск'),(3983,3159,3952,'Свеча'),(3984,3159,3952,'Слободской'),(3985,3159,3952,'Советск'),(3986,3159,3952,'Суна'),(3987,3159,3952,'Тужа'),(3988,3159,3952,'Уни'),(3989,3159,3952,'Уржум'),(3990,3159,3952,'Фаленки'),(3991,3159,3952,'Халтурин'),(3992,3159,3952,'Юрья'),(3993,3159,3952,'Яранск'),(3995,3159,3994,'Абезь'),(3996,3159,3994,'Айкино'),(3997,3159,3994,'Верхняя Инта'),(3998,3159,3994,'Визинга'),(3999,3159,3994,'Водный'),(4000,3159,3994,'Вожаель'),(4001,3159,3994,'Воркута'),(4002,3159,3994,'Вуктыл'),(4003,3159,3994,'Гешарт'),(4004,3159,3994,'Елецкий'),(4005,3159,3994,'Емва'),(4006,3159,3994,'Заполярный'),(4007,3159,3994,'Ижма'),(4008,3159,3994,'Инта'),(4009,3159,3994,'Ираель'),(4010,3159,3994,'Каджером'),(4011,3159,3994,'Кажым'),(4012,3159,3994,'Кожым'),(4013,3159,3994,'Койгородок'),(4014,3159,3994,'Корткерос'),(4015,3159,3994,'Кослан'),(4016,3159,3994,'Объячево'),(4017,3159,3994,'Печора'),(4018,3159,3994,'Сосногорск'),(4019,3159,3994,'Сыктывкар'),(4020,3159,3994,'Троицко-Печерск'),(4021,3159,3994,'Усинск'),(4022,3159,3994,'Усогорск'),(4023,3159,3994,'Усть-Кулом'),(4024,3159,3994,'Усть-Цильма'),(4025,3159,3994,'Ухта'),(4027,3159,4026,'Антропово'),(4028,3159,4026,'Боговарово'),(4029,3159,4026,'Буй'),(4030,3159,4026,'Волгореченск'),(4031,3159,4026,'Галич'),(4032,3159,4026,'Горчуха'),(4033,3159,4026,'Зебляки'),(4034,3159,4026,'Кадый'),(4035,3159,4026,'Кологрив'),(4036,3159,4026,'Кострома'),(4037,3159,4026,'Красное-на-Волге'),(4038,3159,4026,'Макарьев'),(4039,3159,4026,'Мантурово'),(4040,3159,4026,'Нерехта'),(4041,3159,4026,'Нея'),(4042,3159,4026,'Островское'),(4043,3159,4026,'Павино'),(4044,3159,4026,'Парфентьево'),(4045,3159,4026,'Поназырево'),(4046,3159,4026,'Солигалич'),(4047,3159,4026,'Судиславль'),(4048,3159,4026,'Сусанино'),(4049,3159,4026,'Чухлома'),(4050,3159,4026,'Шарья'),(4051,3159,4026,'Шемятино'),(4053,3159,4052,'Абинск'),(4054,3159,4052,'Абрау-Дюрсо'),(4055,3159,4052,'Анапа'),(4056,3159,4052,'Апшеронск'),(4057,3159,4052,'Армавир'),(4058,3159,4052,'Архипо-Осиповка'),(4059,3159,4052,'Афипский'),(4060,3159,4052,'Ахтырский'),(4061,3159,4052,'Ачуево'),(4062,3159,4052,'Белореченск'),(4063,3159,4052,'Верхнебаканский'),(4064,3159,4052,'Выселки'),(4065,3159,4052,'Геленджик'),(4066,3159,4052,'Гиагинская'),(4067,3159,4052,'Горячий Ключ'),(4068,3159,4052,'Джубга'),(4069,3159,4052,'Динская'),(4070,3159,4052,'Ейск'),(4071,3159,4052,'Ильский'),(4072,3159,4052,'Кабардинка'),(4073,3159,4052,'Калинино'),(4074,3159,4052,'Калининская'),(4075,3159,4052,'Каменномостский'),(4076,3159,4052,'Каневская'),(4077,3159,4052,'Кореновск'),(4078,3159,4052,'Красноармейская'),(4079,3159,4052,'Краснодар'),(4080,3159,4052,'Кропоткин'),(4081,3159,4052,'Крыловская'),(4082,3159,4052,'Крымск'),(4083,3159,4052,'Кущевская'),(4084,3159,4052,'Лабинск'),(4085,3159,4052,'Лениградская'),(4086,3159,4052,'Майкоп'),(4087,3159,4052,'Мостовской'),(4088,3159,4052,'Новороссийск'),(4089,3159,4052,'Отрадная'),(4090,3159,4052,'Павловская'),(4091,3159,4052,'Приморско-Ахтарск'),(4092,3159,4052,'Северская'),(4093,3159,4052,'Славянск-на-Кубани'),(4094,3159,4052,'Сочи'),(4095,3159,4052,'Староминская'),(4096,3159,4052,'Старощербиновская'),(4097,3159,4052,'Тбилисская'),(4098,3159,4052,'Темрюк'),(4099,3159,4052,'Тимашевск'),(4100,3159,4052,'Тихорецк'),(4101,3159,4052,'Туапсе'),(4102,3159,4052,'Тульский'),(4103,3159,4052,'Усть-Лабинск'),(4104,3159,4052,'Шовгеновский'),(4106,3159,4105,'Абаза'),(4107,3159,4105,'Абакан'),(4108,3159,4105,'Абан'),(4109,3159,4105,'Агинское'),(4110,3159,4105,'Артемовск'),(4111,3159,4105,'Аскиз'),(4112,3159,4105,'Ачинск'),(4113,3159,4105,'Байкит'),(4114,3159,4105,'Балахта'),(4115,3159,4105,'Балыкса'),(4116,3159,4105,'Белый Яр'),(4117,3159,4105,'Бельтырский'),(4118,3159,4105,'Бея'),(4119,3159,4105,'Бискамжа'),(4120,3159,4105,'Боготол'),(4121,3159,4105,'Боград'),(4122,3159,4105,'Богучаны'),(4123,3159,4105,'Большая Мурта'),(4124,3159,4105,'Большой Улуй'),(4125,3159,4105,'Бородино'),(4126,3159,4105,'Ванавара'),(4127,3159,4105,'Верхнеимбатск'),(4128,3159,4105,'Горячегорск'),(4129,3159,4105,'Дзержинское'),(4130,3159,4105,'Дивногорск'),(4131,3159,4105,'Диксон'),(4132,3159,4105,'Дудинка'),(4133,3159,4105,'Емельяново'),(4134,3159,4105,'Енисейск'),(4135,3159,4105,'Ермаковское'),(4136,3159,4105,'Заозерный'),(4137,3159,4105,'Игарка'),(4138,3159,4105,'Идринское'),(4139,3159,4105,'Иланский'),(4140,3159,4105,'Ирбейское'),(4141,3159,4105,'Казачинское'),(4142,3159,4105,'Канск'),(4143,3159,4105,'Каратузское'),(4144,3159,4105,'Караул'),(4145,3159,4105,'Кежма'),(4146,3159,4105,'Козулька'),(4147,3159,4105,'Копьево'),(4148,3159,4105,'Краснотуранск'),(4149,3159,4105,'Красноярск'),(4150,3159,4105,'Курагино'),(4151,3159,4105,'Лесосибирск'),(4152,3159,4105,'Минусинск'),(4153,3159,4105,'Мотыгино'),(4154,3159,4105,'Назарово'),(4155,3159,4105,'Нижний Ингаш'),(4156,3159,4105,'Новоселово'),(4157,3159,4105,'Норильск'),(4158,3159,4105,'Партизанское'),(4159,3159,4105,'Пировское'),(4160,3159,4105,'Саяногорск'),(4161,3159,4105,'Северо-Енисейский'),(4162,3159,4105,'Тасеево'),(4163,3159,4105,'Таштып'),(4164,3159,4105,'Тура'),(4165,3159,4105,'Туруханск'),(4166,3159,4105,'Тюхтет'),(4167,3159,4105,'Ужур'),(4168,3159,4105,'Усть-Авам'),(4169,3159,4105,'Уяр'),(4170,3159,4105,'Хатанга'),(4171,3159,4105,'Черемушки'),(4172,3159,4105,'Черногорск'),(4173,3159,4105,'Шалинское'),(4174,3159,4105,'Шира'),(4175,3159,4105,'Шушенское'),(4177,3159,4176,'Варгаши'),(4178,3159,4176,'Глядянское'),(4179,3159,4176,'Далматово'),(4180,3159,4176,'Каргаполье'),(4181,3159,4176,'Катайск'),(4182,3159,4176,'Кетово'),(4183,3159,4176,'Курган'),(4184,3159,4176,'Куртамыш'),(4185,3159,4176,'Лебяжье'),(4186,3159,4176,'Макушино'),(4187,3159,4176,'Мишкино'),(4188,3159,4176,'Мокроусово'),(4189,3159,4176,'Петухово'),(4190,3159,4176,'Половинное'),(4191,3159,4176,'Сафакулево'),(4192,3159,4176,'Целинное'),(4193,3159,4176,'Шадринск'),(4194,3159,4176,'Шатрово'),(4195,3159,4176,'Шумиха'),(4196,3159,4176,'Щучье'),(4197,3159,4176,'Юргамыш'),(4199,3159,4198,'Альменево'),(4200,3159,4198,'Белая'),(4201,3159,4198,'Большое Солдатское'),(4202,3159,4198,'Глушково'),(4203,3159,4198,'Горшечное'),(4204,3159,4198,'Дмитриев-Льговский'),(4205,3159,4198,'Железногорск'),(4206,3159,4198,'Золотухино'),(4207,3159,4198,'Касторное'),(4208,3159,4198,'Конышевка'),(4209,3159,4198,'Коренево'),(4210,3159,4198,'Курск'),(4211,3159,4198,'Курчатов'),(4212,3159,4198,'Кшенский'),(4213,3159,4198,'Льгов'),(4214,3159,4198,'Мантурово'),(4215,3159,4198,'Медвенка'),(4216,3159,4198,'Обоянь'),(4217,3159,4198,'Поныри'),(4218,3159,4198,'Пристень'),(4219,3159,4198,'Прямицыно'),(4220,3159,4198,'Рыльск'),(4221,3159,4198,'Суджа'),(4222,3159,4198,'Тим'),(4223,3159,4198,'Фатеж'),(4224,3159,4198,'Хомутовка'),(4225,3159,4198,'Черемисиново'),(4226,3159,4198,'Щигры'),(4228,3159,4227,'Грязи'),(4229,3159,4227,'Данхов'),(4230,3159,4227,'Доброе'),(4231,3159,4227,'Долгоруково'),(4232,3159,4227,'Елец'),(4233,3159,4227,'Задонск'),(4234,3159,4227,'Измалково'),(4235,3159,4227,'Казинка'),(4236,3159,4227,'Лебедянь'),(4237,3159,4227,'Лев Толстой'),(4238,3159,4227,'Липецк'),(4239,3159,4227,'Тербуны'),(4240,3159,4227,'Усмань'),(4241,3159,4227,'Хлевное'),(4242,3159,4227,'Чаплыгин'),(4244,3159,4243,'Анадырь'),(4245,3159,4243,'Атка'),(4246,3159,4243,'Балыгычан'),(4247,3159,4243,'Беринговский'),(4248,3159,4243,'Билибино'),(4249,3159,4243,'Большевик'),(4250,3159,4243,'Ванкарем'),(4251,3159,4243,'Иульитин'),(4252,3159,4243,'Кадыкчан'),(4253,3159,4243,'Лаврентия'),(4254,3159,4243,'Магадан'),(4255,3159,4243,'Мыс Шмидта'),(4256,3159,4243,'Ола'),(4257,3159,4243,'Омонск'),(4258,3159,4243,'Омсукчан'),(4259,3159,4243,'Палатка'),(4260,3159,4243,'Певек'),(4261,3159,4243,'Провидения'),(4262,3159,4243,'Сеймчан'),(4263,3159,4243,'Синегорье'),(4264,3159,4243,'Сусуман'),(4265,3159,4243,'Усть-Белая'),(4266,3159,4243,'Усть-Омчуг'),(4267,3159,4243,'Эвенск'),(4268,3159,4243,'Эгвекинот'),(4269,3159,4243,'Ягодное'),(4271,3159,4270,'Волжск'),(4272,3159,4270,'Дубовский'),(4273,3159,4270,'Звенигово'),(4274,3159,4270,'Йошкар-Ола'),(4275,3159,4270,'Килемары'),(4276,3159,4270,'Козьмодемьянск'),(4277,3159,4270,'Куженер'),(4278,3159,4270,'Мари-Турек'),(4279,3159,4270,'Медведево'),(4280,3159,4270,'Морки'),(4281,3159,4270,'Новый Торьял'),(4282,3159,4270,'Оршанка'),(4283,3159,4270,'Параньга'),(4284,3159,4270,'Сернур'),(4285,3159,4270,'Советский'),(4286,3159,4270,'Юрино'),(4288,3159,4287,'Ардатов'),(4289,3159,4287,'Атюрьево'),(4290,3159,4287,'Атяшево'),(4291,3159,4287,'Большие Березники'),(4292,3159,4287,'Большое Игнатово'),(4293,3159,4287,'Выша'),(4294,3159,4287,'Ельники'),(4295,3159,4287,'Зубова Поляна'),(4296,3159,4287,'Инсар'),(4297,3159,4287,'Кадошкино'),(4298,3159,4287,'Кемля'),(4299,3159,4287,'Ковылкино'),(4300,3159,4287,'Комсомольский'),(4301,3159,4287,'Кочкурово'),(4302,3159,4287,'Краснослободск'),(4303,3159,4287,'Лямбирь'),(4304,3159,4287,'Ромоданово'),(4305,3159,4287,'Рузаевка'),(4306,3159,4287,'Саранск'),(4307,3159,4287,'Старое Шайгово'),(4308,3159,4287,'Темников'),(4309,3159,4287,'Теньгушево'),(4310,3159,4287,'Торбеево'),(4311,3159,4287,'Чамзинка'),(4313,3159,4312,'Абрамцево'),(4314,3159,4312,'Алабино'),(4315,3159,4312,'Апрелевка'),(4316,3159,4312,'Архангельское'),(4317,3159,4312,'Ашитково'),(4318,3159,4312,'Бакшеево'),(4319,3159,4312,'Балашиха'),(4320,3159,4312,'Барыбино'),(4321,3159,4312,'Белоомут'),(4322,3159,4312,'Белые Столбы'),(4323,3159,4312,'Бородино'),(4324,3159,4312,'Бронницы'),(4325,3159,4312,'Быково'),(4326,3159,4312,'Валуево'),(4327,3159,4312,'Вербилки'),(4328,3159,4312,'Верея'),(4329,3159,4312,'Видное'),(4330,3159,4312,'Внуково'),(4331,3159,4312,'Вождь Пролетариата'),(4332,3159,4312,'Волоколамск'),(4333,3159,4312,'Вороново'),(4334,3159,4312,'Воскресенск'),(4335,3159,4312,'Восточный'),(4336,3159,4312,'Востряково'),(4337,3159,4312,'Высоковск'),(4338,3159,4312,'Голицино'),(4339,3159,4312,'Деденево'),(4340,3159,4312,'Дедовск'),(4341,3159,4312,'Джержинский'),(4342,3159,4312,'Дмитров'),(4343,3159,4312,'Долгопрудный'),(4344,3159,4312,'Домодедово'),(4345,3159,4312,'Дорохово'),(4346,3159,4312,'Дрезна'),(4347,3159,4312,'Дубки'),(4348,3159,4312,'Дубна'),(4349,3159,4312,'Егорьевск'),(4350,3159,4312,'Железнодорожный'),(4351,3159,4312,'Жилево'),(4352,3159,4312,'Жуковский'),(4353,3159,4312,'Загорск'),(4354,3159,4312,'Загорянский'),(4355,3159,4312,'Запрудная'),(4356,3159,4312,'Зарайск'),(4357,3159,4312,'Звенигород'),(4358,3159,4312,'Зеленоград'),(4359,3159,4312,'Ивантеевка'),(4360,3159,4312,'Икша'),(4361,3159,4312,'Ильинский'),(4362,3159,4312,'Истра'),(4363,3159,4312,'Калининград'),(4364,3159,4312,'Кашира'),(4365,3159,4312,'Керва'),(4366,3159,4312,'Климовск'),(4367,3159,4312,'Клин'),(4368,3159,4312,'Клязьма'),(4369,3159,4312,'Кожино'),(4370,3159,4312,'Кокошкино'),(4371,3159,4312,'Коломна'),(4372,3159,4312,'Колюбакино'),(4373,3159,4312,'Косино'),(4374,3159,4312,'Котельники'),(4375,3159,4312,'Красково'),(4376,3159,4312,'Красноармейск'),(4377,3159,4312,'Красногорск'),(4378,3159,4312,'Краснозаводск'),(4379,3159,4312,'Красный Ткач'),(4380,3159,4312,'Крюково'),(4381,3159,4312,'Кубинка'),(4382,3159,4312,'Купавна'),(4383,3159,4312,'Куровское'),(4384,3159,4312,'Лесной Городок'),(4385,3159,4312,'Ликино-Дулево'),(4386,3159,4312,'Лобня'),(4387,3159,4312,'Лопатинский'),(4388,3159,4312,'Лосино-Петровский'),(4389,3159,4312,'Лотошино'),(4390,3159,4312,'Лукино'),(4391,3159,4312,'Луховицы'),(4392,3159,4312,'Лыткарино'),(4393,3159,4312,'Львовский'),(4394,3159,4312,'Люберцы'),(4395,3159,4312,'Малаховка'),(4396,3159,4312,'Михайловское'),(4397,3159,4312,'Михнево'),(4398,3159,4312,'Можайск'),(4399,3159,4312,'Монино'),(4400,3159,4312,'Москва'),(4401,3159,4312,'Муханово'),(4402,3159,4312,'Мытищи'),(4403,3159,4312,'Нарофоминск'),(4404,3159,4312,'Нахабино'),(4405,3159,4312,'Некрасовка'),(4406,3159,4312,'Немчиновка'),(4407,3159,4312,'Новобратцевский'),(4408,3159,4312,'Новоподрезково'),(4409,3159,4312,'Ногинск'),(4410,3159,4312,'Обухово'),(4411,3159,4312,'Одинцово'),(4412,3159,4312,'Ожерелье'),(4413,3159,4312,'Озеры'),(4414,3159,4312,'Октябрьский'),(4415,3159,4312,'Опалиха'),(4416,3159,4312,'Орехово-Зуево'),(4417,3159,4312,'Павловский Посад'),(4418,3159,4312,'Первомайский'),(4419,3159,4312,'Пески'),(4420,3159,4312,'Пироговский'),(4421,3159,4312,'Подольск'),(4422,3159,4312,'Полушкино'),(4423,3159,4312,'Правдинский'),(4424,3159,4312,'Привокзальный'),(4425,3159,4312,'Пролетарский'),(4426,3159,4312,'Пушкино'),(4427,3159,4312,'Пущино'),(4428,3159,4312,'Радовицкий'),(4429,3159,4312,'Раменское'),(4430,3159,4312,'Реутов'),(4431,3159,4312,'Решетниково'),(4432,3159,4312,'Родники'),(4433,3159,4312,'Рошаль'),(4434,3159,4312,'Рублево'),(4435,3159,4312,'Руза'),(4436,3159,4312,'Салтыковка'),(4437,3159,4312,'Северный'),(4438,3159,4312,'Сергиев Посад'),(4439,3159,4312,'Серебряные Пруды'),(4440,3159,4312,'Серпухов'),(4441,3159,4312,'Солнечногорск'),(4442,3159,4312,'Солнцево'),(4443,3159,4312,'Софрино'),(4444,3159,4312,'Старая Купавна'),(4445,3159,4312,'Старбеево'),(4446,3159,4312,'Ступино'),(4447,3159,4312,'Сходня'),(4448,3159,4312,'Талдом'),(4449,3159,4312,'Текстильщик'),(4450,3159,4312,'Темпы'),(4451,3159,4312,'Тишково'),(4452,3159,4312,'Томилино'),(4453,3159,4312,'Троицк'),(4454,3159,4312,'Туголесский Бор'),(4455,3159,4312,'Тучково'),(4456,3159,4312,'Уваровка'),(4457,3159,4312,'Удельная'),(4458,3159,4312,'Успенское'),(4459,3159,4312,'Фирсановка'),(4460,3159,4312,'Фосфоритный'),(4461,3159,4312,'Фрязино'),(4462,3159,4312,'Фряново'),(4463,3159,4312,'Химки'),(4464,3159,4312,'Хорлово'),(4465,3159,4312,'Хотьково'),(4466,3159,4312,'Черкизово'),(4467,3159,4312,'Черноголовка'),(4468,3159,4312,'Черусти'),(4469,3159,4312,'Чехов'),(4470,3159,4312,'Шарапово'),(4471,3159,4312,'Шатура'),(4472,3159,4312,'Шатурторф'),(4473,3159,4312,'Шаховская'),(4474,3159,4312,'Шереметьевский'),(4475,3159,4312,'Щелково'),(4476,3159,4312,'Щербинка'),(4477,3159,4312,'Электрогорск'),(4478,3159,4312,'Электросталь'),(4479,3159,4312,'Электроугли'),(4480,3159,4312,'Яхрома'),(4482,3159,4481,'Апатиты'),(4483,3159,4481,'Африканда'),(4484,3159,4481,'Верхнетуломский'),(4485,3159,4481,'Заполярный'),(4486,3159,4481,'Зареченск'),(4487,3159,4481,'Зашеек'),(4488,3159,4481,'Зеленоборский'),(4489,3159,4481,'Кандалакша'),(4490,3159,4481,'Кильдинстрой'),(4491,3159,4481,'Кировск'),(4492,3159,4481,'Ковдор'),(4493,3159,4481,'Кола'),(4494,3159,4481,'Конда'),(4495,3159,4481,'Мончегорск'),(4496,3159,4481,'Мурманск'),(4497,3159,4481,'Мурмаши'),(4498,3159,4481,'Никель'),(4499,3159,4481,'Оленегорск'),(4500,3159,4481,'Полярный'),(4501,3159,4481,'Североморск'),(4502,3159,4481,'Умба'),(4504,3159,4503,'Анциферово'),(4505,3159,4503,'Батецкий'),(4506,3159,4503,'Большая Вишера'),(4507,3159,4503,'Боровичи'),(4508,3159,4503,'Валдай'),(4509,3159,4503,'Волот'),(4510,3159,4503,'Деманск'),(4511,3159,4503,'Зарубино'),(4512,3159,4503,'Кресцы'),(4513,3159,4503,'Любытино'),(4514,3159,4503,'Малая Вишера'),(4515,3159,4503,'Марево'),(4516,3159,4503,'Мошенское'),(4517,3159,4503,'Новгород'),(4518,3159,4503,'Окуловка'),(4519,3159,4503,'Парфино'),(4520,3159,4503,'Пестово'),(4521,3159,4503,'Поддорье'),(4522,3159,4503,'Сольцы'),(4523,3159,4503,'Старая Русса'),(4524,3159,4503,'Хвойное'),(4525,3159,4503,'Холм'),(4526,3159,4503,'Чудово'),(4527,3159,4503,'Шимск'),(4529,3159,4528,'Баган'),(4530,3159,4528,'Барабинск'),(4531,3159,4528,'Бердск'),(4532,3159,4528,'Биаза'),(4533,3159,4528,'Болотное'),(4534,3159,4528,'Венгерово'),(4535,3159,4528,'Довольное'),(4536,3159,4528,'Завьялово'),(4537,3159,4528,'Искитим'),(4538,3159,4528,'Карасук'),(4539,3159,4528,'Каргат'),(4540,3159,4528,'Колывань'),(4541,3159,4528,'Краснозерское'),(4542,3159,4528,'Крутиха'),(4543,3159,4528,'Куйбышев'),(4544,3159,4528,'Купино'),(4545,3159,4528,'Кыштовка'),(4546,3159,4528,'Маслянино'),(4547,3159,4528,'Михайловский'),(4548,3159,4528,'Мошково'),(4549,3159,4528,'Новосибирск'),(4550,3159,4528,'Ордынское'),(4551,3159,4528,'Северное'),(4552,3159,4528,'Сузун'),(4553,3159,4528,'Татарск'),(4554,3159,4528,'Тогучин'),(4555,3159,4528,'Убинское'),(4556,3159,4528,'Усть-Тарка'),(4557,3159,4528,'Чаны'),(4558,3159,4528,'Черепаново'),(4559,3159,4528,'Чистоозерное'),(4560,3159,4528,'Чулым'),(4562,3159,4561,'Береговой'),(4563,3159,4561,'Большеречье'),(4564,3159,4561,'Большие Уки'),(4565,3159,4561,'Горьковское'),(4566,3159,4561,'Знаменское'),(4567,3159,4561,'Исилькуль'),(4568,3159,4561,'Калачинск'),(4569,3159,4561,'Колосовка'),(4570,3159,4561,'Кормиловка'),(4571,3159,4561,'Крутинка'),(4572,3159,4561,'Любинский'),(4573,3159,4561,'Марьяновка'),(4574,3159,4561,'Муромцево'),(4575,3159,4561,'Называевск'),(4576,3159,4561,'Нижняя Омка'),(4577,3159,4561,'Нововаршавка'),(4578,3159,4561,'Одесское'),(4579,3159,4561,'Оконешниково'),(4580,3159,4561,'Омск'),(4581,3159,4561,'Павлоградка'),(4582,3159,4561,'Полтавка'),(4583,3159,4561,'Русская Поляна'),(4584,3159,4561,'Саргатское'),(4585,3159,4561,'Седельниково'),(4586,3159,4561,'Таврическое'),(4587,3159,4561,'Тара'),(4588,3159,4561,'Тевриз'),(4589,3159,4561,'Тюкалинск'),(4590,3159,4561,'Усть-Ишим'),(4591,3159,4561,'Черлак'),(4592,3159,4561,'Шербакуль'),(4594,3159,4593,'Абдулино'),(4595,3159,4593,'Адамовка'),(4596,3159,4593,'Айдырлинский'),(4597,3159,4593,'Акбулак'),(4598,3159,4593,'Аккермановка'),(4599,3159,4593,'Асекеево'),(4600,3159,4593,'Беляевка'),(4601,3159,4593,'Бугуруслан'),(4602,3159,4593,'Бузулук'),(4603,3159,4593,'Гай'),(4604,3159,4593,'Грачевка'),(4605,3159,4593,'Домбаровский'),(4606,3159,4593,'Дубенский'),(4607,3159,4593,'Илек'),(4608,3159,4593,'Ириклинский'),(4609,3159,4593,'Кувандык'),(4610,3159,4593,'Курманаевка'),(4611,3159,4593,'Матвеевка'),(4612,3159,4593,'Медногорск'),(4613,3159,4593,'Новоорск'),(4614,3159,4593,'Новосергиевка'),(4615,3159,4593,'Новотроицк'),(4616,3159,4593,'Октябрьское'),(4617,3159,4593,'Оренбург'),(4618,3159,4593,'Орск'),(4619,3159,4593,'Первомайский'),(4620,3159,4593,'Переволоцкий'),(4621,3159,4593,'Пономаревка'),(4622,3159,4593,'Саракташ'),(4623,3159,4593,'Светлый'),(4624,3159,4593,'Северное'),(4625,3159,4593,'Соль-Илецк'),(4626,3159,4593,'Сорочинск'),(4627,3159,4593,'Ташла'),(4628,3159,4593,'Тоцкое'),(4629,3159,4593,'Тюльган'),(4630,3159,4593,'Шарлык'),(4631,3159,4593,'Энергетик'),(4632,3159,4593,'Ясный'),(4634,3159,4633,'Болхов'),(4635,3159,4633,'Верховье'),(4636,3159,4633,'Глазуновка'),(4637,3159,4633,'Дмитровск-Орловский'),(4638,3159,4633,'Долгое'),(4639,3159,4633,'Залегощь'),(4640,3159,4633,'Змиевка'),(4641,3159,4633,'Знаменское'),(4642,3159,4633,'Колпны'),(4643,3159,4633,'Красная Заря'),(4644,3159,4633,'Кромы'),(4645,3159,4633,'Ливны'),(4646,3159,4633,'Малоархангельск'),(4647,3159,4633,'Мценск'),(4648,3159,4633,'Нарышкино'),(4649,3159,4633,'Новосиль'),(4650,3159,4633,'Орел'),(4651,3159,4633,'Покровское'),(4652,3159,4633,'Сосково'),(4653,3159,4633,'Тросна'),(4654,3159,4633,'Хомутово'),(4655,3159,4633,'Хотынец'),(4656,3159,4633,'Шаблыкино'),(4658,3159,4657,'Башмаково'),(4659,3159,4657,'Беднодемьяновск'),(4660,3159,4657,'Беково'),(4661,3159,4657,'Белинский'),(4662,3159,4657,'Бессоновка'),(4663,3159,4657,'Вадинск'),(4664,3159,4657,'Верхозим'),(4665,3159,4657,'Городище'),(4666,3159,4657,'Евлашево'),(4667,3159,4657,'Земетчино'),(4668,3159,4657,'Золотаревка'),(4669,3159,4657,'Исса'),(4670,3159,4657,'Каменка'),(4671,3159,4657,'Колышлей'),(4672,3159,4657,'Кондоль'),(4673,3159,4657,'Кузнецк'),(4674,3159,4657,'Лопатино'),(4675,3159,4657,'Малая Сердоба'),(4676,3159,4657,'Мокшан'),(4677,3159,4657,'Наровчат'),(4678,3159,4657,'Неверкино'),(4679,3159,4657,'Нижний Ломов'),(4680,3159,4657,'Никольск'),(4681,3159,4657,'Пачелма'),(4682,3159,4657,'Пенза'),(4683,3159,4657,'Русский Камешкир'),(4684,3159,4657,'Сердобск'),(4685,3159,4657,'Сосновоборск'),(4686,3159,4657,'Сура'),(4687,3159,4657,'Тамала'),(4688,3159,4657,'Шемышейка'),(4690,3159,4689,'Барда'),(4691,3159,4689,'Березники'),(4692,3159,4689,'Большая Соснова'),(4693,3159,4689,'Верещагино'),(4694,3159,4689,'Гайны'),(4695,3159,4689,'Горнозаводск'),(4696,3159,4689,'Гремячинск'),(4697,3159,4689,'Губаха'),(4698,3159,4689,'Добрянка'),(4699,3159,4689,'Елово'),(4700,3159,4689,'Зюкайка'),(4701,3159,4689,'Ильинский'),(4702,3159,4689,'Карагай'),(4703,3159,4689,'Керчевский'),(4704,3159,4689,'Кизел'),(4705,3159,4689,'Коса'),(4706,3159,4689,'Кочево'),(4707,3159,4689,'Красновишерск'),(4708,3159,4689,'Краснокамск'),(4709,3159,4689,'Кудымкар'),(4710,3159,4689,'Куеда'),(4711,3159,4689,'Кунгур'),(4712,3159,4689,'Лысьва'),(4713,3159,4689,'Ныроб'),(4714,3159,4689,'Нытва'),(4715,3159,4689,'Октябрьский'),(4716,3159,4689,'Орда'),(4717,3159,4689,'Оса'),(4718,3159,4689,'Оханск'),(4719,3159,4689,'Очер'),(4720,3159,4689,'Пермь'),(4721,3159,4689,'Соликамск'),(4722,3159,4689,'Суксун'),(4723,3159,4689,'Уинское'),(4724,3159,4689,'Усолье'),(4725,3159,4689,'Усть-Кишерть'),(4726,3159,4689,'Чайковский'),(4727,3159,4689,'Частые'),(4728,3159,4689,'Чердынь'),(4729,3159,4689,'Чернореченский'),(4730,3159,4689,'Чернушка'),(4731,3159,4689,'Чусовой'),(4732,3159,4689,'Юрла'),(4733,3159,4689,'Юсьва'),(4735,3159,4734,'Анучино'),(4736,3159,4734,'Арсеньев'),(4737,3159,4734,'Артем'),(4738,3159,4734,'Артемовский'),(4739,3159,4734,'Большой Камень'),(4740,3159,4734,'Валентин'),(4741,3159,4734,'Владивосток'),(4742,3159,4734,'Высокогорск'),(4743,3159,4734,'Горные Ключи'),(4744,3159,4734,'Горный'),(4745,3159,4734,'Дальнегорск'),(4746,3159,4734,'Дальнереченск'),(4747,3159,4734,'Зарубино'),(4748,3159,4734,'Кавалерово'),(4749,3159,4734,'Каменка'),(4750,3159,4734,'Камень-Рыболов'),(4751,3159,4734,'Кировский'),(4752,3159,4734,'Лазо'),(4753,3159,4734,'Лесозаводск'),(4754,3159,4734,'Лучегорск'),(4755,3159,4734,'Михайловка'),(4756,3159,4734,'Находка'),(4757,3159,4734,'Новопокровка'),(4758,3159,4734,'Ольга'),(4759,3159,4734,'Партизанск'),(4760,3159,4734,'Пограничный'),(4761,3159,4734,'Покровка'),(4762,3159,4734,'Русский'),(4763,3159,4734,'Самарга'),(4764,3159,4734,'Славянка'),(4765,3159,4734,'Спасск-Дальний'),(4766,3159,4734,'Терней'),(4767,3159,4734,'Уссурийск'),(4768,3159,4734,'Хасан'),(4769,3159,4734,'Хороль'),(4770,3159,4734,'Черниговка'),(4771,3159,4734,'Чугуевка'),(4772,3159,4734,'Яковлевка'),(4774,3159,4773,'Бежаницы'),(4775,3159,4773,'Великие Луки'),(4776,3159,4773,'Гдов'),(4777,3159,4773,'Дедовичи'),(4778,3159,4773,'Дно'),(4779,3159,4773,'Заплюсье'),(4780,3159,4773,'Идрица'),(4781,3159,4773,'Красногородское'),(4782,3159,4773,'Кунья'),(4783,3159,4773,'Локня'),(4784,3159,4773,'Невель'),(4785,3159,4773,'Новоржев'),(4786,3159,4773,'Новосокольники'),(4787,3159,4773,'Опочка'),(4788,3159,4773,'Остров'),(4789,3159,4773,'Палкино'),(4790,3159,4773,'Печоры'),(4791,3159,4773,'Плюсса'),(4792,3159,4773,'Порхов'),(4793,3159,4773,'Псков'),(4794,3159,4773,'Пустошка'),(4795,3159,4773,'Пушкинские Горы'),(4796,3159,4773,'Пыталово'),(4797,3159,4773,'Себеж'),(4798,3159,4773,'Струги-Красные'),(4799,3159,4773,'Усвяты'),(4801,3159,4800,'Азов'),(4802,3159,4800,'Аксай'),(4803,3159,4800,'Алмазный'),(4804,3159,4800,'Аютинск'),(4805,3159,4800,'Багаевский'),(4806,3159,4800,'Батайск'),(4807,3159,4800,'Белая Калитва'),(4808,3159,4800,'Боковская'),(4809,3159,4800,'Большая Мартыновка'),(4810,3159,4800,'Вешенская'),(4811,3159,4800,'Волгодонск'),(4812,3159,4800,'Восход'),(4813,3159,4800,'Гигант'),(4814,3159,4800,'Горняцкий'),(4815,3159,4800,'Гуково'),(4816,3159,4800,'Донецк'),(4817,3159,4800,'Донской'),(4818,3159,4800,'Дубовское'),(4819,3159,4800,'Жирнов'),(4820,3159,4800,'Заветное'),(4821,3159,4800,'Заводской'),(4822,3159,4800,'Зверево'),(4823,3159,4800,'Зерноград'),(4824,3159,4800,'Зимовники'),(4825,3159,4800,'Кагальницкая'),(4826,3159,4800,'Казанская'),(4827,3159,4800,'Каменоломни'),(4828,3159,4800,'Каменск-Шахтинский'),(4829,3159,4800,'Кашары'),(4830,3159,4800,'Коксовый'),(4831,3159,4800,'Константиновск'),(4832,3159,4800,'Красный Сулин'),(4833,3159,4800,'Куйбышево'),(4834,3159,4800,'Матвеев Курган'),(4835,3159,4800,'Мигулинская'),(4836,3159,4800,'Миллерово'),(4837,3159,4800,'Милютинская'),(4838,3159,4800,'Морозовск'),(4839,3159,4800,'Новочеркасск'),(4840,3159,4800,'Новошахтинск'),(4841,3159,4800,'Обливская'),(4842,3159,4800,'Орловский'),(4843,3159,4800,'Песчанокопское'),(4844,3159,4800,'Покровское'),(4845,3159,4800,'Пролетарск'),(4846,3159,4800,'Ремонтное'),(4847,3159,4800,'Родионово-Несветайская'),(4848,3159,4800,'Ростов-на-Дону'),(4849,3159,4800,'Сальск'),(4850,3159,4800,'Семикаракорск'),(4851,3159,4800,'Таганрог'),(4852,3159,4800,'Тарасовский'),(4853,3159,4800,'Тацинский'),(4854,3159,4800,'Усть-Донецкий'),(4855,3159,4800,'Целина'),(4856,3159,4800,'Цимлянск'),(4857,3159,4800,'Чалтырь'),(4858,3159,4800,'Чертково'),(4859,3159,4800,'Шахты'),(4860,3159,4800,'Шолоховский'),(4862,3159,4861,'Александро-Невский'),(4863,3159,4861,'Горняк'),(4864,3159,4861,'Гусь Железный'),(4865,3159,4861,'Елатьма'),(4866,3159,4861,'Ермишь'),(4867,3159,4861,'Заречный'),(4868,3159,4861,'Захарово'),(4869,3159,4861,'Кадом'),(4870,3159,4861,'Касимов'),(4871,3159,4861,'Кораблино'),(4872,3159,4861,'Милославское'),(4873,3159,4861,'Михайлов'),(4874,3159,4861,'Пителино'),(4875,3159,4861,'Пронск'),(4876,3159,4861,'Путятино'),(4877,3159,4861,'Рыбное'),(4878,3159,4861,'Ряжск'),(4879,3159,4861,'Рязань'),(4880,3159,4861,'Сапожок'),(4881,3159,4861,'Сараи'),(4882,3159,4861,'Сасово'),(4883,3159,4861,'Скопин'),(4884,3159,4861,'Спас-Клепики'),(4885,3159,4861,'Спасск-Рязанский'),(4886,3159,4861,'Старожилово'),(4887,3159,4861,'Ухолово'),(4888,3159,4861,'Чучково'),(4889,3159,4861,'Шацк'),(4890,3159,4861,'Шилово'),(4892,3159,4891,'Алексеевка'),(4893,3159,4891,'Безенчук'),(4894,3159,4891,'Богатое'),(4895,3159,4891,'Богатырь'),(4896,3159,4891,'Большая Глущица'),(4897,3159,4891,'Большая Черниговка'),(4898,3159,4891,'Борское'),(4899,3159,4891,'Волжский'),(4900,3159,4891,'Жигулевск'),(4901,3159,4891,'Зольное'),(4902,3159,4891,'Исаклы'),(4903,3159,4891,'Камышла'),(4904,3159,4891,'Кинель'),(4905,3159,4891,'Кинель-Черкасы'),(4906,3159,4891,'Клявлино'),(4907,3159,4891,'Кошки'),(4908,3159,4891,'Красноармейское'),(4909,3159,4891,'Куйбышев'),(4910,3159,4891,'Нефтегорск'),(4911,3159,4891,'Новокуйбышевск'),(4912,3159,4891,'Октябрьск'),(4913,3159,4891,'Отрадный'),(4914,3159,4891,'Пестравка'),(4915,3159,4891,'Похвистнево'),(4916,3159,4891,'Приволжье'),(4917,3159,4891,'Самара'),(4918,3159,4891,'Сызрань'),(4919,3159,4891,'Тольятти'),(4920,3159,4891,'Хворостянка'),(4921,3159,4891,'Чапаевск'),(4922,3159,4891,'Челно-Вершины'),(4923,3159,4891,'Шентала'),(4924,3159,4891,'Шигоны'),(4926,3159,4925,'Александровская'),(4927,3159,4925,'Бокситогорск'),(4928,3159,4925,'Большая Ижора'),(4929,3159,4925,'Будогощь'),(4930,3159,4925,'Вознесенье'),(4931,3159,4925,'Волосово'),(4932,3159,4925,'Волхов'),(4933,3159,4925,'Всеволожск'),(4934,3159,4925,'Выборг'),(4935,3159,4925,'Вырица'),(4936,3159,4925,'Высоцк'),(4937,3159,4925,'Гатчина'),(4938,3159,4925,'Дружная Горка'),(4939,3159,4925,'Дубровка'),(4940,3159,4925,'Ефимовский'),(4941,3159,4925,'Зеленогорск'),(4942,3159,4925,'Ивангород'),(4943,3159,4925,'Каменногорск'),(4944,3159,4925,'Кикерино'),(4945,3159,4925,'Кингисепп'),(4946,3159,4925,'Кириши'),(4947,3159,4925,'Кировск'),(4948,3159,4925,'Кобринское'),(4949,3159,4925,'Колпино'),(4950,3159,4925,'Кронштадт'),(4952,3159,4925,'Лисий Нос'),(4953,3159,4925,'Лодейное Поле'),(4954,3159,4925,'Ломоносов'),(4955,3159,4925,'Луга'),(4956,3159,4925,'Павловск'),(4957,3159,4925,'Парголово'),(4958,3159,4925,'Петродворец'),(4959,3159,4925,'Подпорожье'),(4960,3159,4925,'Приозерск'),(4961,3159,4925,'Пушкин'),(4962,3159,4925,'Санкт-Петербург'),(4964,3159,4925,'Сестрорецк'),(4965,3159,4925,'Сланцы'),(4966,3159,4925,'Сосновый Бор'),(4967,3159,4925,'Тихвин'),(4968,3159,4925,'Тосно'),(4970,3159,4969,'Александров Гай'),(4971,3159,4969,'Аркадак'),(4972,3159,4969,'Аткарск'),(4973,3159,4969,'Базарный Карабулак'),(4974,3159,4969,'Балаково'),(4975,3159,4969,'Балашов'),(4976,3159,4969,'Балтай'),(4977,3159,4969,'Возрождение'),(4978,3159,4969,'Вольск'),(4979,3159,4969,'Воскресенское'),(4980,3159,4969,'Горный'),(4981,3159,4969,'Дергачи'),(4982,3159,4969,'Духовницкое'),(4983,3159,4969,'Екатериновка'),(4984,3159,4969,'Ершов'),(4985,3159,4969,'Заречный'),(4986,3159,4969,'Ивантеевка'),(4987,3159,4969,'Калининск'),(4988,3159,4969,'Каменский'),(4989,3159,4969,'Красноармейск'),(4990,3159,4969,'Красный Кут'),(4991,3159,4969,'Лысые Горы'),(4992,3159,4969,'Маркс'),(4993,3159,4969,'Мокроус'),(4994,3159,4969,'Новоузенск'),(4995,3159,4969,'Новые Бурасы'),(4996,3159,4969,'Озинки'),(4997,3159,4969,'Перелюб'),(4998,3159,4969,'Петровск'),(4999,3159,4969,'Питерка'),(5000,3159,4969,'Пугачев'),(5001,3159,4969,'Ровное'),(5002,3159,4969,'Романовка'),(5003,3159,4969,'Ртищево'),(5004,3159,4969,'Самойловка'),(5005,3159,4969,'Саратов'),(5006,3159,4969,'Степное'),(5007,3159,4969,'Татищево'),(5008,3159,4969,'Турки'),(5009,3159,4969,'Хвалынск'),(5010,3159,4969,'Энгельс'),(5012,3159,5011,'Абый'),(5013,3159,5011,'Алдан'),(5014,3159,5011,'Амга'),(5015,3159,5011,'Батагай'),(5016,3159,5011,'Бердигестях'),(5017,3159,5011,'Беркакит'),(5018,3159,5011,'Бестях'),(5019,3159,5011,'Борогонцы'),(5020,3159,5011,'Верхневилюйск'),(5021,3159,5011,'Верхнеколымск'),(5022,3159,5011,'Верхоянск'),(5023,3159,5011,'Вилюйск'),(5024,3159,5011,'Витим'),(5025,3159,5011,'Власово'),(5026,3159,5011,'Жиганск'),(5027,3159,5011,'Зырянка'),(5028,3159,5011,'Кангалассы'),(5029,3159,5011,'Канкунский'),(5030,3159,5011,'Ленск'),(5031,3159,5011,'Майя'),(5032,3159,5011,'Менкеря'),(5033,3159,5011,'Мирный'),(5034,3159,5011,'Нерюнгри'),(5035,3159,5011,'Нычалах'),(5036,3159,5011,'Нюрба'),(5037,3159,5011,'Олекминск'),(5038,3159,5011,'Покровск'),(5039,3159,5011,'Сангар'),(5040,3159,5011,'Саскылах'),(5041,3159,5011,'Среднеколымск'),(5042,3159,5011,'Сунтар'),(5043,3159,5011,'Тикси'),(5044,3159,5011,'Усть-Мая'),(5045,3159,5011,'Усть-Нера'),(5046,3159,5011,'Хандыга'),(5047,3159,5011,'Хонуу'),(5048,3159,5011,'Черский'),(5049,3159,5011,'Чокурдах'),(5050,3159,5011,'Чурапча'),(5051,3159,5011,'Якутск'),(5053,3159,5052,'Александровск-Сахалинский'),(5054,3159,5052,'Анбэцу'),(5055,3159,5052,'Анива'),(5056,3159,5052,'Бошняково'),(5057,3159,5052,'Быков'),(5058,3159,5052,'Вахрушев'),(5059,3159,5052,'Взморье'),(5060,3159,5052,'Гастелло'),(5061,3159,5052,'Горнозаводск'),(5062,3159,5052,'Долинск'),(5063,3159,5052,'Ильинский'),(5064,3159,5052,'Катангли'),(5065,3159,5052,'Корсаков'),(5066,3159,5052,'Курильск'),(5067,3159,5052,'Макаров'),(5068,3159,5052,'Невельск'),(5069,3159,5052,'Ноглики'),(5070,3159,5052,'Оха'),(5071,3159,5052,'Поронайск'),(5072,3159,5052,'Северо-Курильск'),(5073,3159,5052,'Смирных'),(5074,3159,5052,'Томари'),(5075,3159,5052,'Тымовское'),(5076,3159,5052,'Углегорск'),(5077,3159,5052,'Холмск'),(5078,3159,5052,'Шахтерск'),(5079,3159,5052,'Южно-Сахалинск'),(5081,3159,5080,'Алапаевск'),(5082,3159,5080,'Алтынай'),(5083,3159,5080,'Арамиль'),(5084,3159,5080,'Артемовский'),(5085,3159,5080,'Арти'),(5086,3159,5080,'Асбест'),(5087,3159,5080,'Ачит'),(5088,3159,5080,'Байкалово'),(5089,3159,5080,'Басьяновский'),(5090,3159,5080,'Белоярский'),(5091,3159,5080,'Березовский'),(5092,3159,5080,'Богданович'),(5093,3159,5080,'Буланаш'),(5094,3159,5080,'Верхний Тагил'),(5095,3159,5080,'Верхняя Пышма'),(5096,3159,5080,'Верхняя Салда'),(5097,3159,5080,'Верхняя Синячиха'),(5098,3159,5080,'Верхняя Сысерть'),(5099,3159,5080,'Верхняя Тура'),(5100,3159,5080,'Верхотурье'),(5101,3159,5080,'Висим'),(5102,3159,5080,'Волчанск'),(5103,3159,5080,'Воронцовка'),(5104,3159,5080,'Гари'),(5105,3159,5080,'Дегтярск'),(5106,3159,5080,'Екатеринбург'),(5107,3159,5080,'Ертарский'),(5108,3159,5080,'Заводоуспенское'),(5109,3159,5080,'Зыряновский'),(5110,3159,5080,'Зюзельский'),(5111,3159,5080,'Ивдель'),(5112,3159,5080,'Изумруд'),(5113,3159,5080,'Ирбит'),(5114,3159,5080,'Ис'),(5115,3159,5080,'Каменск-Уральский'),(5116,3159,5080,'Камышлов'),(5117,3159,5080,'Карпинск'),(5118,3159,5080,'Карпунинский'),(5119,3159,5080,'Качканар'),(5120,3159,5080,'Кировград'),(5121,3159,5080,'Краснотурьинск'),(5122,3159,5080,'Красноуральск'),(5123,3159,5080,'Красноуфимск'),(5124,3159,5080,'Кушва'),(5125,3159,5080,'Михайловск'),(5126,3159,5080,'Невьянск'),(5127,3159,5080,'Нижние Серги'),(5128,3159,5080,'Нижний Тагил'),(5129,3159,5080,'Нижняя Салда'),(5130,3159,5080,'Нижняя Тура'),(5131,3159,5080,'Новая Ляля'),(5132,3159,5080,'Оус'),(5133,3159,5080,'Первоуральск'),(5134,3159,5080,'Полевской'),(5135,3159,5080,'Пышма'),(5136,3159,5080,'Ревда'),(5137,3159,5080,'Реж'),(5138,3159,5080,'Свердловск'),(5139,3159,5080,'Североуральск'),(5140,3159,5080,'Серов'),(5141,3159,5080,'Сосьва'),(5142,3159,5080,'Среднеуральск'),(5143,3159,5080,'Сухой Лог'),(5144,3159,5080,'Сысерть'),(5145,3159,5080,'Таборы'),(5146,3159,5080,'Тавда'),(5147,3159,5080,'Талица'),(5148,3159,5080,'Тугулым'),(5149,3159,5080,'Туринск'),(5150,3159,5080,'Туринская Слобода'),(5152,3159,5151,'Алагир'),(5153,3159,5151,'Ардон'),(5154,3159,5151,'Беслан'),(5155,3159,5151,'Бурон'),(5156,3159,5151,'Владикавказ'),(5157,3159,5151,'Дигора'),(5158,3159,5151,'Моздок'),(5159,3159,5151,'Орджоникидзе'),(5160,3159,5151,'Чикола'),(5162,3159,5161,'Велиж'),(5163,3159,5161,'Верхнеднепровский'),(5164,3159,5161,'Ворга'),(5165,3159,5161,'Вязьма'),(5166,3159,5161,'Гагарин'),(5167,3159,5161,'Глинка'),(5168,3159,5161,'Голынки'),(5169,3159,5161,'Демидов'),(5170,3159,5161,'Дорогобуж'),(5171,3159,5161,'Духовщина'),(5172,3159,5161,'Екимовичи'),(5173,3159,5161,'Ельня'),(5174,3159,5161,'Ершичи'),(5175,3159,5161,'Издешково'),(5176,3159,5161,'Кардымово'),(5177,3159,5161,'Красный'),(5178,3159,5161,'Монастырщина'),(5179,3159,5161,'Новодугино'),(5180,3159,5161,'Починок'),(5181,3159,5161,'Рославль'),(5182,3159,5161,'Рудня'),(5183,3159,5161,'Сафоново'),(5184,3159,5161,'Смоленск'),(5185,3159,5161,'Сычевка'),(5186,3159,5161,'Угра'),(5187,3159,5161,'Хиславичи'),(5188,3159,5161,'Холм-Жирковский'),(5189,3159,5161,'Шумячи'),(5190,3159,5161,'Ярцево'),(5192,3159,5191,'Александровское'),(5193,3159,5191,'Арзгир'),(5194,3159,5191,'Благодарный'),(5195,3159,5191,'Буденновск'),(5196,3159,5191,'Георгиевск'),(5197,3159,5191,'Дивное'),(5198,3159,5191,'Домбай'),(5199,3159,5191,'Донское'),(5200,3159,5191,'Ессентуки'),(5201,3159,5191,'Иноземцево'),(5202,3159,5191,'Ипатово'),(5203,3159,5191,'Карачаевск'),(5204,3159,5191,'Кисловодск'),(5205,3159,5191,'Кочубеевское'),(5206,3159,5191,'Красногвардейское'),(5207,3159,5191,'Курсавка'),(5208,3159,5191,'Левокумское'),(5209,3159,5191,'Минеральные Воды'),(5210,3159,5191,'Невинномысск'),(5211,3159,5191,'Нефтекумск'),(5212,3159,5191,'Новоалександровск'),(5213,3159,5191,'Новоалександровская'),(5214,3159,5191,'Новопавловск'),(5215,3159,5191,'Новоселицкое'),(5216,3159,5191,'Преградная'),(5217,3159,5191,'Пятигорск'),(5218,3159,5191,'Светлоград'),(5219,3159,5191,'Ставрополь'),(5220,3159,5191,'Степное'),(5221,3159,5191,'Теберда'),(5222,3159,5191,'Усть-Джегута'),(5223,3159,5191,'Хабез'),(5224,3159,5191,'Черкесск'),(5226,3159,5225,'Бондари'),(5227,3159,5225,'Гавриловка Вторая'),(5228,3159,5225,'Жердевка'),(5229,3159,5225,'Знаменка'),(5230,3159,5225,'Инжавино'),(5231,3159,5225,'Кирсанов'),(5232,3159,5225,'Котовск'),(5233,3159,5225,'Мичуринск'),(5234,3159,5225,'Мордово'),(5235,3159,5225,'Моршанск'),(5236,3159,5225,'Мучкапский'),(5237,3159,5225,'Петровское'),(5238,3159,5225,'Пичаево'),(5239,3159,5225,'Рассказово'),(5240,3159,5225,'Ржакса'),(5241,3159,5225,'Староюрьево'),(5242,3159,5225,'Тамбов'),(5243,3159,5225,'Токаревка'),(5244,3159,5225,'Уварово'),(5245,3159,5225,'Умет'),(5247,3159,5246,'Агрыз'),(5248,3159,5246,'Азнакаево'),(5249,3159,5246,'Аксубаево'),(5250,3159,5246,'Актюбинский'),(5251,3159,5246,'Алексеевское'),(5252,3159,5246,'Альметьевск'),(5253,3159,5246,'Апастово'),(5254,3159,5246,'Арск'),(5255,3159,5246,'Бавлы'),(5256,3159,5246,'Базарные Матаки'),(5257,3159,5246,'Балтаси'),(5258,3159,5246,'Богатые Сабы'),(5259,3159,5246,'Брежнев'),(5260,3159,5246,'Бугульма'),(5261,3159,5246,'Буинск'),(5262,3159,5246,'Васильево'),(5263,3159,5246,'Верхний Услон'),(5264,3159,5246,'Высокая Гора'),(5265,3159,5246,'Дербешкинский'),(5266,3159,5246,'Елабуга'),(5267,3159,5246,'Заинск'),(5268,3159,5246,'Зеленодольск'),(5269,3159,5246,'Казань'),(5270,3159,5246,'Камское Устье'),(5271,3159,5246,'Карабаш'),(5272,3159,5246,'Куйбышев'),(5273,3159,5246,'Кукмод'),(5274,3159,5246,'Кукмор'),(5275,3159,5246,'Лаишево'),(5276,3159,5246,'Лениногорск'),(5277,3159,5246,'Мамадыш'),(5278,3159,5246,'Менделеевск'),(5279,3159,5246,'Мензелинск'),(5280,3159,5246,'Муслюмово'),(5281,3159,5246,'Набережные Челны'),(5282,3159,5246,'Нижнекамск'),(5283,3159,5246,'Новошешминск'),(5284,3159,5246,'Нурлат'),(5285,3159,5246,'Пестрецы'),(5286,3159,5246,'Рыбная Слобода'),(5287,3159,5246,'Сарманово'),(5288,3159,5246,'Старое Дрожжаное'),(5289,3159,5246,'Тетюши'),(5290,3159,5246,'Чистополь'),(5292,3159,5291,'Александровское'),(5293,3159,5291,'Асино'),(5294,3159,5291,'Бакчар'),(5295,3159,5291,'Батурино'),(5296,3159,5291,'Белый Яр'),(5297,3159,5291,'Зырянское'),(5298,3159,5291,'Итатка'),(5299,3159,5291,'Каргасок'),(5300,3159,5291,'Катайга'),(5301,3159,5291,'Кожевниково'),(5302,3159,5291,'Колпашево'),(5303,3159,5291,'Кривошеино'),(5304,3159,5291,'Мельниково'),(5305,3159,5291,'Молчаново'),(5306,3159,5291,'Парабель'),(5307,3159,5291,'Первомайское'),(5308,3159,5291,'Подгорное'),(5309,3159,5291,'Стрежевой'),(5310,3159,5291,'Томск'),(5311,3159,5291,'Тымск'),(5313,3159,5312,'Ак-Довурак'),(5314,3159,5312,'Бай Хаак'),(5315,3159,5312,'Кызыл'),(5316,3159,5312,'Самагалтай'),(5317,3159,5312,'Сарыг-Сеп'),(5318,3159,5312,'Суть-Холь'),(5319,3159,5312,'Тоора-Хем'),(5320,3159,5312,'Туран'),(5321,3159,5312,'Тээли'),(5322,3159,5312,'Хову-Аксы'),(5323,3159,5312,'Чадан'),(5324,3159,5312,'Шагонар'),(5325,3159,5312,'Эрзин'),(5327,3159,5326,'Агеево'),(5328,3159,5326,'Алексин'),(5329,3159,5326,'Арсеньево'),(5330,3159,5326,'Барсуки'),(5331,3159,5326,'Бегичевский'),(5332,3159,5326,'Богородицк'),(5333,3159,5326,'Болохово'),(5334,3159,5326,'Велегож'),(5335,3159,5326,'Венев'),(5336,3159,5326,'Волово'),(5337,3159,5326,'Горелки'),(5338,3159,5326,'Донской'),(5339,3159,5326,'Дубна'),(5340,3159,5326,'Епифань'),(5341,3159,5326,'Ефремов'),(5342,3159,5326,'Заокский'),(5343,3159,5326,'Казановка'),(5344,3159,5326,'Кимовск'),(5345,3159,5326,'Киреевск'),(5346,3159,5326,'Куркино'),(5347,3159,5326,'Ленинский'),(5348,3159,5326,'Новомосковск'),(5349,3159,5326,'Одоев'),(5350,3159,5326,'Плавск'),(5351,3159,5326,'Суворов'),(5352,3159,5326,'Тула'),(5353,3159,5326,'Узловая'),(5354,3159,5326,'Щекино'),(5355,3159,5326,'Ясногорск'),(5357,3159,5356,'Абатский'),(5358,3159,5356,'Аксарка'),(5359,3159,5356,'Армизонское'),(5360,3159,5356,'Аромашево'),(5361,3159,5356,'Бердюжье'),(5362,3159,5356,'Большое Сорокино'),(5363,3159,5356,'Вагай'),(5364,3159,5356,'Викулово'),(5365,3159,5356,'Винзили'),(5366,3159,5356,'Голышманово'),(5367,3159,5356,'Заводопетровский'),(5368,3159,5356,'Заводоуковск'),(5369,3159,5356,'Исетское'),(5370,3159,5356,'Ишим'),(5371,3159,5356,'Казанское'),(5372,3159,5356,'Казым-Мыс'),(5373,3159,5356,'Кондинское'),(5374,3159,5356,'Красноселькуп'),(5375,3159,5356,'Лабытнанги'),(5376,3159,5356,'Мужи'),(5377,3159,5356,'Надым'),(5378,3159,5356,'Находка'),(5379,3159,5356,'Нефтеюганск'),(5380,3159,5356,'Нижневартовск'),(5381,3159,5356,'Нижняя Тавда'),(5382,3159,5356,'Новый Уренгой'),(5383,3159,5356,'Ноябрьск'),(5384,3159,5356,'Нягань'),(5385,3159,5356,'Октябрьское'),(5386,3159,5356,'Омутинский'),(5387,3159,5356,'Радужный'),(5388,3159,5356,'Салехард'),(5389,3159,5356,'Сладково'),(5390,3159,5356,'Советский'),(5391,3159,5356,'Сургут'),(5392,3159,5356,'Тазовский'),(5393,3159,5356,'Тарко-Сале'),(5394,3159,5356,'Тобольск'),(5395,3159,5356,'Тюмень'),(5396,3159,5356,'Уват'),(5397,3159,5356,'Упорово'),(5398,3159,5356,'Урай'),(5399,3159,5356,'Ханты-Мансийск'),(5400,3159,5356,'Юрибей'),(5401,3159,5356,'Ялуторовск'),(5402,3159,5356,'Яр-Сале'),(5403,3159,5356,'Ярково'),(5405,3159,5404,'Алнаши'),(5406,3159,5404,'Балезино'),(5407,3159,5404,'Вавож'),(5408,3159,5404,'Воткинск'),(5409,3159,5404,'Глазов'),(5410,3159,5404,'Грахово'),(5411,3159,5404,'Дебесы'),(5412,3159,5404,'Завьялово'),(5413,3159,5404,'Игра'),(5414,3159,5404,'Ижевск'),(5415,3159,5404,'Кама'),(5416,3159,5404,'Камбарка'),(5417,3159,5404,'Каракулино'),(5418,3159,5404,'Кез'),(5419,3159,5404,'Кизнер'),(5420,3159,5404,'Киясово'),(5421,3159,5404,'Красногорское'),(5422,3159,5404,'Можга'),(5423,3159,5404,'Сарапул'),(5424,3159,5404,'Селты'),(5425,3159,5404,'Сюмси'),(5426,3159,5404,'Ува'),(5427,3159,5404,'Устинов'),(5428,3159,5404,'Шаркан'),(5429,3159,5404,'Юкаменское'),(5430,3159,5404,'Якшур-Бодья'),(5431,3159,5404,'Яр'),(5433,3159,5432,'Базарный Сызган'),(5434,3159,5432,'Барыш'),(5435,3159,5432,'Большое Нагаткино'),(5436,3159,5432,'Вешкайма'),(5437,3159,5432,'Глотовка'),(5438,3159,5432,'Димитровград'),(5439,3159,5432,'Игнатовка'),(5440,3159,5432,'Измайлово'),(5441,3159,5432,'Инза'),(5442,3159,5432,'Ишеевка'),(5443,3159,5432,'Канадей'),(5444,3159,5432,'Карсун'),(5445,3159,5432,'Кузоватово'),(5446,3159,5432,'Майна'),(5447,3159,5432,'Новая Малыкла'),(5448,3159,5432,'Новоспасское'),(5449,3159,5432,'Павловка'),(5450,3159,5432,'Радищево'),(5451,3159,5432,'Сенгилей'),(5452,3159,5432,'Старая Кулатка'),(5453,3159,5432,'Старая Майна'),(5454,3159,5432,'Сурское'),(5455,3159,5432,'Тереньга'),(5456,3159,5432,'Ульяновск'),(5457,3159,5432,'Чердаклы'),(5459,3159,5458,'Аксай'),(5460,3159,5458,'Дарьинское'),(5461,3159,5458,'Деркул'),(5462,3159,5458,'Джамбейты'),(5463,3159,5458,'Джаныбек'),(5464,3159,5458,'Казталовка'),(5465,3159,5458,'Калмыково'),(5466,3159,5458,'Каратобе'),(5467,3159,5458,'Переметное'),(5468,3159,5458,'Сайхин'),(5469,3159,5458,'Уральск'),(5470,3159,5458,'Федоровка'),(5471,3159,5458,'Фурманово'),(5472,3159,5458,'Чапаев'),(5474,3159,5473,'Амурск'),(5475,3159,5473,'Аян'),(5476,3159,5473,'Березовый'),(5477,3159,5473,'Бикин'),(5478,3159,5473,'Бира'),(5479,3159,5473,'Биракан'),(5480,3159,5473,'Богородское'),(5481,3159,5473,'Болонь'),(5482,3159,5473,'Ванино'),(5483,3159,5473,'Волочаевка Вторая'),(5484,3159,5473,'Высокогорный'),(5485,3159,5473,'Вяземский'),(5486,3159,5473,'Горный'),(5487,3159,5473,'Гурское'),(5488,3159,5473,'Дормидонтовка'),(5489,3159,5473,'Заветы Ильича'),(5490,3159,5473,'Известковый'),(5491,3159,5473,'Иннокентьевка'),(5492,3159,5473,'Комсомольск-на-Амуре'),(5493,3159,5473,'Ленинское'),(5494,3159,5473,'Нелькан'),(5495,3159,5473,'Николаевск-на-Амуре'),(5496,3159,5473,'Облучье'),(5497,3159,5473,'Охотск'),(5498,3159,5473,'Переяславка'),(5499,3159,5473,'Смидович'),(5500,3159,5473,'Советская Гавань'),(5501,3159,5473,'Софийск'),(5502,3159,5473,'Троицкое'),(5503,3159,5473,'Тугур'),(5504,3159,5473,'Хабаровск'),(5505,3159,5473,'Чегдомын'),(5506,3159,5473,'Чумикан'),(5508,3159,5507,'Агаповка'),(5509,3159,5507,'Аргаяш'),(5510,3159,5507,'Аша'),(5511,3159,5507,'Бакал'),(5512,3159,5507,'Бреды'),(5513,3159,5507,'Варна'),(5514,3159,5507,'Верхнеуральск'),(5515,3159,5507,'Верхний Уфалей'),(5516,3159,5507,'Еманжелинск'),(5517,3159,5507,'Златоуст'),(5518,3159,5507,'Карабаш'),(5519,3159,5507,'Карталы'),(5520,3159,5507,'Касли'),(5521,3159,5507,'Катав-Ивановск'),(5522,3159,5507,'Копейск'),(5523,3159,5507,'Коркино'),(5524,3159,5507,'Кунашак'),(5525,3159,5507,'Куса'),(5526,3159,5507,'Кыштым'),(5527,3159,5507,'Магнитогорск'),(5528,3159,5507,'Миасс'),(5529,3159,5507,'Октябрьское'),(5530,3159,5507,'Пласт'),(5531,3159,5507,'Сатка'),(5532,3159,5507,'Сим'),(5533,3159,5507,'Троицк'),(5534,3159,5507,'Увельский'),(5535,3159,5507,'Уйское'),(5536,3159,5507,'Усть-Катав'),(5537,3159,5507,'Фершампенуаз'),(5538,3159,5507,'Чебаркуль'),(5539,3159,5507,'Челябинск'),(5540,3159,5507,'Чесма'),(5541,3159,5507,'Южно-Уральск'),(5542,3159,5507,'Юрюзань'),(5544,3159,5543,'Аргун'),(5545,3159,5543,'Грозный'),(5546,3159,5543,'Гудермез'),(5547,3159,5543,'Малгобек'),(5548,3159,5543,'Назрань'),(5549,3159,5543,'Наурская'),(5550,3159,5543,'Ножай-Юрт'),(5551,3159,5543,'Орджоникидзевская'),(5552,3159,5543,'Советское'),(5553,3159,5543,'Урус-Мартан'),(5554,3159,5543,'Шали'),(5556,3159,5555,'Агинское'),(5557,3159,5555,'Аксеново-Зиловское'),(5558,3159,5555,'Акша'),(5559,3159,5555,'Александровский Завод'),(5560,3159,5555,'Амазар'),(5561,3159,5555,'Арбагар'),(5562,3159,5555,'Атамановка'),(5563,3159,5555,'Балей'),(5564,3159,5555,'Борзя'),(5565,3159,5555,'Букачача'),(5566,3159,5555,'Газимурский Завод'),(5567,3159,5555,'Давенда'),(5568,3159,5555,'Дарасун'),(5569,3159,5555,'Дровяная'),(5570,3159,5555,'Дульдурга'),(5571,3159,5555,'Жиндо'),(5572,3159,5555,'Забайкальск'),(5573,3159,5555,'Итака'),(5574,3159,5555,'Калга'),(5575,3159,5555,'Карымское'),(5576,3159,5555,'Кличка'),(5577,3159,5555,'Ключевский'),(5578,3159,5555,'Кокуй'),(5579,3159,5555,'Краснокаменск'),(5580,3159,5555,'Красный Чикой'),(5581,3159,5555,'Кыра'),(5582,3159,5555,'Моготуй'),(5583,3159,5555,'Могоча'),(5584,3159,5555,'Нерчинск'),(5585,3159,5555,'Нерчинский Завод'),(5586,3159,5555,'Нижний Часучей'),(5587,3159,5555,'Оловянная'),(5588,3159,5555,'Первомайский'),(5589,3159,5555,'Петровск-Забайкальский'),(5590,3159,5555,'Приаргунск'),(5591,3159,5555,'Сретенск'),(5592,3159,5555,'Тупик'),(5593,3159,5555,'Улеты'),(5594,3159,5555,'Хилок'),(5595,3159,5555,'Чара'),(5596,3159,5555,'Чернышевск'),(5597,3159,5555,'Чита'),(5598,3159,5555,'Шелопугино'),(5599,3159,5555,'Шилка'),(5601,3159,5600,'Алатырь'),(5602,3159,5600,'Аликово'),(5603,3159,5600,'Батырева'),(5604,3159,5600,'Буинск'),(5605,3159,5600,'Вурнары'),(5606,3159,5600,'Ибреси'),(5607,3159,5600,'Канаш'),(5608,3159,5600,'Киря'),(5609,3159,5600,'Комсомольское'),(5610,3159,5600,'Красноармейское'),(5611,3159,5600,'Красные Четаи'),(5612,3159,5600,'Кугеси'),(5613,3159,5600,'Мариинский Посад'),(5614,3159,5600,'Моргауши'),(5615,3159,5600,'Новочебоксарск'),(5616,3159,5600,'Порецкое'),(5617,3159,5600,'Урмары'),(5618,3159,5600,'Цивильск'),(5619,3159,5600,'Чебоксары'),(5620,3159,5600,'Шемурша'),(5621,3159,5600,'Шумерля'),(5622,3159,5600,'Ядрин'),(5623,3159,5600,'Яльчики'),(5624,3159,5600,'Янтиково'),(5626,3159,5625,'Андропов'),(5627,3159,5625,'Берендеево'),(5628,3159,5625,'Большое Село'),(5629,3159,5625,'Борисоглебский'),(5630,3159,5625,'Брейтово'),(5631,3159,5625,'Бурмакино'),(5632,3159,5625,'Варегово'),(5633,3159,5625,'Волга'),(5634,3159,5625,'Гаврилов Ям'),(5635,3159,5625,'Данилов'),(5636,3159,5625,'Любим'),(5637,3159,5625,'Мышкино'),(5638,3159,5625,'Некрасовское'),(5639,3159,5625,'Новый Некоуз'),(5640,3159,5625,'Переславль-Залесский'),(5641,3159,5625,'Пошехонье-Володарск'),(5642,3159,5625,'Ростов'),(5643,3159,5625,'Рыбинск'),(5644,3159,5625,'Тутаев'),(5645,3159,5625,'Углич'),(5646,3159,5625,'Ярославль'),(5649,5647,5648,'Ахуачапан'),(5651,5647,5650,'Коютепекуэ'),(5653,5647,5652,'Нуэва Сан Салвадор'),(5655,5647,5654,'Закатеколука'),(5657,5647,5656,'Ла Унион'),(5659,5647,5658,'Сан Мигуэл'),(5661,5647,5660,'Сан-Сальвадор'),(5663,5647,5662,'Санта Ана'),(5665,5647,5664,'Сонсонат'),(5668,5666,5667,'Трнава'),(5670,5666,5669,'Братислава'),(5672,5666,5671,'Зилина'),(5675,5673,5674,'Любляна'),(5677,5673,5676,'Марибор'),(5680,5678,5679,'Парамарибо'),(5683,5681,5682,'Айдахо-Фоллс'),(5684,5681,5682,'Арко'),(5685,5681,5682,'Барли'),(5686,5681,5682,'Бойсе'),(5687,5681,5682,'Левистон'),(5688,5681,5682,'Маунтейн-Хоум'),(5689,5681,5682,'Монтпелье'),(5690,5681,5682,'Орофино'),(5691,5681,5682,'Покателло'),(5692,5681,5682,'Рексбург'),(5693,5681,5682,'Руперт'),(5695,5681,5694,'Айова-Сити'),(5696,5681,5694,'Аллеман'),(5697,5681,5694,'Амес'),(5698,5681,5694,'Асбури'),(5699,5681,5694,'Барлингтон'),(5700,5681,5694,'Беттендорф'),(5701,5681,5694,'Блуэ Грасс'),(5702,5681,5694,'Буффало'),(5703,5681,5694,'Ватерлоо'),(5704,5681,5694,'Вест-Де-Мойн'),(5705,5681,5694,'Виндсор-Хейгтс'),(5706,5681,5694,'Вудвард'),(5707,5681,5694,'Гилбертвилл'),(5708,5681,5694,'Гринфилд'),(5709,5681,5694,'Давенпорт'),(5710,5681,5694,'Де-Мойн'),(5711,5681,5694,'Декора'),(5712,5681,5694,'Денвер'),(5713,5681,5694,'Джайнсвилл'),(5714,5681,5694,'Дубукуэ'),(5715,5681,5694,'Елдридж'),(5716,5681,5694,'Калумет'),(5717,5681,5694,'Камминг'),(5718,5681,5694,'Картер-Лейк'),(5719,5681,5694,'Кеокук'),(5720,5681,5694,'Клайв'),(5721,5681,5694,'Кларинда'),(5722,5681,5694,'Клинтон'),(5723,5681,5694,'Консил-Блаффс'),(5724,5681,5694,'Коридон'),(5725,5681,5694,'Крескент'),(5726,5681,5694,'Креско'),(5727,5681,5694,'Лайон'),(5728,5681,5694,'Маршаллтаун'),(5729,5681,5694,'Масон-Сити'),(5730,5681,5694,'Норвалк'),(5731,5681,5694,'Олбани'),(5732,5681,5694,'Онава'),(5733,5681,5694,'Осадж'),(5734,5681,5694,'Оттумва'),(5735,5681,5694,'Плисант-Хилл'),(5736,5681,5694,'Ред-Оак'),(5737,5681,5694,'Реймонд'),(5738,5681,5694,'Ривердал'),(5739,5681,5694,'Сагевилл'),(5740,5681,5694,'Седар-Рапидс'),(5741,5681,5694,'Седар-Фоллс'),(5742,5681,5694,'Сиу-Сити'),(5743,5681,5694,'Урбандал'),(5744,5681,5694,'Форт-Додж'),(5745,5681,5694,'Чаритон'),(5746,5681,5694,'Чарльс-Сити'),(5747,5681,5694,'Эвансдейл'),(5748,5681,5694,'Элдора'),(5749,5681,5694,'Элк-Ран-Хейгтс'),(5750,5681,5694,'Эмметсбург'),(5751,5681,5694,'Эстервилл'),(5753,5681,5752,'Авон'),(5754,5681,5752,'Айрондейл'),(5755,5681,5752,'Акрон'),(5756,5681,5752,'Альбертвиль'),(5757,5681,5752,'Андалусиа'),(5758,5681,5752,'Аннистон'),(5759,5681,5752,'Атенс'),(5760,5681,5752,'Атмор'),(5761,5681,5752,'Аубурн'),(5762,5681,5752,'Аутаугавилл'),(5763,5681,5752,'Бабби'),(5764,5681,5752,'Бентон'),(5765,5681,5752,'Берри'),(5766,5681,5752,'Бессемер'),(5767,5681,5752,'Бир Крик'),(5768,5681,5752,'Бирмингам'),(5769,5681,5752,'Блу-Маунтайн'),(5770,5681,5752,'Боаз'),(5771,5681,5752,'Бревтон'),(5772,5681,5752,'Бригтон'),(5773,5681,5752,'Бриллиант'),(5774,5681,5752,'Бруквуд'),(5775,5681,5752,'Ваверли'),(5776,5681,5752,'Валдо'),(5777,5681,5752,'Ванк'),(5778,5681,5752,'Вебб'),(5779,5681,5752,'Веставиа Хиллс'),(5780,5681,5752,'Ветумпка'),(5781,5681,5752,'Вивер'),(5782,5681,5752,'Вилмер'),(5783,5681,5752,'Вудвилл'),(5784,5681,5752,'Гадсден'),(5785,5681,5752,'Гардендал'),(5786,5681,5752,'Гералдин'),(5787,5681,5752,'Голдвилл'),(5788,5681,5752,'Грант'),(5789,5681,5752,'Гров Хилл'),(5790,5681,5752,'Гу-Вин'),(5791,5681,5752,'Гудватер'),(5792,5681,5752,'Гуин'),(5793,5681,5752,'Гурли'),(5794,5681,5752,'Далевилл'),(5795,5681,5752,'Дафна'),(5796,5681,5752,'Декатур'),(5797,5681,5752,'Дора'),(5798,5681,5752,'Дотан'),(5799,5681,5752,'Дуттон'),(5800,5681,5752,'Елба'),(5801,5681,5752,'Еуфаула'),(5802,5681,5752,'Карбон Хилл'),(5803,5681,5752,'Кауартс'),(5804,5681,5752,'Кинси'),(5805,5681,5752,'Кинстон'),(5806,5681,5752,'Клантон'),(5807,5681,5752,'Клэйхатчи'),(5808,5681,5752,'Коллинсвилл'),(5809,5681,5752,'Кордова'),(5810,5681,5752,'Коттонвуд'),(5811,5681,5752,'Коуртланд'),(5812,5681,5752,'Креола'),(5813,5681,5752,'Кэмп-Хилл'),(5814,5681,5752,'Лангдал'),(5815,5681,5752,'Ланетт'),(5816,5681,5752,'Лаундесборо'),(5817,5681,5752,'Лафэйетт'),(5818,5681,5752,'Лейгтон'),(5819,5681,5752,'Лексингтон'),(5820,5681,5752,'Липскомб'),(5821,5681,5752,'Лисбург'),(5822,5681,5752,'Литтл Шавмут'),(5823,5681,5752,'Лоачапока'),(5824,5681,5752,'Локсли'),(5825,5681,5752,'Локхарт'),(5826,5681,5752,'Луверн'),(5827,5681,5752,'Мадрид'),(5828,5681,5752,'Малверн'),(5829,5681,5752,'Маундвилл'),(5830,5681,5752,'Мидланд-Сити'),(5831,5681,5752,'Мидфилд'),(5832,5681,5752,'Миллбрук'),(5833,5681,5752,'Мобил'),(5834,5681,5752,'Монтгомери'),(5835,5681,5752,'Моултон'),(5836,5681,5752,'Моунтаин Брук'),(5837,5681,5752,'Моунтаинборо'),(5838,5681,5752,'Муресвилл'),(5839,5681,5752,'Мускл Шоалс'),(5840,5681,5752,'Напир-Филд'),(5841,5681,5752,'Нортпорт'),(5842,5681,5752,'Нотасулга'),(5843,5681,5752,'Нью-Сайт'),(5844,5681,5752,'Ньювилл'),(5845,5681,5752,'Оак Хилл'),(5846,5681,5752,'Оакман'),(5847,5681,5752,'Озарк'),(5848,5681,5752,'Онича'),(5849,5681,5752,'Опелика'),(5850,5681,5752,'Опп'),(5851,5681,5752,'Охатчи'),(5852,5681,5752,'Паинт Рок'),(5853,5681,5752,'Парриш'),(5854,5681,5752,'Пинкард'),(5855,5681,5752,'Плисант Гров'),(5856,5681,5752,'Пойнт-Клир'),(5857,5681,5752,'Поллард'),(5858,5681,5752,'Праттвилл'),(5859,5681,5752,'Прикевилл'),(5860,5681,5752,'Причард'),(5861,5681,5752,'Раинсвилл'),(5862,5681,5752,'Редстон Арсенал'),(5863,5681,5752,'Ривер Вив'),(5864,5681,5752,'Рик-Сити'),(5865,5681,5752,'Робинсон Спрингс'),(5866,5681,5752,'Рогерсвилл'),(5867,5681,5752,'Русселлвилл'),(5868,5681,5752,'Рутледж'),(5869,5681,5752,'Сакс'),(5870,5681,5752,'Санфорд'),(5871,5681,5752,'Сараланд'),(5872,5681,5752,'Сатсума'),(5873,5681,5752,'Седар-Блуфф'),(5874,5681,5752,'Селма'),(5875,5681,5752,'Селмонт'),(5876,5681,5752,'Силакауга'),(5877,5681,5752,'Силваниа'),(5878,5681,5752,'Сипси'),(5879,5681,5752,'Слокомб'),(5880,5681,5752,'Сомервилл'),(5881,5681,5752,'Талладега'),(5882,5681,5752,'Таррант'),(5883,5681,5752,'Таррант-Сити'),(5884,5681,5752,'Теодор'),(5885,5681,5752,'Тиллманс Корнер'),(5886,5681,5752,'Триана'),(5887,5681,5752,'Трой'),(5888,5681,5752,'Тускалуса'),(5889,5681,5752,'Тускеги'),(5890,5681,5752,'Тускумбиа'),(5891,5681,5752,'Уайт-Халл'),(5892,5681,5752,'Унионтаун'),(5893,5681,5752,'Фаирфакс'),(5894,5681,5752,'Фаирфилд'),(5895,5681,5752,'Файрхоп'),(5896,5681,5752,'Фаунсдал'),(5897,5681,5752,'Феникс-Сити'),(5898,5681,5752,'Фифф'),(5899,5681,5752,'Флинт-Сити'),(5900,5681,5752,'Фломатон'),(5901,5681,5752,'Флорала'),(5902,5681,5752,'Флоренк'),(5903,5681,5752,'Форестдал'),(5904,5681,5752,'Форт-Пэйн'),(5905,5681,5752,'Форт-Рукер'),(5906,5681,5752,'Фултон'),(5907,5681,5752,'Фултондал'),(5908,5681,5752,'Хаклебург'),(5909,5681,5752,'Хартфорд'),(5910,5681,5752,'Хенагар'),(5911,5681,5752,'Хидланд'),(5912,5681,5752,'Хомевуд'),(5913,5681,5752,'Хорн Хилл'),(5914,5681,5752,'Хувер'),(5915,5681,5752,'Хугули'),(5916,5681,5752,'Хунтсвилл'),(5917,5681,5752,'Чикасав'),(5918,5681,5752,'Шавмут'),(5919,5681,5752,'Шеффилд'),(5920,5681,5752,'Эшфорд'),(5921,5681,5752,'Яксон'),(5922,5681,5752,'Яксонвилл'),(5923,5681,5752,'Яспер'),(5925,5681,5924,'Анкоридж'),(5926,5681,5924,'Юно'),(5928,5681,5927,'Авондейл'),(5929,5681,5927,'Аджо'),(5930,5681,5927,'Багдад'),(5931,5681,5927,'Бенсон'),(5932,5681,5927,'Велда-Рос-Эстатес'),(5933,5681,5927,'Викенбург'),(5934,5681,5927,'Виллкокс'),(5935,5681,5927,'Гилберт'),(5936,5681,5927,'Глендейл'),(5937,5681,5927,'Глоб'),(5938,5681,5927,'Грин-Вэлли'),(5939,5681,5927,'Гуадалуп'),(5940,5681,5927,'Гудиир'),(5941,5681,5927,'Давис-Монтан АФБ'),(5942,5681,5927,'Дримланд-Вилла'),(5943,5681,5927,'Йоунгтаун'),(5944,5681,5927,'Кайента'),(5945,5681,5927,'Кашион'),(5946,5681,5927,'Кингман'),(5947,5681,5927,'Кларкдейл'),(5948,5681,5927,'Клэйпул'),(5949,5681,5927,'Литчфилд-Парк'),(5950,5681,5927,'Лук'),(5951,5681,5927,'Майми'),(5952,5681,5927,'Марана'),(5953,5681,5927,'Меса'),(5954,5681,5927,'Оракл'),(5955,5681,5927,'Парадайс-Вэлли'),(5956,5681,5927,'Паркер'),(5957,5681,5927,'Пеориа'),(5958,5681,5927,'Пиртлевилл'),(5959,5681,5927,'Прескотт'),(5960,5681,5927,'Пэйсон'),(5961,5681,5927,'Сан-Мануэль'),(5962,5681,5927,'Сан-Сити'),(5963,5681,5927,'Саут-Таксон'),(5964,5681,5927,'Селлс'),(5965,5681,5927,'Скоттсдейл'),(5966,5681,5927,'Спрингервилл'),(5967,5681,5927,'Сьерра-Виста'),(5968,5681,5927,'Таксон'),(5969,5681,5927,'Темп'),(5970,5681,5927,'Толлесон'),(5971,5681,5927,'Туба-Сити'),(5972,5681,5927,'Тусон'),(5973,5681,5927,'Финикс'),(5974,5681,5927,'Флагстафф'),(5975,5681,5927,'Флоренц'),(5976,5681,5927,'Форт-Дефианс'),(5977,5681,5927,'Форт-Хуачука'),(5978,5681,5927,'Чандлер'),(5979,5681,5927,'Чинли'),(5980,5681,5927,'Шау-Ло'),(5981,5681,5927,'Эль-Мираг'),(5982,5681,5927,'Юма'),(5984,5681,5983,'Александер'),(5985,5681,5983,'Арканзас-Сити'),(5986,5681,5983,'Аткинс'),(5987,5681,5983,'Бакнер'),(5988,5681,5983,'Балд-Кноб'),(5989,5681,5983,'Барлинг'),(5990,5681,5983,'Бауксит'),(5991,5681,5983,'Бентон'),(5992,5681,5983,'Бетел-Хейгтс'),(5993,5681,5983,'Блевинс'),(5994,5681,5983,'Блитевилл'),(5995,5681,5983,'Бонанза'),(5996,5681,5983,'Брадфорд'),(5997,5681,5983,'Булл-Шоалс'),(5998,5681,5983,'Бурдетт'),(5999,5681,5983,'Валдрон'),(6000,5681,5983,'Валнут-Ридж'),(6001,5681,5983,'Ван-Бурен'),(6002,5681,5983,'Варрен'),(6003,5681,5983,'Вашингтон'),(6004,5681,5983,'Вест Хелена'),(6005,5681,5983,'Вест-Мемфис'),(6006,5681,5983,'Виллисвилл'),(6007,5681,5983,'Винн'),(6008,5681,5983,'Гассвилл'),(6009,5681,5983,'Де-Витт'),(6010,5681,5983,'Дермотт'),(6011,5681,5983,'Джадсониа'),(6012,5681,5983,'Джексонвилл'),(6013,5681,5983,'Едмондсон'),(6014,5681,5983,'Ингленд'),(6015,5681,5983,'Йонесборо'),(6016,5681,5983,'Калдвелл'),(6017,5681,5983,'Каммак-Виллидж'),(6018,5681,5983,'Кейл'),(6019,5681,5983,'Кенсетт'),(6020,5681,5983,'Киблер'),(6021,5681,5983,'Колт'),(6022,5681,5983,'Корнинг'),(6023,5681,5983,'Коттер'),(6024,5681,5983,'Кравфордсвилл'),(6025,5681,5983,'Кроссетт'),(6026,5681,5983,'Лавака'),(6027,5681,5983,'Левисвилл'),(6028,5681,5983,'Лейк-Виллидж'),(6029,5681,5983,'Лейк-Хамилтон'),(6030,5681,5983,'Литтл-Рок'),(6031,5681,5983,'Лонсдейл'),(6032,5681,5983,'Луксора'),(6033,5681,5983,'Магнесс'),(6034,5681,5983,'Мак-Гехи'),(6035,5681,5983,'Мак-Каскилл'),(6036,5681,5983,'Мак-Нейл'),(6037,5681,5983,'Малверн'),(6038,5681,5983,'Марианна'),(6039,5681,5983,'Марион'),(6040,5681,5983,'Маунтайн-Хоум'),(6041,5681,5983,'Мена'),(6042,5681,5983,'Мэйфлауер'),(6043,5681,5983,'Норт-Литтл-Рок'),(6044,5681,5983,'Норфорк'),(6045,5681,5983,'Ньюарк'),(6046,5681,5983,'Нэшвилл'),(6047,5681,5983,'Оакхавен'),(6048,5681,5983,'Озан'),(6049,5681,5983,'Ойл-Троуг'),(6050,5681,5983,'Олбани'),(6051,5681,5983,'Оскеола'),(6052,5681,5983,'Пайн-Блафф'),(6053,5681,5983,'Парагоулд'),(6054,5681,5983,'Парис'),(6055,5681,5983,'Перритаун'),(6056,5681,5983,'Пигготт'),(6057,5681,5983,'Плисант-Плайнс'),(6058,5681,5983,'Плумервилл'),(6059,5681,5983,'Покахонтас'),(6060,5681,5983,'Поттсвилл'),(6061,5681,5983,'Прескотт'),(6062,5681,5983,'Расселл'),(6063,5681,5983,'Расселлвилл'),(6064,5681,5983,'Рокпорт'),(6065,5681,5983,'Росстон'),(6066,5681,5983,'Руди'),(6067,5681,5983,'Сабиако'),(6068,5681,5983,'Сентрал-Сити'),(6069,5681,5983,'Сирки'),(6070,5681,5983,'Скрантон'),(6071,5681,5983,'Смаковер'),(6072,5681,5983,'Спрингдал'),(6073,5681,5983,'Стампс'),(6074,5681,5983,'Статтгарт'),(6075,5681,5983,'Толлетт'),(6076,5681,5983,'Тонтитаун'),(6077,5681,5983,'Тэйлор'),(6078,5681,5983,'Фейеттевилл'),(6079,5681,5983,'Флиппин'),(6080,5681,5983,'Фордик'),(6081,5681,5983,'Форрест-Сити'),(6082,5681,5983,'Форт-Смит'),(6083,5681,5983,'Харди'),(6084,5681,5983,'Хаскелл'),(6085,5681,5983,'Хелена'),(6086,5681,5983,'Хоппер'),(6087,5681,5983,'Хот-Спрингс'),(6088,5681,5983,'Хот-Спрингс (национальный парк)'),(6089,5681,5983,'Хоуп'),(6090,5681,5983,'Шаннон-Хиллс'),(6091,5681,5983,'Шервуд'),(6092,5681,5983,'Элкинс'),(6093,5681,5983,'Элм-Спрингс'),(6094,5681,5983,'Элма'),(6095,5681,5983,'Эль-Дорадо'),(6096,5681,5983,'Эмерсон'),(6097,5681,5983,'Эммет'),(6099,5681,6098,'Каспер'),(6100,5681,6098,'Олбани'),(6101,5681,6098,'Шайенн'),(6102,5681,6098,'Шеридан'),(6104,5681,6103,'Абердин'),(6105,5681,6103,'Алдервуд-Манор'),(6106,5681,6103,'Беллевуэ'),(6107,5681,6103,'Беллингем'),(6108,5681,6103,'Бонни-Лейк'),(6109,5681,6103,'Ботелл'),(6110,5681,6103,'Бревстер'),(6111,5681,6103,'Бремертон'),(6112,5681,6103,'Брин-Мавр'),(6113,5681,6103,'Ванкувер'),(6114,5681,6103,'Венатчи'),(6115,5681,6103,'Голдендейл'),(6116,5681,6103,'Дишман'),(6117,5681,6103,'Дэйтон'),(6118,5681,6103,'Дюпонт'),(6119,5681,6103,'Женева'),(6120,5681,6103,'Интерсити'),(6121,5681,6103,'Ист-Венатчи-Бенч'),(6122,5681,6103,'Истгейт'),(6123,5681,6103,'Кенмор'),(6124,5681,6103,'Кингсгейт'),(6125,5681,6103,'Киркланд'),(6126,5681,6103,'Клайд-Хилл'),(6127,5681,6103,'Кли-Элам'),(6128,5681,6103,'Колвилл'),(6129,5681,6103,'Колфакс'),(6130,5681,6103,'Лонгвью'),(6131,5681,6103,'Мак-Хорд база ВВС'),(6132,5681,6103,'Маунт-Вернон'),(6133,5681,6103,'Медина'),(6134,5681,6103,'Меркер-Айланд'),(6135,5681,6103,'Миллвуд'),(6136,5681,6103,'Мукилтео'),(6137,5681,6103,'Ньюпорт-Хиллс'),(6138,5681,6103,'Олимпия'),(6139,5681,6103,'Омак'),(6140,5681,6103,'Оппортунити'),(6141,5681,6103,'Паркланд'),(6142,5681,6103,'Порт-Анжелес'),(6143,5681,6103,'Рентон'),(6144,5681,6103,'Ривертон'),(6145,5681,6103,'Ритзвилл'),(6146,5681,6103,'Ричланд'),(6147,5681,6103,'Рок-Айленд'),(6148,5681,6103,'Рос-Хилл'),(6149,5681,6103,'Сентралиа'),(6150,5681,6103,'Сиэттл'),(6151,5681,6103,'Скайвэй'),(6152,5681,6103,'Сноухомиш'),(6153,5681,6103,'Спокан'),(6154,5681,6103,'Такома'),(6155,5681,6103,'Таун-энд-Кантри'),(6156,5681,6103,'Томпсон-Плэйс'),(6157,5681,6103,'Тонаскет'),(6158,5681,6103,'Уайт-Сентер'),(6159,5681,6103,'Файрмонт'),(6160,5681,6103,'Форт-Левис'),(6161,5681,6103,'Хантс-Пойнт'),(6162,5681,6103,'Эверетт'),(6163,5681,6103,'Эйрвэй-Хейгтс'),(6164,5681,6103,'Юанита'),(6165,5681,6103,'Якима'),(6166,5681,6103,'Яррау-Пойнт'),(6168,5681,6167,'Берлингтон'),(6169,5681,6167,'Миддлбури'),(6170,5681,6167,'Монпелье'),(6171,5681,6167,'Ньюбури'),(6172,5681,6167,'Олбани'),(6173,5681,6167,'Олбани-Центр'),(6174,5681,6167,'Питтсфорд'),(6175,5681,6167,'Сант-Албанс'),(6177,5681,6176,'Айсл-оф-Вигт'),(6178,5681,6176,'Александрия'),(6179,5681,6176,'Аннандейл'),(6180,5681,6176,'Арлингтон'),(6181,5681,6176,'Байлис-Кроссроадс'),(6182,5681,6176,'Беллвуд'),(6183,5681,6176,'Бенсли'),(6184,5681,6176,'Блу-Ридж'),(6185,5681,6176,'Бон-Айр'),(6186,5681,6176,'Бурквилл'),(6187,5681,6176,'Вайс'),(6188,5681,6176,'Варрентон'),(6189,5681,6176,'Верона'),(6190,5681,6176,'Вилльямсбург'),(6191,5681,6176,'Винтон'),(6192,5681,6176,'Вирджиния-Бич'),(6193,5681,6176,'Витвилл'),(6194,5681,6176,'Вудбридж'),(6195,5681,6176,'Вэйнсборо'),(6196,5681,6176,'Глен-Аллен'),(6197,5681,6176,'Данвилл'),(6198,5681,6176,'Думбартон'),(6199,5681,6176,'Думс'),(6200,5681,6176,'Ист-Хайленд-Парк'),(6201,5681,6176,'Йорктаун'),(6202,5681,6176,'Кейв-Спринг'),(6203,5681,6176,'Крозет'),(6204,5681,6176,'Лаурел'),(6205,5681,6176,'Лейксайд'),(6206,5681,6176,'Линчбург'),(6207,5681,6176,'Лисбург'),(6208,5681,6176,'Лурэй'),(6209,5681,6176,'Манассас-Парк'),(6210,5681,6176,'Марион'),(6211,5681,6176,'Меканиксвилл'),(6212,5681,6176,'Монтроз'),(6213,5681,6176,'Норфолк'),(6214,5681,6176,'Ньюпорт-Ньюс'),(6215,5681,6176,'Петерсбург'),(6216,5681,6176,'Пирисбург'),(6217,5681,6176,'Портсмут'),(6218,5681,6176,'Радфорд'),(6219,5681,6176,'Рашмер'),(6220,5681,6176,'Ричмонд'),(6221,5681,6176,'Роанок'),(6222,5681,6176,'Салем'),(6223,5681,6176,'Саут-Бостон'),(6224,5681,6176,'Севен-Корнерс'),(6225,5681,6176,'Стаунтон'),(6226,5681,6176,'Стьюартс-Драфт'),(6227,5681,6176,'Таппаханнок'),(6228,5681,6176,'Троутвилл'),(6229,5681,6176,'Тукахоэ'),(6230,5681,6176,'Финкастл'),(6231,5681,6176,'Форт-Бельвоир'),(6232,5681,6176,'Фредериксбург'),(6233,5681,6176,'Хайленд-Спрингс'),(6234,5681,6176,'Холлинс'),(6235,5681,6176,'Хоупвелл'),(6236,5681,6176,'Хэмптон'),(6237,5681,6176,'Чарлоттесвилл'),(6238,5681,6176,'Чесапик'),(6239,5681,6176,'Честер'),(6240,5681,6176,'Честерфилд'),(6241,5681,6176,'Эмпориа'),(6243,5681,6242,'Апплетон'),(6244,5681,6242,'Белоит'),(6245,5681,6242,'Брукфилд'),(6246,5681,6242,'Вауватоса'),(6247,5681,6242,'Ваукеша'),(6248,5681,6242,'Ваусау'),(6249,5681,6242,'Вест-Аллис'),(6250,5681,6242,'Грин-Бэй'),(6251,5681,6242,'И-Клер'),(6252,5681,6242,'Кеноша'),(6253,5681,6242,'Ла-Кросс'),(6254,5681,6242,'Мадисон'),(6255,5681,6242,'Манитауок'),(6256,5681,6242,'Меномони Фаллс'),(6257,5681,6242,'Милвауки'),(6258,5681,6242,'Нью-Берлин'),(6259,5681,6242,'Олбани'),(6260,5681,6242,'Ошкош'),(6261,5681,6242,'Ракин'),(6262,5681,6242,'Супериор'),(6263,5681,6242,'Фонд-дю-Лак'),(6264,5681,6242,'Шебоиган'),(6265,5681,6242,'Янесвилл'),(6267,5681,6266,'Ваикапу'),(6268,5681,6266,'Ваилуку'),(6269,5681,6266,'Гонолулу'),(6270,5681,6266,'Каилуа'),(6271,5681,6266,'Канеоха'),(6272,5681,6266,'Капаау'),(6273,5681,6266,'Кахулуи'),(6274,5681,6266,'Килакекуа'),(6275,5681,6266,'Кихей'),(6276,5681,6266,'Лиху'),(6277,5681,6266,'Паия'),(6278,5681,6266,'Пуунен'),(6279,5681,6266,'Хило'),(6281,5681,6280,'Блэйдс'),(6282,5681,6280,'Вилмингтон'),(6283,5681,6280,'Вилмингтон-Манор'),(6284,5681,6280,'Довер'),(6285,5681,6280,'Миллсборо'),(6286,5681,6280,'Ньюпорт'),(6287,5681,6280,'Стантон'),(6288,5681,6280,'Талливилл'),(6289,5681,6280,'Челси-Эстатес'),(6290,5681,6280,'Элсмер'),(6292,5681,6291,'Авондал Естатес'),(6293,5681,6291,'Атенс'),(6294,5681,6291,'Атланта'),(6295,5681,6291,'Аттапулгус'),(6296,5681,6291,'Баинбридж'),(6297,5681,6291,'Барвик'),(6298,5681,6291,'Белведер Парк'),(6299,5681,6291,'Блаирсвилл'),(6300,5681,6291,'Валдоста'),(6301,5681,6291,'Варнер-Робинс'),(6302,5681,6291,'Вашингтон'),(6303,5681,6291,'Вена'),(6304,5681,6291,'Вернонбург'),(6305,5681,6291,'Вест Поинт'),(6306,5681,6291,'Вестсайд'),(6307,5681,6291,'Вилмингтон-Айленд'),(6308,5681,6291,'Вхигам'),(6309,5681,6291,'Вэйкросс'),(6310,5681,6291,'Гарден-Сити'),(6311,5681,6291,'Грешам Парк'),(6312,5681,6291,'Гринсборо'),(6313,5681,6291,'Грэйсвилл'),(6314,5681,6291,'Декатур'),(6315,5681,6291,'Деморест'),(6316,5681,6291,'Джесап'),(6317,5681,6291,'Друид Хиллс'),(6318,5681,6291,'Дублин'),(6319,5681,6291,'Ист-Пойнт'),(6320,5681,6291,'Каиро'),(6321,5681,6291,'Картерсвилл'),(6322,5681,6291,'Климакс'),(6323,5681,6291,'Клэйтон'),(6324,5681,6291,'Колумбус'),(6325,5681,6291,'Коммерк'),(6326,5681,6291,'Кулидж'),(6327,5681,6291,'Куллоден'),(6328,5681,6291,'Лукоут Моунтаин'),(6329,5681,6291,'Макон'),(6330,5681,6291,'Мариэтта'),(6331,5681,6291,'МкАфи'),(6332,5681,6291,'Моултри'),(6333,5681,6291,'Норт Декатур'),(6334,5681,6291,'Норт Друид Хиллс'),(6335,5681,6291,'Огаста'),(6336,5681,6291,'Олбани'),(6337,5681,6291,'Паво'),(6338,5681,6291,'Порт-Вентворт'),(6339,5681,6291,'Пулер'),(6340,5681,6291,'Рингголд'),(6341,5681,6291,'Ром'),(6342,5681,6291,'Россвилл'),(6343,5681,6291,'Саванна'),(6344,5681,6291,'Тандерболт'),(6345,5681,6291,'Томасвилл'),(6346,5681,6291,'Фитзгералд'),(6347,5681,6291,'Форт Оглеторп'),(6348,5681,6291,'Франклин'),(6349,5681,6291,'Хардвик'),(6350,5681,6291,'Хомервилл'),(6351,5681,6291,'Чикамауга'),(6353,5681,6352,'Бакханнон'),(6354,5681,6352,'Барбурсвилл'),(6355,5681,6352,'Бекли'),(6356,5681,6352,'Бенвуд'),(6357,5681,6352,'Вейртон'),(6358,5681,6352,'Вилинг'),(6359,5681,6352,'Гари'),(6360,5681,6352,'Моргантаун'),(6361,5681,6352,'Паркерсбург'),(6362,5681,6352,'Середо'),(6363,5681,6352,'Файрмонт'),(6364,5681,6352,'Хунтингтон'),(6365,5681,6352,'Чарльстон'),(6366,5681,6352,'Элкинс'),(6368,5681,6367,'Авон'),(6369,5681,6367,'Аледо'),(6370,5681,6367,'Алтон'),(6371,5681,6367,'Анна'),(6372,5681,6367,'Аппл Ривер'),(6373,5681,6367,'Арлингтон'),(6374,5681,6367,'Арлингтон-Хейгтс'),(6375,5681,6367,'Аурора'),(6376,5681,6367,'Белвидер'),(6377,5681,6367,'Беллевилл'),(6378,5681,6367,'Бенлд'),(6379,5681,6367,'Бервин'),(6380,5681,6367,'Беталто'),(6381,5681,6367,'Блумингтон'),(6382,5681,6367,'Бондвилл'),(6383,5681,6367,'Букнер'),(6384,5681,6367,'Бурбанк'),(6385,5681,6367,'Валир'),(6386,5681,6367,'Ваукеган'),(6387,5681,6367,'Венеция'),(6388,5681,6367,'Вест Чикаго'),(6389,5681,6367,'Вилдвуд'),(6390,5681,6367,'Вилла-Парк'),(6391,5681,6367,'Вилметт'),(6392,5681,6367,'Винтроп Харбор'),(6393,5681,6367,'Вуд Ривер'),(6394,5681,6367,'Вхитон'),(6395,5681,6367,'Галесбург'),(6396,5681,6367,'Гленвив'),(6397,5681,6367,'Голф'),(6398,5681,6367,'Гранвилл'),(6399,5681,6367,'Гранит-Сити'),(6400,5681,6367,'Грин Рок'),(6401,5681,6367,'Гурни'),(6402,5681,6367,'Д Калб'),(6403,5681,6367,'Далзелл'),(6404,5681,6367,'Данвилл'),(6405,5681,6367,'Даунерс-Гров'),(6406,5681,6367,'Декатур'),(6407,5681,6367,'Дес-Плайнс'),(6408,5681,6367,'Долтон'),(6409,5681,6367,'Еванстон'),(6410,5681,6367,'Евергрин Парк'),(6411,5681,6367,'Елгин'),(6412,5681,6367,'Елмвуд Парк'),(6413,5681,6367,'Елмхурст'),(6414,5681,6367,'Женева'),(6415,5681,6367,'Зейглер'),(6416,5681,6367,'Ист Алтон'),(6417,5681,6367,'Ист Дубукуэ'),(6418,5681,6367,'Ист Молин'),(6419,5681,6367,'Ист Пеориа'),(6420,5681,6367,'Ист Саинт Лоуис'),(6421,5681,6367,'Йолит'),(6422,5681,6367,'Канкаки'),(6423,5681,6367,'Кантон'),(6424,5681,6367,'Карбон Клифф'),(6425,5681,6367,'Кахокиа'),(6426,5681,6367,'Кенилворт'),(6427,5681,6367,'Коал Валли'),(6428,5681,6367,'Колона'),(6429,5681,6367,'Крев Коэур'),(6430,5681,6367,'Куинси'),(6431,5681,6367,'Ла Салл'),(6432,5681,6367,'Ладд'),(6433,5681,6367,'Лак Блуфф'),(6434,5681,6367,'Лансинг'),(6435,5681,6367,'Либертивилл'),(6436,5681,6367,'Линвуд'),(6437,5681,6367,'Линколнвуд'),(6438,5681,6367,'Лионс'),(6439,5681,6367,'Литчфилд'),(6440,5681,6367,'Ловес Парк'),(6441,5681,6367,'Ломбард'),(6442,5681,6367,'Макомб'),(6443,5681,6367,'Марк'),(6444,5681,6367,'Маркуэтт Хейгтс'),(6445,5681,6367,'Маркхам'),(6446,5681,6367,'Маунт-Проспект'),(6447,5681,6367,'Мелрос-Парк'),(6448,5681,6367,'Меррионетт Парк'),(6449,5681,6367,'Мидаубрук'),(6450,5681,6367,'Милан'),(6451,5681,6367,'Молин'),(6452,5681,6367,'Мортон Гров'),(6453,5681,6367,'Мэйвуд'),(6454,5681,6367,'Нилес'),(6455,5681,6367,'Нормал'),(6456,5681,6367,'Норридж'),(6457,5681,6367,'Норт Парк'),(6458,5681,6367,'Норт Риверсид'),(6459,5681,6367,'Норт-Чикаго'),(6460,5681,6367,'Нортбрук'),(6461,5681,6367,'Нортфилд'),(6462,5681,6367,'Оак Лавн'),(6463,5681,6367,'Оак Парк'),(6464,5681,6367,'Оглесби'),(6465,5681,6367,'Олбани'),(6466,5681,6367,'Палатин'),(6467,5681,6367,'Парк Форест'),(6468,5681,6367,'Парк-Ридж'),(6469,5681,6367,'Парк-Сити'),(6470,5681,6367,'Пекин'),(6471,5681,6367,'Пеориа'),(6472,5681,6367,'Перу'),(6473,5681,6367,'Рантоул'),(6474,5681,6367,'Ривер Гров'),(6475,5681,6367,'Ривер Форест'),(6476,5681,6367,'Риверсид'),(6477,5681,6367,'Ривертон'),(6478,5681,6367,'Роббинс'),(6479,5681,6367,'Роиалтон'),(6480,5681,6367,'Рок Исланд'),(6481,5681,6367,'Роксана'),(6482,5681,6367,'Рокфорд'),(6483,5681,6367,'Росевуд Хейгтс'),(6484,5681,6367,'Роскоэ'),(6485,5681,6367,'Савои'),(6486,5681,6367,'Сант-Чарльз'),(6487,5681,6367,'Саугет'),(6488,5681,6367,'Саут-Роксана'),(6489,5681,6367,'Седар Поинт'),(6490,5681,6367,'Силвис'),(6491,5681,6367,'Ситонвилл'),(6492,5681,6367,'Скоки'),(6493,5681,6367,'Спринг Валли'),(6494,5681,6367,'Спрингфилд'),(6495,5681,6367,'Стандард'),(6496,5681,6367,'Стикни'),(6497,5681,6367,'Суммит'),(6498,5681,6367,'Урбаин'),(6499,5681,6367,'Урбана'),(6500,5681,6367,'Форест Вив'),(6501,5681,6367,'Форест Парк'),(6502,5681,6367,'Форест Хомес'),(6503,5681,6367,'Фрипорт'),(6504,5681,6367,'Хазел Крест'),(6505,5681,6367,'Хамптон'),(6506,5681,6367,'Харви'),(6507,5681,6367,'Харвуд Хейгтс'),(6508,5681,6367,'Харрисбург'),(6509,5681,6367,'Хеброн'),(6510,5681,6367,'Хигланд Парк'),(6511,5681,6367,'Холлауэйвилл'),(6512,5681,6367,'Хометаун'),(6513,5681,6367,'Христофер'),(6514,5681,6367,'Чампаигн'),(6515,5681,6367,'Черри'),(6516,5681,6367,'Черри Валли'),(6517,5681,6367,'Чикаго'),(6518,5681,6367,'Чикаго Хейгтс'),(6519,5681,6367,'Чикеро'),(6521,5681,6520,'Алтона'),(6522,5681,6520,'Андерсон'),(6523,5681,6520,'Бич Гров'),(6524,5681,6520,'Блумингтон'),(6525,5681,6520,'Брук'),(6526,5681,6520,'Бурнеттсвилл'),(6527,5681,6520,'Валпараисо'),(6528,5681,6520,'Валтон'),(6529,5681,6520,'Варрен Парк'),(6530,5681,6520,'Виллиамс Крик'),(6531,5681,6520,'Виннедал'),(6532,5681,6520,'Галвестон'),(6533,5681,6520,'Гари'),(6534,5681,6520,'Гарретт'),(6535,5681,6520,'Грабилл'),(6536,5681,6520,'Гриффит'),(6537,5681,6520,'Диер'),(6538,5681,6520,'Дун-Акрес'),(6539,5681,6520,'Евансвилл'),(6540,5681,6520,'Елкхарт'),(6541,5681,6520,'Индианаполис'),(6542,5681,6520,'Ист Чикаго'),(6543,5681,6520,'Кокомо'),(6544,5681,6520,'Колумбус'),(6545,5681,6520,'Коридон'),(6546,5681,6520,'Краун Поинт'),(6547,5681,6520,'Краус Нест'),(6548,5681,6520,'Кумберланд'),(6549,5681,6520,'Лавренк'),(6550,5681,6520,'Лафэйетт'),(6551,5681,6520,'Лейк-Стейшн'),(6552,5681,6520,'Логанспорт'),(6553,5681,6520,'Мадисон'),(6554,5681,6520,'Марион'),(6555,5681,6520,'Меридиан Хиллс'),(6556,5681,6520,'Мерриллвилл'),(6557,5681,6520,'Мичиган-Сити'),(6558,5681,6520,'Мишавака'),(6559,5681,6520,'Мунки'),(6560,5681,6520,'Норт Краус Нест'),(6561,5681,6520,'Норт-Вернон'),(6562,5681,6520,'Нью-Кастл'),(6563,5681,6520,'Нью-Олбани'),(6564,5681,6520,'Нью-Чикаго'),(6565,5681,6520,'Олбани'),(6566,5681,6520,'Онвард'),(6567,5681,6520,'Портаг'),(6568,5681,6520,'Портер'),(6569,5681,6520,'Равенсвуд'),(6570,5681,6520,'Ричмонд'),(6571,5681,6520,'Роки Риппл'),(6572,5681,6520,'Рушвилл'),(6573,5681,6520,'Саут-Бенд'),(6574,5681,6520,'Саут-Хейвен'),(6575,5681,6520,'Скоттсбург'),(6576,5681,6520,'Сулливан'),(6577,5681,6520,'Счерервилл'),(6578,5681,6520,'Терр Хаут'),(6579,5681,6520,'Улен'),(6580,5681,6520,'Форт Вэйн'),(6581,5681,6520,'Хаммонд'),(6582,5681,6520,'Хигланд'),(6583,5681,6520,'Хобарт'),(6584,5681,6520,'Хомекрофт'),(6585,5681,6520,'Хунтертаун'),(6586,5681,6520,'Хунтингбург'),(6587,5681,6520,'Честертон'),(6588,5681,6520,'Чурубуско'),(6589,5681,6520,'Шелбивилл'),(6590,5681,6520,'Элвуд'),(6592,5681,6591,'Азуса'),(6593,5681,6591,'Аламеда'),(6594,5681,6591,'Алондра-Парк'),(6595,5681,6591,'Алтадена'),(6596,5681,6591,'Альгамбра'),(6597,5681,6591,'Анахейм'),(6598,5681,6591,'Антиох'),(6599,5681,6591,'Апленд'),(6600,5681,6591,'Арден'),(6601,5681,6591,'Аркад'),(6602,5681,6591,'Аркадиа'),(6603,5681,6591,'Артесия'),(6604,5681,6591,'Ашланд'),(6605,5681,6591,'Бакерсфилд'),(6606,5681,6591,'Балдвин-Парк'),(6607,5681,6591,'Барбэнк'),(6608,5681,6591,'Барлингейм'),(6609,5681,6591,'Беверли-Хиллс'),(6610,5681,6591,'Белл'),(6611,5681,6591,'Белл-Гарденс'),(6612,5681,6591,'Беллфлауер'),(6613,5681,6591,'Белмонт'),(6614,5681,6591,'Беркли'),(6615,5681,6591,'Блит'),(6616,5681,6591,'Броули'),(6617,5681,6591,'Буэна-Парк'),(6618,5681,6591,'Вакавилл'),(6619,5681,6591,'Валледжо'),(6620,5681,6591,'Валнут-Крик'),(6621,5681,6591,'Валнут-Парк'),(6622,5681,6591,'Вентура'),(6623,5681,6591,'Вернон'),(6624,5681,6591,'Вест-Атенс'),(6625,5681,6591,'Вест-Голливуд'),(6626,5681,6591,'Вест-Карсон'),(6627,5681,6591,'Вест-Ковайн'),(6628,5681,6591,'Вест-Комптон'),(6629,5681,6591,'Вест-Модесто'),(6630,5681,6591,'Вестминстер'),(6631,5681,6591,'Вестмонт'),(6632,5681,6591,'Виндсор-Хиллс'),(6633,5681,6591,'Висалия'),(6634,5681,6591,'Виста'),(6635,5681,6591,'Виттьер'),(6636,5681,6591,'Вудленд'),(6637,5681,6591,'Вью-Парк'),(6638,5681,6591,'Гавайиан-Гарденс'),(6639,5681,6591,'Гарден-Гров'),(6640,5681,6591,'Гардена'),(6641,5681,6591,'Гасиенда-Хейгтс'),(6642,5681,6591,'Глендейл'),(6643,5681,6591,'Глендейл'),(6644,5681,6591,'Глендора'),(6645,5681,6591,'Грахам'),(6646,5681,6591,'Гридли'),(6647,5681,6591,'Дауни'),(6648,5681,6591,'Дейли-Сити'),(6649,5681,6591,'Дели'),(6650,5681,6591,'Дель-Ри'),(6651,5681,6591,'Дель-Эйр'),(6652,5681,6591,'Денаир'),(6653,5681,6591,'Динуба'),(6654,5681,6591,'Дэвис'),(6655,5681,6591,'Империал-Бич'),(6656,5681,6591,'Инглвуд'),(6657,5681,6591,'Ист-Комптон'),(6658,5681,6591,'Ист-Лос-Анжелес'),(6659,5681,6591,'Ист-Пало-Альто'),(6660,5681,6591,'Ист-Портервилл'),(6661,5681,6591,'Истон'),(6662,5681,6591,'Калва'),(6663,5681,6591,'Кампбелл'),(6664,5681,6591,'Кармичел'),(6665,5681,6591,'Карсон'),(6666,5681,6591,'Карутерс'),(6667,5681,6591,'Каситас-Спрингс'),(6668,5681,6591,'Кастро-Велли'),(6669,5681,6591,'Кингсбург'),(6670,5681,6591,'Кипресс'),(6671,5681,6591,'Клермонт'),(6672,5681,6591,'Кловис'),(6673,5681,6591,'Ковайн'),(6674,5681,6591,'Колтон'),(6675,5681,6591,'Коммерц'),(6676,5681,6591,'Комптон'),(6677,5681,6591,'Конкорд'),(6678,5681,6591,'Корона'),(6679,5681,6591,'Коронадо'),(6680,5681,6591,'Коста-Меса'),(6681,5681,6591,'Кулвер-Сити'),(6682,5681,6591,'Ла-Канада'),(6683,5681,6591,'Ла-Крескента'),(6684,5681,6591,'Ла-Меса'),(6685,5681,6591,'Ла-Мирада'),(6686,5681,6591,'Ла-Пальма'),(6687,5681,6591,'Ла-Пуэнте'),(6688,5681,6591,'Ла-Хабра'),(6689,5681,6591,'Ладера-Хейгтс'),(6690,5681,6591,'Ланкастер'),(6691,5681,6591,'Лаундейл'),(6692,5681,6591,'Лафайетт'),(6693,5681,6591,'Лейквуд'),(6694,5681,6591,'Лейтон'),(6695,5681,6591,'Лемон-Гров'),(6696,5681,6591,'Леннокс'),(6697,5681,6591,'Ливермор'),(6698,5681,6591,'Линвуд'),(6699,5681,6591,'Линда'),(6700,5681,6591,'Лоди'),(6701,5681,6591,'Ломпок'),(6702,5681,6591,'Лонг-Бич'),(6703,5681,6591,'Лондон'),(6704,5681,6591,'Лос Аламитос'),(6705,5681,6591,'Лос-Альтос'),(6706,5681,6591,'Лос-Анжелес'),(6707,5681,6591,'Лос-Гатос'),(6708,5681,6591,'Лос-Ньетос'),(6709,5681,6591,'Мантека'),(6710,5681,6591,'Манхаттан-Бич'),(6711,5681,6591,'Марина-Дель-Ри'),(6712,5681,6591,'Марисвилл'),(6713,5681,6591,'Маунт-Хеликс'),(6714,5681,6591,'Менло-Парк'),(6715,5681,6591,'Меркед'),(6716,5681,6591,'Милл-Вэлли'),(6717,5681,6591,'Миллбре'),(6718,5681,6591,'Милпитас'),(6719,5681,6591,'Модесто'),(6720,5681,6591,'Монровиа'),(6721,5681,6591,'Монтебелло'),(6722,5681,6591,'Монтерей'),(6723,5681,6591,'Монтери-Парк'),(6724,5681,6591,'Монтклейр'),(6725,5681,6591,'Монтроз'),(6726,5681,6591,'Моунтайн-Вью'),(6727,5681,6591,'Мэйвуд'),(6728,5681,6591,'Мэйфлауер-Виллидж'),(6729,5681,6591,'Напа'),(6730,5681,6591,'Нешенал-Сити'),(6731,5681,6591,'Новато'),(6732,5681,6591,'Норволк'),(6733,5681,6591,'Норт-Хайлендс'),(6734,5681,6591,'Ньюарк'),(6735,5681,6591,'Ньюпорт-Бич'),(6736,5681,6591,'Ньюхалл'),(6737,5681,6591,'Оакхарст'),(6738,5681,6591,'Ойлдейл'),(6739,5681,6591,'Окланд'),(6740,5681,6591,'Окснард'),(6741,5681,6591,'Олбани'),(6742,5681,6591,'Оливхарст'),(6743,5681,6591,'Онтарио'),(6744,5681,6591,'Оранж'),(6745,5681,6591,'Оушнсайд'),(6746,5681,6591,'Палм-Спрингс'),(6747,5681,6591,'Пало-Альто'),(6748,5681,6591,'Парамоунт'),(6749,5681,6591,'Парлир'),(6750,5681,6591,'Пасадена'),(6751,5681,6591,'Паттерсон'),(6752,5681,6591,'Пацифика'),(6753,5681,6591,'Петалума'),(6754,5681,6591,'Пико-Ривера'),(6755,5681,6591,'Питтсбург'),(6756,5681,6591,'Плакентиа'),(6757,5681,6591,'Плисант-Хилл'),(6758,5681,6591,'Помона'),(6759,5681,6591,'Ранчо-Кордова'),(6760,5681,6591,'Редвуд-Сити'),(6761,5681,6591,'Реддинг'),(6762,5681,6591,'Редландс'),(6763,5681,6591,'Редондо-Бич'),(6764,5681,6591,'Риалто'),(6765,5681,6591,'Ривербанк'),(6766,5681,6591,'Риверсайд'),(6767,5681,6591,'Риджкрест'),(6768,5681,6591,'Ридли'),(6769,5681,6591,'Рипон'),(6770,5681,6591,'Ричмонд'),(6771,5681,6591,'Росемид'),(6772,5681,6591,'Россмур'),(6773,5681,6591,'Сакраменто'),(6774,5681,6591,'Салинас'),(6775,5681,6591,'Сан-Андрис'),(6776,5681,6591,'Сан-Бернардино'),(6777,5681,6591,'Сан-Бруно'),(6778,5681,6591,'Сан-Габриэль'),(6779,5681,6591,'Сан-Диего'),(6780,5681,6591,'Сан-Карлос'),(6781,5681,6591,'Сан-Линдро'),(6782,5681,6591,'Сан-Лоренцо'),(6783,5681,6591,'Сан-Луис-Обиспо'),(6784,5681,6591,'Сан-Марино'),(6785,5681,6591,'Сан-Матео'),(6786,5681,6591,'Сан-Рафель'),(6787,5681,6591,'Сан-Фернандо'),(6788,5681,6591,'Сан-Франциско'),(6789,5681,6591,'Сан-Хосе'),(6790,5681,6591,'Сангер'),(6791,5681,6591,'Саннивейл'),(6792,5681,6591,'Санта-Ана'),(6793,5681,6591,'Санта-Барбара'),(6794,5681,6591,'Санта-Клара'),(6795,5681,6591,'Санта-Круз'),(6796,5681,6591,'Санта-Мария'),(6797,5681,6591,'Санта-Моника'),(6798,5681,6591,'Санта-Роза'),(6799,5681,6591,'Санта-Фе-Спрингс'),(6800,5681,6591,'Санти'),(6801,5681,6591,'Саратога'),(6802,5681,6591,'Саугус'),(6803,5681,6591,'Саус-Модесто'),(6804,5681,6591,'Саут-Виттьер'),(6805,5681,6591,'Саут-Гейт'),(6806,5681,6591,'Саут-Ель-Монт'),(6807,5681,6591,'Саут-Лейк-Тахо'),(6808,5681,6591,'Саут-Пасадена'),(6809,5681,6591,'Саут-Сан-Габриэль'),(6810,5681,6591,'Саут-Сан-Франциско'),(6811,5681,6591,'Саут-Юба'),(6812,5681,6591,'Селма'),(6813,5681,6591,'Серритос'),(6814,5681,6591,'Сигнал-Хилл'),(6815,5681,6591,'Сил-Бич'),(6816,5681,6591,'Сими'),(6817,5681,6591,'Сисайд'),(6818,5681,6591,'Спринг-Вэлли'),(6819,5681,6591,'Стантон'),(6820,5681,6591,'Стоктон'),(6821,5681,6591,'Сусалито'),(6822,5681,6591,'Сьерра-Мадре'),(6823,5681,6591,'Талмаг'),(6824,5681,6591,'Тамалпаис-Вэлли'),(6825,5681,6591,'Тарлок'),(6826,5681,6591,'Тастин'),(6827,5681,6591,'Темпл-Сити'),(6828,5681,6591,'Тибурон'),(6829,5681,6591,'Тиерра-Буэна'),(6830,5681,6591,'Торранц'),(6831,5681,6591,'Тоусанд-Оакс'),(6832,5681,6591,'Укия'),(6833,5681,6591,'Файрфилд'),(6834,5681,6591,'Фаулер'),(6835,5681,6591,'Фаунтайн-Вэлли'),(6836,5681,6591,'Флинтридж'),(6837,5681,6591,'Флоренц'),(6838,5681,6591,'Фонтана'),(6839,5681,6591,'Фремонт'),(6840,5681,6591,'Фресно'),(6841,5681,6591,'Фуллертон'),(6842,5681,6591,'Хавторн'),(6843,5681,6591,'Хагсон'),(6844,5681,6591,'Хантингтон-Бич'),(6845,5681,6591,'Хантингтон-Парк'),(6846,5681,6591,'Хебер'),(6847,5681,6591,'Хейвард'),(6848,5681,6591,'Холтвилл'),(6849,5681,6591,'Хоумстид-Вэлли'),(6850,5681,6591,'Церес'),(6851,5681,6591,'Цитрус-Хейгтс'),(6852,5681,6591,'Черриленд'),(6853,5681,6591,'Чино'),(6854,5681,6591,'Чула-Виста'),(6855,5681,6591,'Эль-Кайон'),(6856,5681,6591,'Эль-Монт'),(6857,5681,6591,'Эль-Сегундо'),(6858,5681,6591,'Эль-Серрито'),(6859,5681,6591,'Эмеривилл'),(6860,5681,6591,'Эскалон'),(6861,5681,6591,'Эскондидо'),(6862,5681,6591,'Эурека'),(6863,5681,6591,'Юба-Сити'),(6865,5681,6864,'Андовер'),(6866,5681,6864,'Бентон'),(6867,5681,6864,'Вествуд'),(6868,5681,6864,'Вествуд-Хиллс'),(6869,5681,6864,'Винфилд'),(6870,5681,6864,'Вичита'),(6871,5681,6864,'Вэлли-Сентер'),(6872,5681,6864,'Грейт-Бенд'),(6873,5681,6864,'Джанкшин-Сити'),(6874,5681,6864,'Додж-Сити'),(6875,5681,6864,'Индепенденс'),(6876,5681,6864,'Истборо'),(6877,5681,6864,'Канзас-Сити'),(6878,5681,6864,'Кантрисайд'),(6879,5681,6864,'Карбондал'),(6880,5681,6864,'Кечи'),(6881,5681,6864,'Колби'),(6882,5681,6864,'Колвич'),(6883,5681,6864,'Конкордиа'),(6884,5681,6864,'Ларнед'),(6885,5681,6864,'Лекомптон'),(6886,5681,6864,'Ливенворт'),(6887,5681,6864,'Лоуренс'),(6888,5681,6864,'Манхаттан'),(6889,5681,6864,'Мерриам'),(6890,5681,6864,'Миссион'),(6891,5681,6864,'Миссион-Вудс'),(6892,5681,6864,'Миссион-Хиллс'),(6893,5681,6864,'Нортон'),(6894,5681,6864,'Оакли'),(6895,5681,6864,'Обурн'),(6896,5681,6864,'Овербрук'),(6897,5681,6864,'Оверленд-Парк'),(6898,5681,6864,'Огден'),(6899,5681,6864,'Палмер'),(6900,5681,6864,'Парк-Сити'),(6901,5681,6864,'Парсонс'),(6902,5681,6864,'Перри'),(6903,5681,6864,'Прейри-Виллидж'),(6904,5681,6864,'Роланд-Парк'),(6905,5681,6864,'Рос-Хилл'),(6906,5681,6864,'Салина'),(6907,5681,6864,'Силвер-Лейк'),(6908,5681,6864,'Скрантон'),(6909,5681,6864,'Топика'),(6910,5681,6864,'Файрвэй'),(6911,5681,6864,'Форт-Райли'),(6912,5681,6864,'Фронтенак'),(6913,5681,6864,'Хатчинсон'),(6914,5681,6864,'Хэйс'),(6915,5681,6864,'Чанут'),(6916,5681,6864,'Эмпориа'),(6918,5681,6917,'Адубон-Парк'),(6919,5681,6917,'Ашланд'),(6920,5681,6917,'Баулинг Грин'),(6921,5681,6917,'Беллевуэ'),(6922,5681,6917,'Беллефонт'),(6923,5681,6917,'Валлинс-Крик'),(6924,5681,6917,'Версаиллес'),(6925,5681,6917,'Вествуд'),(6926,5681,6917,'Вилмор'),(6927,5681,6917,'Вэйланд'),(6928,5681,6917,'Гутри'),(6929,5681,6917,'Джорджтаун'),(6930,5681,6917,'Дэйтон'),(6931,5681,6917,'Еминенк'),(6932,5681,6917,'Катлеттсбург'),(6933,5681,6917,'Кентон-Вейл'),(6934,5681,6917,'Кингсли'),(6935,5681,6917,'Ковингтон'),(6936,5681,6917,'Ла Фэйетт'),(6937,5681,6917,'Лексингтон'),(6938,5681,6917,'Линнвив'),(6939,5681,6917,'Лондон'),(6940,5681,6917,'Лоуисвилл'),(6941,5681,6917,'Лудлау'),(6942,5681,6917,'Маунт-Стерлинг'),(6943,5681,6917,'Николасвиль'),(6944,5681,6917,'Ньюпорт'),(6945,5681,6917,'Овенсборо'),(6946,5681,6917,'Олбани'),(6947,5681,6917,'Падуках'),(6948,5681,6917,'Парис'),(6949,5681,6917,'Парк-Хиллс'),(6950,5681,6917,'Парквэй-Виллидж'),(6951,5681,6917,'Певи Валли'),(6952,5681,6917,'Пемброк'),(6953,5681,6917,'Ракеланд'),(6954,5681,6917,'Русселл'),(6955,5681,6917,'Саут-Шор'),(6956,5681,6917,'Саутгейт'),(6957,5681,6917,'Сенека-Гарденс'),(6958,5681,6917,'Стампинг-Граунд'),(6959,5681,6917,'Стратмур-Гарденс'),(6960,5681,6917,'Стратмур-Манор'),(6961,5681,6917,'Трентон'),(6962,5681,6917,'Флатвудс'),(6963,5681,6917,'Форт Кампбелл Норт'),(6964,5681,6917,'Форт-Вригт'),(6965,5681,6917,'Форт-Митчелл'),(6966,5681,6917,'Форт-Нокс'),(6967,5681,6917,'Форт-Томас'),(6968,5681,6917,'Франкфорт'),(6969,5681,6917,'Хиден'),(6970,5681,6917,'Хопкинсвилл'),(6971,5681,6917,'Хорс-Кейв'),(6972,5681,6917,'Шивели'),(6974,5681,6973,'Айдахо-Спрингс'),(6975,5681,6973,'Арвада'),(6976,5681,6973,'Аурора'),(6977,5681,6973,'Блэк-Форест'),(6978,5681,6973,'Боулдер'),(6979,5681,6973,'Браш'),(6980,5681,6973,'Велби'),(6981,5681,6973,'Вестминстер'),(6982,5681,6973,'Вет-Ридж'),(6983,5681,6973,'Вудленд-Парк'),(6984,5681,6973,'Гленвуд-Спрингс'),(6985,5681,6973,'Гранд-Джанкшин'),(6986,5681,6973,'Грили'),(6987,5681,6973,'Грин-Маунтайн-Фоллс'),(6988,5681,6973,'Гринвуд-Виллидж'),(6989,5681,6973,'Денвер'),(6990,5681,6973,'Дерби'),(6991,5681,6973,'Диллон'),(6992,5681,6973,'Дуранго'),(6993,5681,6973,'Инглевуд'),(6994,5681,6973,'Колорадо-Спрингс'),(6995,5681,6973,'Коммерц-Сити'),(6996,5681,6973,'Крайг'),(6997,5681,6973,'Лас-Анимас'),(6998,5681,6973,'Лейквуд'),(6999,5681,6973,'Лейксайд'),(7000,5681,6973,'Литтлетон'),(7001,5681,6973,'Манитоу-Спрингс'),(7002,5681,6973,'Нанн'),(7003,5681,6973,'Нортгленн'),(7004,5681,6973,'Пуэбло'),(7005,5681,6973,'Саутгленн'),(7006,5681,6973,'Свинк'),(7007,5681,6973,'Секьюрити'),(7008,5681,6973,'Форт-Карсон'),(7009,5681,6973,'Форт-Коллинс'),(7010,5681,6973,'Черри-Хиллс-Виллидж'),(7011,5681,6973,'Шеридан'),(7012,5681,6973,'Шеррелвуд'),(7013,5681,6973,'Эджуотер'),(7014,5681,6973,'Эйр-Форс-Академи'),(7016,5681,7015,'Берлин'),(7017,5681,7015,'Бриджпорт'),(7018,5681,7015,'Бристоль'),(7019,5681,7015,'Валлингфорд'),(7020,5681,7015,'Вернон'),(7021,5681,7015,'Вест-Хавен'),(7022,5681,7015,'Вест-Хартфорд'),(7023,5681,7015,'Вестпорт'),(7024,5681,7015,'Ветерсфилд'),(7025,5681,7015,'Гластонбури'),(7026,5681,7015,'Гринвич'),(7027,5681,7015,'Данбури'),(7028,5681,7015,'Ист-Хавен'),(7029,5681,7015,'Ист-Хартфорд'),(7030,5681,7015,'Кенсингтон'),(7031,5681,7015,'Куинбаг'),(7032,5681,7015,'Манчестер'),(7033,5681,7015,'Мериден'),(7034,5681,7015,'Миддлетаун'),(7035,5681,7015,'Милфорд'),(7036,5681,7015,'Невингтон'),(7037,5681,7015,'Норвич'),(7038,5681,7015,'Норволк'),(7039,5681,7015,'Норт-Гросвенор-Дейл'),(7040,5681,7015,'Нью-Бритайн'),(7041,5681,7015,'Нью-Лондон'),(7042,5681,7015,'Нью-Хейвен'),(7043,5681,7015,'Патнам'),(7044,5681,7015,'Роки-Хилл'),(7045,5681,7015,'Стамфорд'),(7046,5681,7015,'Стратфорд'),(7047,5681,7015,'Торрингтон'),(7048,5681,7015,'Трамбалл'),(7049,5681,7015,'Уотербури'),(7050,5681,7015,'Файрфилд'),(7051,5681,7015,'Фармингтон'),(7052,5681,7015,'Хамден'),(7053,5681,7015,'Хартфорд'),(7054,5681,7015,'Шелтон'),(7056,5681,7055,'Абита-Спрингс'),(7057,5681,7055,'Александрия'),(7058,5681,7055,'Анги'),(7059,5681,7055,'Балл'),(7060,5681,7055,'Батон-Руж'),(7061,5681,7055,'Бейкер'),(7062,5681,7055,'Бентон'),(7063,5681,7055,'Бланчард'),(7064,5681,7055,'Богалуса'),(7065,5681,7055,'Боссир-Сити'),(7066,5681,7055,'Варнадо'),(7067,5681,7055,'Велш'),(7068,5681,7055,'Видалиа'),(7069,5681,7055,'Вильсон'),(7070,5681,7055,'Вудворт'),(7071,5681,7055,'Гонзалес'),(7072,5681,7055,'Грамблинг'),(7073,5681,7055,'Де-Риддер'),(7074,5681,7055,'Денхам-Спрингс'),(7075,5681,7055,'Джексон'),(7076,5681,7055,'Дусон'),(7077,5681,7055,'Зачари'),(7078,5681,7055,'Канктон'),(7079,5681,7055,'Карвилл'),(7080,5681,7055,'Кеннер'),(7081,5681,7055,'Клейтон'),(7082,5681,7055,'Ковингтон'),(7083,5681,7055,'Коттон-Вэлли'),(7084,5681,7055,'Краули'),(7085,5681,7055,'Куллен'),(7086,5681,7055,'Лафайетт'),(7087,5681,7055,'Лейк-Чарльз'),(7088,5681,7055,'Лекомпт'),(7089,5681,7055,'Лисвилл'),(7090,5681,7055,'Марреро'),(7091,5681,7055,'Мерривилл'),(7092,5681,7055,'Метаири'),(7093,5681,7055,'Монро'),(7094,5681,7055,'Морган-Сити'),(7095,5681,7055,'Мосс-Блуфф'),(7096,5681,7055,'Новый Орлеан'),(7097,5681,7055,'Нью-Ибериа'),(7098,5681,7055,'Олбани'),(7099,5681,7055,'Пайнвилл'),(7100,5681,7055,'Пирл Ривер'),(7101,5681,7055,'Порт-Аллен'),(7102,5681,7055,'Рэйн'),(7103,5681,7055,'Сансет'),(7104,5681,7055,'Сарепта'),(7105,5681,7055,'Скотландвилл'),(7106,5681,7055,'Скотт'),(7107,5681,7055,'Слаутер'),(7108,5681,7055,'Спрингилл'),(7109,5681,7055,'Стоунволл'),(7110,5681,7055,'Сун'),(7111,5681,7055,'Урания'),(7112,5681,7055,'Ферридэй'),(7113,5681,7055,'Франклинтон'),(7114,5681,7055,'Хаугтон'),(7115,5681,7055,'Хоума'),(7116,5681,7055,'Хэйнесвилл'),(7117,5681,7055,'Чёрч-Пойнт'),(7118,5681,7055,'Шонгалу'),(7119,5681,7055,'Шривпорт'),(7121,5681,7120,'Айер'),(7122,5681,7120,'Актон'),(7123,5681,7120,'Андовер'),(7124,5681,7120,'Арлингтон'),(7125,5681,7120,'Атол'),(7126,5681,7120,'Аттлеборо'),(7127,5681,7120,'Аубурн'),(7128,5681,7120,'Беверли'),(7129,5681,7120,'Белмонт'),(7130,5681,7120,'Бостон'),(7131,5681,7120,'Боурн'),(7132,5681,7120,'Брайнтри'),(7133,5681,7120,'Бриджуотер'),(7134,5681,7120,'Броктон'),(7135,5681,7120,'Бруклин'),(7136,5681,7120,'Валтам'),(7137,5681,7120,'Варехам'),(7138,5681,7120,'Вейкфилд'),(7139,5681,7120,'Веллесли'),(7140,5681,7120,'Вест-Бойлстон'),(7141,5681,7120,'Вест-Бриджуотер'),(7142,5681,7120,'Вест-Варехам'),(7143,5681,7120,'Вест-Спрингфилд'),(7144,5681,7120,'Вестборо'),(7145,5681,7120,'Вестон'),(7146,5681,7120,'Вестфилд'),(7147,5681,7120,'Вимоут'),(7148,5681,7120,'Винтроп'),(7149,5681,7120,'Винчестер'),(7150,5681,7120,'Вобурн'),(7151,5681,7120,'Ворчестер'),(7152,5681,7120,'Врентам'),(7153,5681,7120,'Глочестер'),(7154,5681,7120,'Графтон'),(7155,5681,7120,'Гринфилд'),(7156,5681,7120,'Дадли'),(7157,5681,7120,'Данверс'),(7158,5681,7120,'Дедхам'),(7159,5681,7120,'Дракут'),(7160,5681,7120,'Ист-Бриджуотер'),(7161,5681,7120,'Ист-Лонгмидоу'),(7162,5681,7120,'Карвер'),(7163,5681,7120,'Кембридж'),(7164,5681,7120,'Клинтон'),(7165,5681,7120,'Куинси'),(7166,5681,7120,'Лейкестер'),(7167,5681,7120,'Лексингтон'),(7168,5681,7120,'Леоминстер'),(7169,5681,7120,'Линн'),(7170,5681,7120,'Линнфилд'),(7171,5681,7120,'Ловелл'),(7172,5681,7120,'Лоуренс'),(7173,5681,7120,'Малден'),(7174,5681,7120,'Марлборо'),(7175,5681,7120,'Медфорд'),(7176,5681,7120,'Мелроз'),(7177,5681,7120,'Метуэн'),(7178,5681,7120,'Миллбури'),(7179,5681,7120,'Милтон'),(7180,5681,7120,'Натик'),(7181,5681,7120,'Нахант'),(7182,5681,7120,'Нидхам'),(7183,5681,7120,'Норвуд'),(7184,5681,7120,'Норт-Андовер'),(7185,5681,7120,'Норт-Дигтон'),(7186,5681,7120,'Нортамптон'),(7187,5681,7120,'Нортборо'),(7188,5681,7120,'Нью-Бедфорд'),(7189,5681,7120,'Ньютон'),(7190,5681,7120,'Оксфорд'),(7191,5681,7120,'Оксфорд-Сентер'),(7192,5681,7120,'Пибоди'),(7193,5681,7120,'Питтсфилд'),(7194,5681,7120,'Ратланд'),(7195,5681,7120,'Ревер'),(7196,5681,7120,'Рехобот'),(7197,5681,7120,'Ридинг'),(7198,5681,7120,'Рошдейл'),(7199,5681,7120,'Рэндольф'),(7200,5681,7120,'Сагамор'),(7201,5681,7120,'Салем'),(7202,5681,7120,'Сандвич'),(7203,5681,7120,'Саттон'),(7204,5681,7120,'Саугус'),(7205,5681,7120,'Саут-Ланкастер'),(7206,5681,7120,'Свампскотт'),(7207,5681,7120,'Сомервилл'),(7208,5681,7120,'Сомерсет'),(7209,5681,7120,'Спенсер'),(7210,5681,7120,'Спрингфилд'),(7211,5681,7120,'Стерлинг'),(7212,5681,7120,'Стоунхам'),(7213,5681,7120,'Таунтон'),(7214,5681,7120,'Три-Риверс'),(7215,5681,7120,'Тьюксбури'),(7216,5681,7120,'Уотертаун'),(7217,5681,7120,'Фитчбург'),(7218,5681,7120,'Фолл-Ривер'),(7219,5681,7120,'Фрамингам'),(7220,5681,7120,'Фритаун'),(7221,5681,7120,'Хаверхилл'),(7222,5681,7120,'Холден'),(7223,5681,7120,'Холиок'),(7224,5681,7120,'Челси'),(7225,5681,7120,'Чикопи'),(7226,5681,7120,'Шроусбури'),(7227,5681,7120,'Эверетт'),(7229,5681,7228,'Бейпорт'),(7230,5681,7228,'Бирон'),(7231,5681,7228,'Блумингтон'),(7232,5681,7228,'Брайнерд'),(7233,5681,7228,'Браунсвилл'),(7234,5681,7228,'Бруклин-Парк'),(7235,5681,7228,'Бруклин-Сентер'),(7236,5681,7228,'Валкер'),(7237,5681,7228,'Вест-Сант-Пол'),(7238,5681,7228,'Виллмар'),(7239,5681,7228,'Винона'),(7240,5681,7228,'Вреншалл'),(7241,5681,7228,'Германтаун'),(7242,5681,7228,'Голден-Вэлли'),(7243,5681,7228,'Дакота'),(7244,5681,7228,'Дулут'),(7245,5681,7228,'Каннон-Фоллс'),(7246,5681,7228,'Карлтон'),(7247,5681,7228,'Клокуэт'),(7248,5681,7228,'Колумбия-Хейгтс'),(7249,5681,7228,'Кристал'),(7250,5681,7228,'Кун-Рапидс'),(7251,5681,7228,'Лаудердейл'),(7252,5681,7228,'Лилидейл'),(7253,5681,7228,'Литтл-Фоллс'),(7254,5681,7228,'Манкато'),(7255,5681,7228,'Маплвуд'),(7256,5681,7228,'Медисин-Лейк'),(7257,5681,7228,'Мендота'),(7258,5681,7228,'Мендота-Хейгтс'),(7259,5681,7228,'Миннеаполис'),(7260,5681,7228,'Миннетонка'),(7261,5681,7228,'Мурхид'),(7262,5681,7228,'Норт Манкато'),(7263,5681,7228,'Нью-Брайтон'),(7264,5681,7228,'Олбани'),(7265,5681,7228,'Остин'),(7266,5681,7228,'Проктор'),(7267,5681,7228,'Ричфилд'),(7268,5681,7228,'Роббинсдейл'),(7269,5681,7228,'Росевилл'),(7270,5681,7228,'Рочестер'),(7271,5681,7228,'Сант-Антони'),(7272,5681,7228,'Сант-Клауд'),(7273,5681,7228,'Сант-Пол'),(7274,5681,7228,'Сент-Луис-Парк'),(7275,5681,7228,'Сканлон'),(7276,5681,7228,'Скилин'),(7277,5681,7228,'Стефен'),(7278,5681,7228,'Стиллуотер'),(7279,5681,7228,'Томсон'),(7280,5681,7228,'Фалкон-Хейгтс'),(7281,5681,7228,'Фармингтон'),(7282,5681,7228,'Фергус-Фоллс'),(7283,5681,7228,'Фридли'),(7284,5681,7228,'Хиллтоп'),(7285,5681,7228,'Хока'),(7286,5681,7228,'Эдина'),(7287,5681,7228,'Эйота'),(7289,5681,7288,'Абердин'),(7290,5681,7288,'Аккерман'),(7291,5681,7288,'Амори'),(7292,5681,7288,'Балдвин'),(7293,5681,7288,'Бассфилд'),(7294,5681,7288,'Батесвилл'),(7295,5681,7288,'Билокси'),(7296,5681,7288,'Боил'),(7297,5681,7288,'Брандон'),(7298,5681,7288,'Брукхавен'),(7299,5681,7288,'Буневилл'),(7300,5681,7288,'Бэй Спрингс'),(7301,5681,7288,'Ватер Валли'),(7302,5681,7288,'Вейр'),(7303,5681,7288,'Верона'),(7304,5681,7288,'Вест'),(7305,5681,7288,'Вест Поинт'),(7306,5681,7288,'Виксбург'),(7307,5681,7288,'Гаттман'),(7308,5681,7288,'Глендал'),(7309,5681,7288,'Глендора'),(7310,5681,7288,'Гренада'),(7311,5681,7288,'Гринвилл'),(7312,5681,7288,'Гудман'),(7313,5681,7288,'Гулф Хиллс'),(7314,5681,7288,'Гулфпорт'),(7315,5681,7288,'Д`Ибервилл'),(7316,5681,7288,'Декатур'),(7317,5681,7288,'Джексон'),(7318,5681,7288,'Доддсвилл'),(7319,5681,7288,'Древ'),(7320,5681,7288,'Дурант'),(7321,5681,7288,'Еллисвилл'),(7322,5681,7288,'Индианола'),(7323,5681,7288,'Каледониа'),(7324,5681,7288,'Кингс'),(7325,5681,7288,'Клевеланд'),(7326,5681,7288,'Клинтон'),(7327,5681,7288,'Коллинс'),(7328,5681,7288,'Колумбиа'),(7329,5681,7288,'Колумбус'),(7330,5681,7288,'Коринт'),(7331,5681,7288,'Коссут'),(7332,5681,7288,'Коуртланд'),(7333,5681,7288,'Куитман'),(7334,5681,7288,'Лак'),(7335,5681,7288,'Лаурел'),(7336,5681,7288,'Лонг Бич'),(7337,5681,7288,'Лоуин'),(7338,5681,7288,'Лула'),(7339,5681,7288,'Лумбертон'),(7340,5681,7288,'Мадисон'),(7341,5681,7288,'Марион'),(7342,5681,7288,'Мериголд'),(7343,5681,7288,'Меридиан'),(7344,5681,7288,'Миз'),(7345,5681,7288,'МкКул'),(7346,5681,7288,'Монтикелло'),(7347,5681,7288,'Монтрос'),(7348,5681,7288,'Моунд Бэйоу'),(7349,5681,7288,'Натчес'),(7350,5681,7288,'Неллибург'),(7351,5681,7288,'Неттлетон'),(7352,5681,7288,'Норт Гулфпорт'),(7353,5681,7288,'Нью-Олбани'),(7354,5681,7288,'Ньютон'),(7355,5681,7288,'Окин Спрингс'),(7356,5681,7288,'Околона'),(7357,5681,7288,'Олив Бранч'),(7358,5681,7288,'Оранг Гров'),(7359,5681,7288,'Палмерс Кроссинг'),(7360,5681,7288,'Паскагоула'),(7361,5681,7288,'Пасс Чристиан'),(7362,5681,7288,'Паулдинг'),(7363,5681,7288,'Пачута'),(7364,5681,7288,'Пелахатчи'),(7365,5681,7288,'Петал'),(7366,5681,7288,'Пикэйун'),(7367,5681,7288,'Пирл'),(7368,5681,7288,'Пирл-Сити'),(7369,5681,7288,'Плантерсвилл'),(7370,5681,7288,'Поп'),(7371,5681,7288,'Попларвилл'),(7372,5681,7288,'Пурвис'),(7373,5681,7288,'Ралейг'),(7374,5681,7288,'Ренова'),(7375,5681,7288,'Риджеланд'),(7376,5681,7288,'Ринзи'),(7377,5681,7288,'Роллинг-Форк'),(7378,5681,7288,'Рулевилл'),(7379,5681,7288,'Саллис'),(7380,5681,7288,'Салтилло'),(7381,5681,7288,'Сандерсвилл'),(7382,5681,7288,'Саутхейвен'),(7383,5681,7288,'Себастопол'),(7384,5681,7288,'Семинари'),(7385,5681,7288,'Силварена'),(7386,5681,7288,'Силвер-Крик'),(7387,5681,7288,'Смитвилл'),(7388,5681,7288,'Сосо'),(7389,5681,7288,'Старквилл'),(7390,5681,7288,'Суммит'),(7391,5681,7288,'Сумнер'),(7392,5681,7288,'Сумралл'),(7393,5681,7288,'Тилертаун'),(7394,5681,7288,'Тупело'),(7395,5681,7288,'Тутвилер'),(7396,5681,7288,'Флаууд'),(7397,5681,7288,'Флоренк'),(7398,5681,7288,'Форест'),(7399,5681,7288,'Френч Камп'),(7400,5681,7288,'Хармони'),(7401,5681,7288,'Хатли'),(7402,5681,7288,'Хаттисбург'),(7403,5681,7288,'Хендерсон Поинт'),(7404,5681,7288,'Хернандо'),(7405,5681,7288,'Хикори'),(7406,5681,7288,'Хьюстон'),(7407,5681,7288,'Чунки'),(7408,5681,7288,'Шав'),(7409,5681,7288,'Шаннон'),(7410,5681,7288,'Юмпертаун'),(7412,5681,7411,'Авондейл'),(7413,5681,7411,'Бисмарк'),(7414,5681,7411,'Бонн Терр'),(7415,5681,7411,'Брансон'),(7416,5681,7411,'Брентвуд'),(7417,5681,7411,'Варсон Вудс'),(7418,5681,7411,'Вебстер Гровес'),(7419,5681,7411,'Велда Виллидж Хиллс'),(7420,5681,7411,'Веллстон'),(7421,5681,7411,'Вест-Плайнс'),(7422,5681,7411,'Гриндал'),(7423,5681,7411,'Дес Перес'),(7424,5681,7411,'Деслог'),(7425,5681,7411,'Джефферсон-Сити'),(7426,5681,7411,'Джоплин'),(7427,5681,7411,'Диксон'),(7428,5681,7411,'Дулиттл'),(7429,5681,7411,'Елвинс'),(7430,5681,7411,'Естер'),(7431,5681,7411,'Индепенденс'),(7432,5681,7411,'Ирондал'),(7433,5681,7411,'Камдентон'),(7434,5681,7411,'Канзас-Сити'),(7435,5681,7411,'Кап Гирардиу'),(7436,5681,7411,'Кирквуд'),(7437,5681,7411,'Кирксвилл'),(7438,5681,7411,'Клэйтон'),(7439,5681,7411,'Колумбия'),(7440,5681,7411,'Кристал Лак Парк'),(7441,5681,7411,'Лемэй'),(7442,5681,7411,'Лидвуд'),(7443,5681,7411,'Лидингтон'),(7444,5681,7411,'Макензи'),(7445,5681,7411,'Маплевуд'),(7446,5681,7411,'Маршалл'),(7447,5681,7411,'Маундвилл'),(7448,5681,7411,'Метц'),(7449,5681,7411,'Невада'),(7450,5681,7411,'Норт-Канзас-Сити'),(7451,5681,7411,'Нортви'),(7452,5681,7411,'Нортвудс'),(7453,5681,7411,'Нортмур'),(7454,5681,7411,'Нью-Блумфилд'),(7455,5681,7411,'Ньюбург'),(7456,5681,7411,'Окленд-Парк'),(7457,5681,7411,'Олбани (Генри Кантри)'),(7458,5681,7411,'Олбани (Рэй Кантри)'),(7459,5681,7411,'Олбани-Джанкшн'),(7460,5681,7411,'Пагедал'),(7461,5681,7411,'Пакифик'),(7462,5681,7411,'Пилот Кноб'),(7463,5681,7411,'Пин Лавн'),(7464,5681,7411,'Риверминес'),(7465,5681,7411,'Ричмонд Хейгтс'),(7466,5681,7411,'Ролла'),(7467,5681,7411,'Рэйтаун'),(7468,5681,7411,'Салем'),(7469,5681,7411,'Сант-Чарльз'),(7470,5681,7411,'Седалиа'),(7471,5681,7411,'Седар-Сити'),(7472,5681,7411,'Сент-Джозеф'),(7473,5681,7411,'Сент-Луис'),(7474,5681,7411,'Спаниш Лак'),(7475,5681,7411,'Спрингфилд'),(7476,5681,7411,'Упландс Парк'),(7477,5681,7411,'Фаирвив Акрес'),(7478,5681,7411,'Фармингтон'),(7479,5681,7411,'Фергусон'),(7480,5681,7411,'Флат Ривер'),(7481,5681,7411,'Флориссант'),(7482,5681,7411,'Харвуд'),(7483,5681,7411,'Харрисбург'),(7484,5681,7411,'Хартсбург'),(7485,5681,7411,'Хигли Хейгтс'),(7486,5681,7411,'Хиллсдал'),(7487,5681,7411,'Хунтлейг'),(7488,5681,7411,'Шревсбури'),(7489,5681,7411,'Эдгар-Спрингс'),(7490,5681,7411,'Эйрпорт-Драйв'),(7491,5681,7411,'Эшланд'),(7492,5681,7411,'Юниверсити-Сити'),(7494,5681,7493,'Аллен-Парк'),(7495,5681,7493,'Бартон-Хиллс'),(7496,5681,7493,'Баттл Крик'),(7497,5681,7493,'Беллаир'),(7498,5681,7493,'Бессемер'),(7499,5681,7493,'Биг Рапидс'),(7500,5681,7493,'Бингам-Фармс'),(7501,5681,7493,'Бирмингам'),(7502,5681,7493,'Бирч-Ран'),(7503,5681,7493,'Бичер'),(7504,5681,7493,'Бойн-Сити'),(7505,5681,7493,'Бойн-Фоллс'),(7506,5681,7493,'Бэй-Сити'),(7507,5681,7493,'Валкер'),(7508,5681,7493,'Варрен'),(7509,5681,7493,'Вестланд'),(7510,5681,7493,'Виандотт'),(7511,5681,7493,'Виоминг'),(7512,5681,7493,'Волф Лак'),(7513,5681,7493,'Вэйкфилд'),(7514,5681,7493,'Галесбург'),(7515,5681,7493,'Гарден-Сити'),(7516,5681,7493,'Гранд-Бланк'),(7517,5681,7493,'Гранд-Рапидс'),(7518,5681,7493,'Гросс-Пойнт-Парк'),(7519,5681,7493,'Гудрич'),(7520,5681,7493,'Дависон'),(7521,5681,7493,'Детройт'),(7522,5681,7493,'Джексон'),(7523,5681,7493,'Дирборн'),(7524,5681,7493,'Дирборн-Хейгтс'),(7525,5681,7493,'Екорс'),(7526,5681,7493,'Есканаба'),(7527,5681,7493,'Инкстер'),(7528,5681,7493,'Иониа'),(7529,5681,7493,'Ипсиланти'),(7530,5681,7493,'Ист Йордан'),(7531,5681,7493,'Ист-Гранд-Рапидс'),(7532,5681,7493,'Ист-Детройт'),(7533,5681,7493,'Ист-Лансинг'),(7534,5681,7493,'Иствуд'),(7535,5681,7493,'Кадиллак'),(7536,5681,7493,'Каламазу'),(7537,5681,7493,'Каро'),(7538,5681,7493,'Кентвуд'),(7539,5681,7493,'Климакс'),(7540,5681,7493,'Клинтон'),(7541,5681,7493,'Колдватер'),(7542,5681,7493,'Кутлервилл'),(7543,5681,7493,'Ламбертвилл'),(7544,5681,7493,'Лансинг'),(7545,5681,7493,'Лейк-Анжелус'),(7546,5681,7493,'Ливониа'),(7547,5681,7493,'Линкольн-Парк'),(7548,5681,7493,'Луна-Пир'),(7549,5681,7493,'Мадисон-Хейтс'),(7550,5681,7493,'Маркуэтт'),(7551,5681,7493,'Маунт-Моррис'),(7552,5681,7493,'Мелвиндейл'),(7553,5681,7493,'Меномини'),(7554,5681,7493,'Мидланд'),(7555,5681,7493,'Монтроз'),(7556,5681,7493,'Мунисинг'),(7557,5681,7493,'Мускегон'),(7558,5681,7493,'Мускегон-Хейгтс'),(7559,5681,7493,'Норт Мускегон'),(7560,5681,7493,'Нортон Шорес'),(7561,5681,7493,'Ньюберри'),(7562,5681,7493,'Оак Парк'),(7563,5681,7493,'Огаста'),(7564,5681,7493,'Отисвилл'),(7565,5681,7493,'Парчмент'),(7566,5681,7493,'Понтиак'),(7567,5681,7493,'Порт-Гурон'),(7568,5681,7493,'Портаг'),(7569,5681,7493,'Редфорд'),(7570,5681,7493,'Ривер-Руж'),(7571,5681,7493,'Ричланд'),(7572,5681,7493,'Росевилл'),(7573,5681,7493,'Роял-Оак'),(7574,5681,7493,'Сагинав'),(7575,5681,7493,'Сант-Клэр-Шорес'),(7576,5681,7493,'Саут-Лайон'),(7577,5681,7493,'Саут-Хейвен'),(7578,5681,7493,'Саутгейт'),(7579,5681,7493,'Саутфилд'),(7580,5681,7493,'Сентрал Лак'),(7581,5681,7493,'Стерлинг-Хейтс'),(7582,5681,7493,'Траверс-Сити'),(7583,5681,7493,'Траубридж Парк'),(7584,5681,7493,'Трой'),(7585,5681,7493,'Тэйлор'),(7586,5681,7493,'Уитмор-Лейк'),(7587,5681,7493,'Фаир Плаин'),(7588,5681,7493,'Ферндал'),(7589,5681,7493,'Флинт'),(7590,5681,7493,'Хамтрамк'),(7591,5681,7493,'Харпер-Вудс'),(7592,5681,7493,'Хезел-Парк'),(7593,5681,7493,'Хигланд-Парк'),(7594,5681,7493,'Хиллсдал'),(7595,5681,7493,'Холланд'),(7596,5681,7493,'Шварц-Крик'),(7597,5681,7493,'Энн-Арбор'),(7599,5681,7598,'Анаконда'),(7600,5681,7598,'Биллингс'),(7601,5681,7598,'Бьютт'),(7602,5681,7598,'Валкервилл'),(7603,5681,7598,'Гласгоу'),(7604,5681,7598,'Грейт-Фоллс'),(7605,5681,7598,'Калиспелл'),(7606,5681,7598,'Ливингстон'),(7607,5681,7598,'Миссоула'),(7608,5681,7598,'Раундап'),(7609,5681,7598,'Силвер-Бау'),(7610,5681,7598,'Хелена'),(7611,5681,7598,'Эвергрин'),(7612,5681,7598,'Эннис'),(7614,5681,7613,'Бакспорт'),(7615,5681,7613,'Бакстон'),(7616,5681,7613,'Бангор'),(7617,5681,7613,'Биддефорд'),(7618,5681,7613,'Бревер'),(7619,5681,7613,'Вестбрук'),(7620,5681,7613,'Визи'),(7621,5681,7613,'Винтерпорт'),(7622,5681,7613,'Горхам'),(7623,5681,7613,'Камберленд-Сентер'),(7624,5681,7613,'Кейп-Элизабет'),(7625,5681,7613,'Кеннебанк'),(7626,5681,7613,'Левистон'),(7627,5681,7613,'Милфорд'),(7628,5681,7613,'Огаста'),(7629,5681,7613,'Олд-Орчард-Бич'),(7630,5681,7613,'Олд-Таун'),(7631,5681,7613,'Ороно'),(7632,5681,7613,'Портленд'),(7633,5681,7613,'Сако'),(7634,5681,7613,'Саут-Портланд'),(7635,5681,7613,'Скарборо'),(7636,5681,7613,'Фалмаут'),(7637,5681,7613,'Фалмаут-Форсайд'),(7638,5681,7613,'Фрипорт'),(7639,5681,7613,'Хампден'),(7640,5681,7613,'Ярмаут'),(7642,5681,7641,'Аннаполис'),(7643,5681,7641,'Арбутус'),(7644,5681,7641,'Балтимор'),(7645,5681,7641,'Балтимор-Хайлендс'),(7646,5681,7641,'Бауи'),(7647,5681,7641,'Бетесда'),(7648,5681,7641,'Бладенсбург'),(7649,5681,7641,'Брентвуд'),(7650,5681,7641,'Бруклин-Парк'),(7651,5681,7641,'Брукмонт'),(7652,5681,7641,'Витон'),(7653,5681,7641,'Вудлаун'),(7654,5681,7641,'Глен-Барни'),(7655,5681,7641,'Дандальк'),(7656,5681,7641,'Ессекс'),(7657,5681,7641,'Камберленд'),(7658,5681,7641,'Карни'),(7659,5681,7641,'Катонсвилл'),(7660,5681,7641,'Колледж-Парк'),(7661,5681,7641,'Колмар-Манор'),(7662,5681,7641,'Корал-Хиллс'),(7663,5681,7641,'Коттедж-Сити'),(7664,5681,7641,'Крисфилд'),(7665,5681,7641,'Лансдаун'),(7666,5681,7641,'Латервилл'),(7667,5681,7641,'Линтикум'),(7668,5681,7641,'Лочирн'),(7669,5681,7641,'Марлау-Хейгтс'),(7670,5681,7641,'Маунт-Рейнье'),(7671,5681,7641,'Норт-Брентвуд'),(7672,5681,7641,'Оверли'),(7673,5681,7641,'Орчард-Хиллс'),(7674,5681,7641,'Пайксвилл'),(7675,5681,7641,'Памфри'),(7676,5681,7641,'Парквилл'),(7677,5681,7641,'Роквилл'),(7678,5681,7641,'Роседейл'),(7679,5681,7641,'Россвилл'),(7680,5681,7641,'Силвер Спринг'),(7681,5681,7641,'Сомерсет'),(7682,5681,7641,'Суитленд'),(7683,5681,7641,'Такома-Парк'),(7684,5681,7641,'Таусон'),(7685,5681,7641,'Уолкер-Милл'),(7686,5681,7641,'Фрутленд'),(7687,5681,7641,'Хагерстаун'),(7688,5681,7641,'Чеви-Час'),(7689,5681,7641,'Чиллум'),(7690,5681,7641,'Элликотт-Сити'),(7692,5681,7691,'Беллив'),(7693,5681,7691,'Битрайс'),(7694,5681,7691,'Боис-Таун'),(7695,5681,7691,'Гранд-Айленд'),(7696,5681,7691,'Дентон'),(7697,5681,7691,'ЛаВиста'),(7698,5681,7691,'Линкольн'),(7699,5681,7691,'Мак-Кук'),(7700,5681,7691,'Милфорд'),(7701,5681,7691,'Небраска-Сити'),(7702,5681,7691,'Норт-Платт'),(7703,5681,7691,'Норфолк'),(7704,5681,7691,'Олбани'),(7705,5681,7691,'Омаха'),(7706,5681,7691,'Оффутт база ВВС'),(7707,5681,7691,'Папиллион'),(7708,5681,7691,'Ралстон'),(7709,5681,7691,'Рока'),(7710,5681,7691,'Скоттсблуфф'),(7711,5681,7691,'Спрагуэ'),(7712,5681,7691,'Хастингс'),(7713,5681,7691,'Хикман'),(7715,5681,7714,'Вегас-Крик'),(7716,5681,7714,'Виннемукка'),(7717,5681,7714,'Винчестер'),(7718,5681,7714,'Ист-Лас-Вегас'),(7719,5681,7714,'Йерингтон'),(7720,5681,7714,'Калинт'),(7721,5681,7714,'Карсон-Сити'),(7722,5681,7714,'Лас-Вегас'),(7723,5681,7714,'Ловелок'),(7724,5681,7714,'Норт-Лас-Вегас'),(7725,5681,7714,'Овертон'),(7726,5681,7714,'Парадайс'),(7727,5681,7714,'Рино'),(7728,5681,7714,'Тонопа'),(7729,5681,7714,'Фаллон'),(7730,5681,7714,'Хавторн'),(7731,5681,7714,'Эврика'),(7732,5681,7714,'Эли'),(7734,5681,7733,'Вольфеборо'),(7735,5681,7733,'Вудсвилл'),(7736,5681,7733,'Довер'),(7737,5681,7733,'Конкорд'),(7738,5681,7733,'Манчестер'),(7739,5681,7733,'Нашуа'),(7740,5681,7733,'Ньюпорт'),(7741,5681,7733,'Олбани'),(7742,5681,7733,'Портсмоут'),(7743,5681,7733,'Эппинг'),(7745,5681,7744,'Айленд-Хейгтс'),(7746,5681,7744,'Атлантик-Сити'),(7747,5681,7744,'Беллвилл'),(7748,5681,7744,'Белмар'),(7749,5681,7744,'Бергенфилд'),(7750,5681,7744,'Беркли'),(7751,5681,7744,'Бичвуд'),(7752,5681,7744,'Блумфилд'),(7753,5681,7744,'Брик'),(7754,5681,7744,'Брик-Таун'),(7755,5681,7744,'Бруклаун'),(7756,5681,7744,'Бэйонн'),(7757,5681,7744,'Вест-Лонг-Бранч'),(7758,5681,7744,'Вест-Нью-Йорк'),(7759,5681,7744,'Вест-Орандж'),(7760,5681,7744,'Вестфилд'),(7761,5681,7744,'Виктори-Гарденс'),(7762,5681,7744,'Виллингборо'),(7763,5681,7744,'Винланд'),(7764,5681,7744,'Вихавкен'),(7765,5681,7744,'Вэйн'),(7766,5681,7744,'Гарфилд'),(7767,5681,7744,'Гилфорд-Парк'),(7768,5681,7744,'Глен-Ридж'),(7769,5681,7744,'Глочестер-Сити'),(7770,5681,7744,'Гуттенберг'),(7771,5681,7744,'Деланко'),(7772,5681,7744,'Дельран'),(7773,5681,7744,'Денвилл'),(7774,5681,7744,'Джерси-Сити'),(7775,5681,7744,'Довер'),(7776,5681,7744,'Инглевуд'),(7777,5681,7744,'Инглевуд-Клиффс'),(7778,5681,7744,'Ирвингтон'),(7779,5681,7744,'Ист-Брунсвик'),(7780,5681,7744,'Ист-Ньюарк'),(7781,5681,7744,'Ист-Орандж'),(7782,5681,7744,'Итонтаун'),(7783,5681,7744,'Камден'),(7784,5681,7744,'Кирни'),(7785,5681,7744,'Клифтон'),(7786,5681,7744,'Клиффсайд-Парк'),(7787,5681,7744,'Коллингсвуд'),(7788,5681,7744,'Лейк-Парсиппани'),(7789,5681,7744,'Лейквуд'),(7790,5681,7744,'Лейкхарст'),(7791,5681,7744,'Леониа'),(7792,5681,7744,'Ливингстон'),(7793,5681,7744,'Линден'),(7794,5681,7744,'Лоди'),(7795,5681,7744,'Лонг-Бранч'),(7796,5681,7744,'Марлборо'),(7797,5681,7744,'Маунтайн-Лейкс'),(7798,5681,7744,'Медфорд-Лейкс'),(7799,5681,7744,'Мендхам'),(7800,5681,7744,'Мершантвилл'),(7801,5681,7744,'Монтклайр'),(7802,5681,7744,'Моррис-Плайнс'),(7803,5681,7744,'Морристаун'),(7804,5681,7744,'Мэдисон'),(7805,5681,7744,'Натли'),(7806,5681,7744,'Нептун-Сити'),(7807,5681,7744,'Норт-Берген'),(7808,5681,7744,'Нью-Брунсвик'),(7809,5681,7744,'Ньюарк'),(7810,5681,7744,'Оаклин'),(7811,5681,7744,'Орандж'),(7812,5681,7744,'Оушн-Гейт'),(7813,5681,7744,'Пайн-Бич'),(7814,5681,7744,'Палисадес-Парк'),(7815,5681,7744,'Пальмира'),(7816,5681,7744,'Парамус'),(7817,5681,7744,'Парсиппани'),(7818,5681,7744,'Пассаик'),(7819,5681,7744,'Патерсон'),(7820,5681,7744,'Пеннингтон'),(7821,5681,7744,'Пеннсаукен'),(7822,5681,7744,'Перт-Амбой'),(7823,5681,7744,'Пискатавэй'),(7824,5681,7744,'Плайнфилд'),(7825,5681,7744,'Равэй'),(7826,5681,7744,'Риверсайд'),(7827,5681,7744,'Ривертон'),(7828,5681,7744,'Риджвуд'),(7829,5681,7744,'Риджефилд'),(7830,5681,7744,'Рэндольф'),(7831,5681,7744,'Саут-Белмар'),(7832,5681,7744,'Саут-Томс-Ривер'),(7833,5681,7744,'Секаукус'),(7834,5681,7744,'Силвертон'),(7835,5681,7744,'Сисайд-Хейгтс'),(7836,5681,7744,'Сэйревилл'),(7837,5681,7744,'Тинек'),(7838,5681,7744,'Томс-Ривер'),(7839,5681,7744,'Трентон'),(7840,5681,7744,'Трой-Хиллс'),(7841,5681,7744,'Файр-Лаун'),(7842,5681,7744,'Файрвью'),(7843,5681,7744,'Флорам-Парк'),(7844,5681,7744,'Форт-Ли'),(7845,5681,7744,'Хаддон'),(7846,5681,7744,'Хакенсак'),(7847,5681,7744,'Хановер'),(7848,5681,7744,'Харрисон'),(7849,5681,7744,'Хиллсайд'),(7850,5681,7744,'Хобокен'),(7851,5681,7744,'Холидэй-Сити'),(7852,5681,7744,'Черри-Хилл'),(7853,5681,7744,'Чиннаминсон'),(7854,5681,7744,'Эджуотер'),(7855,5681,7744,'Эдисон'),(7856,5681,7744,'Элизабет'),(7857,5681,7744,'Юнион'),(7858,5681,7744,'Юнион-Сити'),(7860,5681,7859,'Айрондекуит'),(7861,5681,7859,'Амстердам'),(7862,5681,7859,'Апалачин'),(7863,5681,7859,'Аргил'),(7864,5681,7859,'Ардсли'),(7865,5681,7859,'Арлингтон'),(7866,5681,7859,'Атенс'),(7867,5681,7859,'Балдвин'),(7868,5681,7859,'Балдвинсвилл'),(7869,5681,7859,'Балмвилл'),(7870,5681,7859,'Батавиа'),(7871,5681,7859,'Беллероз'),(7872,5681,7859,'Бетпейдж'),(7873,5681,7859,'Бикон'),(7874,5681,7859,'Бингамтон'),(7875,5681,7859,'Бласделл'),(7876,5681,7859,'Блаувелт'),(7877,5681,7859,'Блумингдейл'),(7878,5681,7859,'Брайтон'),(7879,5681,7859,'Брайтуотерс'),(7880,5681,7859,'Брентвуд'),(7881,5681,7859,'Бринкерхофф'),(7882,5681,7859,'Броквэй'),(7883,5681,7859,'Бронкс'),(7884,5681,7859,'Бруклин'),(7885,5681,7859,'Буффало'),(7886,5681,7859,'Бэй-Шор'),(7887,5681,7859,'Бэйберри'),(7888,5681,7859,'Ваппингерс-Фоллс'),(7889,5681,7859,'Ватертаун'),(7890,5681,7859,'Вест-Айслип'),(7891,5681,7859,'Вест-Бэбилон'),(7892,5681,7859,'Вест-Сенека'),(7893,5681,7859,'Вест-Хаверстроу'),(7894,5681,7859,'Вест-Хемпстид'),(7895,5681,7859,'Вестал'),(7896,5681,7859,'Вествейл'),(7897,5681,7859,'Вестмер'),(7898,5681,7859,'Виллард'),(7899,5681,7859,'Вилльямсвилл'),(7900,5681,7859,'Виола'),(7901,5681,7859,'Вудсайд'),(7902,5681,7859,'Вэлли-Стрим'),(7903,5681,7859,'Галвэй'),(7904,5681,7859,'Гарден-Сити'),(7905,5681,7859,'Гейтс'),(7906,5681,7859,'Гилбертсвилл'),(7907,5681,7859,'Глен-Коув'),(7908,5681,7859,'Гленхэм'),(7909,5681,7859,'Гованда'),(7910,5681,7859,'Грин-Айленд'),(7911,5681,7859,'Гринвич'),(7912,5681,7859,'Грис'),(7913,5681,7859,'Грэйт-Нек-Эстейтс'),(7914,5681,7859,'Грэйтт-Нек-Плаза'),(7915,5681,7859,'Гувернье'),(7916,5681,7859,'ДеВитт'),(7917,5681,7859,'Депев'),(7918,5681,7859,'Джамайка'),(7919,5681,7859,'Джеймстаун'),(7920,5681,7859,'Джефферсон-Хейгтс'),(7921,5681,7859,'Джонсон-Сити'),(7922,5681,7859,'Дир-Парк'),(7923,5681,7859,'Ист-Вестал'),(7924,5681,7859,'Ист-Миддлтаун'),(7925,5681,7859,'Ист-Мидоу'),(7926,5681,7859,'Ист-Патчога'),(7927,5681,7859,'Ист-Сиракус'),(7928,5681,7859,'Истчестер'),(7929,5681,7859,'Итака'),(7930,5681,7859,'Йонкерс'),(7931,5681,7859,'Йорквилл'),(7932,5681,7859,'Йорктаун-Хейгтс'),(7933,5681,7859,'Камиллус'),(7934,5681,7859,'Катскилл'),(7935,5681,7859,'Каттарагус'),(7936,5681,7859,'Квинс'),(7937,5681,7859,'Кев-Гарденс'),(7938,5681,7859,'Кенмор'),(7939,5681,7859,'Кингстон'),(7940,5681,7859,'Кларенс-Сентер'),(7941,5681,7859,'Кларк-Миллс'),(7942,5681,7859,'Клинтон'),(7943,5681,7859,'Колони'),(7944,5681,7859,'Коммак'),(7945,5681,7859,'Коринт'),(7946,5681,7859,'Корона'),(7947,5681,7859,'Кохоэс'),(7948,5681,7859,'Лакаванна'),(7949,5681,7859,'Ланкастер'),(7950,5681,7859,'Латам'),(7951,5681,7859,'Лауренс'),(7952,5681,7859,'Левиттаун'),(7953,5681,7859,'Лейк-Плэсид'),(7954,5681,7859,'Лейк-Ронконкома'),(7955,5681,7859,'Лейк-Саксесс'),(7956,5681,7859,'Либерти'),(7957,5681,7859,'Ливерпуль'),(7958,5681,7859,'Линденхарст'),(7959,5681,7859,'Линелл-Мидаус'),(7960,5681,7859,'Линкурт'),(7961,5681,7859,'Локпорт'),(7962,5681,7859,'Лонг-Айленд-Сити'),(7963,5681,7859,'Лонг-Бич'),(7964,5681,7859,'Лоудонвилл'),(7965,5681,7859,'Майерс-Корнер'),(7966,5681,7859,'Манхассет'),(7967,5681,7859,'Манхаттан'),(7968,5681,7859,'Маркеллус'),(7969,5681,7859,'Марлборо'),(7970,5681,7859,'Массапеква'),(7971,5681,7859,'Маттидейл'),(7972,5681,7859,'Маунт-Вернон'),(7973,5681,7859,'Маунт-Моррис'),(7974,5681,7859,'Менандс'),(7975,5681,7859,'Меррик'),(7976,5681,7859,'Миддл-Айденд'),(7977,5681,7859,'Миддл-Хоуп'),(7978,5681,7859,'Миноа'),(7979,5681,7859,'Монси'),(7980,5681,7859,'Моррис'),(7981,5681,7859,'Нануэт'),(7982,5681,7859,'Несконсет'),(7983,5681,7859,'Ниагара-Фоллс'),(7984,5681,7859,'Нискаюна'),(7985,5681,7859,'Норт-Бэбилон'),(7986,5681,7859,'Норт-Вэлли-Стрим'),(7987,5681,7859,'Норт-Гейтс'),(7988,5681,7859,'Норт-Коллинс'),(7989,5681,7859,'Норт-Сиракус'),(7990,5681,7859,'Норт-Тонаванда'),(7991,5681,7859,'Нью-Виндсор'),(7992,5681,7859,'Нью-Йорк'),(7993,5681,7859,'Нью-Йорк-Миллс'),(7994,5681,7859,'Нью-Рочелл'),(7995,5681,7859,'Нью-Сити'),(7996,5681,7859,'Нью-Хакенсак'),(7997,5681,7859,'Нью-Хартфорд'),(7998,5681,7859,'Ньюбург'),(7999,5681,7859,'Оберн'),(8000,5681,7859,'Огденсбург'),(8001,5681,7859,'Олбани'),(8002,5681,7859,'Олин'),(8003,5681,7859,'Онеонта'),(8004,5681,7859,'Орискани'),(8005,5681,7859,'Орчард-Парк'),(8006,5681,7859,'Отего'),(8007,5681,7859,'Оушннсайд'),(8008,5681,7859,'Перрисбург'),(8009,5681,7859,'Питчер-Хилл'),(8010,5681,7859,'Плайнвью'),(8011,5681,7859,'Помона'),(8012,5681,7859,'Порт-Джервис'),(8013,5681,7859,'Порт-Джефферсон'),(8014,5681,7859,'Порт-Честер'),(8015,5681,7859,'Пугкипси'),(8016,5681,7859,'Расселл-Гарденс'),(8017,5681,7859,'Ред-Оакс-Милл'),(8018,5681,7859,'Ренсселер'),(8019,5681,7859,'Риверхед'),(8020,5681,7859,'Ром'),(8021,5681,7859,'Рослин-Хейгтс'),(8022,5681,7859,'Росслевилл'),(8023,5681,7859,'Роттердам'),(8024,5681,7859,'Рочестер'),(8025,5681,7859,'Рошдейл'),(8026,5681,7859,'Саддл-Рок'),(8027,5681,7859,'Сант-Джордж'),(8028,5681,7859,'Саранак-Лейк'),(8029,5681,7859,'Саратога-Спрингс'),(8030,5681,7859,'Саут-Бич'),(8031,5681,7859,'Саут-Дэйтон'),(8032,5681,7859,'Саут-Флорал-Парк'),(8033,5681,7859,'Сент-Джеймс'),(8034,5681,7859,'Сентерич'),(8035,5681,7859,'Сентрал-Айслип'),(8036,5681,7859,'Сильвер-Крик'),(8037,5681,7859,'Сиракус'),(8038,5681,7859,'Скарсдейл'),(8039,5681,7859,'Слоан'),(8040,5681,7859,'Смиттаун'),(8041,5681,7859,'Солвэй'),(8042,5681,7859,'Спринг-Вэлли'),(8043,5681,7859,'Стейтен-Айленд'),(8044,5681,7859,'Стони-Пойнт'),(8045,5681,7859,'Стоттвилл'),(8046,5681,7859,'Стьюарт-Манор'),(8047,5681,7859,'Тонаванда'),(8048,5681,7859,'Трой'),(8049,5681,7859,'Уайт-Плайнс'),(8050,5681,7859,'Уайтсборо'),(8051,5681,7859,'Уотервлит'),(8052,5681,7859,'Фейрмаунт'),(8053,5681,7859,'Филмонт'),(8054,5681,7859,'Фишкилл'),(8055,5681,7859,'Флашинг'),(8056,5681,7859,'Флорал-Парк'),(8057,5681,7859,'Форест-Хиллс'),(8058,5681,7859,'Форествилл'),(8059,5681,7859,'Форт-Эдвард'),(8060,5681,7859,'Франклин'),(8061,5681,7859,'Франклин-Сквер'),(8062,5681,7859,'Франкфорт'),(8063,5681,7859,'Фрипорт'),(8064,5681,7859,'Фэйеттевилл'),(8065,5681,7859,'Хавторн'),(8066,5681,7859,'Хадсон'),(8067,5681,7859,'Хадсон-Фоллс'),(8068,5681,7859,'Хантингтон-Стэйшн'),(8069,5681,7859,'Харрис-Хилл'),(8070,5681,7859,'Хартсдал'),(8071,5681,7859,'Хауппауг'),(8072,5681,7859,'Хемпстид'),(8073,5681,7859,'Хид-оф-ти-Харбор'),(8074,5681,7859,'Хиксвилл'),(8075,5681,7859,'Хиллкрест'),(8076,5681,7859,'Холланд-Патент'),(8077,5681,7859,'Черри-Крик'),(8078,5681,7859,'Чиктовага'),(8079,5681,7859,'Шайлервилл'),(8080,5681,7859,'Шенектади'),(8081,5681,7859,'Элма-Сентер'),(8082,5681,7859,'Элмира'),(8083,5681,7859,'Элмонт'),(8084,5681,7859,'Элмхарст'),(8085,5681,7859,'Эльмсфорд'),(8086,5681,7859,'Эндвелл'),(8087,5681,7859,'Эндикотт'),(8088,5681,7859,'Ютика'),(8090,5681,8089,'Аламогордо'),(8091,5681,8089,'Альбукерк'),(8092,5681,8089,'Антони'),(8093,5681,8089,'Байярд'),(8094,5681,8089,'Берналилло'),(8095,5681,8089,'Вагон-Маунд'),(8096,5681,8089,'Гэллап'),(8097,5681,8089,'Декстер'),(8098,5681,8089,'Деминг'),(8099,5681,8089,'Ислета-Пуэбло'),(8100,5681,8089,'Карлсбад'),(8101,5681,8089,'Карризозо'),(8102,5681,8089,'Киртленд'),(8103,5681,8089,'Клейтон'),(8104,5681,8089,'Кловис'),(8105,5681,8089,'Корралес'),(8106,5681,8089,'Куба'),(8107,5681,8089,'Лас-Вегас'),(8108,5681,8089,'Лас-Крукес'),(8109,5681,8089,'Лос-Аламос'),(8110,5681,8089,'Лос-Лунас'),(8111,5681,8089,'Лос-Ранчос-де-Альбукерк'),(8112,5681,8089,'Месилла'),(8113,5681,8089,'Норт-Валли'),(8114,5681,8089,'Парадайс-Хиллс'),(8115,5681,8089,'Пекос'),(8116,5681,8089,'Ранчес-оф-Таос'),(8117,5681,8089,'Ранчос-Де-Таос'),(8118,5681,8089,'Рейтон'),(8119,5681,8089,'Рио-Ранчо-Эстатес'),(8120,5681,8089,'Рой'),(8121,5681,8089,'Росвелл'),(8122,5681,8089,'Сан-Фелипе-Пуэбло'),(8123,5681,8089,'Сандиа'),(8124,5681,8089,'Санта-Роза'),(8125,5681,8089,'Санта-Фе'),(8126,5681,8089,'Саут-Вэлли'),(8127,5681,8089,'Сентрал'),(8128,5681,8089,'Силвер-Сити'),(8129,5681,8089,'Сокорро'),(8130,5681,8089,'Таос'),(8131,5681,8089,'Татум'),(8132,5681,8089,'Тесукуэ'),(8133,5681,8089,'Тийерас'),(8134,5681,8089,'Трас-Ор-Консекуэнсес'),(8135,5681,8089,'Тукумкари'),(8136,5681,8089,'Тьерра-Амарилла'),(8137,5681,8089,'Уайт-Рок'),(8138,5681,8089,'Фармингтон'),(8139,5681,8089,'Форт-Самнер'),(8140,5681,8089,'Харли'),(8141,5681,8089,'Хоббс'),(8142,5681,8089,'Чама'),(8143,5681,8089,'Чимэйо'),(8144,5681,8089,'Эспанола'),(8146,5681,8145,'Авон'),(8147,5681,8145,'Айронтон'),(8148,5681,8145,'Акрон'),(8149,5681,8145,'Алжер'),(8150,5681,8145,'Амберли'),(8151,5681,8145,'Амесвилл'),(8152,5681,8145,'Амхерст'),(8153,5681,8145,'Аппер-Арлингтон'),(8154,5681,8145,'Аппер-Сандуски'),(8155,5681,8145,'Арлингтон-Хейгтс'),(8156,5681,8145,'Аталия'),(8157,5681,8145,'Атенс'),(8158,5681,8145,'Ашланд'),(8159,5681,8145,'Аштабула'),(8160,5681,8145,'Баллвилл'),(8161,5681,8145,'Барбертон'),(8162,5681,8145,'Барнесвилл'),(8163,5681,8145,'Бачтел'),(8164,5681,8145,'Бедфорд'),(8165,5681,8145,'Бедфорд-Хейгтс'),(8166,5681,8145,'Бексли'),(8167,5681,8145,'Беллвилл'),(8168,5681,8145,'Беллив'),(8169,5681,8145,'Белпр'),(8170,5681,8145,'Бери'),(8171,5681,8145,'Берки'),(8172,5681,8145,'Беттсвилл'),(8173,5681,8145,'Бичвуд'),(8174,5681,8145,'Блеклик-Эстатс'),(8175,5681,8145,'Блу-Эш'),(8176,5681,8145,'Боардман'),(8177,5681,8145,'Браднер'),(8178,5681,8145,'Братеналь'),(8179,5681,8145,'Бревстер'),(8180,5681,8145,'Брентвуд'),(8181,5681,8145,'Брик'),(8182,5681,8145,'Брук-Парк'),(8183,5681,8145,'Бруклин'),(8184,5681,8145,'Бруклин-Хейтс'),(8185,5681,8145,'Брукфилд'),(8186,5681,8145,'Букланд'),(8187,5681,8145,'Бургун'),(8188,5681,8145,'Бутлер'),(8189,5681,8145,'Валбридж'),(8190,5681,8145,'Ван Верт'),(8191,5681,8145,'Вандалиа'),(8192,5681,8145,'Варрен'),(8193,5681,8145,'Варренсвилл-Хейгтс'),(8194,5681,8145,'Ватервилл'),(8195,5681,8145,'Ваусеон'),(8196,5681,8145,'Вест Карроллтон'),(8197,5681,8145,'Вест-Портсмут'),(8198,5681,8145,'Вестлак'),(8199,5681,8145,'Виклифф'),(8200,5681,8145,'Виллауик'),(8201,5681,8145,'Виллугби-Хиллс'),(8202,5681,8145,'Виоминг'),(8203,5681,8145,'Вортингтон'),(8204,5681,8145,'Вудбоурн'),(8205,5681,8145,'Вудлавн'),(8206,5681,8145,'Вустер'),(8207,5681,8145,'Галион'),(8208,5681,8145,'Гарфилд-Хейгтс'),(8209,5681,8145,'Гаханна'),(8210,5681,8145,'Генева'),(8211,5681,8145,'Гирард'),(8212,5681,8145,'Голф Манор'),(8213,5681,8145,'Грандвив-Хейтс'),(8214,5681,8145,'Грин-Спрингс'),(8215,5681,8145,'Гринхиллс'),(8216,5681,8145,'Гров-Сити'),(8217,5681,8145,'Гроэсбек'),(8218,5681,8145,'Дальтон'),(8219,5681,8145,'Девола'),(8220,5681,8145,'Дели-Хиллс'),(8221,5681,8145,'Дефианк'),(8222,5681,8145,'Джефферсон'),(8223,5681,8145,'Джорджтаун'),(8224,5681,8145,'Дир-Парк'),(8225,5681,8145,'Доннелсвилл'),(8226,5681,8145,'Дэйтон'),(8227,5681,8145,'Евклид'),(8228,5681,8145,'Енон'),(8229,5681,8145,'Женева-он-Лейк'),(8230,5681,8145,'Залески'),(8231,5681,8145,'Занесвилл'),(8232,5681,8145,'Индепенденс'),(8233,5681,8145,'Индиан Хилл'),(8234,5681,8145,'Ист-Кливленд'),(8235,5681,8145,'Истлак'),(8236,5681,8145,'Йеллоу-Спрингс'),(8237,5681,8145,'Канал-Фултон'),(8238,5681,8145,'Кантон'),(8239,5681,8145,'Касстаун'),(8240,5681,8145,'Касталиа'),(8241,5681,8145,'Катавба'),(8242,5681,8145,'Кембридж'),(8243,5681,8145,'Кенвуд'),(8244,5681,8145,'Кент'),(8245,5681,8145,'Кеттеринг'),(8246,5681,8145,'Кингсвилл'),(8247,5681,8145,'Кливленд'),(8248,5681,8145,'Кливленд-Хейгтс'),(8249,5681,8145,'Клид'),(8250,5681,8145,'Клэй-Сентер'),(8251,5681,8145,'Коал-Гров'),(8252,5681,8145,'Коведал'),(8253,5681,8145,'Ковингтон'),(8254,5681,8145,'Колумбус'),(8255,5681,8145,'Кридерсвилл'),(8256,5681,8145,'Ксениа'),(8257,5681,8145,'Кулвилл'),(8258,5681,8145,'Лакевуд'),(8259,5681,8145,'Лакелин'),(8260,5681,8145,'Ланкастер'),(8261,5681,8145,'Лауелл'),(8262,5681,8145,'Лаура'),(8263,5681,8145,'Лейкмор'),(8264,5681,8145,'Лима'),(8265,5681,8145,'Линдхарст'),(8266,5681,8145,'Линколн-Хейгтс'),(8267,5681,8145,'Линкольн-Виллидж'),(8268,5681,8145,'Линндейл'),(8269,5681,8145,'Лисбург'),(8270,5681,8145,'Логан'),(8271,5681,8145,'Локингтон'),(8272,5681,8145,'Лорейн'),(8273,5681,8145,'Лоудонвилл'),(8274,5681,8145,'Лоуренсвилл'),(8275,5681,8145,'Луки'),(8276,5681,8145,'Мадейра'),(8277,5681,8145,'Майерс-Лейк'),(8278,5681,8145,'Мансфилд'),(8279,5681,8145,'Мапл-Хейгтс'),(8280,5681,8145,'Марбл-Клифф'),(8281,5681,8145,'Маримонт'),(8282,5681,8145,'Марион'),(8283,5681,8145,'Маритта'),(8284,5681,8145,'Массиллон'),(8285,5681,8145,'Масури'),(8286,5681,8145,'Маунт-Вернон'),(8287,5681,8145,'Маунт-Гилид'),(8288,5681,8145,'Маунт-Хелси'),(8289,5681,8145,'Медина'),(8290,5681,8145,'Ментор'),(8291,5681,8145,'Ментор-он-те-Лейк'),(8292,5681,8145,'Миддлбург-Хейтс'),(8293,5681,8145,'Миддлтаун'),(8294,5681,8145,'Милан'),(8295,5681,8145,'Миллбури'),(8296,5681,8145,'Милфорд'),(8297,5681,8145,'Минерва-Парк'),(8298,5681,8145,'Минстер'),(8299,5681,8145,'Миффлин'),(8300,5681,8145,'Могадор'),(8301,5681,8145,'Монтгомери'),(8302,5681,8145,'Монфорт-Хейгтс'),(8303,5681,8145,'Мораин'),(8304,5681,8145,'Муррэй-Сити'),(8305,5681,8145,'Мутуал'),(8306,5681,8145,'Мэдисон'),(8307,5681,8145,'Мэйфилд-Хейгтс'),(8308,5681,8145,'Найлс'),(8309,5681,8145,'Нелсонвилл'),(8310,5681,8145,'Норвич'),(8311,5681,8145,'Норвуд'),(8312,5681,8145,'Норт-Кантон'),(8313,5681,8145,'Норт-Кингсвилл'),(8314,5681,8145,'Норт-Колледж-Хилл'),(8315,5681,8145,'Норт-Мэдисон'),(8316,5681,8145,'Норт-Олмстед'),(8317,5681,8145,'Норт-Рендалл'),(8318,5681,8145,'Норт-Риджевилл'),(8319,5681,8145,'Нортбрук'),(8320,5681,8145,'Нортвуд'),(8321,5681,8145,'Нортридж'),(8322,5681,8145,'Нью-Бостон'),(8323,5681,8145,'Нью-Бремен'),(8324,5681,8145,'Нью-Винна'),(8325,5681,8145,'Нью-Конкорд'),(8326,5681,8145,'Нью-Ригель'),(8327,5681,8145,'Нью-Ром'),(8328,5681,8145,'Нью-Филадельфия'),(8329,5681,8145,'Ньюарк'),(8330,5681,8145,'Ньюбург-Хейгтс'),(8331,5681,8145,'Ньюкомерстаун'),(8332,5681,8145,'Ньютаун'),(8333,5681,8145,'Оберлин'),(8334,5681,8145,'Обетс'),(8335,5681,8145,'Оверлук'),(8336,5681,8145,'Олбани'),(8337,5681,8145,'Олмстед-Фоллс'),(8338,5681,8145,'Онтарио'),(8339,5681,8145,'Орегон'),(8340,5681,8145,'Оррвилл'),(8341,5681,8145,'Остинтаун'),(8342,5681,8145,'Отвэй'),(8343,5681,8145,'Оттава-Хиллс'),(8344,5681,8145,'Парма'),(8345,5681,8145,'Парма-Хейтс'),(8346,5681,8145,'Пейдж-Манор'),(8347,5681,8145,'Пенинсула'),(8348,5681,8145,'Перри'),(8349,5681,8145,'Перрисбург'),(8350,5681,8145,'Пикуа'),(8351,5681,8145,'Писга'),(8352,5681,8145,'Плайнс'),(8353,5681,8145,'Полк'),(8354,5681,8145,'Померой'),(8355,5681,8145,'Порт-Клинтон'),(8356,5681,8145,'Портадж-Лейкс'),(8357,5681,8145,'Портсмоут'),(8358,5681,8145,'Прокторвилл'),(8359,5681,8145,'Рарден'),(8360,5681,8145,'Ратланд'),(8361,5681,8145,'Репаблик'),(8362,5681,8145,'Риверли'),(8363,5681,8145,'Ридинг'),(8364,5681,8145,'Ринолдсбург'),(8365,5681,8145,'Ричмонд-Хейгтс'),(8366,5681,8145,'Розмаунт'),(8367,5681,8145,'Роки-Ривер'),(8368,5681,8145,'Россфорд'),(8369,5681,8145,'Сабина'),(8370,5681,8145,'Саванна'),(8371,5681,8145,'Сандуски'),(8372,5681,8145,'Сант-Бернард'),(8373,5681,8145,'Саут-Амхерст'),(8374,5681,8145,'Саут-Винна'),(8375,5681,8145,'Саут-Евклид'),(8376,5681,8145,'Саут-Маунт-Вернон'),(8377,5681,8145,'Саут-Пойнт'),(8378,5681,8145,'Саут-Салем'),(8379,5681,8145,'Севен-Хиллс'),(8380,5681,8145,'Силваниа'),(8381,5681,8145,'Силвертон'),(8382,5681,8145,'Сиухога-Фоллс'),(8383,5681,8145,'Сиухога-Хейгтс'),(8384,5681,8145,'Спенкервилл'),(8385,5681,8145,'Спрингдал'),(8386,5681,8145,'Спрингфилд'),(8387,5681,8145,'Стубенвилл'),(8388,5681,8145,'Таллмадж'),(8389,5681,8145,'Террак Парк'),(8390,5681,8145,'Тимберлак'),(8391,5681,8145,'Тиффин'),(8392,5681,8145,'Толидо'),(8393,5681,8145,'Тремонт-Сити'),(8394,5681,8145,'Трои'),(8395,5681,8145,'Тротвуд'),(8396,5681,8145,'Уайт-Оак'),(8397,5681,8145,'Уайтехолл'),(8398,5681,8145,'Урбана'),(8399,5681,8145,'Урбанкрест'),(8400,5681,8145,'Фаирборн'),(8401,5681,8145,'Файрвью-Парк'),(8402,5681,8145,'Финдлэй'),(8403,5681,8145,'Флетчер'),(8404,5681,8145,'Форест-Парк'),(8405,5681,8145,'Форт МкКинли'),(8406,5681,8145,'Форт-Лорами'),(8407,5681,8145,'Форт-Шавни'),(8408,5681,8145,'Фремонт'),(8409,5681,8145,'Хаббард'),(8410,5681,8145,'Хайленд-Хейгтс'),(8411,5681,8145,'Хамилтон'),(8412,5681,8145,'Харбор-Вью'),(8413,5681,8145,'Харрод'),(8414,5681,8145,'Хауленд'),(8415,5681,8145,'Хигланд'),(8416,5681,8145,'Хид-Парк'),(8417,5681,8145,'Хиллиард'),(8418,5681,8145,'Холланд'),(8419,5681,8145,'Христиансбург'),(8420,5681,8145,'Хубер-Ридж'),(8421,5681,8145,'Хубер-Хейгтс'),(8422,5681,8145,'Цинциннати'),(8423,5681,8145,'Цирклвилл'),(8424,5681,8145,'Чаунки'),(8425,5681,8145,'Чевиот'),(8426,5681,8145,'Чесапик'),(8427,5681,8145,'Честервилл'),(8428,5681,8145,'Честерхилл'),(8429,5681,8145,'Шакер-Хейгтс'),(8430,5681,8145,'Шелби'),(8431,5681,8145,'Эвендейл'),(8432,5681,8145,'Эдисон'),(8433,5681,8145,'Элирия'),(8434,5681,8145,'Эллианс'),(8435,5681,8145,'Элмвуд-Плейс'),(8436,5681,8145,'Эппл-Крик'),(8437,5681,8145,'Юнгстаун'),(8438,5681,8145,'Юниверсити-Хейгтс'),(8440,5681,8439,'Аркома'),(8441,5681,8439,'Атока'),(8442,5681,8439,'Бартлесвилл'),(8443,5681,8439,'Бетани'),(8444,5681,8439,'Варр-Акрес'),(8445,5681,8439,'Винита'),(8446,5681,8439,'Вудлавн-Парк'),(8447,5681,8439,'Вэлли-Брук'),(8448,5681,8439,'Гленпул'),(8449,5681,8439,'Дель-Сити'),(8450,5681,8439,'Дженкс'),(8451,5681,8439,'Жеронимо'),(8452,5681,8439,'Колгейт'),(8453,5681,8439,'Лаутон'),(8454,5681,8439,'Маскоги'),(8455,5681,8439,'Медсайн-Парк'),(8456,5681,8439,'Мидвест-Сити'),(8457,5681,8439,'Моффетт'),(8458,5681,8439,'Николс-Хиллс'),(8459,5681,8439,'Норман'),(8460,5681,8439,'Оклахома'),(8461,5681,8439,'Олбани'),(8462,5681,8439,'Покола'),(8463,5681,8439,'Понка-Сити'),(8464,5681,8439,'Роланд'),(8465,5681,8439,'Росдейл'),(8466,5681,8439,'Салфур'),(8467,5681,8439,'Санд-Спрингс'),(8468,5681,8439,'Сапалпа'),(8469,5681,8439,'Стиллуотер'),(8470,5681,8439,'Талса'),(8471,5681,8439,'Тарли'),(8472,5681,8439,'Ти-Виллидж'),(8473,5681,8439,'Тулса'),(8474,5681,8439,'Форт-Сапплай'),(8475,5681,8439,'Форт-Силл'),(8476,5681,8439,'Чикаша'),(8477,5681,8439,'Шавни'),(8478,5681,8439,'Энид'),(8480,5681,8479,'Белый Дом'),(8481,5681,8479,'Вашингтон'),(8482,5681,8479,'Штаб Главного Командования'),(8484,5681,8483,'Бивертон'),(8485,5681,8483,'Вест-Слоп'),(8486,5681,8483,'Вильсонвилл'),(8487,5681,8483,'Гарден-Хоум'),(8488,5681,8483,'Даллес'),(8489,5681,8483,'Калли'),(8490,5681,8483,'Коквиль'),(8491,5681,8483,'Корваллис'),(8492,5681,8483,'Ла-Гранд'),(8493,5681,8483,'Лейк-Освего'),(8494,5681,8483,'Мак-Миннвилл'),(8495,5681,8483,'Медфорд'),(8496,5681,8483,'Милуоки'),(8497,5681,8483,'Оак-Гров'),(8498,5681,8483,'Олбани'),(8499,5681,8483,'Освего'),(8500,5681,8483,'Паркрос'),(8501,5681,8483,'Пауэллхарст'),(8502,5681,8483,'Пендлетон'),(8503,5681,8483,'Портланд'),(8504,5681,8483,'Ралей-Хиллс'),(8505,5681,8483,'Редмонд'),(8506,5681,8483,'Салем'),(8507,5681,8483,'Сант-Хеленс'),(8508,5681,8483,'Седар-Хиллс'),(8509,5681,8483,'Спрингфилд'),(8510,5681,8483,'Уайтфорд'),(8511,5681,8483,'Хазелвуд'),(8512,5681,8483,'Хеликс'),(8513,5681,8483,'Хеппнер'),(8514,5681,8483,'Эррол-Хейгтс'),(8515,5681,8483,'Эуген'),(8517,5681,8516,'Абингтон'),(8518,5681,8516,'Авока'),(8519,5681,8516,'Авониа'),(8520,5681,8516,'Алдан'),(8521,5681,8516,'Аликвиппа'),(8522,5681,8516,'Аллентаун'),(8523,5681,8516,'Алтуна'),(8524,5681,8516,'Амблер'),(8525,5681,8516,'Амбридж'),(8526,5681,8516,'Аппер-Даблин'),(8527,5681,8516,'Аппер-Дарби'),(8528,5681,8516,'Аппер-Мерион'),(8529,5681,8516,'Аппер-Сант-Клер'),(8530,5681,8516,'Ардмор'),(8531,5681,8516,'Аспинвалл'),(8532,5681,8516,'Атглен'),(8533,5681,8516,'Баден'),(8534,5681,8516,'Бала-Кинвид'),(8535,5681,8516,'Балдвин'),(8536,5681,8516,'Батлер'),(8537,5681,8516,'Белльвью'),(8538,5681,8516,'Белмонт'),(8539,5681,8516,'Бенсалем'),(8540,5681,8516,'Бенсон'),(8541,5681,8516,'Бентливилл'),(8542,5681,8516,'Бервин'),(8543,5681,8516,'Бетел-Парк'),(8544,5681,8516,'Бетлехем'),(8545,5681,8516,'Бивер'),(8546,5681,8516,'Биллсвилл'),(8547,5681,8516,'Блумфилд'),(8548,5681,8516,'Браддок'),(8549,5681,8516,'Брентвуд'),(8550,5681,8516,'Бриджвилл'),(8551,5681,8516,'Брин-Атин'),(8552,5681,8516,'Брин-Мавр'),(8553,5681,8516,'Брумалл'),(8554,5681,8516,'Бурнхам'),(8555,5681,8516,'Вайомиссинг'),(8556,5681,8516,'Вайомиссинг-Хиллс'),(8557,5681,8516,'Ванпорт'),(8558,5681,8516,'Варминстер'),(8559,5681,8516,'Ваттсбург'),(8560,5681,8516,'Вашингтон'),(8561,5681,8516,'Вашингтонвилл'),(8562,5681,8516,'Веймарт'),(8563,5681,8516,'Вернерсвилл'),(8564,5681,8516,'Весливилл'),(8565,5681,8516,'Вест-Вью'),(8566,5681,8516,'Вест-Коншохокен'),(8567,5681,8516,'Вест-Миддлетаун'),(8568,5681,8516,'Вест-Миффлин'),(8569,5681,8516,'Вест-Норритон'),(8570,5681,8516,'Вест-Ридинг'),(8571,5681,8516,'Вест-Фейрвью'),(8572,5681,8516,'Вестмонт'),(8573,5681,8516,'Вилкес-Барр'),(8574,5681,8516,'Вилкинсбург'),(8575,5681,8516,'Вилльямспорт'),(8576,5681,8516,'Вормлисбург'),(8577,5681,8516,'Вэйн-Хейгтс'),(8578,5681,8516,'Вэйнесборо'),(8579,5681,8516,'Галлицин'),(8580,5681,8516,'Гаррисберг'),(8581,5681,8516,'Гейстаун'),(8582,5681,8516,'Гирард'),(8583,5681,8516,'Грейт-Бенд'),(8584,5681,8516,'Грин-Три'),(8585,5681,8516,'Данвилл'),(8586,5681,8516,'Данмор'),(8587,5681,8516,'Дарби'),(8588,5681,8516,'Даунингтаун'),(8589,5681,8516,'Дженкинтаун'),(8590,5681,8516,'Джонстаун'),(8591,5681,8516,'Дормонт'),(8592,5681,8516,'Дункансвилл'),(8593,5681,8516,'Дэвидсвилл'),(8594,5681,8516,'Инграм'),(8595,5681,8516,'Индастри'),(8596,5681,8516,'Ист Батлер'),(8597,5681,8516,'Ист-Бервик'),(8598,5681,8516,'Ист-Вашингтон'),(8599,5681,8516,'Ист-Конемауг'),(8600,5681,8516,'Ист-Лансдаун'),(8601,5681,8516,'Ист-Норритон'),(8602,5681,8516,'Ист-Проспект'),(8603,5681,8516,'Ист-Рочестер'),(8604,5681,8516,'Истон'),(8605,5681,8516,'Йидон'),(8606,5681,8516,'Йорк'),(8607,5681,8516,'йоркана'),(8608,5681,8516,'Канонсбург'),(8609,5681,8516,'Кармичелс'),(8610,5681,8516,'Карнеги'),(8611,5681,8516,'Катасуква'),(8612,5681,8516,'Кембридж-Спрингс'),(8613,5681,8516,'Кенхорст'),(8614,5681,8516,'Кларкс-Грин'),(8615,5681,8516,'Клифтон-Хейгтс'),(8616,5681,8516,'Клэйсбург'),(8617,5681,8516,'Коатсвилл'),(8618,5681,8516,'Кокбург'),(8619,5681,8516,'Колвин'),(8620,5681,8516,'Колледжвилл'),(8621,5681,8516,'Коллингдейл'),(8622,5681,8516,'Конвей'),(8623,5681,8516,'Коннокуэнессинг'),(8624,5681,8516,'Коншохокен'),(8625,5681,8516,'Крафтон'),(8626,5681,8516,'Крейнсвилл'),(8627,5681,8516,'Крессон'),(8628,5681,8516,'Кэмп-Хилл'),(8629,5681,8516,'Лангелот'),(8630,5681,8516,'Ланкастер'),(8631,5681,8516,'Лансдаун'),(8632,5681,8516,'Латроб'),(8633,5681,8516,'Лаурелдейл'),(8634,5681,8516,'Лебанон'),(8635,5681,8516,'Левисбург'),(8636,5681,8516,'Левистаун'),(8637,5681,8516,'Левиттаун'),(8638,5681,8516,'Лемойн'),(8639,5681,8516,'Линдора'),(8640,5681,8516,'Линнтаун'),(8641,5681,8516,'Лиспорт'),(8642,5681,8516,'Литтл Мидаус'),(8643,5681,8516,'Ловер-Мерион'),(8644,5681,8516,'Ловер-Мореланд'),(8645,5681,8516,'Лорейн'),(8646,5681,8516,'Лоуренс-Парк'),(8647,5681,8516,'Лоусон-Хейгтс'),(8648,5681,8516,'Мак-Кис-Рокс'),(8649,5681,8516,'Мак-Киспорт'),(8650,5681,8516,'Мак-Эвенсвилл'),(8651,5681,8516,'Манхалл'),(8652,5681,8516,'Марианна'),(8653,5681,8516,'Марпл'),(8654,5681,8516,'Мартинсбург'),(8655,5681,8516,'Маунт-Гретна'),(8656,5681,8516,'Маунт-Лебанон'),(8657,5681,8516,'Маунт-Оливер'),(8658,5681,8516,'Меридиан'),(8659,5681,8516,'Миддлтаун'),(8660,5681,8516,'Милл-Виллидж'),(8661,5681,8516,'Миллбурн'),(8662,5681,8516,'Миллвейл'),(8663,5681,8516,'Милтон'),(8664,5681,8516,'Модена'),(8665,5681,8516,'Монака'),(8666,5681,8516,'Монровилл'),(8667,5681,8516,'Монт-Альто'),(8668,5681,8516,'Монтон'),(8669,5681,8516,'Мусик'),(8670,5681,8516,'Нарберт'),(8671,5681,8516,'Немаколин'),(8672,5681,8516,'Норристаун'),(8673,5681,8516,'Нортумберленд'),(8674,5681,8516,'Нью-Камберленд'),(8675,5681,8516,'Нью-Кастл'),(8676,5681,8516,'Ньюри'),(8677,5681,8516,'Олбани (Лехай Кантри)'),(8678,5681,8516,'Олбани (Файет Кантри)'),(8679,5681,8516,'Олд-Форг'),(8680,5681,8516,'Пайнт'),(8681,5681,8516,'Пакстанг'),(8682,5681,8516,'Парксбург'),(8683,5681,8516,'Пенбрук'),(8684,5681,8516,'Пенн-Хиллс'),(8685,5681,8516,'Питтсбург'),(8686,5681,8516,'Плати'),(8687,5681,8516,'Полк'),(8688,5681,8516,'Раднор'),(8689,5681,8516,'Ранкин'),(8690,5681,8516,'Ридинг'),(8691,5681,8516,'Римс'),(8692,5681,8516,'Роаринг-Спринг'),(8693,5681,8516,'Ройерсфорд'),(8694,5681,8516,'Рокледж'),(8695,5681,8516,'Росслин-Фармс'),(8696,5681,8516,'Роузервилл'),(8697,5681,8516,'Рочестер'),(8698,5681,8516,'Саксонбург'),(8699,5681,8516,'Санди'),(8700,5681,8516,'Сант-Марис'),(8701,5681,8516,'Саут-Коатсвилл'),(8702,5681,8516,'Саутмонт'),(8703,5681,8516,'Сватара'),(8704,5681,8516,'Свиссвейл'),(8705,5681,8516,'Селлерсвилл'),(8706,5681,8516,'Синкинг-Спринг'),(8707,5681,8516,'Скалп-Левел'),(8708,5681,8516,'Скрантон'),(8709,5681,8516,'Спринг-Сити'),(8710,5681,8516,'Спрингфилд'),(8711,5681,8516,'Стейт-Колледж'),(8712,5681,8516,'Стилтон'),(8713,5681,8516,'Таннелхилл'),(8714,5681,8516,'Темпл'),(8715,5681,8516,'Торнбург'),(8716,5681,8516,'Трапп'),(8717,5681,8516,'Труп'),(8718,5681,8516,'Трупер'),(8719,5681,8516,'Тэйлор'),(8720,5681,8516,'Уайтмарш'),(8721,5681,8516,'Филадельфия'),(8722,5681,8516,'Финиксвилл'),(8723,5681,8516,'Финливилл'),(8724,5681,8516,'Флемингтон'),(8725,5681,8516,'Фонтайн-Хилл'),(8726,5681,8516,'Фридом'),(8727,5681,8516,'Хавертаун'),(8728,5681,8516,'Хаверфорд'),(8729,5681,8516,'Хазлтон'),(8730,5681,8516,'Хайспайр'),(8731,5681,8516,'Хаммельстаун'),(8732,5681,8516,'Хантингдон'),(8733,5681,8516,'Хеллертаун'),(8734,5681,8516,'Холлидэйсбург'),(8735,5681,8516,'Хоумикр'),(8736,5681,8516,'Хоумстид'),(8737,5681,8516,'Хьюстон'),(8738,5681,8516,'Чарлерой'),(8739,5681,8516,'Челтенхам'),(8740,5681,8516,'Честер'),(8741,5681,8516,'Чикора'),(8742,5681,8516,'Шайрманстаун'),(8743,5681,8516,'Швенксвилл'),(8744,5681,8516,'Шиллингтон'),(8745,5681,8516,'Шиппингпорт'),(8746,5681,8516,'Эвансбург'),(8747,5681,8516,'Эдинборо'),(8748,5681,8516,'Экономи'),(8749,5681,8516,'Эллсворт'),(8750,5681,8516,'Эри'),(8752,5681,8751,'Варвик'),(8753,5681,8751,'Вунсокет'),(8754,5681,8751,'Ист-Провиденкс'),(8755,5681,8751,'Кранстон'),(8756,5681,8751,'Миддлтаун'),(8757,5681,8751,'Ньюпорт'),(8758,5681,8751,'Паутакет'),(8759,5681,8751,'Провиденс'),(8761,5681,8760,'Бисмарк'),(8762,5681,8760,'Гранд-Форкс'),(8763,5681,8760,'Графтон'),(8764,5681,8760,'Лер'),(8765,5681,8760,'Минот'),(8766,5681,8760,'Тиога'),(8767,5681,8760,'Фарго'),(8769,5681,8768,'Абердин'),(8770,5681,8768,'Балфоур'),(8771,5681,8768,'Батнер'),(8772,5681,8768,'Билтмор-Форест'),(8773,5681,8768,'Блак-Маунтайн'),(8774,5681,8768,'Бурлингтон'),(8775,5681,8768,'Валдес'),(8776,5681,8768,'Вест-Конкорд'),(8777,5681,8768,'Виллиамстон'),(8778,5681,8768,'Вильмингтон'),(8779,5681,8768,'Вильсон'),(8780,5681,8768,'Винстон-Салем'),(8781,5681,8768,'Висперинг-Пайнс'),(8782,5681,8768,'Вудфин'),(8783,5681,8768,'Гастониа'),(8784,5681,8768,'Голдсборо'),(8785,5681,8768,'Горман'),(8786,5681,8768,'Гранит-Куарри'),(8787,5681,8768,'Гранит-Фоллс'),(8788,5681,8768,'Гринвилл'),(8789,5681,8768,'Гринсборо'),(8790,5681,8768,'Давидсон'),(8791,5681,8768,'Дархам'),(8792,5681,8768,'Джеймстаун'),(8793,5681,8768,'Джексонвилл'),(8794,5681,8768,'Дрексель'),(8795,5681,8768,'Икард'),(8796,5681,8768,'Индиан-Трейл'),(8797,5681,8768,'Каннаполис'),(8798,5681,8768,'Кинстон'),(8799,5681,8768,'Кливленд'),(8800,5681,8768,'Конкорд'),(8801,5681,8768,'Корнелиус'),(8802,5681,8768,'Кридмур'),(8803,5681,8768,'Кулими'),(8804,5681,8768,'Кэмп-Леюн'),(8805,5681,8768,'Ландис'),(8806,5681,8768,'Ленойр'),(8807,5681,8768,'Мак-Линсвилл'),(8808,5681,8768,'Маунт-Холли'),(8809,5681,8768,'Маунтайн-Хоум'),(8810,5681,8768,'Минт-Хилл'),(8811,5681,8768,'Моксвилл'),(8812,5681,8768,'Моргантон'),(8813,5681,8768,'Муресвилл'),(8814,5681,8768,'Мэттьюс'),(8815,5681,8768,'Норт-Белмонт'),(8816,5681,8768,'Норт-Вилкесборо'),(8817,5681,8768,'Норт-Конкорд'),(8818,5681,8768,'Ньютон'),(8819,5681,8768,'Оксфорд'),(8820,5681,8768,'Пенелоп'),(8821,5681,8768,'Пинеблуфф'),(8822,5681,8768,'Ралейг'),(8823,5681,8768,'Родхисс'),(8824,5681,8768,'Роквелл'),(8825,5681,8768,'Роки-Маунт'),(8826,5681,8768,'Рутерфордтон'),(8827,5681,8768,'Саммерфилд'),(8828,5681,8768,'Саутерн-Пайнс'),(8829,5681,8768,'Сваннаноа'),(8830,5681,8768,'Силва'),(8831,5681,8768,'Сильвер-Сити'),(8832,5681,8768,'Сталлингс'),(8833,5681,8768,'Стем'),(8834,5681,8768,'Стенли'),(8835,5681,8768,'Уайтвилл'),(8836,5681,8768,'Файт'),(8837,5681,8768,'Фэйеттвилл'),(8838,5681,8768,'Хадсон'),(8839,5681,8768,'Хай-Пойнт'),(8840,5681,8768,'Хантерсвилл'),(8841,5681,8768,'Харкерс-Айленд'),(8842,5681,8768,'Харрисбург'),(8843,5681,8768,'Хендерсонвилл'),(8844,5681,8768,'Хикори'),(8845,5681,8768,'Хилдебран'),(8846,5681,8768,'Чайна-Гров'),(8847,5681,8768,'Чапел-Хилл'),(8848,5681,8768,'Шарлотт'),(8849,5681,8768,'Шелби'),(8850,5681,8768,'Эдентон'),(8851,5681,8768,'Элизабет-Сити'),(8852,5681,8768,'Эллерб'),(8853,5681,8768,'Энка'),(8854,5681,8768,'Эночвилл'),(8855,5681,8768,'Эшвилль'),(8857,5681,8856,'Адамсвиль'),(8858,5681,8856,'Айрон-Сити'),(8859,5681,8856,'Аламо'),(8860,5681,8856,'Алгуд'),(8861,5681,8856,'Александрия'),(8862,5681,8856,'Алкоа'),(8863,5681,8856,'Алтамонт'),(8864,5681,8856,'Атенс'),(8865,5681,8856,'Аубурнтаун'),(8866,5681,8856,'Ашланд-Сити'),(8867,5681,8856,'Бакстер'),(8868,5681,8856,'Бартлетт'),(8869,5681,8856,'Белл Букл'),(8870,5681,8856,'Беллс'),(8871,5681,8856,'Бентон'),(8872,5681,8856,'Берри Хилл'),(8873,5681,8856,'Бетел Спрингс'),(8874,5681,8856,'Билтмор'),(8875,5681,8856,'Блайн'),(8876,5681,8856,'Блоунтвилл'),(8877,5681,8856,'Блуфф-Сити'),(8878,5681,8856,'Брадфорд'),(8879,5681,8856,'Браунсвилл'),(8880,5681,8856,'Брентвуд'),(8881,5681,8856,'Бридвелл-Хейтс'),(8882,5681,8856,'Буллс-Гап'),(8883,5681,8856,'Бунес-Крик'),(8884,5681,8856,'Валден'),(8885,5681,8856,'Вартрак'),(8886,5681,8856,'Винчестер'),(8887,5681,8856,'Виола'),(8888,5681,8856,'Вудбури'),(8889,5681,8856,'Вэлли-Форж'),(8890,5681,8856,'Гадсден'),(8891,5681,8856,'Гаинесборо'),(8892,5681,8856,'Галлатин'),(8893,5681,8856,'Гатес'),(8894,5681,8856,'Германтаун'),(8895,5681,8856,'Гибсон'),(8896,5681,8856,'Глисон'),(8897,5681,8856,'Гордонсвилл'),(8898,5681,8856,'Гринфилд'),(8899,5681,8856,'Грэй'),(8900,5681,8856,'Гудлеттсвилл'),(8901,5681,8856,'Даиси'),(8902,5681,8856,'Декатур'),(8903,5681,8856,'Дечерд'),(8904,5681,8856,'Джеллико'),(8905,5681,8856,'Джефферсон-Сити'),(8906,5681,8856,'Джохнсон-Сити'),(8907,5681,8856,'Диер'),(8908,5681,8856,'Диерсбург'),(8909,5681,8856,'Доил'),(8910,5681,8856,'Доуэллтаун'),(8911,5681,8856,'Дресден'),(8912,5681,8856,'Енвилл'),(8913,5681,8856,'Естилл Спрингс'),(8914,5681,8856,'Етридж'),(8915,5681,8856,'Иглетон-Виллидж'),(8916,5681,8856,'Иорквилл'),(8917,5681,8856,'Ист Ридж'),(8918,5681,8856,'Ист-Кливленд'),(8919,5681,8856,'Йонесборо'),(8920,5681,8856,'Калхоун'),(8921,5681,8856,'Карнс'),(8922,5681,8856,'Картейж'),(8923,5681,8856,'Кауан'),(8924,5681,8856,'Кентон'),(8925,5681,8856,'Кингстон-Спрингс'),(8926,5681,8856,'Кларксвилл'),(8927,5681,8856,'Клевеланд'),(8928,5681,8856,'Клинтон'),(8929,5681,8856,'Коалмонт'),(8930,5681,8856,'Ковингтон'),(8931,5681,8856,'Коллегедал'),(8932,5681,8856,'Коллирвилл'),(8933,5681,8856,'Колониал-Хейтс'),(8934,5681,8856,'Колумбиа'),(8935,5681,8856,'Конкорд'),(8936,5681,8856,'Корнерсвилл'),(8937,5681,8856,'Кросс Плаинс'),(8938,5681,8856,'Кукевилл'),(8939,5681,8856,'Кумберленд-Сити'),(8940,5681,8856,'Ла Вергн'),(8941,5681,8856,'Лавренкебург'),(8942,5681,8856,'Лакесит'),(8943,5681,8856,'Лебанон'),(8944,5681,8856,'Левисбург'),(8945,5681,8856,'Лексингтон'),(8946,5681,8856,'Леноир-Сити'),(8947,5681,8856,'Линнвилл'),(8948,5681,8856,'Линчбург'),(8949,5681,8856,'Лоретто'),(8950,5681,8856,'Лукоут Моунтаин'),(8951,5681,8856,'Манчестер'),(8952,5681,8856,'Маривилл'),(8953,5681,8856,'Мартин'),(8954,5681,8856,'Маунтайн-Сити'),(8955,5681,8856,'Маури-Сити'),(8956,5681,8856,'Медина'),(8957,5681,8856,'Медон'),(8958,5681,8856,'Мемфис'),(8959,5681,8856,'Миддл Валли'),(8960,5681,8856,'Милан'),(8961,5681,8856,'Милледжевилл'),(8962,5681,8856,'Миллингтон'),(8963,5681,8856,'Минор Хилл'),(8964,5681,8856,'Мичи'),(8965,5681,8856,'МкКензи'),(8966,5681,8856,'МкЛеморесвилл'),(8967,5681,8856,'МкМиннвилл'),(8968,5681,8856,'Монтигл'),(8969,5681,8856,'Моррисон'),(8970,5681,8856,'Морристаун'),(8971,5681,8856,'Моунт Юлит'),(8972,5681,8856,'Мошейм'),(8973,5681,8856,'Мунфорд'),(8974,5681,8856,'Мурфрисборо'),(8975,5681,8856,'Нашвилл'),(8976,5681,8856,'Ниота'),(8977,5681,8856,'Ноксвилл'),(8978,5681,8856,'Норманди'),(8979,5681,8856,'Норрис'),(8980,5681,8856,'Нью-Маркет'),(8981,5681,8856,'Оак Ридж'),(8982,5681,8856,'Обион'),(8983,5681,8856,'Олбани'),(8984,5681,8856,'Оливер Спрингс'),(8985,5681,8856,'Онейда'),(8986,5681,8856,'Оребанк'),(8987,5681,8856,'Пауелл'),(8988,5681,8856,'Пеграм'),(8989,5681,8856,'Пипертон'),(8990,5681,8856,'Пуласки'),(8991,5681,8856,'Ред Банк'),(8992,5681,8856,'Риджетоп'),(8993,5681,8856,'Рипли'),(8994,5681,8856,'Рокфорд'),(8995,5681,8856,'Руссельвиль'),(8996,5681,8856,'Рутерфорд'),(8997,5681,8856,'СалливаноГарденс'),(8998,5681,8856,'Саут-Картейж'),(8999,5681,8856,'Саут-Кливленд'),(9000,5681,8856,'Саут-Клинтон'),(9001,5681,8856,'Свитуотер'),(9002,5681,8856,'Севани'),(9003,5681,8856,'Селмер'),(9004,5681,8856,'Сентертаун'),(9005,5681,8856,'Сигнал Моунтаин'),(9006,5681,8856,'Силертон'),(9007,5681,8856,'Слэйден'),(9008,5681,8856,'Смирна'),(9009,5681,8856,'Смитвилл'),(9010,5681,8856,'Содди-Даиси'),(9011,5681,8856,'Спарта'),(9012,5681,8856,'Спринг-Сити'),(9013,5681,8856,'Спургеон'),(9014,5681,8856,'Суммит'),(9015,5681,8856,'Траки-Сити'),(9016,5681,8856,'Трезевант'),(9017,5681,8856,'Трентон'),(9018,5681,8856,'Тримбл'),(9019,5681,8856,'Туллахома'),(9020,5681,8856,'Тун'),(9021,5681,8856,'Уайт-Оак'),(9022,5681,8856,'Уайт-Пин'),(9023,5681,8856,'Уайт-Хаус'),(9024,5681,8856,'Фингер'),(9025,5681,8856,'Фолл-Бранч'),(9026,5681,8856,'Франклин'),(9027,5681,8856,'Фриндсвилл'),(9028,5681,8856,'Халлс'),(9029,5681,8856,'Хамптон'),(9030,5681,8856,'Харрисон'),(9031,5681,8856,'Хартсвилл'),(9032,5681,8856,'Хендерсон'),(9033,5681,8856,'Хендерсонвилл'),(9034,5681,8856,'Хеннинг'),(9035,5681,8856,'Хорнсби'),(9036,5681,8856,'Хохенвалд'),(9037,5681,8856,'Хумболдт'),(9038,5681,8856,'Чаттануга'),(9039,5681,8856,'Шарон'),(9040,5681,8856,'Шелбивилл'),(9041,5681,8856,'Элизабеттон'),(9042,5681,8856,'Эрвин'),(9043,5681,8856,'Этвуд'),(9044,5681,8856,'Этова'),(9045,5681,8856,'Яксон'),(9046,5681,8856,'Яместаун'),(9048,5681,9047,'Абилин'),(9049,5681,9047,'Аламо-Хейгтс'),(9050,5681,9047,'Алдайн'),(9051,5681,9047,'Алпин'),(9052,5681,9047,'Амарилло'),(9053,5681,9047,'Арлингтон'),(9054,5681,9047,'Арп'),(9055,5681,9047,'Аспермонт'),(9056,5681,9047,'Бакхольтс'),(9057,5681,9047,'Балконес-Хейгтс'),(9058,5681,9047,'Банкер-Хилл-Виллидж'),(9059,5681,9047,'Барнет'),(9060,5681,9047,'Беверли-Хиллс'),(9061,5681,9047,'Беллэйр'),(9062,5681,9047,'Биг-Спринг'),(9063,5681,9047,'Брайан'),(9064,5681,9047,'Браунсвилл'),(9065,5681,9047,'Брекенридж'),(9066,5681,9047,'Бренхам'),(9067,5681,9047,'Бьюмонт'),(9068,5681,9047,'Бэйтаун'),(9069,5681,9047,'Вако'),(9070,5681,9047,'Васком'),(9071,5681,9047,'Вернон'),(9072,5681,9047,'Вест-Лейк-Хиллс'),(9073,5681,9047,'Вест-Юниверсити-Плэйс'),(9074,5681,9047,'Вестворт'),(9075,5681,9047,'Вестовер-Хиллс'),(9076,5681,9047,'Викториа'),(9077,5681,9047,'Виндкрест'),(9078,5681,9047,'Вичита-Фоллс'),(9079,5681,9047,'Вольффорт'),(9080,5681,9047,'Галвестон'),(9081,5681,9047,'Галена-Парк'),(9082,5681,9047,'Гарленд'),(9083,5681,9047,'Гранд-Прейри'),(9084,5681,9047,'Дайнгерфилд'),(9085,5681,9047,'Даллас'),(9086,5681,9047,'Далхарт'),(9087,5681,9047,'Дель-Рио'),(9088,5681,9047,'Дентон'),(9089,5681,9047,'Джакинто-Сити'),(9090,5681,9047,'Джанкшин'),(9091,5681,9047,'Джордантон'),(9092,5681,9047,'Дир-Парк'),(9093,5681,9047,'Идалоу'),(9094,5681,9047,'Ирвинг'),(9095,5681,9047,'Камерон'),(9096,5681,9047,'Кастл-Хиллс'),(9097,5681,9047,'Кермит'),(9098,5681,9047,'Киллин'),(9099,5681,9047,'Кингсвилл'),(9100,5681,9047,'Кирби'),(9101,5681,9047,'Кловерлиф'),(9102,5681,9047,'Комбес'),(9103,5681,9047,'Корпус-Кристи'),(9104,5681,9047,'Купер'),(9105,5681,9047,'Куэро'),(9106,5681,9047,'Лаббок'),(9107,5681,9047,'Лайон-Вэлли'),(9108,5681,9047,'Лакленд база ВВС'),(9109,5681,9047,'Ларедо'),(9110,5681,9047,'Лейк-Ворт'),(9111,5681,9047,'Либерти'),(9112,5681,9047,'Либерти-Сити'),(9113,5681,9047,'Лонгвью'),(9114,5681,9047,'Лулинг'),(9115,5681,9047,'Мак-Аллен'),(9116,5681,9047,'Мак-Кинни'),(9117,5681,9047,'Малакофф'),(9118,5681,9047,'Манор'),(9119,5681,9047,'Мескуит'),(9120,5681,9047,'Мидленд'),(9121,5681,9047,'Миссури-Сити'),(9122,5681,9047,'Норт-Ричланд-Хиллс'),(9123,5681,9047,'Нью-Дил'),(9124,5681,9047,'Нью-Хоум'),(9125,5681,9047,'Овертон'),(9126,5681,9047,'Одем'),(9127,5681,9047,'Одесса'),(9128,5681,9047,'Олбани'),(9129,5681,9047,'Олмос-Парк'),(9130,5681,9047,'Олни'),(9131,5681,9047,'Остин'),(9132,5681,9047,'Пайни-Пойнт-Виллидж'),(9133,5681,9047,'Пампа'),(9134,5681,9047,'Пасадена'),(9135,5681,9047,'Пирленд'),(9136,5681,9047,'Пирсалл'),(9137,5681,9047,'Порт-Артур'),(9138,5681,9047,'Портланд'),(9139,5681,9047,'Праймера'),(9140,5681,9047,'Пфлугервилл'),(9141,5681,9047,'Ривер-Оакс'),(9142,5681,9047,'Ричардсон'),(9143,5681,9047,'Ричланд-Хиллс'),(9144,5681,9047,'Робстаун'),(9145,5681,9047,'Роллингвуд'),(9146,5681,9047,'Сагинау'),(9147,5681,9047,'Сан-Анжело'),(9148,5681,9047,'Сан-Антонио'),(9149,5681,9047,'Сан-Бенито'),(9150,5681,9047,'Сансет-Вэлли'),(9151,5681,9047,'Саут-Хьюстон'),(9152,5681,9047,'Саутсайд-Плэйс'),(9153,5681,9047,'Сегуин'),(9154,5681,9047,'Сенсом-Парк-Виллидж'),(9155,5681,9047,'Слатон'),(9156,5681,9047,'Сплендора'),(9157,5681,9047,'Стаффорд'),(9158,5681,9047,'Тафт'),(9159,5681,9047,'Тексаркана'),(9160,5681,9047,'Тексас-Сити'),(9161,5681,9047,'Темпл'),(9162,5681,9047,'Террелл'),(9163,5681,9047,'Террелл-Хиллс'),(9164,5681,9047,'Тилер'),(9165,5681,9047,'Торндейл'),(9166,5681,9047,'Тралл'),(9167,5681,9047,'Уайт-Сеттлмент'),(9168,5681,9047,'Фармерс-Бранч'),(9169,5681,9047,'Форест-Хилл'),(9170,5681,9047,'Форт-Ворт'),(9171,5681,9047,'Форт-Уэрт'),(9172,5681,9047,'Халтом-Сити'),(9173,5681,9047,'Хантерс-Крик-Виллидж'),(9174,5681,9047,'Харлинген'),(9175,5681,9047,'Харст'),(9176,5681,9047,'Хедвиг-Виллидж'),(9177,5681,9047,'Хилл-Кантри-Виллидж'),(9178,5681,9047,'Худсон'),(9179,5681,9047,'Хьюстон'),(9180,5681,9047,'Шаллоуотер'),(9181,5681,9047,'Шерман'),(9182,5681,9047,'Эль-Кампо'),(9183,5681,9047,'Эль-Пасо'),(9184,5681,9047,'Эндрюс'),(9186,5681,9185,'Азали-Парк'),(9187,5681,9185,'Айвес-Эстейтс'),(9188,5681,9185,'Алачуа'),(9189,5681,9185,'Апалачикола'),(9190,5681,9185,'Апопка'),(9191,5681,9185,'Арчер'),(9192,5681,9185,'Атлантик-Бич'),(9193,5681,9185,'Багдад'),(9194,5681,9185,'Бал-Харбор'),(9195,5681,9185,'Балдвин'),(9196,5681,9185,'Банч-Парк'),(9197,5681,9185,'Бартау'),(9198,5681,9185,'Беверли-Хиллс'),(9199,5681,9185,'Белл-Глейд'),(9200,5681,9185,'Беллайр'),(9201,5681,9185,'Беллиир'),(9202,5681,9185,'Беллиир-Бич'),(9203,5681,9185,'Беллиир-Блуффс'),(9204,5681,9185,'Бельвью'),(9205,5681,9185,'Би-Ридж'),(9206,5681,9185,'Бискейн-Парк'),(9207,5681,9185,'Бойнтон-Бич'),(9208,5681,9185,'Бока-Рейтон'),(9209,5681,9185,'Брадентон'),(9210,5681,9185,'Брандон'),(9211,5681,9185,'Браунс-Виллидж'),(9212,5681,9185,'Браунсвилл'),(9213,5681,9185,'Брент'),(9214,5681,9185,'Бровардейл'),(9215,5681,9185,'Бродвью-Парк'),(9216,5681,9185,'Брукер'),(9217,5681,9185,'Бэй Пинес'),(9218,5681,9185,'Бэй-Харбор-Айлендс'),(9219,5681,9185,'Бэйшор-Гарденс'),(9220,5681,9185,'Валдо'),(9221,5681,9185,'Валпараисо'),(9222,5681,9185,'Вамо'),(9223,5681,9185,'Вахнета'),(9224,5681,9185,'Векива-Спрингс'),(9225,5681,9185,'Веро-Бич'),(9226,5681,9185,'Вест-Винтер-Хавен'),(9227,5681,9185,'Вест-И-Галли'),(9228,5681,9185,'Вест-Майами'),(9229,5681,9185,'Вест-Палм-Бич'),(9230,5681,9185,'Вествуд-Лейкс'),(9231,5681,9185,'Вествью'),(9232,5681,9185,'Вестчестер'),(9233,5681,9185,'Виллистон'),(9234,5681,9185,'Вилтон-Манорс'),(9235,5681,9185,'Виндермер'),(9236,5681,9185,'Винтер-Парк'),(9237,5681,9185,'Винтер-Хавен'),(9238,5681,9185,'Виргиниа-Гарденс'),(9239,5681,9185,'Вортингтон-Спрингс'),(9240,5681,9185,'Вригт'),(9241,5681,9185,'Гавана'),(9242,5681,9185,'Гайнесвилл'),(9243,5681,9185,'Галф-Гейт-Эстатс'),(9244,5681,9185,'Галф-Стрим'),(9245,5681,9185,'Галфпорт'),(9246,5681,9185,'Гарлем'),(9247,5681,9185,'Гасьенда-Виллидж'),(9248,5681,9185,'Гиффорд'),(9249,5681,9185,'Глен-Ридж'),(9250,5681,9185,'Гленвар-Хейгтс'),(9251,5681,9185,'Голден-Бич'),(9252,5681,9185,'Голливуд'),(9253,5681,9185,'Гордонвилл'),(9254,5681,9185,'Гоулдинг'),(9255,5681,9185,'Гоулдс'),(9256,5681,9185,'Гракевилл'),(9257,5681,9185,'Грин-Ков-Спрингс'),(9258,5681,9185,'Гринакрес-Сити'),(9259,5681,9185,'Дайтона-Бич'),(9260,5681,9185,'Даниа'),(9261,5681,9185,'Деви'),(9262,5681,9185,'Джасмин-Эстатс'),(9263,5681,9185,'Джексонвилл'),(9264,5681,9185,'Джексонвилл-Бич'),(9265,5681,9185,'Довер'),(9266,5681,9185,'Дуннеллон'),(9267,5681,9185,'Египт-Лейк'),(9268,5681,9185,'Еглин Аир Форк Бас'),(9269,5681,9185,'Енсли'),(9270,5681,9185,'Есто'),(9271,5681,9185,'Игл-Лейк'),(9272,5681,9185,'Инвуд'),(9273,5681,9185,'Индиан-Рокс-Бич'),(9274,5681,9185,'Индиан-Харбор-Бич'),(9275,5681,9185,'Индиан-Шорес'),(9276,5681,9185,'Ист-Лейк-Парк'),(9277,5681,9185,'Итон-Парк'),(9278,5681,9185,'Итонвилл'),(9279,5681,9185,'Каллавэй'),(9280,5681,9185,'Каллахан'),(9281,5681,9185,'Кампбеллтон'),(9282,5681,9185,'Карол-Сити'),(9283,5681,9185,'Каррабелл'),(9284,5681,9185,'Кейп-Корал'),(9285,5681,9185,'Кендалл'),(9286,5681,9185,'Ки-Бискейн'),(9287,5681,9185,'Ки-Уэст'),(9288,5681,9185,'Киллирн Естатес'),(9289,5681,9185,'Кипресс-Гарденс'),(9290,5681,9185,'Кистон-Хейтс'),(9291,5681,9185,'Клауд-Лейк'),(9292,5681,9185,'Клевистон'),(9293,5681,9185,'Клейр-Мел-Сити'),(9294,5681,9185,'Кливленд'),(9295,5681,9185,'Клирватер'),(9296,5681,9185,'Кокоа-Бич'),(9297,5681,9185,'Коконат-Крик'),(9298,5681,9185,'Колльер-Сити'),(9299,5681,9185,'Комби-Сеттлмент'),(9300,5681,9185,'Конвей'),(9301,5681,9185,'Корал-Габлс'),(9302,5681,9185,'Лак Магдален'),(9303,5681,9185,'Лак-Керролл'),(9304,5681,9185,'Лакеланд'),(9305,5681,9185,'Лантана'),(9306,5681,9185,'Ларго'),(9307,5681,9185,'Лаудердейл-бай-ти-Си'),(9308,5681,9185,'Лаудердейл-Лейкс'),(9309,5681,9185,'Лаудерхилл'),(9310,5681,9185,'Леди-Лейк'),(9311,5681,9185,'Лей-Люцерн'),(9312,5681,9185,'Лейзи-Лейк'),(9313,5681,9185,'Лейк-Альфред'),(9314,5681,9185,'Лейк-Ворт'),(9315,5681,9185,'Лейк-Кларк-Шорес'),(9316,5681,9185,'Лейквуд-Парк'),(9317,5681,9185,'Лейсур-Сити'),(9318,5681,9185,'Лето'),(9319,5681,9185,'Лив-Оак'),(9320,5681,9185,'Лилман'),(9321,5681,9185,'Линн-Хавен'),(9322,5681,9185,'Лисбург'),(9323,5681,9185,'Локхарт'),(9324,5681,9185,'Лонгбоат-Ки'),(9325,5681,9185,'Лутз'),(9326,5681,9185,'Мадейра-Бич'),(9327,5681,9185,'Майами'),(9328,5681,9185,'Майами-Бич'),(9329,5681,9185,'Майами-Спрингс'),(9330,5681,9185,'Майами-Шорес'),(9331,5681,9185,'Майтленд (Лейк Майтленд)'),(9332,5681,9185,'МакИнтош'),(9333,5681,9185,'Малабар'),(9334,5681,9185,'Маналапан'),(9335,5681,9185,'Манго'),(9336,5681,9185,'Мангониа-Парк'),(9337,5681,9185,'Маратон'),(9338,5681,9185,'Маргейт'),(9339,5681,9185,'Марианна'),(9340,5681,9185,'Медли'),(9341,5681,9185,'Мелрос-Парк'),(9342,5681,9185,'Мельбурн'),(9343,5681,9185,'Мельбурн-Виллидж'),(9344,5681,9185,'Мерритт-Айленд'),(9345,5681,9185,'Мидоубрук-Террас'),(9346,5681,9185,'Миканопи'),(9347,5681,9185,'Милтон'),(9348,5681,9185,'Мирамар'),(9349,5681,9185,'Молино'),(9350,5681,9185,'Наплес'),(9351,5681,9185,'Нептун-Бич'),(9352,5681,9185,'Никевилл'),(9353,5681,9185,'Нома'),(9354,5681,9185,'Норвуд'),(9355,5681,9185,'Норланд'),(9356,5681,9185,'Норт-Бэй-Виллидж'),(9357,5681,9185,'Норт-Майами'),(9358,5681,9185,'Норт-Майами-Бич'),(9359,5681,9185,'Норт-Порт'),(9360,5681,9185,'Норт-Редингтон-Бич'),(9361,5681,9185,'Норт-Форт-Майерс'),(9362,5681,9185,'Норт-Эндрюс-Гарденс'),(9363,5681,9185,'Ньюберри'),(9364,5681,9185,'Оакленд-Парк'),(9365,5681,9185,'Обурндейл'),(9366,5681,9185,'Окала'),(9367,5681,9185,'Окин-Сити'),(9368,5681,9185,'Олдсмар'),(9369,5681,9185,'Олимпиа-Хейгтс'),(9370,5681,9185,'Онеко'),(9371,5681,9185,'Опа-Лока'),(9372,5681,9185,'Оранж-Парк'),(9373,5681,9185,'Оринт-Парк'),(9374,5681,9185,'Орландо'),(9375,5681,9185,'Орловиста'),(9376,5681,9185,'Ормонд-Бич'),(9377,5681,9185,'Оспри'),(9378,5681,9185,'Оушн-Ридж'),(9379,5681,9185,'Оюс'),(9380,5681,9185,'Пайн-Хиллс'),(9381,5681,9185,'Пайнвуд'),(9382,5681,9185,'Пак'),(9383,5681,9185,'Пакстон'),(9384,5681,9185,'Палм-Бич'),(9385,5681,9185,'Палм-Ривер'),(9386,5681,9185,'Палм-Шорес'),(9387,5681,9185,'Панама-Сити'),(9388,5681,9185,'Паркер'),(9389,5681,9185,'Пахоки'),(9390,5681,9185,'Пемброк-Пайнс'),(9391,5681,9185,'Пемброк-Парк'),(9392,5681,9185,'Пенни-Фармс'),(9393,5681,9185,'Пенсакола'),(9394,5681,9185,'Перри'),(9395,5681,9185,'Пинеллас-Парк'),(9396,5681,9185,'Плантешн'),(9397,5681,9185,'Помпано-Бич'),(9398,5681,9185,'Помпано-Парк'),(9399,5681,9185,'Порт-Санта-Лючия'),(9400,5681,9185,'Порт-Шарлотт'),(9401,5681,9185,'Принстон'),(9402,5681,9185,'Пунта-Горда'),(9403,5681,9185,'Реддик'),(9404,5681,9185,'Редингтон-Бич'),(9405,5681,9185,'Редингтон-Шорес'),(9406,5681,9185,'Ричмонд-Хейгтс'),(9407,5681,9185,'Рускин'),(9408,5681,9185,'Самосет'),(9409,5681,9185,'Санни-Айлс'),(9410,5681,9185,'Санрайс'),(9411,5681,9185,'Сант-Аугустин'),(9412,5681,9185,'Сант-Петерсбург'),(9413,5681,9185,'Сант-Петерсбург-Бич'),(9414,5681,9185,'Сарасота'),(9415,5681,9185,'Сарасота-Спрингс'),(9416,5681,9185,'Сарфсайд'),(9417,5681,9185,'Сателлайт-Бич'),(9418,5681,9185,'Саут-Апопка'),(9419,5681,9185,'Саут-Бэй'),(9420,5681,9185,'Саут-Дайтона'),(9421,5681,9185,'Саут-Майами'),(9422,5681,9185,'Саут-Майами-Хейгтс'),(9423,5681,9185,'Саут-Палм-Бич'),(9424,5681,9185,'Саут-Пасадена'),(9425,5681,9185,'Саутгейт'),(9426,5681,9185,'Сафти-Харбор'),(9427,5681,9185,'Свитватер-Крик'),(9428,5681,9185,'Свитвотер'),(9429,5681,9185,'Седар-Гров'),(9430,5681,9185,'Сентури'),(9431,5681,9185,'Сеффнер'),(9432,5681,9185,'Си-Ранч-Лейкс'),(9433,5681,9185,'Сиеста-Ки'),(9434,5681,9185,'Солана'),(9435,5681,9185,'Спрингфилд'),(9436,5681,9185,'Таварес'),(9437,5681,9185,'Тавернир'),(9438,5681,9185,'Талахасси'),(9439,5681,9185,'Тамайами'),(9440,5681,9185,'Тамарак'),(9441,5681,9185,'Тампа'),(9442,5681,9185,'Темпл-Террас'),(9443,5681,9185,'Тик'),(9444,5681,9185,'Титусвилл'),(9445,5681,9185,'Трайлер-Эстатс'),(9446,5681,9185,'Трежа-Айленд'),(9447,5681,9185,'Уайтфилд-Эстатс'),(9448,5681,9185,'Уматилла'),(9449,5681,9185,'Файрвью-Шорес'),(9450,5681,9185,'Ферн-Парк'),(9451,5681,9185,'Ферри Пасс'),(9452,5681,9185,'Форест-Сити'),(9453,5681,9185,'Форт-Лаудердейл'),(9454,5681,9185,'Форт-Майерс'),(9455,5681,9185,'Форт-Мид'),(9456,5681,9185,'Форт-Пирс'),(9457,5681,9185,'Фрутвилл'),(9458,5681,9185,'Фрутланд-Парк'),(9459,5681,9185,'Хаверхилл'),(9460,5681,9185,'Хавторн'),(9461,5681,9185,'Хай-Пойнт'),(9462,5681,9185,'Хайленд-Парк'),(9463,5681,9185,'Хайленд-Сити'),(9464,5681,9185,'Хамптон'),(9465,5681,9185,'Хиалеа'),(9466,5681,9185,'Хиполуксо'),(9467,5681,9185,'Холден-Хейгтс'),(9468,5681,9185,'Холли-Хилл'),(9469,5681,9185,'Хоместид-Айр-Форс-Бэйс'),(9470,5681,9185,'Шалимар'),(9471,5681,9185,'Шарлотт-Парк'),(9472,5681,9185,'Шарлотт-Харбор'),(9473,5681,9185,'Эджвуд'),(9474,5681,9185,'Элоис'),(9475,5681,9185,'Эль-Портал'),(9476,5681,9185,'Яи'),(9478,5681,9477,'Абердин'),(9479,5681,9477,'Ватертаун'),(9480,5681,9477,'Гурон'),(9481,5681,9477,'Кантон'),(9482,5681,9477,'Митчелл'),(9483,5681,9477,'Пирр'),(9484,5681,9477,'Рапид-Сити'),(9485,5681,9477,'Редфилд'),(9486,5681,9477,'Сиу-Фоллс'),(9487,5681,9477,'Спирфиш'),(9488,5681,9477,'Хот-Спрингс'),(9490,5681,9489,'Авондейл'),(9491,5681,9489,'Андерсон'),(9492,5681,9489,'Аркадиа-Лейкс'),(9493,5681,9489,'Валенсиа-Хейгтс'),(9494,5681,9489,'Вест-Колумбиа'),(9495,5681,9489,'Вудфилд'),(9496,5681,9489,'Гринвилл'),(9497,5681,9489,'Гринвуд'),(9498,5681,9489,'Дентсвилл'),(9499,5681,9489,'Джеймс-Айленд'),(9500,5681,9489,'Диллон'),(9501,5681,9489,'Капитол-Вью'),(9502,5681,9489,'Кейси'),(9503,5681,9489,'Клинтон'),(9504,5681,9489,'Колумбиа'),(9505,5681,9489,'Пайнридж'),(9506,5681,9489,'Рок-Хилл'),(9507,5681,9489,'Самтер'),(9508,5681,9489,'Сенека'),(9509,5681,9489,'Спартанбург'),(9510,5681,9489,'Флоренс'),(9511,5681,9489,'Форест-Акрес'),(9512,5681,9489,'Хемингуэй'),(9513,5681,9489,'Чарльстон'),(9514,5681,9489,'Чарльстон-Хейгтс'),(9516,5681,9515,'Американ-Форк'),(9517,5681,9515,'Беннион'),(9518,5681,9515,'Бивер'),(9519,5681,9515,'Боунтифул'),(9520,5681,9515,'Бригам-Сити'),(9521,5681,9515,'Вал-Верда'),(9522,5681,9515,'Вашингтон-Террас'),(9523,5681,9515,'Вернал'),(9524,5681,9515,'Вест-Боунтифул'),(9525,5681,9515,'Вест-Джордан'),(9526,5681,9515,'Вест-Пойнт'),(9527,5681,9515,'Вудс-Кросс'),(9528,5681,9515,'Ганнисон'),(9529,5681,9515,'Гранит-Парк'),(9530,5681,9515,'Делта'),(9531,5681,9515,'Ист-Лэйтон'),(9532,5681,9515,'Ист-Миллкрик'),(9533,5681,9515,'Канаб'),(9534,5681,9515,'Кирнс'),(9535,5681,9515,'Клинтон'),(9536,5681,9515,'Клирфилд'),(9537,5681,9515,'Коттонвуд-Хейгтс'),(9538,5681,9515,'Линдон'),(9539,5681,9515,'Логан'),(9540,5681,9515,'Маунт-Олимпус'),(9541,5681,9515,'Мидвейл'),(9542,5681,9515,'Милфорд'),(9543,5681,9515,'Моаб'),(9544,5681,9515,'Монтичелло'),(9545,5681,9515,'Моунт-Плисант'),(9546,5681,9515,'Муррей'),(9547,5681,9515,'Нефи'),(9548,5681,9515,'Норт-Солт-Лейк'),(9549,5681,9515,'Огден'),(9550,5681,9515,'Орем'),(9551,5681,9515,'Пангуитч'),(9552,5681,9515,'Плисант-Гров'),(9553,5681,9515,'Прик'),(9554,5681,9515,'Прово'),(9555,5681,9515,'Ривердейл'),(9556,5681,9515,'Ричфилд'),(9557,5681,9515,'Рой'),(9558,5681,9515,'Русвельт'),(9559,5681,9515,'Санди'),(9560,5681,9515,'Сансет'),(9561,5681,9515,'Сант-Джордж'),(9562,5681,9515,'Саут-Вебер'),(9563,5681,9515,'Саут-Коттонвуд'),(9564,5681,9515,'Саут-Огден'),(9565,5681,9515,'Саут-Солт-Лейк'),(9566,5681,9515,'Седар-Сити'),(9567,5681,9515,'Солт-Лейк-Сити'),(9568,5681,9515,'Спрингвилл'),(9569,5681,9515,'Тэйлорсвилл'),(9570,5681,9515,'Уинта'),(9571,5681,9515,'Филлмор'),(9572,5681,9515,'Харрисвилл'),(9573,5681,9515,'Холладей'),(9574,5681,9515,'Юнион'),(9577,9575,9576,'Ванч'),(9578,9575,9576,'Висхарв'),(9579,9575,9576,'Вранг'),(9580,9575,9576,'Ишкашим'),(9581,9575,9576,'Кудара'),(9582,9575,9576,'Мургаб'),(9583,9575,9576,'Рангкуль'),(9584,9575,9576,'Рушан'),(9585,9575,9576,'Токтомуш'),(9586,9575,9576,'Хорог'),(9587,9575,9576,'Шазуд'),(9588,9575,9576,'Шаймак'),(9590,9575,9589,'Дангара'),(9591,9575,9589,'Куляб'),(9592,9575,9589,'Лениградский'),(9593,9575,9589,'Пархар'),(9594,9575,9589,'Советский'),(9595,9575,9589,'Ховалинг'),(9597,9575,9596,'Калининабад'),(9598,9575,9596,'Колхозабад'),(9599,9575,9596,'Куйбышевский'),(9600,9575,9596,'Курган-Тюбе'),(9601,9575,9596,'Нижний Пяндж'),(9602,9575,9596,'Пяндж'),(9603,9575,9596,'Шаартуз'),(9604,9575,9596,'Яван'),(9606,9575,9605,'Адрасман'),(9607,9575,9605,'Айни'),(9608,9575,9605,'Ашт'),(9609,9575,9605,'Бустон'),(9610,9575,9605,'Ганчи'),(9611,9575,9605,'Гафуров'),(9612,9575,9605,'Зафарабад'),(9613,9575,9605,'Зеравшан'),(9614,9575,9605,'Исфара'),(9615,9575,9605,'Кайракуум'),(9616,9575,9605,'Канибадам'),(9617,9575,9605,'Кансай'),(9619,9575,9605,'Нау'),(9620,9575,9605,'Наугарзан'),(9621,9575,9605,'Пенджикент'),(9622,9575,9605,'Пролетарск'),(9623,9575,9605,'Табошар'),(9624,9575,9605,'Ура-Тюбе'),(9625,9575,9605,'Чкаловск'),(9626,9575,9605,'Шураб'),(9628,9575,9627,'Варзоб'),(9629,9575,9627,'Гарм'),(9630,9575,9627,'Гиссар'),(9631,9575,9627,'Джиргатель'),(9632,9575,9627,'Душанбе'),(9633,9575,9627,'Калаи-Хумб'),(9634,9575,9627,'Насруд'),(9635,9575,9627,'Нурек'),(9636,9575,9627,'Турсунзаде'),(9637,9575,9627,'Файзабад'),(9640,9638,9639,'Ашхабад'),(9641,9638,9639,'Бабадурмаз'),(9642,9638,9639,'Бахарден'),(9643,9638,9639,'Бахардок'),(9644,9638,9639,'Безмеин'),(9645,9638,9639,'Геок-Тепе'),(9646,9638,9639,'Дарваза'),(9647,9638,9639,'Душак'),(9648,9638,9639,'Каахка'),(9649,9638,9639,'Кировск'),(9650,9638,9639,'Полехатум'),(9651,9638,9639,'Серахс'),(9652,9638,9639,'Теджен'),(9654,9638,9653,'Бекдаш'),(9655,9638,9653,'Гасан-Кули'),(9656,9638,9653,'Джанга'),(9657,9638,9653,'Джебел'),(9658,9638,9653,'Казанджик'),(9659,9638,9653,'Кара-Кала'),(9660,9638,9653,'Карагель'),(9661,9638,9653,'Кизыл-Арват'),(9662,9638,9653,'Кизыл-Атрек'),(9663,9638,9653,'Кизыл-Су'),(9664,9638,9653,'Красноводск'),(9665,9638,9653,'Кум-Даг'),(9666,9638,9653,'Небит-Даг'),(9667,9638,9653,'Уфра'),(9668,9638,9653,'Чагыл'),(9669,9638,9653,'Челекен'),(9671,9638,9670,'Байрам-Али'),(9672,9638,9670,'Захмет'),(9673,9638,9670,'Иолотань'),(9674,9638,9670,'Кушка'),(9675,9638,9670,'Мары'),(9676,9638,9670,'Сакар-Чага'),(9677,9638,9670,'Сандыкачи'),(9678,9638,9670,'Тахта-Базар'),(9679,9638,9670,'Ташкепри'),(9680,9638,9670,'Туркмен-Кала'),(9681,9638,9670,'Уч-Аджи'),(9683,9638,9682,'Куня-Ургенч'),(9684,9638,9682,'Ташауз'),(9686,9638,9685,'Аму-Дарья'),(9687,9638,9685,'Газ-Ачак'),(9688,9638,9685,'Гаурдак'),(9689,9638,9685,'Дарган-Ата'),(9690,9638,9685,'Дейнау'),(9691,9638,9685,'Карабекаул'),(9692,9638,9685,'Керки'),(9693,9638,9685,'Мукры'),(9694,9638,9685,'Нефтезаводск'),(9695,9638,9685,'Сакар'),(9696,9638,9685,'Саят'),(9697,9638,9685,'Фараб'),(9698,9638,9685,'Халач'),(9699,9638,9685,'Чарджоу'),(9700,9638,9685,'Чаршанга'),(9703,9701,9702,'Кев'),(9704,9701,9702,'Теркс-Айлендс'),(9707,9705,9706,'Бартын'),(9709,9705,9708,'Байбурт'),(9711,9705,9710,'Карабук'),(9713,9705,9712,'Адана'),(9715,9705,9714,'Айдын'),(9717,9705,9716,'Амасья'),(9719,9705,9718,'Анкара'),(9721,9705,9720,'Анталия'),(9723,9705,9722,'Артвин'),(9725,9705,9724,'Афьон'),(9727,9705,9726,'Балыкесир'),(9729,9705,9728,'Биледжик'),(9731,9705,9730,'Бурса'),(9733,9705,9732,'Газиантеп'),(9735,9705,9734,'Денизли'),(9737,9705,9736,'Измир'),(9739,9705,9738,'Испарта'),(9741,9705,9740,'Мерсин'),(9743,9705,9742,'Кэйсери'),(9745,9705,9744,'Карс'),(9747,9705,9746,'Измит'),(9748,9705,9746,'Кокели'),(9750,9705,9749,'Кониа'),(9752,9705,9751,'Кириккал'),(9754,9705,9753,'Кутахиа'),(9756,9705,9755,'Малатиа'),(9758,9705,9757,'Маниса'),(9760,9705,9759,'Адапазари'),(9762,9705,9761,'Самсун'),(9764,9705,9763,'Сивас'),(9766,9705,9765,'Стамбул'),(9768,9705,9767,'Трабзон'),(9769,9705,9767,'Требизонд'),(9771,9705,9770,'Корум'),(9773,9705,9772,'Эдирн'),(9775,9705,9774,'Элазиг'),(9777,9705,9776,'Эрзинкан'),(9779,9705,9778,'Эрзурум'),(9781,9705,9780,'Ескисехир'),(9784,9782,9783,'Йиня'),(9786,9782,9785,'Кампала'),(9789,9787,9788,'Алтынкуль'),(9790,9787,9788,'Андижан'),(9791,9787,9788,'Балыкчи'),(9792,9787,9788,'Ленинск'),(9793,9787,9788,'Мархамат'),(9794,9787,9788,'Советабад'),(9795,9787,9788,'Шахрихан'),(9797,9787,9796,'Алат'),(9798,9787,9796,'Бухара'),(9799,9787,9796,'Вабкент'),(9800,9787,9796,'Газли'),(9801,9787,9796,'Галаасия'),(9802,9787,9796,'Гиждуван'),(9803,9787,9796,'Каган'),(9804,9787,9796,'Каракуль'),(9805,9787,9796,'Ромитан'),(9807,9787,9806,'Галляарал'),(9808,9787,9806,'Джизак'),(9809,9787,9806,'Заамин'),(9810,9787,9806,'Койташ'),(9811,9787,9806,'Усмат'),(9812,9787,9806,'Янгикишлак'),(9814,9787,9813,'Кегейли'),(9815,9787,9813,'Кунград'),(9816,9787,9813,'Мангит'),(9817,9787,9813,'Муйнак'),(9818,9787,9813,'Нукус'),(9819,9787,9813,'Тахиаташ'),(9820,9787,9813,'Тахтакупыр'),(9821,9787,9813,'Турткуль'),(9822,9787,9813,'Ходжейли'),(9823,9787,9813,'Чимбай'),(9824,9787,9813,'Шуманай'),(9826,9787,9825,'Бешкент'),(9827,9787,9825,'Гузар'),(9828,9787,9825,'Карши'),(9829,9787,9825,'Касан'),(9830,9787,9825,'Китаб'),(9831,9787,9825,'Шахрисабз'),(9833,9787,9832,'Навои'),(9834,9787,9832,'Тамдыбулак'),(9835,9787,9832,'Учкудук'),(9837,9787,9836,'Касансай'),(9838,9787,9836,'Наманган'),(9839,9787,9836,'Пап'),(9840,9787,9836,'Учкурган'),(9841,9787,9836,'Чартак'),(9842,9787,9836,'Чуст'),(9843,9787,9836,'Янгикурган'),(9845,9787,9844,'Акташ'),(9846,9787,9844,'Ингичка'),(9847,9787,9844,'Каттакурган'),(9848,9787,9844,'Красногвардейск'),(9849,9787,9844,'Самарканд'),(9850,9787,9844,'Ургут'),(9852,9787,9851,'Байсун'),(9853,9787,9851,'Денау'),(9854,9787,9851,'Карлук'),(9855,9787,9851,'Термез'),(9856,9787,9851,'Узун'),(9857,9787,9851,'Шерабад'),(9858,9787,9851,'Шурчи'),(9860,9787,9859,'Бахт'),(9861,9787,9859,'Верхневолынское'),(9862,9787,9859,'Гулистан'),(9863,9787,9859,'Димитровское'),(9864,9787,9859,'Крестьянский'),(9865,9787,9859,'Сырдарья'),(9866,9787,9859,'Сырьдарья'),(9867,9787,9859,'Хаваст'),(9868,9787,9859,'Янгиер'),(9870,9787,9869,'Алимкент'),(9871,9787,9869,'Алмазар'),(9872,9787,9869,'Алмалык'),(9873,9787,9869,'Ангрен'),(9874,9787,9869,'Ахангаран'),(9875,9787,9869,'Бакабад'),(9876,9787,9869,'Бекабад'),(9877,9787,9869,'Бука'),(9878,9787,9869,'Газалкент'),(9879,9787,9869,'Искандар'),(9880,9787,9869,'Келес'),(9881,9787,9869,'Паркент'),(9882,9787,9869,'Пскент'),(9883,9787,9869,'Сиджак'),(9884,9787,9869,'Солдатский'),(9885,9787,9869,'Ташкент'),(9886,9787,9869,'Тойтепа'),(9887,9787,9869,'Чиназ'),(9888,9787,9869,'Чирчик'),(9889,9787,9869,'Янгиабад'),(9890,9787,9869,'Янгибазар'),(9891,9787,9869,'Янгиюль'),(9893,9787,9892,'Алтыарык'),(9894,9787,9892,'Вуадиль'),(9895,9787,9892,'Дангара'),(9896,9787,9892,'Коканд'),(9897,9787,9892,'Кува'),(9898,9787,9892,'Кувасай'),(9899,9787,9892,'Маргилан'),(9900,9787,9892,'Учкуприк'),(9901,9787,9892,'Фергана'),(9902,9787,9892,'Язъяван'),(9903,9787,9892,'Яйпан'),(9904,9787,9892,'Янгикурган'),(9906,9787,9905,'Ургенч'),(9907,9787,9905,'Хива'),(9910,9908,9909,'Бар'),(9911,9908,9909,'Бершадь'),(9912,9908,9909,'Браилов'),(9913,9908,9909,'Брацлав'),(9914,9908,9909,'Вапнярка'),(9915,9908,9909,'Вендичаны'),(9916,9908,9909,'Винница'),(9917,9908,9909,'Вороновица'),(9918,9908,9909,'Гайсин'),(9919,9908,9909,'Гнивань'),(9920,9908,9909,'Дашев'),(9921,9908,9909,'Жмеринка'),(9922,9908,9909,'Ильинцы'),(9923,9908,9909,'Казатин'),(9924,9908,9909,'Калиновка'),(9925,9908,9909,'Крыжополь'),(9926,9908,9909,'Липовец'),(9927,9908,9909,'Литин'),(9928,9908,9909,'Могилев-Подольский'),(9929,9908,9909,'Мурованные Куриловцы'),(9930,9908,9909,'Немиров'),(9931,9908,9909,'Оратов'),(9932,9908,9909,'Песчанка'),(9933,9908,9909,'Погребище'),(9934,9908,9909,'Теплик'),(9935,9908,9909,'Томашполь'),(9936,9908,9909,'Тростянец'),(9937,9908,9909,'Тульчин'),(9938,9908,9909,'Тывров'),(9939,9908,9909,'Хмельник'),(9940,9908,9909,'Чечельник'),(9941,9908,9909,'Шаргород'),(9942,9908,9909,'Ямполь'),(9944,9908,9943,'Берестечко'),(9945,9908,9943,'Владимир-Волынский'),(9946,9908,9943,'Голобы'),(9947,9908,9943,'Головно'),(9948,9908,9943,'Горохов'),(9949,9908,9943,'Заболотье'),(9950,9908,9943,'Иваничи'),(9951,9908,9943,'Камень-Каширский'),(9952,9908,9943,'Киверцы'),(9953,9908,9943,'Ковель'),(9954,9908,9943,'Локачи'),(9955,9908,9943,'Луцк'),(9956,9908,9943,'Любешов'),(9957,9908,9943,'Любомль'),(9958,9908,9943,'Маневичи'),(9959,9908,9943,'Нововолынск'),(9960,9908,9943,'Ратно'),(9961,9908,9943,'Рожище'),(9962,9908,9943,'Старая Выжевка'),(9963,9908,9943,'Турийск'),(9965,9908,9964,'Апостолово'),(9966,9908,9964,'Аулы'),(9967,9908,9964,'Брагиновка'),(9968,9908,9964,'Васильковка'),(9969,9908,9964,'Верхнеднепровск'),(9970,9908,9964,'Верховцево'),(9971,9908,9964,'Вольногорск'),(9972,9908,9964,'Гвардейское'),(9973,9908,9964,'Горняцкое'),(9974,9908,9964,'Губиниха'),(9975,9908,9964,'Демурино'),(9976,9908,9964,'Днепродзержинск'),(9977,9908,9964,'Днепропетровск'),(9978,9908,9964,'Желтые Воды'),(9979,9908,9964,'Зализничное'),(9980,9908,9964,'Зеленое'),(9981,9908,9964,'Илларионово'),(9982,9908,9964,'Ингулец'),(9983,9908,9964,'Калинино'),(9984,9908,9964,'Карнауховка'),(9985,9908,9964,'Кривой Рог'),(9986,9908,9964,'Кринички'),(9987,9908,9964,'Магдалиновка'),(9988,9908,9964,'Марганец'),(9989,9908,9964,'Межевая'),(9990,9908,9964,'Никополь'),(9991,9908,9964,'Новомосковск'),(9992,9908,9964,'Орджоникидзе'),(9993,9908,9964,'Павлоград'),(9994,9908,9964,'Петропавловка'),(9995,9908,9964,'Покровское'),(9996,9908,9964,'Пятихатки'),(9997,9908,9964,'Синельниково'),(9998,9908,9964,'Софиевка'),(9999,9908,9964,'Томаковка'),(10000,9908,9964,'Царичанка'),(10001,9908,9964,'Широкое'),(10003,9908,10002,'Авдеевка'),(10004,9908,10002,'Александровка'),(10005,9908,10002,'Алексеево-Дружковка'),(10006,9908,10002,'Амвросиевка'),(10007,9908,10002,'Андреевка'),(10008,9908,10002,'Артемово'),(10009,9908,10002,'Артемовск'),(10010,9908,10002,'Безыменное'),(10011,9908,10002,'Беленькое'),(10012,9908,10002,'Белицкое'),(10013,9908,10002,'Благодатное'),(10014,9908,10002,'Былбасовка'),(10015,9908,10002,'Великая Новоселка'),(10016,9908,10002,'Войковский'),(10017,9908,10002,'Волноваха'),(10018,9908,10002,'Володарское'),(10019,9908,10002,'Гольмовский'),(10020,9908,10002,'Горбачево-Михайловка'),(10021,9908,10002,'Горловка'),(10022,9908,10002,'Гродовка'),(10023,9908,10002,'Грузско-Зорянское'),(10024,9908,10002,'Дебальцево'),(10025,9908,10002,'Дзержинск'),(10026,9908,10002,'Димитров'),(10027,9908,10002,'Доброполье'),(10028,9908,10002,'Докучаевск'),(10029,9908,10002,'Донецк'),(10030,9908,10002,'Донецкая'),(10031,9908,10002,'Донское'),(10032,9908,10002,'Дробышево'),(10033,9908,10002,'Дружковка'),(10034,9908,10002,'Енакиево'),(10035,9908,10002,'Жданов'),(10036,9908,10002,'Желанное'),(10037,9908,10002,'Зугрэс'),(10038,9908,10002,'Зуевка'),(10039,9908,10002,'Иловайск'),(10040,9908,10002,'Карло-Либкнехтовск'),(10041,9908,10002,'Карло-Марксово'),(10042,9908,10002,'Кировск'),(10043,9908,10002,'Краматорск'),(10044,9908,10002,'Красноармейск'),(10045,9908,10002,'Красный Лиман'),(10046,9908,10002,'Макеевка'),(10047,9908,10002,'Мариуполь'),(10048,9908,10002,'Марьинка'),(10049,9908,10002,'Новоазовск'),(10050,9908,10002,'Новоэкономическое'),(10051,9908,10002,'Першотравневое'),(10052,9908,10002,'Селидово'),(10053,9908,10002,'Славянск'),(10054,9908,10002,'Снежное'),(10055,9908,10002,'Старобешево'),(10056,9908,10002,'Тельманово'),(10057,9908,10002,'Торез'),(10058,9908,10002,'Харцызск'),(10059,9908,10002,'Шахтерск'),(10060,9908,10002,'Ясиноватая'),(10062,9908,10061,'Андрушевка'),(10063,9908,10061,'Барановка'),(10064,9908,10061,'Белая Криница'),(10065,9908,10061,'Бердичев'),(10066,9908,10061,'Броницкая Гута'),(10067,9908,10061,'Брусилов'),(10068,9908,10061,'Быковка'),(10069,9908,10061,'Великие Коровинцы'),(10070,9908,10061,'Володарск-Волынский'),(10071,9908,10061,'Городница'),(10072,9908,10061,'Гришковцы'),(10073,9908,10061,'Дзержинск'),(10074,9908,10061,'Довбыш'),(10075,9908,10061,'Емильчино'),(10076,9908,10061,'Житомир'),(10077,9908,10061,'Иванополь'),(10078,9908,10061,'Каменный Брод'),(10079,9908,10061,'Коростень'),(10080,9908,10061,'Коростышев'),(10081,9908,10061,'Лугины'),(10082,9908,10061,'Любар'),(10083,9908,10061,'Малин'),(10084,9908,10061,'Народичи'),(10085,9908,10061,'Новоград-Волынский'),(10086,9908,10061,'Овруч'),(10087,9908,10061,'Олевск'),(10088,9908,10061,'Попельня'),(10089,9908,10061,'Радомышль'),(10090,9908,10061,'Ружин'),(10091,9908,10061,'Червоноармейск'),(10092,9908,10061,'Черняхов'),(10093,9908,10061,'Чуднов'),(10095,9908,10094,'Берегово'),(10096,9908,10094,'Буштына'),(10097,9908,10094,'Великий Березный'),(10098,9908,10094,'Великий Бычков'),(10099,9908,10094,'Виноградов'),(10100,9908,10094,'Воловец'),(10101,9908,10094,'Иршава'),(10102,9908,10094,'Межгорье'),(10103,9908,10094,'Мукачево'),(10104,9908,10094,'Перечин'),(10105,9908,10094,'Рахов'),(10106,9908,10094,'Свалява'),(10107,9908,10094,'Тячев'),(10108,9908,10094,'Ужгород'),(10109,9908,10094,'Хуст'),(10110,9908,10094,'Чоп'),(10112,9908,10111,'Акимовка'),(10113,9908,10111,'Андреевка'),(10114,9908,10111,'Балабино'),(10115,9908,10111,'Бердянск'),(10116,9908,10111,'Васильевка'),(10117,9908,10111,'Веселое'),(10118,9908,10111,'Гуляйполе'),(10119,9908,10111,'Запорожье'),(10120,9908,10111,'Каменка-Днепровская'),(10121,9908,10111,'Каменное'),(10122,9908,10111,'Камыш-Заря'),(10123,9908,10111,'Куйбышево'),(10124,9908,10111,'Мелитополь'),(10125,9908,10111,'Михайловка'),(10126,9908,10111,'Новониколаевка'),(10127,9908,10111,'Орехов'),(10128,9908,10111,'Пологи'),(10129,9908,10111,'Приазовское'),(10130,9908,10111,'Приморск'),(10131,9908,10111,'Токмак'),(10132,9908,10111,'Черниговка'),(10134,9908,10133,'Богородчаны'),(10135,9908,10133,'Болехов'),(10136,9908,10133,'Болшовцы'),(10137,9908,10133,'Брошнев-Осада'),(10138,9908,10133,'Букачевцы'),(10139,9908,10133,'Бурштын'),(10140,9908,10133,'Бытков'),(10141,9908,10133,'Верховина'),(10142,9908,10133,'Войнилов'),(10143,9908,10133,'Ворохта'),(10144,9908,10133,'Выгода'),(10145,9908,10133,'Галич'),(10146,9908,10133,'Гвоздец'),(10147,9908,10133,'Городенка'),(10148,9908,10133,'Делятин'),(10149,9908,10133,'Жовтень'),(10150,9908,10133,'Заболотов'),(10151,9908,10133,'Ивано-Франковск'),(10152,9908,10133,'Калуж'),(10153,9908,10133,'Калуш'),(10154,9908,10133,'Коломыя'),(10155,9908,10133,'Косов'),(10156,9908,10133,'Надворна'),(10157,9908,10133,'Надворная'),(10158,9908,10133,'Рогатин'),(10159,9908,10133,'Рожнятов'),(10160,9908,10133,'Снятын'),(10161,9908,10133,'Станиславов'),(10162,9908,10133,'Тлумач'),(10163,9908,10133,'Тысменица'),(10164,9908,10133,'Яремча'),(10166,9908,10165,'Барышевка'),(10167,9908,10165,'Белая Церковь'),(10168,9908,10165,'Березань'),(10169,9908,10165,'Богуслав'),(10170,9908,10165,'Борисполь'),(10171,9908,10165,'Боровая'),(10172,9908,10165,'Бородянка'),(10173,9908,10165,'Бровары'),(10174,9908,10165,'Васильков'),(10175,9908,10165,'Володарка'),(10176,9908,10165,'Ворзель'),(10177,9908,10165,'Гребенки'),(10178,9908,10165,'Дымер'),(10179,9908,10165,'Згуровка'),(10180,9908,10165,'Иванков'),(10181,9908,10165,'Ирпень'),(10182,9908,10165,'Кагарлык'),(10183,9908,10165,'Калиновка'),(10184,9908,10165,'Киев'),(10185,9908,10165,'Киевская'),(10186,9908,10165,'Кодра'),(10187,9908,10165,'Кожанка'),(10188,9908,10165,'Козин'),(10189,9908,10165,'Макаров'),(10190,9908,10165,'Обухов'),(10191,9908,10165,'Переяслав-Хмельницкий'),(10192,9908,10165,'Полесское'),(10193,9908,10165,'Ракитное'),(10194,9908,10165,'Сквира'),(10195,9908,10165,'Ставище'),(10196,9908,10165,'Тараща'),(10197,9908,10165,'Тетиев'),(10198,9908,10165,'Фастов'),(10199,9908,10165,'Чернобыль'),(10200,9908,10165,'Яготин'),(10202,9908,10201,'Алексадровка'),(10203,9908,10201,'Александрия'),(10204,9908,10201,'Бобринец'),(10205,9908,10201,'Гайворон'),(10206,9908,10201,'Голованевск'),(10207,9908,10201,'Добровеличковка'),(10208,9908,10201,'Долинская'),(10209,9908,10201,'Елизаветградка'),(10210,9908,10201,'Завалье'),(10211,9908,10201,'Знаменка'),(10212,9908,10201,'Знаменка-Вторая'),(10213,9908,10201,'Капитановка'),(10214,9908,10201,'Кировоград'),(10215,9908,10201,'Компанеевка'),(10216,9908,10201,'Малая Виска'),(10217,9908,10201,'Новгородка'),(10218,9908,10201,'Новоархангельск'),(10219,9908,10201,'Новомиргород'),(10220,9908,10201,'Новоукраинка'),(10221,9908,10201,'Ольшанка'),(10222,9908,10201,'Онуфриевка'),(10223,9908,10201,'Петрово'),(10224,9908,10201,'Светловодск'),(10225,9908,10201,'Ульяновка'),(10226,9908,10201,'Устиновка'),(10228,9908,10227,'Азовское'),(10229,9908,10227,'Алупка'),(10230,9908,10227,'Алушта'),(10231,9908,10227,'Багерово'),(10232,9908,10227,'Балаклава'),(10233,9908,10227,'Бахчисарай'),(10234,9908,10227,'Белогорск'),(10235,9908,10227,'Гаспра'),(10236,9908,10227,'Гвардейское'),(10237,9908,10227,'Гурзуф'),(10238,9908,10227,'Джанкой'),(10239,9908,10227,'Евпатория'),(10240,9908,10227,'Зуя'),(10241,9908,10227,'Керчь'),(10242,9908,10227,'Кировское'),(10243,9908,10227,'Красногвардейское'),(10244,9908,10227,'Красноперекопск'),(10245,9908,10227,'Ленино'),(10246,9908,10227,'Массандра'),(10247,9908,10227,'Нижнегорский'),(10248,9908,10227,'Первомайское'),(10249,9908,10227,'Раздольное'),(10250,9908,10227,'Саки'),(10251,9908,10227,'Севастополь'),(10252,9908,10227,'Симферополь'),(10253,9908,10227,'Советский'),(10254,9908,10227,'Судак'),(10255,9908,10227,'Феодосия'),(10256,9908,10227,'Форос'),(10257,9908,10227,'Черноморское'),(10258,9908,10227,'Ялта'),(10260,9908,10259,'Алексадровск'),(10261,9908,10259,'Алчевск'),(10262,9908,10259,'Антрацит'),(10263,9908,10259,'Артемовск'),(10264,9908,10259,'Байрачки'),(10265,9908,10259,'Беловодск'),(10266,9908,10259,'Белогоровка'),(10267,9908,10259,'Белое'),(10268,9908,10259,'Белокуракино'),(10269,9908,10259,'Белолуцк'),(10270,9908,10259,'Бирюково'),(10271,9908,10259,'Боково-Платово'),(10272,9908,10259,'Боровское'),(10273,9908,10259,'Брянка'),(10274,9908,10259,'Бугаевка'),(10275,9908,10259,'Вахрушево'),(10276,9908,10259,'Великий Лог'),(10277,9908,10259,'Вергулевка'),(10278,9908,10259,'Володарск'),(10279,9908,10259,'Волчеяровка'),(10280,9908,10259,'Ворошиловград'),(10281,9908,10259,'Врубовка'),(10282,9908,10259,'Врубовский'),(10283,9908,10259,'Георгиевка'),(10284,9908,10259,'Горское'),(10285,9908,10259,'Есауловка'),(10286,9908,10259,'Зимогорье'),(10287,9908,10259,'Золотое'),(10288,9908,10259,'Зоринск'),(10289,9908,10259,'Изварино'),(10290,9908,10259,'Калининский'),(10291,9908,10259,'Калиново'),(10292,9908,10259,'Камышеваха'),(10293,9908,10259,'Кировск'),(10294,9908,10259,'Коммунарск'),(10295,9908,10259,'Краснодон'),(10296,9908,10259,'Красный Луч'),(10297,9908,10259,'Кременная'),(10298,9908,10259,'Лисичанск'),(10299,9908,10259,'Луганск'),(10300,9908,10259,'Лутугино'),(10301,9908,10259,'Марковка'),(10302,9908,10259,'Меловое'),(10303,9908,10259,'Новоайдар'),(10304,9908,10259,'Новопсков'),(10305,9908,10259,'Первомайск'),(10306,9908,10259,'Перевальск'),(10307,9908,10259,'Попасная'),(10308,9908,10259,'Ровеньки'),(10309,9908,10259,'Рубежное'),(10310,9908,10259,'Сватово'),(10311,9908,10259,'Свердловск'),(10312,9908,10259,'Северодонецк'),(10313,9908,10259,'Славяносербск'),(10314,9908,10259,'Станично-Луганское'),(10315,9908,10259,'Старобельск'),(10316,9908,10259,'Стаханов'),(10317,9908,10259,'Троицкое'),(10319,9908,10318,'Белз'),(10320,9908,10318,'Бобрка'),(10321,9908,10318,'Борислав'),(10322,9908,10318,'Броды'),(10323,9908,10318,'Буск'),(10324,9908,10318,'Великие Мосты'),(10325,9908,10318,'Верхнее Синевидное'),(10326,9908,10318,'Винники'),(10327,9908,10318,'Глиняны'),(10328,9908,10318,'Горняк'),(10329,9908,10318,'Дашава'),(10330,9908,10318,'Добротвор'),(10331,9908,10318,'Дрогобыч'),(10332,9908,10318,'Жидачов'),(10333,9908,10318,'Жолкиев'),(10334,9908,10318,'Золочев'),(10335,9908,10318,'Ивано-Франково'),(10336,9908,10318,'Каменка-Бугская'),(10337,9908,10318,'Львов'),(10338,9908,10318,'Мостиска'),(10339,9908,10318,'Нестеров'),(10340,9908,10318,'Николаев'),(10341,9908,10318,'Перемышляны'),(10342,9908,10318,'Пустомыты'),(10343,9908,10318,'Рава Русская'),(10344,9908,10318,'Радехов'),(10345,9908,10318,'Самбор'),(10346,9908,10318,'Сколе'),(10347,9908,10318,'Сокаль'),(10348,9908,10318,'Старый Самбор'),(10349,9908,10318,'Стрый'),(10350,9908,10318,'Трускавец'),(10351,9908,10318,'Турка'),(10352,9908,10318,'Червоноград'),(10353,9908,10318,'Яворов'),(10355,9908,10354,'Александровка'),(10356,9908,10354,'Арбузинка'),(10357,9908,10354,'Баштанка'),(10358,9908,10354,'Березнеговатое'),(10359,9908,10354,'Братское'),(10360,9908,10354,'Великая Корениха'),(10361,9908,10354,'Веселиново'),(10362,9908,10354,'Вознесенск'),(10363,9908,10354,'Доманевка'),(10364,9908,10354,'Еланец'),(10365,9908,10354,'Казанка'),(10366,9908,10354,'Кривое Озеро'),(10367,9908,10354,'Николаев'),(10368,9908,10354,'Новая Одесса'),(10369,9908,10354,'Новый Буг'),(10370,9908,10354,'Очаков'),(10371,9908,10354,'Первомайск'),(10372,9908,10354,'Снигиревка'),(10374,9908,10373,'Аккерман'),(10375,9908,10373,'Ананьев'),(10376,9908,10373,'Арциз'),(10377,9908,10373,'Балта'),(10378,9908,10373,'Белгород-Днестровский'),(10379,9908,10373,'Беляевка'),(10380,9908,10373,'Березино'),(10381,9908,10373,'Березовка'),(10382,9908,10373,'Болград'),(10383,9908,10373,'Бородино'),(10384,9908,10373,'Великая Михайловка'),(10385,9908,10373,'Великодолининское'),(10386,9908,10373,'Вилково'),(10387,9908,10373,'Затишье'),(10388,9908,10373,'Измаил'),(10389,9908,10373,'Ильичевск'),(10390,9908,10373,'Килия'),(10391,9908,10373,'Кодыма'),(10392,9908,10373,'Коминтерновское'),(10393,9908,10373,'Котовск'),(10394,9908,10373,'Красные Окны'),(10395,9908,10373,'Любашевка'),(10396,9908,10373,'Николаевка'),(10397,9908,10373,'Овидиополь'),(10398,9908,10373,'Одесса'),(10399,9908,10373,'Раздельная'),(10400,9908,10373,'Рени'),(10401,9908,10373,'Саврань'),(10402,9908,10373,'Сарата'),(10403,9908,10373,'Тарутино'),(10404,9908,10373,'Татарбунары'),(10405,9908,10373,'Фрунзовка'),(10406,9908,10373,'Ширяево'),(10408,9908,10407,'Белики'),(10409,9908,10407,'Великая Багачка'),(10410,9908,10407,'Гадяч'),(10411,9908,10407,'Глобино'),(10412,9908,10407,'Гоголево'),(10413,9908,10407,'Градижск'),(10414,9908,10407,'Гребенка'),(10415,9908,10407,'Диканька'),(10416,9908,10407,'Зеньков'),(10417,9908,10407,'Карловка'),(10418,9908,10407,'Кобеляки'),(10419,9908,10407,'Козельщина'),(10420,9908,10407,'Котельва'),(10421,9908,10407,'Кременчуг'),(10422,9908,10407,'Лохвица'),(10423,9908,10407,'Лубны'),(10424,9908,10407,'Машевка'),(10425,9908,10407,'Миргород'),(10426,9908,10407,'Новые Санжары'),(10427,9908,10407,'Оржица'),(10428,9908,10407,'Пирянтин'),(10429,9908,10407,'Пирятин'),(10430,9908,10407,'Полтава'),(10431,9908,10407,'Решетиловка'),(10432,9908,10407,'Семеновка'),(10433,9908,10407,'Хорол'),(10434,9908,10407,'Чернухи'),(10435,9908,10407,'Чутово'),(10436,9908,10407,'Шишаки'),(10438,9908,10437,'Владимирец'),(10439,9908,10437,'Гоща'),(10440,9908,10437,'Демидовка'),(10441,9908,10437,'Дубно'),(10442,9908,10437,'Дубровица'),(10443,9908,10437,'Заречное'),(10444,9908,10437,'Здолбунов'),(10445,9908,10437,'Клевань'),(10446,9908,10437,'Клесов'),(10447,9908,10437,'Корец'),(10448,9908,10437,'Костополь'),(10449,9908,10437,'Кузнецовск'),(10450,9908,10437,'Млинов'),(10451,9908,10437,'Острог'),(10452,9908,10437,'Ровно'),(10453,9908,10437,'Сарны'),(10454,9908,10437,'Червоноармейск'),(10456,9908,10455,'Ахтырка'),(10457,9908,10455,'Белополье'),(10458,9908,10455,'Бурынь'),(10459,9908,10455,'Великая Писаревка'),(10460,9908,10455,'Ворожба'),(10461,9908,10455,'Воронеж'),(10462,9908,10455,'Глухов'),(10463,9908,10455,'Дружба'),(10464,9908,10455,'Знобь-Новгородское'),(10465,9908,10455,'Кириковка'),(10466,9908,10455,'Конотоп'),(10467,9908,10455,'Краснополье'),(10468,9908,10455,'Кровелец'),(10469,9908,10455,'Лебедин'),(10470,9908,10455,'Липовая Долина'),(10471,9908,10455,'Недригайлов'),(10472,9908,10455,'Путивль'),(10473,9908,10455,'Ромны'),(10474,9908,10455,'Середина-Буда'),(10475,9908,10455,'Сумы'),(10476,9908,10455,'Тростянец'),(10477,9908,10455,'Шостка'),(10478,9908,10455,'Шурупинское'),(10479,9908,10455,'Ямполь'),(10481,9908,10480,'Бережаны'),(10482,9908,10480,'Борщев'),(10483,9908,10480,'Бучач'),(10484,9908,10480,'Великие Борки'),(10485,9908,10480,'Вишневец'),(10486,9908,10480,'Гримайлов'),(10487,9908,10480,'Гусятин'),(10488,9908,10480,'Залещики'),(10489,9908,10480,'Заложцы'),(10490,9908,10480,'Збараж'),(10491,9908,10480,'Зборов'),(10492,9908,10480,'Золотники'),(10493,9908,10480,'Золотойпоток'),(10494,9908,10480,'Козлов'),(10495,9908,10480,'Козова'),(10496,9908,10480,'Кременец'),(10497,9908,10480,'Лановцы'),(10498,9908,10480,'Монастыриска'),(10499,9908,10480,'Подволочиск'),(10500,9908,10480,'Теребовля'),(10501,9908,10480,'Тернополь'),(10502,9908,10480,'Чортков'),(10503,9908,10480,'Шумское'),(10505,9908,10504,'Балаклея'),(10506,9908,10504,'Барвенково'),(10507,9908,10504,'Близнюки'),(10508,9908,10504,'Богодухов'),(10509,9908,10504,'Борки'),(10510,9908,10504,'Боровая'),(10511,9908,10504,'Буды'),(10512,9908,10504,'Валки'),(10513,9908,10504,'Великий Бурлук'),(10514,9908,10504,'Волчанск'),(10515,9908,10504,'Готвальд'),(10516,9908,10504,'Гуты'),(10517,9908,10504,'Дергачи'),(10518,9908,10504,'Зачепиловка'),(10519,9908,10504,'Зидьки'),(10520,9908,10504,'Золочев'),(10521,9908,10504,'Изюм'),(10522,9908,10504,'Казачья Лопань'),(10523,9908,10504,'Кегичевка'),(10524,9908,10504,'Красноград'),(10525,9908,10504,'Краснокутск'),(10526,9908,10504,'Купянск'),(10527,9908,10504,'Лозовая'),(10528,9908,10504,'Люботин'),(10529,9908,10504,'Новая Водолага'),(10530,9908,10504,'Первомайский'),(10531,9908,10504,'Сахновщина'),(10532,9908,10504,'Харьков'),(10533,9908,10504,'Чугуев'),(10534,9908,10504,'Шевченково'),(10536,9908,10535,'Аскания-Нова'),(10537,9908,10535,'Белая Криница'),(10538,9908,10535,'Белозерка'),(10539,9908,10535,'Берислав'),(10540,9908,10535,'Великая Александровка'),(10541,9908,10535,'Великая Лепетиха'),(10542,9908,10535,'Верхний Рогачик'),(10543,9908,10535,'Высокополье'),(10544,9908,10535,'Геническ'),(10545,9908,10535,'Голая Пристань'),(10546,9908,10535,'Горностаевка'),(10547,9908,10535,'Днепряны'),(10548,9908,10535,'Каланчак'),(10549,9908,10535,'Калининское'),(10550,9908,10535,'Каховка'),(10551,9908,10535,'Нижние Серогозы'),(10552,9908,10535,'Новая Каховка'),(10553,9908,10535,'Нововоронцовка'),(10554,9908,10535,'Новотроицкое'),(10555,9908,10535,'Скадовск'),(10556,9908,10535,'Херсон'),(10557,9908,10535,'Цюрупинск'),(10558,9908,10535,'Чаплинка'),(10560,9908,10559,'Антонины'),(10561,9908,10559,'Базалия'),(10562,9908,10559,'Белогорье'),(10563,9908,10559,'Виньковцы'),(10564,9908,10559,'Волочиск'),(10565,9908,10559,'Городок'),(10566,9908,10559,'Грицев'),(10567,9908,10559,'Деражня'),(10568,9908,10559,'Дунаевцы'),(10569,9908,10559,'Изяслав'),(10570,9908,10559,'Каменец-Подольский'),(10571,9908,10559,'Красилов'),(10572,9908,10559,'Летичев'),(10573,9908,10559,'Новая Ушица'),(10574,9908,10559,'Полонное'),(10575,9908,10559,'Славута'),(10576,9908,10559,'Старая Синява'),(10577,9908,10559,'Староконстантинов'),(10578,9908,10559,'Теофиполь'),(10579,9908,10559,'Хмельницкий'),(10580,9908,10559,'Чемеровцы'),(10581,9908,10559,'Шепетовка'),(10582,9908,10559,'Ярмолинцы'),(10584,9908,10583,'Ватутино'),(10585,9908,10583,'Городище'),(10586,9908,10583,'Драбов'),(10587,9908,10583,'Ерки'),(10588,9908,10583,'Жашков'),(10589,9908,10583,'Зараевск'),(10590,9908,10583,'Звенигородка'),(10591,9908,10583,'Золотоноша'),(10592,9908,10583,'Ирдынь'),(10593,9908,10583,'Каменка'),(10594,9908,10583,'Катеринополь'),(10595,9908,10583,'Корсунь-Шевченковский'),(10596,9908,10583,'Лысянка'),(10597,9908,10583,'Маньковка'),(10598,9908,10583,'Монастырище'),(10599,9908,10583,'Смела'),(10600,9908,10583,'Тальное'),(10601,9908,10583,'Умань'),(10602,9908,10583,'Христиновка'),(10603,9908,10583,'Черкассы'),(10604,9908,10583,'Чернобай'),(10605,9908,10583,'Чигирин'),(10606,9908,10583,'Шпола'),(10608,9908,10607,'Батурин'),(10609,9908,10607,'Бахмач'),(10610,9908,10607,'Березна'),(10611,9908,10607,'Бобровица'),(10612,9908,10607,'Борзна'),(10613,9908,10607,'Варва'),(10614,9908,10607,'Вертиевка'),(10615,9908,10607,'Городня'),(10616,9908,10607,'Замглай'),(10617,9908,10607,'Ичня'),(10618,9908,10607,'Козелец'),(10619,9908,10607,'Короп'),(10620,9908,10607,'Корюковка'),(10621,9908,10607,'Куликовка'),(10622,9908,10607,'Мена'),(10623,9908,10607,'Нежин'),(10624,9908,10607,'Новгород Северский'),(10625,9908,10607,'Носовка'),(10626,9908,10607,'Прилуки'),(10627,9908,10607,'Репки'),(10628,9908,10607,'Семеновка'),(10629,9908,10607,'Сосница'),(10630,9908,10607,'Талалаевка'),(10631,9908,10607,'Чернигов'),(10632,9908,10607,'Щорс'),(10634,9908,10633,'Берегомет'),(10635,9908,10633,'Вашковцы'),(10636,9908,10633,'Вижница'),(10637,9908,10633,'Герца'),(10638,9908,10633,'Глыбокая'),(10639,9908,10633,'Заставна'),(10640,9908,10633,'Кельменцы'),(10641,9908,10633,'Кицмань'),(10642,9908,10633,'Новоселица'),(10643,9908,10633,'Путила'),(10644,9908,10633,'Сокиряны'),(10645,9908,10633,'Сторожинец'),(10646,9908,10633,'Хотин'),(10647,9908,10633,'Черновцы'),(10650,10648,10649,'Лаппеенранта'),(10652,10648,10651,'Лахти'),(10654,10648,10653,'Вааса'),(10656,10648,10655,'Оулу'),(10658,10648,10657,'Куопио'),(10660,10648,10659,'Пори'),(10662,10648,10661,'Турку'),(10664,10648,10663,'Ийваскила'),(10666,10648,10665,'Хельсинки'),(10667,10648,10665,'Эспоо'),(10670,10668,10669,'Сен-Брие'),(10672,10668,10671,'Montluзon'),(10674,10668,10673,'Гап'),(10676,10668,10675,'Антибе'),(10677,10668,10675,'Канны'),(10678,10668,10675,'Ницца'),(10680,10668,10679,'Шарлевиль'),(10682,10668,10681,'Бельфор'),(10684,10668,10683,'А-ен-Провенс'),(10685,10668,10683,'Марсель'),(10687,10668,10686,'Аргентоль'),(10689,10668,10688,'Виллежюи'),(10690,10668,10688,'Винсенне'),(10691,10668,10688,'Витри'),(10692,10668,10688,'Иври'),(10693,10668,10688,'Кретейл'),(10694,10668,10688,'Маисон-Альфорт'),(10695,10668,10688,'Сен-Мар-дес-Фоссе'),(10696,10668,10688,'Фонтеней-су-Буа'),(10697,10668,10688,'Чойси-ле-Руа'),(10698,10668,10688,'Шампини-сюр-Мар'),(10700,10668,10699,'Ла-Сен-сюр-Мер'),(10701,10668,10699,'Тулон'),(10703,10668,10702,'Эпиналь'),(10705,10668,10704,'Авиньон'),(10707,10668,10706,'Пуатье'),(10709,10668,10708,'Лимож'),(10711,10668,10710,'Ним'),(10713,10668,10712,'Тулуза'),(10715,10668,10714,'Ньор'),(10717,10668,10716,'Валенс'),(10718,10668,10716,'Монтелимар'),(10720,10668,10719,'Безансон'),(10722,10668,10721,'Бордо'),(10724,10668,10723,'Версаль'),(10726,10668,10725,'Ренн'),(10727,10668,10725,'Сант-Мало'),(10729,10668,10728,'Бриве-ла-Жилларде'),(10731,10668,10730,'Бастия'),(10733,10668,10732,'Аяччо'),(10735,10668,10734,'Дижон'),(10737,10668,10736,'Мон-де-Марсан'),(10739,10668,10738,'Ажен'),(10741,10668,10740,'Манд'),(10743,10668,10742,'Блоис'),(10745,10668,10744,'Руанн'),(10746,10668,10744,'Сен-Этьен'),(10748,10668,10747,'Нант'),(10749,10668,10747,'Сен-Назар'),(10751,10668,10750,'Ле-Пюи'),(10753,10668,10752,'Орлеан'),(10755,10668,10754,'Лаваль'),(10757,10668,10756,'Реймс'),(10758,10668,10756,'Шалон-сюр-Мар'),(10760,10668,10759,'Анже'),(10761,10668,10759,'Чолет'),(10763,10668,10762,'Нанси'),(10765,10668,10764,'Мец'),(10767,10668,10766,'Лорьент'),(10769,10668,10768,'Валенсинье'),(10770,10668,10768,'Дуа'),(10771,10668,10768,'Лилль'),(10772,10668,10768,'Рубо'),(10773,10668,10768,'Туркуа'),(10775,10668,10774,'Невер'),(10777,10668,10776,'Антони'),(10778,10668,10776,'Асньер'),(10779,10668,10776,'Булонь-Билланкур'),(10780,10668,10776,'Женневилльер'),(10781,10668,10776,'Исси-ле-Мулино'),(10782,10668,10776,'Кламарт'),(10783,10668,10776,'Кличи'),(10784,10668,10776,'Коломбес'),(10785,10668,10776,'Курбеву'),(10786,10668,10776,'Левальлуи-Перре'),(10787,10668,10776,'Монпелье'),(10788,10668,10776,'Монтруж'),(10789,10668,10776,'Нантерре'),(10790,10668,10776,'Нюилли-сюр-Сен'),(10791,10668,10776,'Руэль-Мальмасон'),(10792,10668,10776,'Сет'),(10793,10668,10776,'Суреснес'),(10795,10668,10794,'Труа'),(10797,10668,10796,'Каркассон'),(10799,10668,10798,'Алансон'),(10801,10668,10800,'Аррас'),(10802,10668,10800,'Бруа-эн-Арто'),(10803,10668,10800,'Ленс'),(10805,10668,10804,'Париж'),(10807,10668,10806,'Байонна'),(10808,10668,10806,'По'),(10810,10668,10809,'Тарб'),(10812,10668,10811,'Перпиньян'),(10814,10668,10813,'Колмар'),(10815,10668,10813,'Мулхаузен'),(10816,10668,10813,'Мулхаус'),(10818,10668,10817,'Страсбург'),(10820,10668,10819,'Вениссо'),(10821,10668,10819,'Виллеурбанн'),(10822,10668,10819,'Лион'),(10824,10668,10823,'Шамбери'),(10826,10668,10825,'Аннеси'),(10828,10668,10827,'Ле-Ман'),(10830,10668,10829,'Бобини'),(10831,10668,10829,'Бонди'),(10832,10668,10829,'Дранси'),(10833,10668,10829,'Ла-Курнье'),(10834,10668,10829,'Ле-Бланк-Меснил'),(10835,10668,10829,'Монтреуил'),(10836,10668,10829,'Обервилье'),(10837,10668,10829,'Ольни-су-Буа'),(10838,10668,10829,'Пантин'),(10839,10668,10829,'Сен-Дени'),(10840,10668,10829,'Эпини-сюр-Сен'),(10842,10668,10841,'Гавр'),(10843,10668,10841,'Руан'),(10845,10668,10844,'Амьен'),(10847,10668,10846,'Макон'),(10848,10668,10846,'Шалон-сур-Сон'),(10850,10668,10849,'Альби'),(10852,10668,10851,'Бювэ'),(10854,10668,10853,'Брест'),(10855,10668,10853,'Кемпер'),(10857,10668,10856,'Ангулем'),(10859,10668,10858,'Ла-Рошель'),(10861,10668,10860,'Бурже'),(10863,10668,10862,'Сен-Квентин'),(10865,10668,10864,'Шатору'),(10867,10668,10866,'Тур'),(10869,10668,10868,'Гренобль'),(10871,10668,10870,'Безье'),(10873,10668,10872,'Эвре'),(10876,10874,10875,'Чешке-Будейовице'),(10878,10874,10877,'Брно'),(10880,10874,10879,'Карловы Вары'),(10882,10874,10881,'Храдец-Кралове'),(10884,10874,10883,'Либерец'),(10885,10874,10883,'Яблонец-над-Нисой'),(10887,10874,10886,'Карвина'),(10888,10874,10886,'Опава'),(10889,10874,10886,'Острава'),(10890,10874,10886,'Фрыдек-Мистек'),(10891,10874,10886,'Хавиров'),(10893,10874,10892,'Оломоук'),(10894,10874,10892,'Преров'),(10895,10874,10892,'Простеёв'),(10897,10874,10896,'Кладно'),(10899,10874,10898,'Дечин'),(10900,10874,10898,'Мост'),(10901,10874,10898,'Теплик'),(10902,10874,10898,'Усти-над-Лабем'),(10903,10874,10898,'Чомутов'),(10906,10904,10905,'Аарау'),(10907,10904,10905,'Баден'),(10909,10904,10908,'Базель'),(10911,10904,10910,'Берн'),(10912,10904,10910,'Кониц'),(10913,10904,10910,'Тун'),(10915,10904,10914,'Лозанна'),(10917,10904,10916,'Женева'),(10919,10904,10918,'Солотурн'),(10921,10904,10920,'Люцерн'),(10923,10904,10922,'Ла-Шо-Де-Фонд'),(10924,10904,10922,'Ньюшатель'),(10926,10904,10925,'Лугано'),(10928,10904,10927,'Фрейбург'),(10929,10904,10927,'Фрибург'),(10931,10904,10930,'Винтертур'),(10932,10904,10930,'Цюрих'),(10935,10933,10934,'Лунд'),(10936,10933,10934,'Мальмё'),(10937,10933,10934,'Хельсингборг'),(10939,10933,10938,'Борас'),(10940,10933,10938,'Гетеборг'),(10941,10933,10938,'Троллхаттан'),(10943,10933,10942,'Карлстад'),(10945,10933,10944,'Уми'),(10947,10933,10946,'Сундсваль'),(10949,10933,10948,'Вастерас'),(10951,10933,10950,'Гавл'),(10953,10933,10952,'Эстерсунд'),(10955,10933,10954,'Йонкопинг'),(10957,10933,10956,'Эскилстуна'),(10959,10933,10958,'Содерталье'),(10960,10933,10958,'Сольна'),(10961,10933,10958,'Стокгольм'),(10963,10933,10962,'Уппсала'),(10965,10933,10964,'Хельмстад'),(10967,10933,10966,'Линкопинг'),(10970,10968,10969,'Абья-Палуоя'),(10971,10968,10969,'Антсла'),(10972,10968,10969,'Валга'),(10973,10968,10969,'Васкнарва'),(10974,10968,10969,'Вильянди'),(10975,10968,10969,'Выру'),(10976,10968,10969,'Йыгева'),(10977,10968,10969,'Калласте'),(10978,10968,10969,'Кейла'),(10979,10968,10969,'Кехра'),(10980,10968,10969,'Кивиыли'),(10981,10968,10969,'Килинги'),(10982,10968,10969,'Килинги-Нымме'),(10983,10968,10969,'Кингисепп'),(10984,10968,10969,'Кохтла-Ярве'),(10985,10968,10969,'Кярдла'),(10986,10968,10969,'Муствээ'),(10987,10968,10969,'Мыйзакюла'),(10988,10968,10969,'Нарва'),(10989,10968,10969,'Пайде'),(10990,10968,10969,'Пылтсамаа'),(10991,10968,10969,'Пярну'),(10992,10968,10969,'Раквере'),(10993,10968,10969,'Рапла'),(10994,10968,10969,'Силламяэ'),(10995,10968,10969,'Сууре-Яани'),(10997,10968,10969,'Тапа'),(10998,10968,10969,'Тарту'),(10999,10968,10969,'Тырва'),(11000,10968,10969,'Хаапсалу'),(11001,10968,10969,'Эльва'),(11004,11002,11003,'Зренянин'),(11005,11002,11003,'Нови-Сад'),(11006,11002,11003,'Панцево'),(11007,11002,11003,'Суботица'),(11009,11002,11008,'Приштина'),(11011,11002,11010,'Белград'),(11012,11002,11010,'Крагуевач'),(11013,11002,11010,'Нис'),(11016,11014,11015,'Чею'),(11018,11014,11017,'Кунсан'),(11019,11014,11017,'Намвон'),(11021,11014,11020,'Йосу'),(11022,11014,11020,'Мокп`о'),(11023,11014,11020,'Чечон'),(11025,11014,11024,'Чонгжу'),(11026,11014,11024,'Чунгжу'),(11028,11014,11027,'Ч`онан'),(11030,11014,11029,'Инчхон'),(11032,11014,11031,'Вонжу'),(11033,11014,11031,'Кангнунг'),(11034,11014,11031,'Чунчон'),(11036,11014,11035,'Кванчжу'),(11038,11014,11037,'Йонгжу'),(11039,11014,11037,'Сувон'),(11041,11014,11040,'Андонг'),(11042,11014,11040,'Кионгджу'),(11043,11014,11040,'П`оханг'),(11044,11014,11040,'Сангю'),(11046,11014,11045,'Масан'),(11047,11014,11045,'Мирианг'),(11048,11014,11045,'Чинхэ'),(11049,11014,11045,'Чиню'),(11051,11014,11050,'Пусан'),(11053,11014,11052,'Сеул'),(11055,11014,11054,'Тэгу'),(11057,11014,11056,'Тэджон'),(11059,11014,11058,'Ульсан'),(11062,11060,11061,'Ичиномия'),(11063,11060,11061,'Касугаи'),(11064,11060,11061,'Нагоиа'),(11065,11060,11061,'Оказаки'),(11066,11060,11061,'Тойота'),(11067,11060,11061,'Тойохаши'),(11069,11060,11068,'Акита'),(11070,11060,11068,'Иокот'),(11071,11060,11068,'Ноширо'),(11072,11060,11068,'Ога'),(11073,11060,11068,'Одат'),(11075,11060,11074,'Аомори'),(11076,11060,11074,'Гошогавара'),(11077,11060,11074,'Тауада'),(11078,11060,11074,'Хачинохе'),(11079,11060,11074,'Хиросаки'),(11081,11060,11080,'Вакэйама'),(11082,11060,11080,'Шингу'),(11084,11060,11083,'Гифу'),(11085,11060,11083,'Огаки'),(11086,11060,11083,'Тайими'),(11087,11060,11083,'Такэйама'),(11089,11060,11088,'Кириу'),(11090,11060,11088,'Мебаши'),(11091,11060,11088,'Нумата'),(11092,11060,11088,'Ота'),(11093,11060,11088,'Такасаки'),(11095,11060,11094,'Ибараки'),(11096,11060,11094,'Ина'),(11097,11060,11094,'Катсута'),(11098,11060,11094,'Китаибараки'),(11099,11060,11094,'Мито'),(11100,11060,11094,'Омииа'),(11101,11060,11094,'Хитачи'),(11103,11060,11102,'Ичиносеки'),(11104,11060,11102,'Мизусава'),(11105,11060,11102,'Мииако'),(11106,11060,11102,'Мориока'),(11107,11060,11102,'Ханамаки'),(11109,11060,11108,'Кага'),(11110,11060,11108,'Каназава'),(11111,11060,11108,'Коматсу'),(11112,11060,11108,'Нанао'),(11114,11060,11113,'Сакаиде'),(11115,11060,11113,'Такаматсу'),(11117,11060,11116,'Изуми'),(11118,11060,11116,'Кагошима'),(11119,11060,11116,'Каноя'),(11120,11060,11116,'Наз'),(11122,11060,11121,'Айкава'),(11123,11060,11121,'Ацуги'),(11124,11060,11121,'Зуши'),(11125,11060,11121,'Йокогама'),(11126,11060,11121,'Йокосука'),(11127,11060,11121,'Камакура'),(11128,11060,11121,'Одавара'),(11129,11060,11121,'Сагамихара'),(11130,11060,11121,'Фуйисава'),(11131,11060,11121,'Хиратсука'),(11132,11060,11121,'Чигасаки'),(11134,11060,11133,'Камеока'),(11135,11060,11133,'Киото'),(11136,11060,11133,'Маизуру'),(11137,11060,11133,'Уйи'),(11138,11060,11133,'Фукучииама'),(11140,11060,11139,'Кочи'),(11141,11060,11139,'Нанкоку'),(11143,11060,11142,'Кумамото'),(11144,11060,11142,'Минамата'),(11145,11060,11142,'Хитоиоши'),(11147,11060,11146,'Исе'),(11148,11060,11146,'Матсусака'),(11149,11060,11146,'Сузука'),(11150,11060,11146,'Тсу'),(11152,11060,11151,'Ишиномаки'),(11153,11060,11151,'Кесеннума'),(11154,11060,11151,'Фурукава'),(11156,11060,11155,'Кобэйаши'),(11157,11060,11155,'Мииазаки'),(11158,11060,11155,'Мииаконойо'),(11159,11060,11155,'Нобеока'),(11160,11060,11155,'Саито'),(11161,11060,11155,'Хиуга'),(11163,11060,11162,'Матсумото'),(11164,11060,11162,'Нагано'),(11165,11060,11162,'Саку'),(11166,11060,11162,'Сува'),(11167,11060,11162,'Уеда'),(11169,11060,11168,'Исахая'),(11170,11060,11168,'Нагасаки'),(11171,11060,11168,'Сасэбо'),(11172,11060,11168,'Шимабара'),(11174,11060,11173,'Кашихара'),(11175,11060,11173,'Нара'),(11176,11060,11173,'Сакураи'),(11177,11060,11173,'Тенри'),(11179,11060,11178,'Кашивазаки'),(11180,11060,11178,'Нагаока'),(11181,11060,11178,'Оджия'),(11182,11060,11178,'Санйо'),(11183,11060,11178,'Цубаме'),(11184,11060,11178,'Шибата'),(11186,11060,11185,'Курашики'),(11187,11060,11185,'Окэйама'),(11188,11060,11185,'Цуяма'),(11190,11060,11189,'Ишигаки'),(11191,11060,11189,'Наха'),(11193,11060,11192,'Даито'),(11194,11060,11192,'Кайзука'),(11195,11060,11192,'Кишивада'),(11196,11060,11192,'Матсубара'),(11197,11060,11192,'Моригучи'),(11198,11060,11192,'Ниагава'),(11199,11060,11192,'Осака'),(11200,11060,11192,'Суита'),(11201,11060,11192,'Такаиши'),(11202,11060,11192,'Такатсуки'),(11203,11060,11192,'Тоионака'),(11204,11060,11192,'Тондабаяши'),(11205,11060,11192,'Хабикино'),(11206,11060,11192,'Хигашиосака'),(11207,11060,11192,'Хираката'),(11208,11060,11192,'Яо'),(11210,11060,11209,'Имари'),(11211,11060,11209,'Каратсу'),(11212,11060,11209,'Сага'),(11213,11060,11209,'Тосу'),(11214,11060,11209,'Ямато'),(11216,11060,11215,'Вараби'),(11217,11060,11215,'Иватсуки'),(11218,11060,11215,'Йоно'),(11219,11060,11215,'Кавагоэ'),(11220,11060,11215,'Кавагучи'),(11221,11060,11215,'Касукаб'),(11222,11060,11215,'Кошигэйа'),(11223,11060,11215,'Кумагэйа'),(11224,11060,11215,'Сока'),(11225,11060,11215,'Ханно'),(11226,11060,11215,'Чичибу'),(11228,11060,11227,'Отсу'),(11229,11060,11227,'Хикон'),(11231,11060,11230,'Атами'),(11232,11060,11230,'Фуджи'),(11234,11060,11233,'Иаизу'),(11235,11060,11233,'Ивата'),(11236,11060,11233,'Изумо'),(11237,11060,11233,'Ито'),(11238,11060,11233,'Масуда'),(11239,11060,11233,'Матсуэ'),(11240,11060,11233,'Мишима'),(11241,11060,11233,'Нумазу'),(11242,11060,11233,'Ода'),(11243,11060,11233,'Ояма'),(11244,11060,11233,'Хамада'),(11245,11060,11233,'Хамаматсу'),(11246,11060,11233,'Шизуока'),(11247,11060,11233,'Шимизу'),(11249,11060,11248,'Ичикава'),(11250,11060,11248,'Кашива'),(11251,11060,11248,'Кисаразу'),(11252,11060,11248,'Матсудо'),(11253,11060,11248,'Мобара'),(11254,11060,11248,'Нарашино'),(11255,11060,11248,'Нода'),(11256,11060,11248,'Савара'),(11257,11060,11248,'Татиама'),(11258,11060,11248,'Фунабаши'),(11259,11060,11248,'Хоши'),(11260,11060,11248,'Чиба'),(11262,11060,11261,'Кодаира'),(11263,11060,11261,'Мачида'),(11264,11060,11261,'Митака'),(11265,11060,11261,'Мусашино'),(11266,11060,11261,'Тачикава'),(11267,11060,11261,'Токио'),(11268,11060,11261,'Хачиойи'),(11269,11060,11261,'Хофу'),(11271,11060,11270,'Анан'),(11272,11060,11270,'Токушима'),(11274,11060,11273,'Ашикага'),(11275,11060,11273,'Кавачи'),(11276,11060,11273,'Канума'),(11277,11060,11273,'Отавара'),(11278,11060,11273,'Утсуномииа'),(11280,11060,11279,'Йонаго'),(11281,11060,11279,'Курэйоши'),(11282,11060,11279,'Тоттори'),(11284,11060,11283,'Камишии'),(11285,11060,11283,'Такаока'),(11286,11060,11283,'Тояма'),(11287,11060,11283,'Уозу'),(11288,11060,11283,'Хими'),(11290,11060,11289,'Оно'),(11291,11060,11289,'Сабе'),(11292,11060,11289,'Такефу'),(11293,11060,11289,'Тсуруга'),(11294,11060,11289,'Фукуи'),(11296,11060,11295,'Амаги'),(11297,11060,11295,'Иукухаши'),(11298,11060,11295,'Кавасаки'),(11299,11060,11295,'Китакиушу'),(11300,11060,11295,'Курум'),(11301,11060,11295,'Ногата'),(11302,11060,11295,'Омута'),(11303,11060,11295,'Фукуока'),(11305,11060,11304,'Иваки'),(11306,11060,11304,'Корииама'),(11308,11060,11307,'Кур'),(11309,11060,11307,'Ономичи'),(11310,11060,11307,'Фукуиама'),(11311,11060,11307,'Хирошима'),(11313,11060,11312,'Абашири'),(11314,11060,11312,'Асахигава'),(11315,11060,11312,'Ашибецу'),(11316,11060,11312,'Бибаи'),(11317,11060,11312,'Вакканаи'),(11318,11060,11312,'Китами'),(11319,11060,11312,'Куширо'),(11320,11060,11312,'Момбетсу'),(11321,11060,11312,'Муроран'),(11322,11060,11312,'Немуро'),(11323,11060,11312,'Обихиро'),(11324,11060,11312,'Отару'),(11325,11060,11312,'Румои'),(11326,11060,11312,'Саппоро'),(11327,11060,11312,'Томакомаи'),(11328,11060,11312,'Эбетсу'),(11329,11060,11312,'Юбари'),(11331,11060,11330,'Акаши'),(11332,11060,11330,'Амагасаки'),(11333,11060,11330,'Ашия'),(11334,11060,11330,'Итами'),(11335,11060,11330,'Каваниши'),(11336,11060,11330,'Какогава'),(11337,11060,11330,'Нишиномия'),(11338,11060,11330,'Тоёока'),(11339,11060,11330,'Химейи'),(11341,11060,11340,'Иаватахама'),(11342,11060,11340,'Имабари'),(11343,11060,11340,'Матсуиама'),(11344,11060,11340,'Озу'),(11345,11060,11340,'Увайима'),(11347,11060,11346,'Иамагата'),(11348,11060,11346,'Ионезава'),(11349,11060,11346,'Саката'),(11350,11060,11346,'Тендо'),(11351,11060,11346,'Тсуруока'),(11352,11060,11346,'Тсучиура'),(11353,11060,11346,'Шинйо'),(11355,11060,11354,'Ивакуни'),(11356,11060,11354,'Онода'),(11357,11060,11354,'Токуиама'),(11358,11060,11354,'Убе'),(11359,11060,11354,'Хаги'),(11360,11060,11354,'Хофу'),(11361,11060,11354,'Шимоносеки'),(11362,11060,11354,'Ямагучи'),(11364,11060,11363,'Кофу'),(278009,81,82,'Биласувар'),(278010,245,277654,'Апаран'),(278011,245,277654,'Ванадзор'),(278012,245,277654,'Севан'),(278013,1894,277655,'Ак-Коль'),(278014,1894,277655,'Актобе'),(278015,1894,277655,'Акший'),(278016,1894,277655,'Аршалы'),(278017,1894,277655,'Атбасар'),(278018,1894,277655,'Атырау'),(278019,1894,277655,'Балкашино'),(278020,1894,277655,'Жезказган'),(278021,1894,277655,'Кокшетау'),(278022,1894,277655,'Костанай'),(278023,1894,277655,'Курчатов'),(278024,1894,277655,'Кызылорда'),(278025,1894,277655,'Махамбет'),(278026,1894,277655,'Уральск'),(278027,1894,277655,'Шымкент'),(278028,2303,2316,'Ак-Суу'),(278029,2303,2316,'Ананьево'),(278030,2303,2316,'Араван'),(278031,2303,2316,'Балыкчи'),(278032,2303,2316,'Беловодское'),(278033,2303,2316,'Бишкек'),(278034,2303,2316,'Боконбаевское'),(278035,2303,2316,'Жалал Абад'),(278036,2303,2316,'Ивановка'),(278037,2303,2316,'Каинда'),(278038,2303,2316,'Кант'),(278039,2303,2316,'Кара-Балта'),(278040,2303,2316,'Караван'),(278041,2303,2316,'Каракол'),(278042,2303,2316,'Кемин'),(278043,2303,2316,'Кербен'),(278044,2303,2316,'Кызыл Суу'),(278045,2303,2316,'Лебединовка'),(278046,2303,2316,'Майлуу-Суу'),(278047,2303,2316,'Нарын'),(278048,2303,2316,'Ош'),(278049,2303,2316,'Пульгон'),(278050,2303,2316,'Сокулук'),(278051,2303,2316,'Талас'),(278052,2303,2316,'Таш-Кумыр'),(278053,2303,2316,'Токмак'),(278054,2303,2316,'Тюп'),(278055,2303,2316,'Чолпон-Ата'),(278056,2303,2316,'Чуй'),(278057,2303,2316,'Эски-Ноокат'),(278058,2448,2449,'Гулбене'),(278059,2514,2515,'Нида'),(278060,9575,9627,'Вахш'),(278061,9575,9627,'Джиргаталь'),(278062,9575,9627,'Дусти'),(278063,9575,9627,'Кофарнихон'),(278064,9575,9627,'Куйбышевск'),(278065,9575,9627,'Ленинский'),(278066,9575,9627,'Московский'),(278067,9575,9627,'Орджоникидзеабад'),(278068,9908,277656,'Армянск'),(278069,9908,277656,'Артек'),(278070,9908,277656,'Береговое'),(278071,9908,277656,'Ботаническое'),(278072,9908,277656,'Кастрополь'),(278073,9908,277656,'Кацивели'),(278074,9908,277656,'Кореиз'),(278075,9908,277656,'Красногвардейск'),(278076,9908,277656,'Краснокаменка'),(278077,9908,277656,'Курпаты'),(278078,9908,277656,'Ливадия'),(278079,9908,277656,'Мисхор'),(278080,9908,277656,'Олива'),(278081,9908,277656,'Оползневое'),(278082,9908,277656,'Парковое'),(278083,9908,277656,'Понизовка'),(278084,9908,277656,'Санаторное'),(278085,9908,277656,'Симеиз'),(278087,10968,10969,'Таллинн'),(278088,616,277657,'Авимор'),(278089,11002,277658,'Ада'),(278090,63,277659,'Айзенштадт'),(278091,428,277660,'Албена'),(278092,1707,277661,'Альмерия'),(278093,277551,277552,'Амстердам'),(278094,63,277659,'Амштеттен'),(278095,277555,277556,'Арад'),(278096,404,277662,'Арлон'),(278097,1012,277663,'Ауе'),(278098,1012,277663,'Бад Киссинген'),(278099,1012,277663,'Бад Херсфельд'),(278100,2897,277664,'Бельско-Бяла'),(278101,277555,277556,'Бистрица'),(278102,616,277657,'Борнмут'),(278103,428,277660,'Боровец'),(278104,277555,277556,'Ботошани'),(278105,63,277659,'Брегенц'),(278106,1012,277663,'Бремерхафен'),(278107,404,277662,'Брюгге'),(278108,10933,277665,'Бурлэнге'),(278109,277555,277556,'Бухарест'),(278110,1012,277663,'Вайден'),(278111,1012,277663,'Вайсенбург'),(278112,10904,277666,'Вайсенбург'),(278113,1707,277661,'Вальядолид'),(278114,1012,277663,'Варен'),(278115,10648,277667,'Васа'),(278116,1786,277668,'Венеция'),(278117,11002,277658,'Вране'),(278118,428,277660,'Враца'),(278119,11002,277658,'Вршац'),(278120,277551,277552,'Гаага'),(278121,277555,277556,'Галац'),(278122,1012,277663,'Гарделеген'),(278123,1707,277661,'Гвадалахара'),(278124,404,277662,'Гент'),(278125,1012,277663,'Геттинген'),(278126,63,277659,'Гмунден'),(278127,277553,277554,'Госпич'),(278128,277551,277552,'Гус'),(278129,277555,277556,'Дева'),(278130,277555,277556,'Джурджу'),(278131,11002,277658,'Димитровград'),(278132,277551,277552,'Дордрехт'),(278133,63,277659,'Дорнбирн'),(278134,1012,277663,'Дортмунд'),(278135,277553,277554,'Дубровник'),(278136,10668,277669,'Дюнкерк'),(278137,428,277660,'Елхово'),(278138,277553,277554,'Загреб'),(278139,2897,277664,'Замосць'),(278140,1012,277663,'Засниц'),(278141,2897,277664,'Зелена Гура'),(278142,428,277660,'Золотые Пески'),(278143,1707,277661,'Ибиза'),(278144,10648,277667,'Ивало'),(278145,10648,277667,'Иломантси'),(278146,63,277659,'Инсбрук'),(278147,1786,277668,'Иска'),(278148,1786,277668,'Кампобассо'),(278149,428,277660,'Камчия'),(278150,277555,277556,'Карансебеш'),(278151,10874,277670,'Карлови-Вари'),(278152,1707,277661,'Картахена'),(278153,1707,277661,'Касерес'),(278154,1012,277663,'Кельн'),(278155,11002,277658,'Кикинда'),(278156,63,277659,'Кицбюэль'),(278157,277555,277556,'Клуж-Напока'),(278158,11002,277658,'Колашин'),(278159,277555,277556,'Констанца'),(278160,1707,277661,'Кордоба'),(278161,1707,277661,'Коста Брава'),(278162,1707,277661,'Коста Дорада'),(278163,1707,277661,'Коста дель Соль'),(278164,10648,277667,'Котка'),(278165,5673,277671,'Кочевье'),(278166,277555,277556,'Крайова'),(278167,428,277660,'Кранево'),(278168,1786,277668,'Кротоне'),(278169,11002,277658,'Крушевац'),(278170,63,277659,'Куфштайн'),(278171,428,277660,'Кюстендил'),(278172,1707,277661,'Ла-Корунья'),(278173,2897,277664,'Лемборк'),(278174,1012,277663,'Линген'),(278175,428,277660,'Ловеч'),(278176,10904,277666,'Локарно'),(278177,10933,277665,'Лулеа'),(278178,277551,277552,'Маастрихт'),(278179,63,277659,'Майрхофен'),(278180,10933,277665,'Малунг'),(278181,277555,277556,'Мангалия'),(278182,616,277657,'Махриханиш'),(278183,10668,277669,'Мерибель'),(278184,10648,277667,'Миккели'),(278185,428,277660,'Михайловград'),(278186,404,277662,'Монс'),(278187,10648,277667,'Муонио'),(278188,1707,277661,'Мурсия'),(278189,1012,277663,'Мюльдорф'),(278190,1012,277663,'Мюнхен'),(278191,1707,277661,'Наварра'),(278192,404,277662,'Намюр'),(278193,1786,277668,'Неаполь'),(278194,11002,277658,'Ниш'),(278195,1012,277663,'Ольденбург'),(278196,277555,277556,'Орадя'),(278197,2897,277664,'Остроленка'),(278198,10874,277670,'Пардубице'),(278199,10648,277667,'Пелло'),(278200,1786,277668,'Перуджа'),(278201,277555,277556,'Петрошани'),(278202,11002,277658,'Печ'),(278203,1012,277663,'Плауен'),(278204,11002,277658,'Плевля'),(278205,277555,277556,'Плоешти'),(278206,10874,277670,'Пльзень'),(278207,5673,277671,'Постойна'),(278208,1786,277668,'Потенца'),(278209,10874,277670,'Прага'),(278210,11002,277658,'Призрен'),(278211,2897,277664,'Пшемысль'),(278212,1786,277668,'Пьяченца'),(278213,428,277660,'Разград'),(278214,2897,277664,'Рацибуж'),(278215,277553,277554,'Риека'),(278216,1786,277668,'Римини'),(278217,10648,277667,'Рованиеми'),(278218,277555,277556,'Роман'),(278219,277551,277552,'Роттердам'),(278220,428,277660,'Русе'),(278221,428,277660,'Сандански'),(278222,277555,277556,'Сату-Маре'),(278223,10933,277665,'Свег'),(278224,428,277660,'Свиленград'),(278225,2897,277664,'Свиноуйсвце'),(278226,428,277660,'Свиштов'),(278227,1707,277661,'Сеговия'),(278228,277555,277556,'Сибиу'),(278229,1786,277668,'Сиракуза'),(278230,277553,277554,'Славонский Брод'),(278231,11002,277658,'Сомбор'),(278232,1707,277661,'Сория'),(278233,277553,277554,'Сплит'),(278234,10648,277667,'Суомисалми'),(278235,11002,277658,'Сьеница'),(278236,10648,277667,'Тампере'),(278237,1786,277668,'Тарвизио'),(278238,1707,277661,'Теруэль'),(278239,1012,277663,'Тетеров'),(278240,11002,277658,'Тиват'),(278241,277555,277556,'Тимишоара'),(278242,1707,277661,'Толедо'),(278243,1707,277661,'Тортоса'),(278244,2897,277664,'Торунь'),(278245,63,277659,'Трибен'),(278246,428,277660,'Тырговиште'),(278247,277555,277556,'Тырговиште'),(278248,277555,277556,'Тыргу-Жиу'),(278250,11002,277658,'Улцинь'),(278252,10933,277665,'Упсала'),(278253,2897,277664,'Устка'),(278254,277551,277552,'Утрехт'),(278255,1786,277668,'Фаенца'),(278256,63,277659,'Фельдкирх'),(278257,277555,277556,'Фетешти'),(278258,63,277659,'Филлах'),(278259,277555,277556,'Флорешти'),(278260,1012,277663,'Фрайбург'),(278261,63,277659,'Фрайштадт'),(278262,277555,277556,'Фэгэраш'),(278263,428,277660,'Хасково'),(278264,404,277662,'Хасселт'),(278265,1707,277661,'Хаэн'),(278266,277553,277554,'Хвар'),(278267,1012,277663,'Хузум'),(278268,2897,277664,'Ченстохова'),(278269,428,277660,'Шабла'),(278270,10668,277669,'Шамони'),(278271,277553,277554,'Шибеник'),(278272,277551,277552,'Эйндховен'),(278273,2897,277664,'Эльблонг'),(278274,277555,277556,'Яссы'),(279078,3159,2499002,'Аган'),(279122,277563,277564,'Куала-Лумпур'),(279123,5681,277672,'Лос-Анджелес'),(279124,277565,277566,'Сингапур'),(279125,277567,277568,'Тайбэй'),(279126,277561,277562,'Амман'),(279127,277559,277560,'Джакарта'),(279128,1451,277673,'Пуна'),(279129,1451,277673,'Бангалор'),(279130,1451,277673,'Хайдарабад'),(279131,1451,277673,'Мадрас'),(279132,1451,277673,'Дели'),(279133,1451,277673,'Калькутта'),(279134,1451,277673,'Бомбей'),(279135,1380,277674,'Александрия'),(279136,277557,277558,'Гонконг'),(279137,5681,277672,'Сиэтл'),(279138,277569,277570,'Ашгабад'),(279139,277569,277570,'Кушка'),(279140,277569,277570,'Тагтабазар'),(1920542,10968,10969,'Куресааре'),(1923078,2374,2379,'Маньчжурия'),(1923662,3159,5356,'Когалым'),(1924057,616,617,'Милтон Кинз'),(1924171,3159,3251,'Мирный'),(1924532,3159,4925,'Шлиссельбург'),(1924719,2880,1924623,'Бодэ'),(1924732,2880,1924623,'Боде'),(1925143,3159,3761,'Кёнигсберг'),(1929637,3159,3296,'Бураево'),(1998542,3159,1998532,'Майкоп'),(1998584,3159,1998532,'Адыгейск'),(2005016,3159,5507,'Снежинск'),(2019672,3159,5326,'Белев'),(2052982,3159,3529,'нововоронеж'),(2116660,3159,3703,'Саянск'),(2183651,428,277660,'Добрич'),(2184457,3159,5161,'Десногорск'),(2185271,3159,3563,'Саров'),(2185858,3159,4052,'Курганинск'),(2191338,9908,10559,'Нетешин'),(2312857,2514,2515,'Висагинас'),(2316517,3159,2316497,'Саяногорск'),(2331530,1366,2331485,'Копенгаген'),(2331847,1012,1152,'Дортмунд'),(2372615,582051,2316780,'Дубаи'),(2373250,3141,2284951,'Фаро'),(2411629,3159,5356,'Излучинск'),(2412951,582031,2412885,'Сантьяго'),(2413243,3159,4481,'Заозерск'),(2414986,9787,9832,'Зарафшан'),(2415620,3159,2415585,'Анадырь'),(2416390,1786,2416357,'о. Сан-Джорджо Маджоре'),(2479040,3159,4105,'Шарыпово'),(2494153,582060,2494137,'Бейрут'),(2496371,582067,1923301,'Дамаск'),(2498687,582077,2483267,'Гавана'),(2499035,3159,2499002,'Нижневартовск'),(2499082,3159,5080,'Новоуральск'),(2505931,2505884,2505921,'Бейрут'),(2564724,1894,2564700,'Степногорск'),(2567466,2567393,2567441,'Тегусигальпа'),(2577926,2880,2577870,'Олесюн'),(2578039,2577958,2578001,'Санто-доминго'),(2687765,2687701,2687732,'Улан-батор'),(2713541,10648,2713529,'Kouvola'),(2724870,9908,10111,'Энергодар'),(2812698,3159,5356,'Муравленко'),(3024146,1012,3024114,'Саарлэнд'),(3118538,3159,4312,'Королев'),(3138835,3159,3703,'Чунский'),(3138841,3159,4312,'Протвино'),(3141531,616,617,'Оксфорд'),(3145145,3159,3407,'Таксимо'),(3408643,3159,5507,'Трехгорный'),(3409478,3159,4105,'Зеленогорск'),(3410035,1380,3409961,'Шарм эль шейх'),(3410583,3410238,3410574,'Багдад'),(3410623,3410238,3410602,'Басра'),(3410656,3410238,3410645,'Мосул'),(3411630,3159,3872,'Елизово'),(3411741,3159,3703,'Култук'),(3473821,3159,5356,'Ларьяк'),(3474011,3159,5225,'Первомайский'),(3503075,2374,3331120,'Пекин'),(3535020,3159,5356,'Губкинский'),(3661609,3661568,3661590,'Йоханесбург'),(3662512,1012,1083,'Гральхейм'),(3663694,582041,3663676,'Скопье'),(3826797,582105,1930798,'Дуглас'),(3826891,582105,1930798,'Ончан'),(3827069,582105,1930798,'Рамси'),(3827103,582105,1930798,'Пил'),(3827174,582105,1930798,'Каслтаун'),(3827336,582105,1930798,'Порт-Эрин'),(3827374,582105,1930798,'Порт-Сент-Мэри'),(3839871,1012,1064,'Нойётинг'),(3845619,9908,10354,'Южноукраинск'),(3863861,2374,2426,'Циндао'),(3864538,2788,2789,'Днестровск'),(3881459,3159,3371,'Дятьково'),(4041912,3159,2499002,'Лянтор'),(4042496,582065,4042477,'Морокко'),(4049698,1012,1152,'Beckum'),(4049790,10648,277667,'Иматра'),(4055849,5681,6367,'Сент-Луис'),(4093520,3159,5246,'Альметьевск'),(4093601,1707,4093580,'Girona'),(4093692,3159,2316497,'Абакан'),(4119784,582065,4119771,'Танжер'),(4121579,245,277654,'Гюмри'),(4156345,3159,2499002,'Югорск'),(4156678,9908,9964,'Першотравенск'),(4157180,9575,9605,'Худжанд'),(4159626,1393,1403,'Ариэль'),(4211196,1894,277655,'Байконур'),(4267241,245,277654,'Раздан'),(4276360,3159,5356,'Белоярский'),(4285318,1012,1064,'Дингольфинг'),(4285366,3159,3784,'Удомля'),(4285519,3159,5507,'Озерск'),(4285678,1012,1120,'Брауншвейг'),(4285698,1012,1120,'Salzgitter'),(4691664,9908,10165,'Славутич'),(4691841,2374,4691804,'Гуанчжоу'),(4692641,1393,1416,'Кирьят-Ата'),(4694853,1393,1416,'Кирьят-Ата'),(4696311,10968,10969,'Палдиски'),(4697440,10968,10969,'Палдиски'),(4777963,3159,5052,'Южно-Курильск'),(4778019,10933,4778004,'Хапаранда'),(4778059,1894,2564700,'Астана'),(4778176,924,945,'Мошонмадьяровар'),(4778382,1393,1416,'Мигдаль аЭмек'),(4778470,3159,4481,'Снежногорск'),(4778496,3159,5080,'Лесной'),(4778522,3159,2499002,'Мегион'),(4778544,1393,1416,'Кацрин'),(4869089,1380,277674,'Хургада'),(4869166,3159,5191,'Железноводск'),(5019404,3159,5019394,'Салехард'),(5020365,3159,5019394,'Новый Уренгой'),(5020595,3159,4481,'Полярные Зори'),(5020614,3159,4105,'Сосновоборск'),(5020665,3159,4891,'Сургут (Самарская обл.)'),(5020681,3159,2499002,'Сургут'),(5036894,1393,1449,'Модиин'),(5040660,2297,2298,'Ларнака'),(5252498,3159,5019394,'Муравленко'),(5498759,3159,5019394,'Губкинский'),(5555962,3159,3407,'Северобайкальск'),(5556050,3159,3952,'Вятские Поляны'),(5556142,2172,2242,'Торнхилл'),(5556154,2172,2242,'Уиллоудэйл'),(5556193,2172,2242,'Уиллоудэйл'),(5556230,582046,5556213,'Лима'),(5556340,1012,1064,'Бургхаузен'),(5640417,10648,5640357,'Joensuu'),(5640607,1012,1201,'Ноймюнстер'),(5640888,582090,5640862,'Nabeul'),(5641077,3159,5019394,'Пуровск'),(5700918,10968,10969,'Азери'),(5908265,3159,3675,'Кохма'),(5908510,9908,10227,'Щёлкино'),(5908547,3159,5019394,'Надым'),(5908584,277555,277556,'Брашов'),(5908636,2880,2577870,'Молде'),(5908671,1393,1403,'Од-а Шарон'),(5911547,1786,1802,'Падуя'),(5911563,1786,1802,'Венеция'),(6113699,3159,2499002,'Нефтеюганск'),(6128604,582050,6128546,'Бангкок'),(6128656,1258,6128641,'Керкира'),(6128703,582082,6128697,'Мапуто'),(6453661,582040,6453632,'Dagu'),(6454651,3159,5356,'Мегион'),(6456567,3159,4105,'Кодинск'),(6460485,582043,6460443,'Моста'),(6711408,3159,2415585,'Билибино'),(6712230,1366,2077237,'Хельсингор'),(7592215,3159,4800,'Егорлыкская'),(7592385,1012,1053,'Майнинген'),(7592923,3159,4312,'Байконур'),(7593075,3159,4312,'Краснознаменск'),(7593157,2297,2300,'Гирне'),(7593225,3159,2499002,'Нягань'),(7593385,582044,7593366,'Исламабад'),(7593394,582044,7593366,'Карачи'),(7593405,582044,7593366,'Лахоре'),(7715963,582082,6128697,'Бейра'),(7715991,582082,6128697,'Нампула'),(7716005,582082,6128697,'Пемба'),(7716144,7716093,7716133,'Балайм'),(7717502,3159,2499002,'Излучинск'),(7717702,3159,5019394,'Ноябрьск'),(7717820,2374,4691804,'Женьжень'),(7717879,1012,1152,'Нидеркассель'),(7718155,1012,1083,'Роттвайл'),(7718164,1012,1083,'Туттлинген'),(7718175,1012,1083,'Филлинген-Швеннинген'),(7718218,1012,1152,'Хамм'),(7718652,3159,2499002,'Ханты-Мансийск'),(7718848,1012,1064,'Розенхейм'),(7719220,9908,10504,'Мерефа'),(7719232,9908,10504,'Песочин'),(7719603,11060,11094,'Цущиура'),(7719867,2172,2242,'Ньюмаркет'),(7719915,582064,7719907,'Кито'),(7720010,9908,10165,'Вышгород'),(7720744,10904,277666,'Санкт-Галлен'),(10498697,3159,5191,'Солнечнодольск'),(10499269,3159,5019394,'Тарко'),(10499434,10933,4778004,'Кируна'),(10499564,1393,1429,'Наария'),(10499672,3159,3872,'Озерновский'),(10499792,9908,10165,'Боярка'),(10500023,3159,4925,'Коммунар'),(10500094,3159,5191,'Изобильный'),(10500382,3159,3468,'Кириллов'),(10500476,3159,4891,'Красный Яр'),(10500739,1786,1810,'Амалфи'),(10501002,3159,5291,'Северск'),(10501179,9908,10002,'Константиновка'),(10501439,1012,1043,'Волмирстэдт'),(10501749,9908,10227,'Армянск'),(10502412,1786,1810,'Сорренто'),(10502714,3159,4105,' Железногорск'),(10502867,63,75,'Мистельбах'),(10504604,3159,4925,'Пикалёво'),(10505044,11060,11178,'Ниигата'),(10505313,10933,10956,'Еребру'),(10505384,428,446,'Карлово'),(10505475,3159,5191,'Зеленокумск'),(10685075,2880,2881,'Несбиен'),(10688837,3159,2499002,'Покачи'),(10689767,3159,4734,'Фокино'),(11563592,9908,10407,'Комсомольск'),(11564839,582029,11564803,'Нассау'),(11566040,582040,11566007,'Кимпо'),(11567319,3159,2499002,'Радужный'),(11568523,1894,1934356,'Атырау'),(11568719,9787,9859,'Ширин'),(11568964,5681,7641,'Фредерик'),(11569100,3159,2499002,'Урай'),(11569394,3159,2499002,'Пыть-Ях'),(11910622,3159,2499002,'Игрим'),(11910929,5681,6291,'Августа'),(11911051,3159,5356,'Унъюган'),(11911759,1393,1403,'Кфар Саба'),(12363228,248,330,'Дятлово'),(12364262,2837,2844,'Крайстчерч'),(12364794,3159,2499002,'Лангепас'),(12367130,1894,1934356,'Уральска'),(12370062,9908,10373,'Южный'),(12370243,3159,2499002,'Приобье'),(12370550,5681,6591,'Тахо'),(12370850,5681,8516,'Строудсбург'),(12370942,3159,3872,'Вилючинск'),(12371093,3159,5019394,'Заполярный'),(15789520,582106,15789405,'Кингстон');
/*!40000 ALTER TABLE `city` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `country`
--

DROP TABLE IF EXISTS `country`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `country` (
  `country_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `city_id` int(11) NOT NULL DEFAULT '0',
  `name` varchar(128) NOT NULL DEFAULT '',
  PRIMARY KEY (`country_id`),
  KEY `city_id` (`city_id`)
) ENGINE=InnoDB AUTO_INCREMENT=7716094 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `country`
--

LOCK TABLES `country` WRITE;
/*!40000 ALTER TABLE `country` DISABLE KEYS */;
INSERT INTO `country` VALUES (4,0,'Австралия'),(63,0,'Австрия'),(81,0,'Азербайджан'),(173,0,'Ангуилья'),(177,0,'Аргентина'),(245,0,'Армения'),(248,0,'Беларусь'),(401,0,'Белиз'),(404,0,'Бельгия'),(425,0,'Бермуды'),(428,0,'Болгария'),(467,0,'Бразилия'),(616,0,'Великобритания'),(924,0,'Венгрия'),(971,0,'Вьетнам'),(994,0,'Гаити'),(1007,0,'Гваделупа'),(1012,0,'Германия'),(1206,0,'Голландия'),(1258,0,'Греция'),(1280,0,'Грузия'),(1366,0,'Дания'),(1380,0,'Египет'),(1393,0,'Израиль'),(1451,0,'Индия'),(1663,0,'Иран'),(1696,0,'Ирландия'),(1707,0,'Испания'),(1786,0,'Италия'),(1894,0,'Казахстан'),(2163,0,'Камерун'),(2172,0,'Канада'),(2297,0,'Кипр'),(2303,0,'Киргызстан'),(2374,0,'Китай'),(2430,0,'Коста-Рика'),(2443,0,'Кувейт'),(2448,0,'Латвия'),(2509,0,'Ливия'),(2514,0,'Литва'),(2614,0,'Люксембург'),(2617,0,'Мексика'),(2788,0,'Молдова'),(2833,0,'Монако'),(2837,0,'Новая Зеландия'),(2880,0,'Норвегия'),(2897,0,'Польша'),(3141,0,'Португалия'),(3156,0,'Реюньон'),(3159,0,'Россия'),(5647,0,'Сальвадор'),(5666,0,'Словакия'),(5673,0,'Словения'),(5678,0,'Суринам'),(5681,0,'США'),(9575,0,'Таджикистан'),(9638,0,'Туркменистан'),(9701,0,'Туркс и Кейкос'),(9705,0,'Турция'),(9782,0,'Уганда'),(9787,0,'Узбекистан'),(9908,0,'Украина'),(10648,0,'Финляндия'),(10668,0,'Франция'),(10874,0,'Чехия'),(10904,0,'Швейцария'),(10933,0,'Швеция'),(10968,0,'Эстония'),(11002,0,'Югославия'),(11014,0,'Южная Корея'),(11060,0,'Япония'),(277551,0,'Нидерланды'),(277553,0,'Хорватия'),(277555,0,'Румыния'),(277557,0,'Гонконг'),(277559,0,'Индонезия'),(277561,0,'Иордания'),(277563,0,'Малайзия'),(277565,0,'Сингапур'),(277567,0,'Тайвань'),(277569,0,'Туркмения'),(582029,0,'Карибы'),(582031,0,'Чили'),(582040,0,'Корея'),(582041,0,'Македония'),(582043,0,'Мальта'),(582044,0,'Пакистан'),(582046,0,'Перу'),(582050,0,'Тайланд'),(582051,0,'О.А.Э.'),(582060,0,'Ливан'),(582064,0,'Эквадор'),(582065,0,'Морокко'),(582067,0,'Сирия'),(582077,0,'Куба'),(582082,0,'Мозамбик'),(582090,0,'Тунис'),(582105,0,'Остров Мэн'),(582106,0,'Ямайка'),(2505884,0,'Ливан'),(2567393,0,'Гондурас'),(2577958,0,'Доминиканская республика'),(2687701,0,'Монголия'),(3410238,0,'Ирак'),(3661568,0,'ЮАР'),(7716093,0,'Арулько');
/*!40000 ALTER TABLE `country` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `eav_attribute`
--

DROP TABLE IF EXISTS `eav_attribute`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `eav_attribute` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `type` tinyint(1) unsigned NOT NULL COMMENT '0 if the attribute can have only one value or 1 if the attribute can have multiple values',
  `data_type` varchar(255) NOT NULL COMMENT 'The attribute data type',
  `name` varchar(255) NOT NULL COMMENT 'The attribute name',
  `label` varchar(255) DEFAULT NULL COMMENT 'The attribute label',
  `data` text COMMENT 'The serialized data',
  `unit` varchar(20) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `un_eav_attribute_name` (`name`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=20 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `eav_attribute`
--

LOCK TABLES `eav_attribute` WRITE;
/*!40000 ALTER TABLE `eav_attribute` DISABLE KEYS */;
INSERT INTO `eav_attribute` VALUES (1,0,'IntDataType','price','Цена','a:1:{s:5:\"rules\";a:1:{s:9:\"numerical\";a:3:{s:3:\"min\";i:1;s:3:\"max\";i:100000000;s:11:\"integerOnly\";b:1;}}}','руб'),(2,0,'IntDataType','area','Площадь','a:1:{s:5:\"rules\";a:1:{s:9:\"numerical\";a:3:{s:3:\"max\";i:1000;s:3:\"min\";i:1;s:11:\"integerOnly\";b:1;}}}','кв.м.'),(3,0,'VarcharDataType','state','Состояние','a:2:{s:5:\"rules\";a:1:{s:6:\"length\";a:2:{s:3:\"max\";i:100;s:3:\"min\";i:2;}}s:6:\"values\";a:2:{s:10:\"Новый\";s:10:\"Новый\";s:5:\"Б/у\";s:5:\"Б/у\";}}',NULL),(4,0,'IntDataType','engineCapacity','Объем двигателя','a:1:{s:5:\"rules\";a:1:{s:9:\"numerical\";a:3:{s:3:\"max\";i:20000;s:3:\"min\";i:1;s:11:\"integerOnly\";b:1;}}}','см<sup>3</sup>'),(5,0,'IntDataType','mileage','Пробег','a:1:{s:5:\"rules\";a:1:{s:9:\"numerical\";a:3:{s:3:\"max\";i:1000000;s:3:\"min\";i:1;s:11:\"integerOnly\";b:1;}}}','км'),(6,0,'IntDataType','modelYear','Год выпуска','a:1:{s:5:\"rules\";a:1:{s:9:\"numerical\";a:3:{s:3:\"min\";i:1940;s:11:\"integerOnly\";b:1;s:14:\"integerPattern\";s:17:\"/^(19)|(20)\\d\\d$/\";}}}','год'),(7,0,'VarcharDataType','carBody','Тип кузова','a:2:{s:5:\"rules\";a:1:{s:6:\"length\";a:2:{s:3:\"max\";i:100;s:3:\"min\";i:2;}}s:6:\"values\";a:8:{s:10:\"Седан\";s:10:\"Седан\";s:8:\"Купе\";s:8:\"Купе\";s:18:\"Кабриолет\";s:18:\"Кабриолет\";s:10:\"Пикап\";s:10:\"Пикап\";s:18:\"Универсал\";s:18:\"Универсал\";s:14:\"Хэтчбек\";s:14:\"Хэтчбек\";s:14:\"Минивэн\";s:14:\"Минивэн\";s:22:\"Внедорожник\";s:22:\"Внедорожник\";}}',NULL),(8,0,'VarcharDataType','fuel','Вид топлива','a:2:{s:5:\"rules\";a:1:{s:6:\"length\";a:2:{s:3:\"max\";i:100;s:3:\"min\";i:2;}}s:6:\"values\";a:3:{s:12:\"Бензин\";s:12:\"Бензин\";s:12:\"Дизель\";s:12:\"Дизель\";s:12:\"Другой\";s:12:\"Другой\";}}',NULL),(9,0,'VarcharDataType','gear','Коробка передач','a:2:{s:5:\"rules\";a:1:{s:6:\"length\";a:2:{s:3:\"max\";i:100;s:3:\"min\";i:2;}}s:6:\"values\";a:3:{s:24:\"Механическая\";s:24:\"Механическая\";s:28:\"Автоматическая\";s:28:\"Автоматическая\";s:12:\"Другая\";s:12:\"Другая\";}}',NULL),(10,0,'VarcharDataType','color','Цвет','a:2:{s:5:\"rules\";a:1:{s:6:\"length\";a:2:{s:3:\"max\";i:100;s:3:\"min\";i:2;}}s:6:\"values\";a:14:{s:10:\"Белый\";s:10:\"Белый\";s:12:\"Черный\";s:12:\"Черный\";s:10:\"Серый\";s:10:\"Серый\";s:10:\"Синий\";s:10:\"Синий\";s:22:\"Серебристый\";s:22:\"Серебристый\";s:14:\"Красный\";s:14:\"Красный\";s:14:\"Зеленый\";s:14:\"Зеленый\";s:14:\"Бежевый\";s:14:\"Бежевый\";s:14:\"Голубой\";s:14:\"Голубой\";s:12:\"Желтый\";s:12:\"Желтый\";s:20:\"Коричневый\";s:20:\"Коричневый\";s:18:\"Оливковый\";s:18:\"Оливковый\";s:14:\"Розовый\";s:14:\"Розовый\";s:20:\"Фиолетовый\";s:20:\"Фиолетовый\";}}',NULL),(11,0,'VarcharDataType','truckBody','Тип кузова','a:2:{s:5:\"rules\";a:1:{s:6:\"length\";a:2:{s:3:\"max\";i:100;s:3:\"min\";i:2;}}s:6:\"values\";a:14:{s:14:\"Автовоз\";s:14:\"Автовоз\";s:24:\"Автоцистерна\";s:24:\"Автоцистерна\";s:33:\"Бортовой грузовик\";s:33:\"Бортовой грузовик\";s:31:\"Вакуумная машина\";s:31:\"Вакуумная машина\";s:36:\"Для перевозки скота\";s:36:\"Для перевозки скота\";s:39:\"Изотермический кузов\";s:39:\"Изотермический кузов\";s:14:\"Лесовоз\";s:14:\"Лесовоз\";s:16:\"Самосвал\";s:16:\"Самосвал\";s:26:\"Сортиментовоз\";s:26:\"Сортиментовоз\";s:20:\"Термокузов\";s:20:\"Термокузов\";s:12:\"Фургон\";s:12:\"Фургон\";s:10:\"Шасси\";s:10:\"Шасси\";s:18:\"Эвакуатор\";s:18:\"Эвакуатор\";s:12:\"Другой\";s:12:\"Другой\";}}',NULL),(12,0,'IntDataType','clothingSize','Размер','a:1:{s:5:\"rules\";a:1:{s:9:\"numerical\";a:3:{s:3:\"max\";i:150;s:3:\"min\";i:1;s:11:\"integerOnly\";b:1;}}}',NULL),(13,0,'IntDataType','shoeSize','Размер','a:1:{s:5:\"rules\";a:1:{s:9:\"numerical\";a:3:{s:3:\"max\";i:36;s:3:\"min\";i:1;s:11:\"integerOnly\";b:1;}}}',NULL),(14,0,'IntDataType','roomCount','Кол-во комнат','a:1:{s:5:\"rules\";a:1:{s:9:\"numerical\";a:3:{s:3:\"max\";i:10;s:3:\"min\";i:1;s:11:\"integerOnly\";b:1;}}}',NULL),(15,0,'IntDataType','floor','Этаж','a:1:{s:5:\"rules\";a:1:{s:9:\"numerical\";a:3:{s:3:\"max\";i:100;s:3:\"min\";i:1;s:11:\"integerOnly\";b:1;}}}',NULL),(16,0,'IntDataType','roomTotal','Всего комнат','a:1:{s:5:\"rules\";a:1:{s:9:\"numerical\";a:3:{s:3:\"max\";i:10;s:3:\"min\";i:1;s:11:\"integerOnly\";b:1;}}}',NULL),(17,0,'IntDataType','salary','Зарплата','a:1:{s:5:\"rules\";a:1:{s:9:\"numerical\";a:3:{s:3:\"max\";i:1000000;s:3:\"min\";i:1;s:11:\"integerOnly\";b:1;}}}','руб'),(18,0,'VarcharDataType','duration','Вид занятости','a:2:{s:5:\"rules\";a:1:{s:6:\"length\";a:2:{s:3:\"max\";i:100;s:3:\"min\";i:2;}}s:6:\"values\";a:2:{s:33:\"Постоянная работа\";s:33:\"Постоянная работа\";s:31:\"Временная работа\";s:31:\"Временная работа\";}}',NULL),(19,0,'VarcharDataType','employmentType','Тип занятости','a:2:{s:5:\"rules\";a:1:{s:6:\"length\";a:2:{s:3:\"max\";i:100;s:3:\"min\";i:2;}}s:6:\"values\";a:2:{s:30:\"На полную ставку\";s:30:\"На полную ставку\";s:35:\"Неполная занятость\";s:35:\"Неполная занятость\";}}',NULL);
/*!40000 ALTER TABLE `eav_attribute` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `eav_attribute_date`
--

DROP TABLE IF EXISTS `eav_attribute_date`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `eav_attribute_date` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `eav_attribute_id` int(10) unsigned NOT NULL COMMENT 'Foreign key references eav_attribute(id)',
  `entity_id` int(11) NOT NULL COMMENT 'Primary key of an entity',
  `entity` varchar(255) NOT NULL COMMENT 'The entity name',
  `value` datetime NOT NULL COMMENT 'The value of the attribute',
  PRIMARY KEY (`id`),
  KEY `no_eav_attribute_date_entity_entity_id` (`entity`,`entity_id`) USING BTREE,
  KEY `no_eav_attribute_date_eav_attribute_id` (`eav_attribute_id`) USING BTREE,
  KEY `no_eav_attribute_date_value` (`value`) USING BTREE,
  CONSTRAINT `fk_eav_attribute_id_eav_attribute_date` FOREIGN KEY (`eav_attribute_id`) REFERENCES `eav_attribute` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `eav_attribute_date`
--

LOCK TABLES `eav_attribute_date` WRITE;
/*!40000 ALTER TABLE `eav_attribute_date` DISABLE KEYS */;
/*!40000 ALTER TABLE `eav_attribute_date` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `eav_attribute_int`
--

DROP TABLE IF EXISTS `eav_attribute_int`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `eav_attribute_int` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `eav_attribute_id` int(10) unsigned NOT NULL COMMENT 'Foreign key references eav_attribute(id)',
  `entity_id` int(11) NOT NULL COMMENT 'Primary key of an entity',
  `entity` varchar(255) NOT NULL COMMENT 'The entity name',
  `value` int(11) NOT NULL COMMENT 'The value of the attribute',
  PRIMARY KEY (`id`),
  KEY `no_eav_attribute_int_entity_entity_id` (`entity`,`entity_id`) USING BTREE,
  KEY `no_eav_attribute_int_eav_attribute_id` (`eav_attribute_id`) USING BTREE,
  KEY `no_eav_attribute_int_value` (`value`) USING BTREE,
  CONSTRAINT `fk_eav_attribute_id_eav_attribute_int` FOREIGN KEY (`eav_attribute_id`) REFERENCES `eav_attribute` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=779290 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `eav_attribute_int`
--

LOCK TABLES `eav_attribute_int` WRITE;
/*!40000 ALTER TABLE `eav_attribute_int` DISABLE KEYS */;
/*!40000 ALTER TABLE `eav_attribute_int` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `eav_attribute_money`
--

DROP TABLE IF EXISTS `eav_attribute_money`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `eav_attribute_money` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `eav_attribute_id` int(10) unsigned NOT NULL COMMENT 'Foreign key references eav_attribute(id)',
  `entity_id` int(11) NOT NULL COMMENT 'Primary key of an entity',
  `entity` varchar(255) NOT NULL COMMENT 'The entity name',
  `value` decimal(13,2) NOT NULL COMMENT 'The value of the attribute',
  PRIMARY KEY (`id`),
  KEY `no_eav_attribute_money_entity_entity_id` (`entity`,`entity_id`) USING BTREE,
  KEY `no_eav_attribute_money_eav_attribute_id` (`eav_attribute_id`) USING BTREE,
  CONSTRAINT `fk_eav_attribute_id_eav_attribute_money` FOREIGN KEY (`eav_attribute_id`) REFERENCES `eav_attribute` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `eav_attribute_money`
--

LOCK TABLES `eav_attribute_money` WRITE;
/*!40000 ALTER TABLE `eav_attribute_money` DISABLE KEYS */;
/*!40000 ALTER TABLE `eav_attribute_money` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `eav_attribute_numeric`
--

DROP TABLE IF EXISTS `eav_attribute_numeric`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `eav_attribute_numeric` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `eav_attribute_id` int(10) unsigned NOT NULL COMMENT 'Foreign key references eav_attribute(id)',
  `entity_id` int(11) NOT NULL COMMENT 'Primary key of an entity',
  `entity` varchar(255) NOT NULL COMMENT 'The entity name',
  `value` double NOT NULL COMMENT 'The value of the attribute',
  PRIMARY KEY (`id`),
  KEY `no_eav_attribute_numeric_entity_entity_id` (`entity`,`entity_id`) USING BTREE,
  KEY `no_eav_attribute_numeric_eav_attribute_id` (`eav_attribute_id`) USING BTREE,
  CONSTRAINT `fk_eav_attribute_id_eav_attribute_numeric` FOREIGN KEY (`eav_attribute_id`) REFERENCES `eav_attribute` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `eav_attribute_numeric`
--

LOCK TABLES `eav_attribute_numeric` WRITE;
/*!40000 ALTER TABLE `eav_attribute_numeric` DISABLE KEYS */;
/*!40000 ALTER TABLE `eav_attribute_numeric` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `eav_attribute_set`
--

DROP TABLE IF EXISTS `eav_attribute_set`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `eav_attribute_set` (
  `eav_attribute_id` int(10) unsigned NOT NULL COMMENT 'Composite primary key',
  `eav_set_id` int(10) unsigned NOT NULL COMMENT 'Composite primary key',
  `weight` int(10) NOT NULL COMMENT 'The weight of the attribute',
  PRIMARY KEY (`eav_attribute_id`,`eav_set_id`),
  KEY `no_eav_attribute_set_attribute_id` (`eav_attribute_id`) USING BTREE,
  KEY `no_eav_attribute_set_set_id` (`eav_set_id`) USING BTREE,
  KEY `no_eav_attribute_set_weight` (`weight`) USING BTREE,
  CONSTRAINT `fk_eav_attribute_id_eav_attribute_set` FOREIGN KEY (`eav_attribute_id`) REFERENCES `eav_attribute` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `fk_eav_set_id_eav_attribute_set` FOREIGN KEY (`eav_set_id`) REFERENCES `eav_set` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `eav_attribute_set`
--

LOCK TABLES `eav_attribute_set` WRITE;
/*!40000 ALTER TABLE `eav_attribute_set` DISABLE KEYS */;
INSERT INTO `eav_attribute_set` VALUES (1,1,1),(1,2,1),(1,3,1),(1,5,1),(1,6,1),(1,7,1),(1,9,1),(1,10,1),(1,11,1),(3,4,1),(17,8,1),(2,6,2),(2,7,2),(2,10,2),(3,5,2),(3,9,2),(4,1,2),(5,3,2),(11,2,2),(12,4,2),(18,8,2),(1,4,3),(4,3,3),(5,1,3),(13,5,3),(14,6,3),(16,7,3),(19,8,3),(6,1,4),(6,3,4),(15,6,4),(7,1,5),(8,1,6),(9,1,7),(10,1,8);
/*!40000 ALTER TABLE `eav_attribute_set` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `eav_attribute_text`
--

DROP TABLE IF EXISTS `eav_attribute_text`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `eav_attribute_text` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `eav_attribute_id` int(10) unsigned NOT NULL COMMENT 'Foreign key references eav_attribute(id)',
  `entity_id` int(11) NOT NULL COMMENT 'Primary key of an entity',
  `entity` varchar(255) NOT NULL COMMENT 'The entity name',
  `value` text COMMENT 'The value of the attribute',
  PRIMARY KEY (`id`),
  KEY `no_eav_attribute_text_entity_entity_id` (`entity`,`entity_id`) USING BTREE,
  KEY `no_eav_attribute_text_eav_attribute_id` (`eav_attribute_id`) USING BTREE,
  CONSTRAINT `fk_eav_attribute_id_eav_attribute_text` FOREIGN KEY (`eav_attribute_id`) REFERENCES `eav_attribute` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `eav_attribute_text`
--

LOCK TABLES `eav_attribute_text` WRITE;
/*!40000 ALTER TABLE `eav_attribute_text` DISABLE KEYS */;
/*!40000 ALTER TABLE `eav_attribute_text` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `eav_attribute_varchar`
--

DROP TABLE IF EXISTS `eav_attribute_varchar`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `eav_attribute_varchar` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `eav_attribute_id` int(10) unsigned NOT NULL COMMENT 'Foreign key references eav_attribute(id)',
  `entity_id` int(11) NOT NULL COMMENT 'Primary key of an entity',
  `entity` varchar(255) NOT NULL COMMENT 'The entity name',
  `value` varchar(255) NOT NULL COMMENT 'The value of the attribute',
  PRIMARY KEY (`id`),
  KEY `no_eav_attribute_varchar_entity_entity_id` (`entity`,`entity_id`) USING BTREE,
  KEY `no_eav_attribute_varchar_eav_attribute_id` (`eav_attribute_id`) USING BTREE,
  CONSTRAINT `fk_eav_attribute_id_eav_attribute_varchar` FOREIGN KEY (`eav_attribute_id`) REFERENCES `eav_attribute` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `eav_attribute_varchar`
--

LOCK TABLES `eav_attribute_varchar` WRITE;
/*!40000 ALTER TABLE `eav_attribute_varchar` DISABLE KEYS */;
/*!40000 ALTER TABLE `eav_attribute_varchar` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `eav_set`
--

DROP TABLE IF EXISTS `eav_set`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `eav_set` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `name` varchar(255) NOT NULL COMMENT 'Set name',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `eav_set`
--

LOCK TABLES `eav_set` WRITE;
/*!40000 ALTER TABLE `eav_set` DISABLE KEYS */;
INSERT INTO `eav_set` VALUES (1,'Cars'),(2,'Trucks'),(3,'Moto'),(4,'Cloth'),(5,'Shoes'),(6,'Apartments'),(7,'Rooms'),(8,'Jobs'),(9,'Price and condition'),(10,'Price and area'),(11,'Price'),(12,'Empty set');
/*!40000 ALTER TABLE `eav_set` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `photo`
--

DROP TABLE IF EXISTS `photo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `photo` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL,
  `upload_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `ad_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `ad_id` (`ad_id`),
  CONSTRAINT `photo_ibfk_1` FOREIGN KEY (`ad_id`) REFERENCES `ad` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `photo`
--

LOCK TABLES `photo` WRITE;
/*!40000 ALTER TABLE `photo` DISABLE KEYS */;
/*!40000 ALTER TABLE `photo` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `region`
--

DROP TABLE IF EXISTS `region`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `region` (
  `region_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `country_id` int(10) unsigned NOT NULL DEFAULT '0',
  `city_id` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(64) NOT NULL DEFAULT '',
  PRIMARY KEY (`region_id`),
  KEY `country_id` (`country_id`),
  KEY `city_id` (`city_id`),
  CONSTRAINT `region_ibfk_1` FOREIGN KEY (`country_id`) REFERENCES `country` (`country_id`)
) ENGINE=InnoDB AUTO_INCREMENT=15789406 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `region`
--

LOCK TABLES `region` WRITE;
/*!40000 ALTER TABLE `region` DISABLE KEYS */;
INSERT INTO `region` VALUES (5,4,0,'Виктория'),(16,4,0,'Западная Австралия'),(23,4,0,'Кинсленд'),(35,4,0,'Новый Южный Уэльс'),(51,4,0,'Северная Территория'),(54,4,0,'Тасмания'),(58,4,0,'Территория Федеральной столицы'),(60,4,0,'Южная Австралия'),(64,63,0,'Вена'),(66,63,0,'Верхняя Австрия'),(70,63,0,'Зальцбург'),(72,63,0,'Каринтия'),(75,63,0,'Нижняя Австрия'),(77,63,0,'Штирия'),(82,81,0,'Азербайджан'),(164,81,0,'Нагорный Карабах'),(167,81,0,'Нахичеванская обл.'),(174,173,0,'Anguilla'),(178,177,0,'Буэнос-Айрес'),(194,177,0,'Жужуй'),(196,177,0,'Катамарка'),(198,177,0,'Кордоба'),(204,177,0,'Корриентес'),(207,177,0,'Ла-Пампа'),(210,177,0,'Ла-Риоха'),(212,177,0,'Мендоса'),(215,177,0,'Мисьонес'),(217,177,0,'Неукен'),(219,177,0,'Сальта'),(221,177,0,'Сан-Луис'),(223,177,0,'Сан-Хуан'),(225,177,0,'Санта-Крус'),(227,177,0,'Санта-Фе'),(230,177,0,'Сантьяго-дель-Эстеро'),(232,177,0,'Тукуман'),(234,177,0,'Формоса'),(236,177,0,'Чако'),(239,177,0,'Чубут'),(241,177,0,'Энтре-Риос'),(246,245,0,'Ереванская область'),(249,248,0,'Брестская обл.'),(272,248,0,'Витебская обл.'),(304,248,0,'Гомельская обл.'),(330,248,0,'Гродненская обл.'),(349,248,0,'Минская обл.'),(377,248,0,'Могилевская обл.'),(402,401,0,'Belize'),(405,404,0,'Антверпен'),(408,404,0,'Брюссель'),(410,404,0,'Восточная Фландрия'),(413,404,0,'Западная Фландрия'),(417,404,0,'Лимбург'),(419,404,0,'Льеж'),(422,404,0,'Эно'),(426,425,0,'Hamilton'),(429,428,0,'Бургас'),(431,428,0,'Варна'),(433,428,0,'Великотырново'),(436,428,0,'Видин'),(438,428,0,'Габрово'),(440,428,0,'Пазарджик'),(442,428,0,'Перник'),(444,428,0,'Плевен'),(446,428,0,'Пловдив'),(449,428,0,'Русе'),(451,428,0,'Силистра'),(453,428,0,'Сливен'),(455,428,0,'София град'),(457,428,0,'Стара Загора'),(460,428,0,'Хасково'),(463,428,0,'Шумен'),(465,428,0,'Ямбол'),(468,467,0,'Акри'),(470,467,0,'Алагоас'),(473,467,0,'Амазонас'),(475,467,0,'Амапа'),(477,467,0,'Баия'),(490,467,0,'Гояс'),(493,467,0,'Мараньян'),(497,467,0,'Мату-Гросу'),(499,467,0,'Мату-Гросу-ду-Сул'),(503,467,0,'Минас-Жерайс'),(521,467,0,'Пара'),(524,467,0,'Параиба'),(527,467,0,'Парана'),(533,467,0,'Пернамбуку'),(539,467,0,'Пиауи'),(542,467,0,'Рио-де-Жанейро'),(550,467,0,'Риу-Гранди-ду-Норти'),(554,467,0,'Риу-Гранди-ду-Сул'),(568,467,0,'Рондония'),(570,467,0,'Сан-Паулу'),(595,467,0,'Санта-Катарина'),(603,467,0,'Сеара'),(609,467,0,'Сержипи'),(611,467,0,'Федеральный округ'),(613,467,0,'Эспириту-Санту'),(617,616,0,'Англия'),(881,616,0,'Северная Ирландия'),(889,616,0,'Уэльс'),(899,616,0,'Шотландия'),(925,924,0,'Jasz-Nagykun-Szolnok'),(928,924,0,'Баранья'),(930,924,0,'Бач-Кишкун'),(934,924,0,'Бекеш'),(936,924,0,'Боршод-Абауй-Земплен'),(939,924,0,'Будапешт'),(941,924,0,'Ваш'),(943,924,0,'Веспрем'),(945,924,0,'Дьёр-Шопрон'),(947,924,0,'Зала'),(950,924,0,'Комаром'),(952,924,0,'Ноград'),(954,924,0,'Пешт'),(956,924,0,'Сабольч-Сатмар'),(958,924,0,'Фейер'),(961,924,0,'Хайду-Бихар'),(964,924,0,'Хевеш'),(967,924,0,'Чонград'),(972,971,0,'Dong Bang Song Cuu Long'),(975,971,0,'Dong Bang Song Hong'),(979,971,0,'Dong Nam'),(982,971,0,'Duyen Hai Mien Trung'),(987,971,0,'Khu Bon'),(990,971,0,'Mien Nui Va Trung'),(992,971,0,'Thai Nguyen'),(995,994,0,'Artibonite'),(997,994,0,'Grand`Anse'),(999,994,0,'Nord-Ouest'),(1001,994,0,'Ouest'),(1003,994,0,'Sud'),(1005,994,0,'Sud-Est'),(1008,1007,0,'Grande-Terre'),(1010,1007,0,'Бас-Тер'),(1013,1012,0,'Berlin'),(1015,1012,0,'Brandenburg'),(1020,1012,0,'Mecklenburg-Vorpommern'),(1028,1012,0,'Sachsen'),(1043,1012,0,'Sachsen-Anhalt'),(1053,1012,0,'Thuringen'),(1064,1012,0,'Бавария'),(1083,1012,0,'Баден-Вюртемберг'),(1103,1012,0,'Бремен'),(1106,1012,0,'Гамбург'),(1108,1012,0,'Гессен'),(1120,1012,0,'Нижняя Саксония'),(1137,1012,0,'Рейнланд-Пфальц'),(1149,1012,0,'Саарская область'),(1152,1012,0,'Северный Рейн-Вестфалия'),(1201,1012,0,'Шлезвиг-Гольштейн'),(1207,1206,0,'Гелдерланд'),(1213,1206,0,'Гронинген'),(1215,1206,0,'Дренте'),(1217,1206,0,'Лимбург'),(1222,1206,0,'Оверэйсел'),(1227,1206,0,'Северная Голландия'),(1235,1206,0,'Северный Брабант'),(1240,1206,0,'Утрехт'),(1244,1206,0,'Фрисландия'),(1246,1206,0,'Южная Голландия'),(1259,1258,0,'Ahaia'),(1261,1258,0,'Attiki'),(1264,1258,0,'Drama'),(1266,1258,0,'Evros'),(1268,1258,0,'Hania'),(1270,1258,0,'Ioannina'),(1272,1258,0,'Iraklion'),(1274,1258,0,'Kavala'),(1276,1258,0,'Magnisia'),(1278,1258,0,'Thessaloniki'),(1281,1280,0,'Абхазия'),(1292,1280,0,'Аджария'),(1296,1280,0,'Грузия'),(1363,1280,0,'Южная Осетия'),(1367,1366,0,'Baйле'),(1369,1366,0,'Ceверная Ютландия'),(1371,1366,0,'Opxyс'),(1374,1366,0,'Pocкилле'),(1376,1366,0,'Pибе'),(1378,1366,0,'Фюн'),(1381,1380,0,'Аль-Кахира'),(1383,1380,0,'Асуан'),(1385,1380,0,'Асьют'),(1387,1380,0,'Бени-Суэйф'),(1389,1380,0,'Гарбия'),(1391,1380,0,'Думьят'),(1394,1393,0,'Хадаром'),(1403,1393,0,'Хамеркац'),(1416,1393,0,'Хацафон'),(1429,1393,0,'Хефа'),(1440,1393,0,'Тель-Авив'),(1449,1393,0,'Иерусалим'),(1452,1451,0,'Bangla'),(1468,1451,0,'Chhattisgarh'),(1473,1451,0,'Karnataka'),(1488,1451,0,'Uttaranchal'),(1491,1451,0,'Андхра-Прадеш'),(1509,1451,0,'Ассам'),(1512,1451,0,'Бихар'),(1521,1451,0,'Гуджарат'),(1539,1451,0,'Джамму и Кашмир'),(1545,1451,0,'Керала'),(1548,1451,0,'Мадхья-Прадеш'),(1561,1451,0,'Манипур'),(1563,1451,0,'Махараштра'),(1586,1451,0,'Мегхалая'),(1588,1451,0,'Орисса'),(1591,1451,0,'Пенджаб'),(1596,1451,0,'Пондишери'),(1598,1451,0,'Раджастхан'),(1612,1451,0,'Тамилнад'),(1629,1451,0,'Трипура'),(1631,1451,0,'Уттар-Прадеш'),(1656,1451,0,'Харьяна'),(1661,1451,0,'Чандигарх'),(1664,1663,0,'Azarbayjan-e Khavari'),(1667,1663,0,'Esfahan'),(1669,1663,0,'Hamadan'),(1671,1663,0,'Kordestan'),(1673,1663,0,'Markazi'),(1675,1663,0,'Sistan-e Baluches'),(1677,1663,0,'Yazd'),(1679,1663,0,'Керман'),(1681,1663,0,'Керманшахан'),(1683,1663,0,'Мазендеран'),(1686,1663,0,'Тегеран'),(1688,1663,0,'Фарс'),(1691,1663,0,'Хорасан'),(1693,1663,0,'Хузистан'),(1697,1696,0,'Cork'),(1699,1696,0,'Dublin'),(1702,1696,0,'Limerick'),(1704,1696,0,'Waterford'),(1708,1707,0,'Cantabria'),(1710,1707,0,'Ceuta'),(1712,1707,0,'Granada'),(1714,1707,0,'La Rioja'),(1716,1707,0,'Salamanca'),(1718,1707,0,'Алава'),(1720,1707,0,'Аликанте'),(1724,1707,0,'Альбасете'),(1726,1707,0,'Альмерия'),(1728,1707,0,'Астурия'),(1731,1707,0,'Барселона'),(1738,1707,0,'Бискайя'),(1740,1707,0,'Бургос'),(1742,1707,0,'Валенсия'),(1744,1707,0,'Валья­долид'),(1746,1707,0,'Кадис'),(1749,1707,0,'Касерес'),(1753,1707,0,'Кордова'),(1755,1707,0,'Лас-Пальмас'),(1757,1707,0,'Леон'),(1759,1707,0,'Лерида'),(1761,1707,0,'Луго'),(1763,1707,0,'Мадрид'),(1765,1707,0,'Малага'),(1767,1707,0,'Мурсия'),(1769,1707,0,'Острова Балеарские'),(1771,1707,0,'Паленсия'),(1774,1707,0,'Самора'),(1776,1707,0,'Санта-Крус-де-Тенерифе'),(1778,1707,0,'Сарагоса'),(1780,1707,0,'Севилья'),(1782,1707,0,'Сьюдад-Реаль'),(1784,1707,0,'Хаэн'),(1787,1786,0,'Абруцци'),(1789,1786,0,'Апулия'),(1802,1786,0,'Венеция'),(1806,1786,0,'Калабрия'),(1810,1786,0,'Кампания'),(1819,1786,0,'Лацио'),(1821,1786,0,'Лигурия'),(1826,1786,0,'Ломбардия'),(1838,1786,0,'Марке'),(1841,1786,0,'О. Сардиния'),(1844,1786,0,'О. Сицилия'),(1858,1786,0,'Пьемонт'),(1864,1786,0,'Тоскана'),(1876,1786,0,'Трентино-Альто-Адидже'),(1879,1786,0,'Умбрия'),(1882,1786,0,'Фриули-Венеция-Джулия'),(1885,1786,0,'Эмилия-Романья'),(1895,1894,0,'Актюбинская обл.'),(1911,1894,0,'Алма-Атинская обл.'),(1924,1894,0,'Восточно-Казахстанская обл.'),(1942,1894,0,'Гурьевская обл.'),(1954,1894,0,'Джамбулская обл.'),(1974,1894,0,'Джезказганская обл.'),(1994,1894,0,'Карагандинская обл.'),(2010,1894,0,'Кзыл-Ординская обл.'),(2021,1894,0,'Кокчетавская обл.'),(2040,1894,0,'Кустанайская обл.'),(2055,1894,0,'Мангышлакская обл.'),(2061,1894,0,'Павлодарская обл.'),(2074,1894,0,'Северо-Казахстанская обл.'),(2084,1894,0,'Семипалатинская обл.'),(2105,1894,0,'Талды-Курганская обл.'),(2120,1894,0,'Тургайская обл.'),(2128,1894,0,'Целиноградская обл.'),(2146,1894,0,'Чимкентская обл.'),(2164,2163,0,'Littoral'),(2166,2163,0,'Sudouest'),(2168,2163,0,'Северная'),(2170,2163,0,'Центральная'),(2173,2172,0,'Альберта'),(2181,2172,0,'Британская Колумбия'),(2201,2172,0,'Квебек'),(2229,2172,0,'Манитоба'),(2232,2172,0,'Новая Шотландия'),(2235,2172,0,'Нью-Брунсвик'),(2239,2172,0,'Ньюфаундленд'),(2242,2172,0,'Онтарио'),(2291,2172,0,'Принц Эдуард'),(2293,2172,0,'Саскачеван'),(2298,2297,0,'Government controlled area'),(2300,2297,0,'Turkish controlled area'),(2304,2303,0,'Иссык-Кульская обл.'),(2316,2303,0,'Кыргызстан'),(2327,2303,0,'Нарынская обл.'),(2340,2303,0,'Ошская обл.'),(2365,2303,0,'Таласская обл.'),(2375,2374,0,'Xinjiang'),(2379,2374,0,'Внутренняя Монголия'),(2382,2374,0,'Ганьсу'),(2386,2374,0,'Гуанси'),(2388,2374,0,'Ляонин'),(2393,2374,0,'Сычуань'),(2396,2374,0,'Фуцзянь'),(2398,2374,0,'Хубэй'),(2401,2374,0,'Хунань'),(2404,2374,0,'Хэбэй'),(2406,2374,0,'Хэйлунцзян'),(2408,2374,0,'Хэнань'),(2411,2374,0,'Цзилинь'),(2414,2374,0,'Цзянси'),(2416,2374,0,'Цзянсу'),(2419,2374,0,'Чжэцзян'),(2424,2374,0,'Шанхай'),(2426,2374,0,'Шаньдун'),(2428,2374,0,'Шаньси'),(2431,2430,0,'Alajuela'),(2433,2430,0,'Guanacaste'),(2435,2430,0,'Heredia'),(2437,2430,0,'Limon'),(2439,2430,0,'Puntarenas'),(2441,2430,0,'San Jose'),(2444,2443,0,'al-Jahra'),(2446,2443,0,'al-Kuwayt'),(2449,2448,0,'Латвия'),(2510,2509,0,'Tarabulus'),(2512,2509,0,'Бенгази'),(2515,2514,0,'Литва'),(2615,2614,0,'Люксембург'),(2618,2617,0,'Агуаскальентес'),(2620,2617,0,'Веракрус'),(2639,2617,0,'Герреро'),(2646,2617,0,'Гуанахуато'),(2656,2617,0,'Дуранго'),(2661,2617,0,'Идальго'),(2666,2617,0,'Кампече'),(2668,2617,0,'Керетаро'),(2670,2617,0,'Керетаро'),(2672,2617,0,'Коауила'),(2679,2617,0,'Колима'),(2683,2617,0,'Мехико'),(2690,2617,0,'Мичоакан'),(2700,2617,0,'Морелос'),(2704,2617,0,'Наярит'),(2709,2617,0,'Нижняя Калифорния'),(2713,2617,0,'Нуэво-Леон'),(2717,2617,0,'Оахака'),(2725,2617,0,'Пуэбла'),(2730,2617,0,'Сакатекас'),(2735,2617,0,'Сан-Луис-Потоси'),(2741,2617,0,'Синалоа'),(2746,2617,0,'Сонора'),(2755,2617,0,'Табаско'),(2759,2617,0,'Тамаулипас'),(2766,2617,0,'Федеральный округ'),(2768,2617,0,'Халиско'),(2778,2617,0,'Чиуауа'),(2780,2617,0,'Чьяпас'),(2786,2617,0,'Юкатан'),(2789,2788,0,'Молдова'),(2834,2833,0,'Монако'),(2838,2837,0,'Auckland'),(2841,2837,0,'Bay of Plenty'),(2844,2837,0,'Canterbury'),(2847,2837,0,'Gisborne'),(2849,2837,0,'Hawke`s Bay'),(2852,2837,0,'Manawatu-Wanganui'),(2855,2837,0,'Marlborough'),(2857,2837,0,'Nelson'),(2859,2837,0,'Northland'),(2861,2837,0,'Otago'),(2863,2837,0,'Southland'),(2866,2837,0,'Taranaki'),(2869,2837,0,'Tasman'),(2871,2837,0,'Waikato'),(2874,2837,0,'Wellington'),(2878,2837,0,'West Coast'),(2881,2880,0,'Бускеруд'),(2883,2880,0,'Вестагдер'),(2885,2880,0,'Нурлан'),(2887,2880,0,'Осло'),(2889,2880,0,'Ругаланн'),(2891,2880,0,'Сёр-Трённелаг'),(2893,2880,0,'Телемарк'),(2895,2880,0,'Хуралан'),(2898,2897,0,'Dolnoslaskie'),(2919,2897,0,'Kujawsko-Pomorskie'),(2929,2897,0,'Lubuskie'),(2937,2897,0,'Malopolskie'),(2958,2897,0,'Mazowieckie'),(2983,2897,0,'Podkarpackie'),(2994,2897,0,'Podlaskie'),(3003,2897,0,'Pomorskie'),(3018,2897,0,'Slaskie'),(3049,2897,0,'Swietokrzyskie'),(3056,2897,0,'Warminsko-Mazurskie'),(3068,2897,0,'Wielkopolskie'),(3090,2897,0,'Zachodnio-Pomorskie'),(3101,2897,0,'Лодзинское'),(3118,2897,0,'Люблинское'),(3133,2897,0,'Опольское'),(3142,3141,0,'Centro'),(3144,3141,0,'O-ва Мадейра'),(3146,3141,0,'Лисабон'),(3150,3141,0,'Орта'),(3157,3156,0,'Saint-Denis'),(3160,3159,0,'Алтайский край'),(3223,3159,0,'Амурская обл.'),(3251,3159,0,'Архангельская обл.'),(3282,3159,0,'Астраханская обл.'),(3296,3159,0,'Башкортостан(Башкирия)'),(3352,3159,0,'Белгородская обл.'),(3371,3159,0,'Брянская обл.'),(3407,3159,0,'Бурятия'),(3437,3159,0,'Владимирская обл.'),(3468,3159,0,'Волгоградская обл.'),(3503,3159,0,'Вологодская обл.'),(3529,3159,0,'Воронежская обл.'),(3563,3159,0,'Нижегородская (Горьковская)'),(3630,3159,0,'Дагестан'),(3673,3159,0,'Еврейская обл.'),(3675,3159,0,'Ивановская обл.'),(3703,3159,0,'Иркутская обл.'),(3751,3159,0,'Кабардино-Балкария'),(3761,3159,0,'Калининградская обл.'),(3784,3159,0,'Тверская обл.'),(3827,3159,0,'Калмыкия'),(3841,3159,0,'Калужская обл.'),(3872,3159,0,'Камчатская обл.'),(3892,3159,0,'Карелия'),(3921,3159,0,'Кемеровская обл.'),(3952,3159,0,'Кировская обл.'),(3994,3159,0,'Коми'),(4026,3159,0,'Костромская обл.'),(4052,3159,0,'Краснодарский край'),(4105,3159,0,'Красноярский край'),(4176,3159,0,'Курганская обл.'),(4198,3159,0,'Курская обл.'),(4227,3159,0,'Липецкая обл.'),(4243,3159,0,'Магаданская обл.'),(4270,3159,0,'Марий Эл'),(4287,3159,0,'Мордовия'),(4312,3159,0,'Москва и Московская обл.'),(4481,3159,0,'Мурманская обл.'),(4503,3159,0,'Новгородская обл.'),(4528,3159,0,'Новосибирская обл.'),(4561,3159,0,'Омская обл.'),(4593,3159,0,'Оренбургская обл.'),(4633,3159,0,'Орловская обл.'),(4657,3159,0,'Пензенская обл.'),(4689,3159,0,'Пермская обл.'),(4734,3159,0,'Приморский край'),(4773,3159,0,'Псковская обл.'),(4800,3159,0,'Ростовская обл.'),(4861,3159,0,'Рязанская обл.'),(4891,3159,0,'Самарская обл.'),(4925,3159,0,'Санкт-Петербург и область'),(4969,3159,0,'Саратовская обл.'),(5011,3159,0,'Саха (Якутия)'),(5052,3159,0,'Сахалин'),(5080,3159,0,'Свердловская обл.'),(5151,3159,0,'Северная Осетия'),(5161,3159,0,'Смоленская обл.'),(5191,3159,0,'Ставропольский край'),(5225,3159,0,'Тамбовская обл.'),(5246,3159,0,'Татарстан'),(5291,3159,0,'Томская обл.'),(5312,3159,0,'Тува (Тувинская Респ.)'),(5326,3159,0,'Тульская обл.'),(5356,3159,0,'Тюменская обл.'),(5404,3159,0,'Удмуртия'),(5432,3159,0,'Ульяновская обл.'),(5458,3159,0,'Уральская обл.'),(5473,3159,0,'Хабаровский край'),(5507,3159,0,'Челябинская обл.'),(5543,3159,0,'Чечено-Ингушетия'),(5555,3159,0,'Читинская обл.'),(5600,3159,0,'Чувашия'),(5625,3159,0,'Ярославская обл.'),(5648,5647,0,'Ahuachapan'),(5650,5647,0,'Cuscatlan'),(5652,5647,0,'La Libertad'),(5654,5647,0,'La Paz'),(5656,5647,0,'La Union'),(5658,5647,0,'San Miguel'),(5660,5647,0,'San Salvador'),(5662,5647,0,'Santa Ana'),(5664,5647,0,'Sonsonate'),(5667,5666,0,'Trnavsky'),(5669,5666,0,'Братислава'),(5671,5666,0,'Жилина'),(5674,5673,0,'Osrednjeslovenska'),(5676,5673,0,'Podravska'),(5679,5678,0,'Paramaribo'),(5682,5681,0,'Айдахо'),(5694,5681,0,'Айова'),(5752,5681,0,'Алабама'),(5924,5681,0,'Аляска'),(5927,5681,0,'Аризона'),(5983,5681,0,'Арканзас'),(6098,5681,0,'Вайоминг'),(6103,5681,0,'Вашингтон'),(6167,5681,0,'Вермонт'),(6176,5681,0,'Виргиния'),(6242,5681,0,'Висконсин'),(6266,5681,0,'Гаваи'),(6280,5681,0,'Делавар'),(6291,5681,0,'Джорджия'),(6352,5681,0,'Западная Виргиния'),(6367,5681,0,'Иллинойс'),(6520,5681,0,'Индиана'),(6591,5681,0,'Калифорния'),(6864,5681,0,'Канзас'),(6917,5681,0,'Кентукки'),(6973,5681,0,'Колорадо'),(7015,5681,0,'Коннектикут'),(7055,5681,0,'Луизиана'),(7120,5681,0,'Массачусетс'),(7228,5681,0,'Миннесота'),(7288,5681,0,'Миссисипи'),(7411,5681,0,'Миссури'),(7493,5681,0,'Мичиган'),(7598,5681,0,'Монтана'),(7613,5681,0,'Мэн'),(7641,5681,0,'Мэриленд'),(7691,5681,0,'Небраска'),(7714,5681,0,'Невада'),(7733,5681,0,'Нью-Гэмпшир'),(7744,5681,0,'Нью-Джерси'),(7859,5681,0,'Нью-Йорк'),(8089,5681,0,'Нью-Мексико'),(8145,5681,0,'Огайо'),(8439,5681,0,'Оклахома'),(8479,5681,0,'окр.Колумбия'),(8483,5681,0,'Орегон'),(8516,5681,0,'Пенсильвания'),(8751,5681,0,'Род-Айленд'),(8760,5681,0,'Северная Дакота'),(8768,5681,0,'Северная Каролина'),(8856,5681,0,'Теннесси'),(9047,5681,0,'Техас'),(9185,5681,0,'Флорида'),(9477,5681,0,'Южная Дакота'),(9489,5681,0,'Южная Каролина'),(9515,5681,0,'Юта'),(9576,9575,0,'Горно-Бадахшанская обл.'),(9589,9575,0,'Кулябская обл.'),(9596,9575,0,'Курган-Тюбинская обл.'),(9605,9575,0,'Согдийская обл.'),(9627,9575,0,'Таджикистан'),(9639,9638,0,'Ашхабадская обл.'),(9653,9638,0,'Красноводская обл.'),(9670,9638,0,'Марыйская обл.'),(9682,9638,0,'Ташаузская обл.'),(9685,9638,0,'Чарджоуская обл.'),(9702,9701,0,'Grand Turk'),(9706,9705,0,'Bartin'),(9708,9705,0,'Bayburt'),(9710,9705,0,'Karabuk'),(9712,9705,0,'Адана'),(9714,9705,0,'Айдын'),(9716,9705,0,'Амасья'),(9718,9705,0,'Анкара'),(9720,9705,0,'Анталья'),(9722,9705,0,'Артвин'),(9724,9705,0,'Афьон'),(9726,9705,0,'Балыкесир'),(9728,9705,0,'Биледжик'),(9730,9705,0,'Бурса'),(9732,9705,0,'Газиантеп'),(9734,9705,0,'Денизли'),(9736,9705,0,'Измир'),(9738,9705,0,'Испарта'),(9740,9705,0,'Ичель'),(9742,9705,0,'Кайсери'),(9744,9705,0,'Карс'),(9746,9705,0,'Коджаэли'),(9749,9705,0,'Конья'),(9751,9705,0,'Кыркларели'),(9753,9705,0,'Кютахья'),(9755,9705,0,'Малатья'),(9757,9705,0,'Маниса'),(9759,9705,0,'Сакарья'),(9761,9705,0,'Самсун'),(9763,9705,0,'Сивас'),(9765,9705,0,'Стамбул'),(9767,9705,0,'Трабзон'),(9770,9705,0,'Чорум'),(9772,9705,0,'Эдирне'),(9774,9705,0,'Элязыг'),(9776,9705,0,'Эрзинджан'),(9778,9705,0,'Эрзурум'),(9780,9705,0,'Эскишехир'),(9783,9782,0,'Jinja'),(9785,9782,0,'Kampala'),(9788,9787,0,'Андижанская обл.'),(9796,9787,0,'Бухарская обл.'),(9806,9787,0,'Джизакская обл.'),(9813,9787,0,'Каракалпакия'),(9825,9787,0,'Кашкадарьинская обл.'),(9832,9787,0,'Навоийская обл.'),(9836,9787,0,'Наманганская обл.'),(9844,9787,0,'Самаркандская обл.'),(9851,9787,0,'Сурхандарьинская обл.'),(9859,9787,0,'Сырдарьинская обл.'),(9869,9787,0,'Ташкентская обл.'),(9892,9787,0,'Ферганская обл.'),(9905,9787,0,'Хорезмская обл.'),(9909,9908,0,'Винницкая обл.'),(9943,9908,0,'Волынская обл.'),(9964,9908,0,'Днепропетровская обл.'),(10002,9908,0,'Донецкая обл.'),(10061,9908,0,'Житомирская обл.'),(10094,9908,0,'Закарпатская обл.'),(10111,9908,0,'Запорожская обл.'),(10133,9908,0,'Ивано-Франковская обл.'),(10165,9908,0,'Киевская обл.'),(10201,9908,0,'Кировоградская обл.'),(10227,9908,0,'Крымская обл.'),(10259,9908,0,'Луганская обл.'),(10318,9908,0,'Львовская обл.'),(10354,9908,0,'Николаевская обл.'),(10373,9908,0,'Одесская обл.'),(10407,9908,0,'Полтавская обл.'),(10437,9908,0,'Ровенская обл.'),(10455,9908,0,'Сумская обл.'),(10480,9908,0,'Тернопольская обл.'),(10504,9908,0,'Харьковская обл.'),(10535,9908,0,'Херсонская обл.'),(10559,9908,0,'Хмельницкая обл.'),(10583,9908,0,'Черкасская обл.'),(10607,9908,0,'Черниговская обл.'),(10633,9908,0,'Черновицкая обл.'),(10649,10648,0,'Etela-Karjala'),(10651,10648,0,'Paijat-Hame'),(10653,10648,0,'Pohjanmaa'),(10655,10648,0,'Pohjois-Pohjanmaa'),(10657,10648,0,'Pohjois-Savo'),(10659,10648,0,'Satakunta'),(10661,10648,0,'Varsinais-Suomi'),(10663,10648,0,'Кески-Суоми'),(10665,10648,0,'Усима'),(10669,10668,0,'Cotes-d`Armor'),(10671,10668,0,'Алье'),(10673,10668,0,'Альпы Верхние'),(10675,10668,0,'Альпы Приморские'),(10679,10668,0,'Арденны'),(10681,10668,0,'Бельфор'),(10683,10668,0,'Буш-дю-Рон'),(10686,10668,0,'Валь-д` Уаз'),(10688,10668,0,'Валь-де-Марн'),(10699,10668,0,'Вар'),(10702,10668,0,'Вогезы'),(10704,10668,0,'Воклюз'),(10706,10668,0,'Вьенна'),(10708,10668,0,'Вьенна Верхняя'),(10710,10668,0,'Гар'),(10712,10668,0,'Гаронна Верхняя'),(10714,10668,0,'Дё-Севр'),(10716,10668,0,'Дром'),(10719,10668,0,'Ду'),(10721,10668,0,'Жиронда'),(10723,10668,0,'Ивелин'),(10725,10668,0,'Иль и Виден'),(10728,10668,0,'Коррез'),(10730,10668,0,'Корсика Верхняя'),(10732,10668,0,'Корсика Южная'),(10734,10668,0,'Кот-д`Ор'),(10736,10668,0,'Ланды'),(10738,10668,0,'Ло и Гаронна'),(10740,10668,0,'Лозер'),(10742,10668,0,'Луар и Шер'),(10744,10668,0,'Луара'),(10747,10668,0,'Луара Атлантическая'),(10750,10668,0,'Луара Верхняя'),(10752,10668,0,'Луаре'),(10754,10668,0,'Майенн'),(10756,10668,0,'Марна'),(10759,10668,0,'Мен и Луара'),(10762,10668,0,'Мёрт и Мозель'),(10764,10668,0,'Мозель'),(10766,10668,0,'Морбиан'),(10768,10668,0,'Нор'),(10774,10668,0,'Ньевр'),(10776,10668,0,'О-де-Сен'),(10794,10668,0,'Об'),(10796,10668,0,'Од'),(10798,10668,0,'Орн'),(10800,10668,0,'Па-де-Кале'),(10804,10668,0,'Париж'),(10806,10668,0,'Пиренеи Атлантические'),(10809,10668,0,'Пиренеи Верхние'),(10811,10668,0,'Пиренеи Восточные'),(10813,10668,0,'Рейн Верхний'),(10817,10668,0,'Рейн Нижний'),(10819,10668,0,'Рона'),(10823,10668,0,'Савойя'),(10825,10668,0,'Савойя Верхняя'),(10827,10668,0,'Сарта'),(10829,10668,0,'Сен-Сен-Дени'),(10841,10668,0,'Сена Приморская'),(10844,10668,0,'Сомма'),(10846,10668,0,'Сона и Луара'),(10849,10668,0,'Тарн'),(10851,10668,0,'Уаза'),(10853,10668,0,'Финистер'),(10856,10668,0,'Шаранта'),(10858,10668,0,'Шаранта Приморская'),(10860,10668,0,'Шер'),(10862,10668,0,'Эна'),(10864,10668,0,'Эндр'),(10866,10668,0,'Эндр и Луара'),(10868,10668,0,'Эр'),(10870,10668,0,'Эро'),(10872,10668,0,'Эссонн'),(10875,10874,0,'Jihochesky'),(10877,10874,0,'Jihomoravsky'),(10879,10874,0,'Karlovarsky'),(10881,10874,0,'Kralovehradecky'),(10883,10874,0,'Liberecky'),(10886,10874,0,'Moravskoslezsky'),(10892,10874,0,'Olomoucky'),(10896,10874,0,'Stredochesky'),(10898,10874,0,'Ustecky'),(10905,10904,0,'Аргау'),(10908,10904,0,'Базель-Штадт'),(10910,10904,0,'Берн'),(10914,10904,0,'Во'),(10916,10904,0,'Женева'),(10918,10904,0,'Золотурн'),(10920,10904,0,'Люцерн'),(10922,10904,0,'Невшатель'),(10925,10904,0,'Тичино'),(10927,10904,0,'Фрибур'),(10930,10904,0,'Цюрих'),(10934,10933,0,'Skine'),(10938,10933,0,'Vastra Gotaland'),(10942,10933,0,'Вермланд'),(10944,10933,0,'Вестерботтен'),(10946,10933,0,'Вестернорланд'),(10948,10933,0,'Вестманланд'),(10950,10933,0,'Евлеборг'),(10952,10933,0,'Емтланд'),(10954,10933,0,'Йёнчёпинг'),(10956,10933,0,'Сёдерманланд'),(10958,10933,0,'Стокгольм'),(10962,10933,0,'Упсала'),(10964,10933,0,'Халланд'),(10966,10933,0,'Эстерйетланд'),(10969,10968,0,'Эстония'),(11003,11002,0,'Воеводина'),(11008,11002,0,'Косово'),(11010,11002,0,'Сербия'),(11015,11014,0,'Cheju'),(11017,11014,0,'Chollabuk'),(11020,11014,0,'Chollanam'),(11024,11014,0,'Chungcheongbuk'),(11027,11014,0,'Chungcheongnam'),(11029,11014,0,'Incheon'),(11031,11014,0,'Kangweon'),(11035,11014,0,'Kwangju'),(11037,11014,0,'Kyeonggi'),(11040,11014,0,'Kyeongsangbuk'),(11045,11014,0,'Kyeongsangnam'),(11050,11014,0,'Pusan'),(11052,11014,0,'Seoul'),(11054,11014,0,'Taegu'),(11056,11014,0,'Taejeon'),(11058,11014,0,'Ulsan'),(11061,11060,0,'Айти'),(11068,11060,0,'Акита'),(11074,11060,0,'Аомори'),(11080,11060,0,'Вакаяма'),(11083,11060,0,'Гифу'),(11088,11060,0,'Гумма'),(11094,11060,0,'Ибараки'),(11102,11060,0,'Ивате'),(11108,11060,0,'Исикава'),(11113,11060,0,'Кагава'),(11116,11060,0,'Кагосима'),(11121,11060,0,'Канагава'),(11133,11060,0,'Киото'),(11139,11060,0,'Коти'),(11142,11060,0,'Кумамото'),(11146,11060,0,'Миэ'),(11151,11060,0,'Мияги'),(11155,11060,0,'Миядзаки'),(11162,11060,0,'Нагано'),(11168,11060,0,'Нагасаки'),(11173,11060,0,'Нара'),(11178,11060,0,'Ниигата'),(11185,11060,0,'Окаяма'),(11189,11060,0,'Окинава'),(11192,11060,0,'Осака'),(11209,11060,0,'Сага'),(11215,11060,0,'Сайтама'),(11227,11060,0,'Сига'),(11230,11060,0,'Сидзуока'),(11233,11060,0,'Симане'),(11248,11060,0,'Тиба'),(11261,11060,0,'Токио'),(11270,11060,0,'Токусима'),(11273,11060,0,'Тотиги'),(11279,11060,0,'Тоттори'),(11283,11060,0,'Тояма'),(11289,11060,0,'Фукуи'),(11295,11060,0,'Фукуока'),(11304,11060,0,'Фукусима'),(11307,11060,0,'Хиросима'),(11312,11060,0,'Хоккайдо'),(11330,11060,0,'Хпого'),(11340,11060,0,'Эхиме'),(11346,11060,0,'Ямагата'),(11354,11060,0,'Ямагути'),(11363,11060,0,'Яманаси'),(277552,277551,0,'Нидерланды'),(277554,277553,0,'Хорватия'),(277556,277555,0,'Румыния'),(277558,277557,0,'Гонконг'),(277560,277559,0,'Индонезия'),(277562,277561,0,'Иордания'),(277564,277563,0,'Малайзия'),(277566,277565,0,'Сингапур'),(277568,277567,0,'Тайвань'),(277570,277569,0,'Туркмения'),(277654,245,0,'Армения'),(277655,1894,0,'Казахстан'),(277656,9908,0,'Украина'),(277657,616,0,'Великобритания'),(277658,11002,0,'Югославия'),(277659,63,0,'Австрия'),(277660,428,0,'Болгария'),(277661,1707,0,'Испания'),(277662,404,0,'Бельгия'),(277663,1012,0,'Германия'),(277664,2897,0,'Польша'),(277665,10933,0,'Швеция'),(277666,10904,0,'Швейцария'),(277667,10648,0,'Финляндия'),(277668,1786,0,'Италия'),(277669,10668,0,'Франция'),(277670,10874,0,'Чехия'),(277671,5673,0,'Словения'),(277672,5681,0,'США'),(277673,1451,0,'Индия'),(277674,1380,0,'Египет'),(1923301,582067,0,'Дамаск'),(1924623,2880,0,'Нордланд'),(1930798,582105,0,'Остров Мэн'),(1934356,1894,0,'Западно-Казахстанская обл.'),(1998532,3159,0,'Адыгея'),(2077237,1366,0,'Хельсингор'),(2284951,3141,0,'Фаро'),(2316497,3159,0,'Хакасия'),(2316780,582051,0,'Дубаи'),(2331485,1366,0,'Копенгаген'),(2412885,582031,0,'Сантьяго'),(2415585,3159,0,'Чукотский АО'),(2416357,1786,0,'о. Сан-Джорджо Маджоре'),(2483267,582077,0,'Гавана'),(2494137,582060,0,'Бейрут'),(2499002,3159,0,'Ханты-Мансийский АО'),(2505921,2505884,0,'Бейрут'),(2564700,1894,0,'Акмолинская обл.'),(2567441,2567393,0,'Тегусигальпа'),(2577870,2880,0,'Ромсдал'),(2578001,2577958,0,'Санто-доминго'),(2687732,2687701,0,'Улан-батор'),(2713529,10648,0,'Kouvola'),(3024114,1012,0,'Саарлэнд'),(3331120,2374,0,'Пекин'),(3409961,1380,0,'Синай'),(3410574,3410238,0,'Багдад'),(3410602,3410238,0,'Басра'),(3410645,3410238,0,'Мосул'),(3661590,3661568,0,'Йоханесбург'),(3663676,582041,0,'Скопье'),(4042477,582065,0,'Морокко'),(4093580,1707,0,'Gerona'),(4119771,582065,0,'Танжер'),(4691804,2374,0,'Гуандон'),(4778004,10933,0,'Норрботтен'),(5019394,3159,0,'Ямало-Ненецкий АО'),(5556213,582046,0,'Перу'),(5640357,10648,0,'Pohjois-Karjala'),(5640862,582090,0,'Tunisia'),(6128546,582050,0,'Тайланд'),(6128641,1258,0,'Корфу'),(6128697,582082,0,'Мозамбик'),(6453632,582040,0,'Korea'),(6460443,582043,0,'Мальта'),(7593366,582044,0,'Пакистан'),(7716133,7716093,0,'Арулько'),(7719907,582064,0,'Пичинча'),(11564803,582029,0,'Багамы'),(11566007,582040,0,'Южная Корея'),(15789405,582106,0,'Ямайка');
/*!40000 ALTER TABLE `region` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tbl_migration`
--

DROP TABLE IF EXISTS `tbl_migration`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tbl_migration` (
  `version` varchar(180) NOT NULL,
  `apply_time` int(11) DEFAULT NULL,
  PRIMARY KEY (`version`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tbl_migration`
--

LOCK TABLES `tbl_migration` WRITE;
/*!40000 ALTER TABLE `tbl_migration` DISABLE KEYS */;
INSERT INTO `tbl_migration` VALUES ('m000000_000000_base',1441992666),('m150913_135853_add_column_attribute_unit',1442152985),('m150913_181051_add_fk_category_set_id',1442168060),('m150924_202359_drop_table_attr_variant',1443126794);
/*!40000 ALTER TABLE `tbl_migration` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `user` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `email` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  `name` varchar(255) DEFAULT NULL,
  `phone` varchar(255) DEFAULT NULL,
  `vk` varchar(255) DEFAULT NULL,
  `skype` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `email` (`email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user`
--

LOCK TABLES `user` WRITE;
/*!40000 ALTER TABLE `user` DISABLE KEYS */;
/*!40000 ALTER TABLE `user` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-10-17 16:49:44
        with: 
CREATE TABLE `country` (
  `country_id` int(11) unsigned NOT NULL auto_increment,
  `city_id` int(11) NOT NULL default 0,
  `name` varchar(128) NOT NULL default '',
  PRIMARY KEY (`country_id`),
  KEY `city_id` (`city_id`)
);

CREATE TABLE `region` (
  `region_id` int(10) unsigned NOT NULL auto_increment,
  `country_id` int(10) unsigned NOT NULL default 0,
  `city_id` int(10) unsigned NOT NULL default 0,
  `name` varchar(64) NOT NULL default '',
  PRIMARY KEY (`region_id`),
  FOREIGN KEY (`country_id`) REFERENCES `country` (`country_id`),
  KEY `city_id` (`city_id`)
);

CREATE TABLE `city` (
  `city_id` int(11) unsigned NOT NULL auto_increment,
  `country_id` int(11) unsigned NOT NULL default 0,
  `region_id` int(10) unsigned NOT NULL default 0,
  `name` varchar(128) NOT NULL default '',
  PRIMARY KEY  (`city_id`),
  FOREIGN KEY (`country_id`) REFERENCES `country` (`country_id`),
  FOREIGN KEY (`region_id`) REFERENCES `region` (`region_id`)
);

CREATE TABLE `eav_set` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `name` varchar(255) NOT NULL COMMENT 'Set name',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

CREATE TABLE `eav_attribute` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `type` tinyint(1) unsigned NOT NULL COMMENT '0 if the attribute can have only one value or 1 if the attribute can have multiple values',
  `data_type` varchar(255) NOT NULL COMMENT 'The attribute data type',
  `name` varchar(255) NOT NULL COMMENT 'The attribute name',
  `label` varchar(255) DEFAULT NULL COMMENT 'The attribute label',
  `data` text COMMENT 'The serialized data',
  PRIMARY KEY (`id`),
  UNIQUE KEY `un_eav_attribute_name` (`name`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

CREATE TABLE `eav_attribute_set` (
  `eav_attribute_id` int(10) unsigned NOT NULL COMMENT 'Composite primary key',
  `eav_set_id` int(10) unsigned NOT NULL COMMENT 'Composite primary key',
  `weight` int(10) NOT NULL COMMENT 'The weight of the attribute',
  PRIMARY KEY (`eav_attribute_id`,`eav_set_id`),
  KEY `no_eav_attribute_set_attribute_id` (`eav_attribute_id`) USING BTREE,
  KEY `no_eav_attribute_set_set_id` (`eav_set_id`) USING BTREE,
  KEY `no_eav_attribute_set_weight` (`weight`) USING BTREE,
  CONSTRAINT `fk_eav_attribute_id_eav_attribute_set` FOREIGN KEY (`eav_attribute_id`) REFERENCES `eav_attribute` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `fk_eav_set_id_eav_attribute_set` FOREIGN KEY (`eav_set_id`) REFERENCES `eav_set` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `eav_attribute_date` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `eav_attribute_id` int(10) unsigned NOT NULL COMMENT 'Foreign key references eav_attribute(id)',
  `entity_id` int(11) NOT NULL COMMENT 'Primary key of an entity',
  `entity` varchar(255) NOT NULL COMMENT 'The entity name',
  `value` datetime NOT NULL COMMENT 'The value of the attribute',
  PRIMARY KEY (`id`),
  KEY `no_eav_attribute_date_entity_entity_id` (`entity`,`entity_id`) USING BTREE,
  KEY `no_eav_attribute_date_eav_attribute_id` (`eav_attribute_id`) USING BTREE,
  KEY `no_eav_attribute_date_value` (`value`) USING BTREE,
  CONSTRAINT `fk_eav_attribute_id_eav_attribute_date` FOREIGN KEY (`eav_attribute_id`) REFERENCES `eav_attribute` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

CREATE TABLE `eav_attribute_varchar` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `eav_attribute_id` int(10) unsigned NOT NULL COMMENT 'Foreign key references eav_attribute(id)',
  `entity_id` int(11) NOT NULL COMMENT 'Primary key of an entity',
  `entity` varchar(255) NOT NULL COMMENT 'The entity name',
  `value` varchar(255) NOT NULL COMMENT 'The value of the attribute',
  PRIMARY KEY (`id`),
  KEY `no_eav_attribute_varchar_entity_entity_id` (`entity`,`entity_id`) USING BTREE,
  KEY `no_eav_attribute_varchar_eav_attribute_id` (`eav_attribute_id`) USING BTREE,
  CONSTRAINT `fk_eav_attribute_id_eav_attribute_varchar` FOREIGN KEY (`eav_attribute_id`) REFERENCES `eav_attribute` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

CREATE TABLE `eav_attribute_int` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `eav_attribute_id` int(10) unsigned NOT NULL COMMENT 'Foreign key references eav_attribute(id)',
  `entity_id` int(11) NOT NULL COMMENT 'Primary key of an entity',
  `entity` varchar(255) NOT NULL COMMENT 'The entity name',
  `value` int(11) NOT NULL COMMENT 'The value of the attribute',
  PRIMARY KEY (`id`),
  KEY `no_eav_attribute_int_entity_entity_id` (`entity`,`entity_id`) USING BTREE,
  KEY `no_eav_attribute_int_eav_attribute_id` (`eav_attribute_id`) USING BTREE,
  KEY `no_eav_attribute_int_value` (`value`) USING BTREE,
  CONSTRAINT `fk_eav_attribute_id_eav_attribute_int` FOREIGN KEY (`eav_attribute_id`) REFERENCES `eav_attribute` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

CREATE TABLE `eav_attribute_text` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `eav_attribute_id` int(10) unsigned NOT NULL COMMENT 'Foreign key references eav_attribute(id)',
  `entity_id` int(11) NOT NULL COMMENT 'Primary key of an entity',
  `entity` varchar(255) NOT NULL COMMENT 'The entity name',
  `value` text COMMENT 'The value of the attribute',
  PRIMARY KEY (`id`),
  KEY `no_eav_attribute_text_entity_entity_id` (`entity`,`entity_id`) USING BTREE,
  KEY `no_eav_attribute_text_eav_attribute_id` (`eav_attribute_id`) USING BTREE,
  CONSTRAINT `fk_eav_attribute_id_eav_attribute_text` FOREIGN KEY (`eav_attribute_id`) REFERENCES `eav_attribute` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

CREATE TABLE `eav_attribute_numeric` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `eav_attribute_id` int(10) unsigned NOT NULL COMMENT 'Foreign key references eav_attribute(id)',
  `entity_id` int(11) NOT NULL COMMENT 'Primary key of an entity',
  `entity` varchar(255) NOT NULL COMMENT 'The entity name',
  `value` double NOT NULL COMMENT 'The value of the attribute',
  PRIMARY KEY (`id`),
  KEY `no_eav_attribute_numeric_entity_entity_id` (`entity`,`entity_id`) USING BTREE,
  KEY `no_eav_attribute_numeric_eav_attribute_id` (`eav_attribute_id`) USING BTREE,
  CONSTRAINT `fk_eav_attribute_id_eav_attribute_numeric` FOREIGN KEY (`eav_attribute_id`) REFERENCES `eav_attribute` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

CREATE TABLE `eav_attribute_money` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Primary surrogate key',
  `eav_attribute_id` int(10) unsigned NOT NULL COMMENT 'Foreign key references eav_attribute(id)',
  `entity_id` int(11) NOT NULL COMMENT 'Primary key of an entity',
  `entity` varchar(255) NOT NULL COMMENT 'The entity name',
  `value` DECIMAL(13,2) NOT NULL COMMENT 'The value of the attribute',
  PRIMARY KEY (`id`),
  KEY `no_eav_attribute_money_entity_entity_id` (`entity`,`entity_id`) USING BTREE,
  KEY `no_eav_attribute_money_eav_attribute_id` (`eav_attribute_id`) USING BTREE,
  CONSTRAINT `fk_eav_attribute_id_eav_attribute_money` FOREIGN KEY (`eav_attribute_id`) REFERENCES `eav_attribute` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;


CREATE TABLE user (
  id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  email VARCHAR(255) NOT NULL,
  password VARCHAR(255) NOT NULL,
  name VARCHAR(255),
  phone VARCHAR(255),
  vk VARCHAR(255),
  skype VARCHAR(255),
  PRIMARY KEY (id),
  UNIQUE (email)
);

CREATE TABLE `category` (
  `id` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `title` VARCHAR(255) NOT NULL,
  `root` INT(10) UNSIGNED DEFAULT NULL,
  `lft` INT(10) UNSIGNED NOT NULL,
  `rgt` INT(10) UNSIGNED NOT NULL,
  `level` SMALLINT(5) UNSIGNED NOT NULL,
  `set_id` INT(10) UNSIGNED,
  PRIMARY KEY (`id`),
  KEY `root` (`root`),
  KEY `lft` (`lft`),
  KEY `rgt` (`rgt`),
  KEY `level` (`level`),
  FOREIGN KEY (`set_id`) REFERENCES `eav_set` (`id`) ON UPDATE CASCADE ON DELETE SET NULL
);

CREATE TABLE ad (
  id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  title VARCHAR(255) NOT NULL,
  description TEXT NOT NULL,
  added TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  author_id INT UNSIGNED NOT NULL,
  city_id INT UNSIGNED NOT NULL,
  category_id INT(10) UNSIGNED NOT NULL,
  visit_counter INT UNSIGNED NOT NULL DEFAULT 0,
  status ENUM('unpublished', 'published', 'trash') NOT NULL DEFAULT 'unpublished',
  importance ENUM('usual', 'top', 'highlighted') NOT NULL DEFAULT 'usual',
  eav_set_id INT UNSIGNED,
  FOREIGN KEY (author_id) REFERENCES user (id) ON UPDATE CASCADE ON DELETE CASCADE,
  FOREIGN KEY (city_id) REFERENCES city (city_id) ON UPDATE CASCADE ON DELETE CASCADE,
  FOREIGN KEY (category_id) REFERENCES category (id) ON UPDATE CASCADE ON DELETE CASCADE,
  FOREIGN KEY (eav_set_id) REFERENCES eav_set (id) ON UPDATE CASCADE ON DELETE CASCADE,
  PRIMARY KEY (id)
);

CREATE TABLE photo (
  id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  name VARCHAR(255) NOT NULL,
  upload_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  ad_id INT UNSIGNED NOT NULL,
  FOREIGN KEY (ad_id) REFERENCES ad (id) ON UPDATE CASCADE ON DELETE CASCADE,
  PRIMARY KEY (id)
);

CREATE TABLE attr_variant (
  id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  attr_id INT(10) UNSIGNED NOT NULL,
  title VARCHAR(255) NOT NULL,
  FOREIGN KEY (attr_id) REFERENCES eav_attribute (id) ON UPDATE CASCADE ON DELETE CASCADE,
  PRIMARY KEY (id)
);
          cmakeBuildDirectory: <?php

class m150913_135853_add_column_attribute_unit extends CDbMigration
{
        public function up()
        {
                $this->addColumn('eav_attribute', 'unit', 'varchar(20)');
        }

        public function down()
        {
                $this->dropColumn('eav_attribute', 'unit');
        }

        /*
        // Use safeUp/safeDown to do migration with transaction
        public function safeUp()
        {
        }

        public function safeDown()
        {
        }
        */
}
          # Ruleset file that will determine what checks will be run
          ruleset: <?php

class m150924_202359_drop_table_attr_variant extends CDbMigration
{
        public function up()
        {
                $this->dropTable('attr_variant');
        }

        public function down()
        {
                $this->createTable('attr_variant', array(
                        'id' => 'pk',
                        'attr_id' => 'INT(10) UNSIGNED NOT NULL',
                        'title' => 'VARCHAR(255) NOT NULL',
                        ));
                $this->addForeignKey(
                        'ix_attr_id', 'attr_variant', 'attr_id',
                        'eav_attribute', 'id', 'CASCADE', 'CASCADE');
        }

        /*
        // Use safeUp/safeDown to do migration with transaction
        public function safeUp()
        {
        }

        public function safeDown()
        {
        }
        */
}

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: 
#include "MoveCommand.h"

static const std::string _name("move");
static const std::string _description =
    "Arguments: {column_index}{row_index}\n"
    "Example: " +
    _name + " e4\n"
            "Description: Moves the selected piece to the specified square if legal.";

MoveCommand::MoveCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result MoveCommand::apply(const std::vector<std::string> &params)
{
    if (params.size() != 1 || params[0].length() != 2)
        return {true, false};

    int row = params[0][1] - '1';
    int column = params[0][0] - 'a';

    Chess &chess = this->get_chess();
    bool error = chess.move({row, column});
    return {error, false};
}
        uses: #pragma once

#include "../Command.h"

// Moves the selected piece to a new square.
// Params:
//      1. string of format {char}{int} representing a
//          field of the chessboard.
class MoveCommand : public Command
{
public:
    MoveCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};
        with: 
#include "QuitCommand.h"

static const std::string _name = "quit";
static const std::string _description =
    "Arguments: [None]\n"
    "Description: Quits the game.";

QuitCommand::QuitCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result QuitCommand::apply(const std::vector<std::string> &params)
{
    bool error = !params.empty();
    bool quit = true;
    return {error, quit};
}
          sarif_file: #pragma once

#include "../Command.h"

// Sends a quit signal to the session.
// No params.
class QuitCommand : public Command
{
public:
    QuitCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};

      # Upload SARIF file as an Artifact to download and view
      # - name: 
#include "SelectCommand.h"

static const std::string _name("select");
static const std::string _description =
    "Arguments: {column_index}{row_index}\n"
    "Example: " +
    _name + " e2\n"
            "Description: Selects the specified square.";

SelectCommand::SelectCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result SelectCommand::apply(const std::vector<std::string> &params)
{
    if (params.size() != 1 || params[0].length() != 2)
        return {true, false};

    int row = params[0][1] - '1';
    int column = params[0][0] - 'a';

    Chess &chess = this->get_chess();
    bool error = chess.select({row, column});
    return {error, false};
}
      #   uses: #pragma once

#include "../Command.h"

// Selects a square.
// Params:
//     1. row
//     2. column
class SelectCommand : public Command
{
public:
    SelectCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};
      #   with: 
#include "Command.h"

Command::Command(Chess &chess, std::string name, std::string description)
    : chess(chess), name(name), description(description) {}

const std::string &Command::get_name() const
{
    return this->name;
}

const std::string &Command::get_description() const
{
    return this->description;
}

Chess &Command::get_chess()
{
    return this->chess;
}
      #     name: #pragma once

#include <string>
#include <vector>

#include "Result.h"

#include "../model/Chess.h"

class Command
{
public:
    Command(Chess &chess, std::string name, std::string description);
    virtual ~Command() = default;

    const std::string &get_name() const;
    const std::string &get_description() const;

    virtual Result apply(const std::vector<std::string> &params) = 0;

protected:
    Chess &get_chess();

private:
    Chess &chess;
    std::string name;
    std::string description;
};
      #     path: #pragma once

struct Result
{
    bool error;
    bool quit;
};
