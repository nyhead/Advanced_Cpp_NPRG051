**Interval Locker:**
Motivation
We are interested in implementing a locking mechanism on a general set of intervals 
(theoretically as big as allowed by the runtime environment).

A typical use case might be locking contiguous subsets of an array in a parallel application 
to avoid possible data races between the application's threads' accesses to the array. 
More specifically, in this use case, we would like to lock a certain contiguous sub-array with a 
shared lock for read-only access rights or with an exclusive lock for read-write access rights 
to the sub-array.

In practice, if two threads concurrently request exclusive access to two overlapping intervals 
(or one of them requests shared access), 
one will be blocked until the lock representing the access rights of the other is unlocked 
(either explicitly, or by the destruction of the associated lock).

Furthermore, we want to upgrade or downgrade the lock if we want to change our access rights. 
We also expect the locks to implement move semantics to allow transferring ownership
onto a different lock (such that the original loses the ownership; 
this also happens in the case of upgrades and downgrades).

**Cache with smart pointers:**
Your task is to implement a transparent cache with smart pointers for read-only data stored on 
external persistent storage.

A potentially large graph-oriented database is stored on external storage (e.g. in a file, db, etc.). 
Each node (vertex) contains data of one user-defined type (e.g., 32-bit integer or structure) 
and fixed (user-defined) number of oriented edges (internal persistent references) 
to other nodes in the same data collection (file, ...). 
The type of the references is implementation-defined by the repository, it can be, 
for example, an offset or any other type. 
The repository provides a function to retrieve the data of a single node (including associated edges) 
identified by an internal reference.

The client code accesses the above data via smart pointers, 
which transparently make available the data loaded into the cache. 
The semantics is similar to unique_ptr, i.e., 
copying smart pointers is prohibited, they can be moved. 
The cache ensures the presence of the loaded data in memory 
for the entire lifetime of the smart pointer referencing the node. 
Smart pointers have a method to get additional smart pointers obtained 
from the referenced node's persistent links.
A policy is used for the cache eviction.

Your task is to implement a cache, smart pointers, and a specific eviction policy. 
Interfaces to external storage, client (testing) code 
and other (testing) policies are implemented by Recodex.