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

        uses: microsoft/msvc-code-analysis-action@04825f6d9e00f87422d6bf04e1a38b1f3ed60d99
        # Provide a unique ID to access the sarif output path
        id: run-analysis
        with:
          cmakeBuildDirectory: ${{ env.build }}
          # Ruleset file that will determine what checks will be run
          ruleset: NativeRecommendedRules.ruleset

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


