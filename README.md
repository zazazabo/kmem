# kmem

/proc/kmem reimplementation for windows. The process created is named "/proc/kmem" which you can see in task manager + use CreateToolhelp32Snapshot to obtain the PID of this process. 
On windows I was told it was "impossible to name your process something other then the executables name" so I figured I would go the extra mile and throw in some forward slashes into the name
since you dont see that very often on windows.

# How

I copy the kernel pml4e's into the usermode part of the pml4. Windows has an amazing infrastructure/API that can be used to do much more then its been allowed. Lets take a look
at ReadProcessMemory. ReadProcessMemory calls NtReadVirtualMemory which then calls MmCopyVirtualMemory. NtReadVirtualMemory does validation checks on addresses supplied from
usermode ensuring that they are indeed usermode addresses and not kernel addresses. They dont check paging tables for this they just have an if statement. This means if you 
copy the top 256 pml4e's into the bottom (usermode) part of the pml4, NtReadVirtualMemory will work on this memory although it is indeed kernel memory still.

```cpp
  if ( v11 )
  {
    if ( &a2[a4] < a2 || (unsigned __int64)&a2[a4] > 0x7FFFFFFF0000i64 || a3 + a4 < a3 || a3 + a4 > 0x7FFFFFFF0000i64 )
      return 0xC0000005i64;
    v12 = (_QWORD *)a5;
    if ( a5 )
    {
      v13 = (_QWORD *)a5;
      if ( a5 >= 0x7FFFFFFF0000i64 )
        v13 = (_QWORD *)0x7FFFFFFF0000i64;
      *v13 = *v13;
    }
  }
  else
  {
    v12 = (_QWORD *)a5;
  }
```