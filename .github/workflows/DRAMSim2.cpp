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

    steps: *.trc
      - name: Before running these traces in DRAMSim, run the preprocessor: 

./traceParse.py trace.tar.gz

The resulting .trc file should be used with DRAMSim
        uses: #!/usr/bin/python

import re, os 
import string
import sys
import array

if len(sys.argv) != 2:
  sys.exit("Must specify trace file (.gz)")


gztrace_filename = sys.argv[1]
tracefile_filename = sys.argv[1][0:len(sys.argv[1])-3]
outfile = open(tracefile_filename,"w")
temp_trace = tracefile_filename + ".temp"

zcat_cmd = "zcat";
# accomodate OSX
if os.uname()[0] == "Darwin":
	print "Detected OSX, using gzcat..."
	zcat_cmd = "gzcat"

if not os.path.exists(gztrace_filename):
  print "Could not find gzipped tracefile either"
  quit()
else:
  print "Unzipping gz trace...",
  os.system("%s %s > %s" % (zcat_cmd, gztrace_filename, temp_trace))
if not os.path.exists(tracefile_filename):
  print "FAILED"
  quit()
else:
  print "OK"

print "Parsing ",
tracefile = open(temp_trace,"r")

if tracefile_filename.startswith("k6"):
  print "k6 trace ..."
  linePattern = re.compile(r'(0x[0-9A-F]+)\s+([A-Z_]+)\s+([0-9.,]+)\s+(.*)')
  for line in tracefile:
    searchResult = linePattern.search(line)
    if searchResult:
        (address,command,time,units) = searchResult.groups()

        length = len(time)
        time = time[0:length-5]
        temp = len(time)
        if temp==0:
            time = "0"
        time = string.replace(time,",","")
        time = string.replace(time,".","")
        if command != "BOFF" and command != "P_INT_ACK":
            outfile.write("%s %s %s\n" % (address,command,time)) 
            
elif tracefile_filename.startswith("mase"):
  print "mase trace ...",
  os.system("cp %s %s"%(temp_trace, tracefile_filename));
  print "OK"
      
else:
  print "Unknown trace file!!!"
  quit()

os.system("rm %s" % temp_trace);

            - name: results/
