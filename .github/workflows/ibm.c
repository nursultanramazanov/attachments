# This workflow will build a docker container, publish it to IBM Container Registry, and deploy it to IKS when there is a push to the "main" branch.
#
# To configure this workflow:
#
# 1. Ensure that your repository contains a Dockerfile
# 2. Setup secrets in your repository by going to settings: Create ICR_NAMESPACE and IBM_CLOUD_API_KEY
# 3. Change the values for the IBM_CLOUD_REGION, REGISTRY_HOSTNAME, IMAGE_NAME, IKS_CLUSTER, DEPLOYMENT_NAME, and PORT

name: Build and Deploy to IKS

on: <?php

namespace VisitCounter\Db;

interface DbAdapterInterface
{
    public function save(array $data);
}
  push: <?php

namespace VisitCounter\Db;

class PdoAdapter implements DbAdapterInterface
{
    private $connection;

    protected $pk;
    protected $tblName;
    protected $colName;

    public function __construct($connection, $tblName, $colName, $pk = 'id')
    {
        $this->connection = $connection;
        $this->tblName = $tblName;
        $this->colName = $colName;
        $this->pk = $pk;
    }

    public function save(array $visitsPages)
    {
        if (!$this->tblName or !$this->colName) {
            $message = "Properties tblName and colName are mandatory.";
            throw new \VisitCounter\Exception\RedisException($message);
        }
        try {
            foreach ($visitsPages as $visitCount => $pages) {
                $pageList = implode(',', $pages);
                $sql = "UPDATE {$this->tblName}
                        SET {$this->colName} = {$this->colName} + $visitCount
                        WHERE {$this->pk} IN ({$pageList})";
                $sth = $this->connection->prepare($sql);
                $sth->execute();
            }
        } catch (\PDOException $e) {
            throw new \VisitCounter\Exception\DbException($e->getMessage(), 0, $e);
        }
    }
}
    branches: [ "main" ]

# Environment variables available to all jobs and steps in this workflow
env: <?php

namespace VisitCounter\Exception;

class DbException extends \Exception
{
}
  GITHUB_SHA: <?php

namespace VisitCounter\Exception;

class RedisException extends \Exception
{
}
  IBM_CLOUD_API_KEY: <?php

namespace VisitCounter\Redis;

interface RedisAdapterInterface
{
    public function setnx($keyName, $expire, $value = '');
    public function rpush($listName, $value);
    public function llen($listName);
    public function lrange($listName, $start = 0, $end = -1);
    public function ltrim($listName, $start, $end = -1);
    public function hincrby($hashName, $field, $count = 1);
    public function hmget($hashName, array $fields);
}
  IBM_CLOUD_REGION: <?php

namespace VisitCounter\Redis;

class RediskaAdapter implements RedisAdapterInterface
{
    private $client;

    public function __construct(\Rediska $client)
    {
        $this->client = $client;
    }

    public function setnx($keyName, $expire, $value = '')
    {
        $command = new \Rediska_Command_Set(
            $this->client,
            'Set',
            array($keyName, $value, false)
        );
        try {
            if ( !$command->execute() ) return false;
            $key = new \Rediska_Key($keyName);
            $key->expire($expire);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function rpush($listName, $value)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $key->append($value);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function llen($listName)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $length = $key->getLength();
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return $length;
    }

    public function lrange($listName, $start = 0, $end = -1)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $result = $key->getValues($start, $end);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return $result;
    }

