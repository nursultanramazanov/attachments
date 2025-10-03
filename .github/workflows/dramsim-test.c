# This starter workflow is for a CMake project running on multiple platforms. There is a different starter workflow if you just want a single platform.
# See: https://github.com/actions/starter-workflows/blob/main/ci/cmake-single-platform.yml
name: CMake on multiple platforms

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        docker create -v /
#tell the linker the rpath so that we don't have to muck with LD_LIBRARY_PATH, etc
dramsim_test: dramsim_test.cpp
	$(CXX) -g -o dramsim_test dramsim_test.cpp -I../ -L../ -ldramsim -Wl,-rpath=../

clean: 
	rm dramsim_test --name /*********************************************************************************
*  Copyright (c) 2010-2011, Elliott Cooper-Balis
*                             Paul Rosenfeld
*                             Bruce Jacob
*                             University of Maryland 
*                             dramninjas [at] gmail [dot] com
*  All rights reserved.
*  
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*  
*     * Redistributions of source code must retain the above copyright notice,
*        this list of conditions and the following disclaimer.
*  
*     * Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
*  
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
*  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************************************************/




#include <stdio.h>
#include "dramsim_test.h"

using namespace DRAMSim;

/* callback functors */
void some_object::read_complete(unsigned id, uint64_t address, uint64_t clock_cycle)
{
	printf("[Callback] read complete: %d 0x%lx cycle=%lu\n", id, address, clock_cycle);
}

void some_object::write_complete(unsigned id, uint64_t address, uint64_t clock_cycle)
{
	printf("[Callback] write complete: %d 0x%lx cycle=%lu\n", id, address, clock_cycle);
}

/* FIXME: this may be broken, currently */
void power_callback(double a, double b, double c, double d)
{
//	printf("power callback: %0.3f, %0.3f, %0.3f, %0.3f\n",a,b,c,d);
}

int some_object::add_one_and_run(MultiChannelMemorySystem *mem, uint64_t addr)
{

	/* create a transaction and add it */
	bool isWrite = false; 
	mem->addTransaction(isWrite, addr);

	// send a read to channel 1 on the same cycle 
	addr = 1LL<<33 | addr; 
	mem->addTransaction(isWrite, addr);

	for (int i=0; i<5; i++)
	{
		mem->update();
	}

	/* add another some time in the future */

	// send a write to channel 0 
	addr = 0x900012; 
	isWrite = true; 
	mem->addTransaction(isWrite, addr);
	

	/* do a bunch of updates (i.e. clocks) -- at some point the callback will fire */
	for (int i=0; i<45; i++)
	{
		mem->update();
	}

	/* get a nice summary of this epoch */
	mem->printStats(true);

	return 0;
}

int main()
{
	some_object obj;
	TransactionCompleteCB *read_cb = new Callback<some_object, void, unsigned, uint64_t, uint64_t>(&obj, &some_object::read_complete);
	TransactionCompleteCB *write_cb = new Callback<some_object, void, unsigned, uint64_t, uint64_t>(&obj, &some_object::write_complete);

	/* pick a DRAM part to simulate */
	MultiChannelMemorySystem *mem = getMemorySystemInstance("ini/DDR2_micron_16M_8b_x8_sg3E.ini", "system.ini", "..", "example_app", 16384); 


	mem->RegisterCallbacks(read_cb, write_cb, power_callback);
	MultiChannelMemorySystem *mem2 = getMemorySystemInstance("ini/DDR2_micron_16M_8b_x8_sg3E.ini", "system.ini", "..", "example_app", 16384); 

	mem2->RegisterCallbacks(read_cb, write_cb, power_callback);

	printf("dramsim_test main()\n");
	printf("-----MEM1------\n");
	obj.add_one_and_run(mem, 0x100001UL);
	obj.add_one_and_run(mem, 0x200002UL);

	printf("-----MEM2------\n");
	obj.add_one_and_run(mem2, 0x300002UL);
	return 0; 
}
 --privileged tizenrt/tizenrt:/*********************************************************************************
*  Copyright (c) 2010-2011, Elliott Cooper-Balis
*                             Paul Rosenfeld
*                             Bruce Jacob
*                             University of Maryland 
*                             dramninjas [at] gmail [dot] com
*  All rights reserved.
*  
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*  
*     * Redistributions of source code must retain the above copyright notice,
*        this list of conditions and the following disclaimer.
*  
*     * Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
*  
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
*  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************************************************/




#include <string>
#include <stdint.h>
#include <DRAMSim.h>

class some_object
{
	public: 
		void read_complete(unsigned, uint64_t, uint64_t);
		void write_complete(unsigned, uint64_t, uint64_t);
		int add_one_and_run(DRAMSim::MultiChannelMemorySystem *mem, uint64_t addr);
}; /bin/bash
        docker cp ./. ; blah blah
NUM_BANKS=8
NUM_ROWS=16384 
NUM_COLS=1024
DEVICE_WIDTH=8

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=3.0 ;

CL=4 ;
AL=0 ;
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=4 ;
tRAS=14; 
tRCD=4 ;
tRRD=3 ;
tRC=18 ;
tRP=4  ;
tCCD=2 ;
tRTP=3 ;
tWTR=3 ;
tWR=5  ;
tRTRS=1;
tRFC=43;
tFAW=13;
tCKE=3 ;
tXP=2  ;

tCMD=1 ;

IDD0=85
IDD1=100
IDD2P=7;
IDD2Q=40;
IDD2N=40;
IDD3Pf=30;
IDD3Ps=10;
IDD3N=55;
IDD4W=135;
IDD4R=135;
IDD5=215;
IDD6=7;
IDD6L=5;
IDD7=280;
Vdd=1.8 ; TODO: double check this:/; This is for a micron DDR2, -3E part as per the verilog models
NUM_BANKS=4
NUM_ROWS=16384 
NUM_COLS=2048
DEVICE_WIDTH=4

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=3.0 ;*

CL=4 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=4 ;*
tRAS=14;* 
tRCD=4 ;*
tRRD=3 ;*
tRC=18 ;*
tRP=4  ;*
tCCD=2 ;*
tRTP=3 ;*
tWTR=3 ;*
tWR=5  ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=35;*
tFAW=13;*
tCKE=3 ;*
tXP=2  ;*

tCMD=1 ;*

IDD0=90;
IDD1=105;
IDD2P=7;
IDD2Q=45;
IDD2N=50;
IDD3Pf=35;
IDD3Ps=12;
IDD3N=65;
IDD4W=170;
IDD4R=180;
IDD5=180;
IDD6=7;
IDD6L=3;
IDD7=240;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank
Vdd=1.8 ; TODO: double check this
        docker run -d --rm -it --name ; This is for a micron DDR2, -3E part as per the verilog models
NUM_BANKS=8
NUM_ROWS=16384 
NUM_COLS=2048
DEVICE_WIDTH=4

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=2.5 ;*

CL=5 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=4 ;*
tRAS=18;* 
tRCD=5 ;*
tRRD=3 ;*
tRC=23 ;*
tRP=5 ;*
tCCD=2 ;*
tRTP=3 ;*
tWTR=3 ;*
tWR=6 ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=51;*
tFAW=14;*
tCKE=3 ;*
tXP=2  ;*

tCMD=1 ;*