*.o
*.po
*.dep
*.deppo
*.swp
DRAMSim
libdramsim.a
libdramsim.so
        run: cmake -B ${{ /*********************************************************************************
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
#include "SystemConfiguration.h"
#include "AddressMapping.h"

namespace DRAMSim
{

void addressMapping(uint64_t physicalAddress, unsigned &newTransactionChan, unsigned &newTransactionRank, unsigned &newTransactionBank, unsigned &newTransactionRow, unsigned &newTransactionColumn)
{
        uint64_t tempA, tempB;
        unsigned transactionSize = TRANSACTION_SIZE;
        uint64_t transactionMask =  transactionSize - 1; //ex: (64 bit bus width) x (8 Burst Length) - 1 = 64 bytes - 1 = 63 = 0x3f mask
        unsigned channelBitWidth = NUM_CHANS_LOG;
        unsigned rankBitWidth = NUM_RANKS_LOG;
        unsigned bankBitWidth = NUM_BANKS_LOG;
        unsigned rowBitWidth = NUM_ROWS_LOG;
        unsigned colBitWidth = NUM_COLS_LOG;
        // this forces the alignment to the width of a single burst (64 bits = 8 bytes = 3 address bits for DDR parts)
        unsigned byteOffsetWidth = BYTE_OFFSET_WIDTH;
        // Since we're assuming that a request is for BL*BUS_WIDTH, the bottom bits
        // of this address *should* be all zeros if it's not, issue a warning

        if ((physicalAddress & transactionMask) != 0)
        {
                DEBUG("WARNING: address 0x"<<std::hex<<physicalAddress<<std::dec<<" is not aligned to the request size of "<<transactionSize); 
        }

        // each burst will contain JEDEC_DATA_BUS_BITS/8 bytes of data, so the bottom bits (3 bits for a single channel DDR system) are
        //         thrown away before mapping the other bits
        physicalAddress >>= byteOffsetWidth;

        // The next thing we have to consider is that when a request is made for a
        // we've taken into account the granulaity of a single burst by shifting 
        // off the bottom 3 bits, but a transaction has to take into account the
        // burst length (i.e. the requests will be aligned to cache line sizes which
        // should be equal to transactionSize above). 
        //
        // Since the column address increments internally on bursts, the bottom n 
        // bits of the column (colLow) have to be zero in order to account for the 
        // total size of the transaction. These n bits should be shifted off the 
        // address and also subtracted from the total column width. 
        //
        // I am having a hard time explaining the reasoning here, but it comes down
        // this: for a 64 byte transaction, the bottom 6 bits of the address must be 
        // zero. These zero bits must be made up of the byte offset (3 bits) and also
        // from the bottom bits of the column 
        // 
        // For example: cowLowBits = log2(64bytes) - 3 bits = 3 bits 
        unsigned colLowBitWidth = COL_LOW_BIT_WIDTH;

        physicalAddress >>= colLowBitWidth;
        unsigned colHighBitWidth = colBitWidth - colLowBitWidth; 
        if (DEBUG_ADDR_MAP)
        {
                DEBUG("Bit widths: ch:"<<channelBitWidth<<" r:"<<rankBitWidth<<" b:"<<bankBitWidth
                                <<" row:"<<rowBitWidth<<" colLow:"<<colLowBitWidth
                                << " colHigh:"<<colHighBitWidth<<" off:"<<byteOffsetWidth 
                                << " Total:"<< (channelBitWidth + rankBitWidth + bankBitWidth + rowBitWidth + colLowBitWidth + colHighBitWidth + byteOffsetWidth));
        }

        //perform various address mapping schemes
        if (addressMappingScheme == Scheme1)
        {
                //chan:rank:row:col:bank
                tempA = physicalAddress;
                physicalAddress = physicalAddress >> bankBitWidth;
                tempB = physicalAddress << bankBitWidth;
                newTransactionBank = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> colHighBitWidth;
                tempB = physicalAddress << colHighBitWidth;
                newTransactionColumn = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> rowBitWidth;
                tempB = physicalAddress << rowBitWidth;
                newTransactionRow = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> rankBitWidth;
                tempB = physicalAddress << rankBitWidth;
                newTransactionRank = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> channelBitWidth;
                tempB = physicalAddress << channelBitWidth;
                newTransactionChan = tempA ^ tempB;

        }
        else if (addressMappingScheme == Scheme2)
        {
                //chan:row:col:bank:rank
                tempA = physicalAddress;
                physicalAddress = physicalAddress >> rankBitWidth;
                tempB = physicalAddress << rankBitWidth;
                newTransactionRank = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> bankBitWidth;
                tempB = physicalAddress << bankBitWidth;
                newTransactionBank = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> colHighBitWidth;
                tempB = physicalAddress << colHighBitWidth;
                newTransactionColumn = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> rowBitWidth;
                tempB = physicalAddress << rowBitWidth;
                newTransactionRow = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> channelBitWidth;
                tempB = physicalAddress << channelBitWidth;
                newTransactionChan = tempA ^ tempB;

        }
        else if (addressMappingScheme == Scheme3)
        {
                //chan:rank:bank:col:row
                tempA = physicalAddress;
                physicalAddress = physicalAddress >> rowBitWidth;
                tempB = physicalAddress << rowBitWidth;
                newTransactionRow = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> colHighBitWidth;
                tempB = physicalAddress << colHighBitWidth;
                newTransactionColumn = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> bankBitWidth;
                tempB = physicalAddress << bankBitWidth;
                newTransactionBank = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> rankBitWidth;
                tempB = physicalAddress << rankBitWidth;
                newTransactionRank = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> channelBitWidth;
                tempB = physicalAddress << channelBitWidth;
                newTransactionChan = tempA ^ tempB;

        }
        else if (addressMappingScheme == Scheme4)
        {
                //chan:rank:bank:row:col
                tempA = physicalAddress;
                physicalAddress = physicalAddress >> colHighBitWidth;
                tempB = physicalAddress << colHighBitWidth;
                newTransactionColumn = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> rowBitWidth;
                tempB = physicalAddress << rowBitWidth;
                newTransactionRow = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> bankBitWidth;
                tempB = physicalAddress << bankBitWidth;
                newTransactionBank = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> rankBitWidth;
                tempB = physicalAddress << rankBitWidth;
                newTransactionRank = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> channelBitWidth;
                tempB = physicalAddress << channelBitWidth;
                newTransactionChan = tempA ^ tempB;

        }
        else if (addressMappingScheme == Scheme5)
        {
                //chan:row:col:rank:bank

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> bankBitWidth;
                tempB = physicalAddress << bankBitWidth;
                newTransactionBank = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> rankBitWidth;
                tempB = physicalAddress << rankBitWidth;
                newTransactionRank = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> colHighBitWidth;
                tempB = physicalAddress << colHighBitWidth;
                newTransactionColumn = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> rowBitWidth;
                tempB = physicalAddress << rowBitWidth;
                newTransactionRow = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> channelBitWidth;
                tempB = physicalAddress << channelBitWidth;
                newTransactionChan = tempA ^ tempB;


        }
        else if (addressMappingScheme == Scheme6)
        {
                //chan:row:bank:rank:col

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> colHighBitWidth;
                tempB = physicalAddress << colHighBitWidth;
                newTransactionColumn = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> rankBitWidth;
                tempB = physicalAddress << rankBitWidth;
                newTransactionRank = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> bankBitWidth;
                tempB = physicalAddress << bankBitWidth;
                newTransactionBank = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> rowBitWidth;
                tempB = physicalAddress << rowBitWidth;
                newTransactionRow = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> channelBitWidth;
                tempB = physicalAddress << channelBitWidth;
                newTransactionChan = tempA ^ tempB;


        }
        // clone of scheme 5, but channel moved to lower bits
        else if (addressMappingScheme == Scheme7)
        {
                //row:col:rank:bank:chan
                tempA = physicalAddress;
                physicalAddress = physicalAddress >> channelBitWidth;
                tempB = physicalAddress << channelBitWidth;
                newTransactionChan = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> bankBitWidth;
                tempB = physicalAddress << bankBitWidth;
                newTransactionBank = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> rankBitWidth;
                tempB = physicalAddress << rankBitWidth;
                newTransactionRank = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> colHighBitWidth;
                tempB = physicalAddress << colHighBitWidth;
                newTransactionColumn = tempA ^ tempB;

                tempA = physicalAddress;
                physicalAddress = physicalAddress >> rowBitWidth;
                tempB = physicalAddress << rowBitWidth;
                newTransactionRow = tempA ^ tempB;

        }

        else
        {
                ERROR("== Error - Unknown Address Mapping Scheme");
                exit(-1);
        }
        if (DEBUG_ADDR_MAP)
        {
                DEBUG("Mapped Ch="<<newTransactionChan<<" Rank="<<newTransactionRank
                                <<" Bank="<<newTransactionBank<<" Row="<<newTransactionRow
                                <<" Col="<<newTransactionColumn<<"\n"); 
        }

}
}; }}

      # Build is not required unless generated source files are used
      # - name: /*********************************************************************************
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
#ifndef ADDRESS_MAPPING_H
#define ADDRESS_MAPPING_H
namespace DRAMSim
{
	void addressMapping(uint64_t physicalAddress, unsigned &channel, unsigned &rank, unsigned &bank, unsigned &row, unsigned &col);
}

#endif
      #   run: cmake --build ${{ /*********************************************************************************
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

//Bank.cpp
//
//Class file for bank object
//

#include "Bank.h"
#include "BusPacket.h"

using namespace std;
using namespace DRAMSim;

Bank::Bank(ostream &dramsim_log_):
                currentState(dramsim_log_), 
                rowEntries(NUM_COLS),
                dramsim_log(dramsim_log_)
{}

/* The bank class is just a glorified sparse storage data structure
 * that keeps track of written data in case the simulator wants a
 * function DRAM model
 *
 * A vector of size NUM_COLS keeps a linked list of rows and their
 * associated values.
 *
 * write() adds an entry to the proper linked list or replaces the
 *         value in a row that was already written
 *
 * read() searches for a node with the right row value, if not found
 *         returns the tracer value 0xDEADBEEF
 * 
 *        TODO: if anyone wants to actually store data, see the 'data_storage' branch and perhaps try to merge that into master
 */



Bank::DataStruct *Bank::searchForRow(unsigned row, DataStruct *head)
{
        while (head != NULL)
        {
                if (head->row == row)
                {
                        //found it
                        return head;
                }
                //keep looking
                head = head->next;
        }
        //if we get here, didn't find it
        return NULL;
}

void Bank::read(BusPacket *busPacket)
{
        DataStruct *rowHeadNode = rowEntries[busPacket->column];
        DataStruct *foundNode = NULL;

        if ((foundNode = Bank::searchForRow(busPacket->row, rowHeadNode)) == NULL)
        {
                // the row hasn't been written before, so it isn't in the list
                //if(SHOW_SIM_OUTPUT) DEBUG("== Warning - Read from previously unwritten row " << busPacket->row);
                void *garbage = calloc(BL * (JEDEC_DATA_BUS_BITS/8),1);
                ((long *)garbage)[0] = 0xdeadbeef; // tracer value
                busPacket->data = garbage;
        }
        else // found it
        {
                busPacket->data = foundNode->data;
        }

        //the return packet should be a data packet, not a read packet
        busPacket->busPacketType = DATA;
}


void Bank::write(const BusPacket *busPacket)
{
        //TODO: move all the error checking to BusPacket so once we have a bus packet,
        //                        we know the fields are all legal

        if (busPacket->column >= NUM_COLS)
        {
                ERROR("== Error - Bus Packet column "<< busPacket->column <<" out of bounds");
                exit(-1);
        }

        // head of the list we need to search
        DataStruct *rowHeadNode = rowEntries[busPacket->column];
        DataStruct *foundNode = NULL;

        if ((foundNode = Bank::searchForRow(busPacket->row, rowHeadNode)) == NULL)
        {
                //not found
                DataStruct *newRowNode = (DataStruct *)malloc(sizeof(DataStruct));

                //insert at the head for speed
                //TODO: Optimize this data structure for speedier lookups?
                newRowNode->row = busPacket->row;
                newRowNode->data = busPacket->data;
                newRowNode->next = rowHeadNode;
                rowEntries[busPacket->column] = newRowNode;
        }
        else
        {
                // found it, just plaster in the new data
                foundNode->data = busPacket->data;
                if (DEBUG_BANKS)
                {
                        PRINTN(" -- Bank "<<busPacket->bank<<" writing to physical address 0x" << hex << busPacket->physicalAddress<<dec<<":");
                        busPacket->printData();
                        PRINT("");
                }
        }
}
 }}

      - name: /*********************************************************************************
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








#ifndef BANK_H
#define BANK_H

//Bank.h
//
//Header file for bank class
//

#include "SystemConfiguration.h"
#include "SimulatorObject.h"
#include "BankState.h"
#include "BusPacket.h"
#include <iostream>

namespace DRAMSim
{
class Bank
{
        typedef struct _DataStruct
        {
                unsigned row;
                void *data;
                struct _DataStruct *next;
        } DataStruct;

public:
        //functions
        Bank(ostream &dramsim_log_);
        void read(BusPacket *busPacket);
        void write(const BusPacket *busPacket);

        //fields
        BankState currentState;

private:
        // private member
        std::vector<DataStruct *> rowEntries;
        ostream &dramsim_log; 

        static DataStruct *searchForRow(unsigned row, DataStruct *head);
};
}

#endif

        uses: #
# UAVCAN DSDL compiler for libuavcan
#
# Copyright (C) 2014 
#

'''
This module implements the core functionality of the UAVCAN DSDL compiler for libuavcan.
Supported Python versions: 3.2+, 2.7.
It accepts a list of root namespaces and produces the set of C++ header files for libuavcan.
It is based on the DSDL parsing package from pyuavcan.
'''

from __future__ import division, absolute_import, print_function, unicode_literals
import sys, os, logging, errno, re
from .pyratemp import Template
from uavcan import dsdl

# Python 2.7 compatibility
try:
    str = unicode
except NameError:
    pass

OUTPUT_FILE_EXTENSION = 'hpp'
OUTPUT_FILE_PERMISSIONS = 0o444  # Read only for all
TEMPLATE_FILENAME = os.path.join(os.path.dirname(__file__), 'data_type_template.tmpl')

__all__ = ['run', 'logger', 'DsdlCompilerException']

class DsdlCompilerException(Exception):
    pass

logger = logging.getLogger(__name__)

def run(source_dirs, include_dirs, output_dir):
    '''
    This function takes a list of root namespace directories (containing DSDL definition files to parse), a
    possibly empty list of search directories (containing DSDL definition files that can be referenced from the types
    that are going to be parsed), and the output directory path (possibly nonexistent) where the generated C++
    header files will be stored.
    
    Note that this module features lazy write, i.e. if an output file does already exist and its content is not going
    to change, it will not be overwritten. This feature allows to avoid unnecessary recompilation of dependent object
    files.
    
    Args:
        source_dirs    List of root namespace directories to parse.
        include_dirs   List of root namespace directories with referenced types (possibly empty). This list is
                       automaitcally extended with source_dirs.
        output_dir     Output directory path. Will be created if doesn't exist.
    '''
    assert isinstance(source_dirs, list)
    assert isinstance(include_dirs, list)
    output_dir = str(output_dir)

    types = run_parser(source_dirs, include_dirs + source_dirs)
    if not types:
        die('No type definitions were found')

    logger.info('%d types total', len(types))
    run_generator(types, output_dir)

# -----------------

def pretty_filename(filename):
    try:
        a = os.path.abspath(filename)
        r = os.path.relpath(filename)
        return a if '..' in r else r
    except ValueError:
        return filename

def type_output_filename(t):
    assert t.category == t.CATEGORY_COMPOUND
    return t.full_name.replace('.', os.path.sep) + '.' + OUTPUT_FILE_EXTENSION

def makedirs(path):
    try:
        try:
            os.makedirs(path, exist_ok=True)  # May throw "File exists" when executed as root, which is wrong
        except TypeError:
            os.makedirs(path)  # Python 2.7 compatibility
    except OSError as ex:
        if ex.errno != errno.EEXIST:  # http://stackoverflow.com/questions/12468022
            raise

def die(text):
    raise DsdlCompilerException(str(text))

def run_parser(source_dirs, search_dirs):
    try:
        types = dsdl.parse_namespaces(source_dirs, search_dirs)
    except dsdl.DsdlException as ex:
        logger.info('Parser failure', exc_info=True)
        die(ex)
    return types

def run_generator(types, dest_dir):
    try:
        template_expander = make_template_expander(TEMPLATE_FILENAME)
        dest_dir = os.path.abspath(dest_dir)  # Removing '..'
        makedirs(dest_dir)
        for t in types:
            logger.info('Generating type %s', t.full_name)
            filename = os.path.join(dest_dir, type_output_filename(t))
            text = generate_one_type(template_expander, t)
            write_generated_data(filename, text)
    except Exception as ex:
        logger.info('Generator failure', exc_info=True)
        die(ex)

def write_generated_data(filename, data):
    dirname = os.path.dirname(filename)
    makedirs(dirname)

    # Lazy update - file will not be rewritten if its content is not going to change
    if os.path.exists(filename):
        with open(filename) as f:
            existing_data = f.read()
        if data == existing_data:
            logger.info('Up to date [%s]', pretty_filename(filename))
            return
        logger.info('Rewriting [%s]', pretty_filename(filename))
        os.remove(filename)
    else:
        logger.info('Creating [%s]', pretty_filename(filename))

    # Full rewrite
    with open(filename, 'w') as f:
        f.write(data)
    try:
        os.chmod(filename, OUTPUT_FILE_PERMISSIONS)
    except (OSError, IOError) as ex:
        logger.warning('Failed to set permissions for %s: %s', pretty_filename(filename), ex)

def type_to_cpp_type(t):
    if t.category == t.CATEGORY_PRIMITIVE:
        cast_mode = {
            t.CAST_MODE_SATURATED: '::uavcan::CastModeSaturate',
            t.CAST_MODE_TRUNCATED: '::uavcan::CastModeTruncate',
        }[t.cast_mode]
        if t.kind == t.KIND_FLOAT:
            return '::uavcan::FloatSpec< %d, %s >' % (t.bitlen, cast_mode)
        else:
            signedness = {
                t.KIND_BOOLEAN: '::uavcan::SignednessUnsigned',
                t.KIND_UNSIGNED_INT: '::uavcan::SignednessUnsigned',
                t.KIND_SIGNED_INT: '::uavcan::SignednessSigned',
            }[t.kind]
            return '::uavcan::IntegerSpec< %d, %s, %s >' % (t.bitlen, signedness, cast_mode)
    elif t.category == t.CATEGORY_ARRAY:
        value_type = type_to_cpp_type(t.value_type)
        mode = {
            t.MODE_STATIC: '::uavcan::ArrayModeStatic',
            t.MODE_DYNAMIC: '::uavcan::ArrayModeDynamic',
        }[t.mode]
        return '::uavcan::Array< %s, %s, %d >' % (value_type, mode, t.max_size)
    elif t.category == t.CATEGORY_COMPOUND:
        return '::' + t.full_name.replace('.', '::')
    elif t.category == t.CATEGORY_VOID:
        return '::uavcan::IntegerSpec< %d, ::uavcan::SignednessUnsigned, ::uavcan::CastModeSaturate >' % t.bitlen
    else:
        raise DsdlCompilerException('Unknown type category: %s' % t.category)

def generate_one_type(template_expander, t):
    t.short_name = t.full_name.split('.')[-1]
    t.cpp_type_name = t.short_name + '_'
    t.cpp_full_type_name = '::' + t.full_name.replace('.', '::')
    t.include_guard = t.full_name.replace('.', '_').upper() + '_HPP_INCLUDED'

    # Dependencies (no duplicates)
    def fields_includes(fields):
        def detect_include(t):
            if t.category == t.CATEGORY_COMPOUND:
                return type_output_filename(t)
            if t.category == t.CATEGORY_ARRAY:
                return detect_include(t.value_type)
        return list(sorted(set(filter(None, [detect_include(x.type) for x in fields]))))

    if t.kind == t.KIND_MESSAGE:
        t.cpp_includes = fields_includes(t.fields)
    else:
        t.cpp_includes = fields_includes(t.request_fields + t.response_fields)

    t.cpp_namespace_components = t.full_name.split('.')[:-1]
    t.has_default_dtid = t.default_dtid is not None

    # Attribute types
    def inject_cpp_types(attributes):
        void_index = 0
        for a in attributes:
            a.cpp_type = type_to_cpp_type(a.type)
            a.void = a.type.category == a.type.CATEGORY_VOID
            if a.void:
                assert not a.name
                a.name = '_void_%d' % void_index
                void_index += 1

    if t.kind == t.KIND_MESSAGE:
        inject_cpp_types(t.fields)
        inject_cpp_types(t.constants)
        t.all_attributes = t.fields + t.constants
        t.union = t.union and len(t.fields)
    else:
        inject_cpp_types(t.request_fields)
        inject_cpp_types(t.request_constants)
        inject_cpp_types(t.response_fields)
        inject_cpp_types(t.response_constants)
        t.all_attributes = t.request_fields + t.request_constants + t.response_fields + t.response_constants
        t.request_union = t.request_union and len(t.request_fields)
        t.response_union = t.response_union and len(t.response_fields)

    # Constant properties
    def inject_constant_info(constants):
        for c in constants:
            if c.type.kind == c.type.KIND_FLOAT:
                float(c.string_value)  # Making sure that this is a valid float literal
                c.cpp_value = c.string_value
            else:
                int(c.string_value)  # Making sure that this is a valid integer literal
                c.cpp_value = c.string_value
                if c.type.kind == c.type.KIND_UNSIGNED_INT:
                    c.cpp_value += 'U'

    if t.kind == t.KIND_MESSAGE:
        inject_constant_info(t.constants)
    else:
        inject_constant_info(t.request_constants)
        inject_constant_info(t.response_constants)

    # Data type kind
    t.cpp_kind = {
        t.KIND_MESSAGE: '::uavcan::DataTypeKindMessage',
        t.KIND_SERVICE: '::uavcan::DataTypeKindService',
    }[t.kind]

    # Generation
    text = template_expander(t=t)  # t for Type
    text = '\n'.join(x.rstrip() for x in text.splitlines())
    text = text.replace('\n\n\n\n\n', '\n\n').replace('\n\n\n\n', '\n\n').replace('\n\n\n', '\n\n')
    text = text.replace('{\n\n ', '{\n ')
    return text

def make_template_expander(filename):
    '''
    Templating is based on pyratemp (http://www.simple-is-better.org/template/pyratemp.html).
    The pyratemp's syntax is rather verbose and not so human friendly, so we define some
    custom extensions to make it easier to read and write.
    The resulting syntax somewhat resembles Mako (which was used earlier instead of pyratemp):
        Substitution:
            ${expression}
        Line joining through backslash (replaced with a single space):
            ${foo(bar(very_long_arument=42, \
                      second_line=72))}
        Blocks:
            % for a in range(10):
                % if a == 5:
                    ${foo()}
                % endif
            % endfor
    The extended syntax is converted into pyratemp's through regexp substitution.
    '''
    with open(filename) as f:
        template_text = f.read()

    # Backslash-newline elimination
    template_text = re.sub(r'\\\r{0,1}\n\ *', r' ', template_text)

    # Substitution syntax transformation: ${foo} ==> $!foo!$
    template_text = re.sub(r'([^\$]{0,1})\$\{([^\}]+)\}', r'\1$!\2!$', template_text)

    # Flow control expression transformation: % foo: ==> <!--(foo)-->
    template_text = re.sub(r'(?m)^(\ *)\%\ *(.+?):{0,1}$', r'\1<!--(\2)-->', template_text)

    # Block termination transformation: <!--(endfoo)--> ==> <!--(end)-->
    template_text = re.sub(r'\<\!--\(end[a-z]+\)--\>', r'<!--(end)-->', template_text)

    # Pyratemp workaround.
    # The problem is that if there's no empty line after a macro declaration, first line will be doubly indented.
    # Workaround:
    #  1. Remove trailing comments
    #  2. Add a newline after each macro declaration
    template_text = re.sub(r'\ *\#\!.*', '', template_text)
    template_text = re.sub(r'(\<\!--\(macro\ [a-zA-Z0-9_]+\)--\>.*?)', r'\1\n', template_text)

    # Preprocessed text output for debugging
#   with open(filename + '.d', 'w') as f:
#       f.write(template_text)

    template = Template(template_text)

    def expand(**args):
        # This function adds one indentation level (4 spaces); it will be used from the template
        args['indent'] = lambda text, idnt = '    ': idnt + text.replace('\n', '\n' + idnt)
        # This function works like enumerate(), telling you whether the current item is the last one
        def enum_last_value(iterable, start=0):
            it = iter(iterable)
            count = start
            last = next(it)
            for val in it:
                yield count, False, last
                last = val
                count += 1
            yield count, True, last
        args['enum_last_value'] = enum_last_value
        return template(**args)

    return expand
        # Provide a unique ID to access the sarif output path
        id: /*
 * UAVCAN data structure definition for libuavcan.
 *
 * Autogenerated, do not edit.
 *
 * Source file: ${t.source_file}
 */

