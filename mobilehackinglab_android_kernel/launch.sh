qemu-system-aarch64 \
  -machine virt \
  -cpu cortex-a53 \
  -nographic -smp 1 \
  -hda ./rootfs.ext2 \
  -kernel ./Image \
  -append "console=ttyAMA0 root=/dev/vda debug earlyprintk=serial nokaslr" \
  -m 2048 \
  -net user,hostfwd=tcp::10021-:22 -net nic \
  -s
