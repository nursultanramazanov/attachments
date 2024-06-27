# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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
};
  push: *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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
    branches: [ "main" ]
  pull_request: //Bank.cpp
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

    branches: [ "main" ]
  schedule: 







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

    - cron: '28 21 * * 4'

env: 







//BankState.cpp
//
//Class file for bank state object
//

#include "BankState.h"

using namespace std;
using namespace DRAMSim;

//All banks start precharged
BankState::BankState(ostream &dramsim_log_):
                dramsim_log(dramsim_log_),
                currentBankState(Idle),
                openRowAddress(0),
                nextRead(0),
                nextWrite(0),
                nextActivate(0),
                nextPrecharge(0),
                nextPowerUp(0),
                lastCommand(READ),
                stateChangeCountdown(0)
{}

void BankState::print()
{
        PRINT(" == Bank State ");
        if (currentBankState == Idle)
        {
                PRINT("    State : Idle" );
        }
        else if (currentBankState == RowActive)
        {
                PRINT("    State : Active" );
        }
        else if (currentBankState == Refreshing)
        {
                PRINT("    State : Refreshing" );
        }
        else if (currentBankState == PowerDown)
        {
                PRINT("    State : Power Down" );
        }

        PRINT("    OpenRowAddress : " << openRowAddress );
        PRINT("    nextRead       : " << nextRead );
        PRINT("    nextWrite      : " << nextWrite );
        PRINT("    nextActivate   : " << nextActivate );
        PRINT("    nextPrecharge  : " << nextPrecharge );
        PRINT("    nextPowerUp    : " << nextPowerUp );
}
  # Path to the CMake build directory.
  build: '${{ 







#ifndef BANKSTATE_H
#define BANKSTATE_H

//BankState.h
//
//Header file for bank state class
//

#include "SystemConfiguration.h"
#include "BusPacket.h"

namespace DRAMSim
{
enum CurrentBankState
{
        Idle,
        RowActive,
        Precharging,
        Refreshing,
        PowerDown
};

class BankState
{
        ostream &dramsim_log; 
public:
        //Fields
        CurrentBankState currentBankState;
        unsigned openRowAddress;
        uint64_t nextRead;
        uint64_t nextWrite;
        uint64_t nextActivate;
        uint64_t nextPrecharge;
        uint64_t nextPowerUp;

        BusPacketType lastCommand;
        unsigned stateChangeCountdown;

        //Functions
        BankState(ostream &dramsim_log_);
        void print();
};
}

#endif
 }}/build'

permissions: 







//BusPacket.cpp
//
//Class file for bus packet object
//

#include "BusPacket.h"

using namespace DRAMSim;
using namespace std;

BusPacket::BusPacket(BusPacketType packtype, uint64_t physicalAddr, 
                unsigned col, unsigned rw, unsigned r, unsigned b, void *dat, 
                ostream &dramsim_log_) :
        dramsim_log(dramsim_log_),
        busPacketType(packtype),
        column(col),
        row(rw),
        bank(b),
        rank(r),
        physicalAddress(physicalAddr),
        data(dat)
{}

void BusPacket::print(uint64_t currentClockCycle, bool dataStart)
{
        if (this == NULL)
        {
                return;
        }

        if (VERIFICATION_OUTPUT)
        {
                switch (busPacketType)
                {
                case READ:
                        cmd_verify_out << currentClockCycle << ": read ("<<rank<<","<<bank<<","<<column<<",0);"<<endl;
                        break;
                case READ_P:
                        cmd_verify_out << currentClockCycle << ": read ("<<rank<<","<<bank<<","<<column<<",1);"<<endl;
                        break;
                case WRITE:
                        cmd_verify_out << currentClockCycle << ": write ("<<rank<<","<<bank<<","<<column<<",0 , 0, 'h0);"<<endl;
                        break;
                case WRITE_P:
                        cmd_verify_out << currentClockCycle << ": write ("<<rank<<","<<bank<<","<<column<<",1, 0, 'h0);"<<endl;
                        break;
                case ACTIVATE:
                        cmd_verify_out << currentClockCycle <<": activate (" << rank << "," << bank << "," << row <<");"<<endl;
                        break;
                case PRECHARGE:
                        cmd_verify_out << currentClockCycle <<": precharge (" << rank << "," << bank << "," << row <<");"<<endl;
                        break;
                case REFRESH:
                        cmd_verify_out << currentClockCycle <<": refresh (" << rank << ");"<<endl;
                        break;
                case DATA:
                        //TODO: data verification?
                        break;
                default:
                        ERROR("Trying to print unknown kind of bus packet");
                        exit(-1);
                }
        }
}
void BusPacket::print()
{
        if (this == NULL) //pointer use makes this a necessary precaution
        {
                return;
        }
        else
        {
                switch (busPacketType)
                {
                case READ:
                        PRINT("BP [READ] pa[0x"<<hex<<physicalAddress<<dec<<"] r["<<rank<<"] b["<<bank<<"] row["<<row<<"] col["<<column<<"]");
                        break;
                case READ_P:
                        PRINT("BP [READ_P] pa[0x"<<hex<<physicalAddress<<dec<<"] r["<<rank<<"] b["<<bank<<"] row["<<row<<"] col["<<column<<"]");
                        break;
                case WRITE:
                        PRINT("BP [WRITE] pa[0x"<<hex<<physicalAddress<<dec<<"] r["<<rank<<"] b["<<bank<<"] row["<<row<<"] col["<<column<<"]");
                        break;
                case WRITE_P:
                        PRINT("BP [WRITE_P] pa[0x"<<hex<<physicalAddress<<dec<<"] r["<<rank<<"] b["<<bank<<"] row["<<row<<"] col["<<column<<"]");
                        break;
                case ACTIVATE:
                        PRINT("BP [ACT] pa[0x"<<hex<<physicalAddress<<dec<<"] r["<<rank<<"] b["<<bank<<"] row["<<row<<"] col["<<column<<"]");
                        break;
                case PRECHARGE:
                        PRINT("BP [PRE] pa[0x"<<hex<<physicalAddress<<dec<<"] r["<<rank<<"] b["<<bank<<"] row["<<row<<"] col["<<column<<"]");
                        break;
                case REFRESH:
                        PRINT("BP [REF] pa[0x"<<hex<<physicalAddress<<dec<<"] r["<<rank<<"] b["<<bank<<"] row["<<row<<"] col["<<column<<"]");
                        break;
                case DATA:
                        PRINTN("BP [DATA] pa[0x"<<hex<<physicalAddress<<dec<<"] r["<<rank<<"] b["<<bank<<"] row["<<row<<"] col["<<column<<"] data["<<data<<"]=");
                        printData();
                        PRINT("");
                        break;
                default:
                        ERROR("Trying to print unknown kind of bus packet");
                        exit(-1);
                }
        }
}

void BusPacket::printData() const 
{
        if (data == NULL)
        {
                PRINTN("NO DATA");
                return;
        }
        PRINTN("'" << hex);
        for (int i=0; i < 4; i++)
        {
                PRINTN(((uint64_t *)data)[i]);
        }
        PRINTN("'" << dec);
}
  contents: read

jobs: 
#ifndef BUSPACKET_H
#define BUSPACKET_H
//BusPacket.h
//
//Header file for bus packet object
//

#include "SystemConfiguration.h"

namespace DRAMSim
{
enum BusPacketType
{
        READ,
        READ_P,
        WRITE,
        WRITE_P,
        ACTIVATE,
        PRECHARGE,
        REFRESH,
        DATA
};

class BusPacket
{
        BusPacket();
        ostream &dramsim_log; 
public:
        //Fields
        BusPacketType busPacketType;
        unsigned column;
        unsigned row;
        unsigned bank;
        unsigned rank;
        uint64_t physicalAddress;
        void *data;

        //Functions
        BusPacket(BusPacketType packtype, uint64_t physicalAddr, unsigned col, unsigned rw, unsigned r, unsigned b, void *dat, ostream &dramsim_log_);

        void print();
        void print(uint64_t currentClockCycle, bool dataStart);
        void printData() const;

};
}

#endif

  analyze: #ifndef _CSV_WRITER_H_
#define _CSV_WRITER_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>

#include <string.h>

using std::vector; 
using std::ostream;
using std::string; 
/*
 * CSVWriter: Writes CSV data with headers to an underlying ofstream 
 *         This wrapper is meant to look like an ofstream, but it captures 
 *         the names of each field and prints it out to a header before printing
 *         the CSV data below. 
 *
 *         Note: the first finalize() will not print the values out, only the headers.
 *         One way to fix this problem would be to use a sstringstream (or something) 
 *         to buffer out the values and flush them all in one go instead of passing them 
 *         directly to the underlying stream as is the case now. 
 *
 *         Example usage: 
 *
 *         CSVWriter sw(cout);               // send output to cout
 *         sw <<"Bandwidth" << 0.5; // value ignored
 *         sw <<"Latency" << 5;     // value ignored
 *         sw.finalize();                      // flush the header 
 *         sw <<"Bandwidth" << 1.5; // field name ignored
 *         sw <<"Latency" << 15;     // field name ignored
 *         sw.finalize();                                                         // values printed to csv line
 *         sw <<"Bandwidth" << 2.5; // field name ignored
 *         sw <<"Latency" << 25;     // field name ignored
 *         sw.finalize();                                                         // values printed to csv line
 *
 *         The output of this example will be: 
 *
 *         Bandwidth,Latency
 *         1.5,15
 *         2.5,25
 *
 */