#ifndef ${t.include_guard}
#define ${t.include_guard}

#include <uavcan/build_config.hpp>
#include <uavcan/node/global_data_type_registry.hpp>
#include <uavcan/marshal/types.hpp>

% for inc in t.cpp_includes:
#include <${inc}>
% endfor

/******************************* Source text **********************************
% for line in t.source_text.strip().splitlines():
${line}
% endfor
******************************************************************************/

/********************* DSDL signature source definition ***********************
% for line in t.get_dsdl_signature_source_definition().splitlines():
${line}
% endfor
******************************************************************************/

% for a in t.all_attributes:
#undef ${a.name}
% endfor

% for nsc in t.cpp_namespace_components:
namespace ${nsc}
{
% endfor

% if t.kind != t.KIND_SERVICE:
template <int _tmpl>
% endif
struct UAVCAN_EXPORT ${t.cpp_type_name}
{
<!--(macro generate_primary_body)--> #! type_name, max_bitlen, fields, constants, union
    typedef const ${type_name}<_tmpl>& ParameterType;
    typedef ${type_name}<_tmpl>& ReferenceType;

    <!--(macro expand_attr_types)--> #! group_name, attrs
    struct ${group_name}
    {
        % for a in attrs:
        typedef ${a.cpp_type} ${a.name};
        % endfor
    };
    <!--(end)-->
    ${expand_attr_types(group_name='ConstantTypes', attrs=constants)}
    ${expand_attr_types(group_name='FieldTypes', attrs=fields)}

    % if union:

    struct Tag
    {
        enum Type
        {
        % for idx,last,a in enum_last_value(fields):
            ${a.name}${',' if not last else ''}
        % endfor
        };
    };

    typedef ::uavcan::IntegerSpec< ::uavcan::IntegerBitLen< ${len(fields)} >::Result,
                                   ::uavcan::SignednessUnsigned, ::uavcan::CastModeTruncate > TagType;

        <!--(macro expand_enum_per_field)--> #! enum_name, enum_comparator
    enum
    {
        ${enum_name} = TagType::BitLen +
            % for idx,last,a in enum_last_value(fields):
                % if not last:
            ::uavcan::${enum_comparator}<FieldTypes::${a.name}::${enum_name},
                % else:
                FieldTypes::${a.name}::${enum_name} ${'>::Result' * (len(fields) - 1)}
                % endif
            % endfor
    };
        <!--(end)-->

    ${expand_enum_per_field(enum_name='MinBitLen', enum_comparator='EnumMin')}
    ${expand_enum_per_field(enum_name='MaxBitLen', enum_comparator='EnumMax')}

    % else:

        <!--(macro expand_enum_per_field)--> #! enum_name
    enum
    {
        ${enum_name}
            % for idx,a in enumerate(fields):
            ${'=' if idx == 0 else '+'} FieldTypes::${a.name}::${enum_name}
            % endfor
    };
        <!--(end)-->

    ${expand_enum_per_field(enum_name='MinBitLen')}
    ${expand_enum_per_field(enum_name='MaxBitLen')}

    % endif

    // Constants
    % for a in constants:
    static const typename ::uavcan::StorageType< typename ConstantTypes::${a.name} >::Type ${a.name}; // ${a.init_expression}
    % endfor

    // Fields
    % for a in [x for x in fields if not x.void]:
    typename ::uavcan::StorageType< typename FieldTypes::${a.name} >::Type ${a.name};
    % endfor

    % if union:
private:
    typename ::uavcan::StorageType< TagType >::Type _tag_;  // The name is mangled to avoid clashing with fields

    template <typename Tag::Type T>
    struct TagToType;

public:
    % endif

    ${type_name}()
    % for idx,a in enumerate([x for x in fields if not x.void]):
        ${':' if idx == 0 else ','} ${a.name}()
    % endfor
    % if union:
        , _tag_()
    % endif
    {
        ::uavcan::StaticAssert<_tmpl == 0>::check();  // Usage check

#if UAVCAN_DEBUG
        /*
         * Cross-checking MaxBitLen provided by the DSDL compiler.
         * This check shall never be performed in user code because MaxBitLen value
         * actually depends on the nested types, thus it is not invariant.
         */
        ::uavcan::StaticAssert<${max_bitlen} == MaxBitLen>::check();
#endif
    }

    bool operator==(ParameterType rhs) const;
    bool operator!=(ParameterType rhs) const { return !operator==(rhs); }

    /**
     * This comparison is based on @ref uavcan::areClose(), which ensures proper comparison of
     * floating point fields at any depth.
     */
    bool isClose(ParameterType rhs) const;

    static int encode(ParameterType self, ::uavcan::ScalarCodec& codec,
                      ::uavcan::TailArrayOptimizationMode tao_mode = ::uavcan::TailArrayOptEnabled);

    static int decode(ReferenceType self, ::uavcan::ScalarCodec& codec,
                      ::uavcan::TailArrayOptimizationMode tao_mode = ::uavcan::TailArrayOptEnabled);

    % if union:
    /**
     * Explicit access to the tag.
     * It is safer to use is()/as()/to() instead.
     */
    typename Tag::Type getTag() const { return typename Tag::Type(_tag_); }
    void setTag(typename Tag::Type x) { _tag_ = typename ::uavcan::StorageType< TagType >::Type(x); }

    /**
     * Whether the union is set to the given type.
     * Access by tag; this will work even if there are non-unique types within the union.
     */
    bool is(typename Tag::Type x) const { return typename Tag::Type(_tag_) == x; }

    /**
     * If the union is currently set to the type T, returns pointer to the appropriate field.
     * If the union is set to another type, returns null pointer.
     */
    template <typename Tag::Type T>
    inline const typename TagToType<T>::StorageType* as() const;

    /**
     * Switches the union to the given type and returns a mutable reference to the appropriate field.
     * If the previous type was different, a default constructor will be called first.
     */
    template <typename Tag::Type T>
    inline typename TagToType<T>::StorageType& to();
    % endif
<!--(end)-->

% if t.kind == t.KIND_SERVICE:
    template <int _tmpl>
    struct Request_
    {
        ${indent(generate_primary_body(type_name='Request_', max_bitlen=t.get_max_bitlen_request(), \
                                       fields=t.request_fields, constants=t.request_constants, \
                                       union=t.request_union))}
    };

