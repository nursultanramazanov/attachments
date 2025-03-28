name: Rust

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        docker create -v /${#
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
end } --name ${#
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
end } --privileged tizenrt/tizenrt:${#
# Setup macros for the BlackMagic debug probe and NuttX.
#

mon swdp_scan
attach 1 } /bin/bash
        docker cp ./. ${# GDB/Python functions for dealing with NuttX

from __future__ import print_function
import gdb, gdb.types

parse_int = lambda x: int(str(x), 0)

class NX_register_set(object):
	"""Copy of the registers for a given context"""

	v7_regmap = {
		'R13':		0,
		'SP':		0,
		'PRIORITY':	1,
		'R4':		2,
		'R5':		3,
		'R6':		4,
		'R7':		5,
		'R8':		6,
		'R9':		7,
		'R10':		8,
		'R11':		9,
		'EXC_RETURN':	10,
		'R0':		11,
		'R1':		12,
		'R2':		13,
		'R3':		14,
		'R12':		15,
		'R14':		16,
		'LR':		16,
		'R15':		17,
		'PC':		17,
		'XPSR':		18,
	}

	v7em_regmap = {
		'R13':		0,
		'SP':		0,
		'PRIORITY':	1,
		'R4':		2,
		'R5':		3,
		'R6':		4,
		'R7':		5,
		'R8':		6,
		'R9':		7,
		'R10':		8,
		'R11':		9,
		'EXC_RETURN':	10,
		'R0':		27,
		'R1':		28,
		'R2':		29,
		'R3':		30,
		'R12':		31,
		'R14':		32,
		'LR':		32,
		'R15':		33,
		'PC':		33,
		'XPSR':		34,
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
			registers	 = self.registers
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
			registers	 = self.registers
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
		return 	parse_int(a[regmap['SP']])
	
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
		return 	hex(int(a[regmap['PC']]))
	
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
		
NX_my_bt() }:/${#
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
end }
        docker run -d --rm -it --name ${# copy the file to .gdbinit in your Firmware tree, and adjust the path
# below to match your system
# For example:
# target extended /dev/serial/by-id/usb-Black_Sphere_Technologies_Black_Magic_Probe_DDE5A1C4-if00
# target extended /dev/ttyACM4


monitor swdp_scan
attach 1
monitor vector_catch disable hard
set mem inaccessible-by-default off
set print pretty
source Debug/PX4 } --volumes-from=${define f4_memdump
	shell mkdir -p /tmp/dump
	printf "Dumping CCSRAM to /tmp/dump/ccsram\n"
	dump memory /tmp/dump/ccsram 0x10000000 0x10010000
	printf "Dumping SRAM to /tmp/dump/sram\n"
	dump memory /tmp/dump/sram 0x20000000 0x20020000
end

document f4_memdump
Dumps the STM32F4 memory to files in /tmp/dump.
end } -w /${# program a bootable device load on a mavstation
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
 }/os --privileged tizenrt/tizenrt:${target remote :3333

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
continue } /bin/bash

  arm-version: &arm-version
      run: 
      name: ARM GCC Version
      command: |
        docker exec ${#!/bin/bash
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

$opener $graphfile } arm-none-eabi-gcc --version

  build-job: &build-job
    run:
      name: TizenRT Build Test
      command: |
        docker exec -it ${# The latest defaults in OpenOCD 0.7.0 are actually prettymuch correct for the px4fmu

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
} bash -c "cd tools; ./configure.sh ${#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

openocd -f interface/olimex-arm-usb-ocd-h.cfg -f $DIR/px4fmu-v1-board.cfg }"
                docker exec -it ${BUILDER} bash -c "make"


jobs:
  checkout_code:
    machine: 
      image: default
    working_directory: ~/TizenRT
    steps:
      - checkout
      - persist_to_workspace:
          root: ~/TizenRT
          paths:
            - ./
            
  artik055s/audio:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  artik053/tc:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  qemu/build_test:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  esp_wrover_kit/hello_with_tash:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  imxrt1020-evk/loadable_elf_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm/hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8720e/hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8720e/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/flat_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/flat_dev_ddr:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

workflows:
  version: 2
  build-tizenrt:
    jobs:
      - checkout_code
      - artik055s/audio:
          requires:
            - checkout_code
      - artik053/tc:
          requires:
            - checkout_code
      - qemu/build_test:
          requires:
            - checkout_code
      - esp_wrover_kit/hello_with_tash:
          requires:
            - checkout_code
      - imxrt1020-evk/loadable_elf_apps:
          requires:
            - checkout_code
      - rtl8721csm/hello:
          requires:
            - checkout_code
      - rtl8721csm/loadable_apps:
          requires:
            - checkout_code
      - rtl8720e/hello:
          requires:
            - checkout_code
      - rtl8720e/loadable_apps:
          requires:
            - checkout_code
      - rtl8730e/flat_apps:
          requires:
            - checkout_code
      - rtl8730e/loadable_apps:
          requires:
            - checkout_code
      - rtl8730e/flat_dev_ddr:
          requires:
            - checkout_code  
              push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]

env:
  CARGO_TERM_COLOR: always

jobs:
  build:

    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4
    - name: Build
      run: cargo build --verbose
    - name: Run tests
      run: cargo test --verbose


			
