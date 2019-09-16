# two_pole_dc_pass

Implements the old 7/8ths LAG filter but squared and a 15/16 squared lag filter
i.e. two pole versions.

Optimised for 8 bit implementations.

Also feeds the output of the 15/16th squared lag to a 0.01/0.99 double precision lag that
has a very long RC time
