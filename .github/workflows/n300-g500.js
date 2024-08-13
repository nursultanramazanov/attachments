name: NodeJS with Gulp

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

jobs: # GDB/Python functions for dealing with NuttX

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
  build: #
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
    runs-on: # copy the file to .gdbinit in your Firmware tree, and adjust the path
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

    strategy: define f4_memdump
        shell mkdir -p /tmp/dump
        printf "Dumping CCSRAM to /tmp/dump/ccsram\n"
        dump memory /tmp/dump/ccsram 0x10000000 0x10010000
        printf "Dumping SRAM to /tmp/dump/sram\n"
        dump memory /tmp/dump/sram 0x20000000 0x20020000
end

document f4_memdump
Dumps the STM32F4 memory to files in /tmp/dump.
end
      matrix: # program a bootable device load on a mavstation
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

        node-version: [ target remote :3333

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
continue ]

    steps: #!/bin/bash
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
    - uses: # The latest defaults in OpenOCD 0.7.0 are actually prettymuch correct for the px4fmu

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


    - name: #!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

openocd -f interface/olimex-arm-usb-ocd-h.cfg -f $DIR/px4fmu-v1-board.cfg
      uses: #!/usr/bin/env python

################################################################################################
# @File MissionCheck.py
# Automated mission loading, execution and monitoring
# for Continuous Integration
#
# @author Sander Smeets <sander@droneslab.com>
#
# Code partly based on DroneKit (c) Copyright 2015-2016, 3D Robotics.
################################################################################################


################################################################################################
# Settings
################################################################################################

connection_string       = '127.0.0.1:14540'

import_mission_filename = 'VTOLmission.txt'
max_execution_time      = 250
alt_acceptance_radius   = 10

################################################################################################
# Init
################################################################################################

# Import DroneKit-Python
from dronekit import connect, Command
from pymavlink import mavutil
import time, sys, argparse

parser = argparse.ArgumentParser()
parser.add_argument("-c", "--connect", help="connection string")
parser.add_argument("-f", "--filename", help="mission filename")
parser.add_argument("-t", "--timeout", help="execution timeout", type=float)
parser.add_argument("-a", "--altrad", help="altitude acceptance radius", type=float)
args = parser.parse_args()

if args.connect:
    connection_string = args.connect
if args.filename:
    import_mission_filename = args.filename
if args.timeout:
    max_execution_time = args.timeout
if args.altrad:
    alt_acceptance_radius = args.altrad



mission_failed = False
MAV_MODE_AUTO = 4

# start time counter
start_time = time.time()
elapsed_time = time.time() - start_time



# Connect to the Vehicle
print "Connecting"
vehicle = connect(connection_string, wait_ready=True)

while not vehicle.system_status.state == "STANDBY" or vehicle.gps_0.fix_type < 3:
    if time.time() - start_time > 20:
        print "FAILED: SITL did not reach standby with GPS fix within 20 seconds"
        sys.exit(98)
    print "Waiting for vehicle to initialise... %s " % vehicle.system_status.state
    time.sleep(1)

# Display basic vehicle state
print " Type: %s" % vehicle._vehicle_type
print " Armed?: %s" % vehicle.armed
print " System status: %s" % vehicle.system_status.state
print " GPS: %s" % vehicle.gps_0
print " Alt: %s" % vehicle.location.global_relative_frame.alt


################################################################################################
# Functions
################################################################################################


