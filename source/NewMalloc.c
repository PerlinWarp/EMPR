#include "NewMalloc.h"





UsedMemory Used[END];//max number of malloc instructions
int length =0;

void* assignAddress(void* address,unsigned long size)
{
  Used[length].Address = address;
  Used[length].EndAddress = address + size;//address of free memory or next NewMalloc[tm]
  length ++;
  insertionSort();
  return address;
}


void* NewMalloc(unsigned long size)
{
  int i =0;
  if(length == 0)return assignAddress(MALLOC_BASE,size); //if no addresses are set yet
  else if(size <= Used[0].Address - MALLOC_BASE)return assignAddress(MALLOC_BASE,size);//if there's space before the first address
  else
  {
    for(i =1; i<length;i++)
    {
      if(Used[i].Address - Used[i-1].EndAddress >= size)
      {
        if(length<END)
        {
          return assignAddress(Used[i-1].EndAddress,size);
        }
        else
        {
          return 0;
        }
      }
    }
    if(Used[length-1].EndAddress + size < MALLOC_END && length<END)
    {
      return assignAddress(Used[length-1].EndAddress,size);
    }
  }
  return 0x00;
}

void* NewCalloc(unsigned long Count,unsigned long size)
{
  void* addr = NewMalloc(Count*size);
  memset((char*)addr,0,Count*size);
  return addr;
}

void insertionSort()
{
   int i, j;
   void* key;
   for (i = 1; i < length; i++)
   {
       key = Used[i].Address;
       if(key != 0)
       {
         j = i-1;

         /* Move elements of arr[0..i-1], that are
            greater than key, to one position ahead
            of their current position unless the value is */
         while (j >= 0 && Used[j].Address > key)
         {
             Used[j+1] = Used[j];
             j = j-1;
         }
         Used[j+1].Address = key;
       }
   }
}


void NewFree(void* toRemove)
{
  int i;
  for(i =0;i<length;i++)
  {
    if(Used[i].Address == toRemove)
    {
      for(;i<length;++i)//shift everything back
      {
        Used[i-1] = Used[i];
      }
      length--;
      return;
    }
  }
}

void initMalloc()
{
  int i;
  for(i =0;i<END;i++)
  {
    Used[i].Address = 0;
    Used[i].EndAddress = 0;
  }
}
