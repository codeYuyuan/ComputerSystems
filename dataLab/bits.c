/*
 * CS:APP Data Lab
 *
 * <Please put your name and userid here>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code
  must conform to the following style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>

  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.


  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function.
     The max operator count is checked by dlc. Note that '=' is not
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 *
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce
 *      the correct answers.
 */


#endif
/*
 * bitAnd - x&y using only ~ and |
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
  return ~(~x | ~y);
}
/*
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {

  int needle = 0xFF;
  int offset = n << 3;
  int index = x >> offset ;
  int res = index & needle;
  return res;

}
/*
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int logicalShift(int x, int n) {
  int nonSignPart = x & 0x7FFFFFFF;
  int flag = ((0 ^ x) >> 31) & 1;
  int signShift = flag << (32 + ~n) ;
  int nonSignShift = nonSignPart >> n;
  return signShift + nonSignShift;
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
*   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    int mask = 0x11 + (0x11 << 8) + (0x11 << 16) + (0x11 << 24);
    int seg8Count = (x & mask) + ((x >> 1) & mask) + ((x >> 2) & mask) + ((x >> 3) & mask);
    int seg4Count1 = seg8Count + (seg8Count >> 8);
    int seg4Count2 = (seg8Count >> 16) + (seg8Count >> 24);
    int result = (seg4Count1 & 0x0F) + ((seg4Count1 >> 4) & 0x0F) + (seg4Count2 & 0x0F) + ((seg4Count2 >> 4) & 0x0F);
    return result;

}
/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int bang(int x) {
    // ~x + 1 = -x
    int inverse = ~x + 1;
    int y = inverse | x ;
    y = ~y;
    y = y >> 31;
    return y & 1;
}
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return ~0x7FFFFFFF;
}
/*
 * fitsBits - return 1 if x can be represented as an
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
    int shift  = x >> (n-1);
    return !(shift ^ (x>>31));
}
/*
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
    int flag = x & 0x80000000;
    int least = x << (33 + ~ n);
    flag = (flag >>31) & 1;
    int rawres = x >> n;
    int offset = ( (flag & (!!n)) ) & !!least;
    return rawres + offset;
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
*   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
    return ~x + 1;
}
/*
 * isPositive - return 1 if x > 0, return 0 otherwise
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
    int flag = (x + ~1 + 1) & 0x8FFFFFFF;
    flag = flag >>31;
    return !flag;
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    int res = y + ~x + 1;
    int flag = res & 0x8FFFFFFF;
    flag = flag >> 31;
    int flagX =( x & 0x8FFFFFFF) >> 31;
    int flagY = (y & 0x8FFFFFFF) >> 31;
    int except1 = (flagX&1) & ((!flagY)&1);
    int except2 = (!flagX & 1) & (flagY & 1);
    return (!flag | except1) & (!except2);
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
  int i, j, k, l, m;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);

    // i = 0x55555555 
    i = 0x55 | (0x55 << 8); 
    i = i | (i << 16);

    // j = 0x33333333 
    j = 0x33 | (0x33 << 8);
    j = j | (j << 16);

    // k = 0x0f0f0f0f 
    k = 0x0f | (0x0f << 8);
    k = k | (k << 16);

    // l = 0x00ff00ff 
    l = 0xff | (0xff << 16);

    // m = 0x0000ffff 
    m = 0xff | (0xff << 8);

    x = (x & i) + ((x >> 1) & i);
    x = (x & j) + ((x >> 2) & j);
    x = (x & k) + ((x >> 4) & k);
    x = (x & l) + ((x >> 8) & l);
    x = (x & m) + ((x >> 16) & m);
    x = x + ~0;
    return x; 
}
/*
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
    unsigned res = uf ^ 0x80000000;
    if ((uf & 0x7FFFFFFF) > 0x7F800000)
        return uf;
    return res;
}
/*
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
    int sign=x>>31&1;
    int i;
    int exponent; 
    int fraction; 
    int delta;
    int fraction_mask;
    if(x==0)//||x==0x8000000)
        return x;
    else if(x==0x80000000)
        exponent=158;
    else{
        if (sign)//turn negtive to positive
            x = -x;
        i = 30;
        while ( !(x >> i) )//see how many bits do x have(rightshift until 0) 
            i--;
        //printf("%x %d\n",x,i);
        exponent = i + 127;
        x = x << (31 - i);//clean all those zeroes of high bits
        fraction_mask = 0x7fffff;//(1 << 23) - 1;
        fraction = fraction_mask & (x >> 8);//right shift 8 bits to become the fraction,sign and exp have 8 bits total
        x = x & 0xff;
        delta = x > 128 || ((x == 128) && (fraction & 1));//if lowest 8 bits of x is larger than a half,or is 1.5,round up 1
        fraction += delta;
        if(fraction >> 23) {//if after rounding fraction is larger than 23bits
            fraction &= fraction_mask;
            exponent += 1;
        }
    }
    return (sign<<31)|(exponent<<23)|fraction;
}
/*
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
*   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
  if(uf==0 || uf == 0x80000000) return uf;
  //NaN case.
  if(((uf>>23) & 0xff) == 0xff) return uf;
  //Tiny value, but non-zero case.
  if(((uf>>23) & 0xff) == 0x00) {
    return (uf & (1<<31)) | (uf<<1);
  }
  //Otherwise, Add 1 to exp value.
  return uf + (1<<23);
}