namespace DRAMSim {

        class CSVWriter {
                public :
                struct IndexedName {
                        static const size_t MAX_TMP_STR = 64; 
                        static const unsigned SINGLE_INDEX_LEN = 4; 
                        string str; 

                        // functions 
                        static bool isNameTooLong(const char *baseName, unsigned numIndices)
                        {
                                return (strlen(baseName)+(numIndices*SINGLE_INDEX_LEN)) > MAX_TMP_STR;
                        }
                        static void checkNameLength(const char *baseName, unsigned numIndices)
                        {
                                if (isNameTooLong(baseName, numIndices))
                                {
                                        ERROR("Your string "<<baseName<<" is too long for the max stats size ("<<MAX_TMP_STR<<", increase MAX_TMP_STR"); 
                                        exit(-1); 
                                }
                        }
                        IndexedName(const char *baseName, unsigned channel)
                        {
                                checkNameLength(baseName,1);
                                char tmp_str[MAX_TMP_STR]; 
                                snprintf(tmp_str, MAX_TMP_STR,"%s[%u]", baseName, channel); 
                                str = string(tmp_str); 
                        }
                        IndexedName(const char *baseName, unsigned channel, unsigned rank)
                        {
                                checkNameLength(baseName,2);
                                char tmp_str[MAX_TMP_STR]; 
                                snprintf(tmp_str, MAX_TMP_STR,"%s[%u][%u]", baseName, channel, rank); 
                                str = string(tmp_str); 
                        }
                        IndexedName(const char *baseName, unsigned channel, unsigned rank, unsigned bank)
                        {
                                checkNameLength(baseName,3);
                                char tmp_str[MAX_TMP_STR]; 
                                snprintf(tmp_str, MAX_TMP_STR,"%s[%u][%u][%u]", baseName, channel, rank, bank); 
                                str = string(tmp_str);
                        }

                };
                // where the output will eventually go 
                ostream &output; 
                vector<string> fieldNames; 
                bool finalized; 
                unsigned idx; 
                public: 

                // Functions
                void finalize()
                {
                        //TODO: tag unlikely
                        if (!finalized)
                        {
                                for (unsigned i=0; i<fieldNames.size(); i++)
                                {
                                        output << fieldNames[i] << ",";
                                }
                                output << std::endl << std::flush;
                                finalized=true; 
                        }
                        else
                        {
                                if (idx < fieldNames.size()) 
                                {
                                        printf(" Number of fields doesn't match values (fields=%u, values=%u), check each value has a field name before it\n", idx, (unsigned)fieldNames.size());
                                }
                                idx=0; 
                                output << std::endl; 
                        }
                }

                // Constructor 
                CSVWriter(ostream &_output) : output(_output), finalized(false), idx(0)
                {}

                // Insertion operators for field names
                CSVWriter &operator<<(const char *name)
                {
                        if (!finalized)
                        {
//                                cout <<"Adding "<<name<<endl;
                                fieldNames.push_back(string(name));
                        }
                        return *this; 
                }

                CSVWriter &operator<<(const string &name)
                {
                        if (!finalized)
                        {
                                fieldNames.push_back(string(name));
                        }
                        return *this; 
                }

                CSVWriter &operator<<(const IndexedName &indexedName)
                {
                        if (!finalized)
                        {
//                                cout <<"Adding "<<indexedName.str<<endl;
                                fieldNames.push_back(indexedName.str);
                        }
                        return *this; 
                }

                bool isFinalized()
                {
//                        printf("obj=%p", this); 
                        return finalized; 
                }

                ostream &getOutputStream()
                {
                        return output; 
                }
                // Insertion operators for value types 
                // All of the other types just need to pass through to the underlying
                // ofstream, so just write this small wrapper function to make the
                // whole thing less verbose
#define ADD_TYPE(T) \
                CSVWriter &operator<<(T value) \
                {                                \
                        if (finalized)                \
                        {                             \
                                output << value <<",";     \
                                idx++;                     \
                        }                             \
                        return *this;                 \
                }                      

        ADD_TYPE(int);
        ADD_TYPE(unsigned); 
        ADD_TYPE(long);
        ADD_TYPE(uint64_t);
        ADD_TYPE(float);
        ADD_TYPE(double);

        //disable copy constructor and assignment operator
        private:
                CSVWriter(const CSVWriter &); 
                CSVWriter &operator=(const CSVWriter &);

        }; // class CSVWriter


} // namespace DRAMSim

#endif // _CSV_WRITER_H_
    permissions: 


#include <stdint.h> // uint64_t

#ifndef CALLBACK_H
#define CALLBACK_H

namespace DRAMSim
{

template <typename ReturnT, typename Param1T, typename Param2T,
typename Param3T>
class CallbackBase
{
public:
        virtual ~CallbackBase() = 0;
        virtual ReturnT operator()(Param1T, Param2T, Param3T) = 0;
};

template <typename Return, typename Param1T, typename Param2T, typename Param3T>
DRAMSim::CallbackBase<Return,Param1T,Param2T,Param3T>::~CallbackBase() {}

template <typename ConsumerT, typename ReturnT,
typename Param1T, typename Param2T, typename Param3T >
class Callback: public CallbackBase<ReturnT,Param1T,Param2T,Param3T>
{
private:
        typedef ReturnT (ConsumerT::*PtrMember)(Param1T,Param2T,Param3T);

public:
        Callback( ConsumerT* const object, PtrMember member) :
                        object(object), member(member)
        {
        }

        Callback( const Callback<ConsumerT,ReturnT,Param1T,Param2T,Param3T>& e ) :
                        object(e.object), member(e.member)
        {
        }

        ReturnT operator()(Param1T param1, Param2T param2, Param3T param3)
        {
                return (const_cast<ConsumerT*>(object)->*member)
                       (param1,param2,param3);
        }

private:

        ConsumerT* const object;
        const PtrMember  member;
};

typedef CallbackBase <void, unsigned, uint64_t, uint64_t> TransactionCompleteCB;
} // namespace DRAMSim

#endif
      contents: read # for actions/checkout to fetch code
      security-events: write # for github/codeql-action/upload-sarif to upload SARIF results
      actions: read # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status
    name: Analyze
    runs-on: windows-latest

    steps: #include "ClockDomain.h"

using namespace std;



namespace ClockDomain
{
        // "Default" crosser with a 1:1 ratio
        ClockDomainCrosser::ClockDomainCrosser(ClockUpdateCB *_callback)
                : callback(_callback), clock1(1UL), clock2(1UL), counter1(0UL), counter2(0UL)
        {
        }
        ClockDomainCrosser::ClockDomainCrosser(uint64_t _clock1, uint64_t _clock2, ClockUpdateCB *_callback) 
                : callback(_callback), clock1(_clock1), clock2(_clock2), counter1(0), counter2(0)
        {
                //cout << "CTOR: callback address: " << (uint64_t)(this->callback) << "\t ratio="<<clock1<<"/"<<clock2<< endl;
        }

        ClockDomainCrosser::ClockDomainCrosser(double ratio, ClockUpdateCB *_callback)
                : callback(_callback), counter1(0), counter2(0)
        {
                // Compute numerator and denominator for ratio, then pass that to other constructor.
                double x = ratio;

                const int MAX_ITER = 15;
                size_t i;
                unsigned ns[MAX_ITER], ds[MAX_ITER];
                double zs[MAX_ITER];
                ds[0] = 0;
                ds[1] = 1;
                zs[1] = x;
                ns[1] = (int)x; 

                for (i = 1; i<MAX_ITER-1; i++)
                {
                        if (fabs(x - (double)ns[i]/(double)ds[i]) < 0.00005)
                        {
                                //printf("ANSWER= %u/%d\n",ns[i],ds[i]);
                                break;
                        }
                        //TODO: or, if the answers are the same as the last iteration, stop 

                        zs[i+1] = 1.0f/(zs[i]-(int)floor(zs[i])); // 1/(fractional part of z_i)
                        ds[i+1] = ds[i]*(int)floor(zs[i+1])+ds[i-1];
                        double tmp = x*ds[i+1];
                        double tmp2 = tmp - (int)tmp;
                        ns[i+1] = tmp2 >= 0.5 ? ceil(tmp) : floor(tmp); // ghetto implementation of a rounding function
                        //printf("i=%lu, z=%20f n=%5u d=%5u\n",i,zs[i],ns[i],ds[i]);
                }

                //printf("APPROXIMATION= %u/%d\n",ns[i],ds[i]);
                this->clock1=ns[i];
                this->clock2=ds[i];

                //cout << "CTOR: callback address: " << (uint64_t)(this->callback) << "\t ratio="<<clock1<<"/"<<clock2<< endl;
        }