def readmission(aFileName):
    """
    Load a mission from a file into a list. The mission definition is in the Waypoint file
    format (http://qgroundcontrol.org/mavlink/waypoint_protocol#waypoint_file_format).
    This function is used by upload_mission().
    """
    cmds = vehicle.commands
    missionlist=[]
    with open(aFileName) as f:
        for i, line in enumerate(f):
            if i==0:
                if not line.startswith('QGC WPL 110'):
                    raise Exception('File is not supported WP version')
            else:
                linearray=line.split('\t')
                ln_index=int(linearray[0])
                ln_currentwp=int(linearray[1])
                ln_frame=int(linearray[2])
                ln_command=int(linearray[3])
                ln_param1=float(linearray[4])
                ln_param2=float(linearray[5])
                ln_param3=float(linearray[6])
                ln_param4=float(linearray[7])
                ln_param5=float(linearray[8])
                ln_param6=float(linearray[9])
                ln_param7=float(linearray[10])
                ln_autocontinue=int(linearray[11].strip())
                cmd = Command( 0, 0, 0, ln_frame, ln_command, ln_currentwp, ln_autocontinue, ln_param1, ln_param2, ln_param3, ln_param4, ln_param5, ln_param6, ln_param7)
                missionlist.append(cmd)
    return missionlist


def upload_mission(aFileName):
    """
    Upload a mission from a file.
    """
    #Read mission from file
    missionlist = readmission(aFileName)

    #Clear existing mission from vehicle
    cmds = vehicle.commands
    cmds.clear()
    #Add new mission to vehicle
    for command in missionlist:
        cmds.add(command)
    print ' Uploaded mission with %s items' % len(missionlist)
    vehicle.commands.upload()
    return missionlist


def download_mission():
    """
    Downloads the current mission and returns it in a list.
    It is used in save_mission() to get the file information to save.
    """
    print " Download mission from vehicle"
    missionlist=[]
    cmds = vehicle.commands
    cmds.download()
    cmds.wait_ready()
    for cmd in cmds:
        missionlist.append(cmd)
    return missionlist

def save_mission(aFileName):
    """
    Save a mission in the Waypoint file format
    (http://qgroundcontrol.org/mavlink/waypoint_protocol#waypoint_file_format).
    """
    print "\nSave mission from Vehicle to file: %s" % export_mission_filename
    #Download mission from vehicle
    missionlist = download_mission()
    #Add file-format information
    output='QGC WPL 110\n'
    #Add home location as 0th waypoint
    home = vehicle.home_location
    output+="%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n" % (0,1,0,16,0,0,0,0,home.lat,home.lon,home.alt,1)
    #Add commands
    for cmd in missionlist:
        commandline="%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n" % (cmd.seq,cmd.current,cmd.frame,cmd.command,cmd.param1,cmd.param2,cmd.param3,cmd.param4,cmd.x,cmd.y,cmd.z,cmd.autocontinue)
        output+=commandline
    with open(aFileName, 'w') as file_:
        print " Write mission to file"
        file_.write(output)


def printfile(aFileName):
    """
    Print a mission file to demonstrate "round trip"
    """
    print "\nMission file: %s" % aFileName
    with open(aFileName) as f:
        for line in f:
            print ' %s' % line.strip()


################################################################################################
# Listeners
################################################################################################

current_sequence = -1
current_sequence_changed = False
current_landed_state = -1
home_position_set = False

#Create a message listener for mission sequence number
@vehicle.on_message('MISSION_CURRENT')
def listener(self, name, mission_current):
    global current_sequence, current_sequence_changed
    if (current_sequence <> mission_current.seq):
        current_sequence = mission_current.seq;
        current_sequence_changed = True
        print 'current mission sequence: %s' % mission_current.seq

#Create a message listener for mission sequence number
@vehicle.on_message('EXTENDED_SYS_STATE')
def listener(self, name, extended_sys_state):
    global current_landed_state
    if (current_landed_state <> extended_sys_state.landed_state):
        current_landed_state = extended_sys_state.landed_state;

#Create a message listener for home position fix
@vehicle.on_message('HOME_POSITION')
def listener(self, name, home_position):
    global home_position_set
    home_position_set = True



################################################################################################
# Start mission test
################################################################################################


while not home_position_set:
    if time.time() - start_time > 30:
        print "FAILED: getting home position 30 seconds"
        sys.exit(98)
    print "Waiting for home position..."
    time.sleep(1)


#Upload mission from file
missionlist = upload_mission(import_mission_filename)
time.sleep(2)

