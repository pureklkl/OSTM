# OSTM
A Simple Obstruction Free Software Transaction System.

An obstruction Free STM was implemented in C++ with the improvement of the reader list and prediction based backoff contention manager. 

The reader list enables multiple transaction read the same object to improve the concurrency throughput. 

The prediction based back off contention manager estimated the transaction execution time to avoid meaningless waiting for long transaction. 


