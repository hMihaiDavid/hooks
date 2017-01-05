# hooks
A DLL that performs IAT hooking

Once the DLL is loadded into any process address space, the main executable that created that process is going to have
its Import Address Table patched, in order to perform the hooks specified in the sourcecode of the DLL.
This repository provides a template project to intercept function calls.

The actual IAT hooking code is already written in this source code, you only need to write your hook functions and register them.

An example of how to hook a function can be seen in Hooks.cpp (in particular ExitProcess)
