This repository contains the code C++ examples for the 
DDS Tutorial.

The examples included should compile and run with any DDS
implementation that supports the new ISO C++ API.

To make to code more compact and clear I take deliberately
advantage of some of the features introduced by C++11. 
Considered the widely spread availability of C++11 compilers
this should not pose a problem.

That said I've only tested with OpenSplice DDS.

To try the examples you need to have:

  - OpenSplice DDS (http://opensplice.org | http://prismtech.com)
  - g++ or clang with support for C+0x
  - cmake 2.8 or higher

```bash
$> sudo apt install automake1.11 libtool autoconf liblog4cpp5-dev
```

# How to compile code ?

```bash
$> cmake -Bbuild -H.
$> cmake --build build
```

# Note for examples
* Coherent: Please check the [readme](coherent/README.md) within [coherent example](coherent) for more information.