        void ClockDomainCrosser::update()
        {
                //short circuit case for 1:1 ratios
                if (clock1 == clock2 && callback)
                {
                        (*callback)();
                        return; 
                }

                // Update counter 1.
                counter1 += clock1;

                while (counter2 < counter1)
                {
                        counter2 += clock2;
                        //cout << "CALLBACK: counter1= " << counter1 << "; counter2= " << counter2 << "; " << endl;
                        //cout << "callback address: " << (uint64_t)callback << endl;
                        if (callback)
                        {
                                //cout << "Callback() " << (uint64_t)callback<< "Counters: 1="<<counter1<<", 2="<<counter2 <<endl;
                                (*callback)();
                        }
                }

                if (counter1 == counter2)
                {
                        counter1 = 0;
                        counter2 = 0;
                }
        }



        void TestObj::cb()
        {
                        cout << "In Callback\n";
        }

        int TestObj::test()
        {
                ClockUpdateCB *callback = new Callback<TestObj, void>(this, &TestObj::cb);

                //ClockDomainCrosser x(5,2,&cb);
                //ClockDomainCrosser x(2,5,NULL);
                //ClockDomainCrosser x(37,41,NULL);
                //ClockDomainCrosser x(41,37,NULL);
                //cout << "(main) callback address: " << (uint64_t)&cb << endl;
                ClockDomainCrosser x(0.5, callback);
                cout <<"------------------------------------------\n";
                ClockDomainCrosser y(0.3333, callback);
                cout <<"------------------------------------------\n";
                ClockDomainCrosser z(0.9, callback);
                cout <<"------------------------------------------\n";


                for (int i=0; i<10; i++)
                {

                        x.update();
                        cout << "UPDATE: counter1= " << x.counter1 << "; counter2= " << x.counter2 << "; " << endl;
                }

                return 0;
        }


}
      - name: Checkout repository
        uses: actions/checkout@v4

      - name: Configure CMake
        run: cmake -B ${{ #include <iostream>

#include <cmath>
#include <stdint.h>

namespace ClockDomain
{

    template <typename ReturnT>
    class CallbackBase
    {
        public:
        virtual ReturnT operator()() = 0;
    };


    template <typename ConsumerT, typename ReturnT>
    class Callback: public CallbackBase<ReturnT>
    {
        private:
        typedef ReturnT (ConsumerT::*PtrMember)();

        public:
        Callback(ConsumerT* const object, PtrMember member) : object(object), member(member) {}

        Callback(const Callback<ConsumerT,ReturnT> &e) : object(e.object), member(e.member) {}

        ReturnT operator()()
        {
            return (const_cast<ConsumerT*>(object)->*member)();
        }

        private:
        ConsumerT* const object;
        const PtrMember  member;
    };

    typedef CallbackBase <void> ClockUpdateCB;


        class ClockDomainCrosser
        {
                public:
                ClockUpdateCB *callback;
                uint64_t clock1, clock2;
                uint64_t counter1, counter2;
                ClockDomainCrosser(ClockUpdateCB *_callback);
                ClockDomainCrosser(uint64_t _clock1, uint64_t _clock2, ClockUpdateCB *_callback);
                ClockDomainCrosser(double ratio, ClockUpdateCB *_callback);
                void update();
        };


        class TestObj
        {
                public:
                TestObj() {}
                void cb();
                int test();
        };
} }}

      # Build is not required unless generated source files are used
      # - name: Build CMake
      #   run: cmake --build ${{ env.build }}

      - name: Initialize MSVC Code Analysis
        uses: microsoft/msvc-code-analysis-action@04825f6d9e00f87422d6bf04e1a38b1f3ed60d99
        # Provide a unique ID to access the sarif output path
        id: run-analysis
        with: 







//CommandQueue.cpp
//
//Class file for command queue object
//

#include "CommandQueue.h"
#include "MemoryController.h"
#include <assert.h>

using namespace DRAMSim;

CommandQueue::CommandQueue(vector< vector<BankState> > &states, ostream &dramsim_log_) :
                dramsim_log(dramsim_log_),
                bankStates(states),
                nextBank(0),
                nextRank(0),
                nextBankPRE(0),
                nextRankPRE(0),
                refreshRank(0),
                refreshWaiting(false),
                sendAct(true)
{
        //set here to avoid compile errors
        currentClockCycle = 0;

        //use numBankQueus below to create queue structure
        size_t numBankQueues;
        if (queuingStructure==PerRank)
        {
                numBankQueues = 1;
        }
        else if (queuingStructure==PerRankPerBank)
        {
                numBankQueues = NUM_BANKS;
        }
        else
        {
                ERROR("== Error - Unknown queuing structure");
                exit(0);
        }

        //vector of counters used to ensure rows don't stay open too long
        rowAccessCounters = vector< vector<unsigned> >(NUM_RANKS, vector<unsigned>(NUM_BANKS,0));

        //create queue based on the structure we want
        BusPacket1D actualQueue;
        BusPacket2D perBankQueue = BusPacket2D();
        queues = BusPacket3D();
        for (size_t rank=0; rank<NUM_RANKS; rank++)
        {
                //this loop will run only once for per-rank and NUM_BANKS times for per-rank-per-bank
                for (size_t bank=0; bank<numBankQueues; bank++)
                {
                        actualQueue        = BusPacket1D();
                        perBankQueue.push_back(actualQueue);
                }
                queues.push_back(perBankQueue);
        }


        //FOUR-bank activation window
        //        this will count the number of activations within a given window
        //        (decrementing counter)
        //
        //countdown vector will have decrementing counters starting at tFAW
        //  when the 0th element reaches 0, remove it
        tFAWCountdown.reserve(NUM_RANKS);
        for (size_t i=0;i<NUM_RANKS;i++)
        {
                //init the empty vectors here so we don't seg fault later
                tFAWCountdown.push_back(vector<unsigned>());
        }
}
CommandQueue::~CommandQueue()
{
        //ERROR("COMMAND QUEUE destructor");
        size_t bankMax = NUM_RANKS;
        if (queuingStructure == PerRank) {
                bankMax = 1; 
        }
        for (size_t r=0; r< NUM_RANKS; r++)
        {
                for (size_t b=0; b<bankMax; b++) 
                {
                        for (size_t i=0; i<queues[r][b].size(); i++)
                        {
                                delete(queues[r][b][i]);
                        }
                        queues[r][b].clear();
                }
        }
}
//Adds a command to appropriate queue
void CommandQueue::enqueue(BusPacket *newBusPacket)
{
        unsigned rank = newBusPacket->rank;
        unsigned bank = newBusPacket->bank;
        if (queuingStructure==PerRank)
        {
                queues[rank][0].push_back(newBusPacket);
                if (queues[rank][0].size()>CMD_QUEUE_DEPTH)
                {
                        ERROR("== Error - Enqueued more than allowed in command queue");
                        ERROR("                                                Need to call .hasRoomFor(int numberToEnqueue, unsigned rank, unsigned bank) first");
                        exit(0);
                }
        }
        else if (queuingStructure==PerRankPerBank)
        {
                queues[rank][bank].push_back(newBusPacket);
                if (queues[rank][bank].size()>CMD_QUEUE_DEPTH)
                {
                        ERROR("== Error - Enqueued more than allowed in command queue");
                        ERROR("                                                Need to call .hasRoomFor(int numberToEnqueue, unsigned rank, unsigned bank) first");
                        exit(0);
                }
        }
        else
        {
                ERROR("== Error - Unknown queuing structure");
                exit(0);
        }
}

