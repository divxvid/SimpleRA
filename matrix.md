# Block Matrix Transpose

We have developed a pretty simple yet efficient way of transposing a huge
matrix file with minimal amount of computation and block accesses.

There are two phases involved for this operation:

- Matrix Creation
- Block Transpose

## Matrix Creation:

In the matrix creation phase, we first simply read the csv file supplied to us and
divide it into several blocks.

Imagine a 9x9 Sudoku puzzle and it is divided into 9 3x3 blocks. Just recreating
that concept, we divide our large matrix into blocks of 25*25 values.

Since it was mentioned that we need to limit our block size to 8kB, we decided
to fix the block size to 25. This choice of block size is explained as follows:

While writing numbers as characters to a file it takes 1 byte/character and we
were explicitly told that our data will consist of Integer type data which in C++
takes 4 bytes which when converting into bits is 32 bits. So the values that fit in
Integer type data ranges from 0 to 2<sup>32</sup> - 1. 2<sup>32</sup>- 1 is 4294967295 which is a 10 -
digit number, but when written as a character it takes about 10 bytes. The
consecutive row values also need to be separated by a comma which itself is
another character to a file meaning another byte for one number. So, all in all,
in worst case it takes about 11 bytes per element of a matrix when stored as a
character file. Dividing 8kB by 11 bytes => ceil( 8192/11)= 745, it isn’t a perfect
square so the closest perfect square <=745 is 729 which is the square of 27,we
could’ve chosen 27,26 or any number like that but for aesthetic purposes we
went with 25, so each of our matrix blocks can contain atmost 625 numbers
which when converted to bytes in worst case 625*11 will be 6.71kB.So our
block size is basically 6.71kB.

So we divide the large csv file to blocks of size 25*25 and write each block into
files named as Block0,Block1...and so on.


## Block Transpose:

Now that we have our matrix split into blocks, we need to transpose them.

The procedure we follow to achieve this is explained below:

We read each block file Block_i into main memory and transpose them
individually in place and write them back into the same file.

Now we know if in a matrix M, with number of elements per row is x, then if
we store the matrix in 1-D format M[i][j] becomes M[ (i)*x + j] and while
transposing we swap M[i][j] with M[j][i] i.e. M[ (j)*x +i] keeping the constraint
that j > i(Upper triangular matrix is swapped with Lower triangular matrix), the
exact same principal is followed in this block storage format. The number of
blocks per row is ceil(Number of elements per row / Block size ) which will be
our “x” here. The matrix of elements is now logically viewed by the system as a
matrix of blocks. And each consecutive block k is numbered and stored
sequentially. Now likewise if we apply the same 1-D access pattern to access a
Block[i][j] , we can find the file number we need to use as follows:

Block_file_number = Block[ceil(Number of elements per row / Block size )*i+j].

Similary, file_number for Block[j][i] can also be calculated.

Now since the block files have internally been transposed in the first part, we
**ONLY** need to swap the file names corresponding to Block[i][j] and Block[j][i].

That’s it !! We literally never access and open the file again, we only rename it,
which when seen from an operating system POV is just access to the File Table
and not memory disk blocks.

So the total number of block accesses required is :

If the matrix has been divided into B blocks then,

1. B accesses while creating blocks.
2. B accesses while transposing individual blocks.
3. 0 access in renaming the blocks.

So, a total of 2*B block accesses are required for creating, storing and
transposing the large matrix in block transpose format.


