# fixreg_vita

Fix registry tool for PSVita. Adapated from PSP registry fixing tool.
This tool allows to edit the PSVita registry (system.dreg/system.ireg).
It checks every block and patches the corrupted blocks by calculating a checksum.

# Usage

fixreg_vita is simple to use, just put your modded system.dreg, along with the original system.ireg file in the same directory as the executable.
Run fixreg_vita to fix the registry.

The tool will also extract each registry blocks in the directory 'seg'. The blocks can't be edited from this directory, you need to patch system.dreg directly.

The patched system.dreg will be placed in the same directory, under the name "system_.dreg"

# Credit

Adapted from PSP to PSVita by **Major_Tom**.

Original version by **skylark@mips.for.ever**