//Removes the next item from the command queue based on the system's
//command scheduling policy
bool CommandQueue::pop(BusPacket **busPacket)
{
        //this can be done here because pop() is called every clock cycle by the parent MemoryController
        //        figures out the sliding window requirement for tFAW
        //
        //deal with tFAW book-keeping
        //        each rank has it's own counter since the restriction is on a device level
        for (size_t i=0;i<NUM_RANKS;i++)
        {
                //decrement all the counters we have going
                for (size_t j=0;j<tFAWCountdown[i].size();j++)
                {
                        tFAWCountdown[i][j]--;
                }

                //the head will always be the smallest counter, so check if it has reached 0
                if (tFAWCountdown[i].size()>0 && tFAWCountdown[i][0]==0)
                {
                        tFAWCountdown[i].erase(tFAWCountdown[i].begin());
                }
        }

        /* Now we need to find a packet to issue. When the code picks a packet, it will set
                 *busPacket = [some eligible packet]
                 
                 First the code looks if any refreshes need to go
                 Then it looks for data packets
                 Otherwise, it starts looking for rows to close (in open page)
        */

        if (rowBufferPolicy==ClosePage)
        {
                bool sendingREF = false;
                //if the memory controller set the flags signaling that we need to issue a refresh
                if (refreshWaiting)
                {
                        bool foundActiveOrTooEarly = false;
                        //look for an open bank
                        for (size_t b=0;b<NUM_BANKS;b++)
                        {
                                vector<BusPacket *> &queue = getCommandQueue(refreshRank,b);
                                //checks to make sure that all banks are idle
                                if (bankStates[refreshRank][b].currentBankState == RowActive)
                                {
                                        foundActiveOrTooEarly = true;
                                        //if the bank is open, make sure there is nothing else
                                        // going there before we close it
                                        for (size_t j=0;j<queue.size();j++)
                                        {
                                                BusPacket *packet = queue[j];
                                                if (packet->row == bankStates[refreshRank][b].openRowAddress &&
                                                                packet->bank == b)
                                                {
                                                        if (packet->busPacketType != ACTIVATE && isIssuable(packet))
                                                        {
                                                                *busPacket = packet;
                                                                queue.erase(queue.begin() + j);
                                                                sendingREF = true;
                                                        }
                                                        break;
                                                }
                                        }

                                        break;
                                }
                                //        NOTE: checks nextActivate time for each bank to make sure tRP is being
                                //                                satisfied.        the next ACT and next REF can be issued at the same
                                //                                point in the future, so just use nextActivate field instead of
                                //                                creating a nextRefresh field
                                else if (bankStates[refreshRank][b].nextActivate > currentClockCycle)
                                {
                                        foundActiveOrTooEarly = true;
                                        break;
                                }
                        }

                        //if there are no open banks and timing has been met, send out the refresh
                        //        reset flags and rank pointer
                        if (!foundActiveOrTooEarly && bankStates[refreshRank][0].currentBankState != PowerDown)
                        {
                                *busPacket = new BusPacket(REFRESH, 0, 0, 0, refreshRank, 0, 0, dramsim_log);
                                refreshRank = -1;
                                refreshWaiting = false;
                                sendingREF = true;
                        }
                } // refreshWaiting

                //if we're not sending a REF, proceed as normal
                if (!sendingREF)
                {
                        bool foundIssuable = false;
                        unsigned startingRank = nextRank;
                        unsigned startingBank = nextBank;
                        do
                        {
                                vector<BusPacket *> &queue = getCommandQueue(nextRank, nextBank);
                                //make sure there is something in this queue first
                                //        also make sure a rank isn't waiting for a refresh
                                //        if a rank is waiting for a refesh, don't issue anything to it until the
                                //                refresh logic above has sent one out (ie, letting banks close)
                                if (!queue.empty() && !((nextRank == refreshRank) && refreshWaiting))
                                {
                                        if (queuingStructure == PerRank)
                                        {

                                                //search from beginning to find first issuable bus packet
                                                for (size_t i=0;i<queue.size();i++)
                                                {
                                                        if (isIssuable(queue[i]))
                                                        {
                                                                //check to make sure we aren't removing a read/write that is paired with an activate
                                                                if (i>0 && queue[i-1]->busPacketType==ACTIVATE &&
                                                                                queue[i-1]->physicalAddress == queue[i]->physicalAddress)
                                                                        continue;

                                                                *busPacket = queue[i];
                                                                queue.erase(queue.begin()+i);
                                                                foundIssuable = true;
                                                                break;
                                                        }
                                                }
                                        }
                                        else
                                        {
                                                if (isIssuable(queue[0]))
                                                {

                                                        //no need to search because if the front can't be sent,
                                                        // then no chance something behind it can go instead
                                                        *busPacket = queue[0];
                                                        queue.erase(queue.begin());
                                                        foundIssuable = true;
                                                }
                                        }

                                }

                                //if we found something, break out of do-while
                                if (foundIssuable) break;

                                //rank round robin
                                if (queuingStructure == PerRank)
                                {
                                        nextRank = (nextRank + 1) % NUM_RANKS;
                                        if (startingRank == nextRank)
                                        {
                                                break;
                                        }
                                }
                                else 
                                {
                                        nextRankAndBank(nextRank, nextBank);
                                        if (startingRank == nextRank && startingBank == nextBank)
                                        {
                                                break;
                                        }
                                }
                        }
                        while (true);

                        //if we couldn't find anything to send, return false
                        if (!foundIssuable) return false;
                }
        }
        else if (rowBufferPolicy==OpenPage)
        {
                bool sendingREForPRE = false;
                if (refreshWaiting)
                {
                        bool sendREF = true;
                        //make sure all banks idle and timing met for a REF
                        for (size_t b=0;b<NUM_BANKS;b++)
                        {
                                //if a bank is active we can't send a REF yet
                                if (bankStates[refreshRank][b].currentBankState == RowActive)
                                {
                                        sendREF = false;
                                        bool closeRow = true;
                                        //search for commands going to an open row
                                        vector <BusPacket *> &refreshQueue = getCommandQueue(refreshRank,b);

                                        for (size_t j=0;j<refreshQueue.size();j++)
                                        {
                                                BusPacket *packet = refreshQueue[j];
                                                //if a command in the queue is going to the same row . . .
                                                if (bankStates[refreshRank][b].openRowAddress == packet->row &&
                                                                b == packet->bank)
                                                {
                                                        // . . . and is not an activate . . .
                                                        if (packet->busPacketType != ACTIVATE)
                                                        {
                                                                closeRow = false;
                                                                // . . . and can be issued . . .
                                                                if (isIssuable(packet))
                                                                {
                                                                        //send it out
                                                                        *busPacket = packet;
                                                                        refreshQueue.erase(refreshQueue.begin()+j);
                                                                        sendingREForPRE = true;
                                                                }
                                                                break;
                                                        }
                                                        else //command is an activate
                                                        {
                                                                //if we've encountered another act, no other command will be of interest
                                                                break;
                                                        }
                                                }
                                        }

                                        //if the bank is open and we are allowed to close it, then send a PRE
                                        if (closeRow && currentClockCycle >= bankStates[refreshRank][b].nextPrecharge)
                                        {
                                                rowAccessCounters[refreshRank][b]=0;
                                                *busPacket = new BusPacket(PRECHARGE, 0, 0, 0, refreshRank, b, 0, dramsim_log);
                                                sendingREForPRE = true;
                                        }
                                        break;
                                }
                                //        NOTE: the next ACT and next REF can be issued at the same
                                //                                point in the future, so just use nextActivate field instead of
                                //                                creating a nextRefresh field
                                else if (bankStates[refreshRank][b].nextActivate > currentClockCycle) //and this bank doesn't have an open row
                                {
                                        sendREF = false;
                                        break;
                                }
                        }

                        //if there are no open banks and timing has been met, send out the refresh
                        //        reset flags and rank pointer
                        if (sendREF && bankStates[refreshRank][0].currentBankState != PowerDown)
                        {
                                *busPacket = new BusPacket(REFRESH, 0, 0, 0, refreshRank, 0, 0, dramsim_log);
                                refreshRank = -1;
                                refreshWaiting = false;
                                sendingREForPRE = true;
                        }
                }

                if (!sendingREForPRE)
                {
                        unsigned startingRank = nextRank;
                        unsigned startingBank = nextBank;
                        bool foundIssuable = false;
                        do // round robin over queues
                        {
                                vector<BusPacket *> &queue = getCommandQueue(nextRank,nextBank);
                                //make sure there is something there first
                                if (!queue.empty() && !((nextRank == refreshRank) && refreshWaiting))
                                {
                                        //search from the beginning to find first issuable bus packet
                                        for (size_t i=0;i<queue.size();i++)
                                        {
                                                BusPacket *packet = queue[i];
                                                if (isIssuable(packet))
                                                {
                                                        //check for dependencies
                                                        bool dependencyFound = false;
                                                        for (size_t j=0;j<i;j++)
                                                        {
                                                                BusPacket *prevPacket = queue[j];
                                                                if (prevPacket->busPacketType != ACTIVATE &&
                                                                                prevPacket->bank == packet->bank &&
                                                                                prevPacket->row == packet->row)
                                                                {
                                                                        dependencyFound = true;
                                                                        break;
                                                                }
                                                        }
                                                        if (dependencyFound) continue;

                                                        *busPacket = packet;

                                                        //if the bus packet before is an activate, that is the act that was
                                                        //        paired with the column access we are removing, so we have to remove
                                                        //        that activate as well (check i>0 because if i==0 then theres nothing before it)
                                                        if (i>0 && queue[i-1]->busPacketType == ACTIVATE)
                                                        {
                                                                rowAccessCounters[(*busPacket)->rank][(*busPacket)->bank]++;
                                                                // i is being returned, but i-1 is being thrown away, so must delete it here 
                                                                delete (queue[i-1]);

                                                                // remove both i-1 (the activate) and i (we've saved the pointer in *busPacket)
                                                                queue.erase(queue.begin()+i-1,queue.begin()+i+1);
                                                        }
                                                        else // there's no activate before this packet
                                                        {
                                                                //or just remove the one bus packet
                                                                queue.erase(queue.begin()+i);
                                                        }

                                                        foundIssuable = true;
                                                        break;
                                                }
                                        }
                                }

                                //if we found something, break out of do-while
                                if (foundIssuable) break;

                                //rank round robin
                                if (queuingStructure == PerRank)
                                {
                                        nextRank = (nextRank + 1) % NUM_RANKS;
                                        if (startingRank == nextRank)
                                        {
                                                break;
                                        }
                                }
                                else 
                                {
                                        nextRankAndBank(nextRank, nextBank); 
                                        if (startingRank == nextRank && startingBank == nextBank)
                                        {
                                                break;
                                        }
                                }
                        }
                        while (true);

                        //if nothing was issuable, see if we can issue a PRE to an open bank
                        //        that has no other commands waiting
                        if (!foundIssuable)
                        {
                                //search for banks to close
                                bool sendingPRE = false;
                                unsigned startingRank = nextRankPRE;
                                unsigned startingBank = nextBankPRE;

                                do // round robin over all ranks and banks
                                {
                                        vector <BusPacket *> &queue = getCommandQueue(nextRankPRE, nextBankPRE);
                                        bool found = false;
                                        //check if bank is open
                                        if (bankStates[nextRankPRE][nextBankPRE].currentBankState == RowActive)
                                        {
                                                for (size_t i=0;i<queue.size();i++)
                                                {
                                                        //if there is something going to that bank and row, then we don't want to send a PRE
                                                        if (queue[i]->bank == nextBankPRE &&
                                                                        queue[i]->row == bankStates[nextRankPRE][nextBankPRE].openRowAddress)
                                                        {
                                                                found = true;
                                                                break;
                                                        }
                                                }

                                                //if nothing found going to that bank and row or too many accesses have happend, close it
                                                if (!found || rowAccessCounters[nextRankPRE][nextBankPRE]==TOTAL_ROW_ACCESSES)
                                                {
                                                        if (currentClockCycle >= bankStates[nextRankPRE][nextBankPRE].nextPrecharge)
                                                        {
                                                                sendingPRE = true;
                                                                rowAccessCounters[nextRankPRE][nextBankPRE] = 0;
                                                                *busPacket = new BusPacket(PRECHARGE, 0, 0, 0, nextRankPRE, nextBankPRE, 0, dramsim_log);
                                                                break;
                                                        }
                                                }
                                        }
                                        nextRankAndBank(nextRankPRE, nextBankPRE);
                                }
                                while (!(startingRank == nextRankPRE && startingBank == nextBankPRE));

                                //if no PREs could be sent, just return false
                                if (!sendingPRE) return false;
                        }
                }
        }

        //sendAct is flag used for posted-cas
        //  posted-cas is enabled when AL>0
        //  when sendAct is true, when don't want to increment our indexes
        //  so we send the column access that is paid with this act
        if (AL>0 && sendAct)
        {
                sendAct = false;
        }
        else
        {
                sendAct = true;
                nextRankAndBank(nextRank, nextBank);
        }

        //if its an activate, add a tfaw counter
        if ((*busPacket)->busPacketType==ACTIVATE)
        {
                tFAWCountdown[(*busPacket)->rank].push_back(tFAW);
        }

        return true;
}

