# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: dramsim_test 
  push: 
#tell the linker the rpath so that we don't have to muck with LD_LIBRARY_PATH, etc
dramsim_test: dramsim_test.cpp
	$(CXX) -g -o dramsim_test dramsim_test.cpp -I../ -L../ -ldramsim -Wl,-rpath=../

clean: 
	rm dramsim_test
    branches: [ "main" ]
  pull_request: /*********************************************************************************
*  Copyright (c) 2010-2011, 
*                             
*                             
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

    branches: [ "main" ]
  schedule: /*********************************************************************************
*  Copyright (c) 2010-2011, 
*                             
*                             
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
};
    - cron: ; blah blah
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
Vdd=1.8 ; TODO: double check this

env: ; This is for a micron DDR2, -3E part as per the verilog models
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
  # Path to the CMake build directory.
  build: ; This is for a micron DDR2, -3E part as per the verilog models
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
Vdd=1.8 ; TODO: double check this

permissions: NUM_BANKS=8
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

Vdd=1.5 ; TODO: double check this
  contents: ; This parts is taken from the micron MT41J256M4 datasheet ( http://micron.com/document_download/?documentId=425 )

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

Vdd=1.5 ; TODO: double check this

jobs: NUM_BANKS=8
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

Vdd=1.5 ; TODO: double check this
  analyze: NUM_BANKS=8
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

Vdd=1.5 ; TODO: double check this
    permissions: NUM_BANKS=8
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

Vdd=1.5 ; TODO: double check this
      contents: NUM_BANKS=8
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
      security-events: NUM_BANKS=8
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

Vdd=1.5 ; TODO: double check this
      actions: ;  (C) Copyright 2006-2018 Barcelona Supercomputing Center (BSC) 
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

Vdd=1.5; 
    name: ;  (C) Copyright 2006-2018 Barcelona Supercomputing Center (BSC) 
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

    runs-on: ;  (C) Copyright 2006-2018 Barcelona Supercomputing Center (BSC) 
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

Vdd=1.5; 
    steps: model:
  base_learning_rate: 4.5e-6
  target: ldm.models.autoencoder.AutoencoderKL
  params:
    monitor: "val/rec_loss"
    embed_dim: 16
    lossconfig:
      target: ldm.modules.losses.LPIPSWithDiscriminator
      params:
        disc_start: 50001
        kl_weight: 0.000001
        disc_weight: 0.5

    ddconfig:
      double_z: True
      z_channels: 16
      resolution: 256
      in_channels: 3
      out_ch: 3
      ch: 128
      ch_mult: [ 1,1,2,2,4]  # num_down = len(ch_mult)-1
      num_res_blocks: 2
      attn_resolutions: [16]
      dropout: 0.0


data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 12
    wrap: True
    train:
      target: ldm.data.imagenet.ImageNetSRTrain
      params:
        size: 256
        degradation: pil_nearest
    validation:
      target: ldm.data.imagenet.ImageNetSRValidation
      params:
        size: 256
        degradation: pil_nearest

lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 1000
        max_images: 8
        increase_log_steps: True

  trainer:
    benchmark: True
    accumulate_grad_batches: 2
      - name: model:
  base_learning_rate: 4.5e-6
  target: ldm.models.autoencoder.AutoencoderKL
  params:
    monitor: "val/rec_loss"
    embed_dim: 4
    lossconfig:
      target: ldm.modules.losses.LPIPSWithDiscriminator
      params:
        disc_start: 50001
        kl_weight: 0.000001
        disc_weight: 0.5

    ddconfig:
      double_z: True
      z_channels: 4
      resolution: 256
      in_channels: 3
      out_ch: 3
      ch: 128
      ch_mult: [ 1,2,4,4 ]  # num_down = len(ch_mult)-1
      num_res_blocks: 2
      attn_resolutions: [ ]
      dropout: 0.0

data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 12
    wrap: True
    train:
      target: ldm.data.imagenet.ImageNetSRTrain
      params:
        size: 256
        degradation: pil_nearest
    validation:
      target: ldm.data.imagenet.ImageNetSRValidation
      params:
        size: 256
        degradation: pil_nearest

lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 1000
        max_images: 8
        increase_log_steps: True

  trainer:
    benchmark: True
    accumulate_grad_batches: 2
        uses: model:
  base_learning_rate: 4.5e-6
  target: ldm.models.autoencoder.AutoencoderKL
  params:
    monitor: "val/rec_loss"
    embed_dim: 3
    lossconfig:
      target: ldm.modules.losses.LPIPSWithDiscriminator
      params:
        disc_start: 50001
        kl_weight: 0.000001
        disc_weight: 0.5

    ddconfig:
      double_z: True
      z_channels: 3
      resolution: 256
      in_channels: 3
      out_ch: 3
      ch: 128
      ch_mult: [ 1,2,4 ]  # num_down = len(ch_mult)-1
      num_res_blocks: 2
      attn_resolutions: [ ]
      dropout: 0.0


data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 12
    wrap: True
    train:
      target: ldm.data.imagenet.ImageNetSRTrain
      params:
        size: 256
        degradation: pil_nearest
    validation:
      target: ldm.data.imagenet.ImageNetSRValidation
      params:
        size: 256
        degradation: pil_nearest

lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 1000
        max_images: 8
        increase_log_steps: True

  trainer:
    benchmark: True
    accumulate_grad_batches: 2

      - name: model:
  base_learning_rate: 4.5e-6
  target: ldm.models.autoencoder.AutoencoderKL
  params:
    monitor: "val/rec_loss"
    embed_dim: 64
    lossconfig:
      target: ldm.modules.losses.LPIPSWithDiscriminator
      params:
        disc_start: 50001
        kl_weight: 0.000001
        disc_weight: 0.5

    ddconfig:
      double_z: True
      z_channels: 64
      resolution: 256
      in_channels: 3
      out_ch: 3
      ch: 128
      ch_mult: [ 1,1,2,2,4,4]  # num_down = len(ch_mult)-1
      num_res_blocks: 2
      attn_resolutions: [16,8]
      dropout: 0.0

data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 12
    wrap: True
    train:
      target: ldm.data.imagenet.ImageNetSRTrain
      params:
        size: 256
        degradation: pil_nearest
    validation:
      target: ldm.data.imagenet.ImageNetSRValidation
      params:
        size: 256
        degradation: pil_nearest

lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 1000
        max_images: 8
        increase_log_steps: True

  trainer:
    benchmark: True
    accumulate_grad_batches: 2
        run: model:
  base_learning_rate: 2.0e-06
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.0015
    linear_end: 0.0195
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: image
    image_size: 64
    channels: 3
    monitor: val/loss_simple_ema

    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 64
        in_channels: 3
        out_channels: 3
        model_channels: 224
        attention_resolutions:
        # note: this isn\t actually the resolution but
        # the downsampling factor, i.e. this corresnponds to
        # attention on spatial resolution 8,16,32, as the
        # spatial reolution of the latents is 64 for f4
        - 8
        - 4
        - 2
        num_res_blocks: 2
        channel_mult:
        - 1
        - 2
        - 3
        - 4
        num_head_channels: 32
    first_stage_config:
      target: ldm.models.autoencoder.VQModelInterface
      params:
        embed_dim: 3
        n_embed: 8192
        ckpt_path: models/first_stage_models/vq-f4/model.ckpt
        ddconfig:
          double_z: false
          z_channels: 3
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 2
          - 4
          num_res_blocks: 2
          attn_resolutions: []
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity
    cond_stage_config: __is_unconditional__
data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 48
    num_workers: 5
    wrap: false
    train:
      target: taming.data.faceshq.CelebAHQTrain
      params:
        size: 256
    validation:
      target: taming.data.faceshq.CelebAHQValidation
      params:
        size: 256


lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 5000
        max_images: 8
        increase_log_steps: False

  trainer:
    benchmark: True

      # Build is not required unless generated source files are used
      # - name: model:
  base_learning_rate: 1.0e-06
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.0015
    linear_end: 0.0195
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: image
    cond_stage_key: class_label
    image_size: 32
    channels: 4
    cond_stage_trainable: true
    conditioning_key: crossattn
    monitor: val/loss_simple_ema
    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 32
        in_channels: 4
        out_channels: 4
        model_channels: 256
        attention_resolutions:
        #note: this isn\t actually the resolution but
        # the downsampling factor, i.e. this corresnponds to
        # attention on spatial resolution 8,16,32, as the
        # spatial reolution of the latents is 32 for f8
        - 4
        - 2
        - 1
        num_res_blocks: 2
        channel_mult:
        - 1
        - 2
        - 4
        num_head_channels: 32
        use_spatial_transformer: true
        transformer_depth: 1
        context_dim: 512
    first_stage_config:
      target: ldm.models.autoencoder.VQModelInterface
      params:
        embed_dim: 4
        n_embed: 16384
        ckpt_path: configs/first_stage_models/vq-f8/model.yaml
        ddconfig:
          double_z: false
          z_channels: 4
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 2
          - 2
          - 4
          num_res_blocks: 2
          attn_resolutions:
          - 32
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity
    cond_stage_config:
      target: ldm.modules.encoders.modules.ClassEmbedder
      params:
        embed_dim: 512
        key: class_label
data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 64
    num_workers: 12
    wrap: false
    train:
      target: ldm.data.imagenet.ImageNetTrain
      params:
        config:
          size: 256
    validation:
      target: ldm.data.imagenet.ImageNetValidation
      params:
        config:
          size: 256


lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 5000
        max_images: 8
        increase_log_steps: False

  trainer:
    benchmark: True
      #   run: model:
  base_learning_rate: 0.0001
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.0015
    linear_end: 0.0195
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: image
    cond_stage_key: class_label
    image_size: 64
    channels: 3
    cond_stage_trainable: true
    conditioning_key: crossattn
    monitor: val/loss
    use_ema: False

    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 64
        in_channels: 3
        out_channels: 3
        model_channels: 192
        attention_resolutions:
        - 8
        - 4
        - 2
        num_res_blocks: 2
        channel_mult:
        - 1
        - 2
        - 3
        - 5
        num_heads: 1
        use_spatial_transformer: true
        transformer_depth: 1
        context_dim: 512

    first_stage_config:
      target: ldm.models.autoencoder.VQModelInterface
      params:
        embed_dim: 3
        n_embed: 8192
        ddconfig:
          double_z: false
          z_channels: 3
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 2
          - 4
          num_res_blocks: 2
          attn_resolutions: []
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity

    cond_stage_config:
      target: ldm.modules.encoders.modules.ClassEmbedder
      params:
        n_classes: 1001
        embed_dim: 512
        key: class_label

      - name: model:
  base_learning_rate: 2.0e-06
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.0015
    linear_end: 0.0195
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: image
    image_size: 64
    channels: 3
    monitor: val/loss_simple_ema
    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 64
        in_channels: 3
        out_channels: 3
        model_channels: 224
        attention_resolutions:
        # note: this isn\t actually the resolution but
        # the downsampling factor, i.e. this corresnponds to
        # attention on spatial resolution 8,16,32, as the
        # spatial reolution of the latents is 64 for f4
        - 8
        - 4
        - 2
        num_res_blocks: 2
        channel_mult:
        - 1
        - 2
        - 3
        - 4
        num_head_channels: 32
    first_stage_config:
      target: ldm.models.autoencoder.VQModelInterface
      params:
        embed_dim: 3
        n_embed: 8192
        ckpt_path: configs/first_stage_models/vq-f4/model.yaml
        ddconfig:
          double_z: false
          z_channels: 3
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 2
          - 4
          num_res_blocks: 2
          attn_resolutions: []
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity
    cond_stage_config: __is_unconditional__
data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 42
    num_workers: 5
    wrap: false
    train:
      target: taming.data.faceshq.FFHQTrain
      params:
        size: 256
    validation:
      target: taming.data.faceshq.FFHQValidation
      params:
        size: 256


lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 5000
        max_images: 8
        increase_log_steps: False

  trainer:
    benchmark: True
        uses: model:
  base_learning_rate: 2.0e-06
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.0015
    linear_end: 0.0195
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: image
    image_size: 64
    channels: 3
    monitor: val/loss_simple_ema
    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 64
        in_channels: 3
        out_channels: 3
        model_channels: 224
        attention_resolutions:
        # note: this isn\t actually the resolution but
        # the downsampling factor, i.e. this corresnponds to
        # attention on spatial resolution 8,16,32, as the
        # spatial reolution of the latents is 64 for f4
        - 8
        - 4
        - 2
        num_res_blocks: 2
        channel_mult:
        - 1
        - 2
        - 3
        - 4
        num_head_channels: 32
    first_stage_config:
      target: ldm.models.autoencoder.VQModelInterface
      params:
        ckpt_path: configs/first_stage_models/vq-f4/model.yaml
        embed_dim: 3
        n_embed: 8192
        ddconfig:
          double_z: false
          z_channels: 3
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 2
          - 4
          num_res_blocks: 2
          attn_resolutions: []
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity
    cond_stage_config: __is_unconditional__
data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 48
    num_workers: 5
    wrap: false
    train:
      target: ldm.data.lsun.LSUNBedroomsTrain
      params:
        size: 256
    validation:
      target: ldm.data.lsun.LSUNBedroomsValidation
      params:
        size: 256


lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 5000
        max_images: 8
        increase_log_steps: False

  trainer:
    benchmark: True
        # Provide a unique ID to access the sarif output path
        id: model:
  base_learning_rate: 5.0e-5   # set to target_lr by starting main.py with '--scale_lr False'
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.0015
    linear_end: 0.0155
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    loss_type: l1
    first_stage_key: "image"
    cond_stage_key: "image"
    image_size: 32
    channels: 4
    cond_stage_trainable: False
    concat_mode: False
    scale_by_std: True
    monitor: 'val/loss_simple_ema'

    scheduler_config: # 10000 warmup steps
      target: ldm.lr_scheduler.LambdaLinearScheduler
      params:
        warm_up_steps: [10000]
        cycle_lengths: [10000000000000]
        f_start: [1.e-6]
        f_max: [1.]
        f_min: [ 1.]

    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 32
        in_channels: 4
        out_channels: 4
        model_channels: 192
        attention_resolutions: [ 1, 2, 4, 8 ]   # 32, 16, 8, 4
        num_res_blocks: 2
        channel_mult: [ 1,2,2,4,4 ]  # 32, 16, 8, 4, 2
        num_heads: 8
        use_scale_shift_norm: True
        resblock_updown: True

    first_stage_config:
      target: ldm.models.autoencoder.AutoencoderKL
      params:
        embed_dim: 4
        monitor: "val/rec_loss"
        ckpt_path: "models/first_stage_models/kl-f8/model.ckpt"
        ddconfig:
          double_z: True
          z_channels: 4
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult: [ 1,2,4,4 ]  # num_down = len(ch_mult)-1
          num_res_blocks: 2
          attn_resolutions: [ ]
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity

    cond_stage_config: "__is_unconditional__"

data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 96
    num_workers: 5
    wrap: False
    train:
      target: ldm.data.lsun.LSUNChurchesTrain
      params:
        size: 256
    validation:
      target: ldm.data.lsun.LSUNChurchesValidation
      params:
        size: 256

lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 5000
        max_images: 8
        increase_log_steps: False


  trainer:
    benchmark: True
        with: model:
  base_learning_rate: 5.0e-05
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.00085
    linear_end: 0.012
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: image
    cond_stage_key: caption
    image_size: 32
    channels: 4
    cond_stage_trainable: true
    conditioning_key: crossattn
    monitor: val/loss_simple_ema
    scale_factor: 0.18215
    use_ema: False

    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 32
        in_channels: 4
        out_channels: 4
        model_channels: 320
        attention_resolutions:
        - 4
        - 2
        - 1
        num_res_blocks: 2
        channel_mult:
        - 1
        - 2
        - 4
        - 4
        num_heads: 8
        use_spatial_transformer: true
        transformer_depth: 1
        context_dim: 1280
        use_checkpoint: true
        legacy: False

    first_stage_config:
      target: ldm.models.autoencoder.AutoencoderKL
      params:
        embed_dim: 4
        monitor: val/rec_loss
        ddconfig:
          double_z: true
          z_channels: 4
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 2
          - 4
          - 4
          num_res_blocks: 2
          attn_resolutions: []
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity

    cond_stage_config:
      target: ldm.modules.encoders.modules.BERTEmbedder
      params:
        n_embed: 1280
        n_layer: 32
          cmakeBuildDirectory: model:
  base_learning_rate: 0.0001
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.0015
    linear_end: 0.015
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: jpg
    cond_stage_key: nix
    image_size: 48
    channels: 16
    cond_stage_trainable: false
    conditioning_key: crossattn
    monitor: val/loss_simple_ema
    scale_by_std: false
    scale_factor: 0.22765929
    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 48
        in_channels: 16
        out_channels: 16
        model_channels: 448
        attention_resolutions:
        - 4
        - 2
        - 1
        num_res_blocks: 2
        channel_mult:
        - 1
        - 2
        - 3
        - 4
        use_scale_shift_norm: false
        resblock_updown: false
        num_head_channels: 32
        use_spatial_transformer: true
        transformer_depth: 1
        context_dim: 768
        use_checkpoint: true
    first_stage_config:
      target: ldm.models.autoencoder.AutoencoderKL
      params:
        monitor: val/rec_loss
        embed_dim: 16
        ddconfig:
          double_z: true
          z_channels: 16
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 1
          - 2
          - 2
          - 4
          num_res_blocks: 2
          attn_resolutions:
          - 16
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity
    cond_stage_config:
      target: torch.nn.Identity
          # Ruleset file that will determine what checks will be run
          ruleset: model:
  base_learning_rate: 1.0e-04
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.00085
    linear_end: 0.0120
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: "jpg"
    cond_stage_key: "txt"
    image_size: 64
    channels: 4
    cond_stage_trainable: false   # Note: different from the one we trained before
    conditioning_key: crossattn
    monitor: val/loss_simple_ema
    scale_factor: 0.18215
    use_ema: False

    scheduler_config: # 10000 warmup steps
      target: ldm.lr_scheduler.LambdaLinearScheduler
      params:
        warm_up_steps: [ 10000 ]
        cycle_lengths: [ 10000000000000 ] # incredibly large number to prevent corner cases
        f_start: [ 1.e-6 ]
        f_max: [ 1. ]
        f_min: [ 1. ]

    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 32 # unused
        in_channels: 4
        out_channels: 4
        model_channels: 320
        attention_resolutions: [ 4, 2, 1 ]
        num_res_blocks: 2
        channel_mult: [ 1, 2, 4, 4 ]
        num_heads: 8
        use_spatial_transformer: True
        transformer_depth: 1
        context_dim: 768
        use_checkpoint: True
        legacy: False

    first_stage_config:
      target: ldm.models.autoencoder.AutoencoderKL
      params:
        embed_dim: 4
        monitor: val/rec_loss
        ddconfig:
          double_z: true
          z_channels: 4
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 2
          - 4
          - 4
          num_res_blocks: 2
          attn_resolutions: []
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity

    cond_stage_config:
      target: ldm.modules.encoders.modules.FrozenCLIPEmbedder

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: Upload SARIF to GitHub
        uses: github/codeql-action/upload-sarif@v3
        with:
          sarif_file: ${{ steps.run-analysis.outputs.sarif }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v3
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