# set mission mode the hard way
vehicle._master.mav.command_long_send(vehicle._master.target_system, vehicle._master.target_component,
                                           mavutil.mavlink.MAV_CMD_DO_SET_MODE, 0,
                                           MAV_MODE_AUTO,
                                           0, 0, 0, 0, 0, 0)
time.sleep(1)


# Arm vehicle
vehicle.armed = True

while not vehicle.system_status.state == "ACTIVE":
    if time.time() - start_time > 30:
        print "FAILED: vehicle did not arm within 30 seconds"
        sys.exit(98)
    print "Waiting for vehicle to arm..."
    time.sleep(1)



# Wait for completion of mission items
while (current_sequence < len(missionlist)-1 and elapsed_time < max_execution_time):
    time.sleep(.2)
    if current_sequence > 0 and current_sequence_changed:

        if missionlist[current_sequence-1].z - alt_acceptance_radius > vehicle.location.global_relative_frame.alt or missionlist[current_sequence-1].z + alt_acceptance_radius < vehicle.location.global_relative_frame.alt:
            print "waypoint %s out of bounds altitude %s gps altitude: %s" % (current_sequence, missionlist[current_sequence-1].z, vehicle.location.global_relative_frame.alt)
            mission_failed = True
        current_sequence_changed = False
    elapsed_time = time.time() - start_time

if elapsed_time < max_execution_time:
    print "Mission items have been executed"

# wait for the vehicle to have landed
while (current_landed_state != 1 and elapsed_time < max_execution_time):
    time.sleep(1)
    elapsed_time = time.time() - start_time

if elapsed_time < max_execution_time:
    print "Vehicle has landed"

# Disarm vehicle
vehicle.armed = False

# count elapsed time
elapsed_time = time.time() - start_time

# Close vehicle object before exiting script
vehicle.close()
time.sleep(2)

# Validate time constraint
if elapsed_time <= max_execution_time and not mission_failed:
    print "Mission succesful time elapsed %s" % elapsed_time
    sys.exit(0)

if elapsed_time > max_execution_time:
    print "Mission FAILED to execute within %s seconds" % max_execution_time
    sys.exit(99)

if mission_failed:
    print "Mission FAILED out of bounds"
    sys.exit(100)

print "Mission FAILED something strange happened"
sys.exit(101)
      with: QGC WPL 110
0        1        0        16        0        0        0        0        47.3979949951        8.54559612274        25.0        1
1        0        2        3000        4.0        0.0        0.0        0.0        47.3980331421        8.54578971863        25.0        1
2        0        3        16        0.0        0.0        -0.0        0.0        47.399269104        8.54557228088        25.0        1
3        0        3        16        0.0        0.0        -0.0        0.0        47.3992652893        8.54230213165        25.0        1
4        0        3        16        0.0        0.0        -0.0        0.0        47.3974761963        8.54239082336        25.0        1
5        0        3        16        0.0        0.0        -0.0        0.0        47.3976669312        8.54509449005        25.0        1
6        0        2        3000        3.0        0.0        0.0        0.0        47.3977851868        8.54526233673        25.0        1
7        0        3        21        25.0        0.0        -0.0        0.0        47.3979797363        8.54460906982        25.0        1
        node-version: % All rights reserved.
%
% Redistribution and use in source and binary forms, with or without
% modification, are permitted provided that the following conditions are
% met:
%
%     * Redistributions of source code must retain the above copyright
%       notice, this list of conditions and the following disclaimer.
%     * Redistributions in binary form must reproduce the above copyright
%       notice, this list of conditions and the following disclaimer in
%       the documentation and/or other materials provided with the distribution
%
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
% AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
% IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
% ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
% LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
% CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
% SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
% INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
% CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
% ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
% POSSIBILITY OF SUCH DAMAGE.
%