//check if a rank/bank queue has room for a certain number of bus packets
bool CommandQueue::hasRoomFor(unsigned numberToEnqueue, unsigned rank, unsigned bank)
{
        vector<BusPacket *> &queue = getCommandQueue(rank, bank); 
        return (CMD_QUEUE_DEPTH - queue.size() >= numberToEnqueue);
}

//prints the contents of the command queue
void CommandQueue::print()
{
        if (queuingStructure==PerRank)
        {
                PRINT(endl << "== Printing Per Rank Queue" );
                for (size_t i=0;i<NUM_RANKS;i++)
                {
                        PRINT(" = Rank " << i << "  size : " << queues[i][0].size() );
                        for (size_t j=0;j<queues[i][0].size();j++)
                        {
                                PRINTN("    "<< j << "]");
                                queues[i][0][j]->print();
                        }
                }
        }
        else if (queuingStructure==PerRankPerBank)
        {
                PRINT("\n== Printing Per Rank, Per Bank Queue" );

                for (size_t i=0;i<NUM_RANKS;i++)
                {
                        PRINT(" = Rank " << i );
                        for (size_t j=0;j<NUM_BANKS;j++)
                        {
                                PRINT("    Bank "<< j << "   size : " << queues[i][j].size() );

                                for (size_t k=0;k<queues[i][j].size();k++)
                                {
                                        PRINTN("       " << k << "]");
                                        queues[i][j][k]->print();
                                }
                        }
                }
        }
}

/** 
 * return a reference to the queue for a given rank, bank. Since we
 * don't always have a per bank queuing structure, sometimes the bank
 * argument is ignored (and the 0th index is returned 
 */
vector<BusPacket *> &CommandQueue::getCommandQueue(unsigned rank, unsigned bank)
{
        if (queuingStructure == PerRankPerBank)
        {
                return queues[rank][bank];
        }
        else if (queuingStructure == PerRank)
        {
                return queues[rank][0];
        }
        else
        {
                ERROR("Unknown queue structure");
                abort(); 
        }

}

//checks if busPacket is allowed to be issued
bool CommandQueue::isIssuable(BusPacket *busPacket)
{
        switch (busPacket->busPacketType)
        {
        case REFRESH:

                break;
        case ACTIVATE:
                if ((bankStates[busPacket->rank][busPacket->bank].currentBankState == Idle ||
                        bankStates[busPacket->rank][busPacket->bank].currentBankState == Refreshing) &&
                        currentClockCycle >= bankStates[busPacket->rank][busPacket->bank].nextActivate &&
                        tFAWCountdown[busPacket->rank].size() < 4)
                {
                        return true;
                }
                else
                {
                        return false;
                }
                break;
        case WRITE:
        case WRITE_P:
                if (bankStates[busPacket->rank][busPacket->bank].currentBankState == RowActive &&
                        currentClockCycle >= bankStates[busPacket->rank][busPacket->bank].nextWrite &&
                        busPacket->row == bankStates[busPacket->rank][busPacket->bank].openRowAddress &&
                        rowAccessCounters[busPacket->rank][busPacket->bank] < TOTAL_ROW_ACCESSES)
                {
                        return true;
                }
                else
                {
                        return false;
                }
                break;
        case READ_P:
        case READ:
                if (bankStates[busPacket->rank][busPacket->bank].currentBankState == RowActive &&
                        currentClockCycle >= bankStates[busPacket->rank][busPacket->bank].nextRead &&
                        busPacket->row == bankStates[busPacket->rank][busPacket->bank].openRowAddress &&
                        rowAccessCounters[busPacket->rank][busPacket->bank] < TOTAL_ROW_ACCESSES)
                {
                        return true;
                }
                else
                {
                        return false;
                }
                break;
        case PRECHARGE:
                if (bankStates[busPacket->rank][busPacket->bank].currentBankState == RowActive &&
                        currentClockCycle >= bankStates[busPacket->rank][busPacket->bank].nextPrecharge)
                {
                        return true;
                }
                else
                {
                        return false;
                }
                break;
        default:
                ERROR("== Error - Trying to issue a crazy bus packet type : ");
                busPacket->print();
                exit(0);
        }
        return false;
}

//figures out if a rank's queue is empty
bool CommandQueue::isEmpty(unsigned rank)
{
        if (queuingStructure == PerRank)
        {
                return queues[rank][0].empty();
        }
        else if (queuingStructure == PerRankPerBank)
        {
                for (size_t i=0;i<NUM_BANKS;i++)
                {
                        if (!queues[rank][i].empty()) return false;
                }
                return true;
        }
        else
        {
                DEBUG("Invalid Queueing Stucture");
                abort();
        }
}

//tells the command queue that a particular rank is in need of a refresh
void CommandQueue::needRefresh(unsigned rank)
{
        refreshWaiting = true;
        refreshRank = rank;
}

void CommandQueue::nextRankAndBank(unsigned &rank, unsigned &bank)
{
        if (schedulingPolicy == RankThenBankRoundRobin)
        {
                rank++;
                if (rank == NUM_RANKS)
                {
                        rank = 0;
                        bank++;
                        if (bank == NUM_BANKS)
                        {
                                bank = 0;
                        }
                }
        }
        //bank-then-rank round robin
        else if (schedulingPolicy == BankThenRankRoundRobin)
        {
                bank++;
                if (bank == NUM_BANKS)
                {
                        bank = 0;
                        rank++;
                        if (rank == NUM_RANKS)
                        {
                                rank = 0;
                        }
                }
        }
        else
        {
                ERROR("== Error - Unknown scheduling policy");
                exit(0);
        }

}