    public function ltrim($listName, $start = 0, $end = -1)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $key->truncate($start, $end);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function hincrby($hashName, $field, $count = 1)
    {
        $key = new \Rediska_Key_Hash($hashName);
        try {
            $key->increment($field, $count);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function hmget($hashName, array $fields)
    {
        $key = new \Rediska_Key_Hash($hashName);
        try {
            $result = $key->get($fields);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return array_combine($fields, $result);
    }
}
  ICR_NAMESPACE: <?php

class Post
{
    public $id;
    public $title;
    public $message;
    public $visits;
}
  REGISTRY_HOSTNAME: <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Document</title>
    <link rel="stylesheet" href="/css/style.css">
</head>
<body>
    <h1><?php echo $post->title;?></h1>
    <p><?php echo $post->message;?></p>
    <div class="counter">Unique visitors: <?php echo $totalVisits;?></div>
    <form action="" method="POST">
        <input type="submit" value="Save visits">
    </form>
    <p class="message"><?php echo $message;?></p>
</body>
</html>
  IMAGE_NAME: body {
    width: 400px;
    margin: auto;
}
.counter {
    border: 1px solid red;
    padding: 4px;
    float: left;
}
input {
    padding: 4px;
    float: left;
    margin: 0 20px 20px;
}
.message {
    clear: both;
    font-style: italic;
    color: 555;
}
  IKS_CLUSTER: #include  <msp430xG46x.h>
/*Group #7                                                                */
/*,                                                          */
/*,                                                           */
/*,                                                             */
;-------------------------------------------------------------------------------
            RSEG    CSTACK                  ; Define stack segment
;-------------------------------------------------------------------------------
            RSEG    CODE                    ; Assemble to Flash memory
;-----------------------------------------------------------------------------
RESET       mov.w   #SFE(CSTACK),SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW+WDTHOLD,&WDTCTL  ; Stop WDT
SetupFLL    bis.b   #XCAP14PF,&FLL_CTL0     ; Configure load caps

OFIFGcheck  bic.b   #OFIFG,&IFG1            ; Clear OFIFG
            mov.w   #047FFh,R15             
OFIFGwait   dec.w   R15                     ; Wait for OFIFG to set again if
            jnz     OFIFGwait                                ; not stable yet
            bit.b   #OFIFG,&IFG1            ; Has it set again?
            jnz     OFIFGcheck              ; If so, wait some more

SetupBT     mov.b #027h,&BTCTL   ; Basic Timer1 Control Register. 2seconds
            mov.b #BTIE,&IE2              ; Enable Basic Timer interrupt
SetupPx     bis.b #0ffh,&P6DIR      ; P6.x output
            bic.b #0ffh,&P6OUT              ; clear PORT6
            bis.b #0ffh,&P5DIR              ; P5.x output
            bic.b #0ffh,&P5OUT              ; clear PORT5
            bis.b #0ffh,&P4OUT              ; P4.x output
            bic.b #0ffh,&P4OUT              ; clear PORT4
            bis.b #0ffh,&P3OUT              ; P3.x output
            bic.b #0ffh,&P3OUT              ; clear PORT3
            bis.b #0ffh,&P2OUT              ; P2.x output
            bic.b #0ffh,&P2OUT              ; clear PORT2
                bis.b #0ffh,&P1OUT              ; P1.x output
            bic.b #0ffh,&P1OUT              ; clear PORT1                                                                                                                                          
Mainloop   
            bis.b #GIE+LPM3,SR              ; Enter LPM3, enable interrupts
            bis.b   #02h,&P5OUT              ; Set P5.1
            PUSH.w #2000          ; Push Delay 2000 to TOS
Pulse       
            dec.w SP              ; Decrement Value at TOS
            jnz Pulse              ; Delay done?
            incd.w SP              ; Increment destination twice
            bic.b #02h,&P5OUT              ; Reset P5.1
            jmp     Mainloop                ;
                                            ;
;------------------------------------------------------------------------------
BT_ISR;     Exit LPM3 on reti
;------------------------------------------------------------------------------
            bic.w   #LPM3,0(SP)             ;
            reti                            ;                
                                            ;
;-----------------------------------------------------------------------------
            COMMON  INTVEC                  ; Interrupt Vectors
;-----------------------------------------------------------------------------
            ORG     RESET_VECTOR            ; RESET Vector
            DW      RESET                   ;
            ORG     BASICTIMER_VECTOR       ; Basic Timer Vector
            DW      BT_ISR                  ;
            END
  DEPLOYMENT_NAME: */
#include <stdio.h>
void main()
{
        int num1, num2, result, i;
        do
        {
                printf("Choose 2 positive numbers you want to multiply\n");
                printf("Enter first number\n");
                scanf("%d", &num2);
                printf("Enter second number\n");
                scanf("%d", &num1);
                if (num1 < 0 || num2 < 0)//If the user has entered one or more negative numbers.We will loop until positive values are entered.
                {
                        printf("\nERROR:Try again\nNote: Write down only positive numbers\n\n");
                }
                result = 0;
        } while (num1 < 0 || num2 < 0);

        for (i = 1; i <= num2; i++)//Sum all of the addition num1 by itself X times num2
                result += num1;
        printf("\nthe result is:%d", result);
}
  PORT: */
#include <stdio.h>
#include <math.h>
void main()
{
        int num1, num2, number, i, check;
        double sq;
        do
        {
                printf("Please select a range of positive numbers from low to high\n\n");
                printf("Plese enter the first positive number\n");
                scanf("%d", &num1);
                printf("Please enter the positive second number\n");
                scanf("%d", &num2);
                if (num1<0 || num2<0)
                        printf("ERROR:You can try again\nNote: Write down only positive numbers\n\n");
        } while (num1 < 0 || num2 < 0);

        if (num1 > num2)
        {
                printf("You put a higher number in the first place but I will help you! :)\n");//change num1 with num2
                num1 = num1 - num2;
                num2 = num1 + num2;
                num1 = num2 - num1;
        }
        printf("The prime numbers between %d to %d is: ", num1, num2);
        for (number = num1; number <= num2; number++)//run number between num1 to num2
        {
                sq = sqrt(number);//I don't want to run on all the numbers. So, as far as we know, there must be at least one divider between 1 and the root of the number.
                check = 1;
                for (i = 2; i <= (int) sq; i++)//i do casting to get the integer number of the sqrt
                {
                        if ((number%i) == 0)//If a number is divided once, it is not a prime number.
                        {
                                check = 0;
                                break;//We don't have to keep looking for prime number.
                        }
                }
                if (check == 1 && number != 1)//If we dont enter to loop above so the number not divded and the number is never included becuse is not prime number.
                {
                        printf("%d ", number);
                }
        }
}

jobs: */
#include <stdio.h>
int fib(int);
void main()
{
        int i, num;
        do
        {
                printf("choose number in N place to get fibanochi number\n");
                scanf("%d", &num);
                if (num<0)
                        printf("ERROR:You can try again\nNote: Write down only positive numbers\n\n");
        } while (num < 0);
        printf("F%d=%d\n", num, fib(num));
}
int fib(int fn)
{
        int fnow = 0, fnext = 1, nextplus1, i;
        if (fn == 0)
                return fnow;
        for (i = 1; i < fn; i++)
        {
                nextplus1 = fnow + fnext;//F(n+1)=f(n-1)+F(n)
                fnow = fnext;//F(n-1)=F(n)
                fnext = nextplus1;//F(n)=F(n+1)
        }
        return fnext;
}
  setup-build-publish-deploy:  */
#include <stdio.h>
#include <math.h>
int perfect(int);//prototype
void main()
{
        int i, j;
        printf("The perfect number between 1 to 1000 is: \n");
        for (i = 1, j = 1; i <= 1000; ++i)//run from 1 to 1000
        {
                if (perfect(i) == i)//If the result of the function is equal to the same i, we will print the perfect number.
                {
                        printf("%d)%d\n", j, i);
                        j = j++;
                }
        }

}
int perfect(number)
{
        int i, export = 0;
        for (i = 1; i <= number / 2; i++)//run from 1 to half of the i number that we recive from the main.
        {
                if (number%i == 0)//if the number we get devision with the i --> add it to the variable epxort.
                {
                        export += i;
                }
        }
        return export;
}
    name: */
#include <stdio.h>
#define SIZE 10
void SelectionSort(int [],int);//prototype-return nothing
int RemoveDup(int [],int);//prototype-return number
void main()
{
        int listarray[SIZE], i, count;
        printf("Plese insert numbers to the array\n");
        for (i = 0;i < SIZE;i++)
        {
                printf("a[%d]=", i);
                scanf("%d", &listarray[i]);
        }
        SelectionSort(listarray, SIZE);
        printf("\nsort of the arry\n");//Print short array
        for (i = 0;i < SIZE;i++)
                printf("a[%d]=%d\n", i, listarray[i]);
        count = RemoveDup(listarray, SIZE);//recive the result from the function
        printf("\nafter remove duplicates\n");//Print after remove duplicate numbers
        for (i = 0;i < count;i++)
                printf("a[%d]=%d\n", i, listarray[i]);
        printf("\n%d duplicates numbers are remove\n", (SIZE - count));
}
void SelectionSort(int a[], int n)
{
        int i, j, min, swap;
        for (i = 0; i < n - 1; i++)
        {
                min = i;//min number coordinate
                for (j = i + 1; j < n; j++)
                        if (a[j] < a[min])//scan the min number
                                min = j;//the new min coordinate
                swap = a[min];//swap with the first number with the min number loction
                a[min] = a[i];
                a[i] = swap;
        }
}
int RemoveDup(int a[], int n)
{
        int i, count = 0;
        for (i = 0;i < n;i++)
                if (a[count] != a[i + 1])//if its false go to the next loop and move it to the left
                {
                        a[count + 1] = a[i + 1];//number move to the left and remove the duplicate number
                        count++;//Count the number that not duplicate
                }
        return count;
}
    runs-on: */
#include <stdio.h>
#define COLS 5//Number of Columns
#define ROWS 5//Number of Rows
int Check(int[][COLS], int, int);//prototype
void main()
{
        int arry[ROWS][COLS], i, j, result;
        printf("Please write down the numbers of the matrix or table\n");
        for (i = 0; i < ROWS; i++)//Initialize array by user
                for (j = 0; j < COLS; j++)
                {
                        printf("Row %d Column %d\narry[%d][%d]=", i, j, i, j);
                        scanf("%d", &arry[i][j]);
                        printf("\n");
                }
        result = Check(arry, ROWS, COLS);
        if (result == 0)
                printf("More than one row or column of the array, not sorted in ascending order\n");
        if (result == 1)
                printf("One of the rows or columns of the array are not sorted in ascending order\n");
        if (result == 2)
                printf("All rows and all columns of the array are sorted in ascending order\n");
}
int Check(int a[][COLS], int rows, int cols)
{
        int i, j, x = 0;
        for (i = 0;i < rows;i++)//Rows check, As long as X is different from one, I don't want the loop to continue working(double break)
                for (j = 0; j < cols - 1; j++)
                        if (a[i][j] >= a[i][j + 1])//the "if" Checks if the number from left in the row bigger from the right number
                        {
                                x++;//add to the result
                                goto end1;//i use "goto" because i want a double break and not continue to check
                        }
        end1:;
        for (i = 0;i < rows - 1;i++)//Columns check,As long as X<=1, I don't want the loop to continue working (double break)
                for (j = 0; j < cols; j++)
                        if (a[i][j] >= a[i + 1][j])//the "if" Checks if the number from the top in the column bigger from the low number
                        {
                                x++;//add to the result
                                if (x >= 2)//fast exit
                                        return 0;
                                goto end2;//i use "goto" because i want a double break and not continue to check
                        }
        end2:;
        if (x == 1)
                return 1;
        return 2;
}
    environment: */
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#define N 1001
void main()
{
        int i, j;
        printf("All the prime numbers between 1 to 1000:\n\n");
        bool Is_prime[N] = {false};
        for (i = 2;i < N;i++)//Initialize all array to true
                Is_prime[i] = true;
        for (i = 2;i <= (int)sqrt(N);i++)//check up to a square root of 1000
                if (Is_prime[i])//Checks if the number has already become False
                        for (j = i;j*i < N;j++)//Changes all multiples of the current number
                                Is_prime[j*i] = false;
        j = 0;//j is now number counter
        for (i = 2;i < N;i++)//Check the number that left True
                if (Is_prime[i])
                {
                        printf(" %3d", i);//Print the prime number
                        j++;
                        if((j%10)==0)//For a better look
                                printf("\n");
                }
        printf("\n\n%d Prime numbers\n\n",j);//Number of prime number
}
    steps: */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
void GetDate(char *, int *, int *, int *);
void main()
{
        char date[10];
        int day, month, year;
        printf("enter a date as dd-mmm-yy\n");
        gets(date);
        GetDate(date, &day, &month, &year);//Call function.
        printf("Day = %d\n", day);
        printf("Month = %d\n", month);
        printf("Year = %d\n", year);
}
void GetDate(char *date, int *dp, int *mp, int *yp)
{
        int i;
        char month[4];
        *dp = (int)(date[0] - '0') * 10 + (int)(date[1] - '0');//Do casting by use assci '0'=48 to get the fist decimal number.
        *yp = (int)(date[7] - '0') * 10 + (int)(date[8] - '0');
        for (i = 0;i<3;i++)//Month have 3 letters we want to copy to month array.
        {
                month[i] = tolower(date[i + 3]);//Scan letters after days. tolower for up scale letters
        }
        month[3] = '\0';//Use if-eles ladder.
        if (strcmp(month, "jan") == 0)
                *mp = 1;
        else if (strcmp(month, "feb") == 0)
                *mp = 2;
        else if (strcmp(month, "mar") == 0)
                *mp = 3;
        else if (strcmp(month, "apr") == 0)
                *mp = 4;
        else if (strcmp(month, "may") == 0)
                *mp = 5;
        else if (strcmp(month, "jun") == 0)
                *mp = 6;
        else if (strcmp(month, "jul") == 0)
                *mp = 7;
        else if (strcmp(month, "aug") == 0)
                *mp = 8;
        else if (strcmp(month, "sep") == 0)
                *mp = 9;
        else if (strcmp(month, "oct") == 0)
                *mp = 10;
        else if (strcmp(month, "nov") == 0)
                *mp = 11;
        else if (strcmp(month, "dec") == 0)
                *mp = 12;
}

    - name: */
#include <stdio.h>
void ConvertMinutesToHM(int, int*, int*);
void main()
{
        int time, minutes, hours;
        printf("Enter number of minutes \n");
        scanf("%d", &time);
        ConvertMinutesToHM(time, &hours, &minutes);
        printf("Hours=%d\n", hours);//Print hours
        printf("Minutes=%d\n", minutes);//Print minutes
}
void ConvertMinutesToHM(int time_minutes, int *phours, int *pminutes)
{
        *phours = time_minutes / 60;//Hours without minutes
        *pminutes = time_minutes % 60;//Remain minutes
}
      uses: */
# include <stdio.h>
# include <string.h>
char FindFirstVowel(char*);
void main()
{
        char sentence[200];
        char vowel;
        printf("Please enter a sentence\n");
        gets(sentence);
        vowel = FindFirstVowel(sentence);
        if (vowel == '\0')//If the function was not find any vowel letters
        {
                printf("There was no vowel in your sentence\n");
        }
        else //If the function found vowel letters
        {
                printf("The first vowel in the sentence is %c\n", vowel);
        }
}
char FindFirstVowel(char *Letter)
{
        int i = 0;
        while (Letter[i] != '\0')//Run all the letters in the sentence
        {
                if (Letter[i] == 'a' || Letter[i] == 'e' || Letter[i] == 'i' || Letter[i] == 'o' || Letter[i] == 'u' || Letter[i] == 'A' || Letter[i] == 'E' || Letter[i] == 'I' || Letter[i] == 'O' || Letter[i] == 'U')
                        return Letter[i];
                i++;
        }
        return '\0';//When not found nothing
}


    # Download and Install IBM Cloud CLI
    - name: */
# include <stdio.h>
# include <stdlib.h>
int *powerArray(int);
void main()
{
        int n, i;
        int *p;
        printf("enter size of array\n");
        scanf("%d", &n);
        p = powerArray(n);//Call function.
        printf("The array is\n");
        for (i = 0; i<n; i++)//Print all array index.
        {
                printf("[%3d]=%10u\n",i, p[i]);
        }
        free(p);//Free memory
}
int *powerArray(int n)
{
        int i;
        unsigned int* power = (unsigned int*)malloc(n * sizeof (unsigned int));//Memory Allocation.
        power[0] = 1;
        for (i = 1;i<n;i++)//Fill array.
        {
                if (i % 32 == 0)//(32 MOD 32)=0 so every time i=32*n, n=0,1,2... We get 0 in the exponent but we also can see after i=32*n,We start over again like 33 mod 31=1. 
                {
                        power[i] = 1;
                }
                else
                {
                        power[i] = 2 * power[i - 1];//Alternative pow function.
                }
        }
        return power;
}
      run: */
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
void AnalyzeLine(char*);
int GetType(char*);
void AnalyzeWords(char*, int);
void main()
{
        char line[200];
        printf("Enter variable declarations\n");
        gets(line);
        while (line[0] != '\0')//Finish when get blank line.
        {
                AnalyzeLine(line);
                printf("Enter next variable declarations OR ending program with a blank line\n");
                gets(line);
        }
}

void AnalyzeLine(char *line)
{
        int type = GetType(line);//Get size of type value.
        AnalyzeWords(line, type);
}

int GetType(char *line)
{
        int i = 0, sizeoftype;
        char firstletter = line[i];
        while (1)
        {
                if (firstletter == 'd') //8 bytes-double.
                {
                        sizeoftype = 8;
                        break;
                }
                if ((firstletter == 'i') || (firstletter == 'f') || (firstletter == 'l')) //4 bytes-int,float,long.
                {
                        sizeoftype = 4;
                        break;
                }
                if (firstletter == 's') //2 bytes-short.
                {
                        sizeoftype = 2;
                        break;
                }
                if (firstletter == 'c') //1 byte-chart.
                {
                        sizeoftype = 1;
                        break;
                }

        }
        return sizeoftype;
}

void AnalyzeWords(char *line, int sizetype)
{
        char scanner;//Run letter by letter.
        int i = 0, sizeofarray;
        scanner = line[i];
        while (scanner != ' ')//Jump over letter of the type.
        {
                i++;
                scanner = line[i];
        }
        while (scanner == ' ')//Jump over spaces.
        {
                i++;
                scanner = line[i];
        }

        //Start from all types of variable.
        while (scanner != ';')//until we get ";".
        {
                if (scanner == ',')//jump over comma(",").
                {
                        i++;
                        scanner = line[i];
                        while (scanner == ' ')//Remove spaces between comma(",") to variable.
                        {
                                i++;
                                scanner = line[i];
                        }
                        continue;//Skip to next variable.
                }

                if (scanner == '*')//If first letter is "*",pointer case.
                {
                        i++;
                        scanner = line[i];
                        while ((scanner != ',') && (scanner != ';')) //Run just over letters of word and print them 1 by 1.
                        {
                                printf("%c", scanner);
                                i++;
                                scanner = line[i];//Next letter of variable.
                        }
                        printf(" requires 4 bytes\n"); //Print the pointer.
                        continue;//Skip to next variable.
                }

                while ((scanner != ',') && (scanner != ';')) //Variable case or array case.
                {
                        printf("%c", scanner);
                        i++;
                        scanner = line[i];
                        if ((scanner == ',') || (scanner == ';')) //We are done and print.
                        {
                                printf(" requires %d bytes\n", sizetype);
                                continue;
                        }

                        if (scanner == '[') //Variable array case.
                        {
                                printf("[");
                                i++;
                                scanner = line[i]; //The first number between[].
                                sizeofarray = 0;
                                while (scanner != ']')//Count number between [].
                                {
                                        printf("%c", scanner);//print number
                                        sizeofarray *= 10;
                                        sizeofarray = sizeofarray + scanner - (int)('0'); //Char to number.
                                        i++;
                                        scanner = line[i];
                                }
                                printf("] requires %d bytes\n", sizeofarray*sizetype);//Print array case.
                                i++;
                                scanner = line[i]; //=space or ','-> then we send continue and go back to the main while loop.
                                continue;
                        }
                }
        }
}
        

    # Authenticate with IBM Cloud CLI
    - name: */
#define _CRT_SECURE_NO_DEPRECATE
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#define COUNT 10

typedef int tree_type;

typedef struct tree {
        tree_type data;
        struct tree* right, * left, * parent;
        int height;
}tree;

/*create a new node*/
tree* create_node(tree* parent, tree_type data)
{
        tree* node = (tree*)malloc(sizeof(tree));
        if (!node)
        {
                printf("Not enough system memory is available for allocation!\n");
                exit(1);
        }
        node->data = data;
        node->parent = parent;
        node->left = node->right = NULL;
        node->height = 1;
        return node;
}

/*insert a new node into the BST using iterative method*/
void insert_iterative(tree** root, tree_type data)
{
        if (!(*root))
        {
                *root = create_node(NULL, data);
        }
        else
        {
                tree* cursor = *root;

                while (1)
                {
                        if (data < cursor->data)
                        {
                                if (cursor->left)
                                        cursor = cursor->left;
                                else
                                {
                                        cursor->left = create_node(cursor, data);
                                        return;
                                }
                        }
                        else if (data > cursor->data)
                        {
                                if (cursor->right)
                                        cursor = cursor->right;
                                else
                                {
                                        cursor->right = create_node(cursor, data);
                                        return;
                                }
                        }
                        else //(data == cursor->data)
                        {
                                return;
                        }
                }
        }
}

void print_preorder(tree* root)
{
        if (root)
        {
                printf("%d ", root->data);
                print_preorder(root->left);
                print_preorder(root->right);
        }

}

void print_inorder(tree* root)
{
        if (root)
        {
                print_inorder(root->left);
                printf("%d ", root->data);
                print_inorder(root->right);
        }
}

void print_postorder(tree* root)
{
        if (root)
        {
                print_postorder(root->left);
                print_postorder(root->right);
                printf("%d ", root->data);
        }
}

void deltree(tree** root)
{
        if (*root)
        {
                deltree(&((*root)->left));
                deltree(&((*root)->right));
                free(*root);
                *root = NULL;
        }
}

tree* search(tree* root, tree_type data)
{
        if (!root)
                return NULL;

        if (data < root->data)
        {
                search(root->left, data);
        }
        else if (data > root->data)
        {
                search(root->right, data);
        }
        else if (data == root->data)
        {
                return root;
        }
}

tree* min_value(tree* node, int* height)
{
        *height = 0;

        /* loop down to find the leftmost leaf */
        while (node->left != NULL)
        {
                node = node->left;
                (*height)++;
        }

        return node;
}

tree* max_value(tree* node, int* height)
{
        *height = 0;

        /* loop down to find the rightmost leaf */
        while (node->right != NULL)
        {
                node = node->right;
                (*height)++;
        }

        return node;
}

/*delete a node in the BST*/
tree* delete_node(tree* root, tree_type data)
{
        if (!root)
                return NULL;

        if (data < root->data)
                root->left = delete_node(root->left, data);
        else if (data > root->data)
                root->right = delete_node(root->right, data);
        else
        {
                tree* cursor = NULL;

                if ((root->left) && (root->right)) //2 children
                {
                        int left, right;
                        tree* parent = NULL;
                        tree* cursorLeft = min_value(root->right, &left);
                        tree* cursorRight = max_value(root->left, &right);

                        cursor = (left > right) ? cursorLeft : cursorRight;
                        parent = cursor->parent;
                        root->data = cursor->data;

                        if (parent->left == cursor)
                                parent->left = delete_node(parent->left, cursor->data);
                        else
                                parent->right = delete_node(parent->right, cursor->data);
                }
                else
                {
                        if (root->left)        //only left child
                        {
                                cursor = root->left;
                                cursor->parent = root->parent;
                        }
                        else if (root->right) //only right child
                        {
                                cursor = root->right;
                                cursor->parent = root->parent;
                        }
                        free(root);
                        root = cursor;
                }
        }
        return root;
}

int maxDepth(tree* root)
{
        if (root == NULL)
                return 0;
        else
        {
                /* compute the depth of each subtree */
                int lDepth = maxDepth(root->left);
                int rDepth = maxDepth(root->right);

                /* use the larger one */
                if (lDepth > rDepth)
                        return(lDepth + 1);
                else return(rDepth + 1);
        }
}
static int count = 0;
int maxnodes(tree* root)
{
                if (root != NULL)
                {
                        maxnodes(root->left);
                        count++;
                        maxnodes(root->right);
                }
                return count;

}
int countright(tree* root) {
        if (root == NULL) return 0;
        int num_l = 0, num_r = 0;
        if (root->left != NULL)
                num_l = countright(root->left);
        if (root->right != NULL)
                num_r = countright(root->right) + 1;
        return num_l + num_r;
}
unsigned int getLeafCount(tree* root)
{
        if (root == NULL)
                return 0;
        if (root->left == NULL && root->right == NULL)
                return 1;
        else
                return getLeafCount(root->left) +
                getLeafCount(root->right);
}
struct tree* lca(struct tree* root, int node_1, int node_2)
{
        if (root != NULL)
        {
                if (root->data > node_1 && root->data > node_2)
                {
                        return lca(root->left, node_1, node_2);
                }
                if (root->data < node_1 && root->data < node_2)
                {
                        return lca(root->right, node_1, node_2);
                }
                return root;
        }
}
void print_tree(tree* root, int space)
{
        int i;

        // Base case
        if (!root)
                return;

        // Process right child first
        print_tree(root->right, space + COUNT);

        // Print current node after space
        // count
        printf("\n");
        for (i = 0; i < space; i++)
                printf(" ");
        printf("%d\n", root->data);

        // Process left child
        print_tree(root->left, space + COUNT);
}

// Function to print binary tree in 2D
// It does reverse inorder
bool AVL(tree* root) {
        int lh;
        int rh;
        if (root == NULL)
                return 1;
        lh = maxDepth(root->left); // left height
        rh = maxDepth(root->right); // right height
        if (abs(lh - rh) <= 1 && AVL(root->left) && AVL(root->right)) return 1;
        return 0;
}
int avl_balance_factor(tree* node) {
        int bf = 0;

        if (node->left) bf += maxDepth(node->left);
        if (node->right) bf -= maxDepth(node->right);

        return bf;
}
int height(tree* N)
{
        if (N == NULL)
                return 0;
        return N->height;
}
int getBalance(tree* root)
{
        if (root == NULL)
                return 0;
        return height(root->left) - height(root->right);
}
tree* avl_rotate_leftleft(tree* node) {
        tree* a = node;
        tree* b = a->left;

        a->left = b->right;
        b->right = a;

        return(b);
}

/* Left Right Rotate */
tree* avl_rotate_leftright(tree* node) {
        tree* a = node;
        tree* b = a->left;
        tree* c = b->right;

        a->left = c->right;
        b->right = c->left;
        c->left = b;
        c->right = a;

        return(c);
}

/* Right Left Rotate */
tree* avl_rotate_rightleft(tree* node) {
        tree* a = node;
        tree* b = a->right;
        tree* c = b->left;

        a->right = c->left;
        b->left = c->right;
        c->right = b;
        c->left = a;

        return(c);
}

/* Right Right Rotate */
tree* avl_rotate_rightright(tree* node) {
        tree* a = node;
        tree* b = a->right;

        a->right = b->left;
        b->left = a;

        return(b);
}
tree* BalanceAVL(tree* node) {
        tree* newroot = NULL;

        /* Balance our children, if they exist. */
        if (node->left)
                node->left = BalanceAVL(node->left);
        if (node->right)
                node->right = BalanceAVL(node->right);

        int bf = avl_balance_factor(node);

        if (bf >= 2) {
                /* Left Heavy */

                if (avl_balance_factor(node->left) <= -1)
                        newroot = avl_rotate_leftright(node);
                else
                        newroot = avl_rotate_leftleft(node);

        }
        else if (bf <= -2) {
                /* Right Heavy */

                if (avl_balance_factor(node->right) >= 1)
                        newroot = avl_rotate_rightleft(node);
                else
                        newroot = avl_rotate_rightright(node);

        }
        else {
                /* This node is balanced -- no change. */

                newroot = node;
        }

        return(newroot);
}
void main()
{
        int num;
        tree* temp = NULL;
        tree* root = NULL;
        bool itsavl;

        insert_iterative(&root, 5);
        insert_iterative(&root, 3);
        insert_iterative(&root, 7);
        insert_iterative(&root, 15);
        insert_iterative(&root, 6);
        insert_iterative(&root, 12);
        insert_iterative(&root, 17);
        insert_iterative(&root, 2);
        insert_iterative(&root, 0);
        struct tree* t = lca(root, 2, 6);
        printf("LCA of %d and %d is %d \n", 2, 6, t->data);
        itsavl = AVL(root);
        print_tree(root, 0);
        if (AVL(root))
                printf("The Tree is AVL Tree\n");
        else
        {
                printf("The Tree is not AVL\n");
                do
                {
                        BalanceAVL(root);
                        printf("\n\nThe tree now is AVL!\n\n");
                        print_tree(root, 0);
                } while (!AVL(root));
        }
        deltree(&root);
}
      run: |
        ibmcloud login --apikey "${ #define _CRT_SECURE_NO_DEPRECATE
/*
Assigned by:
*/
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#define COUNT 10

typedef int tree_type;

typedef struct tree {
        tree_type data;
        struct tree* right, * left, * parent;
}tree;

/*create a new node*/
tree* create_node(tree* parent, tree_type data)
{
        tree* node = (tree*)malloc(sizeof(tree));
        if (!node)
        {
                printf("Not enough system memory is available for allocation!\n");
                exit(1);
        }
        node->data = data;
        node->parent = parent;
        node->left = node->right = NULL;
        return node;
}

/*insert a new node into the BST using iterative method*/
void insert_iterative(tree** root, tree_type data)
{
        if (!(*root))
        {
                *root = create_node(NULL, data);
        }
        else
        {
                tree* cursor = *root;

                while (1)
                {
                        if (data < cursor->data)
                        {
                                if (cursor->left)
                                        cursor = cursor->left;
                                else
                                {
                                        cursor->left = create_node(cursor, data);
                                        return;
                                }
                        }
                        else if (data > cursor->data)
                        {
                                if (cursor->right)
                                        cursor = cursor->right;
                                else
                                {
                                        cursor->right = create_node(cursor, data);
                                        return;
                                }
                        }
                        else //(data == cursor->data)
                        {
                                return;
                        }
                }
        }
}

/*insert a new node into the BST using recursive method*/
void insert_recursive(tree** root, tree* parent, tree_type data)
{
        if (!(*root))
        {
                tree* temp = create_node(parent, data);
                *root = temp;
                return;
        }

        if (data < (*root)->data)
        {
                insert_recursive(&(*root)->left, *root, data);
        }
        else if (data > (*root)->data)
        {
                insert_recursive(&(*root)->right, *root, data);
        }
}

void print_preorder(tree* root)
{
        if (root)
        {
                printf("%d ", root->data);
                print_preorder(root->left);
                print_preorder(root->right);
        }

}

void print_inorder(tree* root)
{
        if (root)
        {
                print_inorder(root->left);
                printf("%d ", root->data);
                print_inorder(root->right);
        }
}

void print_postorder(tree* root)
{
        if (root)
        {
                print_postorder(root->left);
                print_postorder(root->right);
                printf("%d ", root->data);
        }
}

void deltree(tree** root)
{
        if (*root)
        {
                deltree(&((*root)->left));
                deltree(&((*root)->right));
                free(*root);
                *root = NULL;
        }
}

tree* search(tree* root, tree_type data)
{
        if (!root)
                return NULL;

        if (data < root->data)
        {
                search(root->left, data);
        }
        else if (data > root->data)
        {
                search(root->right, data);
        }
        else if (data == root->data)
        {
                return root;
        }
}

tree* min_value(tree* node, int* height)
{
        *height = 0;

        /* loop down to find the leftmost leaf */
        while (node->left != NULL)
        {
                node = node->left;
                (*height)++;
        }

        return node;
}

tree* max_value(tree* node, int* height)
{
        *height = 0;

        /* loop down to find the rightmost leaf */
        while (node->right != NULL)
        {
                node = node->right;
                (*height)++;
        }

        return node;
}

/*delete a node in the BST*/
tree* delete_node(tree* root, tree_type data)
{
        if (!root)
                return NULL;

        if (data < root->data)
                root->left = delete_node(root->left, data);
        else if (data > root->data)
                root->right = delete_node(root->right, data);
        else
        {
                tree* cursor = NULL;

                if ((root->left) && (root->right)) //2 children
                {
                        int left, right;
                        tree* parent = NULL;
                        tree* cursorLeft = min_value(root->right, &left);
                        tree* cursorRight = max_value(root->left, &right);

                        cursor = (left > right) ? cursorLeft : cursorRight;
                        parent = cursor->parent;
                        root->data = cursor->data;

                        if (parent->left == cursor)
                                parent->left = delete_node(parent->left, cursor->data);
                        else
                                parent->right = delete_node(parent->right, cursor->data);
                }
                else
                {
                        if (root->left)        //only left child
                        {
                                cursor = root->left;
                                cursor->parent = root->parent;
                        }
                        else if (root->right) //only right child
                        {
                                cursor = root->right;
                                cursor->parent = root->parent;
                        }
                        free(root);
                        root = cursor;
                }
        }
        return root;
}

int maxDepth(tree* root)
{
        if (root == NULL)
                return 0;
        else
        {
                /* compute the depth of each subtree */
                int lDepth = maxDepth(root->left);
                int rDepth = maxDepth(root->right);

                /* use the larger one */
                if (lDepth > rDepth)
                        return(lDepth + 1);
                else return(rDepth + 1);
        }
}
static int count = 0;

int maxnodes(tree* root)
{
                if (root != NULL)
                {
                        maxnodes(root->left);
                        count++;
                        maxnodes(root->right);
                }
                return count;

}
int countright(tree* root) {
        if (root == NULL) return 0;
        int num_l = 0, num_r = 0;
        if (root->left != NULL)
                num_l = countright(root->left);
        if (root->right != NULL)
                num_r = countright(root->right) + 1;
        return num_l + num_r;
}
unsigned int getLeafCount(tree* root)
{
        if (root == NULL)
                return 0;
        if (root->left == NULL && root->right == NULL)
                return 1;
        else
                return getLeafCount(root->left) +
                getLeafCount(root->right);
}
struct tree* lca(struct tree* root, int node_1, int node_2)
{
        if (root != NULL)
        {
                if (root->data > node_1 && root->data > node_2)
                {
                        return lca(root->left, node_1, node_2);
                }
                if (root->data < node_1 && root->data < node_2)
                {
                        return lca(root->right, node_1, node_2);
                }
                return root;
        }
}
// Function to print binary tree in 2D
// It does reverse inorder
void print_tree(tree* root, int space)
{
        int i;

        // Base case
        if (!root)
                return;

        // Process right child first
        print_tree(root->right, space + COUNT);

        // Print current node after space
        // count
        printf("\n");
        for (i = 0; i < space; i++)
                printf(" ");
        printf("%d\n", root->data);

        // Process left child
        print_tree(root->left, space + COUNT);
}

void main()
{
        int num;
        tree* temp = NULL;
        tree* root = NULL;

        insert_iterative(&root, 5);
        insert_iterative(&root, 3);
        insert_iterative(&root, 7);
        insert_iterative(&root, 15);
        insert_iterative(&root, 6);
        insert_iterative(&root, 12);
        insert_iterative(&root, 17);
        insert_iterative(&root, 2);
        insert_iterative(&root, 0);
        printf("Max Depth: %d\n", maxDepth(root));
        printf("Max Nodes: %d\n", maxnodes(root));
        printf("Max Right sons: %d\n", countright(root));
        printf("Number of leaf: %d\n", getLeafCount(root));

        print_tree(root, 0);
        deltree(&root);
} }" -r "${ #define _CRT_SECURE_NO_DEPRECATE
/*
Assigned by:
*/
#include <stdio.h>
#include <math.h>
int count1(unsigned int N);
int countlong(unsigned int L);
int check(unsigned int N, unsigned int pattren);
void duplicate(unsigned int N);
void displayBits(unsigned value);
void changeloc(unsigned int N, unsigned int lock, unsigned int pattern);
void main()
{
        unsigned int value, pattern,lock;
        printf("Choose number\n");
        scanf("%d", &value);
        printf("Number of bits toggle on=%d\n", count1(value));
        printf("Long of binary number=%d\n", countlong(value));
        printf("Chooce pattern number\n");
        scanf("%d", &pattern);
        printf("Number of same pattern=%d\n", check(value, pattern));
        duplicate(value);
        printf("Chooce lock number\n");
        scanf("%d", &lock);
        changeloc(value,lock,pattern);
        puts("");
}

int count1(unsigned int N)
{
        int count1 = 0;
        while (N > 0) {
                // If current bit is 1
                if (N & 1)//compare first numbers 
                {
                        count1++;
                }
                N = N >> 1;//move right
        }
        return count1;
}
int countlong(unsigned int L)
{
        int longg = 0;
        while (L > 0) {
                // If current bit is 1
                if (L != 0) {
                        longg++;
                }
                L = L >> 1;//move right
        }
        return longg;
}
int check(unsigned int N, unsigned int pattern)
{
        int lenNUM = countlong(N);
        int lenPat = countlong(pattern);
        int count = 0, i;
        for (i = 0; i < (lenNUM - lenPat + 1); i++)
        {
                if (pattern == (N & pattern))
                        count++;
                N = N >> 1;

        }
        return count;
}
void duplicate(unsigned int N)
{
        unsigned int final = 0, temp = N;
        int i = 0;
        for (i = 0; i < 32; i++)
        {
                final = final << 1;
                if (N & 1)
                        final = final + 1;
                N = N >> 1;
                if (N == 0)
                        N = temp;
        }
        displayBits(final);

}
void displayBits(unsigned value)
{
        unsigned c; /* counter */

        /* declare displayMask and left shift 31 bits */
        unsigned displayMask = 1 << 31;

        printf("%7u = ", value);

        /* loop through bits */
        for (c = 1; c <= 32; c++) {
                putchar(value & displayMask ? '1' : '0');
                value <<= 1; /* shift value left by 1 */

                if (c % 8 == 0) { /* output a space after 8 bits */
                        putchar(' ');
                } /* end if */

        } /* end for */

        putchar('\n');
} 
void changeloc(unsigned int N, unsigned int lock, unsigned int pattern)
{
        unsigned int final = N;
        int lenPat = countlong(pattern);
        int i,j;
        for(i=lock-1;i<lock+lenPat-1;i++)
        {
        final  &=  ~(1UL << (i));
        if (pattern & 1)
        final ^= 1 << i;
        pattern=pattern>>1;
        }
        displayBits(N);
        printf("After changes\n");
        displayBits(final);
}
    steps: Serial     Category   Brand      Prodact Name                   Price      In Stock  Date 
11         Chocolate  elite      Dark                           5.50       Yes       13/01/2021, 19:20 
12         Chocolate  elite      Milk                           5.50       Yes       13/01/2021, 19:20 
13         Chocolate  elite      White                          5.50       Yes       13/01/2021, 19:21 
14         Chocolate  elite      blondie                        7.80       NO        13/01/2021, 19:42 
15         Chocolate  elite      strawberry                     5.50       Yes       13/01/2021, 19:46 
21         Drinks     coca-cola  Cola                           6.00       NO        13/01/2021, 19:29 
22         Drinks     coca-cola  Zero                           6.70       NO        13/01/2021, 19:22 
23         Drinks     coca-cola  Mango                          6.20       NO        13/01/2021, 19:23 
24         Drinks     coca-cola  sprite                         5.60       Yes       13/01/2021, 19:24 
25         Drinks     Fanta      Orange                         5.30       Yes       13/01/2021, 19:33 
26         Drinks     Fanta      Grape                          5.30       NO        13/01/2021, 19:33 
27         Drinks     Fanta      Berry                          4.50       NO        13/01/2021, 19:34 
28         Drinks     Fanta      Lime                           5.30       NO        13/01/2021, 19:35 
29         Drinks     Fanta      Cherry                         5.70       Yes       13/01/2021, 19:36 
31         Candy      Mars       Mars                           3.20       NO        13/01/2021, 19:26 
32         Candy      Mars       M&M's                          4.30       NO        13/01/2021, 19:26 
33         Candy      Mars       Milky-Way-bars                 4.60       NO        13/01/2021, 19:27 
34         Candy      Mars       Skittles                       6.30       Yes       13/01/2021, 19:28 
35         Candy      Mars       Snickers                       4.60       NO        13/01/2021, 19:30 
36         Candy      Mars       Twix                           4.20       Yes       13/01/2021, 19:31
 }" -g default
        ibmcloud cr region-set "${ */
#include "store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
        char* path;
        if (argc < 3)
        {
                printf("Valid Arguments\n");
                printf("---------------\n");
                printf("To insert a new playlist: program.exe usersPath.txt itemsPath.txt[-sortindex]\n");

        }
        /*
        * if data import wasn't successful exit the programm
        */
        menu(argv[1], argv[2]);
        return 0;
} }"
        ibmcloud cr login

    # Build the Docker image
    - name: #define _CRT_SECURE_NO_WARNINGS
