# Script to compile interfuse to wasm, for its use in the web
# David Rivera 02/07/24

buildir="../build"

if [ ! -d "$buildir" ]; then
    echo "INTERFUSE: Build directory doesn't exist, Can't access buildfiles"
    exit 1
fi

cd $buildir || exit 1

# Build cmds
  emcmake cmake ..
  make

  if [ $? -eq 0 ]; then
    echo 'SUCCESS EXPORTING INTEFUSE'
    exit 0
  fi


exit 1