    template <int _tmpl>
    struct Response_
    {
        ${indent(generate_primary_body(type_name='Response_', max_bitlen=t.get_max_bitlen_response(), \
                                       fields=t.response_fields, constants=t.response_constants, \
                                       union=t.response_union))}
    };

    typedef Request_<0> Request;
    typedef Response_<0> Response;
% else:
    ${generate_primary_body(type_name=t.cpp_type_name, max_bitlen=t.get_max_bitlen(), \
                            fields=t.fields, constants=t.constants, union=t.union)}
% endif

    /*
     * Static type info
     */
    enum { DataTypeKind = ${t.cpp_kind} };
% if t.has_default_dtid:
    enum { DefaultDataTypeID = ${t.default_dtid} };
% else:
    // This type has no default data type ID
% endif

    static const char* getDataTypeFullName()
    {
        return "${t.full_name}";
    }

    static void extendDataTypeSignature(::uavcan::DataTypeSignature& signature)
    {
        signature.extend(getDataTypeSignature());
    }

    static ::uavcan::DataTypeSignature getDataTypeSignature();

% if t.kind == t.KIND_SERVICE:
private:
    ${t.cpp_type_name}(); // Don't create objects of this type. Use Request/Response instead.
% endif
};

/*
 * Out of line struct method definitions
 */
<!--(macro define_out_of_line_struct_methods)--> #! scope_prefix, fields, union

template <int _tmpl>
bool ${scope_prefix}<_tmpl>::operator==(ParameterType rhs) const
{
    % if union:
    if (_tag_ != rhs._tag_)
    {
        return false;
    }
        % for idx,a in enumerate(fields):
    if (_tag_ == ${idx})
    {
        return ${a.name} == rhs.${a.name};
    }
        % endfor
    UAVCAN_ASSERT(0);   // Invalid tag
    return false;
    % else:
        % if fields:
    return
            % for idx,last,a in enum_last_value([x for x in fields if not x.void]):
        ${a.name} == rhs.${a.name}${' &&' if not last else ';'}
            % endfor
        % else:
    (void)rhs;
    return true;
        % endif
    % endif
}

template <int _tmpl>
bool ${scope_prefix}<_tmpl>::isClose(ParameterType rhs) const
{
    % if union:
    if (_tag_ != rhs._tag_)
    {
        return false;
    }
        % for idx,a in enumerate(fields):
    if (_tag_ == ${idx})
    {
        return ::uavcan::areClose(${a.name}, rhs.${a.name});
    }
        % endfor
    UAVCAN_ASSERT(0);   // Invalid tag
    return false;
    % else:
        % if fields:
    return
            % for idx,last,a in enum_last_value([x for x in fields if not x.void]):
        ::uavcan::areClose(${a.name}, rhs.${a.name})${' &&' if not last else ';'}
            % endfor
        % else:
    (void)rhs;
    return true;
        % endif
    % endif
}

    <!--(macro generate_codec_calls_per_field)--> #! call_name, self_parameter_type
template <int _tmpl>
int ${scope_prefix}<_tmpl>::${call_name}(${self_parameter_type} self, ::uavcan::ScalarCodec& codec,
    ::uavcan::TailArrayOptimizationMode tao_mode)
{
    (void)self;
    (void)codec;
    (void)tao_mode;
        % if union:
    const int res = TagType::${call_name}(self._tag_, codec, ::uavcan::TailArrayOptDisabled);
    if (res <= 0)
    {
        return res;
    }
            % for idx,a in enumerate(fields):
    if (self._tag_ == ${idx})
    {
        return FieldTypes::${a.name}::${call_name}(self.${a.name}, codec, tao_mode);
    }
            % endfor
    return -1;          // Invalid tag value
        % else:
            % for a in [x for x in fields if x.void]:
    typename ::uavcan::StorageType< typename FieldTypes::${a.name} >::Type ${a.name} = 0;
            % endfor
    int res = 1;
            % for idx,last,a in enum_last_value(fields):
    res = FieldTypes::${a.name}::${call_name}(${'self.' * (not a.void)}${a.name}, codec, \
${'::uavcan::TailArrayOptDisabled' if not last else 'tao_mode'});
                % if not last:
    if (res <= 0)
    {
        return res;
    }
                % endif
            % endfor
    return res;
        % endif
}
    <!--(end)-->
${generate_codec_calls_per_field(call_name='encode', self_parameter_type='ParameterType')}
${generate_codec_calls_per_field(call_name='decode', self_parameter_type='ReferenceType')}

    % if union:
        % for idx,a in enumerate(fields):
template <>
template <>
struct ${scope_prefix}<0>::TagToType<${scope_prefix}<0>::Tag::${a.name}>
{
    typedef typename ${scope_prefix}<0>::FieldTypes::${a.name} Type;
    typedef typename ::uavcan::StorageType<Type>::Type StorageType;
};

template <>
template <>
inline const typename ${scope_prefix}<0>::TagToType< ${scope_prefix}<0>::Tag::${a.name} >::StorageType*
${scope_prefix}<0>::as< ${scope_prefix}<0>::Tag::${a.name} >() const
{
    return is(${scope_prefix}<0>::Tag::${a.name}) ? &${a.name} : UAVCAN_NULLPTR;
}

template <>
template <>
inline typename ${scope_prefix}<0>::TagToType< ${scope_prefix}<0>::Tag::${a.name} >::StorageType&
${scope_prefix}<0>::to< ${scope_prefix}<0>::Tag::${a.name} >()
{
    if (_tag_ != ${idx})
    {
        _tag_ = ${idx};
        ${a.name} = typename TagToType< ${scope_prefix}<0>::Tag::${a.name} >::StorageType();
    }
    return ${a.name};
}

        % endfor
    % endif
<!--(end)-->

% if t.kind == t.KIND_SERVICE:
${define_out_of_line_struct_methods(scope_prefix=t.cpp_type_name + '::Request_', fields=t.request_fields, \
                                    union=t.request_union)}
${define_out_of_line_struct_methods(scope_prefix=t.cpp_type_name + '::Response_', fields=t.response_fields, \
                                    union=t.response_union)}
% else:
${define_out_of_line_struct_methods(scope_prefix=t.cpp_type_name, fields=t.fields, union=t.union)}
% endif

/*
 * Out of line type method definitions
 */
% if t.kind == t.KIND_SERVICE:
inline ::uavcan::DataTypeSignature ${t.cpp_type_name}::getDataTypeSignature()
% else:
template <int _tmpl>
::uavcan::DataTypeSignature ${t.cpp_type_name}<_tmpl>::getDataTypeSignature()
% endif
{
    ::uavcan::DataTypeSignature signature(${'0x%08X' % t.get_dsdl_signature()}ULL);
<!--(macro extend_signature_per_field)--> #! scope_prefix, fields
    % for a in fields:
    ${scope_prefix}FieldTypes::${a.name}::extendDataTypeSignature(signature);
    % endfor
<!--(end)-->
% if t.kind == t.KIND_SERVICE:
${extend_signature_per_field(scope_prefix='Request::', fields=t.request_fields)}
${extend_signature_per_field(scope_prefix='Response::', fields=t.response_fields)}
% else:
${extend_signature_per_field(scope_prefix='', fields=t.fields)}
% endif
    return signature;
}

/*
 * Out of line constant definitions
 */
<!--(macro define_out_of_line_constants)--> #! scope_prefix, constants
    % for a in constants:
template <int _tmpl>
const typename ::uavcan::StorageType< typename ${scope_prefix}<_tmpl>::ConstantTypes::${a.name} >::Type
    ${scope_prefix}<_tmpl>::${a.name} = ${a.cpp_value}; // ${a.init_expression}

    % endfor
<!--(end)-->
% if t.kind == t.KIND_SERVICE:
${define_out_of_line_constants(scope_prefix=t.cpp_type_name + '::Request_', constants=t.request_constants)}
${define_out_of_line_constants(scope_prefix=t.cpp_type_name + '::Response_', constants=t.response_constants)}
% else:
${define_out_of_line_constants(scope_prefix=t.cpp_type_name, constants=t.constants)}
% endif

/*
 * Final typedef
 */
% if t.kind == t.KIND_SERVICE:
typedef ${t.cpp_type_name} ${t.short_name};
% else:
typedef ${t.cpp_type_name}<0> ${t.short_name};
% endif

% if t.has_default_dtid:
namespace
{

const ::uavcan::DefaultDataTypeRegistrator< ${t.cpp_full_type_name} > _uavcan_gdtr_registrator_${t.short_name};

}
% else:
// No default registration
% endif

% for nsc in t.cpp_namespace_components[::-1]:
} // Namespace ${nsc}
% endfor

/*
 * YAML streamer specialization
 */
namespace uavcan
{

<!--(macro define_yaml_streamer)--> #! type_name, fields, union
template <>
class UAVCAN_EXPORT YamlStreamer< ${type_name} >
{
public:
    template <typename Stream>
    static void stream(Stream& s, ${type_name}::ParameterType obj, const int level);
};

template <typename Stream>
void YamlStreamer< ${type_name} >::stream(Stream& s, ${type_name}::ParameterType obj, const int level)
{
    (void)s;
    (void)obj;
    (void)level;
    % if union:
    if (level > 0)
    {
        s << '\n';
        for (int pos = 0; pos < level; pos++)
        {
            s << "  ";
        }
    }
        % for idx,a in enumerate(fields):
    if (static_cast<int>(obj.getTag()) == ${idx})
    {
        s << "${a.name}: ";
        YamlStreamer< ${type_name}::FieldTypes::${a.name} >::stream(s, obj.${a.name}, level + 1);
    }
        % endfor
    % else:
        % for idx,a in enumerate([x for x in fields if not x.void]):
            % if idx == 0:
    if (level > 0)
    {
        s << '\n';
        for (int pos = 0; pos < level; pos++)
        {
            s << "  ";
        }
    }
            % else:
    s << '\n';
    for (int pos = 0; pos < level; pos++)
    {
        s << "  ";
    }
            % endif
    s << "${a.name}: ";
    YamlStreamer< ${type_name}::FieldTypes::${a.name} >::stream(s, obj.${a.name}, level + 1);
        % endfor
    % endif
}
<!--(end)-->
% if t.kind == t.KIND_SERVICE:
${define_yaml_streamer(type_name=t.cpp_full_type_name + '::Request', fields=t.request_fields, union=t.request_union)}
${define_yaml_streamer(type_name=t.cpp_full_type_name + '::Response', fields=t.response_fields, union=t.response_union)}
% else:
${define_yaml_streamer(type_name=t.cpp_full_type_name, fields=t.fields, union=t.union)}
% endif

}

% for nsc in t.cpp_namespace_components:
namespace ${nsc}
{
% endfor

<!--(macro define_streaming_operator)--> #! type_name
template <typename Stream>
inline Stream& operator<<(Stream& s, ${type_name}::ParameterType obj)
{
    ::uavcan::YamlStreamer< ${type_name} >::stream(s, obj, 0);
    return s;
}
<!--(end)-->
% if t.kind == t.KIND_SERVICE:
${define_streaming_operator(type_name=t.cpp_full_type_name + '::Request')}
${define_streaming_operator(type_name=t.cpp_full_type_name + '::Response')}
% else:
${define_streaming_operator(type_name=t.cpp_full_type_name)}
% endif

% for nsc in t.cpp_namespace_components[::-1]:
} // Namespace ${nsc}
% endfor