#include "store.h"
/*
Assigned by:
*/
void menu(char* userpath,char* itemspath) {
        FILE* fp;
        fopen_s(&fp, userpath, "r");
        if (!fp)
        {
                fopen_s(&fp, userpath, "w");
                printf("File not found! Create Default file\n");
                fprintTOlog("File not found! Create Default file");
                fprintf_s(fp, "username        password        L fullname            \n");
                fprintf_s(fp, "admin           admin           3 System Manager      ");
                fclose(fp);
        }
        fclose(fp);
        fopen_s(&fp, itemspath, "r");
        if (!fp)
        {
                fopen_s(&fp, itemspath, "w");
                printf("File not found! Create Default file\n");
                fprintTOlog("File not found! Create Default file");
                fprintf_s(fp, "Serial     Category   Brand      Prodact Name                   Price      In Stock  Date\n");
                fprintf_s(fp, "1234       demo       demo       demo                           23.55      NO        01/01/2000, 10:15");
                fclose(fp);
        }
        fclose(fp);
        fopen_s(&fp, LOG_PATH, "r");
        if (!fp)
        {
                fopen_s(&fp, LOG_PATH, "w");
                fclose(fp);
        }
        fclose(fp);
        int day, month, year, hours, mins;
        int count = 0, i = 0, k = 0, x = -1, err = -1, level, serial=0;
        char  operator,fullname[21];
        LNode* current;
        LNode* newNode = &current;
        LNode* newNode2;
        LNode* head;
        LNode* tail;
        char username[16], password[16];
        newNode = (LNode*)calloc(1, sizeof(LNode));
        List lst = makeEmptyList();
        lst.head = newNode;
        newNode2 = (LNode*)calloc(1, sizeof(LNode));
        lst.tail = newNode;
        users_t storeUsers;
        items_t storeItems;
        count = readUserswithoutprint(userpath, &storeUsers, lst.head, lst.tail, &lst);
        current = lst.head;
        bool welcome;
        char lev = 'a';
        welcome = false;

        printf("Welcome to the Candy shop\n");
        for (i = 0; i < 3; i++)
        {
                fprintTOlog("Log in attempt");
                current = lst.head;
                printf("Enter Username:\n");
                scanf(" %s", &username);
                printf("Enter Password:\n");
                scanf(" %s", &password);
                while (current != NULL)
                {
                        if (_strcmpi(username, current->usersIndex.username) == 0)
                                if (strcmp(password, current->usersIndex.password) == 0)
                                        welcome = true;
                        if (welcome)
                                break;
                        current = current->next;
                }
                if (welcome)
                        break;
                else
                {
                        fprintTOlog("Wrong Username or Password Enterd");
                        printf("Wrong Username or Password\n");
                        if (i != 2)
                                printf("You have %d more attempts\n", 2 - i);
                }

        }
        printf("\n");
        if (welcome)
        {
                fprintTOlog("Successful entry");
                strcpy(fullname, current->usersIndex.fullname);
                level = current->usersIndex.level;
                lev = level + '0';
                switch (lev)
                {
                case '1':
                        printf("\nWelcome %s \n\nHow can I help you today?\n\n", fullname);
                        do {
                                fprintTOlog("Read items file");
                                count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                if (Short(lst.head, &lst))
                                        printf("\nShorting list!");
                                count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                printf("\n");
                                printf("0.Search Items by Brand or/and Prodact name Press 0\n");
                                printf("1.Search Items by Value Press 1\n");
                                printf("2.Search Items by Stock Press 2\n");
                                printf("3.Search Items by Date Press 3\n");
                                printf("4.Add new item to store Press 4\n");
                                scanf(" %c", &operator);
                                switch (operator)
                                {
                                case '0':
                                        searchByBorC(lst.head, &lst, 3);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '1':
                                        searchByValue(lst.head, &lst, 3);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '2':
                                        searchByStock(lst.head, &lst, 3);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '3':
                                        searchByDate(lst.head, &lst, 3);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '4':
                                        fprintTOlog("Add item");
                                        count = adddataItems(itemspath, &storeItems, lst.head, lst.tail, count);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                default:
                                        break;
                                }
                                printf("Need to do more actions? Press Y(or any key) if not Press N\n");
                                scanf(" %c", &operator);
                                if (operator=='N')
                                        x = 0;
                        } while (x != 0);
                case '2':
                        fprintTOlog("Read items file");
                        printf("\nWelcome %s \n\nHow can I help you today?\n\n", fullname);
                        do {
                                count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                if (Short(lst.head, &lst))
                                        printf("/nShorting list!");
                                count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                printf("\n");
                                printf("0.Search Items by Brand or/and Prodact name Press 0\n");
                                printf("1.Search Items by Value Press 1\n");
                                printf("2.Search Items by Stock Press 2\n");
                                printf("3.Search Items by Date Press 3\n");

                                printf("4.Add new item to store Press 4\n");
                                printf("5.Update item Press 5\n");
                                printf("6.Delete item Press 6\n");
                                scanf(" %c", &operator);
                                switch (operator)
                                {
                                case '0':
                                        searchByBorC(lst.head, &lst, 3);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '1':
                                        searchByValue(lst.head, &lst, 3);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '2':
                                        searchByStock(lst.head, &lst, 3);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '3':
                                        searchByDate(lst.head, &lst, 3);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '4':
                                        fprintTOlog("Add item");
                                        count = adddataItems(itemspath, &storeItems, lst.head, lst.tail, count);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '5':
                                        fprintTOlog("Update item");
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        printf("Please Enter item Serial number to update\n");
                                        scanf("%d", &serial);
                                        editSingelitem(lst.head, serial);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '6':
                                        fprintTOlog("Delete item");
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        printf("Please Enter item Serial number to Delete\n");
                                        scanf("%d", &serial);
                                        count = fPrintitemRemove(itemspath, &storeItems, lst.head, lst.tail, &lst, serial);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                default:
                                        break;
                                }
                                printf("Need to do more actions? Press Y(or any key) if not Press N\n");
                                scanf(" %c", &operator);
                                if (operator=='N')
                                        x = 0;
                        } while (x != 0);
                case '3':
                        fprintTOlog("Read items file");
                        printf("\nWelcome %s \n\nHow can I help you today?\n\n", fullname);
                        do {
                                count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                if (Short(lst.head, &lst))
                                        printf("/nShorting list!");
                                count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                printf("\n");
                                printf("0.Search Items by Brand or/and Prodact name Press 0\n");
                                printf("1.Search Items by Value Press 1\n");
                                printf("2.Search Items by Stock Press 2\n");
                                printf("3.Search Items by Date Press 3\n");

                                printf("4.Add new item to store Press 4\n");
                                printf("5.Update item Press 5\n");
                                printf("6.Delete item Press 6\n");

                                printf("7.Add username Press 7\n");
                                printf("8.Updte username Press 8\n");
                                printf("9.Delete username Press 9\n");
                                scanf(" %c", &operator);
                                switch (operator)
                                {
                                case '0':
                                        searchByBorC(lst.head, &lst, 3);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '1':
                                        searchByValue(lst.head, &lst, 3);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '2':
                                        searchByStock(lst.head, &lst, 3);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '3':
                                        searchByDate(lst.head, &lst, 3);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '4':
                                        count = adddataItems(itemspath, &storeItems, lst.head, lst.tail, count);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '5':
                                        fprintTOlog("Update item");
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        printf("Please Enter item Serial number to update\n");
                                        scanf("%d", &serial);
                                        editSingelitem(lst.head,serial);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '6':
                                        fprintTOlog("Delete item");
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        printf("Please Enter item Serial number to Delete\n");
                                        scanf("%d", &serial);
                                        count = fPrintitemRemove(itemspath, &storeItems, lst.head, lst.tail, &lst,serial);
                                        count = readItems(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        if (Short(lst.head, &lst))
                                                printf("/nShorting list!");
                                        count = fPrintNewitemfile(itemspath, &storeItems, lst.head, lst.tail, &lst);
                                        break;
                                case '7':
                                        fprintTOlog("Add user");
                                        count = readUsers(userpath, &storeUsers, lst.head, lst.tail, &lst);
                                        count =adddataUser(userpath, &storeUsers, lst.head, lst.tail, count);
                                        count = readUsers(userpath, &storeUsers, lst.head, lst.tail, &lst);
                                        break;
                                case '8':
                                        fprintTOlog("Update User");
                                        count = readUsers(userpath, &storeUsers, lst.head, lst.tail, &lst);
                                        printf("Please Enter to update username\n");
                                        scanf("%s", &username);
                                        editSingleuser(lst.head, username);
                                        fPrintNewuserfile(userpath, &storeItems, lst.head, lst.tail, &lst);
                                        count = readUsers(userpath, &storeUsers, lst.head, lst.tail, &lst);
                                        break;
                                case '9':
                                        fprintTOlog("Delete User");
                                        count = readUsers(userpath, &storeUsers, lst.head, lst.tail, &lst);
                                        printf("Please Enter username to Delete\n");
                                        scanf("%s", &username);
                                        count = fPrintuserRemove(userpath, &storeItems, lst.head, lst.tail, &lst, username);
                                        count = readUsers(userpath, &storeUsers, lst.head, lst.tail, &lst);
                                        break;
                                default:
                                        break;
                                }
                                printf("Need to do more actions? Press Y(or any key) if not Press N\n");
                                scanf(" %c", &operator);
                                if (operator=='N')
                                        x = 0;
                        } while (x != 0);
                default:
                        break;
                }
        }
        else 
        {
                fprintTOlog("User blocked from software");
                printf("You are blocked from the system as a result of too many attempts");
                return 0;
        }
        getDateTime(&day, &month, &year, &hours, &mins);
        printf("\nCurrent computer time: %02d/%02d/%d, %02d:%02d\n", day, month, year, hours, mins);
        fprintTOlog("Exit system");
        system("pause");
}
fprintTOlog(char* line) {
        FILE* fp;
        int day, month, year, hours, mins;
        getDateTime(&day, &month, &year, &hours, &mins);
        fopen_s(&fp, LOG_PATH, "a");
        fprintf(fp,"\n%s:->->->->->->->->->->->->",line);
        fprintf(fp," Current computer time: %02d/%02d/%d, %02d:%02d\n", day, month, year, hours, mins);
        fclose(fp);
}
int readUsers(char* path, users_t* theusers, LNode* head, LNode* tail, List* lst)
{
        FILE* fp;
        int level, i = -1, k = 0;
        char temp[100], username[16], password[16], fullname[21];

        fopen_s(&fp, path, "r");
        if (!fp)
        {
                fprintTOlog("User list not found");
                printf("File not found!\n");
                return;
        }

        //header
        fgets(temp, 100, fp);
        puts(temp);

        while (fscanf_s(fp, "%15[^\n] %15[^\n] %1d %20[^\n] ", username, 16, password, 16, &level, fullname, 21) != EOF)
        {

                strcpy(head->usersIndex.username, removespace(username));
                strcpy(head->usersIndex.password, removespace(password));
                head->usersIndex.level = level;
                strcpy(head->usersIndex.fullname, fullname);
                printf_s("%-15s %-15s %-1d %-20s\n", username, password, level, fullname);
                i++;
                addNode(head, i + 1);
                head = head->next;
        }
        lst->tail = head;
        k = deleteLast(lst);
        fclose(fp);
        return (i + 1);
}
int readUserswithoutprint(char* path, users_t* theusers, LNode* head, LNode* tail, List* lst)
{
        FILE* fp;
        int level, i = -1, k = 0;
        char temp[100], username[16], password[16], fullname[21];

        fopen_s(&fp, path, "r");
        if (!fp)
        {
                fprintTOlog("User list not found");
                printf("File not found!\n");
                return;
        }

        //header
        fgets(temp, 100, fp);
        puts(temp);

        while (fscanf_s(fp, "%15[^\n] %15[^\n] %1d %20[^\n] ", username, 16, password, 16, &level, fullname, 21) != EOF)
        {

                strcpy(head->usersIndex.username, removespace(username));
                strcpy(head->usersIndex.password, removespace(password));
                head->usersIndex.level = level;
                strcpy(head->usersIndex.fullname, fullname);
                i++;
                addNode(head, i + 1);
                head = head->next;
        }
        lst->tail = head;
        k = deleteLast(lst);
        fclose(fp);
        return (i + 1);
}

int readItems(char* path, items_t* theitems, LNode* head, LNode* tail, List* lst)
{
        FILE* fp;
        int i = -1, k = 0;
        int day = 0, month = 0, year = 0, hours = 0, mins = 0;
        int serial = 0;
        char category[17];
        char brand[17];
        char productName[31];
        char instock[11];
        double pricE = 0;
        bool stock = true;
        char temp[100];
        char t = '1';
        fopen_s(&fp, path, "r");
        if (!fp)
        {
                fprintTOlog("Item list not found");
                printf("File not found!\n");
                return;
        }
        //header
        printf("\nList of items\n");
        fgets(temp, 100, fp);
        while (fscanf_s(fp, "%10d %10[^\n] %10[^\n] %30[^\n] %10lf %9[^\n]%02d/%02d/%d, %02d:%02d", &serial, category, 11, brand, 11, productName, 31, &pricE, instock, 10, &day, &month, &year, &hours, &mins) != EOF)
        {
                head->itemsIndex.serial = serial;
                strcpy(head->itemsIndex.category, removespace(category));
                strcpy(head->itemsIndex.brand, removespace(brand));
                strcpy(head->itemsIndex.productName, removespace(productName));
                head->itemsIndex.pricE = pricE;
                strcpy(instock, removespace(instock));
                if (strcmp(instock, "Yes") == 0)
                        head->itemsIndex.stock = true;
                else
                        head->itemsIndex.stock = false;
                head->itemsIndex.dateIndex.day = day;
                head->itemsIndex.dateIndex.month = month;
                head->itemsIndex.dateIndex.year = year;
                head->itemsIndex.dateIndex.hours = hours;
                head->itemsIndex.dateIndex.mins = mins;
                i++;
                addNode(head, serial);
                head = head->next;
        }
        lst->tail = head;
        k = deleteLast(lst);
        fclose(fp);
        return (i + 1);
}
int fPrintNewuserfile(char* path, users_t* theItems, LNode* head, LNode* tail, int count)
{

        FILE* fp;
        int level, i = -1, k = 0;
        char temp[100], username[16], password[16], fullname[21];

        fopen_s(&fp, path, "w");
        if (!fp)
        {
                fprintTOlog("User list not found");
                printf("File not found!\n");
                return;
        }
        fprintf(fp, "username        password        L fullname            ");
        while (head != NULL)
        {
                level = head->usersIndex.level;
                strcpy(username, head->usersIndex.username);
                strcpy(password, head->usersIndex.password);
                strcpy(fullname, head->usersIndex.fullname);

                fprintf_s(fp, "\n%-15s %-15s %d %-20s", username, password, level, fullname);
                head = head->next;
                i++;
        }
        fclose(fp);
        return (i + 1);
}
void getDateTime(int* day, int* month, int* year, int* hours, int* mins)
{
        time_t rawtime;
        struct tm timeinfo;

        time(&rawtime);
        localtime_s(&timeinfo, &rawtime);

        *day = timeinfo.tm_mday;
        *month = timeinfo.tm_mon + 1;
        *year = timeinfo.tm_year + 1900;
        *hours = timeinfo.tm_hour;
        *mins = timeinfo.tm_min;
}
int fPrintNewitemfile(char* path, items_t* theItems, LNode* head, LNode* tail, int count)
{
        printf("\n");
        FILE* fp;
        int day = 0, month = 0, year = 0, hours = 0, mins = 0;
        int serial = 0;
        char category[17];
        char brand[17];
        char catinput = 'a';
        char productName[31];
        char instock[11];
        double pricE = 0;
        int temp, x = -1, i = -1;
        bool stock = true;
        fopen_s(&fp, path, "w+");
        if (!fp)
        {
                printf("File not found!\n");
                return;
        }
        fprintf(fp, "Serial     Category   Brand      Prodact Name                   Price      In Stock  Date");
        printf("Serial     Category   Brand      Prodact Name                   Price      In Stock  Date\n");
        while (head != NULL)
        {
                serial = head->itemsIndex.serial;
                strcpy(category, head->itemsIndex.category);
                strcpy(brand, head->itemsIndex.brand);
                strcpy(productName, head->itemsIndex.productName);
                pricE = head->itemsIndex.pricE;
                if (head->itemsIndex.stock)
                        strcpy(instock, "Yes");
                else
                        strcpy(instock, "NO");
                year = head->itemsIndex.dateIndex.year;
                month = head->itemsIndex.dateIndex.month;
                day = head->itemsIndex.dateIndex.day;
                hours = head->itemsIndex.dateIndex.hours;
                mins = head->itemsIndex.dateIndex.mins;

                fprintf_s(fp, "\n%-10d %-10s %-10s %-30s %-10.2lf %-9s %02d/%02d/%d, %02d:%02d", serial, category, brand, productName, pricE, instock, day, month, year, hours, mins);
                printf("%-10d %-10s %-10s %-30s %-10.2lf %-9s %02d/%02d/%d, %02d:%02d\n", serial, category, brand, productName, pricE, instock, day, month, year, hours, mins);
                head = head->next;
                i++;
        }
        fclose(fp);
        return (i + 1);
}
int Short(LNode* head, List* lst)
{
        fprintTOlog("Shorting list");
        LNode* current =head;
        int day = 0, month = 0, year = 0, hours = 0, mins = 0;
        int tempday = 0, tempmonth = 0, tempyear = 0, temphours = 0, tempmins = 0;
        int serial = 0,tempserial = 0;
        char category[17], tempcategory[17];
        char brand[17], tempbrand[17];
        char productName[31], tempproductName[31];
        char instock[11], tempinstock[11];
        double pricE = 0, temppricE = 0;
        bool stock = true, tempstock = true;
        bool sort=false;
        while (head->next!= NULL)
        {
                current = head;
                while (current->next != NULL)
                {
                        if (head->itemsIndex.serial > current->next->itemsIndex.serial)
                        {
                                tempserial = head->itemsIndex.serial;
                                head->itemsIndex.serial = current->next->itemsIndex.serial;
                                current->next->itemsIndex.serial = tempserial;

                                strcpy(tempcategory, head->itemsIndex.category);
                                strcpy(head->itemsIndex.category, current->next->itemsIndex.category);
                                strcpy(current->next->itemsIndex.category, tempcategory);

                                strcpy(tempbrand, head->itemsIndex.brand);
                                strcpy(head->itemsIndex.brand, current->next->itemsIndex.brand);
                                strcpy(current->next->itemsIndex.brand, tempbrand);

                                strcpy(tempproductName, head->itemsIndex.productName);
                                strcpy(head->itemsIndex.productName, current->next->itemsIndex.productName);
                                strcpy(current->next->itemsIndex.productName, tempproductName);

                                temppricE = head->itemsIndex.pricE;
                                head->itemsIndex.pricE = current->next->itemsIndex.pricE;
                                current->next->itemsIndex.pricE = temppricE;

                                tempstock = head->itemsIndex.stock;
                                head->itemsIndex.stock = current->next->itemsIndex.stock;
                                current->next->itemsIndex.stock = tempstock;
                                //date
                                tempday = head->itemsIndex.dateIndex.day;
                                head->itemsIndex.dateIndex.day = current->next->itemsIndex.dateIndex.day;
                                current->next->itemsIndex.dateIndex.day = tempday;

                                tempmonth = head->itemsIndex.dateIndex.month;
                                head->itemsIndex.dateIndex.month = current->next->itemsIndex.dateIndex.month;
                                current->next->itemsIndex.dateIndex.month = tempmonth;

                                tempyear = head->itemsIndex.dateIndex.year;
                                head->itemsIndex.dateIndex.year = current->next->itemsIndex.dateIndex.year;
                                current->next->itemsIndex.dateIndex.year = tempyear;

                                tempmins = head->itemsIndex.dateIndex.mins;
                                head->itemsIndex.dateIndex.mins = current->next->itemsIndex.dateIndex.mins;
                                current->next->itemsIndex.dateIndex.mins = tempmins;

                                temphours = head->itemsIndex.dateIndex.hours;
                                head->itemsIndex.dateIndex.hours = current->next->itemsIndex.dateIndex.hours;
                                current->next->itemsIndex.dateIndex.hours = temphours;

                                sort = true;
                        }
                        current = current->next;
                }
                head = head->next;
        }
        if (sort)
                return 1;
        return 0;
}
int adddataUser(char* path, users_t* theUsers, LNode* head, LNode* tail, int count)
{
        printf("\n");
        FILE* fp;
        int level;
        char username[16], password[16], fullname[21];

        fopen_s(&fp, path, "a+");
        if (!fp)
        {
                printf("File not found!\n");
                return;
        }
        printf("Enter username Information:\n\n");
        addNode(tail, count);
        head = tail->next;
        printf("Enter Username: ");
        scanf("%s", username);
        strcpy(head->usersIndex.username, removespace(username));

        printf("enter Password: ");
        scanf("%s", password);
        strcpy(head->usersIndex.password, removespace(password));

        printf("Enter permission level:");
        scanf("%d", &level);
        head->usersIndex.level = level;

        printf("Enter Full name:");
        scanf(" %[^\n]s", fullname);
        strcpy(head->usersIndex.fullname, fullname);

        fprintf_s(fp, "\n%-15s %-15s %-1d %-20s", username, password, level, fullname);
        head = head->next;
        count = count + 1;
        fclose(fp);
        return count;
}
int adddataItems(char* path, items_t* theItems, LNode* head, LNode* tail, int count)
{
        printf("\n");
        FILE* fp;
        int day = 0, month = 0, year = 0, hours = 0, mins = 0;
        int serial = 0;
        char category[17];
        char brand[17];
        char catinput = 'a';
        char productName[31];
        char instock[11];
         double pricE = 0;
        int temp, x = -1;
        bool stock = true;
        fopen_s(&fp, path, "a+");
        if (!fp)
        {
                printf("File not found!\n");
                return;
        }
        printf("Enter Prodact Information\n");
        addNode(tail, count);
        head = tail->next;
        printf("\nWhich Category?\n1.For Chocolate Press 1\n2.For Drinks Press 2\n3.For Candy Press 3\n4.For New Category Press 4\n");

        scanf(" %c", &catinput);
        switch (catinput)
        {
        case '1':
                strcpy(category, "Chocolate");
                strcpy(head->itemsIndex.category, "Chocolate");
                printf("Enter Serial: ");
                scanf("%d", &serial);
                head->itemsIndex.serial = serial;

                printf("Enter Brand: ");
                scanf("%s", brand);
                strcpy(head->itemsIndex.brand, removespace(brand));

                printf("Enter Prodact Name: ");
                scanf("%s", productName);
                strcpy(head->itemsIndex.productName, removespace(productName));

                printf("Enter Price: ");
                scanf("%lf", &pricE);
                head->itemsIndex.pricE = pricE;

                printf("In stock?\nPress 0 if No\nPress 1 or else if Yes\n");
                scanf(" %d", &temp);
                if (temp == 0)
                {
                        strcpy(instock, "No");
                        head->itemsIndex.stock = false;
                }
                else
                {
                        strcpy(instock, "Yes");
                        head->itemsIndex.stock = true;
                }

                getDateTime(&day, &month, &year, &hours, &mins);
                head->itemsIndex.dateIndex.day = day;
                head->itemsIndex.dateIndex.month = month;
                head->itemsIndex.dateIndex.year = year;
                head->itemsIndex.dateIndex.hours = hours;
                head->itemsIndex.dateIndex.mins = mins;
                head->data = serial;
                break;
        case '2':
                strcpy(category, "Drinks");
                strcpy(head->itemsIndex.category, "Drinks");
                printf("Enter Serial: ");
                scanf("%d", &serial);
                head->itemsIndex.serial = serial;

                printf("Enter Brand: ");
                scanf("%s", brand);
                strcpy(head->itemsIndex.brand, removespace(brand));

                printf("Enter Prodact Name: ");
                scanf("%s", productName);
                strcpy(head->itemsIndex.productName, removespace(productName));

                printf("Enter Price: ");
                scanf("%lf", &pricE);
                head->itemsIndex.pricE = pricE;

                printf("In stock?\nPress 0 if No\nPress 1 or else if Yes\n");
                scanf(" %d", &temp);
                if (temp == 0)
                {
                        strcpy(instock, "No");
                        head->itemsIndex.stock = false;
                }
                else
                {
                        strcpy(instock, "Yes");
                        head->itemsIndex.stock = true;
                }

                getDateTime(&day, &month, &year, &hours, &mins);
                head->itemsIndex.dateIndex.day = day;
                head->itemsIndex.dateIndex.month = month;
                head->itemsIndex.dateIndex.year = year;
                head->itemsIndex.dateIndex.hours = hours;
                head->itemsIndex.dateIndex.mins = mins;
                head->data = serial;
                break;
        case '3':
                strcpy(category, "Candy");
                strcpy(head->itemsIndex.category, "Candy");
                printf("Enter Serial: ");
                scanf("%d", &serial);
                head->itemsIndex.serial = serial;

                printf("Enter Brand: ");
                scanf("%s", brand);
                strcpy(head->itemsIndex.brand, removespace(brand));

                printf("Enter Prodact Name: ");
                scanf("%s", productName);
                strcpy(head->itemsIndex.productName, removespace(productName));

                printf("Enter Price: ");
                scanf("%lf", &pricE);
                head->itemsIndex.pricE = pricE;

                printf("In stock?\nPress 0 if No\nPress 1 or else if Yes\n");
                scanf(" %d", &temp);
                if (temp == 0)
                {
                        strcpy(instock, "No");
                        head->itemsIndex.stock = false;
                }
                else
                {
                        strcpy(instock, "Yes");
                        head->itemsIndex.stock = true;
                }

                getDateTime(&day, &month, &year, &hours, &mins);
                head->itemsIndex.dateIndex.day = day;
                head->itemsIndex.dateIndex.month = month;
                head->itemsIndex.dateIndex.year = year;
                head->itemsIndex.dateIndex.hours = hours;
                head->itemsIndex.dateIndex.mins = mins;
                head->data = serial;
                break;
        case '4':
                printf("Type New Category");
                scanf("%s", category);
                strcpy(head->itemsIndex.category, category);

                printf("Enter Serial: ");
                scanf("%d", &serial);
                head->itemsIndex.serial = serial;

                printf("Enter Brand: ");
                scanf("%s", brand);
                strcpy(head->itemsIndex.brand, removespace(brand));

                printf("Enter Prodact Name: ");
                scanf("%s", productName);
                strcpy(head->itemsIndex.productName, removespace(productName));

                printf("Enter Price: ");
                scanf("%lf", &pricE);
                head->itemsIndex.pricE = pricE;

                printf("In stock?\nPress 0 if No\nPress 1 or else if Yes\n");
                scanf(" %d", &temp);
                if (temp == 0)
                {
                        strcpy(instock, "No");
                        head->itemsIndex.stock = false;
                }
                else
                {
                        strcpy(instock, "Yes");
                        head->itemsIndex.stock = true;
                }

                getDateTime(&day, &month, &year, &hours, &mins);
                head->itemsIndex.dateIndex.day = day;
                head->itemsIndex.dateIndex.month = month;
                head->itemsIndex.dateIndex.year = year;
                head->itemsIndex.dateIndex.hours = hours;
                head->itemsIndex.dateIndex.mins = mins;
                head->data = serial;
                break;
        default:
                break;
        }
        fprintf(fp, "\n%-10d %-10s %-10s %-30s %-10.2lf %-9s %02d/%02d/%d, %02d:%02d", serial, category, brand, productName, pricE, instock, day, month, year, hours, mins);
        head = head->next;
        count = count + 1;
        fclose(fp);
        return count;
}
void searchByBorC(LNode* head, List* lst, int level)
{
        printf("\n");
        LNode* current = lst->head;
        int serial;
        int* arr;
        char string[100], string2[100];
        char aj[1000], mj[1000];
        char operator;
        int x = 0;
        int i = 0, j = 0, len = 0;
        char k;
        bool both = false;
        bool edit = false;
        arr = (int*)malloc(sizeof(int) * MAXsearch);
        printf("search by Brand name: press 1\n");
        printf("search by Prodact name: press 2\n");
        printf("search by Brand and Prodact name: press 3\n");
        printf("For Exit Press 9\n");
        scanf(" %c", &operator);
        do {
                if (operator=='9')
                        x = 0;
                switch (operator)
                {
                case '1':
                        printf("Enter Brand name for search: \n");
                        scanf("%s", &string);
                        while (head != NULL)
                        {
                                strcpy(aj, head->itemsIndex.brand);
                                len = strlen(string);   // len stores the length of the input string
                                if (_strnicmp(string, aj, len) == 0)
                                {
                                        arr[i] = head->itemsIndex.serial;
                                        Printitem(head);
                                        i++;
                                }
                                head = head->next;
                        }
                        if (i == 0){
                                printf("Nothing Found!");
                                break;
                                }
                        if (level != 1)
                        {
                                printf("\nDo You want to edit the items?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                scanf(" %c", &operator);
                                if (operator!='N')
                                {
                                        i = 0;
                                        do {
                                                current = lst->head;
                                                printf("Enter Item serial number\n");
                                                scanf("%d", &serial);
                                                while (current != NULL)
                                                {
                                                        if (serial == current->itemsIndex.serial)
                                                        {
                                                                printf("\nWhat do you want to change in this item?\n");
                                                                printf("Serial     Category   Brand      Prodact Name                   Price      In Stock  Date");
                                                                Printitem(current);
                                                                printf("\n\nPress number from menu\n1.Category\n2.Brand\n3.Prodact Name\n4.Price\n5.In Stock\n6.Date\n7.Back to previous menu\n");
                                                                scanf(" %c", &k);
                                                                if (k == '7')
                                                                        continue;
                                                                edititem(current, k);
                                                                i++;
                                                        }
                                                        current = current->next;
                                                }
                                                if (i == 0)
                                                        printf("Wrong serial Number enterd!\n");
                                                printf("Edit another product?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                                scanf(" %c", &operator);
                                        } while (operator!='N');

                                }
                        }
                        break;

                case '2':
                        printf("Enter Prodact name for search: \n");
                        scanf("%s", &string);
                        while (head != NULL)
                        {
                                strcpy(aj, head->itemsIndex.productName);
                                len = strlen(string);   // len stores the length of the input string
                                if (_strnicmp(string, aj, len) == 0)
                                {
                                        Printitem(head);
                                        arr[i] = head->itemsIndex.serial;
                                        i++;
                                }
                                head = head->next;
                        }
                        if (i == 0) {
                                printf("Nothing Found!");
                                break;
                        }
                        if (level != 1)
                        {
                                printf("\nDo You want to edit the items?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                scanf(" %c", &operator);
                                if (operator!='N')
                                {
                                        i = 0;
                                        do {
                                                current = lst->head;
                                                printf("Enter Item serial number\n");
                                                scanf("%d", &serial);
                                                while (current != NULL)
                                                {
                                                        if (serial == current->itemsIndex.serial)
                                                        {
                                                                printf("\nWhat do you want to change in this item?\n");
                                                                printf("Serial     Category   Brand      Prodact Name                   Price      In Stock  Date");
                                                                Printitem(current);
                                                                printf("\n\nPress number from menu\n1.Category\n2.Brand\n3.Prodact Name\n4.Price\n5.In Stock\n6.Date\n7.Back to previous menu\n");
                                                                scanf(" %c", &k);
                                                                if (k == '7')
                                                                        continue;
                                                                edititem(current, k);
                                                                i++;
                                                        }
                                                        current = current->next;
                                                }
                                                if (i == 0)
                                                        printf("Wrong serial Number enterd!\n");
                                                printf("Edit another product?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                                scanf(" %c", &operator);
                                        } while (operator!='N');

                                }
                        }
                        break;
                case '3':
                        printf("Enter Brand name for search: ");
                        scanf(" %s", &string);
                        printf("Enter Prodact name for search: \n");
                        scanf(" %s", &string2);
                        while (head != NULL)
                        {
                                both = false;
                                strcpy(aj, head->itemsIndex.brand);
                                len = strlen(string);   // len stores the length of the input string
                                if (_strnicmp(string, aj, len) == 0)
                                {
                                        both = true;
                                }
                                strcpy(aj, head->itemsIndex.productName);
                                len = strlen(string2);   // len stores the length of the input string
                                if (_strnicmp(string2, aj, len) == 0)
                                {
                                        both = true;
                                }
                                if (both)
                                {
                                        Printitem(head);
                                        arr[i] = head->itemsIndex.serial;
                                        i++;
                                }
                                head = head->next;
                        }
                        if (i == 0) {
                                printf("Nothing Found!");
                                break;
                        }
                        if (level != 1)
                        {
                                printf("\nDo You want to edit the items?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                scanf(" %c", &operator);
                                if (operator!='N')
                                {
                                        i = 0;
                                        do {
                                                current = lst->head;
                                                printf("Enter Item serial number\n");
                                                scanf("%d", &serial);
                                                while (current != NULL)
                                                {
                                                        if (serial == current->itemsIndex.serial)
                                                        {
                                                                printf("\nWhat do you want to change in this item?\n");
                                                                printf("Serial     Category   Brand      Prodact Name                   Price      In Stock  Date");
                                                                Printitem(current);
                                                                printf("\n\nPress number from menu\n1.Category\n2.Brand\n3.Prodact Name\n4.Price\n5.In Stock\n6.Date\n7.Back to previous menu\n");
                                                                scanf(" %c", &k);
                                                                if (k == '7')
                                                                        continue;
                                                                edititem(current, k);
                                                                i++;
                                                        }
                                                        current = current->next;
                                                }
                                                if (i == 0)
                                                        printf("Wrong serial Number enterd!\n");
                                                printf("Edit another product?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                                scanf(" %c", &operator);
                                        } while (operator!='N');

                                }
                        }
                        break;
                case '9':
                        x = 0;
                        break;
                default:
                        printf("Worng Value Enterd choose the currect number from the menu\nFor Exit Press 9");
                        scanf(" %c", &operator);
                        if (operator=='9')
                                x = 0;
                        break;
                }
        } while (x != 0);
        free(arr);
}
void searchByValue(LNode* head, List* lst, int level)
{
        printf("\n");
        LNode* current = lst->head;
        int serial;
        int* arr;
        char string[100], string2[100];
        char aj[1000], mj[1000];
        char operator;
        int x = 0;
        int i = 0, j = 0, len = 0;
        char k;
        int price = 0;
        bool both = false;
        arr = (int*)malloc(sizeof(int) * MAXsearch);
        printf("search Items by MAX Value Price: Press 1\n");
        printf("search Items by MIN Value Price: Press 2\n");
        printf("For Exit Press 9\n");
        scanf(" %c", &operator);
        do {
                if (operator=='9')
                        x = 0;
                switch (operator)
                {
                case '1':
                        printf("Enter MAX value Price for search: ");
                        scanf("%d", &price);
                        while (head != NULL)
                        {
                                if (head->itemsIndex.pricE <= price)
                                {
                                        arr[i] = head->itemsIndex.serial;
                                        Printitem(head);
                                        i++;
                                }
                                head = head->next;
                        }
                        if (i == 0) {
                                printf("Nothing Found!");
                                break;
                        }
                        if (level != 1)
                        {
                                printf("\nDo You want to edit the items?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                scanf(" %c", &operator);
                                if (operator!='N')
                                {
                                        i = 0;
                                        do {
                                                current = lst->head;
                                                printf("Enter Item serial number\n");
                                                scanf("%d", &serial);
                                                while (current != NULL)
                                                {
                                                        if (serial == current->itemsIndex.serial)
                                                        {
                                                                printf("\nWhat do you want to change in this item?\n");
                                                                printf("Serial     Category   Brand      Prodact Name                   Price      In Stock  Date");
                                                                Printitem(current);
                                                                printf("\n\nPress number from menu\n1.Category\n2.Brand\n3.Prodact Name\n4.Price\n5.In Stock\n6.Date\n7.Back to previous menu\n");
                                                                scanf(" %c", &k);
                                                                if (k == '7')
                                                                        continue;
                                                                edititem(current, k);
                                                                i++;
                                                        }
                                                        current = current->next;
                                                }
                                                if (i == 0)
                                                        printf("Wrong serial Number enterd!\n");
                                                printf("Edit another product?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                                scanf(" %c", &operator);
                                        } while (operator!='N');

                                }
                        }
                        break;
                case '2':

                        printf("Enter MIN value Price for search: ");
                        scanf("%d", &price);
                        while (head != NULL)
                        {
                                if (head->itemsIndex.pricE >= price)
                                {
                                        Printitem(head);
                                        arr[i] = head->itemsIndex.serial;
                                        i++;
                                }
                                head = head->next;
                        }
                        if (i == 0) {
                                printf("Nothing Found!");
                                break;
                        }
                        if (level != 1)
                        {
                                printf("\nDo You want to edit the items?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                scanf(" %c", &operator);
                                if (operator!='N')
                                {
                                        i = 0;
                                        do {
                                                current = lst->head;
                                                printf("Enter Item serial number\n");
                                                scanf("%d", &serial);
                                                while (current != NULL)
                                                {
                                                        if (serial == current->itemsIndex.serial)
                                                        {
                                                                printf("\nWhat do you want to change in this item?\n");
                                                                printf("Serial     Category   Brand      Prodact Name                   Price      In Stock  Date");
                                                                Printitem(current);
                                                                printf("\n\nPress number from menu\n1.Category\n2.Brand\n3.Prodact Name\n4.Price\n5.In Stock\n6.Date\n7.Back to previous menu\n");
                                                                scanf(" %c", &k);
                                                                if (k == '7')
                                                                        continue;
                                                                edititem(current, k);
                                                                i++;
                                                        }
                                                        current = current->next;
                                                }
                                                if (i == 0)
                                                        printf("Wrong serial Number enterd!\n");
                                                printf("Edit another product?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                                scanf(" %c", &operator);
                                        } while (operator!='N');

                                }
                        }
                        break;
                case '9':
                        x = 0;
                        break;
                default:
                        printf("Worng Value Enterd choose the currect number from the menu\nFor Exit Press 9");
                        scanf(" %c", &operator);
                        if (operator=='9')
                                x = 0;
                        break;
                }
        } while (x != 0);
        free(arr);
}
void searchByStock(LNode* head, List* lst, int level)
{
        printf("\n");
        LNode* current = lst->head;
        int serial;
        int* arr;
        char string[100], string2[100];
        char aj[1000], mj[1000];
        char operator;
        int x = 0;
        int i = 0, j = 0, len = 0;
        char k;
        int price;
        bool both = false;
        arr = (int*)malloc(sizeof(int) * MAXsearch);
        printf("1.search Items that IN Stock: Press 1\n");
        printf("2.search Items that OUT Stock: Press 2\n");
        printf("3.For Exit Press 9\n");
        scanf(" %c", &operator);
        do {
                if (operator=='9')
                        x = 0;
                switch (operator)
                {
                case '1':
                        while (head != NULL)
                        {
                                if (head->itemsIndex.stock)
                                {
                                        arr[i] = head->itemsIndex.serial;
                                        Printitem(head);
                                        i++;
                                }
                                head = head->next;
                        }
                        if (i == 0) {
                                printf("Nothing Found!");
                                break;
                        }
                        if (level != 1)
                        {
                                printf("\nDo You want to edit the items?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                scanf(" %c", &operator);
                                if (operator!='N')
                                {
                                        i = 0;
                                        do {
                                                current = lst->head;
                                                printf("Enter Item serial number\n");
                                                scanf("%d", &serial);
                                                while (current != NULL)
                                                {
                                                        if (serial == current->itemsIndex.serial)
                                                        {
                                                                printf("\nWhat do you want to change in this item?\n");
                                                                printf("Serial     Category   Brand      Prodact Name                   Price      In Stock  Date");
                                                                Printitem(current);
                                                                printf("\n\nPress number from menu\n1.Category\n2.Brand\n3.Prodact Name\n4.Price\n5.In Stock\n6.Date\n7.Back to previous menu\n");
                                                                scanf(" %c", &k);
                                                                if (k == '7')
                                                                        continue;
                                                                edititem(current, k);
                                                                i++;
                                                        }
                                                        current = current->next;
                                                }
                                                if (i == 0)
                                                        printf("Wrong serial Number enterd!\n");
                                                printf("Edit another product?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                                scanf(" %c", &operator);
                                        } while (operator!='N');

                                }
                        }
                        break;
                case '2':
                        while (head != NULL)
                        {
                                if (!head->itemsIndex.stock)
                                {
                                        arr[i] = head->itemsIndex.serial;
                                        Printitem(head);
                                        i++;
                                }
                                head = head->next;
                        }
                        if (i == 0) {
                                printf("Nothing Found!");
                                break;
                        }
                        if (level != 1)
                        {
                                printf("\nDo You want to edit the items?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                scanf(" %c", &operator);
                                if (operator!='N')
                                {
                                        i = 0;
                                        do {
                                                current = lst->head;
                                                printf("Enter Item serial number\n");
                                                scanf("%d", &serial);
                                                while (current != NULL)
                                                {
                                                        if (serial == current->itemsIndex.serial)
                                                        {
                                                                printf("\nWhat do you want to change in this item?\n");
                                                                printf("Serial     Category   Brand      Prodact Name                   Price      In Stock  Date");
                                                                Printitem(current);
                                                                printf("\n\nPress number from menu\n1.Category\n2.Brand\n3.Prodact Name\n4.Price\n5.In Stock\n6.Date\n7.Back to previous menu\n");
                                                                scanf(" %c", &k);
                                                                if (k == '7')
                                                                        continue;
                                                                edititem(current, k);
                                                                i++;
                                                        }
                                                        current = current->next;
                                                }
                                                if (i == 0)
                                                        printf("Wrong serial Number enterd!\n");
                                                printf("Edit another product?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                                scanf(" %c", &operator);
                                        } while (operator!='N');

                                }
                        }
                        break;
                case '9':
                        x = 0;
                        break;
                default:
                        printf("Worng Value Enterd choose the currect number from the menu\nFor Exit Press 9");
                        scanf(" %c", &operator);
                        if (operator=='9')
                                x = 0;
                        break;
                }
        } while (x != 0);
        free(arr);
}
void searchByDate(LNode* head, List* lst, int level)
{
        printf("\n");
        LNode* current = lst->head;
        int serial;
        int* arr;
        char string[100], string2[100];
        char aj[1000], mj[1000];
        char operator;
        int x = 0;
        int i = 0, j = 0, len = 0;
        char k;
        int price;
        bool time = false;
        bool HourMin = false;
        int day = 0, month = 0, year = 0, hours = 0, mins = 0;
        arr = (int*)malloc(sizeof(int) * MAXsearch);
        printf("Search Items Before the Date : Press 1\n");
        printf("Search Items After the Date : Press 2\n");
        printf("For Exit Press 9\n");
        scanf(" %c", &operator);
        do {
                if (operator=='9')
                        x = 0;
                switch (operator)
                {
                case '1':
                        printf("Enter Year\n");
                        scanf("%d", &year);
                        printf("Enter Month\n");
                        scanf("%d", &month);
                        printf("Enter day\n");
                        scanf("%d", &day);
                        printf("You want Also by Hours and Minutes?\nIf YES Press Y(or any key)\nNO? press N\n");
                        scanf(" %c", &operator);
                        if (operator!='N')
                        {
                                HourMin = true;
                                printf("Print Enter Hours\n");
                                scanf("%d", &hours);
                                printf("Print Enter Minutes\n");
                                scanf("%d", &mins);
                        }
                        while (head != NULL)
                        {
                                if (head->itemsIndex.dateIndex.year <= year)
                                        if (head->itemsIndex.dateIndex.month <= month)
                                                if (head->itemsIndex.dateIndex.day <= day)
                                                        if (!HourMin)
                                                        {
                                                                arr[i] = head->itemsIndex.serial;
                                                                Printitem(head);
                                                                i++;
                                                        }
                                                        else
                                                                if (head->itemsIndex.dateIndex.hours <= hours)
                                                                        if (head->itemsIndex.dateIndex.mins <= mins)
                                                                        {
                                                                                arr[i] = head->itemsIndex.serial;
                                                                                Printitem(head);
                                                                                i++;
                                                                        }

                                head = head->next;
                        }
                        if (i == 0) {
                                printf("Nothing Found!");
                                break;
                        }
                        if (level != 1)
                        {
                                printf("\nDo You want to edit the items?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                scanf(" %c", &operator);
                                if (operator!='N')
                                {
                                        i = 0;
                                        do {
                                                current = lst->head;
                                                printf("Enter Item serial number\n");
                                                scanf("%d", &serial);
                                                while (current != NULL)
                                                {
                                                        if (serial == current->itemsIndex.serial)
                                                        {
                                                                printf("\nWhat do you want to change in this item?\n");
                                                                printf("Serial     Category   Brand      Prodact Name                   Price      In Stock  Date");
                                                                Printitem(current);
                                                                printf("\n\nPress number from menu\n1.Category\n2.Brand\n3.Prodact Name\n4.Price\n5.In Stock\n6.Date\n7.Back to previous menu\n");
                                                                scanf(" %c", &k);
                                                                if (k == '7')
                                                                        continue;
                                                                edititem(current, k);
                                                                i++;
                                                        }
                                                        current = current->next;
                                                }
                                                if (i == 0)
                                                        printf("Wrong serial Number enterd!\n");
                                                printf("Edit another product?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                                scanf(" %c", &operator);
                                        } while (operator!='N');

                                }
                        }
                        break;

                case '2':
                        printf("Enter Year\n");
                        scanf("%d", &year);
                        printf("Enter Month\n");
                        scanf("%d", &month);
                        printf("Enter day\n");
                        scanf("%d", &day);
                        printf("You want Also by Hours and Minutes?\nIf YES Press Y(or any key)\nNO? press N\n ");
                        scanf(" %c", &operator);
                        if (operator!='N')
                        {
                                HourMin = true;
                                printf("Print Enter Hours\n");
                                scanf("%d", &hours);
                                printf("Print Enter Minutes\n");
                                scanf("%d", &mins);
                        }
                        while (head != NULL)
                        {
                                if (head->itemsIndex.dateIndex.year >= year)
                                        if (head->itemsIndex.dateIndex.month >= month)
                                                if (head->itemsIndex.dateIndex.day >= day)
                                                        if (!HourMin)
                                                        {
                                                                arr[i] = head->itemsIndex.serial;
                                                                Printitem(head);
                                                                i++;
                                                        }
                                                        else
                                                                if (head->itemsIndex.dateIndex.hours >= hours)
                                                                        if (head->itemsIndex.dateIndex.mins >= mins)
                                                                        {
                                                                                arr[i] = head->itemsIndex.serial;
                                                                                Printitem(head);
                                                                                i++;
                                                                        }

                                head = head->next;
                        }
                        if (i == 0) {
                                printf("Nothing Found!");
                                break;
                        }
                        if (level != 1)
                        {
                                printf("\nDo You want to edit the items?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                scanf(" %c", &operator);
                                if (operator!='N')
                                {
                                        i = 0;
                                        do {
                                                current = lst->head;
                                                printf("Enter Item serial number\n");
                                                scanf("%d", &serial);
                                                while (current != NULL)
                                                {
                                                        if (serial == current->itemsIndex.serial)
                                                        {
                                                                printf("\nWhat do you want to change in this item?\n");
                                                                printf("Serial     Category   Brand      Prodact Name                   Price      In Stock  Date");
                                                                Printitem(current);
                                                                printf("\n\nPress number from menu\n1.Category\n2.Brand\n3.Prodact Name\n4.Price\n5.In Stock\n6.Date\n7.Back to previous menu\n");
                                                                scanf(" %c", &k);
                                                                if (k == '7')
                                                                        continue;
                                                                edititem(current, k);
                                                                i++;
                                                        }
                                                        current = current->next;
                                                }
                                                if (i == 0)
                                                        printf("Wrong serial Number enterd!\n");
                                                printf("Edit another product?\n1.Press Y(or any key) if Yes\n2.Press N if no\n");
                                                scanf(" %c", &operator);
                                        } while (operator!='N');

                                }
                        }
                        break;
                default:
                        printf("Worng Value Enterd choose the currect number from the menu\nFor Exit Press 9");
                        scanf(" %c", &operator);
                        if (operator=='9')
                                x = 0;
                        break;
                }
        } while (x != 0);
        free(arr);
}
void fprintitem(char path, LNode* head)
{
        char instock='a';
        FILE* fp = fopen(path, "a");
        if (head->itemsIndex.stock)
                strcpy(instock, "Yes");
        else
                strcpy(instock, "NO");
        fprintf(fp, "\n%-10d %-10s %-10s %-30s %-10.2lf %-9s %02d/%02d/%d, %02d:%02d", head->itemsIndex.serial, head->itemsIndex.category, head->itemsIndex.brand, head->itemsIndex.productName, head->itemsIndex.pricE, instock, head->itemsIndex.dateIndex.day, head->itemsIndex.dateIndex.month, head->itemsIndex.dateIndex.year, head->itemsIndex.dateIndex.hours, head->itemsIndex.dateIndex.mins);
        fclose(fp);
}
void edititem(LNode* head, char operator)
{
        int day = 0, month = 0, year = 0, hours = 0, mins = 0;
        int serial = 0;
        char category[17];
        char brand[17];
        char catinput = 'a';
        char productName[31];
        char instock[11];
        double pricE = 0;
        int x = -1;
        char temp;
        bool stock = true;

        do {
                switch (operator)
                {
                case '1':
                        printf("Type Category\n");
                        scanf("%s", category);
                        strcpy(head->itemsIndex.category, category);
                        break;
                case '2':
                        printf("Enter Brand: \n");
                        scanf("%s", brand);
                        strcpy(head->itemsIndex.brand, removespace(brand));
                        break;
                case '3':
                        printf("Enter Prodact Name: \n");
                        scanf("%s", productName);
                        strcpy(head->itemsIndex.productName, removespace(productName));
                        break;
                case '4':
                        printf("Enter Price: \n");
                        scanf("%lf", &pricE);
                        head->itemsIndex.pricE = pricE;
                        break;
                case '5':
                        printf("In stock?\n1.Press Y(or any key) if In stock\n2.Press N if Out stock\n");
                        scanf(" %c", &temp);
                        if (temp == 'N')
                        {
                                strcpy(instock, "No");
                                head->itemsIndex.stock = false;
                        }
                        else
                        {
                                strcpy(instock, "Yes");
                                head->itemsIndex.stock = true;
                        }
                        break;
                case '6':
                        printf("Enter Year\n");
                        scanf("%d", &year);
                        head->itemsIndex.dateIndex.year = year;
                        printf("Enter Month\n");
                        scanf("%d", &month);
                        head->itemsIndex.dateIndex.month = month;
                        printf("Enter Day\n");
                        scanf("%d", &day);
                        head->itemsIndex.dateIndex.day = day;
                        printf("Enter Hours\n");
                        scanf("%d", &hours);
                        head->itemsIndex.dateIndex.hours = hours;
                        printf("Enter Min\n");
                        scanf("%d", &mins);
                        head->itemsIndex.dateIndex.mins = mins;
                        break;
                default:
                        printf("Worng Value Enterd choose the currect number from the menu\n");
                        printf("What do you want to change?\nPress number from menu\n1.Category\n2.Brand\n3.Prodact Name\n4.Price\n5.In Stock\n6.Date\n");
                        printf("For Exit Press 9\n");
                        scanf(" %c", &operator);
                        if (operator=='9')
                                x = 0;
                        break;
                }
                printf("Want to Edit More? Press Y or any key to exit\n");
                scanf(" %c", &operator);
                if (operator=='Y')
                {
                        x = -1;
                        printf("What do you want to change more?\nPress number from menu\n1.Category\n2.Brand\n3.Prodact Name\n4.Price\n5.In Stock\n6.Date\n");
                        scanf(" %c", &operator);
                }
                else
                        x = 0;
        } while (x != 0);
}
void editSingelitem(LNode* head, int serial)
{
        int day = 0, month = 0, year = 0, hours = 0, mins = 0;
        char category[17];
        char brand[17];
        char catinput = 'a';
        char productName[31];
        char instock[11];
        double pricE = 0;
        int x = -1;
        char temp;
        char operator;
        bool stock = true;
        while (head != NULL)
        {
                if(head->itemsIndex.serial == serial)
                break;
                head = head->next;
        }

        printf("Worng Value Enterd choose the currect number from the menu\n");
        printf("What do you want to change?\nPress number from menu\n1.Category\n2.Brand\n3.Prodact Name\n4.Price\n5.In Stock\n6.Date\n");
        printf("For Exit Press 9\n");
        scanf(" %c", &operator);
        do {
                switch (operator)
                {
                case '1':
                        printf("Type Category\n");
                        scanf("%s", category);
                        strcpy(head->itemsIndex.category, category);
                        break;
                case '2':
                        printf("Enter Brand: \n");
                        scanf("%s", brand);
                        strcpy(head->itemsIndex.brand, removespace(brand));
                        break;
                case '3':
                        printf("Enter Prodact Name: \n");
                        scanf("%s", productName);
                        strcpy(head->itemsIndex.productName, removespace(productName));
                        break;
                case '4':
                        printf("Enter Price: \n");
                        scanf("%lf", &pricE);
                        head->itemsIndex.pricE = pricE;
                        break;
                case '5':
                        printf("In stock?\n1.Press Y(or any key) if In stock\n2.Press N if Out stock\n");
                        scanf(" %c", &temp);
                        if (temp == 'N')
                        {
                                strcpy(instock, "No");
                                head->itemsIndex.stock = false;
                        }
                        else
                        {
                                strcpy(instock, "Yes");
                                head->itemsIndex.stock = true;
                        }
                        break;
                case '6':
                        printf("Enter Year\n");
                        scanf("%d", &year);
                        head->itemsIndex.dateIndex.year = year;
                        printf("Enter Month\n");
                        scanf("%d", &month);
                        head->itemsIndex.dateIndex.month = month;
                        printf("Enter Day\n");
                        scanf("%d", &day);
                        head->itemsIndex.dateIndex.day = day;
                        printf("Enter Hours\n");
                        scanf("%d", &hours);
                        head->itemsIndex.dateIndex.hours = hours;
                        printf("Enter Min\n");
                        scanf("%d", &mins);
                        head->itemsIndex.dateIndex.mins = mins;
                        break;
                default:
                        printf("Worng Value Enterd choose the currect number from the menu\n");
                        printf("What do you want to change?\nPress number from menu\n1.Category\n2.Brand\n3.Prodact Name\n4.Price\n5.In Stock\n6.Date\n");
                        printf("For Exit Press 9\n");
                        scanf(" %c", &operator);
                        if (operator=='9')
                                x = 0;
                        break;
                }
                printf("Want to Edit More? Press Y or any key to exit\n");
                scanf(" %c", &operator);
                if (operator=='Y')
                {
                        x = -1;
                        printf("What do you want to change more?\nPress number from menu\n1.Category\n2.Brand\n3.Prodact Name\n4.Price\n5.In Stock\n6.Date\n");
                        scanf(" %c", &operator);
                }
                else
                        x = 0;
        } while (x != 0);
}
void editSingleuser(LNode* head, char* username)
{
        int day = 0, month = 0, year = 0, hours = 0, mins = 0;
        int level;
        char temp[100];
        char newusername[16];
        char password[16];
        char fullname[21];
        int x = -1;
        char operator;
        bool stock = true;
        while (head != NULL)
        {
                if (_strcmpi(head->usersIndex.username,username)==0)
                break;
                head = head->next;
        }

        printf("\nWhat do you want to change?\n\nPress number from menu\n1.Username\n2.Password\n3.level\n4.Full name\n");
        printf("For Exit Press 9\n");
        scanf(" %c", &operator);
        do {
                switch (operator)
                {
                case '1':
                        printf("Type username\n");
                        scanf("%s", newusername);
                        strcpy(head->usersIndex.username, newusername);
                        break;
                case '2':
                        printf("Enter password: \n");
                        scanf("%s", password);
                        strcpy(head->usersIndex.password, removespace(password));
                        break;
                case '3':
                        printf("Enter level: \n");
                        scanf("%d", &level);
                        head->usersIndex.level = level;
                        break;
                case '4':
                        printf("Enter Full name: \n");
                        scanf("%s", fullname);
                        strcpy(head->usersIndex.fullname,fullname);
                        break;
                default:
                        printf("Worng Value Enterd choose the currect number from the menu\n");
                        printf("What do you want to change?\nPress number from menu\n1.Username\n2.Password\n3.level\n4.Full name\n");
                        printf("For Exit Press 9\n");
                        scanf(" %c", &operator);
                        if (operator=='9')
                                x = 0;
                        break;
                }
                printf("Want to Edit More? Press Y or any key to exit\n");
                scanf(" %c", &operator);
                if (operator=='Y')
                {
                        x = -1;
                        printf("What do you want to change?\nPress number from menu\n1.Username\n2.Password\n3.level\n4.Full name\n");
                        scanf(" %c", &operator);
                }
                else
                        x = 0;
        } while (x != 0);
}
void initList(List* lst)
{
        lst->head = lst->tail = NULL;
}
int deleteLast(List* lst)
{
        //if list is empty there is nothing to delete
        if (!isEmpty(lst))
        {
                //save reference to last link
                LNode* temp = lst->tail;

                //if only one link
                if (temp->prev == NULL)
                {
                        initList(lst);
                }
                else
                {
                        lst->tail->prev->next = NULL;
                        lst->tail = lst->tail->prev;
                }
                free(temp);
                return 1;
        }
        return 0;
}
void Printitem(LNode* head)
{
        char instock[11];
        if (head->itemsIndex.stock)
                strcpy(instock, "Yes");
        else
                strcpy(instock, "No");
        printf("\n%-10d %-10s %-10s %-30s %-10.2lf %-9s %02d/%02d/%d, %02d:%02d", head->itemsIndex.serial, head->itemsIndex.category, head->itemsIndex.brand, head->itemsIndex.productName, head->itemsIndex.pricE, instock, head->itemsIndex.dateIndex.day, head->itemsIndex.dateIndex.month, head->itemsIndex.dateIndex.year, head->itemsIndex.dateIndex.hours, head->itemsIndex.dateIndex.mins);
}
int moveforward(LNode* current, LNode* head, LNode* tail)
{
        if (current == tail)
                return head;
        else
                current = current->next;
        return current;
}
int movebackward(LNode* current, LNode* head, LNode* tail)
{
        if (current == head)
                return tail;
        else
                current = current->prev;
        return current;
}
char* removespace(char txt[])
{
        char aj[100], mj[100], temp[1001], temp2[100];
        int n, m, len;
        n = 0, m = 0;
        strcpy(aj, txt);
        len = strlen(aj);   // len stores the length of the input string
        while (aj[n] != '\0')    // till string doesn't terminate
        {
                if (aj[n] != ' ') {
                        mj[m++] = aj[n];
                }
                else {
                        break;
                }
                n++;
        }
        mj[m] = '\0';
        strcpy(temp, mj);
        return temp;
}
LNode* createNode()
{
        LNode* temp; // declare a node
        temp = (LNode*)malloc(sizeof(LNode)); // allocate memory using malloc()
        temp->next = NULL;// make next point to NULL
        return temp;//return the new node
}
LNode* removeNode()
{
        LNode* temp; // declare a node
        temp = (LNode*)malloc(sizeof(LNode)); // allocate memory using malloc()
        temp->prev = NULL;// make next point to NULL
        return temp;//return the new node
}
LNode* addNode(LNode* head, int value)
{
        LNode* temp, * p;// declare two nodes temp and p
        temp = createNode();//createNode will return a new node with data = value and next pointing to NULL.
        temp->data = value; // add element's value to data part of node
        if (head == NULL)
        {
                head = temp; //when linked list is empty
        }
        else
        {
                p = head;//assign head to p
                while (p->next != NULL)
                {
                        p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
                }
                temp->prev = p;
                p->next = temp;//Point the previous last node to the new node created.

        }
        return head;
}
LNode* getLinkByIndex(List* lst, int idx)
{
        int i;
        //start from the first link
        LNode* link = lst->head;

        //navigate through list
        for (i = 0; i < idx && link != NULL; i++)
                link = link->next;

        return link;
}
List makeEmptyList()
{
        List lst;
        lst.head = lst.tail = NULL;
        return lst;
}
int getlistLength(const List* lst)
{
        int count = 0;
        LNode* temp = lst->head;
        while (temp != NULL)
        {
                count++;
                temp = temp->next;
        }
        return count;
}
void freeList(List* thelist)
{
        LNode* current = thelist->head;
        LNode* next;
        while (current)
        {
                next = current->next;
                free(current);
                current = next;
        }
        thelist->head = NULL;
}
void printCharList(const List* lst)
{
        LNode* temp = lst->head;
        while (temp != NULL)
        {
                printf("%c", temp->data);
                temp = temp->next;
        }
}
int isEmpty(const List* lst)
{
        return lst->head == NULL;
}
void insertNodeToTail(List* lst, LNode* newNode)
{
        if (isEmpty(lst))
                lst->head = lst->tail = newNode;
        else
        {
                lst->tail->next = newNode;
                lst->tail = newNode;
        }
}
int fPrintitemRemove(char* path, items_t* theItems, LNode* head, LNode* tail, int count,int inputserial)
{
        printf("\n");
        FILE* fp;
        int day = 0, month = 0, year = 0, hours = 0, mins = 0;
        int serial;
        char category[17];
        char brand[17];
        char catinput = 'a';
        char productName[31];
        char instock[11];
        double pricE = 0;
        int temp, x = -1, i = -1;
        bool stock = true;
        fopen_s(&fp, path, "w+");
        if (!fp)
        {
                printf("File not found!\n");
                return;
        }
        fprintf(fp, "Serial     Category   Brand      Prodact Name                   Price      In Stock  Date");
        while (head != NULL)
        {
                if (head->itemsIndex.serial == inputserial) {
                        head = head->next;
                        continue;
                }
                serial = head->itemsIndex.serial;
                strcpy(category, head->itemsIndex.category);
                strcpy(brand, head->itemsIndex.brand);
                strcpy(productName, head->itemsIndex.productName);
                pricE = head->itemsIndex.pricE;
                if (head->itemsIndex.stock)
                        strcpy(instock, "Yes");
                else
                        strcpy(instock, "NO");
                year = head->itemsIndex.dateIndex.year;
                month = head->itemsIndex.dateIndex.month;
                day = head->itemsIndex.dateIndex.day;
                hours = head->itemsIndex.dateIndex.hours;
                mins = head->itemsIndex.dateIndex.mins;

                fprintf(fp, "\n%-10d %-10s %-10s %-30s %-10.2lf %-9s %02d/%02d/%d, %02d:%02d", serial, category, brand, productName, pricE, instock, day, month, year, hours, mins);
                head = head->next;
                i++;
        }
        fclose(fp);
        return (i + 1);
}
int fPrintuserRemove(char* path, users_t* theItems, LNode* head, LNode* tail, int count,char* inputusername)
{

        FILE* fp;
        int level, i = -1, k = 0;
        char temp[100], username[16], password[16], fullname[21];

        fopen_s(&fp, path, "w");
        if (!fp)
        {
                printf("File not found!\n");
                return;
        }
        fprintf(fp, "username        password        L fullname            ");
        while (head != NULL)
        {
                if (_strcmpi(head->usersIndex.username,inputusername)==0) {
                        head = head->next;
                        continue;
                }
                level = head->usersIndex.level;
                strcpy(username, head->usersIndex.username);
                strcpy(password, head->usersIndex.password);
                strcpy(fullname, head->usersIndex.fullname);

                fprintf_s(fp, "\n%-15s %-15s %-1d %-20s", username, password, level, fullname);
                head = head->next;
                i++;
        }
        fclose(fp);
        return (i + 1);
}

      run: /*
Assigned by:
*/
#ifndef store
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <string.h>
#include <time.h>
/*
* programm menu - options to choose what to do with the playlist
* e.g.: play song, next song, prev song, etc.
*/

void menu(char* userspath, char* itemspath);
#define _CRT_SECURE_NO_WARNINGS
#define MAXsearch 50
#define USERS_PATH "users.txt"
/***
* This is the path where the users are located.
***/
#define ITEMS_PATH "items.txt"
/***
* This is the path where the item file is located.
***/
#define LOG_PATH "log.txt"
/***
* This is the path where the log file is located.
* Open this file and append new records into it.
***/

typedef struct users
{
        int level;
        char temp[100];
        char username[16];
        char password[16];
        char fullname[21];
}users_t;
typedef struct date
{
        int day;
        int month;
        int year;
        int hours;
        int mins;
}date_t;
typedef struct items
{
        int serial;
        char category[17];
        char brand[17];
        char productName[31];
        double pricE;
        bool stock;
        date_t dateIndex;
        struct items_t* next;
        struct items_t* prev;

}items_t;
typedef struct LNode
{
        int data;
        users_t usersIndex;
        items_t itemsIndex;
        struct LNode* next;
        struct LNode* prev;
}LNode;
typedef struct List
{
        LNode* head;
        LNode* tail;
}List;
int readUserswithoutprint(char* path, users_t* theusers, LNode* head, LNode* tail, List* lst);
int Short(LNode* head,List* lst);
void fprintitem(char path, LNode* head);
fprintTOlog(char* line);
int adddataUser(char* path, users_t* theUsers, LNode* head, LNode* tail, int count);
int adddataItems(char* path, items_t* theItems, LNode* head, LNode* tail, int count);
List makeEmptyList();
LNode* getLinkByIndex(List* lst, int idx);
int getlistLength(const List* lst);
void freeList(List* thelist);
int isEmpty(const List* lst);
void printCharList(const List* lst);
void menu(char* userpath, char* itemspath);
LNode* addNode(LNode* head, int value);
void Printitem(LNode* head);
int moveforward(LNode* current, LNode* head, LNode* tail);
int movebackward(LNode* current, LNode* head, LNode* tail);
void searchByBorC(LNode* head, List* lst, int level);
void searchByValue(LNode* head, List* lst, int level);
void searchByStock(LNode* head, List* lst, int level);
void searchByDate(LNode* head, List* lst, int level);
char* removespace(char txt[]);
int deleteLast(List* lst);
void initList(List* lst);
void getDateTime(int* day, int* month, int* year, int* hours, int* mins);
int readUsers(char* path, users_t* theusers, LNode* head, LNode* tail, List* lst);
void edititem(LNode* head, char operator);
int fPrintNewitemfile(char* path, items_t* theItems, LNode* head, LNode* tail, int count);
void editSingleuser(LNode* head, char* username);
void editSingelitem(LNode* head, int serial);
int fPrintitemRemove(char* path, items_t* theItems, LNode* head, LNode* tail, int count, int serial);
int fPrintuserRemove(char* path, users_t* theItems, LNode* head, LNode* tail, int count, char* username);
#endif
      run: username        password        L fullname             
admin           admin           3 System Manager       

        
    # Push the image to IBM Container Registry
    - name: #define _CRT_SECURE_NO_DEPRECATE
/*
Assigned by:
*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
typedef int type;
typedef struct LNode
{
        type data;
        struct LNode *next;
        struct LNode *prev;
}LNode;
typedef struct List
{
        struct LNode *head, *tail;
}List;
List copyList(const List* lst);
List makeEmptyList();
LNode* getLinkByIndex(List* lst, int idx);
LNode* createNewNode(type newData, LNode* next);
LNode* findData(List lst, type data);
int getlistLength(const List* lst);
void printList(const List* lst);
void freeList(List* thelist);
int isEmpty(const List* lst);
void insertValueToHead(List* lst, type newData);
void insertValueToTail(List* lst, type newData);
void insertAfterValue(List* lst, type insertAfter, type newValue);
int insertAfter(List* lst, LNode* item, LNode* link);
int insertBefore(List* lst, LNode* item, LNode* link);
int insertFirst(List* lst, LNode* item);
void Qtwo(List* lst, int k, int length);
void Qtree(const List* lst);
void removeAll(List* lst, type toremove);
void printCharList(const List* lst);
void main()
{
        int i,num,value,length=0,k=0;
        char ch[256];
        /*List lst=makeEmptyList();
        printf("Enter how many numbers in the list\n");
        scanf("%d", &num);
        for(i=0;i<num;i++)
        {
                printf("enter #%d number: ", i + 1);
                scanf("%d", &value);
                insertValueToTail(&lst, value);
        }
        length=getlistLength(&lst);
        printf("length of the list: %d\n", length);
        printf("%s", checkpelndrom(&lst, length) ? "The number is palendrum\n" : "The number is not palendrum\n");
        //Q2
        printf("Enter K number: ");
        scanf("%d", &k);
        Qtwo(&lst,k,length);
        //Q3*/
        List lststring = makeEmptyList();
        printf("Please enter string\n");
        scanf("%s", &ch);
        for (i = 0; i < strlen(ch); i++)
        {
                insertValueToTail(&lststring, ch[i]);
        }
        printf("The new string without duplicates is : ");
        Qtree(&lststring);

}
void removeAll(List* lst, type toremove)
{
        LNode* current=lst->head, * next;
        if (current == NULL)
                return;
        while (current->data==toremove)
        {
                lst->head = current->next;
                if (lst->head == NULL)
                        lst->tail = NULL;
                free(current);
                if (lst->head == NULL)
                        return;
                else
                        current = lst->head;
        }
        current = lst->head;
        next = current->next;
        while (next)
        {
                if (next->data == toremove)
                {
                        current->next = next->next;
                        free(next);
                        if (current->next == NULL)
                        {
                                lst->tail = current;
                                next = NULL;
                        }
                        else
                        {
                                next = current->next;
                        }
                }
                else
                {
                        current = next;
                        next = current->next;
                }
        }
}
void Qtwo(List* lst, int k,int length)
{
        List temp = makeEmptyList();
        int i = 0,j=0;
        for (; i < length; i++)
        {
                if (getLinkByIndex(lst, i)->data == k)
                {
                        insertValueToTail(&temp, 0);
                        insertValueToTail(&temp, getLinkByIndex(lst, i)->data);
                }
                else
                {
                        insertValueToTail(&temp, getLinkByIndex(lst, i)->data);
                }
        }
        printf("The New List is : ");
        printList(&temp);
}
void Qtree(const List* lst)
{
        int i;
        char c;
        List temp = makeEmptyList();
        temp = copyList(lst);
        List new = makeEmptyList();
        int length = getlistLength(&temp);
        for (i = 0; i < length; i++)
        {
                c=getLinkByIndex(&temp, i)->data;
                removeAll(&temp, c);
                insertValueToTail(&new, c);
                length = getlistLength(&temp);
        }
        printCharList(&new);
}
int insertLast(List* lst, LNode* item)
{
        if (item != NULL)
        {
                //reset item pointers
                item->next = item->prev = NULL;

                if (isEmpty(lst))
                {
                        //make head & tail pointing to item
                        lst->head = lst->tail = item;
                }
                else
                {
                        //update last next link
                        lst->tail->next = item;
                        //point item to old last link
                        item->prev = lst->tail;
                        //point last to new last node
                        lst->tail = item;
                }
                return 1;
        }
        return 0;
}
int insertAfter(List* lst, LNode* item, LNode* link)
{
        if (!isEmpty(lst))
        {
                if ((item == NULL) || (link == NULL))
                        return 0;
                if (link == lst->tail)
                        return insertLast(lst, item);

                //assign new item pointers
                item->prev = link;
                item->next = link->next;
                //chain link and link->next to new item
                link->next->prev = item;
                link->next = item;
                return 1;
        }
        return 0;
}
int insertBefore(List* lst, LNode* item, LNode* link)
{
        if (!isEmpty(lst))
        {
                if ((item == NULL) || (link == NULL))
                        return 0;
                if (link == lst->head)
                        return insertFirst(lst, item);

                //assign new item pointers
                item->next = link;
                item->prev = link->prev;
                //chain link and link->prev to new item
                link->prev->next = item;
                link->prev = item;
                return 1;
        }
        return 0;
}
int insertFirst(List* lst, LNode* item)
{
        if (item != NULL)
        {
                //reset item pointers
                item->next = item->prev = NULL;

                if (isEmpty(lst))
                {
                        //make head & tail pointing to item
                        lst->head = lst->tail = item;
                }
                else
                {
                        //update first prev link
                        lst->head->prev = item;
                        //point item to old first link
                        item->next = lst->head;
                        //point head to new first node
                        lst->head = item;
                }
                return 1;
        }
        return 0;
}
int checkpelndrom(const List* lst,int len)
{
        int i;
                for (i = 0; i < len / 2; i++)
                {
                        if (getLinkByIndex(lst, i)->data != getLinkByIndex(lst, len - 1 - i)->data)
                                return 0;
                }
                return 1;
}
LNode* getLinkByIndex(List* lst, int idx)
{
        int i;
        //start from the first link
        LNode* link = lst->head;

        //navigate through list
        for (i = 0; i < idx && link != NULL; i++)
                link = link->next;

        return link;
}
List copyList(const List* lst)
{
        List res = makeEmptyList();
        LNode* temp = lst->head;
        while (temp)
        {
                insertValueToTail(&res, temp->data);
                temp = temp->next;
        }
        return res;
}
List makeEmptyList()
{
        List lst;
        lst.head = lst.tail = NULL;
        return lst;
}
LNode* createNewNode(type newData, LNode* next)
{
        LNode* newNode = (LNode*)calloc(1, sizeof(LNode));
        newNode->data = newData;
        newNode->next = next;
        return newNode;
}
LNode* findData(List lst, type data)
{
        LNode* temp = lst.head;
        while (temp)
        {
                if (temp->data == data)
                        return temp;
                temp = temp->next;
        }
        return NULL;
}
int getlistLength(const List* lst)
{
        int count = 0;
        LNode* temp = lst->head;
        while (temp!=NULL)
        {
                count++;
                temp = temp->next;
        }
        return count;
}
void freeList(List* thelist)
{
        LNode* current = thelist->head;
        LNode* next;
        while (current)
        {
                next = current->next;
                free(current);
                current = next;
        }
        thelist->head = NULL;
}
void printList(const List* lst)
{
        LNode* temp = lst->head;
        while (temp != NULL)
        {
                printf("%d", temp->data);
                temp = temp->next;
        }
}
void printCharList(const List* lst)
{
        LNode* temp = lst->head;
        while (temp != NULL)
        {
                printf("%c", temp->data);
                temp = temp->next;
        }
}
int isEmpty(const List* lst)
{
        return lst->head == NULL;
}
void insertValueToHead(List* lst, type newData)
{
        LNode* newNode = createNewNode(newData, lst->head);
        if (isEmpty(lst))
                lst->head = lst->tail = newNode;
        else
                lst->head = newNode;
}
void insertValueToTail(List* lst, type newData)
{
        LNode* newNode = createNewNode(newData, NULL);
        if (isEmpty(lst))
                lst->head = lst->tail = newNode;
        else
        {
                lst->tail->next = newNode;
                lst->tail = newNode;
        }
}
void insertNodeToTail(List* lst, LNode* newNode)
{
        if (isEmpty(lst))
                lst->head = lst->tail = newNode;
        else
        {
                lst->tail->next = newNode;
                lst->tail = newNode;
        }
}
void insertAfterValue(List* lst, type insertAfter, type newValue)
{
        LNode* temp = lst->head;
        temp = findData(*lst, insertAfter);
        if (temp)
        {
                LNode* newNode = createNewNode(newValue, temp->next);
                temp->next = newNode;
                if (newNode->next == NULL)
                        lst->tail = newNode;
        }
}
      run: #define _CRT_SECURE_NO_DEPRECATE
/*
Assigned by:

*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <string.h>
typedef struct songs_t
{
        int id;
        int trackNo;
        char artistName[31];
        char albumName[31];
        char trackName[31];
}songs_t;
typedef const songs_t* type;
typedef struct LNode
{
        int data;
        songs_t songsindex;
        struct LNode* next;
        struct LNode* prev;
}LNode;

typedef struct List
{
        LNode* head;
        LNode* tail;
}List;
List copyList(const List* lst);
List makeEmptyList();
LNode* getLinkByIndex(List* lst, int idx);
LNode* createNewNode(type newData, LNode* next);
LNode* findData(List lst, type data);
int getlistLength(const List* lst);
void printList(const List* lst, int i);
void freeList(List* thelist);
int isEmpty(const List* lst);
void insertValueToHead(List* lst, type newData);
void insertValueToTail(List* lst, type newData);
void insertAfterValue(List* lst, type insertAfter, type newValue);
int insertAfter(List* lst, LNode* item, LNode* link);
int insertBefore(List* lst, LNode* item, LNode* link);
int insertFirst(List* lst, LNode* item);
void Qtwo(List* lst, int k, int length);
void Qtree(const List* lst);
void removeAll(List* lst, type toremove);
void printCharList(const List* lst);
void menu();
int importData(char* path, songs_t* thePlaylist, LNode* head, LNode* tail, List* lst);
int createplaylist(char* path, songs_t* thePlaylist);
LNode* addNode(LNode* head, int value);
void playsong(LNode* head);
int moveforward(LNode* current, LNode* head, LNode* tail);
int movebackward(LNode* current, LNode* head, LNode* tail);
void search(LNode* head, LNode* tail);
int deletedata(char* path, songs_t* thePlaylist, LNode* head, LNode* tail, int count);
int adddata(char* path, songs_t* thePlaylist, LNode* head, LNode* tail, int count);
void sortby(List* lst, char* path, int count, LNode* head);
void fprintsong(char path, LNode* head);
void sortList(LNode* head);
void removedata(List* lst, int toremove, LNode* head);
char* removespace(char txt[]);
int deleteLast(List* lst);
void bsort(List* lst);
void initList(List* lst);
struct LNode* swap(struct LNode* ptr1, struct LNode* ptr2);
int bubbleSortArtist(char* path, LNode** head, int count, List* lst);
int bubbleSortAlbum(char* path, LNode** head, int count, List* lst);
int bubbleSortTrack(char* path, LNode** head, int count, List* lst);
void main()
{
        int count = 0, k = 0, x = 0, err = -1;
        char  operator;
        LNode* current;
        LNode* newNode = &current;
        LNode* newNode2;
        LNode* head;
        LNode* tail;
        newNode = (LNode*)calloc(1, sizeof(LNode));
        List lst = makeEmptyList();
        lst.head = newNode;
        newNode2 = (LNode*)calloc(1, sizeof(LNode));
        lst.tail = newNode;
        songs_t playlist;
        count = importData("playlist.txt", &playlist, lst.head, lst.tail, &lst);
        current = lst.head;
        FILE* flog = fopen("logfile.txt", "a+");
        if (!flog)
        {
                printf("File not found!\n");
                return 0;
        }
        do {
                printf("Play :press 1\n");fprintf(flog, "Play :press 1\n");
                printf("Previous Song : Press 2\n"); fprintf(flog, "Previous Song : Press 2\n");
                printf("Next Song : Press 3\n"); fprintf(flog, "Next Song : Press 3\n");
                printf("Search : Press 4\n"); fprintf(flog, "Search : Press 4\n");
                printf("Delete : Press 5\n"); fprintf(flog, "Delete : Press 5\n");
                printf("Add Song : Press 6\n"); fprintf(flog, "Add Song : Press 6\n");
                printf("Sort Playlist by : \n Artist Press 7\nAlbum Press 8\nSong Press 9\n"); fprintf(flog, "Sort Playlist by : \n Artist Press 7\nAlbum Press 8\nSong Press 9\n");
                printf("For Exit Press 0\n"); fprintf(flog, "For Exit Press 0\n");
                scanf(" %c", &operator);
                switch (operator)
                {
                case '1':
                        playsong(current);
                        printf("\n"); fprintf(flog, "\n");
                        break;
                case '2':
                        current = movebackward(current, lst.head, lst.tail);
                        printf("\n"); fprintf(flog, "\n");
                        break;
                case '3':
                        current = moveforward(current, lst.head, lst.tail);
                        break;
                case '4':
                        search(lst.head, lst.tail);
                        printf("\n"); fprintf(flog, "\n");
                        break;
                case '5':
                        count = deletedata("playlist.txt", &playlist, lst.head, lst.tail, count);
                        break;
                case '6':
                        count = adddata("playlist.txt", &playlist, lst.head, lst.tail, count);
                        count = importData("playlist.txt", &playlist, lst.head, lst.tail, &lst);
                        break;
                case '7':
                        bubbleSortArtist("playlist.txt", &lst.head, count - 1, &lst);
                        printf("\n"); fprintf(flog, "\n");
                        break;
                case '8':
                        bubbleSortAlbum("playlist.txt", &lst.head, count - 1, &lst);
                        printf("\n"); fprintf(flog, "\n");
                        break;
                case '9':
                        bubbleSortTrack("playlist.txt", &lst.head, count - 1, &lst);
                        printf("\n"); fprintf(flog, "\n");
                        break;
                default:
                case '0':
                        err = 0;
                        break;
                }
        } while (err != 0);
        fclose(flog);
        freeList(&lst);

}
struct LNode* swap(struct LNode* ptr1, struct LNode* ptr2)
{
        LNode* tmp = ptr2->next;
        ptr2->next = ptr1;
        ptr1->next = tmp;
        return ptr2;
}

/* Function to sort the list */
int bubbleSortArtist(char* path, LNode** head, int count, List* lst)
{
        FILE* flog = fopen("logfile.txt", "a+");
        if (!flog)
        {
                printf("File not found!\n");
                return 0;
        }
        LNode** h;
        int i, j, swapped;
        char h1[6], h2[31], h3[31], h4[10], h5[10];
        for (i = 0; i <= count; i++) {

                h = head;
                swapped = 0;

                for (j = 0; j < count - i; j++) {

                        LNode* p1 = *h;
                        LNode* p2 = p1->next;
                        if (p2 == NULL)
                                continue;
                        if (_strcmpi(p1->songsindex.artistName, p2->songsindex.artistName) > 0) {

                                /* update the link after swapping */
                                *h = swap(p1, p2);
                                swapped = 1;
                        }

                        h = &(*h)->next;
                }

                /* break if the loop ended without any swap */
                if (swapped == 0)
                        break;
        }
        FILE* fp = fopen(path, "r");
        if (!fp)
        {
                printf("File not found!\n"); fprintf(flog, "File not found!\n");
                return 0;
        }
        fscanf(fp, "%5s%30s%30s%10s%30s", h1, h2, h3, h4, h5);
        fclose(fp);
        fp = fopen(path, "w+");
        if (!fp)
        {
                printf("File not found!\n"); fprintf(flog, "File not found!\n");
                return 0;
        }
        fprintf(fp, "%-5s %-30s %-30s %-10s %-30s \n", h1, h2, h3, h4, h5); fprintf(flog, "%-5s %-30s %-30s %-10s %-30s \n", h1, h2, h3, h4, h5);
        printf("%-5s %-30s %-30s %-10s %-30s \n", h1, h2, h3, h4, h5);
        fclose(fp);
        for (i = 0; i <= count; i++)
        {
                fprintsong("playlist.txt", getLinkByIndex(&lst->head, i));
                playsong(getLinkByIndex(&lst->head, i));
        }
        fclose(flog);
}
int bubbleSortAlbum(char* path, LNode** head, int count, List* lst)
{
        FILE* flog = fopen("logfile.txt", "a+");
        if (!flog)
        {
                printf("File not found!\n");
                return 0;
        }
        LNode** h;
        int i, j, swapped;
        char h1[6], h2[31], h3[31], h4[10], h5[10];
        for (i = 0; i <= count; i++) {

                h = head;
                swapped = 0;

                for (j = 0; j < count - i; j++) {

                        LNode* p1 = *h;
                        LNode* p2 = p1->next;
                        if (p2 == NULL)
                                continue;
                        if (_strcmpi(p1->songsindex.albumName, p2->songsindex.albumName) > 0) {

                                /* update the link after swapping */
                                *h = swap(p1, p2);
                                swapped = 1;
                        }

                        h = &(*h)->next;
                }

                /* break if the loop ended without any swap */
                if (swapped == 0)
                        break;
        }
        FILE* fp = fopen(path, "r");
        if (!fp)
        {
                printf("File not found!\n"); fprintf(flog, "File not found!\n");
                return 0;
        }
        fscanf(fp, "%5s%30s%30s%10s%30s", h1, h2, h3, h4, h5);
        fclose(fp);
        fp = fopen(path, "w+");
        if (!fp)
        {
                printf("File not found!\n"); fprintf(flog, "File not found!\n");
                return 0;
        }
        fprintf(fp, "%-5s %-30s %-30s %-10s %-30s \n", h1, h2, h3, h4, h5); fprintf(fp, "%-5s %-30s %-30s %-10s %-30s \n", h1, h2, h3, h4, h5);
        printf("%-5s %-30s %-30s %-10s %-30s \n", h1, h2, h3, h4, h5);
        fclose(fp);
        for (i = 0; i <= count; i++)
        {
                fprintsong("playlist.txt", getLinkByIndex(&lst->head, i));
                playsong(getLinkByIndex(&lst->head, i));
        }
        fclose(flog);
}
int bubbleSortTrack(char* path, LNode** head, int count, List* lst)
{
        FILE* flog = fopen("logfile.txt", "a+");
        if (!flog)
        {
                printf("File not found!\n");
                return 0;
        }
        LNode** h;
        int i, j, swapped;
        char h1[6], h2[31], h3[31], h4[10], h5[10];
        for (i = 0; i <= count; i++) {

                h = head;
                swapped = 0;

                for (j = 0; j < count - i; j++) {

                        LNode* p1 = *h;
                        LNode* p2 = p1->next;
                        if (p2 == NULL)
                                continue;
                        if (_strcmpi(p1->songsindex.trackName, p2->songsindex.trackName) > 0) {

                                /* update the link after swapping */
                                *h = swap(p1, p2);
                                swapped = 1;
                        }

                        h = &(*h)->next;
                }

                /* break if the loop ended without any swap */
                if (swapped == 0)
                        break;
        }
        FILE* fp = fopen(path, "r");
        if (!fp)
        {
                printf("File not found!\n"); fprintf(flog, "File not found!\n");
                return 0;
        }
        fscanf(fp, "%5s%30s%30s%10s%30s", h1, h2, h3, h4, h5);
        fclose(fp);
        fp = fopen(path, "w+");
        if (!fp)
        {
                printf("File not found!\n");
                return 0;
        }
        fprintf(fp, "%-5s %-30s %-30s %-10s %-30s \n", h1, h2, h3, h4, h5); fprintf(fp, "%-5s %-30s %-30s %-10s %-30s \n", h1, h2, h3, h4, h5);
        printf("%-5s %-30s %-30s %-10s %-30s \n", h1, h2, h3, h4, h5);
        fclose(fp);
        for (i = 0; i <= count; i++)
        {
                fprintsong("playlist.txt", getLinkByIndex(&lst->head, i));
                playsong(getLinkByIndex(&lst->head, i));
        }
        fclose(flog);
}
void initList(List* lst)
{
        lst->head = lst->tail = NULL;
}
int deleteLast(List* lst)
{
        //if list is empty there is nothing to delete
        if (!isEmpty(lst))
        {
                //save reference to last link
                LNode* temp = lst->tail;

                //if only one link
                if (temp->prev == NULL)
                {
                        initList(lst);
                }
                else
                {
                        lst->tail->prev->next = NULL;
                        lst->tail = lst->tail->prev;
                }
                free(temp);
                return 1;
        }
        return 0;
}
void playsong(LNode* head)
{
        FILE* flog = fopen("logfile.txt", "a+");
        if (!flog)
        {
                printf("File not found!\n");
                return 0;
        }
        printf("%5d %30s %30s %10d %30s \n", head->songsindex.id, head->songsindex.artistName, head->songsindex.albumName, head->songsindex.trackNo, head->songsindex.trackName);
        fprintf(flog,"%5d %30s %30s %10d %30s \n", head->songsindex.id, head->songsindex.artistName, head->songsindex.albumName, head->songsindex.trackNo, head->songsindex.trackName);
        fclose(flog);
}
void fprintsong(char* path, LNode* head)
{
        FILE* flog = fopen("logfile.txt", "a+");
        if (!flog)
        {
                printf("File not found!\n");
                return 0;
        }
        FILE* fp = fopen(path, "a");
        if (!fp)
        {
                printf("File not found!\n"); fprintf(flog,"File not found!\n");
                return 0;
        }
        fprintf(fp, "%5d %30s %30s %10d %30s \n", head->songsindex.id, head->songsindex.artistName, head->songsindex.albumName, head->songsindex.trackNo, head->songsindex.trackName);
        fclose(fp,flog);
}
int moveforward(LNode* current, LNode* head, LNode* tail)
{
        if (current == tail)
                return head;
        else
                current = current->next;
        return current;
}
int movebackward(LNode* current, LNode* head, LNode* tail)
{
        if (current == head)
                return tail;
        else
                current = current->prev;
        return current;
}
void search(LNode* head, LNode* tail)
{
        FILE* flog = fopen("logfile.txt", "a+");
        if (!flog)
        {
                printf("File not found!\n");
                return 0;
        }
        char string[100];
        char aj[1000], mj[1000];
        char operator;
        int x = 0;
        int i, j, len;
        printf("search by artist name: press 1\n"); fprintf(flog,"search by artist name: press 1\n");
        printf("search by track name: press 2\n"); fprintf(flog,"search by track name: press 2\n");
        printf("search by album name: press 3\n"); fprintf(flog,"search by album name: press 3\n");
        scanf(" %c", &operator);
        do {
                switch (operator)
                {
                case '1':
                        printf("Enter artist name for search: "); fprintf(flog,"Enter artist name for search: ");
                        scanf("%s", &string);
                        while (head != NULL)
                        {
                                i = 0, j = 0;
                                strcpy(aj, head->songsindex.artistName);
                                len = strlen(aj);   // len stores the length of the input string
                                while (aj[i] != '\0')    // till string doesn't terminate
                                {
                                        if (aj[i] != ' ')  // if the char is not a white space
                                        {
                                                mj[j++] = aj[i];
                                        }

                                        i++;
                                }
                                mj[j] = '\0';
                                if (_strcmpi(mj, string) == 0)
                                        playsong(head);
                                head = head->next;
                        }
                        break;

                case '2':
                        printf("Enter track name for search: "); fprintf(flog,"Enter track name for search: ");
                        scanf("%s", &string);
                        while (head != NULL)
                        {
                                i = 0, j = 0;
                                strcpy(aj, head->songsindex.trackName);
                                len = strlen(aj);   // len stores the length of the input string
                                while (aj[i] != '\0')    // till string doesn't terminate
                                {
                                        if (aj[i] != ' ')  // if the char is not a white space
                                        {

                                                mj[j++] = aj[i];
                                        }

                                        i++;
                                }
                                mj[j] = '\0';
                                if (_strcmpi(mj, string) == 0)
                                        playsong(head);
                                head = head->next;
                        }
                        break;

                case '3':
                        printf("Enter album name for search: "); fprintf(flog,"Enter album name for search: ");
                        scanf("%s", &string);
                        while (head != NULL)
                        {
                                i = 0, j = 0;
                                strcpy(aj, head->songsindex.albumName);
                                len = strlen(aj);   // len stores the length of the input string
                                while (aj[i] != '\0')    // till string doesn't terminate
                                {
                                        if (aj[i] != ' ')  // if the char is not a white space
                                        {

                                                mj[j++] = aj[i];
                                        }

                                        i++;
                                }
                                mj[j] = '\0';
                                if (_strcmpi(mj, string) == 0)
                                        playsong(head);
                                head = head->next;
                        }
                default:
                        x = 0;
                        break;
                }
        } while (x != 0);
        fclose(flog);
}
int deletedata(char* path, songs_t* thePlaylist, LNode* head, LNode* tail, int count)
{
        FILE* flog = fopen("logfile.txt", "a+");
        if (!flog)
        {
                printf("File not found!\n");
                return 0;
        }
        printf("\n"); fprintf(flog,"\n");
        int id, trackNo, i = 0;
        char artistName[31], albumName[31], trackName[31];        //note +1 for '\0'
        char h1[6], h2[31], h3[31], h4[10], h5[10];
        FILE* fp = fopen(path, "r");
        if (!fp)
        {
                printf("File not found!\n"); fprintf(flog,"File not found!\n");
                return 0;
        }
        fscanf(fp, "%5s%30s%30s%10s%30s", h1, h2, h3, h4, h5);
        printf("%-5s %-30s %-30s %-10s %-30s \n", h1, h2, h3, h4, h5); fprintf(flog,"%-5s %-30s %-30s %-10s %-30s \n", h1, h2, h3, h4, h5);
        while (fscanf(fp, "%5d%30[^\n]%30[^\n]%5d%30[^\n]", &id, artistName, albumName, &trackNo, trackName) > 0)
        {
                if (i > count - 2)
                        break;
                head->songsindex.id = id;
                strcpy(head->songsindex.artistName, artistName);
                strcpy(head->songsindex.albumName, albumName);
                head->songsindex.trackNo = trackNo;
                strcpy(head->songsindex.trackName, trackName);
                printf("%5d %30s %30s %10d %30s \n", id, artistName, albumName, trackNo, trackName); fprintf(flog,"%5d %30s %30s %10d %30s \n", id, artistName, albumName, trackNo, trackName);
                addNode(head, 1);
                head = head->next;
                i++;
        }
        fclose(fp,flog);
        return i;
}
int adddata(char* path, songs_t* thePlaylist, LNode* head, LNode* tail, int count)
{
        FILE* flog = fopen("logfile.txt", "a+");
        if (!flog)
        {
                printf("File not found!\n");
                return 0;
        }
        printf("\n"); fprintf(flog,"\n");
        int id, trackNo, i = 0;
        char artistName[31], albumName[31], trackName[31];        //note +1 for '\0'
        char h1[6], h2[31], h3[31], h4[10], h5[10];
        FILE* fp = fopen(path, "a");
        if (!fp)
        {
                printf("File not found!\n"); fprintf(flog,"File not found!\n");
                return count;
        }
        printf("Enter track information\n"); fprintf(flog,"Enter track information\n");
        addNode(tail, count);
        head = tail->next;
        printf("enter track id: "); fprintf(flog,"enter track id: ");
        scanf("%d", &id);
        head->songsindex.id = id;

        printf("enter artist name: "); fprintf(flog,"enter artist name: ");
        scanf("%s", artistName);
        strcpy(head->songsindex.artistName, removespace(artistName));

        printf("enter album name: ");
        scanf("%s", albumName);
        strcpy(head->songsindex.albumName, removespace(albumName));

        printf("enter track number: "); fprintf(flog,"enter track number: ");
        scanf("%5d", &trackNo);
        head->songsindex.trackNo = trackNo;

        printf("enter track name: "); fprintf(flog,"enter track name: ");
        scanf("%s", trackName);
        strcpy(head->songsindex.trackName, removespace(trackName));

        fprintf(fp, "%-5d %-30s %-30s%-10d%-30s\n", id, artistName, albumName, trackNo, trackName);
        head = head->next;
        count = count + 1;
        fclose(fp,flog);
        return count;
}
int importData(char* path, songs_t* thePlaylist, LNode* head, LNode* tail, List* lst)
{
        FILE* flog = fopen("logfile.txt", "a+");
        if (!flog)
        {
                printf("File not found!\n");
                return 0;
        }
        int id, trackNo, i = -1, k = 0;
        char artistName[31], albumName[31], trackName[31];        //note +1 for '\0'
        char h1[6], h2[31], h3[31], h4[10], h5[10];
        FILE* fp = fopen(path, "r");
        if (!fp)
        {
                printf("File not found!\n");
                return 0;
        }
        fscanf(fp, "%5s%30s%30s%10s%30s", h1, h2, h3, h4, h5);
        printf("%-5s %-30s %-30s %-10s %-30s \n", h1, h2, h3, h4, h5); fprintf(flog,"%-5s %-30s %-30s %-10s %-30s \n", h1, h2, h3, h4, h5);
        while (fscanf(fp, "%5d%30[^\n]%30[^\n]%5d%30[^\n]", &id, artistName, albumName, &trackNo, trackName) > 0)
        {

                head->songsindex.id = id;
                strcpy(head->songsindex.artistName, removespace(artistName));
                strcpy(head->songsindex.albumName, removespace(albumName));
                head->songsindex.trackNo = trackNo;
                strcpy(head->songsindex.trackName, removespace(trackName));
                printf("%5d %30s %30s %10d %30s \n", id, artistName, albumName, trackNo, trackName); fprintf(flog,"%5d %30s %30s %10d %30s \n", id, artistName, albumName, trackNo, trackName);
                i++;
                addNode(head, i + 1);
                head = head->next;
        }
        lst->tail = head;
        k = deleteLast(lst);
        fclose(fp,flog);
        return (i + 1);
}
char* removespace(char txt[])
{
        char aj[100], mj[100], temp[1001], temp2[100];
        int n, m, len;
        n = 0, m = 0;
        strcpy(aj, txt);
        len = strlen(aj);   // len stores the length of the input string
        bool raitiot = false;
        while (aj[n] != '\0')    // till string doesn't terminate
        {
                if (aj[n] != ' ') {
                        raitiot = true;
                }
                if (!raitiot) {
                        if (aj[n] != ' ')  // if the char is not a white space
                        {
                                mj[m++] = aj[n];
                        }
                }
                else {
                        mj[m++] = aj[n];
                }
                n++;
        }
        mj[m] = '\0';
        strcpy(temp, mj);
        return temp;
}
LNode* createNode()
{
        LNode* temp; // declare a node
        temp = (LNode*)malloc(sizeof(LNode)); // allocate memory using malloc()
        temp->next = NULL;// make next point to NULL
        return temp;//return the new node
}
LNode* removeNode()
{
        LNode* temp; // declare a node
        temp = (LNode*)malloc(sizeof(LNode)); // allocate memory using malloc()
        temp->prev = NULL;// make next point to NULL
        return temp;//return the new node
}
LNode* addNode(LNode* head, int value)
{
        LNode* temp, * p;// declare two nodes temp and p
        temp = createNode();//createNode will return a new node with data = value and next pointing to NULL.
        temp->data = value; // add element's value to data part of node
        if (head == NULL)
        {
                head = temp; //when linked list is empty
        }
        else
        {
                p = head;//assign head to p
                while (p->next != NULL)
                {
                        p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
                }
                temp->prev = p;
                p->next = temp;//Point the previous last node to the new node created.

        }
        return head;
}
void removeAll(List* lst, type toremove)
{
        LNode* current = lst->head, * next;
        if (current == NULL)
                return;
        while (current->data == toremove)
        {
                lst->head = current->next;
                if (lst->head == NULL)
                        lst->tail = NULL;
                free(current);
                if (lst->head == NULL)
                        return;
                else
                        current = lst->head;
        }
        current = lst->head;
        next = current->next;
        while (next)
        {
                if (next->data == toremove)
                {
                        current->next = next->next;
                        free(next);
                        if (current->next == NULL)
                        {
                                lst->tail = current;
                                next = NULL;
                        }
                        else
                        {
                                next = current->next;
                        }
                }
                else
                {
                        current = next;
                        next = current->next;
                }
        }
}
int insertLast(List* lst, LNode* item)
{
        if (item != NULL)
        {
                //reset item pointers
                item->next = item->prev = NULL;

                if (isEmpty(lst))
                {
                        //make head & tail pointing to item
                        lst->head = lst->tail = item;
                }
                else
                {
                        //update last next link
                        lst->tail->next = item;
                        //point item to old last link
                        item->prev = lst->tail;
                        //point last to new last node
                        lst->tail = item;
                }
                return 1;
        }
        return 0;
}
int insertAfter(List* lst, LNode* item, LNode* link)
{
        if (!isEmpty(lst))
        {
                if ((item == NULL) || (link == NULL))
                        return 0;
                if (link == lst->tail)
                        return insertLast(lst, item);

                //assign new item pointers
                item->prev = link;
                item->next = link->next;
                //chain link and link->next to new item
                link->next->prev = item;
                link->next = item;
                return 1;
        }
        return 0;
}
int insertBefore(List* lst, LNode* item, LNode* link)
{
        if (!isEmpty(lst))
        {
                if ((item == NULL) || (link == NULL))
                        return 0;
                if (link == lst->head)
                        return insertFirst(lst, item);

                //assign new item pointers
                item->next = link;
                item->prev = link->prev;
                //chain link and link->prev to new item
                link->prev->next = item;
                link->prev = item;
                return 1;
        }
        return 0;
}
int insertFirst(List* lst, LNode* item)
{
        if (item != NULL)
        {
                //reset item pointers
                item->next = item->prev = NULL;

                if (isEmpty(lst))
                {
                        //make head & tail pointing to item
                        lst->head = lst->tail = item;
                }
                else
                {
                        //update first prev link
                        lst->head->prev = item;
                        //point item to old first link
                        item->next = lst->head;
                        //point head to new first node
                        lst->head = item;
                }
                return 1;
        }
        return 0;
}
LNode* getLinkByIndex(List* lst, int idx)
{
        int i;
        //start from the first link
        LNode* link = lst->head;

        //navigate through list
        for (i = 0; i < idx && link != NULL; i++)
                link = link->next;

        return link;
}
List copyList(const List* lst)
{
        List res = makeEmptyList();
        LNode* temp = lst->head;
        while (temp)
        {
                insertValueToTail(&res, temp->data);
                temp = temp->next;
        }
        return res;
}
List makeEmptyList()
{
        List lst;
        lst.head = lst.tail = NULL;
        return lst;
}
LNode* createNewNode(type newData, LNode* next)
{
        LNode* newNode = (LNode*)calloc(1, sizeof(LNode));
        newNode->data = newData;
        newNode->next = next;
        return newNode;
}
LNode* findData(List lst, type data)
{
        LNode* temp = lst.head;
        while (temp)
        {
                if (temp->data == data)
                        return temp;
                temp = temp->next;
        }
        return NULL;
}
int getlistLength(const List* lst)
{
        int count = 0;
        LNode* temp = lst->head;
        while (temp != NULL)
        {
                count++;
                temp = temp->next;
        }
        return count;
}
void freeList(List* thelist)
{
        LNode* current = thelist->head;
        LNode* next;
        while (current)
        {
                next = current->next;
                free(current);
                current = next;
        }
        thelist->head = NULL;
}
void printCharList(const List* lst)
{

        LNode* temp = lst->head;
        while (temp != NULL)
        {
                printf("%c", temp->data);
                temp = temp->next;
        }
}
int isEmpty(const List* lst)
{
        return lst->head == NULL;
}
void insertValueToHead(List* lst, type newData)
{
        LNode* newNode = createNewNode(newData, lst->head);
        if (isEmpty(lst))
                lst->head = lst->tail = newNode;
        else
                lst->head = newNode;
}
void insertValueToTail(List* lst, type newData)
{
        LNode* newNode = createNewNode(newData, NULL);
        if (isEmpty(lst))
                lst->head = lst->tail = newNode;
        else
        {
                lst->tail->next = newNode;
                lst->tail = newNode;
        }
}
void insertNodeToTail(List* lst, LNode* newNode)
{
        if (isEmpty(lst))
                lst->head = lst->tail = newNode;
        else
        {
                lst->tail->next = newNode;
                lst->tail = newNode;
        }
}
void insertAfterValue(List* lst, type insertAfter, type newValue)
{
        LNode* temp = lst->head;
        temp = findData(*lst, insertAfter);
        if (temp)
        {
                LNode* newNode = createNewNode(newValue, temp->next);
                temp->next = newNode;
                if (newNode->next == NULL)
                        lst->tail = newNode;
        }
}
void removedata(List* lst, int toremove, LNode* head)
{
        LNode* current, * next;
        current = lst->head;
        if (current == NULL)
                return;
        if (current->data == toremove)
        {
                lst->head = current->next;
                if (lst->head = NULL)
                        lst->tail = NULL;
                free(current);
                return;
        }
        next = current->next;
        while (next)
        {
                if (next->data == toremove)
                {
                        current->next = next->next;
                        if (current->next == NULL)
                                lst->tail = current;
                        free(next);
                }
                else
                {
                        current = next;
                        next = current->next;
                }
        }
        

    # Deploy the Docker image to the IKS cluster
    - name: #define _CRT_SECURE_NO_DEPRECATE
/*
Assigned by:

*/
// C program for array implementation of queue 
#include <limits.h> 
#include <stdio.h> 
#include <stdlib.h> 

// A structure to represent a queue 
struct Queue {
        int front, rear, size;
        unsigned capacity;
        int* array;
};

// function to create a queue 
// of given capacity. 
// It initializes size of queue as 0 
struct Queue* createQueue(unsigned capacity)
{
        struct Queue* queue = (struct Queue*)malloc(
                sizeof(struct Queue));
        queue->capacity = capacity;
        queue->front = queue->size = 0;

        // This is important, see the enqueue 
        queue->rear = capacity - 1;
        queue->array = (int*)malloc(
                queue->capacity * sizeof(int));
        return queue;
}

// Queue is full when size becomes 
// equal to the capacity 
int isFull(struct Queue* queue)
{
        return (queue->size == queue->capacity);
}

// Queue is empty when size is 0 
int isEmpty(struct Queue* queue)
{
        return (queue->size == 0);
}

// Function to add an item to the queue. 
// It changes rear and size 
void enqueue(struct Queue* queue, int item)
{
        if (isFull(queue))
                return;
        queue->rear = (queue->rear + 1)
                % queue->capacity;
        queue->array[queue->rear] = item;
        queue->size = queue->size + 1;
        printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue. 
// It changes front and size 
int dequeue(struct Queue* queue)
{
        if (isEmpty(queue))
                return INT_MIN;
        int item = queue->array[queue->front];
        queue->front = (queue->front + 1)
                % queue->capacity;
        queue->size = queue->size - 1;
        return item;
}

// Function to get front of queue 
int front(struct Queue* queue)
{
        if (isEmpty(queue))
                return INT_MIN;
        return queue->array[queue->front];
}

// Function to get rear of queue 
int rear(struct Queue* queue)
{
        if (isEmpty(queue))
                return INT_MIN;
        return queue->array[queue->rear];
}

// Driver program to test above functions./ 
int main()
{
        struct Queue* queue1 = createQueue(10);
        struct Queue* queue2 = createQueue(10);
        int temp, i;
        char opertaor = 'a';
        enqueue(queue1, 3);
        enqueue(queue1, 7);
        enqueue(queue1, 13);
        enqueue(queue1, 20);
        enqueue(queue1, 47);
        enqueue(queue1, 70);
        enqueue(queue2, 12);
        enqueue(queue2, 20);
        enqueue(queue2, 65);
        enqueue(queue2, 70);
        enqueue(queue2, 75);
        printf("Press 1 to Q1\nPress 2 to Q2");
        scanf_s(" %c", &opertaor);
        switch (opertaor)
        {
        case '1':
                while (isEmpty(queue1) != 1 || isEmpty(queue2) != 1)
                {
                        if (front(queue1) < front(queue2))
                                dequeue(queue1);
                        if (front(queue1) == front(queue2))
                        {
                                printf("\nThe lower number that equal between 2 queues is = %d\n", front(queue1));
                                break;
                        }
                        if (front(queue2) < front(queue1))
                                dequeue(queue2);
                }
                        break;
        case '2':
                printf("start sort\n");
                i = queue2->size;
                temp = front(queue1);
                while (isEmpty(queue2) != 1 || i == 1)
                {
                        i = queue2->size;
                        if (front(queue1) < front(queue2)) {
                                enqueue(queue1, front(queue1));
                                dequeue(queue1);
                        }

                        if (front(queue1) == front(queue2))
                        {
                                enqueue(queue1, front(queue2));
                                dequeue(queue1);
                                dequeue(queue2);
                        }
                        if (front(queue2) < front(queue1)) {
                                enqueue(queue1, front(queue2));
                                dequeue(queue2);
                        }
                        if (temp == front(queue1))
                                if (i == 1)
                                {
                                        enqueue(queue1, front(queue2));
                                        dequeue(queue2);
                                        break;
                                }
                }
                printf("\n\nAfter sort\n\n");
                temp = front(queue1);
                do {
                        enqueue(queue1, front(queue1));
                        dequeue(queue1);
                } while (temp != front(queue1));
                break;

        default:
                break;
                }
                return 0;
        } }" -g default
      run: #define _CRT_SECURE_NO_DEPRECATE
/*
Assigned by:

*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define max_student 30
struct student
{
        char* name;
        int grade;
}typedef student_t;
struct class
{
        student_t classtudents;
        int  numofstudent;
        student_t* students[max_student];
        int avargeclass;
}typedef class_t;

void createclass(class_t* theclass);
void createstudent(student_t* theclass);
void freeclass(class_t* theclass);
void printavarge(class_t* theclass);
void main()
{
        class_t newclass;
        createclass(&newclass);
        printavarge(&newclass);
        freeclass(&newclass);

}
void createclass(class_t* theclass)
{
        int i,count=0;
        printf("how many students?");
        scanf("%d", &theclass->numofstudent);
        for (i = 0; i < theclass->numofstudent; i++)
        {
                theclass->students[i] = (student_t*)malloc(sizeof(student_t));
                printf("enter information about student #&d: \n", (i + 1));
                createstudent(theclass->students[i]);
        }

}
void createstudent(student_t* thestudent)
{
        char temp[20];
        printf("enter full name\n");
        scanf("%s",&thestudent->name);
        printf("enter avarge grade\n");
        scanf("%d", &thestudent->grade);
}
void freeclass(class_t* theclass)
{
        int i;
        for (i = 0; i < theclass->numofstudent; i++)
                free(theclass->students[i]);
}
void printavarge(class_t* theclass)
{
        int i,count=0;
        for (i = 0; i < theclass->numofstudent; i++)
        {
                count += theclass->students[i]->grade;
        }
theclass->avargeclass = count / theclass->numofstudent;
        printf("the avarge is %d", theclass->avargeclass);
} }"
        ibmcloud cr login
