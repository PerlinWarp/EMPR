#include "NewMalloc.h"





UsedMemory Used[END];//max number of malloc instructions

void* NewMalloc(long size)
{
  void* newAddress;
  int i =0;
  if(Used.Address[0] == 0)
  {
    newAddress = LPC_AHBRAM1_BASE;
    break;
  }
  else
  {
    for(i =1; i<END;i++)
    {
      if(Used.Address[i] - Used.EndAddress[i-1] > size)
      {
        newAddress = Used.EndAddress[i-1];
        break;
      }
    }
  }

  for (i =0; i<END;i++)
  {
    if(Used.Address[i] == 0x00)
    {
      Used.Address[i] = newAddress;
      Used.EndAddress[i] = newAddress + size;
      insertionSort();
      return newAddress;
    }
  }
  return 0x00;
}

void insertionSort()
{
   int i, key, j,v,c;
   for (i = 1; i < END; i++)
   {
       key = Used.Address[i];
       if(key != 0)
       {
         j = i-1;

         /* Move elements of arr[0..i-1], that are
            greater than key, to one position ahead
            of their current position unless the value is */
         v = 1;
         c=0;
         while (j >= 0 && (Used.Address[j] > key || Used.Address[j] == 0))
         {
           if(Used.Address[j+v] == 0)v++;
           else
           {
             c++;
             Used.Address[j+v] = Used.Address[j];
             Used.EndAddress[j+v] = Used.EndAddress[j];
             j = j-v;
           }
         }
         Used.Address[j+v] = key;
         for(j=0;j<c;j++) //shift everything back to remove zeros
         {
           Used.Address[j] = Used.Address[j+v];
           Used.EndAddress[j] = Used.EndAddress[j+v];
         }
       }
   }
   for(i=0;i<END;i++)
   {
     if(Used.Address[i] != 0)break;
   }
}


void NewFree(void* toRemove)
{
  int i;
  for(i =0;i< END;i++)
  {
    if(Used.Address[i] == toRemove)
    {
      Used.Address[i] == 0x00;//Ghost it
      Used.EndAddress[i] = 0x00;
    }
  }
}

void initMalloc()
{
  int i;
  for(i =0;i<END;i++)
  {
    Used.Address = 0;
    Used.EndAddress = 0;
  }
}