void CommandQueue::update()
{
        //do nothing since pop() is effectively update(),
        //needed for SimulatorObject
        //TODO: make CommandQueue not a SimulatorObject
}
          cmakeBuildDirectory: ${{ 





#ifndef CMDQUEUE_H
#define CMDQUEUE_H

//CommandQueue.h
//
//Header
//

#include "BusPacket.h"
#include "BankState.h"
#include "Transaction.h"
#include "SystemConfiguration.h"
#include "SimulatorObject.h"

using namespace std;

namespace DRAMSim
{
class CommandQueue : public SimulatorObject
{
        CommandQueue();
        ostream &dramsim_log;
public:
        //typedefs
        typedef vector<BusPacket *> BusPacket1D;
        typedef vector<BusPacket1D> BusPacket2D;
        typedef vector<BusPacket2D> BusPacket3D;

        //functions
        CommandQueue(vector< vector<BankState> > &states, ostream &dramsim_log);
        virtual ~CommandQueue(); 

        void enqueue(BusPacket *newBusPacket);
        bool pop(BusPacket **busPacket);
        bool hasRoomFor(unsigned numberToEnqueue, unsigned rank, unsigned bank);
        bool isIssuable(BusPacket *busPacket);
        bool isEmpty(unsigned rank);
        void needRefresh(unsigned rank);
        void print();
        void update(); //SimulatorObject requirement
        vector<BusPacket *> &getCommandQueue(unsigned rank, unsigned bank);

        //fields

        BusPacket3D queues; // 3D array of BusPacket pointers
        vector< vector<BankState> > &bankStates;
private:
        void nextRankAndBank(unsigned &rank, unsigned &bank);
        //fields
        unsigned nextBank;
        unsigned nextRank;

        unsigned nextBankPRE;
        unsigned nextRankPRE;

        unsigned refreshRank;
        bool refreshWaiting;

        vector< vector<unsigned> > tFAWCountdown;
        vector< vector<unsigned> > rowAccessCounters;

        bool sendAct;
};
}

#endif
 }}
          # Ruleset file that will determine what checks will be run
          ruleset: NativeRecommendedRules.ruleset

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: Upload SARIF to GitHub
        uses: github/codeql-action/upload-sarif@v2
        with: 

#ifndef DRAMSIM_H
#define DRAMSIM_H
/*
 * This is a public header for DRAMSim including this along with libdramsim.so should
 * provide all necessary functionality to talk to an external simulator
 */
#include "Callback.h"
#include <string>
using std::string;

namespace DRAMSim 
{

        class MultiChannelMemorySystem {
                public: 
                        bool addTransaction(bool isWrite, uint64_t addr);
                        void setCPUClockSpeed(uint64_t cpuClkFreqHz);
                        void update();
                        void printStats(bool finalStats);
                        bool willAcceptTransaction(); 
                        bool willAcceptTransaction(uint64_t addr); 
                        std::ostream &getLogFile();

                        void RegisterCallbacks( 
                                TransactionCompleteCB *readDone,
                                TransactionCompleteCB *writeDone,
                                void (*reportPower)(double bgpower, double burstpower, double refreshpower, double actprepower));
                        int getIniBool(const std::string &field, bool *val);
                        int getIniUint(const std::string &field, unsigned int *val);
                        int getIniUint64(const std::string &field, uint64_t *val);
                        int getIniFloat(const std::string &field, float *val);
        };
        MultiChannelMemorySystem *getMemorySystemInstance(const string &dev, const string &sys, const string &pwd, const string &trc, unsigned megsOfMemory, std::string *visfilename=NULL);
}

