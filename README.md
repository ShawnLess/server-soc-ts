# RISC-V Server SoC Compliance Test Suite

1. This suite is built for RISC-V Server SoC Test
Specification, **Version v0.51**.
2. This suite is built on ARM BSA ACS (Architecture Compliance Suite) framework, for more information about ARM BSA-ACS, please refer to https://github.com/ARM-software/bsa-acs.


## RISC-V Server SoC Spec

The RISC-V server SoC specification defines a standardized set of capabilities that portable system software such as operating systems and hypervisors, can rely on being present in a RISC-V server SoC. For the spec information, please refer to [https://github.com/riscv-non-isa/server-soc](https://github.com/riscv-non-isa/server-soc).


## RISC-V Server SoC Test Spec

Along with the Server SoC Spec, there is a test spec which defines a set of tests to verify if the requirements specified in RISC-V Server SoC specification are implemented. This test suite will be designed based on the test spec. For more information about the test spec, please also refer to [https://github.com/riscv-non-isa/server-soc](https://github.com/riscv-non-isa/server-soc).

### Other Repo dependence

|  Repos    |   URL                                |   tag                    |
| -------   |  ----------------------------------- |  ---------------------   |
| edk2      | https://github.com/tianocore/edk2    |  edk2-stable202405       |
| qemu      | https://github.com/qemu/qemu.git     |  v9.0.2                  |


## Compile Server SoC TestSuite
### 1. Build Env
    Before you start the build, ensure that the following requirements are met.

- Ubuntu 22.04 (Currently Tested)
- git clone the [EDK2 tree](https://github.com/tianocore/edk2). Recommended edk2 tag is edk2-stable202405
- git clone the [EDK2 port of libc](https://github.com/tianocore/edk2-libc) to local <edk2_path>.
- Install the build prerequisite packages to build EDK2.<br />
  ```
  apt install gcc-riscv64-linux-gnu acpica-tools \
    git curl mtools gdisk gcc openssl automake autotools-dev libtool \
    bison flex bc uuid-dev python3 \
    libglib2.0-dev libssl-dev autopoint libslirp-dev \
    make g++ gcc-riscv64-unknown-elf gettext \
    gcc-aarch64-linux-gnu \
    dosfstools
  ```

### 2. Clone Repo and Patch the EDK2
1.  cd local\_edk2\_path
2.  git submodule update --init --recursive
3.  git clone git@github.com:riscv-non-isa/server-soc-ts.git ShellPkg/Application/server-soc-ts
4.  git apply ShellPkg/Application/server-soc-ts/patches/0001-Apply-patch-ShellPkg-Application-server-soc-ts-patch.patch

### 3. Build the TestSuite under UEFI
1.  export GCC5\_RISCV64\_PREFIX= GCC10.3 toolchain path pointing to **/bin/riscv64-linux-gnu-** in case of x86 machine.
2.  export PACKAGES\_PATH= path pointing to edk2-libc
3.  source edksetup.sh
4.  make -C BaseTools/Source/C
5.  source ShellPkg/Application/server-soc-ts/tools/scripts/acsbuild.sh
The EFI executable file is generated at <edk2_path>/Build/Shell/DEBUG\_GCC5/RISCV64/Bsa.efi

### 4. Run the TestSuite under UEFI
1.  Build EDK2 RISC-V QEMU virt platform as described in [EDK2 Support for RISC-V QEMU virt platform](https://github.com/tianocore/edk2/tree/master/OvmfPkg/RiscVVirt).
2.  Package Bsa.efi into a disk image.
    ```
    dd if=/dev/zero of=disk.img bs=1M count=128
    mformat -i disk.img -t 4096 -h 64 -s 32 -F Z:
    mcopy -i disk.img -s ./Build/Shell/DEBUG_GCC5/RISCV64/Bsa.efi  ::/
    ```
3.  Mount disk.img as a drive in RISC-V QEMU virt platform.
    ```
    $QEMU_INSTALL/bin/qemu-system-riscv64 \
            -machine virt,aia=aplic-imsic,pflash0=pflash0,pflash1=pflash1 \
            ...
            -drive file=./disk.img,format=raw,if=virtio
    ```
4.  Boot to UEFI Shell and run Bsa.efi to start the test suite.

## Current Test Result
Refer test_result/riscv_qemu_virt.md for EDK2 RISC-V QEMU virt platform test result.

    Status:
    * TBI - To be implemented，means a test is feasible in UEFI ACPI PAL but not implemented.
    * NA - Not applicable，means no test is needed\
    * Blocked - Test (or test result confirm) is blocked due to QEMU/FW/OS issue or missing features.
    * Pending - Need further investigation
## Next Steps

### Qemu Model of the Server SoC Reference Machine

Right now around 40% testcase failed or skipped due to the generic virtual platform **virt** lacks the required models in Server SoC, e.g., RCiEP devices. We need a more realistic server SoC reference machine in qemu. This would not only benefit the SoC testsuite, but also a good reference model for SoC and firmware development.

### EDK II for the Server SoC Reference Machine

The **RiscVVirt** is incomplete which leads to the failure of some cases. A more comprehensive UEFI package need for the testsuite, and also a good start point for EDK II product.

### PCIe testcard for the Server SoC Reference Machine
PCIe is an important component in Server SoC and that's why there are multiple rules in Server SoC spec. However, PCIe subsystem in Qemu is very abstract and far from real SoC. To make Server SoC testsuite more comprehensive, we need a platform with more detailed Root Complex and End Point Device.  There maybe two solutions:
1. Integrated a more dedicated PCIe EP model in SystemC, which are more close to real hardware.
2. Design an open source FPGA PCIe EP, which can generate various packets for the compliance test.

--------------------------------------------------------------------------------------------

## License
RISC-V Server SoC Testsuit is distributed under Apache v2.0 License.

## Feedback, contributions, and support

* For feedback, use the GitHub Issue Tracker that is associated with this repository.
* Arm licensees may contact Arm directly through their partner managers.
* Arm welcomes code contributions through GitHub pull requests. See the GitHub documentation on how to raise pull requests.

--------------

*Copyright (c) 2021-2023, Arm Limited and Contributors. All rights reserved.*
