### Update

除了仙剑之外的PA4部分。

PA4中解决了PA3中遗留的cache的bug，该问题在PA3中不会暴露（应该）。

master分支中未合并PA4。

# NEMU2020 Programming Assignment

This project is the programming assignment of the class ICS(Introduction to Computer System) in College of Intelligence and Computing, Tianjin Univerisity.

This project is introduced from Nanjin University in 2016. Thank you for  Prof. Chunfeng Yuan of NJU and Dr. Zihao Yu of ICT.

The following subprojects/components are included. Some of them are not fully implemented.
* NEMU
* testcase
* uClibc
* kernel
* typing game
* NEMU-PAL

## NEMU

NEMU(NJU Emulator) is a simple but complete full-system x86 emulator designed for teaching. It is the main part of this programming assignment. Small x86 programs can run under NEMU. The main features of NEMU include
* a small monitor with a simple debugger
 * single step
 * register/memory examination
 * expression evaluation with the support of symbols
 * watch point
 * backtrace
* CPU core with support of most common used x86 instructions in protected mode
 * real mode is not supported
 * x87 floating point instructions are not supported
* DRAM with row buffer and burst
* two-level unified cache
* IA-32 segmentation and paging with TLB
 * protection is not supported
* IA-32 interrupt and exception
 * protection is not supported
* 6 devices
 * timer, keyboard, VGA, serial, IDE, i8259 PIC
 * most of them are simplified and unprogrammable
* 2 types of I/O
 * port-mapped I/O and memory-mapped I/O

## testcase

Some small C programs to test the implementation of NEMU.

## uClibc

uClibc(https://www.uclibc.org/) is a C library for embedding systems. It requires much fewer run-time support than glibc and is very friendly to NEMU.

## kernel

This is the simplified version of Nanos(http://cslab.nju.edu.cn/opsystem). It is a uni-tasking kernel with the following features.
* 2 device drivers
 * Ramdisk
 * IDE
* ELF32 loader
* memory management with paging
* a simple file system
 * with fix number and size of files
 * without directory
* 6 system calls
 * open, read, write, lseek, close, brk

## typing game

This is a fork of the demo of NJU 2013 oslab0(the origin repository has been deleted, but we have a fork of it -- https://github.com/nju-ics/os-lab0). It is ported to NEMU.

## NEMU-PAL

This is a fork of Wei Mingzhi's SDLPAL(https://github.com/CecilHarvey/sdlpal). It is obtained by refactoring the original SDLPAL, as well as porting to NEMU.