IDD0=90;
IDD1=110;
IDD2P=7;
IDD2Q=45;
IDD2N=50;
IDD3Pf=35;
IDD3Ps=12;
IDD3N=60;
IDD4W=145;
IDD4R=145;
IDD5=235;
IDD6=7;
IDD6L=3;
IDD7=335;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank
Vdd=1.8 ; TODO: double check this --volumes-from=NUM_BANKS=8
NUM_ROWS=16384
NUM_COLS=1024
DEVICE_WIDTH=16

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=1.5 ;*

CL=10 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=8 ;*
tRAS=24;* 
tRCD=10 ;*
tRRD=4 ;*
tRC=34 ;*
tRP=10  ;*
tCCD=4 ;*
tRTP=5 ;*
tWTR=5 ;*
tWR=10 ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=74;*
tFAW=20;*
tCKE=4 ;*
tXP=4 ;*

tCMD=1 ;*

IDD0=110;
IDD1=130;
IDD2P=12;
IDD2Q=60;
IDD2N=65;
IDD3Pf=40;
IDD3Ps=40;
IDD3N=62;
IDD4W=220;
IDD4R=200;
IDD5=240;
IDD6=6;
IDD6L=9;
IDD7=490;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank

Vdd=1.5 ; TODO: double check this -w /; This parts is taken from the micron MT41J256M4 datasheet ( http://micron.com/document_download/?documentId=425 )

NUM_BANKS=8
NUM_ROWS=16384
NUM_COLS=2048
DEVICE_WIDTH=4

;in nanoseconds
REFRESH_PERIOD=7800
tCK=1.25

CL=11
AL=0
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=8
tRAS=28
tRCD=11
tRRD=5 ; actually 6ns=4.8CK, but rounded up
tRC=39
tRP=11
tCCD=4
tRTP=6 ; 7.5ns
tWTR=6 ; 7.5ns
tWR=12 ; 15ns
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=88
tFAW=24 ; This part has 1KB (2k columns x 4) = 30ns
tCKE=4 ; 5ns
tXP=5 ; 6ns = 4.8CK rounded up

tCMD=1 ;*

; x4 width; DDR3-1660; page 44
IDD0=95
IDD1=115
IDD2P=45 ; assuming 'fast mode'
IDD2Q=67
IDD2N=70
IDD3Pf=45 ; unused -- also DDR3 doesn't have f,s versions 
IDD3Ps=45 ; also unused
IDD3N=67  
IDD4W=250
IDD4R=250
IDD5=260
IDD6=6 ; this is unused
IDD6L=6 ; this is unused
IDD7=400 ; this is unused

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank

Vdd=1.5 ; TODO: double check this/os --privileged tizenrt/tizenrt:NUM_BANKS=8
NUM_ROWS=16384
NUM_COLS=2048
DEVICE_WIDTH=4

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=1.5 ;*

CL=10 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=8 ;*
tRAS=24;* 
tRCD=10 ;*
tRRD=4 ;*
tRC=34 ;*
tRP=10  ;*
tCCD=4 ;*
tRTP=5 ;*
tWTR=5 ;*
tWR=10 ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=74;*
tFAW=20;*
tCKE=4 ;*
tXP=4 ;*

tCMD=1 ;*

IDD0=85;
IDD1=105;
IDD2P=12;
IDD2Q=60;
IDD2N=65;
IDD3Pf=40;
IDD3Ps=40;
IDD3N=62;
IDD4W=220;
IDD4R=200;
IDD5=240;
IDD6=6;
IDD6L=9;
IDD7=315;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank

Vdd=1.5 ; TODO: double check this /bin/bash

  arm-version: &arm-version
      run: 
      name: ARM GCC Version
      command: |
        docker exec NUM_BANKS=8
NUM_ROWS=32768
NUM_COLS=1024
DEVICE_WIDTH=8

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=1.5 ;*

CL=10 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=8 ;*
tRAS=24;* 
tRCD=10 ;*
tRRD=4 ;*
tRC=34 ;*
tRP=10  ;*
tCCD=4 ;*
tRTP=5 ;*
tWTR=5 ;*
tWR=10 ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=107;*
tFAW=20;*
tCKE=4 ;*
tXP=4 ;*

tCMD=1 ;*

IDD0=130;
IDD1=155;
IDD2P=10;
IDD2Q=70;
IDD2N=70;
IDD3Pf=60;
IDD3Ps=60;
IDD3N=90;
IDD4W=300;
IDD4R=255;
IDD5=305;
IDD6=9;
IDD6L=12;
IDD7=460;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank

Vdd=1.5 ; TODO: double check this arm-none-eabi-gcc --version

  build-job: &build-job
    run:
      name: TizenRT Build Test
      command: |
        docker exec -it NUM_BANKS=8
NUM_ROWS=32768
NUM_COLS=1024
DEVICE_WIDTH=8

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=2.5 ;*

CL=5 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=8 ;*
tRAS=15;* 
tRCD=5 ;*
tRRD=4 ;*
tRC=20 ;*
tRP=5  ;*
tCCD=4 ;*
tRTP=4 ;*
tWTR=4 ;*
tWR=6 ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=64;*
tFAW=16;*
tCKE=3 ;*
tXP=3 ;*

tCMD=1 ;*

IDD0=100;
IDD1=115;
IDD2P=10;
IDD2Q=50;
IDD2N=50;
IDD3Pf=45;
IDD3Ps=45;
IDD3N=65;
IDD4W=230;
IDD4R=195;
IDD5=275;
IDD6=9;
IDD6L=12;
IDD7=400;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank

Vdd=1.5 ; TODO: double check this bash -c "cd tools; ./configure.sh NUM_BANKS=8
NUM_ROWS=32768
NUM_COLS=2048
DEVICE_WIDTH=4

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=1.5 ;*

CL=10 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=8 ;*
tRAS=24;* 
tRCD=10 ;*
tRRD=4 ;*
tRC=34 ;*
tRP=10  ;*
tCCD=4 ;*
tRTP=5 ;*
tWTR=5 ;*
tWR=10 ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=107;*
tFAW=20;*
tCKE=4 ;*
tXP=4 ;*

tCMD=1 ;*

IDD0=100;
IDD1=130;
IDD2P=10;
IDD2Q=70;
IDD2N=70;
IDD3Pf=60;
IDD3Ps=60;
IDD3N=90;
IDD4W=255;
IDD4R=230;
IDD5=305;
IDD6=9;
IDD6L=12;
IDD7=415;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank

Vdd=1.5 ; TODO: double check this
        docker exec -it version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        docker create -v /NUM_BANKS=8
NUM_ROWS=8192
NUM_COLS=1024
DEVICE_WIDTH=16

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=1.5 ;*

CL=10 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=8 ;*
tRAS=24;* 
tRCD=10 ;*
tRRD=4 ;*
tRC=34 ;*
tRP=10  ;*
tCCD=4 ;*
tRTP=5 ;*
tWTR=5 ;*
tWR=10 ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=74;*
tFAW=20;*
tCKE=4 ;*
tXP=4 ;*

tCMD=1 ;*

IDD0=110;
IDD1=150;
IDD2P=12;
IDD2Q=60;
IDD2N=65;
IDD3Pf=40;
IDD3Ps=40;
IDD3N=60;
IDD4W=355;
IDD4R=290;
IDD5=240;
IDD6=6;
IDD6L=9;
IDD7=420;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank

Vdd=1.5 ; TODO: double check this --name ;  (C) Copyright 2006-2018 Barcelona Supercomputing Center (BSC) 
;
;The copyright holder is BSC-CNS, and the authorship correspond to Kazi Asifuzzaman, Rommel Sanchez Verdejo, and Petar Radojkovic. The complete explanation of the derivation of the data can be found in the following study: Kazi Asifuzzaman, Rommel Sanchez Verdejo, and Petar Radojkovic. 2017. Enabling a reliable STT-MRAM main memory simulation. In Proceedings of the International Symposium on Memory Systems (MEMSYS '17). Washington DC, USA, 283-292. DOI: https://doi.org/10.1145/3132402.3132416

;Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

;1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

;2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.”

; This configuration lists detailed timing parameters for STT-MRAM main memory, specifying a 1.2x deviation from respective DRAM timing parameters. Please note, the current parameters (IDD0, IDD1.. etc) DOES NOT correspond to STT-MRAM and should not be used for current/energy estimations.    

NUM_BANKS=8;    
NUM_ROWS=32768; 
NUM_COLS=1024;  

DEVICE_WIDTH=8; 

REFRESH_PERIOD=7800;
tCK=1.25; 

CL=11; 
AL=0; 

BL=4;            
tRAS=20;         
tRCD=14;        
tRRD=6;         
tRC=34;         
tRP=14;        
tCCD=4;
tRTP=6;
tWTR=6;  
tWR=12; 
tRTRS=1;         
tRFC=1;      
tFAW=29;      
tCKE=4;        
tXP=5;          
tCMD=1;

; The following current parameters DOES NOT correspond to STT-MRAM, and should not be used used for current/energy estimations.    
IDD0=1305;      
IDD1=1395;     
IDD2P=846;      
IDD2Q=1030;     
IDD2N=1050;     
IDD3Pf=60;     
IDD3Ps=60;      
IDD3N=1310;    
IDD4W=1765;     
IDD4R=230;      
IDD5=1940;     
IDD6=246;       
IDD6L=246;     
IDD7=2160;      

Vdd=1.5;  --privileged tizenrt/tizenrt:;  (C) Copyright 2006-2018 Barcelona Supercomputing Center (BSC) 
;
;The copyright holder is BSC-CNS, and the authorship correspond to Kazi Asifuzzaman, Rommel Sanchez Verdejo, and Petar Radojkovic. The complete explanation of the derivation of the data can be found in the following study: Kazi Asifuzzaman, Rommel Sanchez Verdejo, and Petar Radojkovic. 2017. Enabling a reliable STT-MRAM main memory simulation. In Proceedings of the International Symposium on Memory Systems (MEMSYS '17). Washington DC, USA, 283-292. DOI: https://doi.org/10.1145/3132402.3132416

;Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

;1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

;2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.”

; This configuration lists detailed timing parameters for STT-MRAM main memory, specifying a 1.5x deviation from respective DRAM timing parameters. Please note, the current parameters (IDD0, IDD1.. etc) DOES NOT correspond to STT-MRAM and should not be used for current/energy estimations.    

NUM_BANKS=8;    
NUM_ROWS=32768; 
NUM_COLS=1024;  

DEVICE_WIDTH=8; 

REFRESH_PERIOD=7800; 
tCK=1.25;

CL=11; 
AL=0; 

BL=4;           
tRAS=23;        
tRCD=17;        
tRRD=8;         
tRC=40;         
tRP=17;        
tCCD=4;         
tRTP=6;        
tWTR=6;         
tWR=12;         
tRTRS=1;         
tRFC=1;         
tFAW=36;        
tCKE=4;         
tXP=5;         
tCMD=1; 

; The following current parameters DOES NOT correspond to STT-MRAM, and should not be used used for current/energy estimations.    
IDD0=1305;      
IDD1=1395;      
IDD2P=846;      
IDD2Q=1030;     
IDD2N=1050;      
IDD3Pf=60;      
IDD3Ps=60;      
IDD3N=1310;     
IDD4W=1765;     
IDD4R=230;      
IDD5=1940;      
IDD6=246;       
IDD6L=246;      
IDD7=2160;      

Vdd=1.5; 
 /bin/bash
        docker cp ./. ;  (C) Copyright 2006-2018 Barcelona Supercomputing Center (BSC) 
;
;The copyright holder is BSC-CNS, and the authorship correspond to Kazi Asifuzzaman, Rommel Sanchez Verdejo, and Petar Radojkovic. The complete explanation of the derivation of the data can be found in the following study: Kazi Asifuzzaman, Rommel Sanchez Verdejo, and Petar Radojkovic. 2017. Enabling a reliable STT-MRAM main memory simulation. In Proceedings of the International Symposium on Memory Systems (MEMSYS '17). Washington DC, USA, 283-292. DOI: https://doi.org/10.1145/3132402.3132416

;Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

;1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

;2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.”

; This configuration lists detailed timing parameters for STT-MRAM main memory, specifying a 2x deviation from respective DRAM timing parameters. Please note, the current parameters (IDD0, IDD1.. etc) DOES NOT correspond to STT-MRAM and should not be used for current/energy estimations.    

NUM_BANKS=8;    
NUM_ROWS=32768; 
NUM_COLS=1024;  

DEVICE_WIDTH=8; 

REFRESH_PERIOD=7800; 
tCK=1.25; 

CL=11; 
AL=0; 

BL=4;           
tRAS=28;        
tRCD=22;        
tRRD=10;        
tRC=50;         
tRP=22;         
tCCD=4;         
tRTP=6;         
tWTR=6;         
tWR=12;         
tRTRS=1;         
tRFC=1;         
tFAW=48;        
tCKE=4;         
tXP=5;          
tCMD=1; 

; The following current parameters DOES NOT correspond to STT-MRAM, and should not be used used for current/energy estimations.
IDD0=1305;      
IDD1=1395;      
IDD2P=846;      
IDD2Q=1030;     
IDD2N=1050;     
IDD3Pf=60;      
IDD3Ps=60;      
IDD3N=1310;     
IDD4W=1765;     
IDD4R=230;     
IDD5=1940;     
IDD6=246;      
IDD6L=246;      
IDD7=2160;      

Vdd=1.5; :/import torch
import torch.nn as nn
import numpy as np
from pytorch3d.structures import Meshes
from core.BaseModel import BaseReconModel
from pytorch3d.renderer import (
    look_at_view_transform,
    FoVPerspectiveCameras,
    PointLights,
    RasterizationSettings,
    MeshRenderer,
    MeshRasterizer,
    SoftPhongShader,
    TexturesVertex,
    blending
)


class BFM09ReconModel(BaseReconModel):
    def __init__(self, model_dict, **kargs):
        super(BFM09ReconModel, self).__init__(**kargs)

        self.skinmask = torch.tensor(
            model_dict['skinmask'], requires_grad=False, device=self.device)

        self.kp_inds = torch.tensor(
            model_dict['keypoints']-1).squeeze().long().to(self.device)

        self.meanshape = torch.tensor(model_dict['meanshape'],
                                      dtype=torch.float32, requires_grad=False,
                                      device=self.device)

        self.idBase = torch.tensor(model_dict['idBase'],
                                   dtype=torch.float32, requires_grad=False,
                                   device=self.device)

        self.expBase = torch.tensor(model_dict['exBase'],
                                    dtype=torch.float32, requires_grad=False,
                                    device=self.device)

        self.meantex = torch.tensor(model_dict['meantex'],
                                    dtype=torch.float32, requires_grad=False,
                                    device=self.device)

        self.texBase = torch.tensor(model_dict['texBase'],
                                    dtype=torch.float32, requires_grad=False,
                                    device=self.device)

        self.tri = torch.tensor(model_dict['tri']-1,
                                dtype=torch.int64, requires_grad=False,
                                device=self.device)

        self.point_buf = torch.tensor(model_dict['point_buf']-1,
                                      dtype=torch.int64, requires_grad=False,
                                      device=self.device)

    def get_lms(self, vs):
        lms = vs[:, self.kp_inds, :]
        return lms

    def split_coeffs(self, coeffs):
        id_coeff = coeffs[:, :80]  # identity(shape) coeff of dim 80
        exp_coeff = coeffs[:, 80:144]  # expression coeff of dim 64
        tex_coeff = coeffs[:, 144:224]  # texture(albedo) coeff of dim 80
        # ruler angles(x,y,z) for rotation of dim 3
        angles = coeffs[:, 224:227]
        # lighting coeff for 3 channel SH function of dim 27
        gamma = coeffs[:, 227:254]
        translation = coeffs[:, 254:]  # translation coeff of dim 3

        return id_coeff, exp_coeff, tex_coeff, angles, gamma, translation

    def merge_coeffs(self, id_coeff, exp_coeff, tex_coeff, angles, gamma, translation):
        coeffs = torch.cat([id_coeff, exp_coeff, tex_coeff,
                            angles, gamma, translation], dim=1)
        return coeffs

    def forward(self, coeffs, render=True):
        batch_num = coeffs.shape[0]

        id_coeff, exp_coeff, tex_coeff, angles, gamma, translation = self.split_coeffs(
            coeffs)

        vs = self.get_vs(id_coeff, exp_coeff)

        rotation = self.compute_rotation_matrix(angles)

        vs_t = self.rigid_transform(
            vs, rotation, translation)

        lms_t = self.get_lms(vs_t)
        lms_proj = self.project_vs(lms_t)
        lms_proj = torch.stack(
            [lms_proj[:, :, 0], self.img_size-lms_proj[:, :, 1]], dim=2)
        if render:
            face_texture = self.get_color(tex_coeff)
            face_norm = self.compute_norm(vs, self.tri, self.point_buf)
            face_norm_r = face_norm.bmm(rotation)
            face_color = self.add_illumination(
                face_texture, face_norm_r, gamma)
            face_color_tv = TexturesVertex(face_color)

            mesh = Meshes(vs_t, self.tri.repeat(
                batch_num, 1, 1), face_color_tv)
            rendered_img = self.renderer(mesh)
            rendered_img = torch.clamp(rendered_img, 0, 255)

            return {'rendered_img': rendered_img,
                    'lms_proj': lms_proj,
                    'face_texture': face_texture,
                    'vs': vs_t,
                    'tri': self.tri,
                    'color': face_color}
        else:
            return {'lms_proj': lms_proj}

    def get_vs(self, id_coeff, exp_coeff):
        n_b = id_coeff.size(0)

        face_shape = torch.einsum('ij,aj->ai', self.idBase, id_coeff) + \
            torch.einsum('ij,aj->ai', self.expBase, exp_coeff) + self.meanshape

        face_shape = face_shape.view(n_b, -1, 3)
        face_shape = face_shape - \
            self.meanshape.view(1, -1, 3).mean(dim=1, keepdim=True)

        return face_shape

    def get_color(self, tex_coeff):
        n_b = tex_coeff.size(0)
        face_texture = torch.einsum(
            'ij,aj->ai', self.texBase, tex_coeff) + self.meantex

        face_texture = face_texture.view(n_b, -1, 3)
        return face_texture

    def get_skinmask(self):
        return self.skinmask

    def init_coeff_dims(self):
        self.id_dims = 80
        self.tex_dims = 80
        self.exp_dims = 64
        docker run -d --rm -it --name import torch
import torch.nn as nn
import numpy as np
from pytorch3d.structures import Meshes
from pytorch3d.renderer import (
    look_at_view_transform,
    FoVPerspectiveCameras,
    PointLights,
    RasterizationSettings,
    MeshRenderer,
    MeshRasterizer,
    SoftPhongShader,
    TexturesVertex,
    blending
)


class BaseReconModel(nn.Module):
    def __init__(self, batch_size=1,
                 focal=1015, img_size=224, device='cuda:0'):
        super(BaseReconModel, self).__init__()

        self.focal = focal
        self.batch_size = batch_size
        self.img_size = img_size

        self.device = torch.device(device)
        self.renderer = self._get_renderer(self.device)

        self.p_mat = self._get_p_mat(device)
        self.reverse_z = self._get_reverse_z(device)
        self.camera_pos = self._get_camera_pose(device)

        self.rot_tensor = None
        self.exp_tensor = None
        self.id_tensor = None
        self.tex_tensor = None
        self.trans_tensor = None
        self.gamma_tensor = None

        self.init_coeff_dims()

        self.init_coeff_tensors()

    def _get_camera_pose(self, device):
        camera_pos = torch.tensor(
            [0.0, 0.0, 10.0], device=device).reshape(1, 1, 3)
        return camera_pos

    def _get_p_mat(self, device):
        half_image_width = self.img_size // 2
        p_matrix = np.array([self.focal, 0.0, half_image_width,
                             0.0, self.focal, half_image_width,
                             0.0, 0.0, 1.0], dtype=np.float32).reshape(1, 3, 3)
        return torch.tensor(p_matrix, device=device)

    def _get_reverse_z(self, device):
        reverse_z = np.reshape(
            np.array([1.0, 0, 0, 0, 1, 0, 0, 0, -1.0], dtype=np.float32), [1, 3, 3])
        return torch.tensor(reverse_z, device=device)

    def _get_renderer(self, device):
        R, T = look_at_view_transform(10, 0, 0)  # camera's position
        cameras = FoVPerspectiveCameras(device=device, R=R, T=T, znear=0.01,
                                        zfar=50,
                                        fov=2*np.arctan(self.img_size//2/self.focal)*180./np.pi)

        lights = PointLights(device=device, location=[[0.0, 0.0, 1e5]],
                             ambient_color=[[1, 1, 1]],
                             specular_color=[[0., 0., 0.]], diffuse_color=[[0., 0., 0.]])

        raster_settings = RasterizationSettings(
            image_size=self.img_size,
            blur_radius=0.0,
            faces_per_pixel=1,
        )
        blend_params = blending.BlendParams(background_color=[0, 0, 0])

        renderer = MeshRenderer(
            rasterizer=MeshRasterizer(
                cameras=cameras,
                raster_settings=raster_settings
            ),
            shader=SoftPhongShader(
                device=device,
                cameras=cameras,
                lights=lights,
                blend_params=blend_params
            )
        )
        return renderer

    def compute_norm(self, vs, tri, point_buf):

        face_id = tri
        point_id = point_buf
        v1 = vs[:, face_id[:, 0], :]
        v2 = vs[:, face_id[:, 1], :]
        v3 = vs[:, face_id[:, 2], :]
        e1 = v1 - v2
        e2 = v2 - v3
        face_norm = e1.cross(e2)
        empty = torch.zeros((face_norm.size(0), 1, 3),
                            dtype=face_norm.dtype, device=face_norm.device)
        face_norm = torch.cat((face_norm, empty), 1)
        v_norm = face_norm[:, point_id, :].sum(2)
        v_norm = v_norm / v_norm.norm(dim=2).unsqueeze(2)

        return v_norm

    def project_vs(self, vs):
        batchsize = vs.shape[0]

        vs = torch.matmul(vs, self.reverse_z.repeat(
            (batchsize, 1, 1))) + self.camera_pos
        aug_projection = torch.matmul(
            vs, self.p_mat.repeat((batchsize, 1, 1)).permute((0, 2, 1)))

        face_projection = aug_projection[:, :, :2] / \
            torch.reshape(aug_projection[:, :, 2], [batchsize, -1, 1])
        return face_projection

    def compute_rotation_matrix(self, angles):
        n_b = angles.size(0)
        sinx = torch.sin(angles[:, 0])
        siny = torch.sin(angles[:, 1])
        sinz = torch.sin(angles[:, 2])
        cosx = torch.cos(angles[:, 0])
        cosy = torch.cos(angles[:, 1])
        cosz = torch.cos(angles[:, 2])

        rotXYZ = torch.eye(3).view(1, 3, 3).repeat(
            n_b * 3, 1, 1).view(3, n_b, 3, 3).to(angles.device)

        rotXYZ[0, :, 1, 1] = cosx
        rotXYZ[0, :, 1, 2] = -sinx
        rotXYZ[0, :, 2, 1] = sinx
        rotXYZ[0, :, 2, 2] = cosx
        rotXYZ[1, :, 0, 0] = cosy
        rotXYZ[1, :, 0, 2] = siny
        rotXYZ[1, :, 2, 0] = -siny
        rotXYZ[1, :, 2, 2] = cosy
        rotXYZ[2, :, 0, 0] = cosz
        rotXYZ[2, :, 0, 1] = -sinz
        rotXYZ[2, :, 1, 0] = sinz
        rotXYZ[2, :, 1, 1] = cosz

        rotation = rotXYZ[2].bmm(rotXYZ[1]).bmm(rotXYZ[0])

        return rotation.permute(0, 2, 1)

    def add_illumination(self, face_texture, norm, gamma):

        n_b, num_vertex, _ = face_texture.size()
        n_v_full = n_b * num_vertex
        gamma = gamma.view(-1, 3, 9).clone()
        gamma[:, :, 0] += 0.8

        gamma = gamma.permute(0, 2, 1)

        a0 = np.pi
        a1 = 2 * np.pi / np.sqrt(3.0)
        a2 = 2 * np.pi / np.sqrt(8.0)
        c0 = 1 / np.sqrt(4 * np.pi)
        c1 = np.sqrt(3.0) / np.sqrt(4 * np.pi)
        c2 = 3 * np.sqrt(5.0) / np.sqrt(12 * np.pi)
        d0 = 0.5 / np.sqrt(3.0)

        Y0 = torch.ones(n_v_full).to(gamma.device).float() * a0 * c0
        norm = norm.view(-1, 3)
        nx, ny, nz = norm[:, 0], norm[:, 1], norm[:, 2]
        arrH = []

        arrH.append(Y0)
        arrH.append(-a1 * c1 * ny)
        arrH.append(a1 * c1 * nz)
        arrH.append(-a1 * c1 * nx)
        arrH.append(a2 * c2 * nx * ny)
        arrH.append(-a2 * c2 * ny * nz)
        arrH.append(a2 * c2 * d0 * (3 * nz.pow(2) - 1))
        arrH.append(-a2 * c2 * nx * nz)
        arrH.append(a2 * c2 * 0.5 * (nx.pow(2) - ny.pow(2)))

        H = torch.stack(arrH, 1)
        Y = H.view(n_b, num_vertex, 9)
        lighting = Y.bmm(gamma)

        face_color = face_texture * lighting
        return face_color

    def rigid_transform(self, vs, rot, trans):

        vs_r = torch.matmul(vs, rot)
        vs_t = vs_r + trans.view(-1, 1, 3)

        return vs_t

    def get_lms(self, vs):
        raise NotImplementedError()

    def forward(self, coeffs, render=True):
        raise NotImplementedError()

    def get_vs(self, id_coeff, exp_coeff):
        raise NotImplementedError()

    def get_color(self, tex_coeff):
        raise NotImplementedError()

    def get_rot_tensor(self):
        return self.rot_tensor

    def get_trans_tensor(self):
        return self.trans_tensor

    def get_exp_tensor(self):
        return self.exp_tensor

    def get_tex_tensor(self):
        return self.tex_tensor

    def get_id_tensor(self):
        return self.id_tensor

    def get_gamma_tensor(self):
        return self.gamma_tensor

    def init_coeff_dims(self):
        raise NotImplementedError()

    def get_packed_tensors(self):
        return self.merge_coeffs(self.id_tensor.repeat(self.batch_size, 1),
                                 self.exp_tensor,
                                 self.tex_tensor.repeat(self.batch_size, 1),
                                 self.rot_tensor, self.gamma_tensor,
                                 self.trans_tensor)

    def init_coeff_tensors(self, id_coeff=None, tex_coeff=None):
        if id_coeff is None:
            self.id_tensor = torch.zeros(
                (1, self.id_dims), dtype=torch.float32,
                requires_grad=True, device=self.device)
        else:
            assert id_coeff.shape == (1, self.id_dims)
            self.id_tensor = torch.tensor(
                id_coeff, dtype=torch.float32,
                requires_grad=True, device=self.device)

        if tex_coeff is None:
            self.tex_tensor = torch.zeros(
                (1, self.tex_dims), dtype=torch.float32,
                requires_grad=True, device=self.device)
        else:
            assert tex_coeff.shape == (1, self.tex_dims)
            self.tex_tensor = torch.tensor(
                tex_coeff, dtype=torch.float32,
                requires_grad=True, device=self.device)

        self.exp_tensor = torch.zeros(
            (self.batch_size, self.exp_dims), dtype=torch.float32,
            requires_grad=True, device=self.device)
        self.gamma_tensor = torch.zeros(
            (self.batch_size, 27), dtype=torch.float32,
            requires_grad=True, device=self.device)
        self.trans_tensor = torch.zeros(
            (self.batch_size, 3), dtype=torch.float32,
            requires_grad=True, device=self.device)
        self.rot_tensor = torch.zeros(
            (self.batch_size, 3), dtype=torch.float32,
            requires_grad=True, device=self.device) --volumes-from=from core.BFM09Model import BFM09ReconModel
from scipy.io import loadmat


def get_recon_model(model='bfm09', **kargs):
    if model == 'bfm09':
        model_path = 'BFM/BFM09_model_info.mat'
        model_dict = loadmat(model_path)
        recon_model = BFM09ReconModel(model_dict, **kargs)
        return recon_model
    else:
        raise NotImplementedError() -w /
import numpy as np
from torch.utils import data
import torch
import os
import pickle
import cv2


class FittingDataset(data.Dataset):

    def __init__(self, img_folder, pkl_path, worker_num=1, worker_ind=0):
        super(FittingDataset, self).__init__()
        with open(pkl_path, 'rb') as f:
            lm_dict = pickle.load(f)
        self.lm_dict = lm_dict
        keys = list(lm_dict.keys())
        keys = sorted(keys)
        self.item_list = []
        for k in keys:
            img_full_path = os.path.join(img_folder, str(k)+'.png')
            assert os.path.exists(
                img_full_path), 'file %s does not exist' % img_full_path
            self.item_list.append((k, img_full_path))
        num_insts = len(self.item_list)
        self.start_ind = worker_ind*(num_insts//worker_num)
        if worker_ind == worker_num-1:
            self.group_len = num_insts - self.start_ind
        else:
            self.group_len = num_insts//worker_num

    def __getitem__(self, index):

        k, img_full_path = self.item_list[self.start_ind:self.start_ind +
                                          self.group_len][index]
        lms = self.lm_dict[k]
        img = cv2.imread(img_full_path)[:, :, ::-1].astype(np.float32)
        return torch.tensor(lms), torch.tensor(img), k

    def __len__(self):
        return self.group_len/os --privileged tizenrt/tizenrt:import numpy as np
import torch
import torch.nn.functional as F


def photo_loss(pred_img, gt_img, img_mask):
    pred_img = pred_img.float()
    loss = torch.sqrt(torch.sum(torch.square(
        pred_img - gt_img), 3))*img_mask/255
    loss = torch.sum(loss, dim=(1, 2)) / torch.sum(img_mask, dim=(1, 2))
    loss = torch.mean(loss)

    return loss


def lm_loss(pred_lms, gt_lms, weight, img_size=224):
    loss = torch.sum(torch.square(pred_lms/img_size - gt_lms /
                                  img_size), dim=2) * weight.reshape(1, -1)
    loss = torch.mean(loss.sum(1))

    return loss


# def reg_loss(id_coeff, ex_coeff, tex_coeff):

#     loss = torch.square(id_coeff).sum() + \
#         torch.square(tex_coeff).sum() * 1.7e-3 + \
#         torch.square(ex_coeff).sum(1).mean() * 0.8

#     return loss
def get_l2(tensor):
    return torch.square(tensor).sum()


def reflectance_loss(tex, skin_mask):

    skin_mask = skin_mask.unsqueeze(2)
    tex_mean = torch.sum(tex*skin_mask, 1, keepdims=True)/torch.sum(skin_mask)
    loss = torch.sum(torch.square((tex-tex_mean)*skin_mask/255.)) / \
        (tex.shape[0]*torch.sum(skin_mask))

    return loss


def gamma_loss(gamma):

    gamma = gamma.reshape(-1, 3, 9)
    gamma_mean = torch.mean(gamma, dim=1, keepdims=True)
    gamma_loss = torch.mean(torch.square(gamma - gamma_mean))

    return gamma_loss /bin/bash

  arm-version: &arm-version
      run: 
      name: ARM GCC Version
      command: |
        docker exec import torch
import torch.nn as nn
import numpy as np
from pytorch3d.structures import Meshes
from pytorch3d.renderer import (
    look_at_view_transform,
    FoVPerspectiveCameras,
    PointLights,
    RasterizationSettings,
    MeshRenderer,
    MeshRasterizer,
    SoftPhongShader,
    TexturesVertex,
    blending
)


class ReconModel(nn.Module):
    def __init__(self, face_model, 
                focal=1015, img_size=224, device='cuda:0'):
        super(ReconModel, self).__init__()
        self.facemodel = face_model

        self.focal = focal
        self.img_size = img_size

        self.device = torch.device(device)

        self.renderer = self.get_renderer(self.device)

        self.kp_inds = torch.tensor(self.facemodel['keypoints']-1).squeeze().long()
        
        meanshape = nn.Parameter(torch.from_numpy(self.facemodel['meanshape'],).float(), requires_grad=False)
        self.register_parameter('meanshape', meanshape)

        idBase = nn.Parameter(torch.from_numpy(self.facemodel['idBase']).float(), requires_grad=False)
        self.register_parameter('idBase', idBase)

        exBase = nn.Parameter(torch.from_numpy(self.facemodel['exBase']).float(), requires_grad=False)
        self.register_parameter('exBase', exBase)

        meantex = nn.Parameter(torch.from_numpy(self.facemodel['meantex']).float(), requires_grad=False)
        self.register_parameter('meantex', meantex)

        texBase = nn.Parameter(torch.from_numpy(self.facemodel['texBase']).float(), requires_grad=False)
        self.register_parameter('texBase', texBase)

        tri = nn.Parameter(torch.from_numpy(self.facemodel['tri']).float(), requires_grad=False)
        self.register_parameter('tri', tri)

        point_buf = nn.Parameter(torch.from_numpy(self.facemodel['point_buf']).float(), requires_grad=False)
        self.register_parameter('point_buf', point_buf)

    def get_renderer(self, device):
        R, T = look_at_view_transform(10, 0, 0)
        cameras = FoVPerspectiveCameras(device=device, R=R, T=T, znear=0.01, zfar=50,
                                        fov=2*np.arctan(self.img_size//2/self.focal)*180./np.pi)

        lights = PointLights(device=device, location=[[0.0, 0.0, 1e5]], ambient_color=[[1, 1, 1]],
                             specular_color=[[0., 0., 0.]], diffuse_color=[[0., 0., 0.]])

        raster_settings = RasterizationSettings(
            image_size=self.img_size,
            blur_radius=0.0,
            faces_per_pixel=1,
        )
        blend_params = blending.BlendParams(background_color=[0, 0, 0])

        renderer = MeshRenderer(
            rasterizer=MeshRasterizer(
                cameras=cameras,
                raster_settings=raster_settings
            ),
            shader=SoftPhongShader(
                device=device,
                cameras=cameras,
                lights=lights,
                blend_params=blend_params
            )
        )
        return renderer

    def Split_coeff(self, coeff):
        id_coeff = coeff[:, :80]  # identity(shape) coeff of dim 80
        ex_coeff = coeff[:, 80:144]  # expression coeff of dim 64
        tex_coeff = coeff[:, 144:224]  # texture(albedo) coeff of dim 80
        angles = coeff[:, 224:227]  # ruler angles(x,y,z) for rotation of dim 3
        gamma = coeff[:, 227:254]  # lighting coeff for 3 channel SH function of dim 27
        translation = coeff[:, 254:]  # translation coeff of dim 3

        return id_coeff, ex_coeff, tex_coeff, angles, gamma, translation

    def Shape_formation(self, id_coeff, ex_coeff):
        n_b = id_coeff.size(0)

        face_shape = torch.einsum('ij,aj->ai', self.idBase, id_coeff) + \
                     torch.einsum('ij,aj->ai', self.exBase, ex_coeff) + self.meanshape

        face_shape = face_shape.view(n_b, -1, 3)
        face_shape = face_shape - self.meanshape.view(1, -1, 3).mean(dim=1, keepdim=True)

        return face_shape

    def Texture_formation(self, tex_coeff):
        n_b = tex_coeff.size(0)
        face_texture = torch.einsum('ij,aj->ai', self.texBase, tex_coeff) + self.meantex

        face_texture = face_texture.view(n_b, -1, 3)
        return face_texture

    def Compute_norm(self, face_shape):

        face_id = self.tri.long() - 1
        point_id = self.point_buf.long() - 1 
        shape = face_shape
        v1 = shape[:, face_id[:, 0], :]
        v2 = shape[:, face_id[:, 1], :]
        v3 = shape[:, face_id[:, 2], :]
        e1 = v1 - v2
        e2 = v2 - v3
        face_norm = e1.cross(e2)
        empty = torch.zeros((face_norm.size(0), 1, 3), dtype=face_norm.dtype, device=face_norm.device)
        face_norm = torch.cat((face_norm, empty), 1) 
        v_norm = face_norm[:, point_id, :].sum(2)  
        v_norm = v_norm / v_norm.norm(dim=2).unsqueeze(2)

        return v_norm

    def Projection_block(self, face_shape):
        half_image_width = self.img_size // 2
        batchsize = face_shape.shape[0]
        camera_pos = torch.tensor([0.0,0.0,10.0], device=face_shape.device).reshape(1, 1, 3)
        # tensor.reshape(constant([0.0,0.0,10.0]),[1,1,3])
        p_matrix = np.array([self.focal, 0.0, half_image_width, \
                            0.0, self.focal, half_image_width, \
                            0.0, 0.0, 1.0], dtype=np.float32)

        p_matrix = np.tile(p_matrix.reshape(1, 3, 3), [batchsize, 1, 1])
        reverse_z = np.tile(np.reshape(np.array([1.0,0,0,0,1,0,0,0,-1.0], dtype=np.float32),[1,3,3]),
                            [batchsize,1,1])
        
        p_matrix = torch.tensor(p_matrix, device=face_shape.device)
        reverse_z = torch.tensor(reverse_z, device=face_shape.device)
        face_shape = torch.matmul(face_shape,reverse_z) + camera_pos
        aug_projection = torch.matmul(face_shape,p_matrix.permute((0,2,1)))

        face_projection = aug_projection[:,:,:2]/ \
                        torch.reshape(aug_projection[:,:,2],[batchsize,-1,1])
        return face_projection

    @staticmethod
    def Compute_rotation_matrix(angles):
        n_b = angles.size(0)
        sinx = torch.sin(angles[:, 0])
        siny = torch.sin(angles[:, 1])
        sinz = torch.sin(angles[:, 2])
        cosx = torch.cos(angles[:, 0])
        cosy = torch.cos(angles[:, 1])
        cosz = torch.cos(angles[:, 2])

        rotXYZ = torch.eye(3).view(1, 3, 3).repeat(n_b * 3, 1, 1).view(3, n_b, 3, 3)

        if angles.is_cuda: rotXYZ = rotXYZ.cuda()

        rotXYZ[0, :, 1, 1] = cosx
        rotXYZ[0, :, 1, 2] = -sinx
        rotXYZ[0, :, 2, 1] = sinx
        rotXYZ[0, :, 2, 2] = cosx
        rotXYZ[1, :, 0, 0] = cosy
        rotXYZ[1, :, 0, 2] = siny
        rotXYZ[1, :, 2, 0] = -siny
        rotXYZ[1, :, 2, 2] = cosy
        rotXYZ[2, :, 0, 0] = cosz
        rotXYZ[2, :, 0, 1] = -sinz
        rotXYZ[2, :, 1, 0] = sinz
        rotXYZ[2, :, 1, 1] = cosz

        rotation = rotXYZ[2].bmm(rotXYZ[1]).bmm(rotXYZ[0])

        return rotation.permute(0, 2, 1)

    @staticmethod
    def Rigid_transform_block(face_shape, rotation, translation):
        face_shape_r = torch.matmul(face_shape, rotation)
        face_shape_t = face_shape_r + translation.view(-1, 1, 3)

        return face_shape_t

    @staticmethod
    def Illumination_layer(face_texture, norm, gamma):

        n_b, num_vertex, _ = face_texture.size()
        n_v_full = n_b * num_vertex
        gamma = gamma.view(-1, 3, 9).clone()
        gamma[:, :, 0] += 0.8

        gamma = gamma.permute(0, 2, 1)

        a0 = np.pi
        a1 = 2 * np.pi / np.sqrt(3.0)
        a2 = 2 * np.pi / np.sqrt(8.0)
        c0 = 1 / np.sqrt(4 * np.pi)
        c1 = np.sqrt(3.0) / np.sqrt(4 * np.pi)
        c2 = 3 * np.sqrt(5.0) / np.sqrt(12 * np.pi)
        d0 = 0.5/ np.sqrt(3.0)

        Y0 = torch.ones(n_v_full).to(gamma.device).float() * a0 * c0
        norm = norm.view(-1, 3)
        nx, ny, nz = norm[:, 0], norm[:, 1], norm[:, 2]
        arrH = []

        arrH.append(Y0)
        arrH.append(-a1 * c1 * ny)
        arrH.append(a1 * c1 * nz)
        arrH.append(-a1 * c1 * nx)
        arrH.append(a2 * c2 * nx * ny)
        arrH.append(-a2 * c2 * ny * nz)
        arrH.append(a2 * c2 * d0 * (3 * nz.pow(2) - 1))
        arrH.append(-a2 * c2 * nx * nz)
        arrH.append(a2 * c2 * 0.5 * (nx.pow(2) - ny.pow(2)))

        H = torch.stack(arrH, 1)
        Y = H.view(n_b, num_vertex, 9)
        lighting = Y.bmm(gamma)

        face_color = face_texture * lighting
        return face_color

    def get_lms(self, face_shape, kp_inds):
        lms = face_shape[:, kp_inds, :]
        return lms

    def forward(self, coeff):

        batch_num = coeff.shape[0]
        
        id_coeff, ex_coeff, tex_coeff, angles, gamma, translation = self.Split_coeff(coeff)

        face_shape = self.Shape_formation(id_coeff, ex_coeff)
        face_texture = self.Texture_formation(tex_coeff)
        face_norm = self.Compute_norm(face_shape)
        rotation = self.Compute_rotation_matrix(angles)
        face_norm_r = face_norm.bmm(rotation)
        face_shape_t = self.Rigid_transform_block(face_shape, rotation, translation)
        face_color = self.Illumination_layer(face_texture, face_norm_r, gamma)
        face_lms_t = self.get_lms(face_shape_t, self.kp_inds)
        lms = self.Projection_block(face_lms_t)
        lms = torch.stack([lms[:, :, 0], self.img_size-lms[:, :, 1]], dim=2)


        face_color = TexturesVertex(face_color)

        tri = self.tri - 1
        mesh = Meshes(face_shape_t, tri.repeat(batch_num, 1, 1), face_color)
        rendered_img = self.renderer(mesh)
        rendered_img = torch.clamp(rendered_img, 0, 255)

        return rendered_img, lms, face_texture, mesh arm-none-eabi-gcc --version

  build-job: &build-job
    run:
      name: TizenRT Build Test
      command: |
        docker exec -it import argparse
import os
import torch


class BaseOptions():
    def __init__(self):
        self.parser = argparse.ArgumentParser()
        self.initialized = False

    def initialize(self):
        self.parser.add_argument('--tar_size', type=int, default=256,
                                 help='size for rendering window. We use a square window.')
        self.parser.add_argument('--padding_ratio', type=float, default=0.3,
                                 help='enlarge the face detection bbox by a margin.')
        self.parser.add_argument('--recon_model', type=str, default='bfm09',
                                 help='choose a 3dmm model, default: bfm09')
        self.parser.add_argument('--first_rf_iters', type=int, default=1000,
                                 help='iteration number of rigid fitting for the first frame in video fitting.')
        self.parser.add_argument('--first_nrf_iters', type=int, default=500,
                                 help='iteration number of non-rigid fitting for the first frame in video fitting.')
        self.parser.add_argument('--rest_rf_iters', type=int, default=50,
                                 help='iteration number of rigid fitting for the remaining frames in video fitting.')
        self.parser.add_argument('--rest_nrf_iters', type=int, default=30,
                                 help='iteration number of non-rigid fitting for the remaining frames in video fitting.')
        self.parser.add_argument('--rf_lr', type=float, default=1e-2,
                                 help='learning rate for rigid fitting')
        self.parser.add_argument('--nrf_lr', type=float, default=1e-2,
                                 help='learning rate for non-rigid fitting')
        self.parser.add_argument('--lm_loss_w', type=float, default=100,
                                 help='weight for landmark loss')
        self.parser.add_argument('--rgb_loss_w', type=float, default=1.6,
                                 help='weight for rgb loss')
        self.parser.add_argument('--id_reg_w', type=float, default=1e-3,
                                 help='weight for id coefficient regularizer')
        self.parser.add_argument('--exp_reg_w', type=float, default=0.8e-3,
                                 help='weight for expression coefficient regularizer')
        self.parser.add_argument('--tex_reg_w', type=float, default=1.7e-6,
                                 help='weight for texture coefficient regularizer')
        self.parser.add_argument('--rot_reg_w', type=float, default=1,
                                 help='weight for rotation regularizer')
        self.parser.add_argument('--trans_reg_w', type=float, default=1,
                                 help='weight for translation regularizer')

        self.parser.add_argument('--tex_w', type=float, default=1,
                                 help='weight for texture reflectance loss.')
        self.parser.add_argument('--cache_folder', type=str, default='fitting_cache',
                                 help='path for the cache folder')
        self.parser.add_argument('--nframes_shape', type=int, default=16,
                                 help='number of frames used to estimate shape coefficient in video fitting')
        self.parser.add_argument('--res_folder', type=str, required=True,
                                 help='output path for the image')

        self.initialized = True

    def parse(self):
        if not self.initialized:
            self.initialize()
        self.opt = self.parser.parse_args()

        args = vars(self.opt)

        print('------------ Options -------------')
        for k, v in sorted(args.items()):
            print('%s: %s' % (str(k), str(v)))
        print('-------------- End ----------------')

        return self.opt


class ImageFittingOptions(BaseOptions):
    def initialize(self):
        BaseOptions.initialize(self)
        self.parser.add_argument('--img_path', type=str, required=True,
                                 help='path for the image')
        self.parser.add_argument('--gpu', type=int, default=0,
                                 help='gpu device')


class VideoFittingOptions(BaseOptions):
    def initialize(self):
        BaseOptions.initialize(self)
        self.parser.add_argument('--v_path', type=str, required=True,
                                 help='path for the video')
        self.parser.add_argument('--ngpus', type=int, default=1,
                                 help='gpu device')
        self.parser.add_argument('--nworkers', type=int, default=1,
                                 help='number of workers') bash -c "cd tools; ./configure.sh import pickle
import numpy as np
import os
import torch


def pad_bbox(bbox, img_wh, padding_ratio=0.2):
    x1, y1, x2, y2 = bbox
    width = x2 - x1
    height = y2 - y1
    size_bb = int(max(width, height) * (1+padding_ratio))
    center_x, center_y = (x1 + x2) // 2, (y1 + y2) // 2
    x1 = max(int(center_x - size_bb // 2), 0)
    y1 = max(int(center_y - size_bb // 2), 0)
    size_bb = min(img_wh[0] - x1, size_bb)
    size_bb = min(img_wh[1] - y1, size_bb)

    return [x1, y1, x1+size_bb, y1+size_bb]


def mymkdirs(path):
    if not os.path.exists(path):
        os.makedirs(path)


def get_lm_weights(device):
    w = torch.ones(68).to(device)
    w[28:31] = 10
    w[48:68] = 10
    norm_w = w / w.sum()
    return norm_w


def save_obj(path, v, f, c):
    with open(path, 'w') as file:
        for i in range(len(v)):
            file.write('v %f %f %f %f %f %f\n' %
                       (v[i, 0], v[i, 1], v[i, 2], c[i, 0], c[i, 1], c[i, 2]))

        file.write('\n')

        for i in range(len(f)):
            file.write('f %d %d %d\n' % (f[i, 0], f[i, 1], f[i, 2]))

    file.close()
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

  rtl8730e/loadable_ext_ddr:
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
      - rtl8730e/loadable_ext_ddr:
          requires:
            - checkout_code bash -c "make"


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

  rtl8730e/loadable_ext_ddr:
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
      - rtl8730e/loadable_ext_ddr:
          requires:
            - checkout_code
  push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]

jobs:
  build:
    runs-on: ${{ matrix.os }}

    strategy:
      # Set fail-fast to false to ensure that feedback is delivered for all matrix combinations. Consider changing this to true when your workflow is stable.
      fail-fast: false

      # Set up a matrix to run the following 3 configurations:
      # 1. <Windows, Release, latest MSVC compiler toolchain on the default runner image, default generator>
      # 2. <Linux, Release, latest GCC compiler toolchain on the default runner image, default generator>
      # 3. <Linux, Release, latest Clang compiler toolchain on the default runner image, default generator>
      #
      # To add more build types (Release, Debug, RelWithDebInfo, etc.) customize the build_type list.
      matrix:
        os: [ubuntu-latest, windows-latest]
        build_type: [Release]
        c_compiler: [gcc, clang, cl]
        include:
          - os: windows-latest
            c_compiler: cl
            cpp_compiler: cl
          - os: ubuntu-latest
            c_compiler: gcc
            cpp_compiler: g++
          - os: ubuntu-latest
            c_compiler: clang
            cpp_compiler: clang++
        exclude:
          - os: windows-latest
            c_compiler: gcc
          - os: windows-latest
            c_compiler: clang
          - os: ubuntu-latest
            c_compiler: cl

    steps:
    - uses: actions/checkout@v4

    - name: Set reusable strings
      # Turn repeated input strings (such as the build output directory) into step outputs. These step outputs can be used throughout the workflow file.
      id: strings
      shell: bash
      run: |
        echo "build-output-dir=${{ github.workspace }}/build" >> "$GITHUB_OUTPUT"

    - name: Configure CMake
      # Configure CMake in a 'build' subdirectory. `CMAKE_BUILD_TYPE` is only required if you are using a single-configuration generator such as make.
      # See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html?highlight=cmake_build_type
      run: >
        cmake -B ${{ steps.strings.outputs.build-output-dir }}
        -DCMAKE_CXX_COMPILER=${{ matrix.cpp_compiler }}
        -DCMAKE_C_COMPILER=${{ matrix.c_compiler }}
        -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
        -S ${{ github.workspace }}

    - name: Build
      # Build your program with the given configuration. Note that --config is needed because the default Windows generator is a multi-config generator (Visual Studio generator).
      run: cmake --build ${{ steps.strings.outputs.build-output-dir }} --config ${{ matrix.build_type }}

    - name: Test
      working-directory: ${{ steps.strings.outputs.build-output-dir }}
      # Execute tests defined by the CMake configuration. Note that --build-config is needed because the default Windows generator is a multi-config generator (Visual Studio generator).
      # See https://cmake.org/cmake/help/latest/manual/ctest.1.html for more detail
      run: ctest --build-config ${{ matrix.build_type }}