function [ center, radii, evecs, v ] = ellipsoid_fit( X, flag, equals )
%
% Fit an ellispoid/sphere to a set of xyz data points:
%
%   [center, radii, evecs, pars ] = ellipsoid_fit( X )
%   [center, radii, evecs, pars ] = ellipsoid_fit( [x y z] );
%   [center, radii, evecs, pars ] = ellipsoid_fit( X, 1 );
%   [center, radii, evecs, pars ] = ellipsoid_fit( X, 2, 'xz' );
%   [center, radii, evecs, pars ] = ellipsoid_fit( X, 3 );
%
% Parameters:
% * X, [x y z]   - Cartesian data, n x 3 matrix or three n x 1 vectors
% * flag         - 0 fits an arbitrary ellipsoid (default),
%                - 1 fits an ellipsoid with its axes along [x y z] axes
%                - 2 followed by, say, 'xy' fits as 1 but also x_rad = y_rad
%                - 3 fits a sphere
%
% Output:
% * center    -  ellispoid center coordinates [xc; yc; zc]
% * ax        -  ellipsoid radii [a; b; c]
% * evecs     -  ellipsoid radii directions as columns of the 3x3 matrix
% * v         -  the 9 parameters describing the ellipsoid algebraically: 
%                Ax^2 + By^2 + Cz^2 + 2Dxy + 2Exz + 2Fyz + 2Gx + 2Hy + 2Iz = 1
%
% Author:
% Yury Petrov, Northeastern University, Boston, MA
%

error( nargchk( 1, 3, nargin ) );  % check input arguments
if nargin == 1
    flag = 0;  % default to a free ellipsoid
end
if flag == 2 && nargin == 2
    equals = 'xy';
end

if size( X, 2 ) ~= 3
    error( 'Input data must have three columns!' );
else
    x = X( :, 1 );
    y = X( :, 2 );
    z = X( :, 3 );
end

% need nine or more data points
if length( x ) < 9 && flag == 0 
   error( 'Must have at least 9 points to fit a unique ellipsoid' );
end
if length( x ) < 6 && flag == 1
   error( 'Must have at least 6 points to fit a unique oriented ellipsoid' );
end
if length( x ) < 5 && flag == 2
   error( 'Must have at least 5 points to fit a unique oriented ellipsoid with two axes equal' );
end
if length( x ) < 3 && flag == 3
   error( 'Must have at least 4 points to fit a unique sphere' );
end

if flag == 0
    % fit ellipsoid in the form Ax^2 + By^2 + Cz^2 + 2Dxy + 2Exz + 2Fyz + 2Gx + 2Hy + 2Iz = 1
    D = [ x .* x, ...
          y .* y, ...
          z .* z, ...
      2 * x .* y, ...
      2 * x .* z, ...
      2 * y .* z, ...
      2 * x, ...
      2 * y, ... 
      2 * z ];  % ndatapoints x 9 ellipsoid parameters
elseif flag == 1
    % fit ellipsoid in the form Ax^2 + By^2 + Cz^2 + 2Gx + 2Hy + 2Iz = 1
    D = [ x .* x, ...
          y .* y, ...
          z .* z, ...
      2 * x, ...
      2 * y, ... 
      2 * z ];  % ndatapoints x 6 ellipsoid parameters
elseif flag == 2
    % fit ellipsoid in the form Ax^2 + By^2 + Cz^2 + 2Gx + 2Hy + 2Iz = 1,
    % where A = B or B = C or A = C
    if strcmp( equals, 'yz' ) || strcmp( equals, 'zy' )
        D = [ y .* y + z .* z, ...
            x .* x, ...
            2 * x, ...
            2 * y, ...
            2 * z ];
    elseif strcmp( equals, 'xz' ) || strcmp( equals, 'zx' )
        D = [ x .* x + z .* z, ...
            y .* y, ...
            2 * x, ...
            2 * y, ...
            2 * z ];
    else
        D = [ x .* x + y .* y, ...
            z .* z, ...
            2 * x, ...
            2 * y, ...
            2 * z ];
    end
