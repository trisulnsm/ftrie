# ftrie
Library to lookup IPv4 address with focus on speed 

Simple library uses a slightly modified version of a RADIX tree.
Used to lookup IPv4 Address Blocks to some arbitrary metadata.

This repo currently supports the MAXMIND and IP2LOCATION databases.
The primary use case is network analytics that requires a very high speed lookup.