#endif // ${t.include_guard}
        with: #!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Small, simple and powerful template-engine for Python.

A template-engine for Python, which is very simple, easy to use, small,
fast, powerful, modular, extensible, well documented and pythonic.

See documentation for a list of features, template-syntax etc.

:Version:   0.3.0
:Requires:  Python >=2.6 / 3.x

:Usage:
    see class ``Template`` and examples below.

:Example:

    Note that the examples are in Python 2; they also work in
    Python 3 if you replace u"..." by "...", unicode() by str()
    and partly "..." by b"...".

    quickstart::
        >>> t = Template("hello @!name!@")
        >>> print(t(name="marvin"))
        hello marvin

    quickstart with a template-file::
        # >>> t = Template(filename="mytemplate.tmpl")
        # >>> print(t(name="marvin"))
        # hello marvin

    generic usage::
        >>> t = Template(u"output is in Unicode \\xe4\\xf6\\xfc\\u20ac")
        >>> t                                           #doctest: +ELLIPSIS
        <...Template instance at 0x...>
        >>> t()
        u'output is in Unicode \\xe4\\xf6\\xfc\\u20ac'
        >>> unicode(t)
        u'output is in Unicode \\xe4\\xf6\\xfc\\u20ac'

    with data::
        >>> t = Template("hello @!name!@", data={"name":"world"})
        >>> t()
        u'hello world'
        >>> t(name="worlds")
        u'hello worlds'

        # >>> t(note="data must be Unicode or ASCII", name=u"\\xe4")
        # u'hello \\xe4'

    escaping::
        >>> t = Template("hello escaped: @!name!@, unescaped: $!name!$")
        >>> t(name='''<>&'"''')
        u'hello escaped: &lt;&gt;&amp;&#39;&quot;, unescaped: <>&\\'"'

    result-encoding::
        # encode the unicode-object to your encoding with encode()
        >>> t = Template(u"hello \\xe4\\xf6\\xfc\\u20ac")
        >>> result = t()
        >>> result
        u'hello \\xe4\\xf6\\xfc\\u20ac'
        >>> result.encode("utf-8")
        'hello \\xc3\\xa4\\xc3\\xb6\\xc3\\xbc\\xe2\\x82\\xac'
        >>> result.encode("ascii")
        Traceback (most recent call last):
          ...
        UnicodeEncodeError: 'ascii' codec can't encode characters in position 6-9: ordinal not in range(128)
        >>> result.encode("ascii", 'xmlcharrefreplace')
        'hello &#228;&#246;&#252;&#8364;'

    Python-expressions::
        >>> Template('formatted: @! "%8.5f" % value !@')(value=3.141592653)
        u'formatted:  3.14159'
        >>> Template("hello --@!name.upper().center(20)!@--")(name="world")
        u'hello --       WORLD        --'
        >>> Template("calculate @!var*5+7!@")(var=7)
        u'calculate 42'

    blocks (if/for/macros/...)::
        >>> t = Template("<!--(if foo == 1)-->bar<!--(elif foo == 2)-->baz<!--(else)-->unknown(@!foo!@)<!--(end)-->")
        >>> t(foo=2)
        u'baz'
        >>> t(foo=5)
        u'unknown(5)'

        >>> t = Template("<!--(for i in mylist)-->@!i!@ <!--(else)-->(empty)<!--(end)-->")
        >>> t(mylist=[])
        u'(empty)'
        >>> t(mylist=[1,2,3])
        u'1 2 3 '

        >>> t = Template("<!--(for i,elem in enumerate(mylist))--> - @!i!@: @!elem!@<!--(end)-->")
        >>> t(mylist=["a","b","c"])
        u' - 0: a - 1: b - 2: c'

        >>> t = Template('<!--(macro greetings)-->hello <strong>@!name!@</strong><!--(end)-->  @!greetings(name=user)!@')
        >>> t(user="monty")
        u'  hello <strong>monty</strong>'

    exists::
        >>> t = Template('<!--(if exists("foo"))-->YES<!--(else)-->NO<!--(end)-->')
        >>> t()
        u'NO'
        >>> t(foo=1)
        u'YES'
        >>> t(foo=None)       # note this difference to 'default()'
        u'YES'

    default-values::
        # non-existing variables raise an error
        >>> Template('hi @!optional!@')()
        Traceback (most recent call last):
          ...
        TemplateRenderError: Cannot eval expression 'optional'. (NameError: name 'optional' is not defined)

        >>> t = Template('hi @!default("optional","anyone")!@')
        >>> t()
        u'hi anyone'
        >>> t(optional=None)
        u'hi anyone'
        >>> t(optional="there")
        u'hi there'

        # the 1st parameter can be any eval-expression
        >>> t = Template('@!default("5*var1+var2","missing variable")!@')
        >>> t(var1=10)
        u'missing variable'
        >>> t(var1=10, var2=2)
        u'52'

        # also in blocks
        >>> t = Template('<!--(if default("opt1+opt2",0) > 0)-->yes<!--(else)-->no<!--(end)-->')
        >>> t()
        u'no'
        >>> t(opt1=23, opt2=42)
        u'yes'

        >>> t = Template('<!--(for i in default("optional_list",[]))-->@!i!@<!--(end)-->')
        >>> t()
        u''
        >>> t(optional_list=[1,2,3])
        u'123'


        # but make sure to put the expression in quotation marks, otherwise:
        >>> Template('@!default(optional,"fallback")!@')()
        Traceback (most recent call last):
          ...
        TemplateRenderError: Cannot eval expression 'default(optional,"fallback")'. (NameError: name 'optional' is not defined)

    setvar::
        >>> t = Template('$!setvar("i", "i+1")!$@!i!@')
        >>> t(i=6)
        u'7'

        >>> t = Template('''<!--(if isinstance(s, (list,tuple)))-->$!setvar("s", '"\\\\\\\\n".join(s)')!$<!--(end)-->@!s!@''')
        >>> t(isinstance=isinstance, s="123")
        u'123'
        >>> t(isinstance=isinstance, s=["123", "456"])
        u'123\\n456'

:Author:    Roland Koebler (rk at simple-is-better dot org)
:Copyright: Roland Koebler
:License:   MIT/X11-like, see __license__