else
    % fit sphere in the form A(x^2 + y^2 + z^2) + 2Gx + 2Hy + 2Iz = 1
    D = [ x .* x + y .* y + z .* z, ...
      2 * x, ...
      2 * y, ... 
      2 * z ];  % ndatapoints x 4 sphere parameters
end

% solve the normal system of equations
v = ( D' * D ) \ ( D' * ones( size( x, 1 ), 1 ) );

% find the ellipsoid parameters
if flag == 0
    % form the algebraic form of the ellipsoid
    A = [ v(1) v(4) v(5) v(7); ...
          v(4) v(2) v(6) v(8); ...
          v(5) v(6) v(3) v(9); ...
          v(7) v(8) v(9) -1 ];
    % find the center of the ellipsoid
    center = -A( 1:3, 1:3 ) \ [ v(7); v(8); v(9) ];
    % form the corresponding translation matrix
    T = eye( 4 );
    T( 4, 1:3 ) = center';
    % translate to the center
    R = T * A * T';
    % solve the eigenproblem
    [ evecs evals ] = eig( R( 1:3, 1:3 ) / -R( 4, 4 ) );
    radii = sqrt( 1 ./ diag( evals ) );
else
    if flag == 1
        v = [ v(1) v(2) v(3) 0 0 0 v(4) v(5) v(6) ];
    elseif flag == 2
        if strcmp( equals, 'xz' ) || strcmp( equals, 'zx' )
            v = [ v(1) v(2) v(1) 0 0 0 v(3) v(4) v(5) ];
        elseif strcmp( equals, 'yz' ) || strcmp( equals, 'zy' )
            v = [ v(2) v(1) v(1) 0 0 0 v(3) v(4) v(5) ];
        else % xy
            v = [ v(1) v(1) v(2) 0 0 0 v(3) v(4) v(5) ];
        end
    else
        v = [ v(1) v(1) v(1) 0 0 0 v(2) v(3) v(4) ];
    end
    center = ( -v( 7:9 ) ./ v( 1:3 ) )';
    gam = 1 + ( v(7)^2 / v(1) + v(8)^2 / v(2) + v(9)^2 / v(3) );
    radii = ( sqrt( gam ./ v( 1:3 ) ) )';
    evecs = eye( 3 );
end



    - name: clear all;
close all;

% Measurement data
% 1045 propeller
% Robbe Roxxy Motor (1100 kV, data collected in 2010)
data = [ 45, 7.4;...
         38, 5.6;...
         33, 4.3;...
         26, 3.0;...
         18, 2.0;...
         10, 1.0 ];

% Normalize the data, as we're operating later
% anyways in normalized units
data(:,1) = data(:,1) ./ max(data(:,1));
data(:,2) = data(:,2) ./ max(data(:,2));

% Fit a 2nd degree polygon to the data and
% print the x2, x1, x0 coefficients
p = polyfit(data(:,2), data(:,1),2)

% Override the first coffefficient for testing
% purposes
pf = 0.62;

% Generate plotting data
px1 = linspace(0, max(data(:,2)));
py1 = polyval(p, px1);

pyt = zeros(size(data, 1), 1);
corr = zeros(size(data, 1), 1);

% Actual code test
% the two lines below are the ones needed to be ported to C:
%   pf: Power factor parameter.
%   px1(i): The current normalized motor command (-1..1)
%   corr(i): The required correction. The motor speed is:
%            px1(i) 
for i=1:size(px1, 2)

    % The actual output throttle
    pyt(i) = -pf * (px1(i) * px1(i)) + (1 + pf) * px1(i);

    % Solve for input throttle
    % y = -p * x^2 + (1+p) * x;
    % 
end

plot(data(:,2), data(:,1), '*r');
hold on;
plot(px1, py1, '*b');
hold on;
plot([0 px1(end)], [0 py1(end)], '-k');
hold on;
plot(px1, pyt, '-b');
hold on;
plot(px1, corr, '-m');
      run: %