#endif
          sarif_file: ${{ 







#include "IniReader.h"

using namespace std;

// these are the values that are extern'd in SystemConfig.h so that they
// have global scope even though they are set by IniReader

uint64_t TOTAL_STORAGE;
unsigned NUM_BANKS;
unsigned NUM_BANKS_LOG;
unsigned NUM_CHANS;
unsigned NUM_CHANS_LOG;
unsigned NUM_ROWS;
unsigned NUM_ROWS_LOG;
unsigned NUM_COLS;
unsigned NUM_COLS_LOG;
unsigned DEVICE_WIDTH;
unsigned BYTE_OFFSET_WIDTH;
unsigned TRANSACTION_SIZE;
unsigned THROW_AWAY_BITS;
unsigned COL_LOW_BIT_WIDTH;

unsigned REFRESH_PERIOD;
float tCK;
float Vdd;
unsigned CL;
unsigned AL;
unsigned BL;
unsigned tRAS;
unsigned tRCD;
unsigned tRRD;
unsigned tRC;
unsigned tRP;
unsigned tCCD;
unsigned tRTP;
unsigned tWTR;
unsigned tWR;
unsigned tRTRS;
unsigned tRFC;
unsigned tFAW;
unsigned tCKE;
unsigned tXP;
unsigned tCMD;

unsigned IDD0;
unsigned IDD1;
unsigned IDD2P;
unsigned IDD2Q;
unsigned IDD2N;
unsigned IDD3Pf;
unsigned IDD3Ps;
unsigned IDD3N;
unsigned IDD4W;
unsigned IDD4R;
unsigned IDD5;
unsigned IDD6;
unsigned IDD6L;
unsigned IDD7;


//in bytes
unsigned JEDEC_DATA_BUS_BITS;

//Memory Controller related parameters
unsigned TRANS_QUEUE_DEPTH;
unsigned CMD_QUEUE_DEPTH;

//cycles within an epoch
unsigned EPOCH_LENGTH;

//row accesses allowed before closing (open page)
unsigned TOTAL_ROW_ACCESSES;

// strings and their associated enums
string ROW_BUFFER_POLICY;
string SCHEDULING_POLICY;
string ADDRESS_MAPPING_SCHEME;
string QUEUING_STRUCTURE;

bool DEBUG_TRANS_Q;
bool DEBUG_CMD_Q;
bool DEBUG_ADDR_MAP;
bool DEBUG_BANKSTATE;
bool DEBUG_BUS;
bool DEBUG_BANKS;
bool DEBUG_POWER;
bool USE_LOW_POWER;
bool VIS_FILE_OUTPUT;

bool VERIFICATION_OUTPUT;

bool DEBUG_INI_READER=false;

namespace DRAMSim
{
RowBufferPolicy rowBufferPolicy;
SchedulingPolicy schedulingPolicy;
AddressMappingScheme addressMappingScheme;
QueuingStructure queuingStructure;


//Map the string names to the variables they set
static ConfigMap configMap[] =
{
        //DEFINE_UINT_PARAM -- see IniReader.h
        DEFINE_UINT_PARAM(NUM_BANKS,DEV_PARAM),
        DEFINE_UINT_PARAM(NUM_ROWS,DEV_PARAM),
        DEFINE_UINT_PARAM(NUM_COLS,DEV_PARAM),
        DEFINE_UINT_PARAM(DEVICE_WIDTH,DEV_PARAM),
        DEFINE_UINT_PARAM(REFRESH_PERIOD,DEV_PARAM),
        DEFINE_FLOAT_PARAM(tCK,DEV_PARAM),
        DEFINE_UINT_PARAM(CL,DEV_PARAM),
        DEFINE_UINT_PARAM(AL,DEV_PARAM),
        DEFINE_UINT_PARAM(BL,DEV_PARAM),
        DEFINE_UINT_PARAM(tRAS,DEV_PARAM),
        DEFINE_UINT_PARAM(tRCD,DEV_PARAM),
        DEFINE_UINT_PARAM(tRRD,DEV_PARAM),
        DEFINE_UINT_PARAM(tRC,DEV_PARAM),
        DEFINE_UINT_PARAM(tRP,DEV_PARAM),
        DEFINE_UINT_PARAM(tCCD,DEV_PARAM),
        DEFINE_UINT_PARAM(tRTP,DEV_PARAM),
        DEFINE_UINT_PARAM(tWTR,DEV_PARAM),
        DEFINE_UINT_PARAM(tWR,DEV_PARAM),
        DEFINE_UINT_PARAM(tRTRS,DEV_PARAM),
        DEFINE_UINT_PARAM(tRFC,DEV_PARAM),
        DEFINE_UINT_PARAM(tFAW,DEV_PARAM),
        DEFINE_UINT_PARAM(tCKE,DEV_PARAM),
        DEFINE_UINT_PARAM(tXP,DEV_PARAM),
        DEFINE_UINT_PARAM(tCMD,DEV_PARAM),
        DEFINE_UINT_PARAM(IDD0,DEV_PARAM),
        DEFINE_UINT_PARAM(IDD1,DEV_PARAM),
        DEFINE_UINT_PARAM(IDD2P,DEV_PARAM),
        DEFINE_UINT_PARAM(IDD2Q,DEV_PARAM),
        DEFINE_UINT_PARAM(IDD2N,DEV_PARAM),
        DEFINE_UINT_PARAM(IDD3Pf,DEV_PARAM),
        DEFINE_UINT_PARAM(IDD3Ps,DEV_PARAM),
        DEFINE_UINT_PARAM(IDD3N,DEV_PARAM),
        DEFINE_UINT_PARAM(IDD4W,DEV_PARAM),
        DEFINE_UINT_PARAM(IDD4R,DEV_PARAM),
        DEFINE_UINT_PARAM(IDD5,DEV_PARAM),
        DEFINE_UINT_PARAM(IDD6,DEV_PARAM),
        DEFINE_UINT_PARAM(IDD6L,DEV_PARAM),
        DEFINE_UINT_PARAM(IDD7,DEV_PARAM),
        DEFINE_FLOAT_PARAM(Vdd,DEV_PARAM),

        DEFINE_UINT_PARAM(NUM_CHANS,SYS_PARAM),
        DEFINE_UINT_PARAM(JEDEC_DATA_BUS_BITS,SYS_PARAM),

        //Memory Controller related parameters
        DEFINE_UINT_PARAM(TRANS_QUEUE_DEPTH,SYS_PARAM),
        DEFINE_UINT_PARAM(CMD_QUEUE_DEPTH,SYS_PARAM),

        DEFINE_UINT_PARAM(EPOCH_LENGTH,SYS_PARAM),
        //Power
        DEFINE_BOOL_PARAM(USE_LOW_POWER,SYS_PARAM),

        DEFINE_UINT_PARAM(TOTAL_ROW_ACCESSES,SYS_PARAM),
        DEFINE_STRING_PARAM(ROW_BUFFER_POLICY,SYS_PARAM),
        DEFINE_STRING_PARAM(SCHEDULING_POLICY,SYS_PARAM),
        DEFINE_STRING_PARAM(ADDRESS_MAPPING_SCHEME,SYS_PARAM),
        DEFINE_STRING_PARAM(QUEUING_STRUCTURE,SYS_PARAM),
        // debug flags
        DEFINE_BOOL_PARAM(DEBUG_TRANS_Q,SYS_PARAM),
        DEFINE_BOOL_PARAM(DEBUG_CMD_Q,SYS_PARAM),
        DEFINE_BOOL_PARAM(DEBUG_ADDR_MAP,SYS_PARAM),
        DEFINE_BOOL_PARAM(DEBUG_BANKSTATE,SYS_PARAM),
        DEFINE_BOOL_PARAM(DEBUG_BUS,SYS_PARAM),
        DEFINE_BOOL_PARAM(DEBUG_BANKS,SYS_PARAM),
        DEFINE_BOOL_PARAM(DEBUG_POWER,SYS_PARAM),
        DEFINE_BOOL_PARAM(VIS_FILE_OUTPUT,SYS_PARAM),
        DEFINE_BOOL_PARAM(VERIFICATION_OUTPUT,SYS_PARAM),
        {"", NULL, UINT, SYS_PARAM, false} // tracer value to signify end of list; if you delete it, epic fail will result
};

void IniReader::WriteParams(std::ofstream &visDataOut, paramType type)
{
        for (size_t i=0; configMap[i].variablePtr != NULL; i++)
        {
                if (configMap[i].parameterType == type)
                {
                        visDataOut<<configMap[i].iniKey<<"=";
                        switch (configMap[i].variableType)
                        {
                                //parse and set each type of variable
                        case UINT:
                                visDataOut << *((unsigned *)configMap[i].variablePtr);
                                break;
                        case UINT64:
                                visDataOut << *((uint64_t *)configMap[i].variablePtr);
                                break;
                        case FLOAT:
                                visDataOut << *((float *)configMap[i].variablePtr);
                                break;
                        case STRING:
                                visDataOut << *((string *)configMap[i].variablePtr);
                                break;
                        case BOOL:
                                if (*((bool *)configMap[i].variablePtr))
                                {
                                        visDataOut <<"true";
                                }
                                else
                                {
                                        visDataOut <<"false";
                                }
                                break;
                        }
                        visDataOut << endl;
                }
        }
        if (type == SYS_PARAM)
        {
                visDataOut<<"NUM_RANKS="<<NUM_RANKS <<"\n";
        }
}
void IniReader::WriteValuesOut(std::ofstream &visDataOut)
{
        visDataOut<<"!!SYSTEM_INI"<<endl;

        WriteParams(visDataOut, SYS_PARAM); 
        visDataOut<<"!!DEVICE_INI"<<endl;

        WriteParams(visDataOut, DEV_PARAM); 
        visDataOut<<"!!EPOCH_DATA"<<endl;

}

void IniReader::SetKey(string key, string valueString, bool isSystemParam, size_t lineNumber)
{
        size_t i;
        unsigned intValue;
        uint64_t int64Value;
        float floatValue;

        for (i=0; configMap[i].variablePtr != NULL; i++)
        {
                istringstream iss(valueString);
                // match up the string in the config map with the key we parsed
                if (key.compare(configMap[i].iniKey) == 0)
                {
                        switch (configMap[i].variableType)
                        {
                                //parse and set each type of variable
                        case UINT:
                                if ((iss >> dec >> intValue).fail())
                                {
                                        ERROR("could not parse line "<<lineNumber<<" (non-numeric value '"<<valueString<<"')?");
                                }
                                *((unsigned *)configMap[i].variablePtr) = intValue;
                                if (DEBUG_INI_READER)
                                {
                                        DEBUG("\t - SETTING "<<configMap[i].iniKey<<"="<<intValue);
                                }
                                break;
                        case UINT64:
                                if ((iss >> dec >> int64Value).fail())
                                {
                                        ERROR("could not parse line "<<lineNumber<<" (non-numeric value '"<<valueString<<"')?");
                                }
                                *((uint64_t *)configMap[i].variablePtr) = int64Value;
                                if (DEBUG_INI_READER)
                                {
                                        DEBUG("\t - SETTING "<<configMap[i].iniKey<<"="<<int64Value);
                                }
                                break;
                        case FLOAT:
                                if ((iss >> dec >> floatValue).fail())
                                {
                                        ERROR("could not parse line "<<lineNumber<<" (non-numeric value '"<<valueString<<"')?");
                                }
                                *((float *)configMap[i].variablePtr) = floatValue;
                                if (DEBUG_INI_READER)
                                {
                                        DEBUG("\t - SETTING "<<configMap[i].iniKey<<"="<<floatValue);
                                }
                                break;
                        case STRING:
                                *((string *)configMap[i].variablePtr) = string(valueString);
                                if (DEBUG_INI_READER)
                                {
                                        DEBUG("\t - SETTING "<<configMap[i].iniKey<<"="<<valueString);
                                }

                                break;
                        case BOOL:
                                if (valueString == "true" || valueString == "1")
                                {
                                        *((bool *)configMap[i].variablePtr) = true;
                                }
                                else
                                {
                                        *((bool *)configMap[i].variablePtr) = false;
                                }
                        }
                        // lineNumber == 0 implies that this is an override parameter from the command line, so don't bother doing these checks
                        if (lineNumber > 0)
                        {
                                if (isSystemParam && configMap[i].parameterType == DEV_PARAM)
                                {
                                        DEBUG("WARNING: Found device parameter "<<configMap[i].iniKey<<" in system config file");
                                }
                                else if (!isSystemParam && configMap[i].parameterType == SYS_PARAM)
                                {
                                        DEBUG("WARNING: Found system parameter "<<configMap[i].iniKey<<" in device config file");
                                }
                        }
                        // use the pointer stored in the config map to set the value of the variable
                        // to make sure all parameters are in the ini file
                        configMap[i].wasSet = true;
                        break;
                }
        }

        if (configMap[i].variablePtr == NULL)
        {
                DEBUG("WARNING: UNKNOWN KEY '"<<key<<"' IN INI FILE");
        }
}

void IniReader::ReadIniFile(string filename, bool isSystemFile)
{
        ifstream iniFile;
        string line;
        string key,valueString;

        size_t commentIndex, equalsIndex;
        size_t lineNumber=0;

        iniFile.open(filename.c_str());
        if (iniFile.is_open())
        {
                while (!iniFile.eof())
                {
                        lineNumber++;
                        getline(iniFile, line);
                        //this can happen if the filename is actually a directory
                        if (iniFile.bad())
                        {
                                ERROR("Cannot read ini file '"<<filename<<"'");
                                exit(-1);
                        }
                        // skip zero-length lines
                        if (line.size() == 0)
                        {
//                                        DEBUG("Skipping blank line "<<lineNumber);
                                continue;
                        }
                        //search for a comment char
                        if ((commentIndex = line.find_first_of(";")) != string::npos)
                        {
                                //if the comment char is the first char, ignore the whole line
                                if (commentIndex == 0)
                                {
//                                                DEBUG("Skipping comment line "<<lineNumber);
                                        continue;
                                }
//                                        DEBUG("Truncating line at comment"<<line[commentIndex-1]);
                                //truncate the line at first comment before going on
                                line = line.substr(0,commentIndex);
                        }
                        // trim off the end spaces that might have been between the value and comment char
                        size_t whiteSpaceEndIndex;
                        if ((whiteSpaceEndIndex = line.find_last_not_of(" \t")) != string::npos)
                        {
                                line = line.substr(0,whiteSpaceEndIndex+1);
                        }

                        // at this point line should be a valid, commentless string

                        // a line has to have an equals sign
                        if ((equalsIndex = line.find_first_of("=")) == string::npos)
                        {
                                ERROR("Malformed Line "<<lineNumber<<" (missing equals)");
                                abort();
                        }
                        size_t strlen = line.size();
                        // all characters before the equals are the key
                        key = line.substr(0, equalsIndex);
                        // all characters after the equals are the value
                        valueString = line.substr(equalsIndex+1,strlen-equalsIndex);

                        IniReader::SetKey(key, valueString, isSystemFile, lineNumber);
                        // got to the end of the config map without finding the key
                }
        }
        else
        {
                ERROR ("Unable to load ini file "<<filename);
                abort();
        }
        /* precompute frequently used values */
        NUM_BANKS_LOG                = dramsim_log2(NUM_BANKS);
        NUM_CHANS_LOG                = dramsim_log2(NUM_CHANS);
        NUM_ROWS_LOG                = dramsim_log2(NUM_ROWS);
        NUM_COLS_LOG                = dramsim_log2(NUM_COLS);
        BYTE_OFFSET_WIDTH        = dramsim_log2(JEDEC_DATA_BUS_BITS / 8);
        TRANSACTION_SIZE        = JEDEC_DATA_BUS_BITS / 8 * BL;
        THROW_AWAY_BITS                = dramsim_log2(TRANSACTION_SIZE);
        COL_LOW_BIT_WIDTH        = THROW_AWAY_BITS - BYTE_OFFSET_WIDTH;
}

void IniReader::OverrideKeys(const OverrideMap *map)
{
        if (!map)
                return; 

        OverrideIterator it = map->begin();
        DEBUG("Key overrides from command line:"); 
        for (it=map->begin(); it != map->end(); it++)
        {
                string key = it->first; 
                string value = it->second; 
                DEBUG("\t'"<< key <<"'->'"<< value<< "'"); 
                IniReader::SetKey(key,value);
        }
}

bool IniReader::CheckIfAllSet()
{
        // check to make sure all parameters that we exepected were set
        for (size_t i=0; configMap[i].variablePtr != NULL; i++)
        {
                if (!configMap[i].wasSet)
                {
                        DEBUG("WARNING: KEY "<<configMap[i].iniKey<<" NOT FOUND IN INI FILE.");
                        switch (configMap[i].variableType)
                        {
                                //the string and bool values can be defaulted, but generally we need all the numeric values to be set to continue
                        case UINT:
                        case UINT64:
                        case FLOAT:
                                ERROR("Cannot continue without key '"<<configMap[i].iniKey<<"' set.");
                                return false;
                                break;
                        case BOOL:
                                *((bool *)configMap[i].variablePtr) = false;
                                DEBUG("\tSetting Default: "<<configMap[i].iniKey<<"=false");
                                break;
                        case STRING:
                                break;
                        }
                }
        }
        return true;
}

/*
 * There is probably a way of doing this with templates, but since
 * we have the types defined as an enum, doing this with macros is trivial.
 *
 * Return value: 0 on success, -1 on error
 */
#define DEF_GETTER(_funcname, _type, _typename)                                \
        int _funcname(const std::string& field, _type *val)                \
        {                                                                \
                int i;                                                        \
                                                                        \
                for (i=0; configMap[i].variablePtr != NULL; i++)        \
                {                                                        \
                        if (field.compare(configMap[i].iniKey))                \
                                continue;                                \
                        if (configMap[i].variableType != _typename)        \
                                return -1;                                \
                        *val = *(_type *)configMap[i].variablePtr;        \
                        return 0;                                        \
                }                                                        \
                return -1;                                                \
        }

/* TODO: getter for strings is missing. Probably not that useful though */
DEF_GETTER(IniReader::getBool, bool, BOOL)
DEF_GETTER(IniReader::getUint, unsigned int, UINT)
DEF_GETTER(IniReader::getUint64, uint64_t, UINT64)
DEF_GETTER(IniReader::getFloat, float, FLOAT)

void IniReader::InitEnumsFromStrings()
{
        if (ADDRESS_MAPPING_SCHEME == "scheme1")
        {
                addressMappingScheme = Scheme1;
                if (DEBUG_INI_READER) 
                {
                        DEBUG("ADDR SCHEME: 1");
                }
        }
        else if (ADDRESS_MAPPING_SCHEME == "scheme2")
        {
                addressMappingScheme = Scheme2;
                if (DEBUG_INI_READER) 
                {
                        DEBUG("ADDR SCHEME: 2");
                }
        }
        else if (ADDRESS_MAPPING_SCHEME == "scheme3")
        {
                addressMappingScheme = Scheme3;
                if (DEBUG_INI_READER) 
                {
                        DEBUG("ADDR SCHEME: 3");
                }
        }
        else if (ADDRESS_MAPPING_SCHEME == "scheme4")
        {
                addressMappingScheme = Scheme4;
                if (DEBUG_INI_READER) 
                {
                        DEBUG("ADDR SCHEME: 4");
                }
        }
        else if (ADDRESS_MAPPING_SCHEME == "scheme5")
        {
                addressMappingScheme = Scheme5;
                if (DEBUG_INI_READER) 
                {
                        DEBUG("ADDR SCHEME: 5");
                }
        }
        else if (ADDRESS_MAPPING_SCHEME == "scheme6")
        {
                addressMappingScheme = Scheme6;
                if (DEBUG_INI_READER) 
                {
                        DEBUG("ADDR SCHEME: 6");
                }
        }
        else if (ADDRESS_MAPPING_SCHEME == "scheme7")
        {
                addressMappingScheme = Scheme7;
                if (DEBUG_INI_READER) 
                {
                        DEBUG("ADDR SCHEME: 7");
                }
        }
        else
        {
                cout << "WARNING: unknown address mapping scheme '"<<ADDRESS_MAPPING_SCHEME<<"'; valid values are 'scheme1'...'scheme7'. Defaulting to scheme1"<<endl;
                addressMappingScheme = Scheme1;
        }

        if (ROW_BUFFER_POLICY == "open_page")
        {
                rowBufferPolicy = OpenPage;
                if (DEBUG_INI_READER) 
                {
                        DEBUG("ROW BUFFER: open page");
                }
        }
        else if (ROW_BUFFER_POLICY == "close_page")
        {
                rowBufferPolicy = ClosePage;
                if (DEBUG_INI_READER) 
                {
                        DEBUG("ROW BUFFER: close page");
                }
        }
        else
        {
                cout << "WARNING: unknown row buffer policy '"<<ROW_BUFFER_POLICY<<"'; valid values are 'open_page' or 'close_page', Defaulting to Close Page."<<endl;
                rowBufferPolicy = ClosePage;
        }

        if (QUEUING_STRUCTURE == "per_rank_per_bank")
        {
                queuingStructure = PerRankPerBank;
                if (DEBUG_INI_READER) 
                {
                        DEBUG("QUEUING STRUCT: per rank per bank");
                }
        }
        else if (QUEUING_STRUCTURE == "per_rank")
        {
                queuingStructure = PerRank;
                if (DEBUG_INI_READER) 
                {
                        DEBUG("QUEUING STRUCT: per rank");
                }
        }
        else
        {
                cout << "WARNING: Unknown queueing structure '"<<QUEUING_STRUCTURE<<"'; valid options are 'per_rank' and 'per_rank_per_bank', defaulting to Per Rank Per Bank"<<endl;
                queuingStructure = PerRankPerBank;
        }

        if (SCHEDULING_POLICY == "rank_then_bank_round_robin")
        {
                schedulingPolicy = RankThenBankRoundRobin;
                if (DEBUG_INI_READER) 
                {
                        DEBUG("SCHEDULING: Rank Then Bank");
                }
        }
        else if (SCHEDULING_POLICY == "bank_then_rank_round_robin")
        {
                schedulingPolicy = BankThenRankRoundRobin;
                if (DEBUG_INI_READER) 
                {
                        DEBUG("SCHEDULING: Bank Then Rank");
                }
        }
        else
        {
                cout << "WARNING: Unknown scheduling policy '"<<SCHEDULING_POLICY<<"'; valid options are 'rank_then_bank_round_robin' or 'bank_then_rank_round_robin'; defaulting to Bank Then Rank Round Robin" << endl;
                schedulingPolicy = BankThenRankRoundRobin;
        }

}

} // namespace DRAMSim }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v3
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
