# hooks
A DLL that performs IAT hooking

Once the DLL is loadded into any process address space, the main executable that created that process is going to have
its Import Address Table patched, in order to perform the hooks specified in the sourcecode of the DLL.
This repository provides a template project to intercept function calls.

The actual IAT hooking code is already written in this source code, you only need to write your hook functions and register them.
An example of how to hook/unhook a function can be seen in Hooks.cpp (in particular, ExitProcess)

This code only works on 32bit processes.

It is not intended to be production-quality code. Out there there are many flexible toolkits for manipulating both binaries on disk([LIEF](https://github.com/lief-project)) or runing process ([Frida](https://www.frida.re)) and perform hooks among other things. The prurpose of this code was to experiment with the PE file format and to learn how PE Executables handle imported symbols. I hope you find reading this code along with the marvelous [Iczelion's PE Imports tutorial](http://www.programminghorizon.com/win32assembly/pe-tut6.html) useful :) Happy hacking!
