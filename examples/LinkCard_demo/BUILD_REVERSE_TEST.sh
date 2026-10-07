#!/usr/bin/env bash
set -e
export DEVKITPRO=${DEVKITPRO:-/opt/devkitpro}
export DEVKITARM=${DEVKITARM:-$DEVKITPRO/devkitARM}
export PATH="$DEVKITARM/bin:$DEVKITPRO/tools/bin:$PATH"

cd "$(dirname "$0")"

echo "[1/3] Building Japanese/English experimental e-Reader loader..."
cd "#loader"
make clean
make JAP=1 ENG=1
cd ..

echo "[2/3] Installing experimental loader into GBFS..."
if [ ! -f gbfs_files/japeng.loader.original ]; then
  cp gbfs_files/japeng.loader gbfs_files/japeng.loader.original
fi
cp "#loader/main.loader" gbfs_files/japeng.loader

echo "[3/3] Building packaged GBA ROM..."
make clean
make package

echo
echo "DONE: LinkCard_demo.out.gba"
echo "Flash LinkCard_demo.out.gba (not LinkCard_demo.gba)."
