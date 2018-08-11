#!/bin/sh
rm ./*.cpp ./*.lis
proc -lclntsh parse=no  THREADS=YES CPOOL=YES CMAX=1000 CMIN=500 CINCR=2 code=cpp cpp_suffix=cpp iname=DBOperation.pc 
#proc -lclntsh parse=no  code=cpp cpp_suffix=cpp iname=DBOperation.pc 