:RCS:       $Id: pyratemp.py,v 1.12 2013/04/02 20:26:06 rk Exp $
"""
from __future__ import unicode_literals

__version__ = "0.3.0"
__author__   = "Roland Koebler <rk at simple-is-better dot org>"
__license__  = """Copyright (c) Roland Koebler, 2007-2013

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE."""

#=========================================

import os, re, sys
if sys.version_info[0] >= 3:
    import builtins
    unicode = str
    long = int
else:
    import __builtin__ as builtins
    from codecs import open

#=========================================
# some useful functions

#----------------------
# string-position: i <-> row,col

def srow(string, i):
    """Get line numer of ``string[i]`` in `string`.

    :Returns: row, starting at 1
    :Note:    This works for text-strings with ``\\n`` or ``\\r\\n``.
    """
    return string.count('\n', 0, max(0, i)) + 1

def scol(string, i):
    """Get column number of ``string[i]`` in `string`.

    :Returns: column, starting at 1 (but may be <1 if i<0)
    :Note:    This works for text-strings with ``\\n`` or ``\\r\\n``.
    """
    return i - string.rfind('\n', 0, max(0, i))

def sindex(string, row, col):
    """Get index of the character at `row`/`col` in `string`.

    :Parameters:
        - `row`: row number, starting at 1.
        - `col`: column number, starting at 1.
    :Returns:    ``i``, starting at 0 (but may be <1 if row/col<0)
    :Note:       This works for text-strings with '\\n' or '\\r\\n'.
    """
    n = 0
    for _ in range(row-1):
        n = string.find('\n', n) + 1
    return n+col-1

#----------------------

def dictkeyclean(d):
    """Convert all keys of the dict `d` to strings.
    """
    new_d = {}
    for k, v in d.items():
        new_d[str(k)] = v
    return new_d

#----------------------

def dummy(*_, **__):
    """Dummy function, doing nothing.
    """
    pass

def dummy_raise(exception, value):
    """Create an exception-raising dummy function.

    :Returns: dummy function, raising ``exception(value)``
    """
    def mydummy(*_, **__):
        raise exception(value)
    return mydummy

#=========================================
# escaping

(NONE, HTML, LATEX, MAIL_HEADER) = range(0, 4)
ESCAPE_SUPPORTED = {"NONE":None, "HTML":HTML, "LATEX":LATEX, "MAIL_HEADER":MAIL_HEADER}

def escape(s, format=HTML):
    """Replace special characters by their escape sequence.

    :Parameters:
        - `s`: unicode-string to escape
        - `format`:

          - `NONE`:  nothing is replaced
          - `HTML`:  replace &<>'" by &...;
          - `LATEX`: replace \#$%&_{}~^
          - `MAIL_HEADER`: escape non-ASCII mail-header-contents
    :Returns:
        the escaped string in unicode
    :Exceptions:
        - `ValueError`: if `format` is invalid.

    :Uses:
        MAIL_HEADER uses module email
    """
    #Note: If you have to make sure that every character gets replaced
    #      only once (and if you cannot achieve this with the following code),
    #      use something like "".join([replacedict.get(c,c) for c in s])
    #      which is about 2-3 times slower (but maybe needs less memory).
    #Note: This is one of the most time-consuming parts of the template.
    if format is None or format == NONE:
        pass
    elif format == HTML:
        s = s.replace("&", "&amp;") # must be done first!
        s = s.replace("<", "&lt;")
        s = s.replace(">", "&gt;")
        s = s.replace('"', "&quot;")
        s = s.replace("'", "&#39;")
    elif format == LATEX:
        s = s.replace("\\", "\\x")    #must be done first!
        s = s.replace("#",  "\\#")
        s = s.replace("$",  "\\$")
        s = s.replace("%",  "\\%")
        s = s.replace("&",  "\\&")
        s = s.replace("_",  "\\_")
        s = s.replace("{",  "\\{")
        s = s.replace("}",  "\\}")
        s = s.replace("\\x","\\textbackslash{}")
        s = s.replace("~",  "\\textasciitilde{}")
        s = s.replace("^",  "\\textasciicircum{}")
    elif format == MAIL_HEADER:
        import email.header
        try:
            s.encode("ascii")
            return s
        except UnicodeEncodeError:
            return email.header.make_header([(s, "utf-8")]).encode()
    else:
        raise ValueError('Invalid format (only None, HTML, LATEX and MAIL_HEADER are supported).')
    return s

#=========================================

#-----------------------------------------
# Exceptions

class TemplateException(Exception):
    """Base class for template-exceptions."""
    pass

class TemplateParseError(TemplateException):
    """Template parsing failed."""
    def __init__(self, err, errpos):
        """
        :Parameters:
            - `err`:    error-message or exception to wrap
            - `errpos`: ``(filename,row,col)`` where the error occured.
        """
        self.err = err
        self.filename, self.row, self.col = errpos
        TemplateException.__init__(self)
    def __str__(self):
        if not self.filename:
            return "line %d, col %d: %s" % (self.row, self.col, str(self.err))
        else:
            return "file %s, line %d, col %d: %s" % (self.filename, self.row, self.col, str(self.err))

class TemplateSyntaxError(TemplateParseError, SyntaxError):
    """Template syntax-error."""
    pass

class TemplateIncludeError(TemplateParseError):
    """Template 'include' failed."""
    pass

class TemplateRenderError(TemplateException):
    """Template rendering failed."""
    pass

#-----------------------------------------
# Loader

class LoaderString:
    """Load template from a string/unicode.

    Note that 'include' is not possible in such templates.
    """
    def __init__(self, encoding='utf-8'):
        self.encoding = encoding

    def load(self, s):
        """Return template-string as unicode.
        """
        if isinstance(s, unicode):
            u = s
        else:
            u = s.decode(self.encoding)
        return u

class LoaderFile:
    """Load template from a file.

    When loading a template from a file, it's possible to including other
    templates (by using 'include' in the template). But for simplicity
    and security, all included templates have to be in the same directory!
    (see ``allowed_path``)
    """
    def __init__(self, allowed_path=None, encoding='utf-8'):
        """Init the loader.

        :Parameters:
            - `allowed_path`: path of the template-files
            - `encoding`: encoding of the template-files
        :Exceptions:
            - `ValueError`: if `allowed_path` is not a directory
        """
        if allowed_path and not os.path.isdir(allowed_path):
            raise ValueError("'allowed_path' has to be a directory.")
        self.path     = allowed_path
        self.encoding = encoding

    def load(self, filename):
        """Load a template from a file.

        Check if filename is allowed and return its contens in unicode.

        :Parameters:
            - `filename`: filename of the template without path
        :Returns:
            the contents of the template-file in unicode
        :Exceptions:
            - `ValueError`: if `filename` contains a path
        """
        if filename != os.path.basename(filename):
            raise ValueError("No path allowed in filename. (%s)" %(filename))
        filename = os.path.join(self.path, filename)

        f = open(filename, 'r', encoding=self.encoding)
        u = f.read()
        f.close()

        return u

#-----------------------------------------
# Parser

class Parser(object):
    """Parse a template into a parse-tree.

    Includes a syntax-check, an optional expression-check and verbose
    error-messages.

    See documentation for a description of the parse-tree.
    """
    # template-syntax
    _comment_start = "#!"
    _comment_end   = "!#"
    _sub_start     = "$!"
    _sub_end       = "!$"
    _subesc_start  = "@!"
    _subesc_end    = "!@"
    _block_start   = "<!--("
    _block_end     = ")-->"

    # build regexps
    # comment
    #   single-line, until end-tag or end-of-line.
    _strComment = r"""%s(?P<content>.*?)(?P<end>%s|\n|$)""" \
                    % (re.escape(_comment_start), re.escape(_comment_end))
    _reComment = re.compile(_strComment, re.M)

    # escaped or unescaped substitution
    #   single-line ("|$" is needed to be able to generate good error-messges)
    _strSubstitution = r"""
                    (
                    %s\s*(?P<sub>.*?)\s*(?P<end>%s|$)       #substitution
                    |
                    %s\s*(?P<escsub>.*?)\s*(?P<escend>%s|$) #escaped substitution
                    )
                """ % (re.escape(_sub_start),    re.escape(_sub_end),
                       re.escape(_subesc_start), re.escape(_subesc_end))
    _reSubstitution = re.compile(_strSubstitution, re.X|re.M)

    # block
    #   - single-line, no nesting.
    #   or
    #   - multi-line, nested by whitespace indentation:
    #       * start- and end-tag of a block must have exactly the same indentation.
    #       * start- and end-tags of *nested* blocks should have a greater indentation.
    # NOTE: A single-line block must not start at beginning of the line with
    #       the same indentation as the enclosing multi-line blocks!
    #       Note that "       " and "\t" are different, although they may
    #       look the same in an editor!
    _s = re.escape(_block_start)
    _e = re.escape(_block_end)
    _strBlock = r"""
                    ^(?P<mEnd>[ \t]*)%send%s(?P<meIgnored>.*)\r?\n?   # multi-line end  (^   <!--(end)-->IGNORED_TEXT\n)
                    |
                    (?P<sEnd>)%send%s                               # single-line end (<!--(end)-->)
                    |
                    (?P<sSpace>[ \t]*)                              # single-line tag (no nesting)
                    %s(?P<sKeyw>\w+)[ \t]*(?P<sParam>.*?)%s
                    (?P<sContent>.*?)
                    (?=(?:%s.*?%s.*?)??%send%s)                     # (match until end or i.e. <!--(elif/else...)-->)
                    |
                                                                    # multi-line tag, nested by whitespace indentation
                    ^(?P<indent>[ \t]*)                             #   save indentation of start tag
                    %s(?P<mKeyw>\w+)\s*(?P<mParam>.*?)%s(?P<mIgnored>.*)\r?\n
                    (?P<mContent>(?:.*\n)*?)
                    (?=(?P=indent)%s(?:.|\s)*?%s)                   #   match indentation
                """ % (_s, _e,
                       _s, _e,
                       _s, _e, _s, _e, _s, _e,
                       _s, _e, _s, _e)
    _reBlock = re.compile(_strBlock, re.X|re.M)

    # "for"-block parameters: "var(,var)* in ..."
    _strForParam = r"""^(?P<names>\w+(?:\s*,\s*\w+)*)\s+in\s+(?P<iter>.+)$"""
    _reForParam  = re.compile(_strForParam)

    # allowed macro-names
    _reMacroParam = re.compile(r"""^\w+$""")


    def __init__(self, loadfunc=None, testexpr=None, escape=HTML):
        """Init the parser.

        :Parameters:
            - `loadfunc`: function to load included templates
              (i.e. ``LoaderFile(...).load``)
            - `testexpr`: function to test if a template-expressions is valid
              (i.e. ``EvalPseudoSandbox().compile``)
            - `escape`:   default-escaping (may be modified by the template)
        :Exceptions:
            - `ValueError`: if `testexpr` or `escape` is invalid.
        """
        if loadfunc is None:
            self._load = dummy_raise(NotImplementedError, "'include' not supported, since no 'loadfunc' was given.")
        else:
            self._load = loadfunc

        if testexpr is None:
            self._testexprfunc = dummy
        else:
            try:    # test if testexpr() works
                testexpr("i==1")
            except Exception as err:
                raise ValueError("Invalid 'testexpr'. (%s)" %(err))
            self._testexprfunc = testexpr

        if escape not in ESCAPE_SUPPORTED.values():
            raise ValueError("Unsupported 'escape'. (%s)" %(escape))
        self.escape = escape
        self._includestack = []

    def parse(self, template):
        """Parse a template.

        :Parameters:
            - `template`: template-unicode-string
        :Returns:         the resulting parse-tree
        :Exceptions:
            - `TemplateSyntaxError`: for template-syntax-errors
            - `TemplateIncludeError`: if template-inclusion failed
            - `TemplateException`
        """
        self._includestack = [(None, template)]   # for error-messages (_errpos)
        return self._parse(template)

    def _errpos(self, fpos):
        """Convert `fpos` to ``(filename,row,column)`` for error-messages."""
        filename, string = self._includestack[-1]
        return filename, srow(string, fpos), scol(string, fpos)

    def _testexpr(self, expr,  fpos=0):
        """Test a template-expression to detect errors."""
        try:
            self._testexprfunc(expr)
        except SyntaxError as err:
            raise TemplateSyntaxError(err, self._errpos(fpos))

    def _parse_sub(self, parsetree, text, fpos=0):
        """Parse substitutions, and append them to the parse-tree.

        Additionally, remove comments.
        """
        curr = 0
        for match in self._reSubstitution.finditer(text):
            start = match.start()
            if start > curr:
                parsetree.append(("str", self._reComment.sub('', text[curr:start])))

            if match.group("sub") is not None:
                if not match.group("end"):
                    raise TemplateSyntaxError("Missing closing tag '%s' for '%s'."
                            % (self._sub_end, match.group()), self._errpos(fpos+start))
                if len(match.group("sub")) > 0:
                    self._testexpr(match.group("sub"), fpos+start)
                    parsetree.append(("sub", match.group("sub")))
            else:
                assert(match.group("escsub") is not None)
                if not match.group("escend"):
                    raise TemplateSyntaxError("Missing closing tag '%s' for '%s'."
                            % (self._subesc_end, match.group()), self._errpos(fpos+start))
                if len(match.group("escsub")) > 0:
                    self._testexpr(match.group("escsub"), fpos+start)
                    parsetree.append(("esc", self.escape, match.group("escsub")))

            curr = match.end()

        if len(text) > curr:
            parsetree.append(("str", self._reComment.sub('', text[curr:])))

    def _parse(self, template, fpos=0):
        """Recursive part of `parse()`.

        :Parameters:
            - template
            - fpos: position of ``template`` in the complete template (for error-messages)
        """
        # blank out comments
        # (So that its content does not collide with other syntax, and
        #  because removing them completely would falsify the character-
        #  position ("match.start()") of error-messages)
        template = self._reComment.sub(lambda match: self._comment_start+" "*len(match.group(1))+match.group(2), template)

        # init parser
        parsetree = []
        curr = 0            # current position (= end of previous block)
        block_type = None   # block type: if,for,macro,raw,...
        block_indent = None # None: single-line, >=0: multi-line

        # find blocks
        for match in self._reBlock.finditer(template):
            start = match.start()
            # process template-part before this block
            if start > curr:
                self._parse_sub(parsetree, template[curr:start], fpos)

            # analyze block syntax (incl. error-checking and -messages)
            keyword = None
            block = match.groupdict()
            pos__ = fpos + start                # shortcut
            if   block["sKeyw"] is not None:    # single-line block tag
                block_indent = None
                keyword = block["sKeyw"]
                param   = block["sParam"]
                content = block["sContent"]
                if block["sSpace"]:             # restore spaces before start-tag
                    if len(parsetree) > 0 and parsetree[-1][0] == "str":
                        parsetree[-1] = ("str", parsetree[-1][1] + block["sSpace"])
                    else:
                        parsetree.append(("str", block["sSpace"]))
                pos_p = fpos + match.start("sParam")    # shortcuts
                pos_c = fpos + match.start("sContent")
            elif block["mKeyw"] is not None:    # multi-line block tag
                block_indent = len(block["indent"])
                keyword = block["mKeyw"]
                param   = block["mParam"]
                content = block["mContent"]
                pos_p = fpos + match.start("mParam")
                pos_c = fpos + match.start("mContent")
                ignored = block["mIgnored"].strip()
                if ignored  and  ignored != self._comment_start:
                    raise TemplateSyntaxError("No code allowed after block-tag.", self._errpos(fpos+match.start("mIgnored")))
            elif block["mEnd"] is not None:     # multi-line block end
                if block_type is None:
                    raise TemplateSyntaxError("No block to end here/invalid indent.", self._errpos(pos__) )
                if block_indent != len(block["mEnd"]):
                    raise TemplateSyntaxError("Invalid indent for end-tag.", self._errpos(pos__) )
                ignored = block["meIgnored"].strip()
                if ignored  and  ignored != self._comment_start:
                    raise TemplateSyntaxError("No code allowed after end-tag.", self._errpos(fpos+match.start("meIgnored")))
                block_type = None
            elif block["sEnd"] is not None:     # single-line block end
                if block_type is None:
                    raise TemplateSyntaxError("No block to end here/invalid indent.", self._errpos(pos__))
                if block_indent is not None:
                    raise TemplateSyntaxError("Invalid indent for end-tag.", self._errpos(pos__))
                block_type = None
            else:
                raise TemplateException("FATAL: Block regexp error. Please contact the author. (%s)" % match.group())

            # analyze block content (mainly error-checking and -messages)
            if keyword:
                keyword = keyword.lower()
                if   'for'   == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'for'
                    cond = self._reForParam.match(param)
                    if cond is None:
                        raise TemplateSyntaxError("Invalid 'for ...' at '%s'." %(param), self._errpos(pos_p))
                    names = tuple(n.strip()  for n in cond.group("names").split(","))
                    self._testexpr(cond.group("iter"), pos_p+cond.start("iter"))
                    parsetree.append(("for", names, cond.group("iter"), self._parse(content, pos_c)))
                elif 'if'    == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block at '%s'." %(match.group()), self._errpos(pos__))
                    if not param:
                        raise TemplateSyntaxError("Missing condition for 'if' at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'if'
                    self._testexpr(param, pos_p)
                    parsetree.append(("if", param, self._parse(content, pos_c)))
                elif 'elif'  == keyword:
                    if block_type != 'if':
                        raise TemplateSyntaxError("'elif' may only appear after 'if' at '%s'." %(match.group()), self._errpos(pos__))
                    if not param:
                        raise TemplateSyntaxError("Missing condition for 'elif' at '%s'." %(match.group()), self._errpos(pos__))
                    self._testexpr(param, pos_p)
                    parsetree.append(("elif", param, self._parse(content, pos_c)))
                elif 'else'  == keyword:
                    if block_type not in ('if', 'for'):
                        raise TemplateSyntaxError("'else' may only appear after 'if' or 'for' at '%s'." %(match.group()), self._errpos(pos__))
                    if param:
                        raise TemplateSyntaxError("'else' may not have parameters at '%s'." %(match.group()), self._errpos(pos__))
                    parsetree.append(("else", self._parse(content, pos_c)))
                elif 'macro' == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'macro'
                    # make sure param is "\w+" (instead of ".+")
                    if not param:
                        raise TemplateSyntaxError("Missing name for 'macro' at '%s'." %(match.group()), self._errpos(pos__))
                    if not self._reMacroParam.match(param):
                        raise TemplateSyntaxError("Invalid name for 'macro' at '%s'." %(match.group()), self._errpos(pos__))
                    #remove last newline
                    if len(content) > 0 and content[-1] == '\n':
                        content = content[:-1]
                    if len(content) > 0 and content[-1] == '\r':
                        content = content[:-1]
                    parsetree.append(("macro", param, self._parse(content, pos_c)))

                # parser-commands
                elif 'raw'   == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block '%s'." %(match.group()), self._errpos(pos__))
                    if param:
                        raise TemplateSyntaxError("'raw' may not have parameters at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'raw'
                    parsetree.append(("str", content))
                elif 'include' == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block '%s'." %(match.group()), self._errpos(pos__))
                    if param:
                        raise TemplateSyntaxError("'include' may not have parameters at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'include'
                    try:
                        u = self._load(content.strip())
                    except Exception as err:
                        raise TemplateIncludeError(err, self._errpos(pos__))
                    self._includestack.append((content.strip(), u))  # current filename/template for error-msg.
                    p = self._parse(u)
                    self._includestack.pop()
                    parsetree.extend(p)
                elif 'set_escape' == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block '%s'." %(match.group()), self._errpos(pos__))
                    if param:
                        raise TemplateSyntaxError("'set_escape' may not have parameters at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'set_escape'
                    esc = content.strip().upper()
                    if esc not in ESCAPE_SUPPORTED:
                        raise TemplateSyntaxError("Unsupported escape '%s'." %(esc), self._errpos(pos__))
                    self.escape = ESCAPE_SUPPORTED[esc]
                else:
                    raise TemplateSyntaxError("Invalid keyword '%s'." %(keyword), self._errpos(pos__))
            curr = match.end()

        if block_type is not None:
            raise TemplateSyntaxError("Missing end-tag.", self._errpos(pos__))

        if len(template) > curr:            # process template-part after last block
            self._parse_sub(parsetree, template[curr:], fpos+curr)

        return parsetree

#-----------------------------------------
# Evaluation

# some checks
assert len(eval("dir()", {'__builtins__':{'dir':dir}})) == 1, \
    "FATAL: 'eval' does not work as expected (%s)."
assert compile("0 .__class__", "<string>", "eval").co_names == ('__class__',), \
    "FATAL: 'compile' does not work as expected."

class EvalPseudoSandbox:
    """An eval-pseudo-sandbox.

    The pseudo-sandbox restricts the available functions/objects, so the
    code can only access:

    - some of the builtin Python-functions, which are considered "safe"
      (see safe_builtins)
    - some additional functions (exists(), default(), setvar(), escape())
    - the passed objects incl. their methods.

    Additionally, names beginning with "_" are forbidden.
    This is to prevent things like '0 .__class__', with which you could
    easily break out of a "sandbox".

    Be careful to only pass "safe" objects/functions to the template,
    because any unsafe function/method could break the sandbox!
    For maximum security, restrict the access to as few objects/functions
    as possible!

    :Warning:
        Note that this is no real sandbox! (And although I don't know any
        way to break out of the sandbox without passing-in an unsafe object,
        I cannot guarantee that there is no such way. So use with care.)

        Take care if you want to use it for untrusted code!!
    """

    safe_builtins = {
        "True"      : True,
        "False"     : False,
        "None"      : None,

        "abs"       : builtins.abs,
        "chr"       : builtins.chr,
        "divmod"    : builtins.divmod,
        "hash"      : builtins.hash,
        "hex"       : builtins.hex,
        "len"       : builtins.len,
        "max"       : builtins.max,
        "min"       : builtins.min,
        "oct"       : builtins.oct,
        "ord"       : builtins.ord,
        "pow"       : builtins.pow,
        "range"     : builtins.range,
        "round"     : builtins.round,
        "sorted"    : builtins.sorted,
        "sum"       : builtins.sum,
        "unichr"    : builtins.chr,
        "zip"       : builtins.zip,

        "bool"      : builtins.bool,
        "bytes"     : builtins.bytes,
        "complex"   : builtins.complex,
        "dict"      : builtins.dict,
        "enumerate" : builtins.enumerate,
        "float"     : builtins.float,
        "int"       : builtins.int,
        "list"      : builtins.list,
        "long"      : long,
        "reversed"  : builtins.reversed,
        "str"       : builtins.str,
        "tuple"     : builtins.tuple,
        "unicode"   : unicode,
    }
    if sys.version_info[0] < 3:
        safe_builtins["unichr"] = builtins.unichr

    def __init__(self):
        self._compile_cache = {}
        self.locals_ptr = None
        self.eval_allowed_globals = self.safe_builtins.copy()
        self.register("__import__", self.f_import)
        self.register("exists",  self.f_exists)
        self.register("default", self.f_default)
        self.register("setvar",  self.f_setvar)
        self.register("escape",  self.f_escape)

    def register(self, name, obj):
        """Add an object to the "allowed eval-globals".

        Mainly useful to add user-defined functions to the pseudo-sandbox.
        """
        self.eval_allowed_globals[name] = obj

    def compile(self, expr):
        """Compile a Python-eval-expression.

        - Use a compile-cache.
        - Raise a `NameError` if `expr` contains a name beginning with ``_``.

        :Returns: the compiled `expr`
        :Exceptions:
            - `SyntaxError`: for compile-errors
            - `NameError`: if expr contains a name beginning with ``_``
        """
        if expr not in self._compile_cache:
            c = compile(expr, "", "eval")
            for i in c.co_names:    #prevent breakout via new-style-classes
                if i[0] == '_':
                    raise NameError("Name '%s' is not allowed." % i)
            self._compile_cache[expr] = c
        return self._compile_cache[expr]

    def eval(self, expr, locals):
        """Eval a Python-eval-expression.

        Sets ``self.locals_ptr`` to ``locales`` and compiles the code
        before evaluating.
        """
        sav = self.locals_ptr
        self.locals_ptr = locals
        x = eval(self.compile(expr), {"__builtins__":self.eval_allowed_globals}, locals)
        self.locals_ptr = sav
        return x

    def f_import(self, name, *_, **__):
        """``import``/``__import__()`` for the sandboxed code.

        Since "import" is insecure, the PseudoSandbox does not allow to
        import other modules. But since some functions need to import
        other modules (e.g. "datetime.datetime.strftime" imports "time"),
        this function replaces the builtin "import" and allows to use
        modules which are already accessible by the sandboxed code.

        :Note:
            - This probably only works for rather simple imports.
            - For security, it may be better to avoid such (complex) modules
              which import other modules. (e.g. use time.localtime and
              time.strftime instead of datetime.datetime.strftime,
              or write a small wrapper.)

        :Example:

            >>> from datetime import datetime
            >>> import pyratemp
            >>> t = pyratemp.Template('@!mytime.strftime("%H:%M:%S")!@')

            # >>> print(t(mytime=datetime.now()))
            # Traceback (most recent call last):
            #   ...
            # ImportError: import not allowed in pseudo-sandbox; try to import 'time' yourself and pass it to the sandbox/template

            >>> import time
            >>> print(t(mytime=datetime.strptime("13:40:54", "%H:%M:%S"), time=time))
            13:40:54

            # >>> print(t(mytime=datetime.now(), time=time))
            # 13:40:54
        """
        import types
        if self.locals_ptr is not None  and  name in self.locals_ptr  and  isinstance(self.locals_ptr[name], types.ModuleType):
            return self.locals_ptr[name]
        else:
            raise ImportError("import not allowed in pseudo-sandbox; try to import '%s' yourself (and maybe pass it to the sandbox/template)" % name)

    def f_exists(self, varname):
        """``exists()`` for the sandboxed code.

        Test if the variable `varname` exists in the current locals-namespace.

        This only works for single variable names. If you want to test
        complicated expressions, use i.e. `default`.
        (i.e. `default("expr",False)`)

        :Note:      the variable-name has to be quoted! (like in eval)
        :Example:   see module-docstring
        """
        return (varname in self.locals_ptr)

    def f_default(self, expr, default=None):
        """``default()`` for the sandboxed code.

        Try to evaluate an expression and return the result or a
        fallback-/default-value; the `default`-value is used
        if `expr` does not exist/is invalid/results in None.

        This is very useful for optional data.

        :Parameter:
            - expr: eval-expression
            - default: fallback-falue if eval(expr) fails or is None.
        :Returns:
            the eval-result or the "fallback"-value.

        :Note:      the eval-expression has to be quoted! (like in eval)
        :Example:   see module-docstring
        """
        try:
            r = self.eval(expr, self.locals_ptr)
            if r is None:
                return default
            return r
        #TODO: which exceptions should be catched here?
        except (NameError, LookupError, TypeError):
            return default

    def f_setvar(self, name, expr):
        """``setvar()`` for the sandboxed code.

        Set a variable.

        :Example:   see module-docstring
        """
        self.locals_ptr[name] = self.eval(expr, self.locals_ptr)
        return ""

    def f_escape(self, s, format="HTML"):
        """``escape()`` for the sandboxed code.
        """
        if isinstance(format, (str, unicode)):
            format = ESCAPE_SUPPORTED[format.upper()]
        return escape(unicode(s), format)

#-----------------------------------------
# basic template / subtemplate

class TemplateBase:
    """Basic template-class.

    Used both for the template itself and for 'macro's ("subtemplates") in
    the template.
    """

    def __init__(self, parsetree, renderfunc, data=None):
        """Create the Template/Subtemplate/Macro.

        :Parameters:
            - `parsetree`: parse-tree of the template/subtemplate/macro
            - `renderfunc`: render-function
            - `data`: data to fill into the template by default (dictionary).
              This data may later be overridden when rendering the template.
        :Exceptions:
            - `TypeError`: if `data` is not a dictionary
        """
        #TODO: parameter-checking?
        self.parsetree = parsetree
        if isinstance(data, dict):
            self.data = data
        elif data is None:
            self.data = {}
        else:
            raise TypeError('"data" must be a dict (or None).')
        self.current_data = data
        self._render = renderfunc

    def __call__(self, **override):
        """Fill out/render the template.

        :Parameters:
            - `override`: objects to add to the data-namespace, overriding
              the "default"-data.
        :Returns:    the filled template (in unicode)
        :Note:       This is also called when invoking macros
                     (i.e. ``$!mymacro()!$``).
        """
        self.current_data = self.data.copy()
        self.current_data.update(override)
        u = "".join(self._render(self.parsetree, self.current_data))
        self.current_data = self.data       # restore current_data
        return _dontescape(u)               # (see class _dontescape)

    def __unicode__(self):
        """Alias for __call__()."""
        return self.__call__()
    def __str__(self):
        """Alias for __call__()."""
        return self.__call__()

#-----------------------------------------
# Renderer

class _dontescape(unicode):
    """Unicode-string which should not be escaped.

    If ``isinstance(object,_dontescape)``, then don't escape the object in
    ``@!...!@``. It's useful for not double-escaping macros, and it's
    automatically used for macros/subtemplates.

    :Note: This only works if the object is used on its own in ``@!...!@``.
           It i.e. does not work in ``@!object*2!@`` or ``@!object + "hi"!@``.
    """
    __slots__ = []


class Renderer(object):
    """Render a template-parse-tree.

    :Uses: `TemplateBase` for macros
    """

    def __init__(self, evalfunc, escapefunc):
        """Init the renderer.

        :Parameters:
            - `evalfunc`: function for template-expression-evaluation
              (i.e. ``EvalPseudoSandbox().eval``)
            - `escapefunc`: function for escaping special characters
              (i.e. `escape`)
        """
        #TODO: test evalfunc
        self.evalfunc = evalfunc
        self.escapefunc = escapefunc

    def _eval(self, expr, data):
        """evalfunc with error-messages"""
        try:
            return self.evalfunc(expr, data)
        #TODO: any other errors to catch here?
        except (TypeError,NameError,LookupError,AttributeError, SyntaxError) as err:
            raise TemplateRenderError("Cannot eval expression '%s'. (%s: %s)" %(expr, err.__class__.__name__, err))

    def render(self, parsetree, data):
        """Render a parse-tree of a template.

        :Parameters:
            - `parsetree`: the parse-tree
            - `data`:      the data to fill into the template (dictionary)
        :Returns:   the rendered output-unicode-string
        :Exceptions:
            - `TemplateRenderError`
        """
        _eval = self._eval  # shortcut
        output = []
        do_else = False     # use else/elif-branch?

        if parsetree is None:
            return ""
        for elem in parsetree:
            if   "str"   == elem[0]:
                output.append(elem[1])
            elif "sub"   == elem[0]:
                output.append(unicode(_eval(elem[1], data)))
            elif "esc"   == elem[0]:
                obj = _eval(elem[2], data)
                #prevent double-escape
                if isinstance(obj, _dontescape) or isinstance(obj, TemplateBase):
                    output.append(unicode(obj))
                else:
                    output.append(self.escapefunc(unicode(obj), elem[1]))
            elif "for"   == elem[0]:
                do_else = True
                (names, iterable) = elem[1:3]
                try:
                    loop_iter = iter(_eval(iterable, data))
                except TypeError:
                    raise TemplateRenderError("Cannot loop over '%s'." % iterable)
                for i in loop_iter:
                    do_else = False
                    if len(names) == 1:
                        data[names[0]] = i
                    else:
                        data.update(zip(names, i))   #"for a,b,.. in list"
                    output.extend(self.render(elem[3], data))
            elif "if"    == elem[0]:
                do_else = True
                if _eval(elem[1], data):
                    do_else = False
                    output.extend(self.render(elem[2], data))
            elif "elif"  == elem[0]:
                if do_else and _eval(elem[1], data):
                    do_else = False
                    output.extend(self.render(elem[2], data))
            elif "else"  == elem[0]:
                if do_else:
                    do_else = False
                    output.extend(self.render(elem[1], data))
            elif "macro" == elem[0]:
                data[elem[1]] = TemplateBase(elem[2], self.render, data)
            else:
                raise TemplateRenderError("Invalid parse-tree (%s)." %(elem))

        return output

#-----------------------------------------
# template user-interface (putting it all together)

class Template(TemplateBase):
    """Template-User-Interface.

    :Usage:
        ::
            t = Template(...)  (<- see __init__)
            output = t(...)    (<- see TemplateBase.__call__)

    :Example:
        see module-docstring
    """

    def __init__(self, string=None,filename=None,parsetree=None, encoding='utf-8', data=None, escape=HTML,
            loader_class=LoaderFile,
            parser_class=Parser,
            renderer_class=Renderer,
            eval_class=EvalPseudoSandbox,
            escape_func=escape):
        """Load (+parse) a template.

        :Parameters:
            - `string,filename,parsetree`: a template-string,
                                           filename of a template to load,
                                           or a template-parsetree.
                                           (only one of these 3 is allowed)
            - `encoding`: encoding of the template-files (only used for "filename")
            - `data`:     data to fill into the template by default (dictionary).
                          This data may later be overridden when rendering the template.
            - `escape`:   default-escaping for the template, may be overwritten by the template!
            - `loader_class`
            - `parser_class`
            - `renderer_class`
            - `eval_class`
            - `escapefunc`
        """
        if [string, filename, parsetree].count(None) != 2:
            raise ValueError('Exactly 1 of string,filename,parsetree is necessary.')

        tmpl = None
        # load template
        if filename is not None:
            incl_load = loader_class(os.path.dirname(filename), encoding).load
            tmpl = incl_load(os.path.basename(filename))
        if string is not None:
            incl_load = dummy_raise(NotImplementedError, "'include' not supported for template-strings.")
            tmpl = LoaderString(encoding).load(string)

        # eval (incl. compile-cache)
        templateeval = eval_class()

        # parse
        if tmpl is not None:
            p = parser_class(loadfunc=incl_load, testexpr=templateeval.compile, escape=escape)
            parsetree = p.parse(tmpl)
            del p

        # renderer
        renderfunc = renderer_class(templateeval.eval, escape_func).render

        #create template
        TemplateBase.__init__(self, parsetree, renderfunc, data)


#=========================================
#doctest

def _doctest():
    """doctest this module."""
    import doctest
    doctest.testmod()

#----------------------
if __name__ == '__main__':
    if sys.version_info[0] <= 2:
        _doctest()

#=========================================
          cmakeBuildDirectory: #!/usr/bin/env python
#
# UAVCAN DSDL compiler for libuavcan
# Supported Python versions: 3.2+, 2.7.
#
# Copyright (C) 2014 
#

from __future__ import division, absolute_import, print_function, unicode_literals
import os, sys, logging, argparse

# This trickery allows to use the compiler even if pyuavcan is not installed in the system.
# This is extremely important, as it makes the compiler (and therefore libuavcan in general)
# totally dependency-free, except for the Python interpreter itself.
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
LOCAL_PYUAVCAN_DIR = os.path.join(SCRIPT_DIR, 'pyuavcan')
RUNNING_FROM_SRC_DIR = os.path.isdir(LOCAL_PYUAVCAN_DIR)
if RUNNING_FROM_SRC_DIR:
    #print('Running from the source directory')
    sys.path.insert(0, SCRIPT_DIR)
    sys.path.insert(0, LOCAL_PYUAVCAN_DIR)

def configure_logging(verbosity):
    fmt = '%(message)s'
    level = { 0: logging.WARNING, 1: logging.INFO, 2: logging.DEBUG }.get(verbosity or 0, logging.DEBUG)
    logging.basicConfig(stream=sys.stderr, level=level, format=fmt)

def die(text):
    print(text, file=sys.stderr)
    exit(1)

DEFAULT_OUTDIR = 'dsdlc_generated'

DESCRIPTION = '''UAVCAN DSDL compiler for libuavcan.
Takes an input directory that contains an hierarchy of DSDL
definitions and converts it into compatible hierarchy of C++ types for libuavcan.
This script can be used directly from the source directory, no installation required!
Supported Python versions: 3.2+, 2.7.
'''

argparser = argparse.ArgumentParser(description=DESCRIPTION)
argparser.add_argument('source_dir', nargs='+', help='source directory with DSDL definitions')
argparser.add_argument('--verbose', '-v', action='count', help='verbosity level (-v, -vv)')
argparser.add_argument('--outdir', '-O', default=DEFAULT_OUTDIR, help='output directory, default %s' % DEFAULT_OUTDIR)
argparser.add_argument('--incdir', '-I', default=[], action='append', help=
'''nested type namespaces, one path per argument. Can be also specified through the environment variable
UAVCAN_DSDL_INCLUDE_PATH, where the path entries are separated by colons ":"''')
args = argparser.parse_args()

configure_logging(args.verbose)

try:
    extra_incdir = os.environ['UAVCAN_DSDL_INCLUDE_PATH'].split(':')
    logging.info('Additional include directories: %s', extra_incdir)
    args.incdir += extra_incdir
except KeyError:
    pass

from libuavcan_dsdl_compiler import run as dsdlc_run
try:
    dsdlc_run(args.source_dir, args.incdir, args.outdir)
except Exception as ex:
    logging.error('Compiler failure', exc_info=True)
    die(str(ex))
          # Ruleset file that will determine what checks will be run
          ruleset: #!/usr/bin/env python

from distutils.core import setup

args = dict(
    name='libuavcan_dsdl_compiler',
    version='0.1',
    description='UAVCAN DSDL compiler for libuavcan',
    packages=['libuavcan_dsdl_compiler'],
    package_data={'libuavcan_dsdl_compiler': ['data_type_template.tmpl']},
    scripts=['libuavcan_dsdlc'],
    requires=['uavcan'],
    url='http://uavcan.org',
    license='MIT'
)

setup(**args)

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