% Tool for plotting mag data
%
% Reference values:
% telem> [cal] mag #0 off: x:0.15    y:0.07     z:0.14 Ga
%                  MATLAB: x:0.1581  y: 0.0701  z: 0.1439 Ga
% telem> [cal] mag #0 scale: x:1.10 y:0.97 z:1.02
%                  MATLAB: 0.5499, 0.5190, 0.4907
%
% telem> [cal] mag #1 off: x:-0.18    y:0.11    z:-0.09 Ga
%                  MATLAB: x:-0.1827  y:0.1147  z:-0.0848 Ga
% telem> [cal] mag #1 scale: x:1.00 y:1.00 z:1.00
%                  MATLAB: 0.5122, 0.5065, 0.4915
%
%
% User-guided values:
%
% telem> [cal] mag #0 off: x:0.12 y:0.09 z:0.14 Ga
% telem> [cal] mag #0 scale: x:0.88 y:0.99 z:0.95
% telem> [cal] mag #1 off: x:-0.18 y:0.11 z:-0.09 Ga
% telem> [cal] mag #1 scale: x:1.00 y:1.00 z:1.00

close all;
clear all;

plot_scale = 0.8;

xmax = plot_scale;
xmin = -xmax;
ymax = plot_scale;
ymin = -ymax;
zmax = plot_scale;
zmin = -zmax;

mag0_raw = load('../../mag0_raw3.csv');
mag1_raw = load('../../mag1_raw3.csv');

mag0_cal = load('../../mag0_cal3.csv');
mag1_cal = load('../../mag1_cal3.csv');

fm0r = figure();

mag0_x_scale = 0.88;
mag0_y_scale = 0.99;
mag0_z_scale = 0.95;

plot3(mag0_raw(:,1), mag0_raw(:,2), mag0_raw(:,3), '*r');
[mag0_raw_center, mag0_raw_radii, evecs, pars ] = ellipsoid_fit( [mag0_raw(:,1) mag0_raw(:,2) mag0_raw(:,3)] );
mag0_raw_center
mag0_raw_radii
axis([xmin xmax ymin ymax zmin zmax])
viscircles([mag0_raw_center(1), mag0_raw_center(2)], [mag0_raw_radii(1)]);

fm1r = figure();
plot3(mag1_raw(:,1), mag1_raw(:,2), mag1_raw(:,3), '*r');
[center, radii, evecs, pars ] = ellipsoid_fit( [mag1_raw(:,1) mag1_raw(:,2) mag1_raw(:,3)] );
center
radii
axis([xmin xmax ymin ymax zmin zmax])

fm0c = figure();
plot3(mag0_cal(:,1) .* mag0_x_scale, mag0_cal(:,2) .* mag0_y_scale, mag0_cal(:,3) .* mag0_z_scale, '*b');
[mag0_cal_center, mag0_cal_radii, evecs, pars ] = ellipsoid_fit( [mag1_raw(:,1) .* mag0_x_scale mag1_raw(:,2) .* mag0_y_scale mag1_raw(:,3) .* mag0_z_scale] );
mag0_cal_center
mag0_cal_radii
axis([xmin xmax ymin ymax zmin zmax])
viscircles([0, 0], [mag0_cal_radii(3)]);

fm1c = figure();
plot3(mag1_cal(:,1), mag1_cal(:,2), mag1_cal(:,3), '*b');
axis([xmin xmax ymin ymax zmin zmax])
[center, radii, evecs, pars ] = ellipsoid_fit( [mag1_raw(:,1) mag1_raw(:,2) mag1_raw(:,3)] );
viscircles([0, 0], [radii(3)]);

mag0_x_scale_matlab = 1 / (mag0_cal_radii(1) / mag0_raw_radii(1))
mag0_y_scale_matlab = 1 / (mag0_cal_radii(2) / mag0_raw_radii(2))
mag0_z_scale_matlab = 1 / (mag0_cal_radii(3) / mag0_raw_radii(3))
        
